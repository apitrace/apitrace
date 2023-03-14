/**************************************************************************
 *
 * Copyright 2013 Samsung
 * Contributed by Eugene Velesevich
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

/*
 *  Wrapper for platform-specific code for obtaining symbolic backtraces
 *  on Android and Linux
 */


#include "os_backtrace.hpp"

#include <set>
#include <vector>
#include "os.hpp"

#if HAVE_BACKTRACE
#  include <stdint.h>
#  include <dlfcn.h>
#  include <unistd.h>
#  include <map>
#  include <vector>
#  include <cxxabi.h>
#  include <backtrace.h>
#endif


using trace::Id;

namespace os {

/*
 * Pascal string (with zero terminator optionally omitted)
 * This is a helper class for storing a set of exact strings or prefixes
 * to match a zero-terminated string against later.
 * Two zero-terminated pstrings compare equal iff they are the same.
 * Otherwise, they compare equal iff one is a prefix of the other
 * (a zero-terminated pstring cannot be a prefix)
 */

struct pstring {
    const char* s;
    int n;
    pstring(const char* s, int n)
    {
        this->s = s;
        this->n = n;
    }
    bool operator<(const pstring q2) const
    {
        return memcmp(s, q2.s, n < q2.n? n : q2.n) < 0;
    }
};


class StringPrefixes {
private:
    std::set<pstring> pset;

    void addPrefix(char* startbuf, int n) {
        std::set<pstring>::iterator elem = pset.find(pstring(startbuf, n));
        bool replace = elem != pset.end() && n < elem->n;
        if (replace) {
            pset.erase(elem);
        }
        if (replace || elem == pset.end()) {
            pset.insert(pstring(startbuf, n));
        }
    }
public:
    StringPrefixes();

    bool contain(const char* s) {
        return pset.find(pstring(s, strlen(s) + 1)) != pset.end();
    }
};

StringPrefixes::StringPrefixes() {
    char *list = getenv("APITRACE_BACKTRACE");
    if (!list)
        return;
    list = strdup(list);
    for (char *t = list; ; t = NULL) {
        char *tok = strtok(t, " \t\r\n");
        if (!tok)
            break;
        if (tok[0] == '#')
            continue;
        if (tok[strlen(tok) - 1] == '*')
            addPrefix(tok, strlen(tok) - 1);
        else
            addPrefix(tok, strlen(tok) + 1);
    }
    free(list);
}


bool backtrace_is_needed(const char* fname) {
    static StringPrefixes backtraceFunctionNamePrefixes;
    return backtraceFunctionNamePrefixes.contain(fname);
}

#if HAVE_BACKTRACE


static char* format(uintptr_t num, int base, char *buf, int maxlen)
{
    static const char digits[] = "0123456789abcdef";
    buf += maxlen;
    do {
        *--buf = digits[num % base];
        num /= base;
        maxlen--;
    } while (num != 0 && maxlen != 0);
    return buf;
}

static void dump(const char *str, int len)
{
    static int fd = dup(STDERR_FILENO);
    if (write(fd, str, len) != len) {
        // Do nothing
    }
}

static void dumpFrame(const RawStackFrame &frame)
{
    char buf[sizeof(long long) * 2], *p;
#define DUMP(string) dump(string, strlen(string))
    DUMP(frame.module ? frame.module : "?");
    if (frame.function) {
        DUMP(": ");
        DUMP(frame.function);
    }
    if (frame.offset >= 0) {
        DUMP("+0x");
        p = format((uintptr_t) frame.offset, 16, buf, sizeof(buf));
        dump(p, buf + sizeof(buf) - p);
    }
    if (frame.filename) {
        DUMP(": ");
        DUMP(frame.filename);
        if (frame.linenumber >= 0) {
            DUMP(":");
            p = format((uintptr_t) frame.linenumber, 10, buf, sizeof(buf));
            dump(p, buf + sizeof(buf) - p);
        }
    }
    DUMP("\n");
#undef DUMP
}


#define BT_DEPTH 10

class libbacktraceProvider {
    struct backtrace_state *state;
    int skipFrames;
    Id nextFrameId;
    std::map<uintptr_t, std::vector<RawStackFrame> > cache;
    std::vector<RawStackFrame> *current, *current_frames;
    RawStackFrame *current_frame;
    bool missingDwarf;

    static void bt_err_callback(void *vdata, const char *msg, int errnum)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        if (errnum == -1)
            this_->missingDwarf = true;
        else if (errnum)
            os::log("libbacktrace: %s: %s\n", msg, strerror(errnum));
        else
            os::log("libbacktrace: %s\n", msg);
    }

    static int bt_countskip(void *vdata, uintptr_t pc)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        Dl_info info1, info2;
        if (!dladdr((void*)bt_countskip, &info2)) {
            os::log("dladdr failed, cannot cull stack traces\n");
            return 1;
        }
        if (!dladdr((void*)pc, &info1))
            return 1;
        if (info1.dli_fbase != info2.dli_fbase)
            return 1;
        this_->skipFrames++;
        return 0;
    }

    static int bt_full_callback(void *vdata, uintptr_t pc,
                                 const char *file, int line, const char *func)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        RawStackFrame frame = *this_->current_frame;
        frame.id = this_->nextFrameId++;
        frame.filename = file;
        frame.linenumber = line;
        if (func)
            frame.function = func;
        int status;
        if (func && (func = abi::__cxa_demangle(func, NULL, NULL, &status)))
            frame.function = func;
        this_->current_frames->push_back(frame);
        return 0;
    }

    static void dl_fill(RawStackFrame *frame, uintptr_t pc)
    {
        Dl_info info = {0};
        dladdr((void*)pc, &info);
        frame->module = info.dli_fname;
        frame->function = info.dli_sname;
        frame->offset = info.dli_saddr ? pc - (uintptr_t)info.dli_saddr
                                       : pc - (uintptr_t)info.dli_fbase;
    }

    static int bt_callback(void *vdata, uintptr_t pc)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        std::vector<RawStackFrame> &frames = this_->cache[pc];
        if (!frames.size()) {
            RawStackFrame frame;
            dl_fill(&frame, pc);
            this_->current_frame = &frame;
            this_->current_frames = &frames;
            backtrace_pcinfo(this_->state, pc, bt_full_callback, bt_err_callback, vdata);
            if (!frames.size()) {
                frame.id = this_->nextFrameId++;
                frames.push_back(frame);
            }
        }
        this_->current->insert(this_->current->end(), frames.begin(), frames.end());
        return this_->current->size() >= BT_DEPTH;
    }

    static int bt_full_dump_callback(void *vdata, uintptr_t pc,
                                     const char *file, int line, const char *func)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        RawStackFrame *frame = this_->current_frame;
        frame->filename = file;
        frame->linenumber = line;
        if (func)
            frame->function = func;
        dumpFrame(*frame);
        return 0;
    }

    static int bt_dump_callback(void *vdata, uintptr_t pc)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        RawStackFrame frame;
        dl_fill(&frame, pc);
        this_->current_frame = &frame;
        this_->missingDwarf = false;
        backtrace_pcinfo(this_->state, pc, bt_full_dump_callback, bt_err_callback, vdata);
        if (this_->missingDwarf)
            dumpFrame(frame);
        return 0;
    }

public:
    libbacktraceProvider():
        state(backtrace_create_state(NULL, 0, bt_err_callback, NULL))
    {
        backtrace_simple(state, 0, bt_countskip, bt_err_callback, this);
    }

    std::vector<RawStackFrame> getParsedBacktrace()
    {
        std::vector<RawStackFrame> parsedBacktrace;
        current = &parsedBacktrace;
        backtrace_simple(state, skipFrames, bt_callback, bt_err_callback, this);
        return parsedBacktrace;
    }

    void dumpBacktrace()
    {
        backtrace_simple(state, 0, bt_dump_callback, bt_err_callback, this);
    }
};

std::vector<RawStackFrame> get_backtrace() {
    static libbacktraceProvider backtraceProvider;
    return backtraceProvider.getParsedBacktrace();
}

void dump_backtrace() {
    static libbacktraceProvider backtraceProvider;
    backtraceProvider.dumpBacktrace();
}


#else /* !HAVE_BACKTRACE */

std::vector<RawStackFrame> get_backtrace() {
    return std::vector<RawStackFrame>();
}

void dump_backtrace() {
}

#endif


} /* namespace os */

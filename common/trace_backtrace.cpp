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



#include "trace_backtrace.hpp"

#if TRACE_BACKTRACE

#include <set>
#include "os.hpp"


namespace trace {

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
    for (char *t = strdup(list); ; t = NULL) {
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
}


bool backtrace_is_needed(const char* fname) {
    static StringPrefixes backtraceFunctionNamePrefixes;
    return backtraceFunctionNamePrefixes.contain(fname);
}

} /* namespace trace */

#if defined(ANDROID)

#include <dlfcn.h>
#include "os.hpp"
#include <vector>

namespace trace {

/* The following two declarations are copied from Android sources */

enum DebugTargetKind {
    kDebugTargetUnknown = 0,
    kDebugTargetLog,
    kDebugTargetFile,
};

struct DebugOutputTarget {
    DebugTargetKind which;

    union {
        struct {
            int priority;
            const char* tag;
        } log;
        struct {
            FILE* fp;
        } file;
    } data;
};

#define THREAD_SELF_NAME "_Z13dvmThreadSelfv"
#define CREATE_DEBUG_TARGET_NAME "_Z25dvmCreateFileOutputTargetP17DebugOutputTargetP7__sFILE"
#define DUMP_BACKTRACE_NAME "_Z18dvmDumpThreadStackPK17DebugOutputTargetP6Thread"


class DalvikBacktraceProvider {
private:
    bool errorOccured;
    void* (*threadself)(void);
    FILE* streamInMemory;
    char* buf;
    size_t bufSize;
    void (*dumpBacktrace)(const DebugOutputTarget*, void*);
    DebugOutputTarget debugTarget;
    Id nextFrameId;
public:
    DalvikBacktraceProvider() {
        nextFrameId = 0;
        FILE* (*open_memstream_exp)(char**, size_t*);
        void (*createDebugTarget)(DebugOutputTarget*, FILE*);
        void* handle = dlopen("/system/lib/libdvm.so", 0);
        errorOccured = true;
        if (!handle) {
            os::log("dlopen failed\n");
            return;
        }
        threadself = (void* (*)())dlsym(handle, THREAD_SELF_NAME);
        if (threadself == NULL) {
            os::log("dlsym ThreadSelf failed\n");
            return;
        }
        createDebugTarget = (void (*)(DebugOutputTarget*, FILE*))dlsym(handle, CREATE_DEBUG_TARGET_NAME);
        if (createDebugTarget == NULL) {
            os::log("dlsym CreateFileOutput failed\n");
            return;
        }
        dumpBacktrace = (void (*)(const DebugOutputTarget*, void*))dlsym(handle, DUMP_BACKTRACE_NAME);
        if (dumpBacktrace == NULL) {
            os::log("dlsym DumpThreadStack failed\n");
            return;
        }
        void* handle2 = dlopen("/system/lib/libcutils.so", 0);
        if (!handle2) {
            os::log("dlopen failed\n");
            return;
        }
        open_memstream_exp = (FILE* (*)(char**, size_t*))dlsym(handle2, "open_memstream");
        if (open_memstream_exp == NULL) {
            os::log("dlsym open_memstream failed\n");
            return;
        }
        streamInMemory = open_memstream_exp(&buf, &bufSize);
        if (streamInMemory == NULL) {
            os::log("open_memstream failed\n");
            return;
        }
        createDebugTarget(&debugTarget, streamInMemory);
        errorOccured = false;
    }

    inline char* getBacktrace() {
        if (errorOccured) {
            return NULL;
        }
        rewind(streamInMemory);
        dumpBacktrace(&debugTarget, threadself());
        fflush(streamInMemory);
        return buf;
    }
/*
 * Parse a stack frame, expecting:
 * "  at android.view.HardwareRenderer$GlRenderer.initializeEgl(HardwareRenderer.java:547)"
 * or
 * "  at android.view.HardwareRenderer$GlRenderer.initializeEgl(Native Method)"
 */
    std::vector<RawStackFrame> parseBacktrace(char *rawBacktrace) {
        std::vector<RawStackFrame> parsedBacktrace;
        char* rawBacktrace_it = rawBacktrace;
        while (*rawBacktrace_it != '\0') {
            RawStackFrame stackFrame;
            // TODO: Keep a cache of stack frames
            stackFrame.id = nextFrameId++;
            /* skip leading space */
            while (*rawBacktrace_it == ' ') {
                rawBacktrace_it++;
            }
            /* Skip "at " */
            rawBacktrace_it += 3;
            stackFrame.function = rawBacktrace_it;
            while (*rawBacktrace_it != '(') {
                rawBacktrace_it++;
            }
            *rawBacktrace_it = '\0';
            stackFrame.filename = rawBacktrace_it + 1;
            while (*rawBacktrace_it != ':' && *rawBacktrace_it != ')') {
                rawBacktrace_it++;
            }
            if (*rawBacktrace_it == ':') {
                const char *linenumber = rawBacktrace_it + 1;
                *rawBacktrace_it = '\0';
                while (*rawBacktrace_it != ')') {
                    rawBacktrace_it++;
                }
                *rawBacktrace_it = '\0';
                rawBacktrace_it++;
                stackFrame.linenumber = atoi(linenumber);
            }
            else {
                stackFrame.filename = NULL;
                while (*rawBacktrace_it != '\n') {
                    rawBacktrace_it++;
                }
            }
            while (*rawBacktrace_it == '\n' || *rawBacktrace_it == ' ') {
                    rawBacktrace_it++;
            }
            parsedBacktrace.push_back(stackFrame); /* module */
        }
        return parsedBacktrace;
    }
};

std::vector<RawStackFrame> get_backtrace() {
    static DalvikBacktraceProvider backtraceProvider;
    return backtraceProvider.parseBacktrace(backtraceProvider.getBacktrace());
}

/* end ANDROID */
#elif defined(__ELF__)

#include <stdint.h>
#include <dlfcn.h>
#include <map>
#include <vector>
#include <cxxabi.h>

#include <backtrace.h>

namespace trace {




#define BT_DEPTH 10

class libbacktraceProvider {
    struct backtrace_state *state;
    int skipFrames;
    Id nextFrameId;
    std::map<uintptr_t, std::vector<RawStackFrame> > cache;
    std::vector<RawStackFrame> *current, *current_frames;
    RawStackFrame *current_frame;

    static void bt_err_callback(void *vdata, const char *msg, int errnum)
    {
        if (errnum == -1)
            return;// no debug/sym info
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

    static int bt_callback(void *vdata, uintptr_t pc)
    {
        libbacktraceProvider *this_ = (libbacktraceProvider*)vdata;
        std::vector<RawStackFrame> &frames = this_->cache[pc];
        if (!frames.size()) {
            RawStackFrame frame;
            Dl_info info = {0};
            dladdr((void*)pc, &info);
            frame.module = info.dli_fname;
            frame.function = info.dli_sname;
            frame.offset = info.dli_saddr ? pc - (uintptr_t)info.dli_saddr
                                          : pc - (uintptr_t)info.dli_fbase;
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
};

std::vector<RawStackFrame> get_backtrace() {
    static libbacktraceProvider backtraceProvider;
    return backtraceProvider.getParsedBacktrace();
}

#endif /* LINUX */

} /* namespace trace */

#endif /* TRACE_BACKTRACE */

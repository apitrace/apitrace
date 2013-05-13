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

#if defined(ANDROID) or defined(__linux__)

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


#define PREFIX_BUF_SIZE (PREFIX_MAX_FUNC_NAME * MAX_BT_FUNC)

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
    StringPrefixes(const char* source);

    bool contain(const char* s) {
        return pset.find(pstring(s, strlen(s) + 1)) != pset.end();
    }
};

bool backtrace_is_needed(const char* fname) {
    static StringPrefixes backtraceFunctionNamePrefixes(APITRACE_FNAMES_SOURCE);
    return backtraceFunctionNamePrefixes.contain(fname);
}

} /* namespace trace */

#ifdef ANDROID

#include <dlfcn.h>
#include "os.hpp"
#include <vector>

namespace trace {

StringPrefixes::StringPrefixes(const char* source) {
    char* buf = (char*)malloc(sizeof(char) * PREFIX_BUF_SIZE);
    char* startbuf = buf;
    int n = 0;
    FILE* f = fopen(source, "r");
    if (f == NULL) {
        os::log("Cannot open " APITRACE_FNAMES_FILE);
    }
    while ((startbuf = fgets(startbuf, PREFIX_MAX_FUNC_NAME, f))) {
        n = strlen(startbuf);
        if (startbuf[n - 1] == '\n') {
            n--;
        }
        if (n > 2 && startbuf[0] != '#') {
            int psize;
            if (startbuf[n - 1] != '*') {
                startbuf[n] = '\0';
                psize = n + 1;
            }
            else {
                psize = n - 1;
            }
            addPrefix(startbuf, psize);
            startbuf += n + 1;
            n = 0;
        }
    }
    fclose(f);
}


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
#elif defined __linux__

#include <execinfo.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <stdio.h>

namespace trace {


StringPrefixes::StringPrefixes(const char* source) {
    char* buf = (char*)malloc(sizeof(char) * PREFIX_BUF_SIZE);
    char* startbuf = buf;
    int n = 0;
    char* s = getenv(source);
    char end = ';';
    if (s == NULL) {
        return;
    }
    *buf = ';';
    strncpy(buf + 1, s, PREFIX_BUF_SIZE - 2);
    while (end != '\0') {
        startbuf++;
        while (*(startbuf + n) != ';' && *(startbuf + n) != '\0') {
            n++;
        }
        end = startbuf[n];
        if (n > 2 && startbuf[0] != '#') {
            int psize;
            if (startbuf[n - 1] != '*') {
                startbuf[n] = '\0';
                psize = n + 1;
            }
            else {
                psize = n - 1;
            }
            addPrefix(startbuf, psize);
            startbuf += n;
            n = 0;
        }
    }
}


#define BT_DEPTH 10

class GlibcBacktraceProvider {
private:
    std::map<void*, RawStackFrame*> cache;
    /*
     * Backtrace being returned by glibc backtrace() contains stack frames
     * belonging to apitrace wrapper module. We count the number of apitrace
     * functions on the stack to avoid recording these frames.
     */
    int numOfNestedFunctions;
    Id nextFrameId;
private:
/*
 * Parse a stack frame, expecting:
 * /lib/libc.so.6.1(__libc_start_main+0x50308) [0x2000000000097630]
 * or
 * /lib/libc.so.6.1(+0x50308) [0x2000000000097630]
 * or
 * /lib/libc.so.6.1() [0x2000000000097630]
 */
    RawStackFrame* parseFrame(void* frame, char* frame_symbol) {
        if (cache.find(frame) == cache.end()) {
            char* frame_symbol_copy = new char[strlen(frame_symbol) + 1];
            strcpy(frame_symbol_copy, frame_symbol);
            RawStackFrame* parsedFrame = new RawStackFrame;
            parsedFrame->id = nextFrameId++;
            char* frame_it = frame_symbol_copy;
            parsedFrame->module = frame_it;
            char* offset = NULL;
            while (true) {
                switch (*frame_it) {
                case '(':
                    *frame_it = '\0';
                    frame_it++;
                    if (*frame_it != ')' && *frame_it != '+') {
                        parsedFrame->function = frame_it;
                        while (*frame_it != '+' && *frame_it != ')') {
                            frame_it++;
                        }
                        *frame_it = '\0';
                        frame_it++;
                    }
                    break;
                case '[':
                    *frame_it = '\0';
                    frame_it++;
                    offset = frame_it;
                    break;
                case ']':
                    *frame_it = '\0';
                    sscanf(offset, "%llx", &parsedFrame->offset);
                    cache[frame] = parsedFrame;
                    return parsedFrame;
                case '\0':
                    cache[frame] = NULL;
                    delete[] frame_symbol_copy;
                    delete[] parsedFrame;
                    return NULL;
                default:
                    frame_it++;
                }
            }
        }
        else {
            return cache[frame];
        }
    }
public:
    GlibcBacktraceProvider() :
      numOfNestedFunctions(0),
      nextFrameId(0)
    {}

    std::vector<RawStackFrame> getParsedBacktrace() {
        std::vector<RawStackFrame> parsedBacktrace;
        void *array[numOfNestedFunctions + BT_DEPTH];
        size_t size;
        char **strings;
        size_t i;
        const char* firstModule;
        size = backtrace(array, numOfNestedFunctions + BT_DEPTH);
        strings = backtrace_symbols(array, size);
        for (i = numOfNestedFunctions; i < size; i++) {
            RawStackFrame* parsedFrame = parseFrame(array[i], strings[i]);
            if (numOfNestedFunctions == 0) {
                if (i == 0) {
                    firstModule = parsedFrame->module;
                }
                else {
                    if (strcmp(firstModule, parsedFrame->module)) {
                        numOfNestedFunctions = i;
                        free(strings);
                        parsedBacktrace = getParsedBacktrace();
                        numOfNestedFunctions--;
                        return parsedBacktrace;
                    }
                }
            } else {
                if (parsedFrame != NULL) {
                    parsedBacktrace.push_back(*parsedFrame);
                }
            }
        }
        free(strings);
        return parsedBacktrace;
    }
};


std::vector<RawStackFrame> get_backtrace() {
    static GlibcBacktraceProvider backtraceProvider;
    return backtraceProvider.getParsedBacktrace();
}

#endif /* LINUX */

} /* namespace trace */

#endif /* ANDROID or LINUX */

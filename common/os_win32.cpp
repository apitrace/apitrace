/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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

#include <windows.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <string>

#include "os.hpp"
#include "os_string.hpp"


namespace os {


/* 
 * Trick from http://locklessinc.com/articles/pthreads_on_windows/
 */
static CRITICAL_SECTION
criticalSection = {
    (PCRITICAL_SECTION_DEBUG)-1, -1, 0, 0, 0, 0
};


void
acquireMutex(void)
{
    EnterCriticalSection(&criticalSection);
}


void
releaseMutex(void)
{
    LeaveCriticalSection(&criticalSection);
}


String
getProcessName(void)
{
    String path;
    size_t size = MAX_PATH;
    char *buf = path.buf(size);

    DWORD nWritten = GetModuleFileNameA(NULL, buf, size);
    (void)nWritten;

    path.truncate();

    return path;
}

String
getCurrentDir(void)
{
    String path;
    size_t size = MAX_PATH;
    char *buf = path.buf(size);
    
    DWORD ret = GetCurrentDirectoryA(size, buf);
    (void)ret;
    
    buf[size - 1] = 0;
    path.truncate();

    return path;
}

bool
String::exists(void) const
{
    DWORD attrs = GetFileAttributesA(str());
    return attrs != INVALID_FILE_ATTRIBUTES;
}

/**
 * Determine whether an argument should be quoted.
 */
static bool
needsQuote(const char *arg)
{
    char c;
    while (true) {
        c = *arg++;
        if (c == '\0') {
            break;
        }
        if (c == ' ' || c == '\t' || c == '\"') {
            return true;
        }
        if (c == '\\') {
            c = *arg++;
            if (c == '\0') {
                break;
            }
            if (c == '"') {
                return true;
            }
        }
    }
    return false;
}

static void
quoteArg(std::string &s, const char *arg)
{
    char c;
    unsigned backslashes = 0;
    
    s.push_back('"');
    while (true) {
        c = *arg++;
        switch (c)
        if (c == '\0') {
            break;
        } else if (c == '"') {
            while (backslashes) {
                s.push_back('\\');
                --backslashes;
            }
            s.push_back('\\');
        } else {
            if (c == '\\') {
                ++backslashes;
            } else {
                backslashes = 0;
            }
        }
        s.push_back(c);
    }
    s.push_back('"');
}

int execute(char * const * args)
{
    std::string commandLine;
   
    const char *arg0 = *args;
    const char *arg;
    char sep = 0;
    while ((arg = *args++) != NULL) {
        if (sep) {
            commandLine.push_back(sep);
        }

        if (needsQuote(arg)) {
            quoteArg(commandLine, arg);
        } else {
            commandLine.append(arg);
        }

        sep = ' ';
    }

    STARTUPINFO startupInfo;
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInformation;

    if (!CreateProcessA(NULL,
                        const_cast<char *>(commandLine.c_str()), // only modified by CreateProcessW
                        0, // process attributes
                        0, // thread attributes
                        FALSE, // inherit handles
                        0, // creation flags,
                        NULL, // environment
                        NULL, // current directory
                        &startupInfo,
                        &processInformation
                        )) {
        log("error: failed to execute %s\n", arg0);
    }

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    DWORD exitCode = ~0;
    GetExitCodeProcess(processInformation.hProcess, &exitCode);

    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);

    return (int)exitCode;
}

void
log(const char *format, ...)
{
    char buf[4096];

    va_list ap;
    va_start(ap, format);
    fflush(stdout);
    vsnprintf(buf, sizeof buf, format, ap);
    va_end(ap);

    OutputDebugStringA(buf);

    /*
     * Also write the message to stderr, when a debugger is not present (to
     * avoid duplicate messages in command line debuggers).
     */
#if _WIN32_WINNT > 0x0400
    if (!IsDebuggerPresent()) {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
    }
#endif
}

long long
getTime(void)
{
    static LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    if (!frequency.QuadPart)
        QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return counter.QuadPart*1000000LL/frequency.QuadPart;
}

void
abort(void)
{
#ifndef NDEBUG
    DebugBreak();
#else
    ExitProcess(0);
#endif
}


static LPTOP_LEVEL_EXCEPTION_FILTER prevExceptionFilter = NULL;
static void (*gCallback)(void) = NULL;

static LONG WINAPI
unhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    if (gCallback) {
        gCallback();
    }

	if (prevExceptionFilter) {
		return prevExceptionFilter(pExceptionInfo);
    } else {
		return EXCEPTION_CONTINUE_SEARCH;
    }
}

void
setExceptionCallback(void (*callback)(void))
{
    assert(!gCallback);

    if (!gCallback) {
        gCallback = callback;

        assert(!prevExceptionFilter);

        /*
         * TODO: Unfortunately it seems that the CRT will reset the exception
         * handler in certain circumnstances.  See
         * http://www.codeproject.com/KB/winsdk/crash_hook.aspx
         */
        prevExceptionFilter = SetUnhandledExceptionFilter(unhandledExceptionFilter);
    }
}

void
resetExceptionCallback(void)
{
    gCallback = NULL;
}


} /* namespace os */

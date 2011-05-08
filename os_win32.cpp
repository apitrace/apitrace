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
#include <string.h>
#include <stdio.h>

#include "os.hpp"


namespace OS {


/* 
 * Trick from http://locklessinc.com/articles/pthreads_on_windows/
 */
static CRITICAL_SECTION
CriticalSection = {
    (PCRITICAL_SECTION_DEBUG)-1, -1, 0, 0, 0, 0
};


void
AcquireMutex(void)
{
    EnterCriticalSection(&CriticalSection); 
}


void
ReleaseMutex(void)
{
    LeaveCriticalSection(&CriticalSection); 
}


bool
GetProcessName(char *str, size_t size)
{
    char szProcessPath[PATH_MAX];
    char *lpProcessName;
    char *lpProcessExt;

    GetModuleFileNameA(NULL, szProcessPath, sizeof(szProcessPath)/sizeof(szProcessPath[0]));

    lpProcessName = strrchr(szProcessPath, '\\');
    lpProcessName = lpProcessName ? lpProcessName + 1 : szProcessPath;

    lpProcessExt = strrchr(lpProcessName, '.');
    if (lpProcessExt) {
        *lpProcessExt = '\0';
    }

    strncpy(str, lpProcessName, size);

    return true;
}

bool
GetCurrentDir(char *str, size_t size)
{
    DWORD ret;
    ret = GetCurrentDirectoryA(size, str);
    str[size - 1] = 0;
    return ret == 0 ? false : true;
}

void
DebugMessage(const char *format, ...)
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

long long GetTime(void)
{
    static LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    if (!frequency.QuadPart)
        QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return counter.QuadPart*1000000LL/frequency.QuadPart;
}

void
Abort(void)
{
#ifndef NDEBUG
    DebugBreak();
#else
    ExitProcess(0);
#endif
}

} /* namespace OS */

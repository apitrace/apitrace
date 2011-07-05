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


static DWORD dwPageSize = 0;


bool queryVirtualAddress(const void *address, MemoryInfo *info)
{
    if (!address) {
        assert(0);
        return false;
    }

    if (!dwPageSize) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        dwPageSize = si.dwPageSize;
    }

    MEMORY_BASIC_INFORMATION mbi;

    LPCVOID lpAddress = (const void *)((SIZE_T)address & ~(dwPageSize - 1));

    if (VirtualQuery(lpAddress, &mbi, sizeof mbi) != sizeof mbi) {
        return false;
    }

    if (mbi.State == MEM_FREE) {
        return false;
    }

    info->start = mbi.AllocationBase;
    
    do {
        info->stop  = (const char *)mbi.BaseAddress + mbi.RegionSize;

        if (VirtualQuery(info->stop, &mbi, sizeof mbi) != sizeof mbi) {
            return false;
        }

    } while (mbi.State != MEM_FREE && mbi.AllocationBase == info->start);

    DebugMessage("%p -> base = %p, size = 0x%lx, offset = %lx\n",
            address,
            info->start,
            (unsigned long)((SIZE_T)info->stop - (SIZE_T)info->start),
            (unsigned long)((SIZE_T)address - (SIZE_T)info->start));

#if 0
    HMODULE hModule;
    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPSTR)address,
            &hModule) ) {
        char szFileName[512];
        if (GetModuleFileNameA(hModule, szFileName, sizeof szFileName)) {
            DebugMessage("  %s\n", szFileName);
        }
    }
#endif

    return true;
}


} /* namespace OS */

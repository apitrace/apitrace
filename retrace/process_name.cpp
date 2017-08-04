/**************************************************************************
 *
 * Copyright 2017 VMware, Inc
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

#include "process_name.hpp"

#include <assert.h>

#if defined(__linux__)
#include <features.h>  // __GLIBC__
#endif

#include <iostream>

#include "os.hpp"
#include "os_symbols.hpp"

#if defined(__GLIBC__)

#include <dlfcn.h>
#include <string.h>
#include <unistd.h>


static std::string g_processName;

extern "C"  {


PUBLIC ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
    unsigned long pid = 0;
    if (strcmp(pathname, "/proc/self/exe") == 0 ||
        (sscanf(pathname, "/proc/%lu/exe", &pid) == 1 &&
         pid == getpid())) {
#ifndef _NDEBUG
        std::cerr << "readlink(" << pathname << ") from " << getModuleFromAddress(ReturnAddress()) << "\n";
#endif
        if (!g_processName.empty()) {
            size_t len = g_processName.length();
            if (len < bufsiz) {
                memcpy(buf, g_processName.data(), len);
                buf[len] = 0;
                return len;
            } else {
                memcpy(buf, g_processName.data(), bufsiz);
                return bufsiz;
            }
        } else {
            // FIXME: We need to defer retrace::setUp() until after the first trace is loaded.
            std::cerr << "warning: process name not yet set\n";
            return -1;
        }
    }

    typedef ssize_t (*p_readlink)(const char *pathname, char *buf, size_t bufsiz);
    p_readlink p = (p_readlink)dlsym(RTLD_NEXT, "readlink");
    assert(p != nullptr);
    return p(pathname, buf, bufsiz);
}


} /* extern C */


void
setProcessName(const char *processName)
{
    g_processName = processName;

    char **p__progname_full = (char **)dlsym(RTLD_DEFAULT, "__progname_full");
    if (p__progname_full == nullptr) {
        std::cerr << "error: failed to get address of __progname_full symbol" << std::endl;
        return;
    }

    char *progname_full = strdup(processName);
    assert(progname_full != nullptr);

    *p__progname_full = progname_full;

    char **p__progname = (char **)dlsym(RTLD_DEFAULT, "__progname");
    if (p__progname == nullptr) {
        std::cerr << "error: failed to get address of __progname symbol" << std::endl;
        return;
    }

    char *progname = strrchr(progname_full, '/');
    if (progname == nullptr) {
        progname = progname_full;
    } else {
        progname += 1;
    }

    *p__progname = progname;
}


#elif defined(_WIN32)

#include <windows.h>
#include "mhook.h"

#ifdef _MSC_VER
#define ReturnAddress() _ReturnAddress()
#else
#define ReturnAddress() __builtin_return_address(0)
#endif

typedef DWORD (WINAPI *PFNGETMODULEFILENAMEA)(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
static PFNGETMODULEFILENAMEA pfnGetModuleFileNameA = &GetModuleFileNameA;

static inline HMODULE
GetModuleFromAddress(PVOID pAddress)
{
    HMODULE hModule = nullptr;
    BOOL bRet = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                  (LPCTSTR)pAddress,
                                  &hModule);
    return bRet ? hModule : nullptr;
}

static std::string g_processName;


static DWORD WINAPI
MyGetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    if (hModule == nullptr) {
#ifndef NDEBUG
        void *pCallerAddr = ReturnAddress();
        HMODULE hCallerModule = GetModuleFromAddress(pCallerAddr);
        char szCaller[MAX_PATH];
        DWORD dwRet = pfnGetModuleFileNameA(hCallerModule, szCaller, sizeof szCaller);
        assert(dwRet > 0);

        std::cerr << "GetModuleFileNameA(" << hModule << ") from " << szCaller << "\n";
#endif

        assert(!g_processName.empty());
        assert(nSize != 0);

        size_t len = g_processName.length();
        if (len < nSize) {
            memcpy(lpFilename, g_processName.data(), len);
            lpFilename[len] = 0;
            SetLastError(ERROR_SUCCESS);
            return len;
        } else {
            memcpy(lpFilename, g_processName.data(), nSize - 1);
            lpFilename[nSize - 1] = 0;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return nSize;
        }
    }

    return pfnGetModuleFileNameA(hModule, lpFilename, nSize);
}


void
setProcessName(const char *processName)
{
    g_processName = processName;

    static BOOL bHooked = FALSE;
    if (!bHooked) {
        bHooked = TRUE;

        LPVOID lpOrigAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32"), "GetModuleFileNameA");
        if (lpOrigAddress) {
            LPVOID lpHookAddress = (LPVOID)&MyGetModuleFileNameA;
            LPVOID lpRealAddress = lpOrigAddress;
            if (!Mhook_SetHook(&lpRealAddress, lpHookAddress)) {
                std::cerr << "error: failed to GetModuleFileNameA\n";
            }
            pfnGetModuleFileNameA = (PFNGETMODULEFILENAMEA)lpRealAddress;
        }
    }
}


#else

void
setProcessName(const char *processName)
{
}

#endif

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


// Must not use std::string to prevent it from being destroyed.
static char g_processName[4097];

extern "C"  {


PUBLIC ssize_t
readlink(const char *pathname, char *buf, size_t bufsiz)
{
    /*
     * NOTE: Best to avoid std::cerr/cout as this fuction might be called
     * before libstdc++ has initialized.
     */
    unsigned long pid = 0;
    if (strcmp(pathname, "/proc/self/exe") == 0 ||
        (sscanf(pathname, "/proc/%lu/exe", &pid) == 1 &&
         pid == getpid())) {
        if (0) {
            std::string callerModule(getModuleFromAddress(ReturnAddress()));
            fprintf(stderr, "readlink(\"%s\") from %s\n", pathname, callerModule.c_str());
        }
        size_t len = strlen(g_processName);
        if (len) {
            if (len < bufsiz) {
                memcpy(buf, g_processName, len + 1);
                return len;
            } else {
                memcpy(buf, g_processName, bufsiz);
                return bufsiz;
            }
        } else {
            // FIXME: We need to defer retrace::setUp() until after the first trace is loaded.
            fprintf(stderr, "warning: process name not yet set\n");
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
    strncpy(g_processName, processName, sizeof g_processName - 1);
    g_processName[sizeof g_processName - 1] = '\0';

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

void
setProcessCommandLine(const char* processCommandLine)
{
}

#elif defined(_WIN32) && (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_AMD64))

#include <windows.h>
#include "mhook.h"

typedef DWORD (WINAPI *PFNGETMODULEFILENAMEA)(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
static PFNGETMODULEFILENAMEA pfnGetModuleFileNameA = &GetModuleFileNameA;

typedef DWORD (WINAPI *PFNGETMODULEFILENAMEW)(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
static PFNGETMODULEFILENAMEW pfnGetModuleFileNameW = &GetModuleFileNameW;

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

// Must not use std::string to prevent it from being destroyed.
static char g_processName[4097];
static char g_processCommandLine[8192];


static DWORD WINAPI
MyGetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    if (hModule == nullptr) {
        if (0) {
            void *pCallerAddr = ReturnAddress();
            HMODULE hCallerModule = GetModuleFromAddress(pCallerAddr);
            char szCaller[MAX_PATH];
            DWORD dwRet = pfnGetModuleFileNameA(hCallerModule, szCaller, sizeof szCaller);
            assert(dwRet > 0);

            std::cerr << "GetModuleFileNameA(" << hModule << ") from " << szCaller << "\n";
        }

        assert(*g_processName);
        assert(nSize != 0);

        size_t len = strlen(g_processName);
        if (len < nSize) {
            memcpy(lpFilename, g_processName, len);
            lpFilename[len] = 0;
            SetLastError(ERROR_SUCCESS);
            return len;
        } else {
            memcpy(lpFilename, g_processName, nSize - 1);
            lpFilename[nSize - 1] = 0;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return nSize;
        }
    }

    return pfnGetModuleFileNameA(hModule, lpFilename, nSize);
}

static DWORD WINAPI
MyGetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
    if (hModule == nullptr) {
        if (0) {
            void *pCallerAddr = ReturnAddress();
            HMODULE hCallerModule = GetModuleFromAddress(pCallerAddr);
            WCHAR szCaller[MAX_PATH];
            DWORD dwRet = pfnGetModuleFileNameW(hCallerModule, szCaller, sizeof szCaller);
            assert(dwRet > 0);

            std::cerr << "GetModuleFileNameW(" << hModule << ") from " << szCaller << "\n";
        }

        assert(*g_processName);
        assert(nSize != 0);

        size_t len = strlen(g_processName);
        if (len < nSize) {
            ::MultiByteToWideChar(CP_UTF8, 0, g_processName, -1, lpFilename, len);
            lpFilename[len] = 0;
            SetLastError(ERROR_SUCCESS);
            return len;
        } else {
            ::MultiByteToWideChar(CP_UTF8, 0, g_processName, -1, lpFilename, nSize - 1);
            lpFilename[nSize - 1] = L'\0';
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return nSize;
        }
    }

    return pfnGetModuleFileNameW(hModule, lpFilename, nSize);
}

void
setProcessName(const char *processName)
{
    strncpy(g_processName, processName, sizeof g_processName - 1);
    g_processName[sizeof g_processName - 1] = '\0';

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

        lpOrigAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32"), "GetModuleFileNameW");
        if (lpOrigAddress) {
            LPVOID lpHookAddress = (LPVOID)&MyGetModuleFileNameW;
            LPVOID lpRealAddress = lpOrigAddress;
            if (!Mhook_SetHook(&lpRealAddress, lpHookAddress)) {
                std::cerr << "error: failed to GetModuleFileNameW\n";
            }
            pfnGetModuleFileNameW = (PFNGETMODULEFILENAMEW)lpRealAddress;
        }
    }
}

static LPSTR WINAPI
MyGetCommandLineA()
{
    return g_processCommandLine;
}

static LPWSTR WINAPI
MyGetCommandLineW()
{
    static wchar_t processCommandLineW[8192];
    static bool bConverted = false;
    if (!bConverted) {
        ::MultiByteToWideChar(CP_UTF8, 0, g_processName, -1, processCommandLineW, sizeof(processCommandLineW) / sizeof(wchar_t));
        bConverted = true;
    }
    return processCommandLineW;
}

void
setProcessCommandLine(const char* processCommandLine)
{
    strncpy(g_processCommandLine, processCommandLine, sizeof g_processCommandLine - 1);
    g_processCommandLine[sizeof g_processCommandLine - 1] = '\0';

    static BOOL bHooked = FALSE;
    if (!bHooked) {
        bHooked = TRUE;

        LPVOID lpOrigAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32"), "GetCommandLineA");
        if (lpOrigAddress) {
            LPVOID lpHookAddress = (LPVOID)&MyGetCommandLineA;
            if (!Mhook_SetHook(&lpOrigAddress, lpHookAddress)) {
                std::cerr << "error: failed to hook GetCommandLineA\n";
            }
        }

        lpOrigAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32"), "GetCommandLineW");
        if (lpOrigAddress) {
            LPVOID lpHookAddress = (LPVOID)&MyGetCommandLineW;
            if (!Mhook_SetHook(&lpOrigAddress, lpHookAddress)) {
                std::cerr << "error: failed to hook GetCommandLineW\n";
            }
        }
    }
}

#else

void
setProcessName(const char *processName)
{
}

void
setProcessCommandLine(const char* processCommandLine)
{
}

#endif

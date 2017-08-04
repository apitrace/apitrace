/**************************************************************************
 *
 * Copyright 2016 VMware, Inc.
 * Copyright 2011-2012 Jose Fonseca
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
 * Code for the DLL that will be injected in the target process.
 *
 * The injected DLL will manipulate the import tables to hook the
 * modules/functions of interest.
 *
 * See also:
 * - http://www.codeproject.com/KB/system/api_spying_hack.aspx
 * - http://www.codeproject.com/KB/threads/APIHooking.aspx
 * - http://msdn.microsoft.com/en-us/magazine/cc301808.aspx
 */


#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <algorithm>
#include <set>
#include <map>
#include <functional>
#include <iterator>

#include <windows.h>
#include <tlhelp32.h>
#include <delayimp.h>

#include "inject.h"
#include "mhook.h"

#include "os_symbols.hpp"


static int VERBOSITY = 0;
#define NOOP 0


static CRITICAL_SECTION g_Mutex;



static HMODULE g_hThisModule = NULL;
static HMODULE g_hHookModule = NULL;


static std::map<PVOID, PVOID>
g_pRealFunctions;


typedef HMODULE
(WINAPI *PFNLOADLIBRARYA)(LPCSTR);

typedef HMODULE
(WINAPI *PFNLOADLIBRARYW)(LPCWSTR);

typedef HMODULE
(WINAPI *PFNLOADLIBRARYEXA)(LPCSTR, HANDLE, DWORD);

typedef HMODULE
(WINAPI *PFNLOADLIBRARYEXW)(LPCWSTR, HANDLE, DWORD);

typedef BOOL
(WINAPI *PFNFREELIBRARY)(HMODULE);

static PFNLOADLIBRARYA RealLoadLibraryA = LoadLibraryA;
static PFNLOADLIBRARYW RealLoadLibraryW = LoadLibraryW;
static PFNLOADLIBRARYEXA RealLoadLibraryExA = LoadLibraryExA;
static PFNLOADLIBRARYEXW RealLoadLibraryExW = LoadLibraryExW;
static PFNFREELIBRARY RealFreeLibrary = FreeLibrary;

typedef FARPROC (WINAPI * PFNGETPROCADDRESS)(HMODULE hModule, LPCSTR lpProcName);

static PFNGETPROCADDRESS RealGetProcAddress = GetProcAddress;

typedef BOOL
(WINAPI *PFNCREATEPROCESSA) (LPCSTR, LPSTR,
        LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID,
        LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

static PFNCREATEPROCESSA RealCreateProcessA = CreateProcessA;

typedef BOOL
(WINAPI *PFNCREATEPROCESSW) (LPCWSTR, LPWSTR,
        LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID,
        LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

static PFNCREATEPROCESSW RealCreateProcessW = CreateProcessW;

typedef BOOL
(WINAPI *PFNCREATEPROCESSASUSERW) (HANDLE, LPCWSTR, LPWSTR,
        LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID,
        LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

static PFNCREATEPROCESSASUSERW RealCreateProcessAsUserW = CreateProcessAsUserW;


static void
debugPrintf(const char *format, ...)
{
    char buf[512];

    va_list ap;
    va_start(ap, format);
    _vsnprintf(buf, sizeof buf, format, ap);
    va_end(ap);

    OutputDebugStringA(buf);
}


EXTERN_C void
_assert(const char *_Message, const char *_File, unsigned _Line)
{
    debugPrintf("Assertion failed: %s, file %s, line %u\n", _Message, _File, _Line);
    TerminateProcess(GetCurrentProcess(), 1);
}


EXTERN_C void
_wassert(const wchar_t * _Message, const wchar_t *_File, unsigned _Line)
{
    debugPrintf("Assertion failed: %S, file %S, line %u\n", _Message, _File, _Line);
    TerminateProcess(GetCurrentProcess(), 1);
}


static void
MyCreateProcessCommon(BOOL bRet,
                      DWORD dwCreationFlags,
                      LPPROCESS_INFORMATION lpProcessInformation)
{
    if (!bRet) {
        debugPrintf("inject: warning: failed to create child process\n");
        return;
    }

    DWORD dwLastError = GetLastError();

    if (isDifferentArch(lpProcessInformation->hProcess)) {
        debugPrintf("inject: error: child process %lu has different architecture\n",
                    GetProcessId(lpProcessInformation->hProcess));
    } else {
        char szDllPath[MAX_PATH];
        GetModuleFileNameA(g_hThisModule, szDllPath, sizeof szDllPath);

        if (!injectDll(lpProcessInformation->hProcess, szDllPath)) {
            debugPrintf("inject: warning: failed to inject into child process %lu\n",
                        GetProcessId(lpProcessInformation->hProcess));
        }
    }

    if (!(dwCreationFlags & CREATE_SUSPENDED)) {
        ResumeThread(lpProcessInformation->hThread);
    }

    SetLastError(dwLastError);
}

static BOOL WINAPI
MyCreateProcessA(LPCSTR lpApplicationName,
                 LPSTR lpCommandLine,
                 LPSECURITY_ATTRIBUTES lpProcessAttributes,
                 LPSECURITY_ATTRIBUTES lpThreadAttributes,
                 BOOL bInheritHandles,
                 DWORD dwCreationFlags,
                 LPVOID lpEnvironment,
                 LPCSTR lpCurrentDirectory,
                 LPSTARTUPINFOA lpStartupInfo,
                 LPPROCESS_INFORMATION lpProcessInformation)
{
    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(\"%s\", \"%s\", ...)\n",
                    __FUNCTION__,
                    lpApplicationName,
                    lpCommandLine);
    }

    BOOL bRet;
    bRet = RealCreateProcessA(lpApplicationName,
                              lpCommandLine,
                              lpProcessAttributes,
                              lpThreadAttributes,
                              bInheritHandles,
                              dwCreationFlags | CREATE_SUSPENDED,
                              lpEnvironment,
                              lpCurrentDirectory,
                              lpStartupInfo,
                              lpProcessInformation);

    MyCreateProcessCommon(bRet, dwCreationFlags, lpProcessInformation);

    return bRet;
}

static BOOL WINAPI
MyCreateProcessW(LPCWSTR lpApplicationName,
                 LPWSTR lpCommandLine,
                 LPSECURITY_ATTRIBUTES lpProcessAttributes,
                 LPSECURITY_ATTRIBUTES lpThreadAttributes,
                 BOOL bInheritHandles,
                 DWORD dwCreationFlags,
                 LPVOID lpEnvironment,
                 LPCWSTR lpCurrentDirectory,
                 LPSTARTUPINFOW lpStartupInfo,
                 LPPROCESS_INFORMATION lpProcessInformation)
{
    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(\"%S\", \"%S\", ...)\n",
                    __FUNCTION__,
                    lpApplicationName,
                    lpCommandLine);
    }

    BOOL bRet;
    bRet = RealCreateProcessW(lpApplicationName,
                              lpCommandLine,
                              lpProcessAttributes,
                              lpThreadAttributes,
                              bInheritHandles,
                              dwCreationFlags | CREATE_SUSPENDED,
                              lpEnvironment,
                              lpCurrentDirectory,
                              lpStartupInfo,
                              lpProcessInformation);

    MyCreateProcessCommon(bRet, dwCreationFlags, lpProcessInformation);

    return bRet;
}

static BOOL WINAPI
MyCreateProcessAsUserW(HANDLE hToken,
                       LPCWSTR lpApplicationName,
                       LPWSTR lpCommandLine,
                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                       BOOL bInheritHandles,
                       DWORD dwCreationFlags,
                       LPVOID lpEnvironment,
                       LPCWSTR lpCurrentDirectory,
                       LPSTARTUPINFOW lpStartupInfo,
                       LPPROCESS_INFORMATION lpProcessInformation)
{
    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(\"%S\", \"%S\", ...)\n",
                    __FUNCTION__,
                    lpApplicationName,
                    lpCommandLine);
    }

    BOOL bRet;
    bRet = RealCreateProcessAsUserW(hToken,
                                    lpApplicationName,
                                    lpCommandLine,
                                    lpProcessAttributes,
                                    lpThreadAttributes,
                                    bInheritHandles,
                                    dwCreationFlags,
                                    lpEnvironment,
                                    lpCurrentDirectory,
                                    lpStartupInfo,
                                    lpProcessInformation);

    MyCreateProcessCommon(bRet, dwCreationFlags, lpProcessInformation);

    return bRet;
}


template< class T, class I >
inline T *
rvaToVa(HMODULE hModule, I rva)
{
    assert(rva != 0);
    return reinterpret_cast<T *>(reinterpret_cast<PBYTE>(hModule) + rva);
}


static PIMAGE_OPTIONAL_HEADER
getOptionalHeader(HMODULE hModule,
                  const char *szModule)
{
    PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        debugPrintf("inject: warning: %s: unexpected DOS header magic (0x%04x)\n",
                    szModule, pDosHeader->e_magic);
        return NULL;
    }
    PIMAGE_NT_HEADERS pNtHeaders = rvaToVa<IMAGE_NT_HEADERS>(hModule, pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        debugPrintf("inject: warning: %s: unexpected NT header signature (0x%08lx)\n",
                    szModule, pNtHeaders->Signature);
        return NULL;
    }
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pNtHeaders->OptionalHeader;
    return pOptionalHeader;
}

static PVOID
getImageDirectoryEntry(HMODULE hModule,
                       const char *szModule,
                       UINT Entry)
{
    MEMORY_BASIC_INFORMATION MemoryInfo;
    if (VirtualQuery(hModule, &MemoryInfo, sizeof MemoryInfo) != sizeof MemoryInfo) {
        debugPrintf("inject: warning: %s: VirtualQuery failed\n", szModule);
        return NULL;
    }
    if (MemoryInfo.Protect & (PAGE_NOACCESS | PAGE_EXECUTE)) {
        debugPrintf("inject: warning: %s: no read access (Protect = 0x%08lx)\n", szModule, MemoryInfo.Protect);
        return NULL;
    }

    PIMAGE_OPTIONAL_HEADER pOptionalHeader = getOptionalHeader(hModule, szModule);
    if (!pOptionalHeader ||
        pOptionalHeader->DataDirectory[Entry].Size == 0) {
        return NULL;
    }

    UINT_PTR ImportAddress = pOptionalHeader->DataDirectory[Entry].VirtualAddress;
    if (!ImportAddress) {
        return NULL;
    }

    return rvaToVa<VOID>(hModule, ImportAddress);
}


static PIMAGE_EXPORT_DIRECTORY
getExportDescriptor(HMODULE hModule)
{
    PVOID pEntry = getImageDirectoryEntry(hModule, "(wrapper)", IMAGE_DIRECTORY_ENTRY_EXPORT);
    return reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(pEntry);
}


/* Set of previously hooked modules */
static std::set<HMODULE>
g_hHookedModules;


enum Action {
    ACTION_HOOK,
    ACTION_UNHOOK,

};


static void
patchModule(HMODULE hModule,
            const char *szModule,
            Action action)
{
    /* Never patch this module */
    if (hModule == g_hThisModule) {
        return;
    }

    /* Never patch our hook module */
    if (hModule == g_hHookModule) {
        return;
    }

    /* Hook modules only once */
    if (action == ACTION_HOOK) {
        std::pair< std::set<HMODULE>::iterator, bool > ret;
        EnterCriticalSection(&g_Mutex);
        ret = g_hHookedModules.insert(hModule);
        LeaveCriticalSection(&g_Mutex);
        if (!ret.second) {
            return;
        }
    }

    if (VERBOSITY > 0) {
        debugPrintf("inject: found module %s\n", szModule);
    }

    const char *szBaseName = getBaseName(szModule);

    if (stricmp(szBaseName, "opengl32.dll") != 0 &&
        stricmp(szBaseName, "dxgi.dll") != 0 &&
        stricmp(szBaseName, "d3d10.dll") != 0 &&
        stricmp(szBaseName, "d3d10_1.dll") != 0 &&
        stricmp(szBaseName, "d3d11.dll") != 0 &&
        stricmp(szBaseName, "d3d9.dll") != 0 &&
        stricmp(szBaseName, "d3d8.dll") != 0 &&
        stricmp(szBaseName, "ddraw.dll") != 0 &&
        stricmp(szBaseName, "dcomp.dll") != 0 &&
        stricmp(szBaseName, "dxva2.dll") != 0 &&
        stricmp(szBaseName, "d2d1.dll") != 0 &&
        stricmp(szBaseName, "dwrite.dll") != 0) {
        return;
    }

    PIMAGE_EXPORT_DIRECTORY pExportDescriptor = getExportDescriptor(g_hHookModule);
    assert(pExportDescriptor);

    DWORD *pAddressOfNames = (DWORD *)((BYTE *)g_hHookModule + pExportDescriptor->AddressOfNames);
    for (DWORD i = 0; i < pExportDescriptor->NumberOfNames; ++i) {
        const char *szFunctionName = (const char *)((BYTE *)g_hHookModule + pAddressOfNames[i]);

        LPVOID lpOrigAddress = (LPVOID)RealGetProcAddress(hModule, szFunctionName);
        if (lpOrigAddress) {
            LPVOID lpHookAddress = (LPVOID)RealGetProcAddress(g_hHookModule, szFunctionName);
            assert(lpHookAddress);

            // With mhook we intercept the inner gl* calls, so no need to trace the
            // outer wglUseFont* calls.
            if (strncmp(szFunctionName, "wglUseFont", strlen("wglUseFont")) == 0) {
                debugPrintf("inject: not hooking %s!%s\n", szBaseName, szFunctionName);
                continue;
            }

            if (VERBOSITY > 0) {
                debugPrintf("inject: hooking %s!%s\n", szModule, szFunctionName);
            }

            LPVOID lpRealAddress = lpOrigAddress;
            if (!Mhook_SetHook(&lpRealAddress, lpHookAddress)) {
                debugPrintf("inject: error: failed to hook %s!%s\n", szModule, szFunctionName);
            }

            EnterCriticalSection(&g_Mutex);
            g_pRealFunctions[lpOrigAddress] = lpRealAddress;
            LeaveCriticalSection(&g_Mutex);
            
            pSharedMem->bReplaced = TRUE;
        }
    }
}


static void
patchAllModules(Action action)
{
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof me32;
    if (Module32First(hModuleSnap, &me32)) {
        do  {
            patchModule(me32.hModule, me32.szExePath, action);
        } while (Module32Next(hModuleSnap, &me32));
    }

    CloseHandle(hModuleSnap);
}


static HMODULE WINAPI
MyLoadLibraryA(LPCSTR lpLibFileName)
{
    HMODULE hModule = RealLoadLibraryA(lpLibFileName);
    DWORD dwLastError = GetLastError();

    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(\"%s\") = 0x%p\n",
                    __FUNCTION__ + 2, lpLibFileName, hModule);
    }

    // Hook all new modules (and not just this one, to pick up any dependencies)
    patchAllModules(ACTION_HOOK);

    SetLastError(dwLastError);
    return hModule;
}

static HMODULE WINAPI
MyLoadLibraryW(LPCWSTR lpLibFileName)
{
    HMODULE hModule = RealLoadLibraryW(lpLibFileName);
    DWORD dwLastError = GetLastError();

    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(L\"%S\") = 0x%p\n",
                    __FUNCTION__ + 2, lpLibFileName, hModule);
    }

    // Hook all new modules (and not just this one, to pick up any dependencies)
    patchAllModules(ACTION_HOOK);

    SetLastError(dwLastError);
    return hModule;
}

#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR    0x00000100
#endif
#ifndef LOAD_LIBRARY_SEARCH_APPLICATION_DIR
#define LOAD_LIBRARY_SEARCH_APPLICATION_DIR 0x00000200
#endif
#ifndef LOAD_LIBRARY_SEARCH_USER_DIRS
#define LOAD_LIBRARY_SEARCH_USER_DIRS       0x00000400
#endif
#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32        0x00000800
#endif
#ifndef LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
#define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS    0x00001000
#endif

static inline DWORD
adjustFlags(DWORD dwFlags)
{
    /*
     * XXX: LoadLibraryEx seems to interpret "application directory" in respect
     * to the module that's calling it.  So when the application restricts the
     * search path to application directory via
     * LOAD_LIBRARY_SEARCH_APPLICATION_DIR or LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
     * flags, kernel32.dll ends up searching on the directory of the inject.dll
     * module.
     *
     * XXX: What about SetDefaultDllDirectories?
     *
     */
    if (dwFlags & (LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                   LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
        dwFlags &= ~(LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                     LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                     LOAD_LIBRARY_SEARCH_USER_DIRS |
                     LOAD_LIBRARY_SEARCH_SYSTEM32 |
                     LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    }

    return dwFlags;
}

static HMODULE WINAPI
MyLoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = RealLoadLibraryExA(lpLibFileName, hFile, adjustFlags(dwFlags));
    DWORD dwLastError = GetLastError();

    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(\"%s\", 0x%p, 0x%lx) = 0x%p\n",
                    __FUNCTION__ + 2, lpLibFileName, hFile, dwFlags, hModule);
    }

    // Hook all new modules (and not just this one, to pick up any dependencies)
    patchAllModules(ACTION_HOOK);

    SetLastError(dwLastError);
    return hModule;
}

static HMODULE WINAPI
MyLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = RealLoadLibraryExW(lpLibFileName, hFile, adjustFlags(dwFlags));
    DWORD dwLastError = GetLastError();

    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(L\"%S\", 0x%p, 0x%lx) = 0x%p\n",
                    __FUNCTION__ + 2, lpLibFileName, hFile, dwFlags, hModule);
    }

    // Hook all new modules (and not just this one, to pick up any dependencies)
    patchAllModules(ACTION_HOOK);

    SetLastError(dwLastError);
    return hModule;
}


static void
logGetProcAddress(PCSTR szAction, HMODULE hModule, LPCSTR lpProcName, HMODULE hCallerModule)
{
    char szCaller[MAX_PATH];
    DWORD dwRet = GetModuleFileNameA(hCallerModule, szCaller, sizeof szCaller);
    assert(dwRet);

    if (HIWORD(lpProcName) == 0) {
        debugPrintf("inject: %s %s(%u) from %s\n", szAction, "GetProcAddress", LOWORD(lpProcName), szCaller);
    } else {
        debugPrintf("inject: %s %s(\"%s\") from %s\n", szAction, "GetProcAddress", lpProcName, szCaller);
    }
}

static HMODULE
GetModuleFromAddress(PVOID pAddress)
{
    HMODULE hModule = nullptr;
    BOOL bRet = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                  (LPCTSTR)pAddress,
                                  &hModule);
    return bRet ? hModule : nullptr;
}


static FARPROC WINAPI
MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    FARPROC lpProcAddress = RealGetProcAddress(hModule, lpProcName);
    LPCSTR szAction = "ignoring";

    void *pCallerAddr = ReturnAddress();
    HMODULE hCallerModule = GetModuleFromAddress(pCallerAddr);

    if (lpProcAddress) {
        if (hCallerModule == g_hThisModule ||
            hCallerModule == g_hHookModule) {

            assert(HIWORD(lpProcName) != 0);

            if (hCallerModule == g_hHookModule) {
                EnterCriticalSection(&g_Mutex);
                auto search = g_pRealFunctions.find((PVOID)lpProcAddress);
                if (search != g_pRealFunctions.end()) {
                    szAction = "overriding";
                    lpProcAddress = (FARPROC)search->second;
                }
                LeaveCriticalSection(&g_Mutex);
            }

            // Check for recursion
            HMODULE hResultModule = GetModuleFromAddress((PVOID)lpProcAddress);
            if (hResultModule == g_hThisModule ||
                hResultModule == g_hHookModule) {
                debugPrintf("inject: error: recursion in GetProcAddress(\"%s\")\n", lpProcName);
                TerminateProcess(GetCurrentProcess(), 1);
            }
        }
    }

    if (VERBOSITY >= 3) {
        /* XXX this can cause segmentation faults */
        logGetProcAddress(szAction, hModule, lpProcName, hCallerModule);
    }

    return lpProcAddress;
}


static BOOL WINAPI
MyFreeLibrary(HMODULE hModule)
{
    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(0x%p)\n", __FUNCTION__, hModule);
    }

    BOOL bRet = RealFreeLibrary(hModule);
    DWORD dwLastError = GetLastError();

    std::set<HMODULE> hCurrentModules;
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hModuleSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me32;
        me32.dwSize = sizeof me32;
        if (Module32First(hModuleSnap, &me32)) {
            do  {
                hCurrentModules.insert(me32.hModule);
            } while (Module32Next(hModuleSnap, &me32));
        }
        CloseHandle(hModuleSnap);
    }

    // Clear the modules that have been freed
    EnterCriticalSection(&g_Mutex);
    std::set<HMODULE> hIntersectedModules;
    std::set_intersection(g_hHookedModules.begin(), g_hHookedModules.end(),
                          hCurrentModules.begin(), hCurrentModules.end(),
                          std::inserter(hIntersectedModules, hIntersectedModules.begin()));
    g_hHookedModules = std::move(hIntersectedModules);
    LeaveCriticalSection(&g_Mutex);

    SetLastError(dwLastError);
    return bRet;
}


static void
setHooks(void)
{
    HMODULE hKernel32 = GetModuleHandleA("kernel32");
    assert(hKernel32);

#   define SET_HOOK(_name) \
        Real##_name = reinterpret_cast<decltype(Real##_name)>(RealGetProcAddress(hKernel32, #_name)); \
        assert(Real##_name); \
        assert(Real##_name != My##_name); \
        if (!Mhook_SetHook((PVOID*)&Real##_name, (PVOID)My##_name)) { \
            debugPrintf("inject: error: failed to hook " #_name "\n"); \
        }

    SET_HOOK(LoadLibraryA)
    SET_HOOK(LoadLibraryW)
    SET_HOOK(LoadLibraryExA)
    SET_HOOK(LoadLibraryExW)
    SET_HOOK(FreeLibrary)
    SET_HOOK(GetProcAddress)
    SET_HOOK(CreateProcessA)
    SET_HOOK(CreateProcessW)
    SET_HOOK(CreateProcessAsUserW)

#   undef SET_HOOK
}


EXTERN_C BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    const char *szNewDllName = NULL;

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&g_Mutex);

        g_hThisModule = hinstDLL;

        /*
         * Calling LoadLibrary inside DllMain is strongly discouraged.  But it
         * works quite well, provided that the loaded DLL does not require or do
         * anything special in its DllMain, which seems to be the general case.
         *
         * See also:
         * - http://stackoverflow.com/questions/4370812/calling-loadlibrary-from-dllmain
         * - http://msdn.microsoft.com/en-us/library/ms682583
         */

        if (!USE_SHARED_MEM) {
            szNewDllName = getenv("INJECT_DLL");
            if (!szNewDllName) {
                debugPrintf("inject: warning: INJECT_DLL not set\n");
                return FALSE;
            }
        } else {
            SharedMem *pSharedMem = OpenSharedMemory(NULL);
            if (!pSharedMem) {
                debugPrintf("inject: error: failed to open shared memory\n");
                return FALSE;
            }

            VERBOSITY = pSharedMem->cVerbosity;

            static char szSharedMemCopy[MAX_PATH];
            strncpy(szSharedMemCopy, pSharedMem->szDllName, _countof(szSharedMemCopy) - 1);
            szSharedMemCopy[_countof(szSharedMemCopy) - 1] = '\0';

            szNewDllName = szSharedMemCopy;
        }

        if (VERBOSITY > 0) {
            debugPrintf("inject: DLL_PROCESS_ATTACH\n");
        }

        if (VERBOSITY > 0) {
            char szProcess[MAX_PATH];
            GetModuleFileNameA(NULL, szProcess, sizeof szProcess);
            debugPrintf("inject: attached to process %s\n", szProcess);
        }

        if (VERBOSITY > 0) {
            debugPrintf("inject: loading %s\n", szNewDllName);
        }

        g_hHookModule = LoadLibraryA(szNewDllName);
        if (!g_hHookModule) {
            debugPrintf("inject: warning: failed to load %s\n", szNewDllName);
            return FALSE;
        }

        patchAllModules(ACTION_HOOK);

        setHooks();

        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        if (VERBOSITY > 0) {
            debugPrintf("inject: DLL_PROCESS_DETACH\n");
        }

        assert(!lpvReserved);

        patchAllModules(ACTION_UNHOOK);

        if (g_hHookModule) {
            FreeLibrary(g_hHookModule);
        }

        Mhook_Unhook((PVOID*)&RealGetProcAddress);

        break;
    }
    return TRUE;
}


/*
 * Prevent the C/C++ runtime from destroying things when the program
 * terminates.
 *
 * There is no effective way to control the order DLLs receive
 * DLL_PROCESS_DETACH -- patched DLLs might get detacched after we are --, and
 * unpatching our hooks doesn't always work.  So instead just do nothing (and
 * prevent C/C++ runtime from doing anything too), so our hooks can still work
 * after we are dettached.
 */

#ifdef _MSC_VER
#  define DLLMAIN_CRT_STARTUP _DllMainCRTStartup
#else
#  define DLLMAIN_CRT_STARTUP DllMainCRTStartup
#  pragma GCC optimize ("no-stack-protector")
#endif

EXTERN_C BOOL WINAPI
DLLMAIN_CRT_STARTUP(HANDLE hDllHandle, DWORD dwReason, LPVOID lpvReserved);

EXTERN_C BOOL WINAPI
DllMainStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpvReserved)
{
    if (dwReason == DLL_PROCESS_DETACH && lpvReserved) {
        if (VERBOSITY > 0) {
            debugPrintf("inject: DLL_PROCESS_DETACH\n");
        }
        return TRUE;
    }

    return DLLMAIN_CRT_STARTUP(hDllHandle, dwReason, lpvReserved);
}

/**************************************************************************
 *
 * Copyright 2016-2018 VMware, Inc.
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


static int VERBOSITY = 0;
#define NOOP 0


static CRITICAL_SECTION g_Mutex;



static HMODULE g_hThisModule = NULL;
static HMODULE g_hHookModule = NULL;


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


static HMODULE WINAPI
MyLoadLibraryA(LPCSTR lpLibFileName);

static HMODULE WINAPI
MyLoadLibraryW(LPCWSTR lpLibFileName);

static HMODULE WINAPI
MyLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags);

static HMODULE WINAPI
MyLoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);

static FARPROC WINAPI
MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName);


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
    bRet = CreateProcessA(lpApplicationName,
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
    bRet = CreateProcessW(lpApplicationName,
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

typedef BOOL
(WINAPI *PFNCREATEPROCESSASUSERW) (HANDLE, LPCWSTR, LPWSTR,
        LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID,
        LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

static PFNCREATEPROCESSASUSERW pfnCreateProcessAsUserW;

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

    // Certain WINE versions (at least 1.6.2) don't export
    // kernel32.dll!CreateProcessAsUserW
    assert(pfnCreateProcessAsUserW);

    BOOL bRet;
    bRet = pfnCreateProcessAsUserW(hToken,
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


static const char *
getDescriptorName(HMODULE hModule,
                        const PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor)
{
    return rvaToVa<const char>(hModule, pImportDescriptor->Name);
}

static const char *
getDescriptorName(HMODULE hModule,
                  const PImgDelayDescr pDelayDescriptor)
{
    if (pDelayDescriptor->grAttrs & dlattrRva) {
        return rvaToVa<const char>(hModule, pDelayDescriptor->rvaDLLName);
    } else {
#ifdef _WIN64
        assert(pDelayDescriptor->grAttrs & dlattrRva);
        return "???";
#else
        // old-style, with ImgDelayDescr::szName being a LPCSTR
        return reinterpret_cast<LPCSTR>(pDelayDescriptor->rvaDLLName);
#endif
    }
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

template< typename T >
static bool
getImageDirectoryEntry(HMODULE hModule,
                       const char *szModule,
                       UINT Entry,
                       T ** ppEntry)
{
    MEMORY_BASIC_INFORMATION MemoryInfo;
    if (VirtualQuery(hModule, &MemoryInfo, sizeof MemoryInfo) != sizeof MemoryInfo) {
        debugPrintf("inject: warning: %s: VirtualQuery failed\n", szModule);
        return false;
    }
    if (MemoryInfo.Protect & (PAGE_NOACCESS | PAGE_EXECUTE)) {
        debugPrintf("inject: warning: %s: no read access (Protect = 0x%08lx)\n", szModule, MemoryInfo.Protect);
        return false;
    }

    PIMAGE_OPTIONAL_HEADER pOptionalHeader = getOptionalHeader(hModule, szModule);
    if (!pOptionalHeader) {
        return false;
    }

    assert(pOptionalHeader->NumberOfRvaAndSizes > Entry);
    if (pOptionalHeader->DataDirectory[Entry].Size == 0) {
        return false;
    }

    assert(pOptionalHeader->DataDirectory[Entry].Size >= sizeof(T));
    UINT_PTR ImportAddress = pOptionalHeader->DataDirectory[Entry].VirtualAddress;
    if (!ImportAddress) {
        return false;
    }

    *ppEntry = rvaToVa<T>(hModule, ImportAddress);
    return true;
}


static PIMAGE_IMPORT_DESCRIPTOR
getFirstImportDescriptor(HMODULE hModule, const char *szModule)
{
    PIMAGE_IMPORT_DESCRIPTOR pEntry = nullptr;
    return getImageDirectoryEntry(hModule, szModule, IMAGE_DIRECTORY_ENTRY_IMPORT, &pEntry) ? pEntry : nullptr;
}


static PImgDelayDescr
getDelayImportDescriptor(HMODULE hModule, const char *szModule)
{
    PImgDelayDescr pEntry = nullptr;
    return getImageDirectoryEntry(hModule, szModule, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, &pEntry) ? pEntry : nullptr;
}


static PIMAGE_EXPORT_DIRECTORY
getExportDescriptor(HMODULE hModule)
{
    PIMAGE_EXPORT_DIRECTORY pEntry = nullptr;
    return getImageDirectoryEntry(hModule, "(wrapper)", IMAGE_DIRECTORY_ENTRY_EXPORT, &pEntry) ? pEntry : nullptr;
}


static BOOL
replaceAddress(LPVOID *lpOldAddress, LPVOID lpNewAddress)
{
    DWORD flOldProtect;

    if (*lpOldAddress == lpNewAddress) {
        return TRUE;
    }

    EnterCriticalSection(&g_Mutex);

    if (!(VirtualProtect(lpOldAddress, sizeof *lpOldAddress, PAGE_READWRITE, &flOldProtect))) {
        LeaveCriticalSection(&g_Mutex);
        return FALSE;
    }

    *lpOldAddress = lpNewAddress;

    if (!(VirtualProtect(lpOldAddress, sizeof *lpOldAddress, flOldProtect, &flOldProtect))) {
        LeaveCriticalSection(&g_Mutex);
        return FALSE;
    }

    LeaveCriticalSection(&g_Mutex);
    return TRUE;
}


/* Return pointer to patcheable function address.
 *
 * See also:
 *
 * - An In-Depth Look into the Win32 Portable Executable File Format, Part 2, Matt Pietrek,
 *   http://msdn.microsoft.com/en-gb/magazine/cc301808.aspx
 *
 * - http://www.microsoft.com/msj/1298/hood/hood1298.aspx
 *
 */
static LPVOID *
getPatchAddress(HMODULE hModule,
                const char *szDescriptorName,
                DWORD OriginalFirstThunk,
                DWORD FirstThunk,
                const char* pszFunctionName,
                LPVOID lpOldAddress)
{
    if (VERBOSITY >= 4) {
        debugPrintf("inject: %s(%s, %s)\n", __FUNCTION__,
                    szDescriptorName,
                    pszFunctionName);
    }

    PIMAGE_THUNK_DATA pThunkIAT = rvaToVa<IMAGE_THUNK_DATA>(hModule, FirstThunk);

    UINT_PTR pOldFunction = (UINT_PTR)lpOldAddress;

    PIMAGE_THUNK_DATA pThunk;
    if (OriginalFirstThunk) {
        pThunk = rvaToVa<IMAGE_THUNK_DATA>(hModule, OriginalFirstThunk);
    } else {
        pThunk = pThunkIAT;
    }

    while (pThunk->u1.Function) {
        if (OriginalFirstThunk == 0 ||
            pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
            // No name -- search by the real function address
            if (!pOldFunction) {
                return NULL;
            }
            if (pThunkIAT->u1.Function == pOldFunction) {
                return (LPVOID *)(&pThunkIAT->u1.Function);
            }
        } else {
            // Search by name
            PIMAGE_IMPORT_BY_NAME pImport = rvaToVa<IMAGE_IMPORT_BY_NAME>(hModule, pThunk->u1.AddressOfData);
            const char* szName = (const char* )pImport->Name;
            if (strcmp(pszFunctionName, szName) == 0) {
                return (LPVOID *)(&pThunkIAT->u1.Function);
            }
        }
        ++pThunk;
        ++pThunkIAT;
    }

    return NULL;
}


static LPVOID *
getPatchAddress(HMODULE hModule,
                PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor,
                const char* pszFunctionName,
                LPVOID lpOldAddress)
{
    assert(pImportDescriptor->TimeDateStamp != 0 || pImportDescriptor->Name != 0);

    return getPatchAddress(hModule,
                           getDescriptorName(hModule, pImportDescriptor),
                           pImportDescriptor->OriginalFirstThunk,
                           pImportDescriptor->FirstThunk,
                           pszFunctionName,
                           lpOldAddress);
}


// See
// http://www.microsoft.com/msj/1298/hood/hood1298.aspx
// http://msdn.microsoft.com/en-us/library/16b2dyk5.aspx
static LPVOID *
getPatchAddress(HMODULE hModule,
                PImgDelayDescr pDelayDescriptor,
                const char* pszFunctionName,
                LPVOID lpOldAddress)
{
    assert(pDelayDescriptor->rvaDLLName != 0);

    return getPatchAddress(hModule,
                           getDescriptorName(hModule, pDelayDescriptor),
                           pDelayDescriptor->rvaINT,
                           pDelayDescriptor->rvaIAT,
                           pszFunctionName,
                           lpOldAddress);
}


template< class T >
static BOOL
patchFunction(HMODULE hModule,
              const char *szModule,
              const char *pszDllName,
              T pImportDescriptor,
              const char *pszFunctionName,
              LPVOID lpOldAddress,
              LPVOID lpNewAddress)
{
    LPVOID* lpPatchAddress = getPatchAddress(hModule, pImportDescriptor, pszFunctionName, lpOldAddress);
    if (lpPatchAddress == NULL) {
        return FALSE;
    }

    if (*lpPatchAddress == lpNewAddress) {
        return TRUE;
    }

    DWORD Offset = (DWORD)(UINT_PTR)lpPatchAddress - (UINT_PTR)hModule;
    if (VERBOSITY > 0) {
        debugPrintf("inject: patching %s!0x%lx -> %s!%s\n", szModule, Offset, pszDllName, pszFunctionName);
    }

    BOOL bRet;
    bRet = replaceAddress(lpPatchAddress, lpNewAddress);
    if (!bRet) {
        debugPrintf("inject: failed to patch %s!0x%lx -> %s!%s\n", szModule, Offset, pszDllName, pszFunctionName);
    }

    return bRet;
}



struct StrCompare : public std::binary_function<const char *, const char *, bool> {
    bool operator() (const char * s1, const char * s2) const {
        return strcmp(s1, s2) < 0;
    }
};

typedef std::map<const char *, LPVOID, StrCompare> FunctionMap;

struct StrICompare : public std::binary_function<const char *, const char *, bool> {
    bool operator() (const char * s1, const char * s2) const {
        return stricmp(s1, s2) < 0;
    }
};

struct Module {
    bool bInternal;
    FunctionMap functionMap;
};

typedef std::map<const char *, Module, StrICompare> ModulesMap;

/* This is only modified at DLL_PROCESS_ATTACH time. */
static ModulesMap modulesMap;


static inline bool
isMatchModuleName(const char *szModuleName)
{
    ModulesMap::const_iterator modIt = modulesMap.find(szModuleName);
    return modIt != modulesMap.end();
}


/* Set of previously hooked modules */
static std::set<HMODULE>
g_hHookedModules;


enum Action {
    ACTION_HOOK,
    ACTION_UNHOOK,

};


template< class T >
void
patchDescriptor(HMODULE hModule,
                const char *szModule,
                T pImportDescriptor,
                Action action)
{
    const char* szDescriptorName = getDescriptorName(hModule, pImportDescriptor);

    ModulesMap::const_iterator modIt = modulesMap.find(szDescriptorName);
    if (modIt != modulesMap.end()) {
        const char *szMatchModule = modIt->first; // same as szDescriptorName
        const Module & module = modIt->second;

        const FunctionMap & functionMap = module.functionMap;
        FunctionMap::const_iterator fnIt;
        for (fnIt = functionMap.begin(); fnIt != functionMap.end(); ++fnIt) {
            const char *szFunctionName = fnIt->first;
            LPVOID lpHookAddress = fnIt->second;

            // Knowning the real address is useful when patching imports by ordinal
            LPVOID lpRealAddress = NULL;
            HMODULE hRealModule = GetModuleHandleA(szDescriptorName);
            if (hRealModule) {
                // FIXME: this assertion can fail when the wrapper name is the same as the original DLL
                //assert(hRealModule != g_hHookModule);
                if (hRealModule != g_hHookModule) {
                    lpRealAddress = (LPVOID)GetProcAddress(hRealModule, szFunctionName);
                }
            }
            
            LPVOID lpOldAddress = lpRealAddress;
            LPVOID lpNewAddress = lpHookAddress;

            if (action == ACTION_UNHOOK) {
                std::swap(lpOldAddress, lpNewAddress);
            }

            BOOL bPatched;
            bPatched = patchFunction(hModule, szModule, szMatchModule, pImportDescriptor, szFunctionName, lpOldAddress, lpNewAddress);
            if (action == ACTION_HOOK && bPatched && !module.bInternal && pSharedMem) {
                pSharedMem->bReplaced = TRUE;
            }
        }
    }
}


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

    const char *szBaseName = getBaseName(szModule);

    /* Don't hook our replacement modules to avoid tracing internal APIs */
    /* XXX: is this really a good idea? */
    if (isMatchModuleName(szBaseName)) {
        return;
    }

    /* Leave these modules alone.
     *
     * Hooking other injection DLLs easily leads to infinite recursion (and
     * stack overflow), especially when those libraries use techniques like
     * modifying the hooked functions prolog (instead of patching IAT like we
     * do).
     *
     * See also:
     * - http://www.nynaeve.net/?p=62
     */
    if (stricmp(szBaseName, "kernel32.dll") == 0 ||
        stricmp(szBaseName, "AcLayers.dll") == 0 ||
        stricmp(szBaseName, "ConEmuHk.dll") == 0 ||
        stricmp(szBaseName, "gameoverlayrenderer.dll") == 0 ||
        stricmp(szBaseName, "gameoverlayrenderer64.dll") == 0) {
        return;
    }

    if (VERBOSITY > 0) {
        debugPrintf("inject: found module %s\n", szModule);
    }

    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = getFirstImportDescriptor(hModule, szModule);
    if (pImportDescriptor) {
        while (pImportDescriptor->FirstThunk) {

            patchDescriptor(hModule, szModule, pImportDescriptor, action);

            ++pImportDescriptor;
        }
    }

    PImgDelayDescr pDelayDescriptor = getDelayImportDescriptor(hModule, szModule);
    if (pDelayDescriptor) {
        while (pDelayDescriptor->rvaDLLName) {
            if (pDelayDescriptor->grAttrs > 1) {
                debugPrintf("inject: warning: ignoring delay import section (grAttrs = 0x%08lx)\n",
                            pDelayDescriptor->grAttrs);
                break;
            }

            if (VERBOSITY > 1) {
                const char* szName = getDescriptorName(hModule, pDelayDescriptor);
                debugPrintf("inject: found %sdelay-load import entry for module %s\n",
                            pDelayDescriptor->grAttrs & dlattrRva ? "" : "old-style ",
                            szName);
            }

#ifdef _WIN64
            assert(pDelayDescriptor->grAttrs & dlattrRva);
#endif

            patchDescriptor(hModule, szModule, pDelayDescriptor, action);

            ++pDelayDescriptor;
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
    HMODULE hModule = LoadLibraryA(lpLibFileName);
    DWORD dwLastError = GetLastError();

    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(\"%s\") = 0x%p\n",
                    __FUNCTION__ + 2, lpLibFileName, hModule);
    }

    if (VERBOSITY > 0) {
        const char *szBaseName = getBaseName(lpLibFileName);
        if (isMatchModuleName(szBaseName)) {
            if (VERBOSITY < 2) {
                debugPrintf("inject: intercepting %s(\"%s\")\n", __FUNCTION__, lpLibFileName);
            }
#ifdef __GNUC__
            void *caller = __builtin_return_address (0);

            HMODULE hModule = 0;
            BOOL bRet = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                          (LPCTSTR)caller,
                                          &hModule);
            assert(bRet);
            char szCaller[MAX_PATH];
            DWORD dwRet = GetModuleFileNameA(hModule, szCaller, sizeof szCaller);
            assert(dwRet);
            debugPrintf("inject: called from %s\n", szCaller);
#endif
        }
    }

    // Hook all new modules (and not just this one, to pick up any dependencies)
    patchAllModules(ACTION_HOOK);

    SetLastError(dwLastError);
    return hModule;
}

static HMODULE WINAPI
MyLoadLibraryW(LPCWSTR lpLibFileName)
{
    HMODULE hModule = LoadLibraryW(lpLibFileName);
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
    HMODULE hModule = LoadLibraryExA(lpLibFileName, hFile, adjustFlags(dwFlags));
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
    HMODULE hModule = LoadLibraryExW(lpLibFileName, hFile, adjustFlags(dwFlags));
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
logGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    if (HIWORD(lpProcName) == 0) {
        debugPrintf("inject: intercepting %s(%u)\n", "GetProcAddress", LOWORD(lpProcName));
    } else {
        debugPrintf("inject: intercepting %s(\"%s\")\n", "GetProcAddress", lpProcName);
    }
}

static FARPROC WINAPI
MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {

    if (VERBOSITY >= 3) {
        /* XXX this can cause segmentation faults */
        logGetProcAddress(hModule, lpProcName);
    }

    if (!NOOP) {
        char szModule[MAX_PATH];
        DWORD dwRet = GetModuleFileNameA(hModule, szModule, sizeof szModule);
        assert(dwRet);
        const char *szBaseName = getBaseName(szModule);

        ModulesMap::const_iterator modIt;
        modIt = modulesMap.find(szBaseName);
        if (modIt != modulesMap.end()) {
            if (VERBOSITY > 1 && VERBOSITY < 3) {
                logGetProcAddress(hModule, lpProcName);
            }

            const Module & module = modIt->second;
            const FunctionMap & functionMap = module.functionMap;
            FunctionMap::const_iterator fnIt;

            if (HIWORD(lpProcName) == 0) {
                FARPROC proc = GetProcAddress(hModule, lpProcName);
                if (!proc) {
                    return proc;
                }

                for (fnIt = functionMap.begin(); fnIt != functionMap.end(); ++fnIt) {
                    FARPROC pRealProc = GetProcAddress(hModule, fnIt->first);
                    if (proc == pRealProc) {
                        if (VERBOSITY > 0) {
                            debugPrintf("inject: replacing %s!%s\n", szBaseName, lpProcName);
                        }
                        return (FARPROC)fnIt->second;
                    }

                }
                
                debugPrintf("inject: ignoring %s!@%u\n", szBaseName, LOWORD(lpProcName));

                return proc;
            }

            fnIt = functionMap.find(lpProcName);

            if (fnIt != functionMap.end()) {
                LPVOID pProcAddress = fnIt->second;
                if (VERBOSITY > 0) {
                    debugPrintf("inject: replacing %s!%s\n", szBaseName, lpProcName);
                }
                if (!module.bInternal && pSharedMem) {
                    pSharedMem->bReplaced = TRUE;
                }
                return (FARPROC)pProcAddress;
            } else {
                if (VERBOSITY > 0 && !module.bInternal) {
                    debugPrintf("inject: ignoring %s!%s\n", szBaseName, lpProcName);
                }
            }
        }
    }

    return GetProcAddress(hModule, lpProcName);
}


static BOOL WINAPI
MyFreeLibrary(HMODULE hModule)
{
    if (VERBOSITY >= 2) {
        debugPrintf("inject: intercepting %s(0x%p)\n", __FUNCTION__, hModule);
    }

    BOOL bRet = FreeLibrary(hModule);
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
registerLibraryLoaderHooks(const char *szMatchModule)
{
    Module & module = modulesMap[szMatchModule];
    module.bInternal = true;
    FunctionMap & functionMap = module.functionMap;
    functionMap["LoadLibraryA"]   = (LPVOID)MyLoadLibraryA;
    functionMap["LoadLibraryW"]   = (LPVOID)MyLoadLibraryW;
    functionMap["LoadLibraryExA"] = (LPVOID)MyLoadLibraryExA;
    functionMap["LoadLibraryExW"] = (LPVOID)MyLoadLibraryExW;
    functionMap["GetProcAddress"] = (LPVOID)MyGetProcAddress;
    functionMap["FreeLibrary"]    = (LPVOID)MyFreeLibrary;
}

static void
registerProcessThreadsHooks(const char *szMatchModule)
{
    Module & module = modulesMap[szMatchModule];
    module.bInternal = true;
    FunctionMap & functionMap = module.functionMap;
    functionMap["CreateProcessA"]       = (LPVOID)MyCreateProcessA;
    functionMap["CreateProcessW"]       = (LPVOID)MyCreateProcessW;
    // NOTE: CreateProcessAsUserA is implemented by advapi32.dll
    functionMap["CreateProcessAsUserW"] = (LPVOID)MyCreateProcessAsUserW;
    // TODO: CreateProcessWithTokenW
}

static void
registerModuleHooks(const char *szMatchModule, HMODULE hReplaceModule)
{
    Module & module = modulesMap[szMatchModule];
    module.bInternal = false;
    FunctionMap & functionMap = module.functionMap;

    PIMAGE_EXPORT_DIRECTORY pExportDescriptor = getExportDescriptor(hReplaceModule);
    assert(pExportDescriptor);

    DWORD *pAddressOfNames = (DWORD *)((BYTE *)hReplaceModule + pExportDescriptor->AddressOfNames);
    for (DWORD i = 0; i < pExportDescriptor->NumberOfNames; ++i) {
        const char *szFunctionName = (const char *)((BYTE *)hReplaceModule + pAddressOfNames[i]);
        LPVOID lpNewAddress = (LPVOID)GetProcAddress(hReplaceModule, szFunctionName);
        assert(lpNewAddress);

        functionMap[szFunctionName] = lpNewAddress;
    }
}

static void
dumpRegisteredHooks(void)
{
    if (VERBOSITY > 1) {
        ModulesMap::const_iterator modIt;
        for (modIt = modulesMap.begin(); modIt != modulesMap.end(); ++modIt) {
            const char *szMatchModule = modIt->first;
            const Module & module = modIt->second;
            const FunctionMap & functionMap = module.functionMap;
            FunctionMap::const_iterator fnIt;
            for (fnIt = functionMap.begin(); fnIt != functionMap.end(); ++fnIt) {
                const char *szFunctionName = fnIt->first;
                debugPrintf("inject: registered hook for %s!%s%s\n",
                            szMatchModule, szFunctionName,
                            module.bInternal ? " (internal)" : "");
            }
        }
    }
}


EXTERN_C BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    const char *szNewDllName = NULL;
    const char *szNewDllBaseName;

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

        // Ensure we use kernel32.dll's CreateProcessAsUserW, and not advapi32.dll's.
        {
            HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
            assert(hKernel32);
            pfnCreateProcessAsUserW = (PFNCREATEPROCESSASUSERW)GetProcAddress(hKernel32, "CreateProcessAsUserW");
        }

        /*
         * Hook kernel32.dll functions, and its respective Windows API Set.
         *
         * http://msdn.microsoft.com/en-us/library/dn505783.aspx (Windows 8.1)
         * http://msdn.microsoft.com/en-us/library/hh802935.aspx (Windows 8)
         */

        registerLibraryLoaderHooks("kernel32.dll");
        registerLibraryLoaderHooks("api-ms-win-core-libraryloader-l1-1-0.dll");
        registerLibraryLoaderHooks("api-ms-win-core-libraryloader-l1-1-1.dll");
        registerLibraryLoaderHooks("api-ms-win-core-libraryloader-l1-2-0.dll");
        registerLibraryLoaderHooks("api-ms-win-core-kernel32-legacy-l1-1-0.dll");
        registerLibraryLoaderHooks("api-ms-win-core-kernel32-legacy-l1-1-1.dll");

        registerProcessThreadsHooks("kernel32.dll");
        registerProcessThreadsHooks("api-ms-win-core-processthreads-l1-1-0.dll");
        registerProcessThreadsHooks("api-ms-win-core-processthreads-l1-1-1.dll");
        registerProcessThreadsHooks("api-ms-win-core-processthreads-l1-1-2.dll");

        szNewDllBaseName = getBaseName(szNewDllName);
        if (stricmp(szNewDllBaseName, "dxgitrace.dll") == 0) {
            registerModuleHooks("dxgi.dll",    g_hHookModule);
            registerModuleHooks("d3d10.dll",   g_hHookModule);
            registerModuleHooks("d3d10_1.dll", g_hHookModule);
            registerModuleHooks("d3d11.dll",   g_hHookModule);
            registerModuleHooks("d3d9.dll",    g_hHookModule); // for D3DPERF_*
            registerModuleHooks("dcomp.dll",   g_hHookModule);
        } else if (stricmp(szNewDllBaseName, "d3d9.dll") == 0) {
            registerModuleHooks("d3d9.dll",    g_hHookModule);
            registerModuleHooks("dxva2.dll",   g_hHookModule);
        } else if (stricmp(szNewDllBaseName, "d2d1trace.dll") == 0) {
            registerModuleHooks("d2d1.dll",    g_hHookModule);
            registerModuleHooks("dwrite.dll",  g_hHookModule);
        } else {
            registerModuleHooks(szNewDllBaseName, g_hHookModule);
        }

        dumpRegisteredHooks();

        patchAllModules(ACTION_HOOK);
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

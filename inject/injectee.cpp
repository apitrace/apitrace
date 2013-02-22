/**************************************************************************
 *
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

#include <windows.h>
#include <tlhelp32.h>

#include "inject.h"


#define VERBOSITY 0
#define NOOP 0


static CRITICAL_SECTION Mutex = {(PCRITICAL_SECTION_DEBUG)-1, -1, 0, 0, 0, 0};


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


static const char *
getImportDescriptionName(HMODULE hModule, const PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor) {
    const char* szName = (const char*)((PBYTE)hModule + pImportDescriptor->Name);
    return szName;
}


static PIMAGE_IMPORT_DESCRIPTOR
getImportDescriptor(HMODULE hModule,
                    const char *szModule,
                    const char *pszDllName)
{
    MEMORY_BASIC_INFORMATION MemoryInfo;
    if (VirtualQuery(hModule, &MemoryInfo, sizeof MemoryInfo) != sizeof MemoryInfo) {
        debugPrintf("%s: %s: VirtualQuery failed\n", __FUNCTION__, szModule);
        return NULL;
    }
    if (MemoryInfo.Protect & (PAGE_NOACCESS | PAGE_EXECUTE)) {
        debugPrintf("%s: %s: no read access (Protect = 0x%08x)\n", __FUNCTION__, szModule, MemoryInfo.Protect);
        return NULL;
    }

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((PBYTE)hModule + pDosHeader->e_lfanew);

    PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pNtHeaders->OptionalHeader;

    UINT_PTR ImportAddress = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    if (!ImportAddress) {
        return NULL;
    }

    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)hModule + ImportAddress);

    while (pImportDescriptor->FirstThunk) {
        const char* szName = getImportDescriptionName(hModule, pImportDescriptor);
        if (stricmp(pszDllName, szName) == 0) {
            return pImportDescriptor;
        }
        ++pImportDescriptor;
    }

    return NULL;
}


static BOOL
replaceAddress(LPVOID *lpOldAddress, LPVOID lpNewAddress)
{
    DWORD flOldProtect;

    if (*lpOldAddress == lpNewAddress) {
        return TRUE;
    }

    EnterCriticalSection(&Mutex);

    if (!(VirtualProtect(lpOldAddress, sizeof *lpOldAddress, PAGE_READWRITE, &flOldProtect))) {
        LeaveCriticalSection(&Mutex);
        return FALSE;
    }

    *lpOldAddress = lpNewAddress;

    if (!(VirtualProtect(lpOldAddress, sizeof *lpOldAddress, flOldProtect, &flOldProtect))) {
        LeaveCriticalSection(&Mutex);
        return FALSE;
    }

    LeaveCriticalSection(&Mutex);
    return TRUE;
}


static LPVOID *
getOldFunctionAddress(HMODULE hModule,
                    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor,
                    const char* pszFunctionName)
{
    PIMAGE_THUNK_DATA pOriginalFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pImportDescriptor->OriginalFirstThunk);
    PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pImportDescriptor->FirstThunk);

    //debugPrintf("  %s\n", __FUNCTION__);

    while (pOriginalFirstThunk->u1.Function) {
        PIMAGE_IMPORT_BY_NAME pImport = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hModule + pOriginalFirstThunk->u1.AddressOfData);
        const char* szName = (const char* )pImport->Name;
        //debugPrintf("    %s\n", szName);
        if (strcmp(pszFunctionName, szName) == 0) {
            //debugPrintf("  %s succeeded\n", __FUNCTION__);
            return (LPVOID *)(&pFirstThunk->u1.Function);
        }
        ++pOriginalFirstThunk;
        ++pFirstThunk;
    }

    //debugPrintf("  %s failed\n", __FUNCTION__);

    return NULL;
}


static void
replaceModule(HMODULE hModule,
              const char *szModule,
              PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor,
              HMODULE hNewModule)
{
    PIMAGE_THUNK_DATA pOriginalFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pImportDescriptor->OriginalFirstThunk);
    PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pImportDescriptor->FirstThunk);

    while (pOriginalFirstThunk->u1.Function) {
        PIMAGE_IMPORT_BY_NAME pImport = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hModule + pOriginalFirstThunk->u1.AddressOfData);
        const char* szFunctionName = (const char* )pImport->Name;
        if (VERBOSITY > 0) {
            debugPrintf("      hooking %s->%s!%s\n", szModule,
                    getImportDescriptionName(hModule, pImportDescriptor),
                    szFunctionName);
        }

        PROC pNewProc = GetProcAddress(hNewModule, szFunctionName);
        if (!pNewProc) {
            debugPrintf("warning: no replacement for %s\n", szFunctionName);
        } else {
            LPVOID *lpOldAddress = (LPVOID *)(&pFirstThunk->u1.Function);
            replaceAddress(lpOldAddress, (LPVOID)pNewProc);
        }

        ++pOriginalFirstThunk;
        ++pFirstThunk;
    }
}


static BOOL
hookFunction(HMODULE hModule,
             const char *szModule,
             const char *pszDllName,
             const char *pszFunctionName,
             LPVOID lpNewAddress)
{
    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = getImportDescriptor(hModule, szModule, pszDllName);
    if (pImportDescriptor == NULL) {
        return FALSE;
    }
    LPVOID* lpOldFunctionAddress = getOldFunctionAddress(hModule, pImportDescriptor, pszFunctionName);
    if (lpOldFunctionAddress == NULL) {
        return FALSE;
    }

    if (*lpOldFunctionAddress == lpNewAddress) {
        return TRUE;
    }

    if (VERBOSITY >= 3) {
        debugPrintf("      hooking %s->%s!%s\n", szModule, pszDllName, pszFunctionName);
    }

    return replaceAddress(lpOldFunctionAddress, lpNewAddress);
}


static BOOL
replaceImport(HMODULE hModule,
              const char *szModule,
              const char *pszDllName,
              HMODULE hNewModule)
{
    if (NOOP) {
        return TRUE;
    }

    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = getImportDescriptor(hModule, szModule, pszDllName);
    if (pImportDescriptor == NULL) {
        return TRUE;
    }

    replaceModule(hModule, szModule, pImportDescriptor, hNewModule);

    return TRUE;
}

static HMODULE g_hThisModule = NULL;


struct Replacement {
    const char *szMatchModule;
    HMODULE hReplaceModule;
};

static unsigned numReplacements = 0;
static Replacement replacements[32];



static void
hookModule(HMODULE hModule,
           const char *szModule)
{
    if (hModule == g_hThisModule) {
        return;
    }

    for (unsigned i = 0; i < numReplacements; ++i) {
        if (hModule == replacements[i].hReplaceModule) {
            return;
        }
    }

    hookFunction(hModule, szModule, "kernel32.dll", "LoadLibraryA", (LPVOID)MyLoadLibraryA);
    hookFunction(hModule, szModule, "kernel32.dll", "LoadLibraryW", (LPVOID)MyLoadLibraryW);
    hookFunction(hModule, szModule, "kernel32.dll", "LoadLibraryExA", (LPVOID)MyLoadLibraryExA);
    hookFunction(hModule, szModule, "kernel32.dll", "LoadLibraryExW", (LPVOID)MyLoadLibraryExW);
    hookFunction(hModule, szModule, "kernel32.dll", "GetProcAddress", (LPVOID)MyGetProcAddress);

    const char *szBaseName = getBaseName(szModule);
    for (unsigned i = 0; i < numReplacements; ++i) {
        if (stricmp(szBaseName, replacements[i].szMatchModule) == 0) {
            return;
        }
    }

    /* Don't hook internal dependencies */
    if (stricmp(szBaseName, "d3d10core.dll") == 0 ||
        stricmp(szBaseName, "d3d10level9.dll") == 0 ||
        stricmp(szBaseName, "d3d10sdklayers.dll") == 0 ||
        stricmp(szBaseName, "d3d10_1core.dll") == 0 ||
        stricmp(szBaseName, "d3d11sdklayers.dll") == 0 ||
        stricmp(szBaseName, "d3d11_1sdklayers.dll") == 0) {
        return;
    }

    for (unsigned i = 0; i < numReplacements; ++i) {
        replaceImport(hModule, szModule, replacements[i].szMatchModule, replacements[i].hReplaceModule);
        replaceImport(hModule, szModule, replacements[i].szMatchModule, replacements[i].hReplaceModule);
        replaceImport(hModule, szModule, replacements[i].szMatchModule, replacements[i].hReplaceModule);
    }
}

static void
hookAllModules(void)
{
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof me32;

    if (VERBOSITY > 0) {
        static bool first = true;
        if (first) {
            if (Module32First(hModuleSnap, &me32)) {
                debugPrintf("  modules:\n");
                do  {
                    debugPrintf("     %s\n", me32.szExePath);
                } while (Module32Next(hModuleSnap, &me32));
            }
            first = false;
        }
    }

    if (Module32First(hModuleSnap, &me32)) {
        do  {
            hookModule(me32.hModule, me32.szExePath);
        } while (Module32Next(hModuleSnap, &me32));
    }

    CloseHandle(hModuleSnap);
}




static HMODULE WINAPI
MyLoadLibrary(LPCSTR lpLibFileName, HANDLE hFile = NULL, DWORD dwFlags = 0)
{
    // To Send the information to the server informing that,
    // LoadLibrary is invoked.
    HMODULE hModule = LoadLibraryExA(lpLibFileName, hFile, dwFlags);

    //hookModule(hModule, lpLibFileName);
    hookAllModules();

    return hModule;
}

static HMODULE WINAPI
MyLoadLibraryA(LPCSTR lpLibFileName)
{
    if (VERBOSITY >= 2) {
        debugPrintf("%s(\"%s\")\n", __FUNCTION__, lpLibFileName);
    }

    if (VERBOSITY > 0) {
        const char *szBaseName = getBaseName(lpLibFileName);
        for (unsigned i = 0; i < numReplacements; ++i) {
            if (stricmp(szBaseName, replacements[i].szMatchModule) == 0) {
                debugPrintf("%s(\"%s\")\n", __FUNCTION__, lpLibFileName);
#ifdef __GNUC__
                void *caller = __builtin_return_address (0);

                HMODULE hModule = 0;
                BOOL bRet = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                         (LPCTSTR)caller,
                                         &hModule);
                assert(bRet);
                char szCaller[256];
                DWORD dwRet = GetModuleFileNameA(hModule, szCaller, sizeof szCaller);
                assert(dwRet);
                debugPrintf("  called from %s\n", szCaller);
#endif
                break;
            }
        }
    }

    return MyLoadLibrary(lpLibFileName);
}

static HMODULE WINAPI
MyLoadLibraryW(LPCWSTR lpLibFileName)
{
    if (VERBOSITY >= 2) {
        debugPrintf("%s(L\"%S\")\n", __FUNCTION__, lpLibFileName);
    }

    char szFileName[256];
    wcstombs(szFileName, lpLibFileName, sizeof szFileName);

    return MyLoadLibrary(szFileName);
}

static HMODULE WINAPI
MyLoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    if (VERBOSITY >= 2) {
        debugPrintf("%s(\"%s\")\n", __FUNCTION__, lpLibFileName);
    }
    return MyLoadLibrary(lpLibFileName, hFile, dwFlags);
}

static HMODULE WINAPI
MyLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    if (VERBOSITY >= 2) {
        debugPrintf("%s(L\"%S\")\n", __FUNCTION__, lpLibFileName);
    }

    char szFileName[256];
    wcstombs(szFileName, lpLibFileName, sizeof szFileName);

    return MyLoadLibrary(szFileName, hFile, dwFlags);
}

static FARPROC WINAPI
MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {

    if (VERBOSITY >= 99) {
        /* XXX this can cause segmentation faults */
        debugPrintf("%s(\"%s\")\n", __FUNCTION__, lpProcName);
    }

    assert(hModule != g_hThisModule);
    for (unsigned i = 0; i < numReplacements; ++i) {
        if (hModule == replacements[i].hReplaceModule) {
            return GetProcAddress(hModule, lpProcName);
        }
    }

    if (!NOOP) {
        char szModule[256];
        DWORD dwRet = GetModuleFileNameA(hModule, szModule, sizeof szModule);
        assert(dwRet);
        const char *szBaseName = getBaseName(szModule);

        for (unsigned i = 0; i < numReplacements; ++i) {

            if (stricmp(szBaseName, replacements[i].szMatchModule) == 0) {
                if (VERBOSITY > 0) {
                    debugPrintf("  %s(\"%s\", \"%s\")\n", __FUNCTION__, szModule, lpProcName);
                }
                FARPROC pProcAddress = GetProcAddress(replacements[i].hReplaceModule, lpProcName);
                if (pProcAddress) {
                    if (VERBOSITY >= 2) {
                        debugPrintf("      replacing %s!%s\n", szBaseName, lpProcName);
                    }
                    return pProcAddress;
                } else {
                    if (VERBOSITY > 0) {
                        debugPrintf("      ignoring %s!%s\n", szBaseName, lpProcName);
                    }
                    break;
                }
            }
        }
    }

    return GetProcAddress(hModule, lpProcName);
}


EXTERN_C BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    const char *szNewDllName = NULL;
    HMODULE hNewModule = NULL;
    const char *szNewDllBaseName;

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        if (VERBOSITY > 0) {
            debugPrintf("DLL_PROCESS_ATTACH\n");
        }

        g_hThisModule = hinstDLL;

        {
            char szProcess[MAX_PATH];
            GetModuleFileNameA(NULL, szProcess, sizeof szProcess);
            if (VERBOSITY > 0) {
                debugPrintf("  attached to %s\n", szProcess);
            }
        }

        /*
         * Calling LoadLibrary inside DllMain is strongly discouraged.  But it
         * works quite well, provided that the loaded DLL does not require or do
         * anything special in its DllMain, which seems to be the general case.
         *
         * See also:
         * - http://stackoverflow.com/questions/4370812/calling-loadlibrary-from-dllmain
         * - http://msdn.microsoft.com/en-us/library/ms682583
         */

#if !USE_SHARED_MEM
        szNewDllName = getenv("INJECT_DLL");
        if (!szNewDllName) {
            debugPrintf("warning: INJECT_DLL not set\n");
            return FALSE;
        }
#else
        static char szSharedMemCopy[MAX_PATH];
        GetSharedMem(szSharedMemCopy, sizeof szSharedMemCopy);
        szNewDllName = szSharedMemCopy;
#endif
        if (VERBOSITY > 0) {
            debugPrintf("  injecting %s\n", szNewDllName);
        }

        hNewModule = LoadLibraryA(szNewDllName);
        if (!hNewModule) {
            debugPrintf("warning: failed to load %s\n", szNewDllName);
            return FALSE;
        }

        szNewDllBaseName = getBaseName(szNewDllName);
        if (stricmp(szNewDllBaseName, "dxgitrace.dll") == 0) {
            replacements[numReplacements].szMatchModule = "dxgi.dll";
            replacements[numReplacements].hReplaceModule = hNewModule;
            ++numReplacements;

            replacements[numReplacements].szMatchModule = "d3d10.dll";
            replacements[numReplacements].hReplaceModule = hNewModule;
            ++numReplacements;

            replacements[numReplacements].szMatchModule = "d3d10_1.dll";
            replacements[numReplacements].hReplaceModule = hNewModule;
            ++numReplacements;

            replacements[numReplacements].szMatchModule = "d3d11.dll";
            replacements[numReplacements].hReplaceModule = hNewModule;
            ++numReplacements;
        } else {
            replacements[numReplacements].szMatchModule = szNewDllBaseName;
            replacements[numReplacements].hReplaceModule = hNewModule;
            ++numReplacements;
        }

        hookAllModules();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        if (VERBOSITY > 0) {
            debugPrintf("DLL_PROCESS_DETACH\n");
        }
        break;
    }
    return TRUE;
}

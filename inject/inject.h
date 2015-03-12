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


#include <windows.h>
#include <sddl.h>


static void
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
debugPrintf(const char *format, ...);


static void
logLastError(const char *szMsg)
{
    DWORD dwLastError = GetLastError();

    // http://msdn.microsoft.com/en-gb/library/windows/desktop/ms680582.aspx
    LPSTR lpErrorMsg = NULL;
    DWORD cbWritten;
    cbWritten = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &lpErrorMsg,
        0, NULL);

    if (cbWritten) {
        debugPrintf("inject: error: %s: %s", szMsg, lpErrorMsg);
    } else {
        debugPrintf("inject: error: %s: %lu\n", szMsg, dwLastError);
    }

    LocalFree(lpErrorMsg);
}


static inline const char *
getSeparator(const char *szFilename) {
    const char *p, *q;
    p = NULL;
    q = szFilename;
    char c;
    do  {
        c = *q++;
        if (c == '\\' || c == '/' || c == ':') {
            p = q;
        }
    } while (c);
    return p;
}


static inline const char *
getBaseName(const char *szFilename) {
    const char *pSeparator = getSeparator(szFilename);
    if (!pSeparator) {
        return szFilename;
    }
    return pSeparator;
}


static inline void
getDirName(char *szFilename) {
    char *pSeparator = const_cast<char *>(getSeparator(szFilename));
    if (pSeparator) {
        *pSeparator = '\0';
    }
}


static inline void
getModuleName(char *szModuleName, size_t n, const char *szFilename) {
    char *p = szModuleName;
    const char *q = getBaseName(szFilename);
    char c;
    while (--n) {
        c = *q++;
        if (c == '.' || c == '\0') {
            break;
        }
        *p++ = c;
    };
    *p++ = '\0';
}


#define USE_SHARED_MEM 1


struct SharedMem
{
    BOOL bReplaced;
    char szDllName[4096 - sizeof(BOOL)];
};


static SharedMem *pSharedMem = NULL;
static HANDLE hFileMapping = NULL;


static SharedMem *
OpenSharedMemory(void) {
    if (pSharedMem) {
        return pSharedMem;
    }

    // Create a NULL DACL to enable the shared memory being accessed by any
    // process we attach to.
    SECURITY_ATTRIBUTES sa;
    SECURITY_DESCRIPTOR sd;
    LPSECURITY_ATTRIBUTES lpSA;
    if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) &&
        SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
    {
        ZeroMemory(&sa, sizeof sa);
        sa.nLength = sizeof sa;
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = &sd;
        lpSA = &sa;
    }

    hFileMapping = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // system paging file
        lpSA,                   // lpAttributes
        PAGE_READWRITE,         // read/write access
        0,                      // dwMaximumSizeHigh
        sizeof(SharedMem),      // dwMaximumSizeLow
        TEXT("injectfilemap")); // name of map object

    if (hFileMapping == NULL) {
        logLastError("failed to create file mapping");
        return NULL;
    }

    BOOL bAlreadyExists = (GetLastError() == ERROR_ALREADY_EXISTS);

    pSharedMem = (SharedMem *)MapViewOfFile(
        hFileMapping,
        FILE_MAP_WRITE, // read/write access
        0,              // dwFileOffsetHigh
        0,              // dwFileOffsetLow
        0);             // dwNumberOfBytesToMap (entire file)
    if (pSharedMem == NULL) {
        logLastError("failed to map view");
        return NULL;
    }

    if (!bAlreadyExists) {
        memset(pSharedMem, 0, sizeof *pSharedMem);
    }

    return pSharedMem;
}


static inline VOID
CloseSharedMem(void) {
    if (!pSharedMem) {
        return;
    }

    UnmapViewOfFile(pSharedMem);
    pSharedMem = NULL;

    CloseHandle(hFileMapping);
    hFileMapping = NULL;
}


static inline VOID
SetSharedMem(LPCSTR lpszSrc) {
    SharedMem *pSharedMem = OpenSharedMemory();
    if (!pSharedMem) {
        return;
    }

    LPSTR lpszDst = pSharedMem->szDllName;

    size_t n = 1;
    while (*lpszSrc && n < sizeof pSharedMem->szDllName) {
        *lpszDst++ = *lpszSrc++;
        n++;
    }
    *lpszDst = '\0';
}


static inline VOID
GetSharedMem(LPSTR lpszDst, size_t n) {
    SharedMem *pSharedMem = OpenSharedMemory();
    if (!pSharedMem) {
        return;
    }

    LPCSTR lpszSrc = pSharedMem->szDllName;

    while (*lpszSrc && --n) {
        *lpszDst++ = *lpszSrc++;
    }
    *lpszDst = '\0';
}


static BOOL
injectDll(HANDLE hProcess, const char *szDllPath)
{
    BOOL bRet = FALSE;
    PTHREAD_START_ROUTINE lpStartAddress;
    HANDLE hThread;
    DWORD hModule;

    // Allocate memory in the target process to hold the DLL name
    size_t szDllPathLength = strlen(szDllPath) + 1;
    void *lpMemory = VirtualAllocEx(hProcess, NULL, szDllPathLength, MEM_COMMIT, PAGE_READWRITE);
    if (!lpMemory) {
        logLastError("failed to allocate memory in the process");
        goto no_memory;
    }

    // Copy DLL name into the target process
    if (!WriteProcessMemory(hProcess, lpMemory, szDllPath, szDllPathLength, NULL)) {
        logLastError("failed to write into process memory");
        goto no_thread;
    }

    /*
     * Get LoadLibraryA address from kernel32.dll.  It's the same for all the
     * process (XXX: but only within the same architecture).
     */
    lpStartAddress =
        (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("KERNEL32"), "LoadLibraryA");

    // Create remote thread in another process
    hThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, lpMemory, 0, NULL);
    if (!hThread) {
        logLastError("failed to create remote thread");
        goto no_thread;
    }

    // Wait for it to finish
    WaitForSingleObject(hThread, INFINITE);

    GetExitCodeThread(hThread, &hModule);
    if (!hModule) {
        debugPrintf("inject: error: failed to load %s into the remote process\n", szDllPath);
    } else {
        bRet = TRUE;
    }

    CloseHandle(hThread);
no_thread:
    VirtualFreeEx(hProcess, lpMemory, 0, MEM_RELEASE);
no_memory:
    return bRet;
}

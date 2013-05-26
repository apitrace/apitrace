/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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
 * Main program to start and inject a DLL into a process via a remote thread.
 *
 * For background see:
 * - http://en.wikipedia.org/wiki/DLL_injection#Approaches_on_Microsoft_Windows
 * - http://www.codeproject.com/KB/threads/completeinject.aspx
 * - http://www.codeproject.com/KB/threads/winspy.aspx
 * - http://www.codeproject.com/KB/DLL/DLL_Injection_tutorial.aspx
 * - http://www.codeproject.com/KB/threads/APIHooking.aspx
 *
 * Other slightly different techniques:
 * - http://www.fr33project.org/pages/projects/phook.htm
 * - http://www.hbgary.com/loading-a-dll-without-calling-loadlibrary
 * - http://securityxploded.com/ntcreatethreadex.php
 */

#include <string>

#include <windows.h>
#include <stdio.h>

#include "inject.h"


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


int
main(int argc, char *argv[])
{

    if (argc < 3) {
        fprintf(stderr, "inject dllname.dll command [args] ...\n");
        return 1;
    }

    const char *szDll = argv[1];
#if !USE_SHARED_MEM
    SetEnvironmentVariableA("INJECT_DLL", szDll);
#else
    SetSharedMem(szDll);
#endif

    PROCESS_INFORMATION processInfo;
    HANDLE hProcess;
    BOOL bAttach;
    if (isdigit(argv[2][0])) {
        bAttach = TRUE;

        BOOL bRet;
        HANDLE hToken   = NULL;
        bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
        if (!bRet) {
            fprintf(stderr, "error: OpenProcessToken returned %u\n", (unsigned)bRet);
            return 1;
        }

        LUID Luid;
        bRet = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid);
        if (!bRet) {
            fprintf(stderr, "error: LookupPrivilegeValue returned %u\n", (unsigned)bRet);
            return 1;
        }

        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = Luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof tp, NULL, NULL);
        if (!bRet) {
            fprintf(stderr, "error: AdjustTokenPrivileges returned %u\n", (unsigned)bRet);
            return 1;
        }

        DWORD dwDesiredAccess =
            PROCESS_CREATE_THREAD |
            PROCESS_QUERY_INFORMATION |
            PROCESS_QUERY_LIMITED_INFORMATION |
            PROCESS_VM_OPERATION |
            PROCESS_VM_WRITE |
            PROCESS_VM_READ;
        DWORD dwProcessId = atol(argv[2]);
        hProcess = OpenProcess(
            dwDesiredAccess,
            FALSE /* bInheritHandle */,
            dwProcessId);
        if (!hProcess) {
            DWORD dwLastError = GetLastError();
            fprintf(stderr, "error: failed to open process %lu (%lu)\n", dwProcessId, dwLastError);
            return 1;
        }
    } else {
        bAttach = FALSE;
        std::string commandLine;
        char sep = 0;
        for (int i = 2; i < argc; ++i) {
            const char *arg = argv[i];

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
        memset(&startupInfo, 0, sizeof startupInfo);
        startupInfo.cb = sizeof startupInfo;

        // Create the process in suspended state
        if (!CreateProcessA(
               NULL,
               const_cast<char *>(commandLine.c_str()), // only modified by CreateProcessW
               0, // process attributes
               0, // thread attributes
               TRUE, // inherit handles
               CREATE_SUSPENDED,
               NULL, // environment
               NULL, // current directory
               &startupInfo,
               &processInfo)) {
            fprintf(stderr, "error: failed to execute %s\n", commandLine.c_str());
            return 1;
        }

        hProcess = processInfo.hProcess;
    }

    /*
     * XXX: Mixed architecture don't quite work.  See also
     * http://www.corsix.org/content/dll-injection-and-wow64
     */
    const char *szDllName;
    szDllName = "inject.dll";

    char szDllPath[MAX_PATH];
    GetModuleFileNameA(NULL, szDllPath, sizeof szDllPath);
    getDirName(szDllPath);
    strncat(szDllPath, szDllName, sizeof szDllPath - strlen(szDllPath) - 1);

    size_t szDllPathLength = strlen(szDllPath) + 1;

    // Allocate memory in the target process to hold the DLL name
    void *lpMemory = VirtualAllocEx(hProcess, NULL, szDllPathLength, MEM_COMMIT, PAGE_READWRITE);
    if (!lpMemory) {
        fprintf(stderr, "error: failed to allocate memory in the process\n");
        TerminateProcess(hProcess, 1);
        return 1;
    }

    // Copy DLL name into the target process
    if (!WriteProcessMemory(hProcess, lpMemory, szDllPath, szDllPathLength, NULL)) {
        fprintf(stderr, "error: failed to write into process memory\n");
        TerminateProcess(hProcess, 1);
        return 1;
    }

    /*
     * Get LoadLibraryA address from kernel32.dll.  It's the same for all the
     * process (XXX: but only within the same architecture).
     */
    PTHREAD_START_ROUTINE lpStartAddress =
        (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("KERNEL32"), "LoadLibraryA");

    // Create remote thread in another process
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, lpMemory, 0, NULL);
    if (!hThread) {
        fprintf(stderr, "error: failed to create remote thread\n");
        TerminateProcess(hProcess, 1);
        return 1;
    }

    // Wait for it to finish
    WaitForSingleObject(hThread, INFINITE);

    DWORD hModule = 0;
    GetExitCodeThread(hThread, &hModule);
    if (!hModule) {
        fprintf(stderr, "error: failed to inject %s\n", szDllPath);
        TerminateProcess(hProcess, 1);
        return 1;
    }

    if (bAttach) {
        return 0;
    }

    // Start main process thread
    ResumeThread(processInfo.hThread);

    // Wait for it to finish
    WaitForSingleObject(hProcess, INFINITE);

    DWORD exitCode = ~0;
    GetExitCodeProcess(hProcess, &exitCode);

    CloseHandle(hProcess);
    CloseHandle(processInfo.hThread);

    return (int)exitCode;

}

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
#include <stdio.h>
#include <assert.h>

#include <windows.h>
#include <psapi.h>
#include <dwmapi.h>
#include <tlhelp32.h>

#ifndef ERROR_ELEVATION_REQUIRED
#define ERROR_ELEVATION_REQUIRED 740
#endif

#include "os_version.hpp"
#include "devcon.hpp"
#include "inject.h"


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif


static void
debugPrintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
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


// http://msdn.microsoft.com/en-gb/library/windows/desktop/ms686335.aspx
static void
restartService(const char *lpServiceName)
{
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    assert(hSCManager);
    if (!hSCManager) {
        return;
    }

    SC_HANDLE hService = OpenServiceA(hSCManager, lpServiceName, SC_MANAGER_ALL_ACCESS);
    assert(hService);
    if (!hService) {
        return;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD cbBytesNeeded;
    QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, sizeof ssp, &cbBytesNeeded);

    BOOL bRet;
    if (ssp.dwCurrentState == SERVICE_RUNNING) {
        bRet = ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp);
        assert(bRet);
        while (ssp.dwCurrentState != SERVICE_STOPPED) {
            Sleep(ssp.dwWaitHint);
            QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, sizeof ssp, &cbBytesNeeded);
        }
        bRet = StartService(hService, 0, NULL);
        assert(bRet);
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
}


// Force DWM process to recreate all its Direct3D objects.
static void
restartDwmComposition(HANDLE hProcess)
{
    HRESULT hr;

    HMODULE hModule = LoadLibraryA("dwmapi");
    assert(hModule);
    if (!hModule) {
        return;
    }

    typedef HRESULT (WINAPI *PFNDWMISCOMPOSITIONENABLED)(BOOL *pfEnabled);
    PFNDWMISCOMPOSITIONENABLED pfnDwmIsCompositionEnabled = (PFNDWMISCOMPOSITIONENABLED)GetProcAddress(hModule, "DwmIsCompositionEnabled");
    assert(pfnDwmIsCompositionEnabled);
    if (!pfnDwmIsCompositionEnabled) {
        return;
    }

    typedef HRESULT (WINAPI *PFNDWMENABLECOMPOSITION)(UINT uCompositionAction);
    PFNDWMENABLECOMPOSITION pfnDwmEnableComposition = (PFNDWMENABLECOMPOSITION)GetProcAddress(hModule, "DwmEnableComposition");
    assert(pfnDwmEnableComposition);
    if (!pfnDwmEnableComposition) {
        return;
    }


    BOOL bIsWindows8OrGreater = IsWindows8OrGreater();
    if (bIsWindows8OrGreater) {
        // Windows 8 ignores DwmEnableComposition(DWM_EC_DISABLECOMPOSITION).
        // It is however possible to force DWM to restart by restarting the
        // display device via the devcon utility 
        devconEnable(DEVCON_CLASS_DISPLAY);
    } else {

        BOOL fEnabled = FALSE;
        hr = pfnDwmIsCompositionEnabled(&fEnabled);
        if (FAILED(hr) || !fEnabled) {
            return;
        }

        fprintf(stderr, "info: restarting DWM composition\n");

        hr = pfnDwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
        assert(SUCCEEDED(hr));
        if (FAILED(hr)) {
            return;
        }

        Sleep(1000/30);

        hr = pfnDwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
        assert(SUCCEEDED(hr));
        (void)hr;
    }

    fprintf(stderr, "Press any key when finished tracing\n");
    getchar();

    DWORD dwExitCode;
    if (GetExitCodeProcess(hProcess, &dwExitCode) &&
        dwExitCode != STILL_ACTIVE) {
        // DWM process has already terminated
        return;
    }

    fprintf(stderr, "info: restarting DWM process\n");
    if (bIsWindows8OrGreater) {
        // From Windows 8 onwards DWM no longer runs as a service.  We just
        // kill it and winlogon parent process will respawn it.
        if (!TerminateProcess(hProcess, 0)) {
            logLastError("failed to terminate DWM process");
        }
    } else {
        hr = pfnDwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
        assert(SUCCEEDED(hr));

        restartService("uxsms");
    }
}


BOOL
getProcessIdByName(const char *szProcessName, DWORD *pdwProcessID)
{
    BOOL bRet = FALSE;

    HANDLE hProcessSnap;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof pe32;
        if (Process32First(hProcessSnap, &pe32)) {
            do {
                if (stricmp(szProcessName, pe32.szExeFile) == 0) {
                    *pdwProcessID = pe32.th32ProcessID;
                    bRet = TRUE;
                    break;
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
        CloseHandle(hProcessSnap);
    }

    return bRet;
}


static bool
isNumber(const char *arg) {
    while (*arg) {
        if (!isdigit(*arg++)) {
            return false;
        }
    }
    return true;
}


int
main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr,
                "usage:\n"
                "  inject <dllname.dll> <command> [args] ...\n"
                "  inject <dllname.dll> <process-id>\n"
                "  inject <dllname.dll> !<process-name>\n"
        );
        return 1;
    }

    BOOL bAttach = FALSE;
    DWORD dwProcessId = ~0;
    if (isNumber(argv[2])) {
        dwProcessId = atol(argv[2]);
        bAttach = TRUE;
    } else if (argv[2][0] == '!') {
        const char *szProcessName = &argv[2][1];
        if (!getProcessIdByName(szProcessName, &dwProcessId)) {
            fprintf(stderr, "error: failed to find process %s\n", szProcessName);
            return 1;
        }
        bAttach = TRUE;
        fprintf(stderr, "dwProcessId = %lu\n", dwProcessId);
    }

    HANDLE hSemaphore = NULL;
    const char *szDll = argv[1];
    if (!USE_SHARED_MEM) {
        SetEnvironmentVariableA("INJECT_DLL", szDll);
    } else {
        hSemaphore = CreateSemaphore(NULL, 1, 1, "inject_semaphore");
        if (hSemaphore == NULL) {
            fprintf(stderr, "error: failed to create semaphore\n");
            return 1;
        }

        DWORD dwWait = WaitForSingleObject(hSemaphore, 0);
        if (dwWait == WAIT_TIMEOUT) {
            fprintf(stderr, "info: waiting for another inject instance to finish\n");
            dwWait = WaitForSingleObject(hSemaphore, INFINITE);
        }
        if (dwWait != WAIT_OBJECT_0) {
            fprintf(stderr, "error: failed to enter semaphore gate\n");
            return 1;
        }

        SetSharedMem(szDll);
    }

    BOOL bAttachDwm = FALSE;
    PROCESS_INFORMATION processInfo;
    HANDLE hProcess;
    if (bAttach) {
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
            PROCESS_VM_READ |
            PROCESS_TERMINATE;
        hProcess = OpenProcess(
            dwDesiredAccess,
            FALSE /* bInheritHandle */,
            dwProcessId);
        if (!hProcess) {
            logLastError("failed to open process");
            return 1;
        }

        char szProcess[MAX_PATH];
        DWORD dwRet = GetModuleFileNameEx(hProcess, 0, szProcess, sizeof szProcess);
        assert(dwRet);
        if (dwRet &&
            stricmp(getBaseName(szProcess), "dwm.exe") == 0) {
            bAttachDwm = TRUE;
        }
    } else {
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
            DWORD dwLastError = GetLastError();
            fprintf(stderr, "error: failed to execute %s (%lu)\n",
                    commandLine.c_str(), dwLastError);
            if (dwLastError == ERROR_ELEVATION_REQUIRED) {
                fprintf(stderr, "error: target program requires elevated priviledges and must be started from an Administrator Command Prompt, or UAC must be disabled\n");
            }
            return 1;
        }

        hProcess = processInfo.hProcess;
    }

    /*
     * XXX: Mixed architecture don't quite work.  See also
     * http://www.corsix.org/content/dll-injection-and-wow64
     */
    {
        typedef BOOL (WINAPI *PFNISWOW64PROCESS)(HANDLE, PBOOL);
        PFNISWOW64PROCESS pfnIsWow64Process;
        pfnIsWow64Process = (PFNISWOW64PROCESS)
            GetProcAddress(GetModuleHandleA("kernel32"), "IsWow64Process");
        if (pfnIsWow64Process) {
            BOOL isParentWow64 = FALSE;
            BOOL isChildWow64 = FALSE;
            if (pfnIsWow64Process(GetCurrentProcess(), &isParentWow64) &&
                pfnIsWow64Process(hProcess, &isChildWow64) &&
                isParentWow64 != isChildWow64) {
                fprintf(stderr, "error: binaries mismatch: you need to use the "
#ifdef _WIN64
                        "32-bits"
#else
                        "64-bits"
#endif
                        " apitrace binaries to trace this application\n");
                TerminateProcess(hProcess, 1);
                return 1;
            }
        }
    }

    if (bAttachDwm && IsWindows8OrGreater()) {
        // Switch to Microsoft Basic Display Driver before injecting, so that
        // we don't trace with it.
        devconDisable(DEVCON_CLASS_DISPLAY);
        Sleep(1000);
    }

    const char *szDllName;
    szDllName = "injectee.dll";

    char szDllPath[MAX_PATH];
    GetModuleFileNameA(NULL, szDllPath, sizeof szDllPath);
    getDirName(szDllPath);
    strncat(szDllPath, szDllName, sizeof szDllPath - strlen(szDllPath) - 1);

#if 1
    if (!injectDll(hProcess, szDllPath)) {
        TerminateProcess(hProcess, 1);
        return 1;
    }
#endif

    DWORD exitCode;

    if (bAttach) {
        if (bAttachDwm) {
            restartDwmComposition(hProcess);
        }

        exitCode = 0;
    } else {
        // Start main process thread
        ResumeThread(processInfo.hThread);

        // Wait for it to finish
        WaitForSingleObject(hProcess, INFINITE);

        if (pSharedMem && !pSharedMem->bReplaced) {
            fprintf(stderr, "warning: %s was never used: application probably does not use this API\n", szDll);
        }

        exitCode = ~0;
        GetExitCodeProcess(hProcess, &exitCode);

        CloseHandle(processInfo.hThread);
    }

    CloseHandle(hProcess);

    if (hSemaphore) {
        ReleaseSemaphore(hSemaphore, 1, NULL);
        CloseHandle(hSemaphore);
    }

    return (int)exitCode;
}

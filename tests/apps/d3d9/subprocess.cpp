/**************************************************************************
 *
 * Copyright 2016 VMware Inc.
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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <windows.h>

#include <d3d9.h>


static int
testCreateProcessA(void)
{
    char szProcessName[MAX_PATH];
    GetModuleFileNameA(nullptr, szProcessName, _countof(szProcessName));

    STARTUPINFOA startupInfo;
    ZeroMemory(&startupInfo, sizeof startupInfo);
    startupInfo.cb = sizeof startupInfo;

    PROCESS_INFORMATION processInformation;

    if (!CreateProcessA(nullptr,
                        szProcessName,
                        0, // process attributes
                        0, // thread attributes
                        FALSE, // inherit handles
                        0, // creation flags,
                        nullptr, // environment
                        nullptr, // current directory
                        &startupInfo,
                        &processInformation
                        )) {
        return EXIT_FAILURE;
    }

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    return EXIT_SUCCESS;
}


static int
testCreateProcessW(void)
{
    wchar_t szProcessName[MAX_PATH];
    GetModuleFileNameW(nullptr, szProcessName, _countof(szProcessName));

    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof startupInfo);
    startupInfo.cb = sizeof startupInfo;

    PROCESS_INFORMATION processInformation;

    if (!CreateProcessW(nullptr,
                        szProcessName,
                        0, // process attributes
                        0, // thread attributes
                        FALSE, // inherit handles
                        0, // creation flags,
                        nullptr, // environment
                        nullptr, // current directory
                        &startupInfo,
                        &processInformation
                        )) {
        return EXIT_FAILURE;
    }

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    return EXIT_SUCCESS;
}


static int
testCreateProcessAsUserW(void)
{
    wchar_t szProcessName[MAX_PATH];
    GetModuleFileNameW(nullptr, szProcessName, _countof(szProcessName));

    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof startupInfo);
    startupInfo.cb = sizeof startupInfo;

    PROCESS_INFORMATION processInformation;

    if (!CreateProcessAsUserW(nullptr,
                              nullptr,
                              szProcessName,
                              0, // process attributes
                              0, // thread attributes
                              FALSE, // inherit handles
                              0, // creation flags,
                              nullptr, // environment
                              nullptr, // current directory
                              &startupInfo,
                              &processInformation
                              )) {
        return EXIT_FAILURE;
    }

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    return EXIT_SUCCESS;
}


int
main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(stderr, "Child process.\n");
        fflush(stderr);

        D3DPERF_SetMarker(D3DCOLOR_XRGB(255, 255, 255), L"Child");
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Parent process.\n");
    fflush(stderr);

    char *testName = argv[1];
    if (stricmp(testName, "CreateProcessA") == 0) {
        return testCreateProcessA();
    }
    if (stricmp(testName, "CreateProcessW") == 0) {
        return testCreateProcessW();
    }
    if (stricmp(testName, "CreateProcessAsUserW") == 0) {
        return testCreateProcessAsUserW();
    }

    fprintf(stderr, "error: unsupported method `%s`\n", testName);

    return EXIT_FAILURE;
}


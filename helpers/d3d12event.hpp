/**************************************************************************
 *
 * Copyright 2020 Joshua Ashton for Valve Software
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * AUTHORS,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **************************************************************************/

#pragma once

#include <windows.h>
#include <set>
#include <mutex>
#include "detours.h"

namespace trace
{
    NTSTATUS fakeWaitForSingleObject(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER Timeout);
    NTSTATUS fakeWaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER Timeout);
}

extern std::mutex g_D3D12FenceEventSetMutex;
extern std::set<HANDLE> g_D3D12FenceEventSet;

#ifdef _WIN64

using _ZwWaitForSingleObject = NTSTATUS(NTAPI *)(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER Timeout);
static _ZwWaitForSingleObject TrueZwWaitForSingleObject = (_ZwWaitForSingleObject) GetProcAddress(GetModuleHandleA("ntdll"), "ZwWaitForSingleObject");

using _ZwWaitForMultipleObjects = NTSTATUS(NTAPI *)(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER Timeout);
static _ZwWaitForMultipleObjects TrueZwWaitForMultipleObjects = (_ZwWaitForMultipleObjects) GetProcAddress(GetModuleHandleA("ntdll"), "ZwWaitForMultipleObjects");

namespace D3D12EventHooks
{
    NTSTATUS D3D12ZwWaitForSingleObject(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER Timeout)
    {

        bool shouldFake = false;
        {
            std::unique_lock<std::mutex> lock{ g_D3D12FenceEventSetMutex };
            auto elem = g_D3D12FenceEventSet.find(hHandle);
            if (elem != g_D3D12FenceEventSet.end()) {
                shouldFake = true;
                g_D3D12FenceEventSet.erase(elem);
            }
        }

        if (shouldFake)
            return trace::fakeWaitForSingleObject(hHandle, bAlertable, Timeout);
        else
            return TrueZwWaitForSingleObject(hHandle, bAlertable, Timeout);
    }

    DWORD D3D12ZwWaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER Timeout)
    {
        HANDLE fakeHandles[MAXIMUM_WAIT_OBJECTS];
        assert(nCount < MAXIMUM_WAIT_OBJECTS);

        DWORD fakeCount = 0;
        {
            std::unique_lock<std::mutex> lock{ g_D3D12FenceEventSetMutex };
            for (DWORD i = 0; i < nCount; i++)
            {
                auto elem = g_D3D12FenceEventSet.find(lpHandles[i]);
                if (elem != g_D3D12FenceEventSet.end()) {
                    fakeHandles[fakeCount++] = lpHandles[i];
                    g_D3D12FenceEventSet.erase(elem);
                }
            }
        }

        if (fakeCount != 0)
            return trace::fakeWaitForMultipleObjects(nCount, lpHandles, fakeCount, fakeHandles, bWaitAny, bAlertable, Timeout);
        else
            return TrueZwWaitForMultipleObjects(nCount, lpHandles, bWaitAny, bAlertable, Timeout);
    }
}

static void _setup_event_hooking()
{
    LONG error = DetourTransactionBegin();
    if (error != NO_ERROR) {
        return;
    }
    DetourUpdateThread(GetCurrentThread());
    error = DetourAttach(reinterpret_cast<PVOID*>(&TrueZwWaitForSingleObject), reinterpret_cast<PVOID>(D3D12EventHooks::D3D12ZwWaitForSingleObject));
    if (error == NO_ERROR) {
        error = DetourAttach(reinterpret_cast<PVOID*>(&TrueZwWaitForMultipleObjects), reinterpret_cast<PVOID>(D3D12EventHooks::D3D12ZwWaitForMultipleObjects));
    }
    if (error == NO_ERROR) {
        DetourTransactionCommit();
    } else {
        DetourTransactionAbort();
    }
}

#else // !_WIN64

/*
 * On WOW64 the ntdll Zw* stubs dispatch through a WOW64 transition thunk
 * whose instructions Detours cannot safely relocate.  Hook one layer up
 * instead, at the plain (non-Ex) Win32 wait APIs, which have ordinary,
 * relocatable prologues.  This only catches the non-alertable
 * WaitForSingleObject/WaitForMultipleObjects calls that the D3D12
 * fence-wait pattern actually uses; *Ex variants go untraced.
 *
 * ZwWaitForSingleObject/ZwWaitForMultipleObjects's NTSTATUS results
 * (STATUS_WAIT_0, STATUS_ABANDONED_WAIT_0, STATUS_TIMEOUT) are numerically
 * identical to the Win32 WAIT_OBJECT_0/WAIT_ABANDONED/WAIT_TIMEOUT they're
 * built from, so they can be returned as-is without translation.
 */

using _WaitForSingleObject = DWORD(WINAPI *)(HANDLE hHandle, DWORD dwMilliseconds);
static _WaitForSingleObject TrueWaitForSingleObject = (_WaitForSingleObject) GetProcAddress(GetModuleHandleA("kernel32"), "WaitForSingleObject");

using _WaitForMultipleObjects = DWORD(WINAPI *)(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);
static _WaitForMultipleObjects TrueWaitForMultipleObjects = (_WaitForMultipleObjects) GetProcAddress(GetModuleHandleA("kernel32"), "WaitForMultipleObjects");

static inline PLARGE_INTEGER _millisecondsToRelativeTimeout(DWORD dwMilliseconds, LARGE_INTEGER& timeout)
{
    if (dwMilliseconds == INFINITE) {
        return nullptr;
    }
    timeout.QuadPart = -(static_cast<LONGLONG>(dwMilliseconds) * 10000);
    return &timeout;
}

namespace D3D12EventHooks
{
    DWORD WINAPI D3D12WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
    {
        bool shouldFake = false;
        {
            std::unique_lock<std::mutex> lock{ g_D3D12FenceEventSetMutex };
            auto elem = g_D3D12FenceEventSet.find(hHandle);
            if (elem != g_D3D12FenceEventSet.end()) {
                shouldFake = true;
                g_D3D12FenceEventSet.erase(elem);
            }
        }

        if (!shouldFake)
            return TrueWaitForSingleObject(hHandle, dwMilliseconds);

        LARGE_INTEGER timeout;
        PLARGE_INTEGER pTimeout = _millisecondsToRelativeTimeout(dwMilliseconds, timeout);
        return static_cast<DWORD>(trace::fakeWaitForSingleObject(hHandle, FALSE, pTimeout));
    }

    DWORD WINAPI D3D12WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
    {
        HANDLE fakeHandles[MAXIMUM_WAIT_OBJECTS];
        assert(nCount < MAXIMUM_WAIT_OBJECTS);

        DWORD fakeCount = 0;
        {
            std::unique_lock<std::mutex> lock{ g_D3D12FenceEventSetMutex };
            for (DWORD i = 0; i < nCount; i++)
            {
                auto elem = g_D3D12FenceEventSet.find(lpHandles[i]);
                if (elem != g_D3D12FenceEventSet.end()) {
                    fakeHandles[fakeCount++] = lpHandles[i];
                    g_D3D12FenceEventSet.erase(elem);
                }
            }
        }

        if (fakeCount == 0)
            return TrueWaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);

        LARGE_INTEGER timeout;
        PLARGE_INTEGER pTimeout = _millisecondsToRelativeTimeout(dwMilliseconds, timeout);
        return static_cast<DWORD>(trace::fakeWaitForMultipleObjects(nCount, lpHandles, fakeCount, fakeHandles, !bWaitAll, FALSE, pTimeout));
    }
}

static void _setup_event_hooking()
{
    LONG error = DetourTransactionBegin();
    if (error != NO_ERROR) {
        return;
    }
    DetourUpdateThread(GetCurrentThread());
    error = DetourAttach(reinterpret_cast<PVOID*>(&TrueWaitForSingleObject), reinterpret_cast<PVOID>(D3D12EventHooks::D3D12WaitForSingleObject));
    if (error == NO_ERROR) {
        error = DetourAttach(reinterpret_cast<PVOID*>(&TrueWaitForMultipleObjects), reinterpret_cast<PVOID>(D3D12EventHooks::D3D12WaitForMultipleObjects));
    }
    if (error == NO_ERROR) {
        DetourTransactionCommit();
    } else {
        DetourTransactionAbort();
    }
}

#endif // _WIN64

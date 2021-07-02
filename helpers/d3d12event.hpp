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
#include <map>
#include <mutex>
#include "mhook.h"

namespace trace
{
    NTSTATUS fakeZwWaitForSingleObject(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER Timeout);
    NTSTATUS fakeZwWaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER Timeout);
}

extern std::mutex g_D3D12FenceEventMapMutex;
extern std::map<HANDLE, HANDLE> g_D3D12FenceEventMap;

using _ZwWaitForSingleObject = NTSTATUS(*)(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER Timeout);
static _ZwWaitForSingleObject TrueZwWaitForSingleObject = (_ZwWaitForSingleObject) GetProcAddress(GetModuleHandleA("ntdll"), "ZwWaitForSingleObject");

using _ZwWaitForMultipleObjects = NTSTATUS(*)(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER Timeout);
static _ZwWaitForMultipleObjects TrueZwWaitForMultipleObjects = (_ZwWaitForMultipleObjects) GetProcAddress(GetModuleHandleA("ntdll"), "ZwWaitForMultipleObjects");

namespace D3D12EventHooks
{
    NTSTATUS D3D12ZwWaitForSingleObject(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER Timeout)
    {
        bool shouldFake = false;
        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);

            auto elem = g_D3D12FenceEventMap.find(hHandle);
            if (elem != g_D3D12FenceEventMap.end()) {
                shouldFake = true;
                g_D3D12FenceEventMap.erase(elem);
            }
        }

        if (shouldFake)
            return trace::fakeZwWaitForSingleObject(hHandle, bAlertable, Timeout);
        else
            return TrueZwWaitForSingleObject(hHandle, bAlertable, Timeout);
    }

    DWORD D3D12ZwWaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER Timeout)
    {
        HANDLE fakeHandles[MAXIMUM_WAIT_OBJECTS];

        DWORD fakeCount = 0;
        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);

            for (DWORD i = 0; i < nCount; i++)
            {
                auto elem = g_D3D12FenceEventMap.find(lpHandles[i]);
                if (elem != g_D3D12FenceEventMap.end()) {
                    fakeHandles[fakeCount] = lpHandles[i];
                    fakeCount++;
                    g_D3D12FenceEventMap.erase(elem);
                }
            }
        }

        if (fakeCount != 0)
            return trace::fakeZwWaitForMultipleObjects(nCount, lpHandles, bWaitAny, bAlertable, Timeout);
        else
            return TrueZwWaitForMultipleObjects(nCount, lpHandles, bWaitAny, bAlertable, Timeout);
    }
}

static void _setup_event_hooking()
{
    Mhook_SetHook(reinterpret_cast<PVOID*>(&TrueZwWaitForSingleObject), reinterpret_cast<PVOID>(D3D12EventHooks::D3D12ZwWaitForSingleObject));
    Mhook_SetHook(reinterpret_cast<PVOID*>(&TrueZwWaitForMultipleObjects), reinterpret_cast<PVOID>(D3D12EventHooks::D3D12ZwWaitForMultipleObjects));
}

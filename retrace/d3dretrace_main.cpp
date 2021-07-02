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


#include <string.h>

#include "os_string.hpp"

#include "d3dstate.hpp"
#include "retrace.hpp"
#include "d3dretrace.hpp"

#ifdef HAVE_DXGI
#include <mutex>

extern std::map<HANDLE, HANDLE> g_D3D12FenceEventMap;
extern std::mutex g_D3D12FenceEventMapMutex;

using _ZwWaitForSingleObject = NTSTATUS(*)(HANDLE hHandle, BOOL bAlertable, PLARGE_INTEGER pTimeout);
static _ZwWaitForSingleObject TrueZwWaitForSingleObject = (_ZwWaitForSingleObject) GetProcAddress(GetModuleHandleA("ntdll"), "ZwWaitForSingleObject");

using _ZwWaitForMultipleObjects = NTSTATUS(*)(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAny, BOOL bAlertable, PLARGE_INTEGER pTimeout);
static _ZwWaitForMultipleObjects TrueZwWaitForMultipleObjects = (_ZwWaitForMultipleObjects) GetProcAddress(GetModuleHandleA("ntdll"), "ZwWaitForMultipleObjects");

namespace d3dretrace {

    static void retrace_ZwWaitForSingleObject(trace::Call &call) {
        HANDLE         hHandle    = reinterpret_cast<HANDLE>(call.arg(0).toUInt());
        BOOL           bAlertable = static_cast     <BOOL>  (call.arg(1).toUInt());
        LONGLONG       TimeoutVal = call.arg(2).toArray() ? call.arg(2).toArray()->values[0]->toSInt() : 0;
        PLARGE_INTEGER pTimeout   = call.arg(2).toNull()  ? nullptr : reinterpret_cast<PLARGE_INTEGER>(&TimeoutVal);

        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);
            auto iter = g_D3D12FenceEventMap.find(hHandle);
            hHandle = iter->second;
            g_D3D12FenceEventMap.erase(iter);
        }
        TrueZwWaitForSingleObject(hHandle, bAlertable, pTimeout);
    }

    static void retrace_ZwWaitForMultipleObjects(trace::Call& call) {
        HANDLE Handles[MAXIMUM_WAIT_OBJECTS];

        DWORD   nCount          = static_cast<DWORD> (call.arg(0).toUInt());
        if (call.arg(1).toArray()) {
            for (DWORD i = 0; i < nCount; i++)
                Handles[i] = static_cast<HANDLE>(call.arg(1).toArray()->values[i]->toPointer());
        }
        BOOL    bWaitAny        = static_cast<BOOL> (call.arg(2).toUInt());
        BOOL    bAlertable      = static_cast<BOOL> (call.arg(3).toUInt());
        LONGLONG TimeoutVal     = call.arg(4).toArray() ? call.arg(4).toArray()->values[0]->toSInt() : 0;
        PLARGE_INTEGER pTimeout = call.arg(4).toNull()  ? nullptr : reinterpret_cast<PLARGE_INTEGER>(&TimeoutVal);

        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);
            for (DWORD i = 0; i < nCount; i++) {
                auto iter = g_D3D12FenceEventMap.find(Handles[i]);
                Handles[i] = iter->second;
                g_D3D12FenceEventMap.erase(iter);
            }
        }

        TrueZwWaitForMultipleObjects(nCount, Handles, bWaitAny, bAlertable, pTimeout);
    }

    const retrace::Entry event_callbacks[] = {
        {"ZwWaitForSingleObject", &retrace_ZwWaitForSingleObject},
        {"ZwWaitForMultipleObjects", &retrace_ZwWaitForMultipleObjects},
        {NULL, NULL}
    };
}
#endif

void
retrace::setFeatureLevel(const char *featureLevel) {
    /* TODO: Allow to override D3D feature level. */
}


void
retrace::setUp(void) {
}


void
retrace::addCallbacks(retrace::Retracer &retracer)
{
    retracer.addCallbacks(d3dretrace::ddraw_callbacks);
    retracer.addCallbacks(d3dretrace::d3d8_callbacks);
    retracer.addCallbacks(d3dretrace::d3d9_callbacks);
#ifdef HAVE_DXGI
    retracer.addCallbacks(d3dretrace::dxgi_callbacks);
    retracer.addCallbacks(d3dretrace::event_callbacks);
#endif
}


void
retrace::flushRendering(void) {
}

void
retrace::finishRendering(void) {
}

void
retrace::waitForInput(void) {
    /* TODO */
}

void
retrace::cleanUp(void) {
}

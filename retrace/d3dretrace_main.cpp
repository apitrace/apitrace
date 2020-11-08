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

namespace d3dretrace {

    static void retrace_WaitForSingleObject(trace::Call& call) {
        HANDLE hHandle        = reinterpret_cast<HANDLE>(call.arg(0).toUInt());
        DWORD  dwMilliseconds = static_cast     <DWORD> (call.arg(1).toUInt());

        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);
            auto iter = g_D3D12FenceEventMap.find(hHandle);
            hHandle = iter->second;
            g_D3D12FenceEventMap.erase(iter);
        }
        WaitForSingleObject(hHandle, dwMilliseconds);
    }

    static void retrace_WaitForSingleObjectEx(trace::Call &call) {
        HANDLE hHandle        = reinterpret_cast<HANDLE>(call.arg(0).toUInt());
        DWORD  dwMilliseconds = static_cast     <DWORD> (call.arg(1).toUInt());
        BOOL   bAlertable     = static_cast     <BOOL>  (call.arg(2).toUInt());

        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);
            auto iter = g_D3D12FenceEventMap.find(hHandle);
            hHandle = iter->second;
            g_D3D12FenceEventMap.erase(iter);
        }
        WaitForSingleObjectEx(hHandle, dwMilliseconds, bAlertable);
    }

    static void retrace_WaitForMultipleObjects(trace::Call& call) {
        retrace::ScopedAllocator allocator;

        DWORD   nCount         = static_cast<DWORD>           (call.arg(0).toUInt());
        HANDLE* lpHandles      = allocator.allocArray<HANDLE>(&call.arg(1));
        BOOL    bWaitAll       = static_cast<BOOL>            (call.arg(2).toUInt());
        DWORD   dwMilliseconds = static_cast<DWORD>           (call.arg(3).toUInt());

        {
            const trace::Array* traceHandles = (call.arg(2)).toArray();
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);
            for (size_t i = 0; i < traceHandles->values.size(); i++) {
                auto iter = g_D3D12FenceEventMap.find(traceHandles->values[i]);
                lpHandles[i] = iter->second;
                g_D3D12FenceEventMap.erase(iter);
            }
        }

        WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);
    }

    static void retrace_WaitForMultipleObjectsEx(trace::Call& call) {
        retrace::ScopedAllocator allocator;

        DWORD   nCount         = static_cast<DWORD>           (call.arg(0).toUInt());
        HANDLE* lpHandles      = allocator.allocArray<HANDLE>(&call.arg(1));
        BOOL    bWaitAll       = static_cast<BOOL>            (call.arg(2).toUInt());
        DWORD   dwMilliseconds = static_cast<DWORD>           (call.arg(3).toUInt());
        BOOL    bAlertable     = static_cast<BOOL>            (call.arg(4).toUInt());

        {
            const trace::Array* traceHandles = (call.arg(2)).toArray();
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);
            for (size_t i = 0; i < traceHandles->values.size(); i++) {
                auto iter = g_D3D12FenceEventMap.find(traceHandles->values[i]);
                lpHandles[i] = iter->second;
                g_D3D12FenceEventMap.erase(iter);
            }
        }

        WaitForMultipleObjectsEx(nCount, lpHandles, bWaitAll, dwMilliseconds, bAlertable);
    }

    const retrace::Entry event_callbacks[] = {
        {"WaitForSingleObject",   &retrace_WaitForSingleObject},
        {"WaitForSingleObjectEx", &retrace_WaitForSingleObjectEx},
        {"WaitForMultipleObjects", &retrace_WaitForMultipleObjects},
        {"WaitForMultipleObjectsEx", &retrace_WaitForMultipleObjectsEx},
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

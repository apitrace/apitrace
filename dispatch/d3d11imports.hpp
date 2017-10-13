/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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
 * Central place for all D3D11 includes, and respective OS dependent headers.
 */

#pragma once


#include <windows.h>

#include "winsdk_compat.h"

#include <d3d11_4.h>


/*
 * Windows 10.0.15063.0 SDK definitions, so we can build with 10.0.14393.0 a
 * bit longer, until AppVeyor includes something newer.
 */

#ifndef __ID3D11Fence_INTERFACE_DEFINED__
#define __ID3D11Fence_INTERFACE_DEFINED__
EXTERN_C const IID IID_ID3D11Fence;
struct ID3D11Fence: public ID3D11DeviceChild {
    virtual HRESULT STDMETHODCALLTYPE CreateSharedHandle(const SECURITY_ATTRIBUTES *, DWORD, LPCWSTR, HANDLE *) = 0;
    virtual UINT64 STDMETHODCALLTYPE GetCompletedValue(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEventOnCompletion(UINT64, HANDLE) = 0;
};
#endif

#ifndef __ID3D11DeviceContext4_INTERFACE_DEFINED__
#define __ID3D11DeviceContext4_INTERFACE_DEFINED__
EXTERN_C const IID IID_ID3D11DeviceContext4;
struct ID3D11DeviceContext4: public ID3D11DeviceContext3 {
    virtual HRESULT STDMETHODCALLTYPE Signal(ID3D11Fence *, UINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE Wait(ID3D11Fence *, UINT64) = 0;
};
#endif

#ifndef __ID3D11Device5_INTERFACE_DEFINED__
#define __ID3D11Device5_INTERFACE_DEFINED__
typedef enum D3D11_FENCE_FLAG {
    D3D11_FENCE_FLAG_NONE                 = 0x1,
    D3D11_FENCE_FLAG_SHARED               = 0x2,
    D3D11_FENCE_FLAG_SHARED_CROSS_ADAPTER = 0x4
} D3D11_FENCE_FLAG;
EXTERN_C const IID IID_ID3D11Device5;
struct ID3D11Device5: public ID3D11Device4 {
    virtual HRESULT STDMETHODCALLTYPE OpenSharedFence(HANDLE, REFIID, void **) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFence(UINT64, D3D11_FENCE_FLAG, REFIID, void **) = 0;
};
#endif


#include "dxgiint.h"

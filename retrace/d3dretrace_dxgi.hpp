/**************************************************************************
 *
 * Copyright 2012 VMware Inc
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

#pragma once


#include "dxgiint.h"

#include "retrace.hpp"
#include "d3dretrace.hpp"


/*
 * This module implements the IDXGIFactoryDWM and IDXGISwapChainDWM
 * undocumented interfaces used by DWM, in terms of the standard IDXGIFactory
 * and IDXGISwapChain interfaces, just for sake of d3dretrace.  Retracing on
 * top of the undocumented interfaces works, but it may interfere with running
 * DWM and causes corruption of the desktop upon exit.
 */


struct ID3D10Device;
struct ID3D11Device;
struct ID3D10DeviceChild;
struct ID3D11DeviceChild;


namespace d3dretrace {

HRESULT
createAdapter(IDXGIFactory *pFactory, REFIID riid, void **ppvAdapter);

static inline void
createWindowForSwapChain(DXGI_SWAP_CHAIN_DESC *pSwapChainDesc) {
    UINT Width  = pSwapChainDesc->BufferDesc.Width;
    UINT Height = pSwapChainDesc->BufferDesc.Height;
    if (!Width)  Width = 1024;
    if (!Height) Height = 768;
    if (retrace::forceWindowed &&
        pSwapChainDesc->BufferDesc.Format != DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) {
        pSwapChainDesc->Windowed = TRUE;
        pSwapChainDesc->Flags &= ~DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
    }
    if (pSwapChainDesc->OutputWindow) {
        pSwapChainDesc->OutputWindow = d3dretrace::createWindow(Width, Height);
    }
}


BOOL
overrideQueryInterface(IUnknown *pThis, REFIID riid, void **ppvObj, HRESULT *pResult);


HRESULT
createSharedResource(ID3D10Device *pDevice, REFIID ReturnedInterface, void **ppResource);

HRESULT
createSharedResource(ID3D11Device *pDevice, REFIID ReturnedInterface, void **ppResource);


inline void
deviceRemoved(trace::Call &call, IUnknown *pUnknown) {
    exit(EXIT_FAILURE);
}

void
deviceRemoved(trace::Call &call, ID3D11Device *pDevice);

void
deviceRemoved(trace::Call &call, ID3D10Device *pDevice);

void
deviceRemoved(trace::Call &call, ID3D11DeviceChild *pDevice);

void
deviceRemoved(trace::Call &call, ID3D10DeviceChild *pDevice);

void
deviceRemoved(trace::Call &call, IDXGISwapChain *pSwapChain);


} /* namespace d3dretrace */



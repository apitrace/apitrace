/**************************************************************************
 *
 * Copyright 2026 Broadcom
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


#include <d3dcommon.h>

#include "dxgiint.h"

#include "retrace.hpp"
#include "d3dretrace.hpp"


/*
 * This module implements d3d11on12 driver type, which replays a D3D11 trace
 * via D3D11On12 layer by intercepting and replacing D3D11CreateDevice with
 * D3D11On12CreateDevice.
 */


struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;


typedef HRESULT
(__stdcall *PFN_CREATEDXGIFACTORY1)(REFIID riid, void **ppFactory);
extern PFN_CREATEDXGIFACTORY1 _CreateDXGIFactory1;

namespace d3dretrace {
namespace d3d11on12 {

HRESULT
createDevice(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
             const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
             ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext);

HRESULT
createDeviceAndSwapChain(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
                         const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
                         const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain,
                         ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext);

void
fixupSwapChainDesc(DXGI_SWAP_EFFECT *pSwapEffect, UINT *pBufferCount, DXGI_FORMAT *pFormat = nullptr, DXGI_USAGE *pBufferUsage = nullptr, DXGI_SAMPLE_DESC *pSampleDesc = nullptr, UINT *pFlags = nullptr);

IDXGISwapChain *
wrapSwapChain(IDXGISwapChain *pSwapChain, IUnknown *pDevice, DXGI_USAGE bufferUsage, DXGI_SAMPLE_DESC sampleDesc);

IUnknown *
getCommandQueue(IUnknown *pDevice);

bool
isDevice(IUnknown *pDevice);

bool
getBackBufferFormat(ID3D11Resource *pResource, DXGI_FORMAT *pFormat);

bool
isBackBufferMultisampled(ID3D11Resource *pResource);

} /* namespace d3d11on12 */
} /* namespace d3dretrace */

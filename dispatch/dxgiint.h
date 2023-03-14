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


#pragma once

#include <windows.h>

#include "winsdk_compat.h"

#include <dxgi1_6.h>


DEFINE_GUID(IID_IDXGISwapChainDWM,0xF69F223B,0x45D3,0x4AA0,0x98,0xC8,0xC4,0x0C,0x2B,0x23,0x10,0x29);
struct IDXGISwapChainDWM: public IDXGIDeviceSubObject
{
    virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void **ppSurface) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput **ppOutput) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT *pLastPresentCount) = 0;
};

DEFINE_GUID(IID_IDXGIFactoryDWM,0x713F394E,0x92CA,0x47E7,0xAB,0x81,0x11,0x59,0xC2,0x79,0x1E,0x54);
struct IDXGIFactoryDWM: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChain) = 0;
};


DEFINE_GUID(IID_IWarpPrivateAPI,0xF13EBCD1,0x672C,0x4F8B,0xA6,0x31,0x95,0x39,0xCA,0x74,0x8D,0x71);


struct IUseCounted: public IUnknown
{
    virtual ULONG STDMETHODCALLTYPE UCAddUse(void) = 0;
    virtual ULONG STDMETHODCALLTYPE UCReleaseUse(void) = 0;
    virtual void STDMETHODCALLTYPE UCBreakCyclicReferences(void) = 0;
    virtual void STDMETHODCALLTYPE UCEstablishCyclicReferences(void) = 0;
    virtual void STDMETHODCALLTYPE UCDestroy(void) = 0;
};

struct ILayeredUseCounted: public IUseCounted
{
#if 0
    virtual HRESULT STDMETHODCALLTYPE LUCCompleteLayerConstruction(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCBeginLayerDestruction(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCDeferDeletion(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCDestroyStatus(E_LUC_Destroy eStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCGetInternalIdentity(void const *pLayeredChildArgs, UINT uiArgSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCGetInternalIdentity(void **ppInternalIdentity) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCGetChildType(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCCompleteInitialization(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCPrepareForDestruction(void) = 0;
#endif
};

struct ID3D11LayeredUseCounted: public ILayeredUseCounted
{
};

struct IUseCounted2: public IUseCounted
{
    virtual HRESULT STDMETHODCALLTYPE UCQueryInterface(UINT flags, REFIID riid, void **ppvObj) = 0;
};


struct ID3D10Buffer;

struct ID3D10DebugTest: public IUnknown
{
    virtual void STDMETHODCALLTYPE PreventFurtherExecutionOnError(BOOL Enable) = 0;
    virtual void STDMETHODCALLTYPE VSSetConstantBuffer14(ID3D10Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE GSSetConstantBuffer14(ID3D10Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE PSSetConstantBuffer14(ID3D10Buffer *pCB14) = 0;
};

struct ID3D11Buffer;

struct ID3D11DebugTest: public IUnknown
{
    virtual void STDMETHODCALLTYPE PreventFurtherExecutionOnError(BOOL Enable) = 0;
    virtual void STDMETHODCALLTYPE VSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE HSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE DSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE GSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE PSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE CSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
};


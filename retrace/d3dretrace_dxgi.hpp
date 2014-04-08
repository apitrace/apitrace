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

#ifndef _D3DRETRACE_DXGI_HPP_
#define _D3DRETRACE_DXGI_HPP_


#include "dxgiint.h"

#include "retrace.hpp"


/*
 * This module implements the IDXGIFactoryDWM and IDXGISwapChainDWM
 * undocumented interfaces used by DWM, in terms of the standard IDXGIFactory
 * and IDXGISwapChain interfaces, just for sake of d3dretrace.  Retracing on
 * top of the undocumented interfaces works, but it may interfere with running
 * DWM and causes corruption of the desktop upon exit.
 */


namespace d3dretrace {


class CDXGISwapChainDWM : public IDXGISwapChainDWM
{
private:
    IDXGISwapChain *m_pSwapChain;

public:
    CDXGISwapChainDWM(IDXGISwapChain *pSwapChain);
    ~CDXGISwapChainDWM();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObj);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void *pData);
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown);
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData);
    HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppParent);
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID riid, void **ppDevice);

    HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags);
    HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void **ppSurface);
    HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc);
    HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters);
    HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput **ppOutput);
    HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats);
    HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT *pLastPresentCount);
    HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, IDXGIOutput *pTarget);
    HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL *pFullscreen, IDXGIOutput **ppTarget);
};

CDXGISwapChainDWM::CDXGISwapChainDWM(IDXGISwapChain *pSwapChain) :
    m_pSwapChain(pSwapChain)
{
}

CDXGISwapChainDWM::~CDXGISwapChainDWM()
{
    m_pSwapChain->SetFullscreenState(FALSE, NULL);
    m_pSwapChain->Release();
}


HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::QueryInterface(REFIID riid, void **ppvObj)
{
    return m_pSwapChain->QueryInterface(riid, ppvObj);
}

ULONG STDMETHODCALLTYPE CDXGISwapChainDWM::AddRef(void)
{
    // FIXME
    return 1;
}

ULONG STDMETHODCALLTYPE CDXGISwapChainDWM::Release(void)
{
    // FIXME
    return 1;
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData)
{
    return m_pSwapChain->SetPrivateData(Name, DataSize, pData);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown)
{
    return m_pSwapChain->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData)
{
    return m_pSwapChain->GetPrivateData(Name, pDataSize, pData);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetParent(REFIID riid, void **ppParent)
{
    return m_pSwapChain->GetParent(riid, ppParent);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetDevice(REFIID riid, void **ppDevice)
{
    return m_pSwapChain->GetDevice(riid, ppDevice);
}


HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::Present(UINT SyncInterval, UINT Flags)
{
    return m_pSwapChain->Present(SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetBuffer(UINT Buffer, REFIID riid, void **ppSurface)
{
    return m_pSwapChain->GetBuffer(Buffer, riid, ppSurface);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc)
{
    return m_pSwapChain->GetDesc(pDesc);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    return m_pSwapChain->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters)
{
    return m_pSwapChain->ResizeTarget(pNewTargetParameters);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetContainingOutput(IDXGIOutput **ppOutput)
{
    return m_pSwapChain->GetContainingOutput(ppOutput);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats)
{
    return m_pSwapChain->GetFrameStatistics(pStats);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetLastPresentCount(UINT *pLastPresentCount)
{
    return m_pSwapChain->GetLastPresentCount(pLastPresentCount);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::SetFullscreenState(BOOL Fullscreen, IDXGIOutput *pTarget)
{
    return m_pSwapChain->SetFullscreenState(Fullscreen, pTarget);
}

HRESULT STDMETHODCALLTYPE CDXGISwapChainDWM::GetFullscreenState(BOOL *pFullscreen, IDXGIOutput **ppTarget)
{
    return m_pSwapChain->GetFullscreenState(pFullscreen, ppTarget);
}



class CDXGIFactoryDWM : public IDXGIFactoryDWM
{
private:
    IDXGIFactory *m_pFactory;

    ~CDXGIFactoryDWM();

public:
    CDXGIFactoryDWM(IDXGIFactory *pFactory);

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObj);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChain);
};

CDXGIFactoryDWM::CDXGIFactoryDWM(IDXGIFactory *pFactory) :
    m_pFactory(pFactory)
{}

CDXGIFactoryDWM::~CDXGIFactoryDWM()
{
    m_pFactory->Release();
}


HRESULT STDMETHODCALLTYPE CDXGIFactoryDWM::QueryInterface(REFIID riid, void **ppvObj)
{
    return m_pFactory->QueryInterface(riid, ppvObj);
}

ULONG STDMETHODCALLTYPE CDXGIFactoryDWM::AddRef(void)
{
    // FIXME
    return 1;
}

ULONG STDMETHODCALLTYPE CDXGIFactoryDWM::Release(void)
{
    // FIXME
    return 1;
}

HRESULT STDMETHODCALLTYPE CDXGIFactoryDWM::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChain)
{
    IDXGISwapChain *pSwapChain = NULL;
    if (retrace::forceWindowed) {
        pDesc->Windowed = TRUE;
    }
    HRESULT hr = m_pFactory->CreateSwapChain(pDevice, pDesc, &pSwapChain);
    if (SUCCEEDED(hr)) {
        if (!retrace::forceWindowed) {
            pSwapChain->SetFullscreenState(TRUE, pOutput);
        }
        *ppSwapChain = new CDXGISwapChainDWM(pSwapChain);
    }
    return hr;
}



} /* namespace d3dretrace */


#endif /* _D3DRETRACE_DXGI_HPP_ */

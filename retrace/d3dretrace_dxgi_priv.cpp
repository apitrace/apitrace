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


#ifdef HAVE_DXGI


#include "d3dretrace_dxgi.hpp"


/*
 * This module implements the IDXGIFactoryDWM and IDXGISwapChainDWM
 * undocumented interfaces used by DWM, in terms of the standard IDXGIFactory
 * and IDXGISwapChain interfaces, just for sake of d3dretrace.  Retracing on
 * top of the undocumented interfaces works, but it may interfere with running
 * DWM and causes corruption of the desktop upon exit.
 *
 * Note that we don't maintain our own reference counts in these
 * implementations, as there should only be one reference count for all
 * interfaces.  See http://msdn.microsoft.com/en-us/library/ms686590.aspx
 */


namespace d3dretrace {


static HRESULT __stdcall
GetInterface(IUnknown *pUnknown, void **ppvObj) {
    if (!ppvObj) {
        return E_POINTER;
    }
    *ppvObj = (LPVOID)pUnknown;
    pUnknown->AddRef();
    return S_OK;
}


class CDXGISwapChainDWM : public IDXGISwapChainDWM
{
protected:
    IDXGISwapChain *m_pSwapChain;
    IDXGIOutput *m_pOutput;

    virtual ~CDXGISwapChainDWM() {
        m_pSwapChain->SetFullscreenState(FALSE, NULL);
        m_pOutput->Release();
    }

public:
    CDXGISwapChainDWM(IDXGISwapChain *pSwapChain, IDXGIOutput *pOutput) :
        m_pSwapChain(pSwapChain),
        m_pOutput(pOutput)
    {
        m_pOutput->AddRef();
        if (!retrace::forceWindowed) {
            pSwapChain->SetFullscreenState(TRUE, pOutput);
        }
    }

    /*
     * IUnknown
     */

    HRESULT STDMETHODCALLTYPE
    QueryInterface(REFIID riid, void **ppvObj)
    {
        if (riid == IID_IDXGISwapChainDWM) {
            return GetInterface(this, ppvObj);
        }
        return m_pSwapChain->QueryInterface(riid, ppvObj);
    }

    ULONG STDMETHODCALLTYPE
    AddRef(void) {
        return m_pSwapChain->AddRef();
    }

    ULONG STDMETHODCALLTYPE
    Release(void) {
        ULONG cRef = m_pSwapChain->Release();
        if (cRef == 0) {
            delete this;
        }
        return cRef;
    }

    /*
     * IDXGIObject
     */

    HRESULT STDMETHODCALLTYPE
    SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
        return m_pSwapChain->SetPrivateData(Name, DataSize, pData);
    }

    HRESULT STDMETHODCALLTYPE
    SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
        return m_pSwapChain->SetPrivateDataInterface(Name, pUnknown);
    }

    HRESULT STDMETHODCALLTYPE
    GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
        return m_pSwapChain->GetPrivateData(Name, pDataSize, pData);
    }

    HRESULT STDMETHODCALLTYPE
    GetParent(REFIID riid, void **ppParent) {
        return m_pSwapChain->GetParent(riid, ppParent);
    }

    /*
     * IDXGIDeviceSubObject
     */

    HRESULT STDMETHODCALLTYPE
    GetDevice(REFIID riid, void **ppDevice) {
        return m_pSwapChain->GetDevice(riid, ppDevice);
    }

    /*
     * IDXGISwapChainDWM
     */

    HRESULT STDMETHODCALLTYPE
    Present(UINT SyncInterval, UINT Flags) {
        return m_pSwapChain->Present(SyncInterval, Flags);
    }

    HRESULT STDMETHODCALLTYPE
    GetBuffer(UINT Buffer, REFIID riid, void **ppSurface) {
        /* XXX: IDXGISwapChain buffers with indexes greater than zero can only
         * be read from, per
         * http://msdn.microsoft.com/en-gb/library/windows/desktop/bb174570.aspx,
         * but it appears that IDXGISwapChainDWM doesn't have that limitation.
         */
        return m_pSwapChain->GetBuffer(Buffer, riid, ppSurface);
    }

    HRESULT STDMETHODCALLTYPE
    GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc) {
        return m_pSwapChain->GetDesc(pDesc);
    }

    HRESULT STDMETHODCALLTYPE
    ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        return m_pSwapChain->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
    }

    HRESULT STDMETHODCALLTYPE
    ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters) {
        return m_pSwapChain->ResizeTarget(pNewTargetParameters);
    }

    HRESULT STDMETHODCALLTYPE
    GetContainingOutput(IDXGIOutput **ppOutput) {
        return GetInterface(m_pOutput, (void **) ppOutput);
    }

    HRESULT STDMETHODCALLTYPE
    GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats) {
        return m_pSwapChain->GetFrameStatistics(pStats);
    }

    HRESULT STDMETHODCALLTYPE
    GetLastPresentCount(UINT *pLastPresentCount) {
        return m_pSwapChain->GetLastPresentCount(pLastPresentCount);
    }
};


class CDXGIFactoryDWM : public IDXGIFactoryDWM
{
private:
    IDXGIFactory *m_pFactory;

    virtual ~CDXGIFactoryDWM() {
    }

public:
    CDXGIFactoryDWM(IDXGIFactory *pFactory) :
        m_pFactory(pFactory)
    {}

    /*
     * IUnknown
     */

    HRESULT STDMETHODCALLTYPE
    QueryInterface(REFIID riid, void **ppvObj)
    {
        if (riid == IID_IDXGIFactoryDWM) {
            return GetInterface(static_cast<IDXGIFactoryDWM *>(this), ppvObj);
        }
        return m_pFactory->QueryInterface(riid, ppvObj);
    }

    ULONG STDMETHODCALLTYPE
    AddRef(void) {
        return m_pFactory->AddRef();
    }

    ULONG STDMETHODCALLTYPE
    Release(void) {
        ULONG cRef = m_pFactory->Release();
        if (cRef == 0) {
            delete this;
        }
        return cRef;
    }

    /*
     * IDXGIFactoryDWM
     */

    HRESULT STDMETHODCALLTYPE
    CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChain)
    {
        assert(pOutput);
        IDXGISwapChain *pSwapChain = NULL;
        if (retrace::forceWindowed) {
            assert(pDesc->Windowed);
        }
        assert(!pDesc->OutputWindow);
        pDesc->OutputWindow = d3dretrace::createWindow(pDesc->BufferDesc.Width, pDesc->BufferDesc.Height);
        HRESULT hr = m_pFactory->CreateSwapChain(pDevice, pDesc, &pSwapChain);
        if (SUCCEEDED(hr)) {
            *ppSwapChain = new CDXGISwapChainDWM(pSwapChain, pOutput);
        }
        return hr;
    }
};


BOOL
overrideQueryInterface(IUnknown *pUnknown, REFIID riid, void **ppvObj, HRESULT *pResult)
{
    HRESULT hr;

    if (!ppvObj) {
        *pResult = E_POINTER;
        return TRUE;
    }

    if (riid == IID_IDXGIFactoryDWM) {
        IDXGIFactory *pFactory = NULL;
        hr = pUnknown->QueryInterface(IID_IDXGIFactory, (VOID **)&pFactory);
        if (SUCCEEDED(hr)) {
            *ppvObj = new d3dretrace::CDXGIFactoryDWM(pFactory);
            *pResult = S_OK;
            return TRUE;
        }
    }

    return FALSE;
}


} /* namespace d3dretrace */


#endif /* HAVE_DXGI */

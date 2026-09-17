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

#include "d3dretrace_dxgi.hpp"
#include "d3d11imports.hpp"
#include "d3dretrace.hpp"
#include <wrl/client.h>
#include <map>
#include <vector>

using Microsoft::WRL::ComPtr;

#ifdef RETRACE
#define CreateDXGIFactory1 _CreateDXGIFactory1
#endif /* RETRACE */


namespace d3dretrace {
namespace d3d11on12 {

static const GUID GUID_D3D11On12DeviceState =
    {0x85654fec,0x1087,0x40c2,{0x87,0xfb,0xcc,0xb9,0x5e,0xa4,0x3e,0x9f}};

static const GUID GUID_D3D11On12BackBufferFormat =
    {0x6f9b3c17,0x4e6b,0x4f0a,{0x9b,0x3f,0x2a,0x6a,0x0e,0x7c,0x1d,0x52}};

typedef HRESULT
(WINAPI *PFN_D3D12CreateDevice)(IUnknown *pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void **ppDevice);

typedef HRESULT
(WINAPI *PFN_D3D11On12CreateDevice)(IUnknown *pDevice, UINT Flags, const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels,
                                    IUnknown *const *ppCommandQueues, UINT NumQueues, UINT NodeMask,
                                    ID3D11Device **ppDevice, ID3D11DeviceContext **ppImmediateContext, D3D_FEATURE_LEVEL *pChosenFeatureLevel);

template< typename PFN >
static PFN
loadProcAddress(const char *szModule, const char *szProc)
{
    HMODULE hModule = LoadLibraryA(szModule);
    if (!hModule) {
        std::cerr << "error: failed to load " << szModule << " for --driver=d3d11on12\n";
        return nullptr;
    }
    PFN pfn = reinterpret_cast<PFN>(GetProcAddress(hModule, szProc));
    if (!pfn) {
        std::cerr << "error: failed to resolve " << szProc << " from " << szModule << " for --driver=d3d11on12\n";
    }
    return pfn;
}

static HRESULT
loadedD3D12CreateDevice(IUnknown *pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void **ppDevice)
{
    static PFN_D3D12CreateDevice pfn = loadProcAddress<PFN_D3D12CreateDevice>("d3d12.dll", "D3D12CreateDevice");
    if (!pfn) {
        return E_FAIL;
    }
    return pfn(pAdapter, MinimumFeatureLevel, riid, ppDevice);
}

static HRESULT
loadedD3D11On12CreateDevice(IUnknown *pDevice, UINT Flags, const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels,
                            IUnknown *const *ppCommandQueues, UINT NumQueues, UINT NodeMask,
                            ID3D11Device **ppDevice, ID3D11DeviceContext **ppImmediateContext, D3D_FEATURE_LEVEL *pChosenFeatureLevel)
{
    static PFN_D3D11On12CreateDevice pfn = loadProcAddress<PFN_D3D11On12CreateDevice>("d3d11.dll", "D3D11On12CreateDevice");
    if (!pfn) {
        return E_FAIL;
    }
    return pfn(pDevice, Flags, pFeatureLevels, FeatureLevels, ppCommandQueues, NumQueues, NodeMask,
               ppDevice, ppImmediateContext, pChosenFeatureLevel);
}


struct DeviceState {
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory> factory;
    ComPtr<ID3D12Device> d3d12Device;
    ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D11DeviceContext> immediateContext;
    ComPtr<ID3D12CommandAllocator> copyCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> copyCommandList;
    ComPtr<ID3D12Fence> copyFence;
    UINT64 copyFenceValue = 0;
    HANDLE copyFenceEvent = nullptr;
};


class IDeviceState : public IUnknown {
public:
    DeviceState state;

    HRESULT STDMETHODCALLTYPE
    QueryInterface(REFIID riid, void **ppvObj) override
    {
        if (!ppvObj) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown) {
            *ppvObj = static_cast<IUnknown *>(this);
            AddRef();
            return S_OK;
        }
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE
    AddRef(void) override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE
    Release(void) override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

private:
    LONG m_refCount = 1;
};


template< typename T >
static DeviceState *
getDeviceState(T *pObject, REFGUID guid)
{
    if (!pObject) {
        return nullptr;
    }
    IUnknown *pRaw = nullptr;
    ComPtr<IUnknown> pUnk;
    UINT size = sizeof(pRaw);
    if (FAILED(pObject->GetPrivateData(guid, &size, &pRaw)) || !pRaw) {
        return nullptr;
    }

    pUnk.Attach(pRaw);
    return &static_cast<IDeviceState *>(pRaw)->state;
}


/*
 * D3D11on12/D3D12 only creates flip-model swapchains which doesn't support
 * any _SRGB format. Attempting to forward _SRGB formats from original d3d11
 * traces to flip swapchains will fail with DXGI_ERROR_INVALID_CALL or
 * E_INVALIDARG. Thus we need to strip the _SRGB formats.
 */

static DXGI_FORMAT
stripSrgb(DXGI_FORMAT format)
{
    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    default:
        return format;
    }
}


/*
 * Typeless equivalent of a (non-SRGB, see stripSrgb) swapchain back buffer
 * format. Used so the wrapped back buffer resource (see GetBuffer) can
 * support views of either the UNORM or UNORM_SRGB member of the same format
 * family: unlike a real (non-wrapped) swapchain back buffer, a resource
 * obtained via ID3D11On12Device::CreateWrappedResource doesn't get that
 * casting leniency unless it's typeless to begin with.
 */
static DXGI_FORMAT
toTypelessFormat(DXGI_FORMAT format)
{
    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_TYPELESS;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_TYPELESS;
    default:
        return format;
    }
}


bool
getBackBufferFormat(ID3D11Resource *pResource, DXGI_FORMAT *pFormat)
{
    if (!pResource) {
        return false;
    }
    UINT size = sizeof *pFormat;
    return SUCCEEDED(pResource->GetPrivateData(GUID_D3D11On12BackBufferFormat, &size, pFormat));
}


bool
isBackBufferMultisampled(ID3D11Resource *pResource)
{
    ComPtr<ID3D11Texture2D> pTexture2D;
    if (!pResource || FAILED(pResource->QueryInterface(IID_PPV_ARGS(&pTexture2D)))) {
        return false;
    }
    D3D11_TEXTURE2D_DESC desc;
    pTexture2D->GetDesc(&desc);
    return desc.SampleDesc.Count > 1;
}


class CDXGISwapChainD3D11On12 : public IDXGISwapChain3
{
protected:
    IDXGISwapChain3 *m_pSwapChain;
    ID3D11Device *m_pDevice;
    LONG m_refCount = 1;
    /*
     * The app's originally-requested BufferUsage and SampleDesc. Neither is
     * necessarily what m_pSwapChain itself was actually created with, see
     * fixupSwapChainDesc().
     */
    DXGI_USAGE m_bufferUsage = 0;
    DXGI_SAMPLE_DESC m_sampleDesc = {1, 0};

    std::map<UINT, ComPtr<ID3D11Resource>> m_wrappedBuffers;
    std::map<UINT, ComPtr<ID3D12Resource>> m_privateBuffers;
    /*
     * Saved/restored around each Present's release/acquire of the wrapped
     * resources, since D3D11 auto-unbinds a resource still bound to the
     * pipeline when ReleaseWrappedResources is called on it.
     */
    UINT m_savedRTVCount = 0;
    ComPtr<ID3D11RenderTargetView> m_savedRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    ComPtr<ID3D11DepthStencilView> m_savedDSV;

    virtual ~CDXGISwapChainD3D11On12() {}

    DeviceState *
    deviceState()
    {
        return getDeviceState(m_pDevice, GUID_D3D11On12DeviceState);
    }

    /*
     * Release the wrapped D3D11 resources (required before Present or
     * ResizeBuffers, since they're actually D3D12 resources under the hood),
     * optionally saving the OM state that referenced them so it can be
     * restored afterwards.
     */
    bool
    releaseWrappedResources(DeviceState **ppDevState, bool saveOM)
    {
        DeviceState *pDevState = deviceState();
        if (!pDevState) {
            return false;
        }

        ComPtr<ID3D11On12Device> pDevice11On12;
        if (FAILED(m_pDevice->QueryInterface(IID_PPV_ARGS(&pDevice11On12)))) {
            return false;
        }

        if (saveOM) {
            ID3D11RenderTargetView *rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
            ID3D11DepthStencilView *dsv = nullptr;
            pDevState->immediateContext->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, &dsv);
            UINT rtvCount = 0;
            for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
                if (rtvs[i]) {
                    rtvCount = i + 1;
                }
            }
            m_savedRTVCount = rtvCount;
            for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
                m_savedRTVs[i].Attach(rtvs[i]);
            }
            m_savedDSV.Attach(dsv);
        }

        std::vector<ID3D11Resource *> resources;
        resources.reserve(m_wrappedBuffers.size());
        for (auto &kv : m_wrappedBuffers) {
            resources.push_back(kv.second.Get());
        }
        if (!resources.empty()) {
            pDevice11On12->ReleaseWrappedResources(resources.data(), (UINT)resources.size());
        }

        pDevState->immediateContext->Flush();

        *ppDevState = pDevState;
        return true;
    }

    void
    acquireWrappedResources()
    {
        DeviceState *pDevState = deviceState();
        if (!pDevState) {
            return;
        }

        ComPtr<ID3D11On12Device> pDevice11On12;
        if (FAILED(m_pDevice->QueryInterface(IID_PPV_ARGS(&pDevice11On12)))) {
            return;
        }

        std::vector<ID3D11Resource *> resources;
        resources.reserve(m_wrappedBuffers.size());
        for (auto &kv : m_wrappedBuffers) {
            resources.push_back(kv.second.Get());
        }
        if (!resources.empty()) {
            pDevice11On12->AcquireWrappedResources(resources.data(), (UINT)resources.size());
        }

        if (m_savedRTVCount > 0 || m_savedDSV) {
            ID3D11RenderTargetView *rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
            for (UINT i = 0; i < m_savedRTVCount; ++i) {
                rtvs[i] = m_savedRTVs[i].Get();
            }
            pDevState->immediateContext->OMSetRenderTargets(m_savedRTVCount, rtvs, m_savedDSV.Get());
        }
        for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
            m_savedRTVs[i].Reset();
        }
        m_savedDSV.Reset();
        m_savedRTVCount = 0;
    }

    /*
     * Blit the app's private, stable back buffer into whichever of the real
     * swapchain's rotating buffers is current, the same way D3D11On12 does
     * internally for genuine D3D11 swapchains. Must be called with the
     * private buffer not currently acquired by D3D11 (after
     * releaseWrappedResources), and before the real Present() call.
     */
    void
    blitPresentingBuffer(DeviceState &devState)
    {
        auto bufIt = m_privateBuffers.find(0);
        if (bufIt == m_privateBuffers.end()) {
            return;
        }
        ID3D12Resource *pPrivateBuffer = bufIt->second.Get();

        UINT index = m_pSwapChain->GetCurrentBackBufferIndex();

        ComPtr<ID3D12Resource> pRealBuffer;
        if (FAILED(m_pSwapChain->GetBuffer(index, IID_PPV_ARGS(&pRealBuffer)))) {
            return;
        }

        if (FAILED(devState.copyCommandAllocator->Reset())) {
            return;
        }
        if (FAILED(devState.copyCommandList->Reset(devState.copyCommandAllocator.Get(), nullptr))) {
            return;
        }

        /*
         * The private back buffer keeps the app's real (possibly
         * multisampled) SampleDesc, but the real swapchain is always
         * single-sampled (see fixupSwapChainDesc), so a multisampled
         * private buffer has to be resolved down into the real buffer
         * rather than just copied.
         */
        bool resolve = m_sampleDesc.Count > 1;
        D3D12_RESOURCE_STATES realStateAfter = resolve ? D3D12_RESOURCE_STATE_RESOLVE_DEST : D3D12_RESOURCE_STATE_COPY_DEST;
        D3D12_RESOURCE_STATES privateStateAfter = resolve ? D3D12_RESOURCE_STATE_RESOLVE_SOURCE : D3D12_RESOURCE_STATE_COPY_SOURCE;

        D3D12_RESOURCE_BARRIER barriers[2] = {};
        barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barriers[0].Transition.pResource = pRealBuffer.Get();
        barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barriers[0].Transition.StateAfter = realStateAfter;
        barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barriers[1].Transition.pResource = pPrivateBuffer;
        barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barriers[1].Transition.StateAfter = privateStateAfter;
        devState.copyCommandList->ResourceBarrier(2, barriers);

        if (resolve) {
            DXGI_SWAP_CHAIN_DESC desc;
            m_pSwapChain->GetDesc(&desc);
            devState.copyCommandList->ResolveSubresource(pRealBuffer.Get(), 0, pPrivateBuffer, 0, desc.BufferDesc.Format);
        } else {
            devState.copyCommandList->CopyResource(pRealBuffer.Get(), pPrivateBuffer);
        }

        std::swap(barriers[0].Transition.StateBefore, barriers[0].Transition.StateAfter);
        std::swap(barriers[1].Transition.StateBefore, barriers[1].Transition.StateAfter);
        devState.copyCommandList->ResourceBarrier(2, barriers);

        devState.copyCommandList->Close();

        ID3D12CommandList *lists[] = { devState.copyCommandList.Get() };
        devState.commandQueue->ExecuteCommandLists(1, lists);

        /*
         * Wait for the copy to complete before letting the real Present() proceed
         */
        devState.copyFenceValue++;
        if (SUCCEEDED(devState.commandQueue->Signal(devState.copyFence.Get(), devState.copyFenceValue)) &&
            devState.copyFence->GetCompletedValue() < devState.copyFenceValue) {
            devState.copyFence->SetEventOnCompletion(devState.copyFenceValue, devState.copyFenceEvent);
            WaitForSingleObject(devState.copyFenceEvent, INFINITE);
        }
    }

    void
    presentPrologue()
    {
        DeviceState *pDevState;
        if (!releaseWrappedResources(&pDevState, /*saveOM=*/true)) {
            return;
        }
        blitPresentingBuffer(*pDevState);
    }

    void
    presentEpilogue()
    {
        acquireWrappedResources();
    }

public:
    CDXGISwapChainD3D11On12(IDXGISwapChain3 *pSwapChain, ID3D11Device *pDevice, DXGI_USAGE bufferUsage, DXGI_SAMPLE_DESC sampleDesc) :
        m_pSwapChain(pSwapChain),
        m_pDevice(pDevice),
        m_bufferUsage(bufferUsage),
        m_sampleDesc(sampleDesc)
    {
        m_pSwapChain->AddRef();
    }

    /*
     * IUnknown
     */

    HRESULT STDMETHODCALLTYPE
    QueryInterface(REFIID riid, void **ppvObj) override
    {
        if (!ppvObj) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IDXGIObject ||
            riid == IID_IDXGIDeviceSubObject || riid == IID_IDXGISwapChain ||
            riid == IID_IDXGISwapChain1 || riid == IID_IDXGISwapChain2 ||
            riid == IID_IDXGISwapChain3) {
            *ppvObj = static_cast<IDXGISwapChain3 *>(this);
            AddRef();
            return S_OK;
        }
        return m_pSwapChain->QueryInterface(riid, ppvObj);
    }

    ULONG STDMETHODCALLTYPE
    AddRef(void) override
    {
        InterlockedIncrement(&m_refCount);
        return m_pSwapChain->AddRef();
    }

    ULONG STDMETHODCALLTYPE
    Release(void) override
    {
        ULONG cRef = m_pSwapChain->Release();
        if (InterlockedDecrement(&m_refCount) == 0) {
            m_pSwapChain->Release();
            delete this;
        }
        return cRef;
    }

    /*
     * IDXGIObject
     */

    HRESULT STDMETHODCALLTYPE
    SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) override
    {
        return m_pSwapChain->SetPrivateData(Name, DataSize, pData);
    }

    HRESULT STDMETHODCALLTYPE
    SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) override
    {
        return m_pSwapChain->SetPrivateDataInterface(Name, pUnknown);
    }

    HRESULT STDMETHODCALLTYPE
    GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) override
    {
        return m_pSwapChain->GetPrivateData(Name, pDataSize, pData);
    }

    HRESULT STDMETHODCALLTYPE
    GetParent(REFIID riid, void **ppParent) override
    {
        return m_pSwapChain->GetParent(riid, ppParent);
    }

    /*
     * IDXGIDeviceSubObject
     */

    HRESULT STDMETHODCALLTYPE
    GetDevice(REFIID riid, void **ppDevice) override
    {
        return m_pDevice->QueryInterface(riid, ppDevice);
    }

    /*
     * IDXGISwapChain
     */

    HRESULT STDMETHODCALLTYPE
    Present(UINT SyncInterval, UINT Flags) override
    {
        presentPrologue();
        HRESULT hr = m_pSwapChain->Present(SyncInterval, Flags);
        presentEpilogue();
        return hr;
    }

    HRESULT STDMETHODCALLTYPE
    GetBuffer(UINT Buffer, REFIID riid, void **ppSurface) override
    {
        /*
         * The app-facing buffer is our own private resource, not anything
         * obtained from the real swapchain, so it's stable and can just be
         * reused if the app calls GetBuffer(N) more than once.
         */
        auto wrapIt = m_wrappedBuffers.find(Buffer);
        if (wrapIt != m_wrappedBuffers.end()) {
            return wrapIt->second->QueryInterface(riid, ppSurface);
        }

        DXGI_SWAP_CHAIN_DESC desc;
        HRESULT hr = m_pSwapChain->GetDesc(&desc);
        if (FAILED(hr)) {
            return hr;
        }

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Width = desc.BufferDesc.Width;
        resourceDesc.Height = desc.BufferDesc.Height;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = toTypelessFormat(desc.BufferDesc.Format);
        resourceDesc.SampleDesc = m_sampleDesc;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        if (m_bufferUsage & DXGI_USAGE_UNORDERED_ACCESS) {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.BufferDesc.Format;

        DeviceState *pDevState = deviceState();
        if (!pDevState) {
            return E_FAIL;
        }

        ComPtr<ID3D12Resource> pPrivateResource;
        hr = pDevState->d3d12Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                                             D3D12_RESOURCE_STATE_PRESENT, &clearValue,
                                                             IID_PPV_ARGS(&pPrivateResource));
        if (FAILED(hr)) {
            std::cerr << "error: ID3D12Device::CreateCommittedResource failed for --driver=d3d11on12 back buffer "
                      << Buffer << " (0x" << std::hex << (unsigned long)hr << std::dec << ")\n";
            return hr;
        }

        ComPtr<ID3D11On12Device> pDevice11On12;
        hr = m_pDevice->QueryInterface(IID_PPV_ARGS(&pDevice11On12));
        if (FAILED(hr)) {
            return hr;
        }

        D3D11_RESOURCE_FLAGS resourceFlags = {};
        resourceFlags.BindFlags = D3D11_BIND_RENDER_TARGET;
        if (m_bufferUsage & DXGI_USAGE_SHADER_INPUT) {
            resourceFlags.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        }
        if (m_bufferUsage & DXGI_USAGE_UNORDERED_ACCESS) {
            resourceFlags.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }

        ComPtr<ID3D11Resource> pWrapped;
        hr = pDevice11On12->CreateWrappedResource(pPrivateResource.Get(), &resourceFlags,
                                                  D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT,
                                                  IID_PPV_ARGS(&pWrapped));
        if (FAILED(hr)) {
            std::cerr << "error: ID3D11On12Device::CreateWrappedResource failed for swapchain buffer "
                      << Buffer << " (0x" << std::hex << (unsigned long)hr << std::dec << ")\n";
            return hr;
        }

        hr = pWrapped->QueryInterface(riid, ppSurface);
        if (FAILED(hr)) {
            return hr;
        }

        DXGI_FORMAT bufferFormat = desc.BufferDesc.Format;
        pWrapped->SetPrivateData(GUID_D3D11On12BackBufferFormat, sizeof bufferFormat, &bufferFormat);

        m_privateBuffers[Buffer] = pPrivateResource;
        m_wrappedBuffers[Buffer] = pWrapped;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    SetFullscreenState(BOOL Fullscreen, IDXGIOutput *pTarget) override
    {
        return m_pSwapChain->SetFullscreenState(Fullscreen, pTarget);
    }

    HRESULT STDMETHODCALLTYPE
    GetFullscreenState(BOOL *pFullscreen, IDXGIOutput **ppTarget) override
    {
        return m_pSwapChain->GetFullscreenState(pFullscreen, ppTarget);
    }

    HRESULT STDMETHODCALLTYPE
    GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc) override
    {
        return m_pSwapChain->GetDesc(pDesc);
    }

    HRESULT STDMETHODCALLTYPE
    ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) override
    {
        DeviceState *pDevState;
        releaseWrappedResources(&pDevState, false);
        m_wrappedBuffers.clear();
        m_privateBuffers.clear();
        return m_pSwapChain->ResizeBuffers(BufferCount, Width, Height, stripSrgb(NewFormat), SwapChainFlags);
    }

    HRESULT STDMETHODCALLTYPE
    ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters) override
    {
        return m_pSwapChain->ResizeTarget(pNewTargetParameters);
    }

    HRESULT STDMETHODCALLTYPE
    GetContainingOutput(IDXGIOutput **ppOutput) override
    {
        return m_pSwapChain->GetContainingOutput(ppOutput);
    }

    HRESULT STDMETHODCALLTYPE
    GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats) override
    {
        return m_pSwapChain->GetFrameStatistics(pStats);
    }

    HRESULT STDMETHODCALLTYPE
    GetLastPresentCount(UINT *pLastPresentCount) override
    {
        return m_pSwapChain->GetLastPresentCount(pLastPresentCount);
    }

    /*
     * IDXGISwapChain1
     */

    HRESULT STDMETHODCALLTYPE
    GetDesc1(DXGI_SWAP_CHAIN_DESC1 *pDesc) override
    {
        return m_pSwapChain->GetDesc1(pDesc);
    }

    HRESULT STDMETHODCALLTYPE
    GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pDesc) override
    {
        return m_pSwapChain->GetFullscreenDesc(pDesc);
    }

    HRESULT STDMETHODCALLTYPE
    GetHwnd(HWND *pHwnd) override
    {
        return m_pSwapChain->GetHwnd(pHwnd);
    }

    HRESULT STDMETHODCALLTYPE
    GetCoreWindow(REFIID refiid, void **ppUnk) override
    {
        return m_pSwapChain->GetCoreWindow(refiid, ppUnk);
    }

    HRESULT STDMETHODCALLTYPE
    Present1(UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS *pPresentParameters) override
    {
        presentPrologue();
        HRESULT hr = m_pSwapChain->Present1(SyncInterval, PresentFlags, pPresentParameters);
        presentEpilogue();
        return hr;
    }

    BOOL STDMETHODCALLTYPE
    IsTemporaryMonoSupported(void) override
    {
        return m_pSwapChain->IsTemporaryMonoSupported();
    }

    HRESULT STDMETHODCALLTYPE
    GetRestrictToOutput(IDXGIOutput **ppRestrictToOutput) override
    {
        return m_pSwapChain->GetRestrictToOutput(ppRestrictToOutput);
    }

    HRESULT STDMETHODCALLTYPE
    SetBackgroundColor(const DXGI_RGBA *pColor) override
    {
        return m_pSwapChain->SetBackgroundColor(pColor);
    }

    HRESULT STDMETHODCALLTYPE
    GetBackgroundColor(DXGI_RGBA *pColor) override
    {
        return m_pSwapChain->GetBackgroundColor(pColor);
    }

    HRESULT STDMETHODCALLTYPE
    SetRotation(DXGI_MODE_ROTATION Rotation) override
    {
        return m_pSwapChain->SetRotation(Rotation);
    }

    HRESULT STDMETHODCALLTYPE
    GetRotation(DXGI_MODE_ROTATION *pRotation) override
    {
        return m_pSwapChain->GetRotation(pRotation);
    }

    /*
     * IDXGISwapChain2
     */

    HRESULT STDMETHODCALLTYPE
    SetSourceSize(UINT Width, UINT Height) override
    {
        return m_pSwapChain->SetSourceSize(Width, Height);
    }

    HRESULT STDMETHODCALLTYPE
    GetSourceSize(UINT *pWidth, UINT *pHeight) override
    {
        return m_pSwapChain->GetSourceSize(pWidth, pHeight);
    }

    HRESULT STDMETHODCALLTYPE
    SetMaximumFrameLatency(UINT MaxLatency) override
    {
        return m_pSwapChain->SetMaximumFrameLatency(MaxLatency);
    }

    HRESULT STDMETHODCALLTYPE
    GetMaximumFrameLatency(UINT *pMaxLatency) override
    {
        return m_pSwapChain->GetMaximumFrameLatency(pMaxLatency);
    }

    HANDLE STDMETHODCALLTYPE
    GetFrameLatencyWaitableObject(void) override
    {
        return m_pSwapChain->GetFrameLatencyWaitableObject();
    }

    HRESULT STDMETHODCALLTYPE
    SetMatrixTransform(const DXGI_MATRIX_3X2_F *pMatrix) override
    {
        return m_pSwapChain->SetMatrixTransform(pMatrix);
    }

    HRESULT STDMETHODCALLTYPE
    GetMatrixTransform(DXGI_MATRIX_3X2_F *pMatrix) override
    {
        return m_pSwapChain->GetMatrixTransform(pMatrix);
    }

    /*
     * IDXGISwapChain3
     */

    UINT STDMETHODCALLTYPE
    GetCurrentBackBufferIndex(void) override
    {
        return m_pSwapChain->GetCurrentBackBufferIndex();
    }

    HRESULT STDMETHODCALLTYPE
    CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE ColorSpace, UINT *pColorSpaceSupport) override
    {
        return m_pSwapChain->CheckColorSpaceSupport(ColorSpace, pColorSpaceSupport);
    }

    HRESULT STDMETHODCALLTYPE
    SetColorSpace1(DXGI_COLOR_SPACE_TYPE ColorSpace) override
    {
        return m_pSwapChain->SetColorSpace1(ColorSpace);
    }

    HRESULT STDMETHODCALLTYPE
    ResizeBuffers1(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags,
                  const UINT *pCreationNodeMask, IUnknown *const *ppPresentQueue) override
    {
        DeviceState *pDevState;
        releaseWrappedResources(&pDevState, /*saveOM=*/false);
        m_wrappedBuffers.clear();
        m_privateBuffers.clear();
        return m_pSwapChain->ResizeBuffers1(BufferCount, Width, Height, stripSrgb(Format), SwapChainFlags, pCreationNodeMask, ppPresentQueue);
    }
};


HRESULT
createDevice(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
             const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
             ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext)
{
    (void)DriverType;
    (void)Software;
    (void)SDKVersion;

    ComPtr<ID3D12Device> pD3D12Device;
    HRESULT hr = loadedD3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pD3D12Device));
    if (FAILED(hr)) {
        std::cerr << "error: D3D12CreateDevice failed for --driver=d3d11on12 (0x" << std::hex
                  << (unsigned long)hr << std::dec << ")\n";
        return hr;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (Flags & D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT) {
        queueDesc.Flags |= D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
    }
    ComPtr<ID3D12CommandQueue> pCommandQueue;
    hr = pD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue));
    if (FAILED(hr)) {
        std::cerr << "error: ID3D12Device::CreateCommandQueue failed for --driver=d3d11on12 (0x" << std::hex
                  << (unsigned long)hr << std::dec << ")\n";
        return hr;
    }

    IUnknown *ppCommandQueues[] = { pCommandQueue.Get() };

    ID3D11Device *pDevice11 = nullptr;
    ID3D11DeviceContext *pContext11 = nullptr;
    hr = loadedD3D11On12CreateDevice(pD3D12Device.Get(), Flags, pFeatureLevels, FeatureLevels,
                                     ppCommandQueues, 1, 1, &pDevice11, &pContext11, pFeatureLevel);
    if (FAILED(hr)) {
        std::cerr << "error: D3D11On12CreateDevice failed (0x" << std::hex << (unsigned long)hr << std::dec << ")\n";
        return hr;
    }

    IDeviceState *pIDeviceState = new IDeviceState();
    DeviceState &state = pIDeviceState->state;
    state.adapter = pAdapter;
    state.d3d12Device = pD3D12Device;
    state.commandQueue = pCommandQueue;
    state.immediateContext = pContext11;

    hr = pD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&state.copyCommandAllocator));
    if (SUCCEEDED(hr)) {
        hr = pD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, state.copyCommandAllocator.Get(),
                                             nullptr, IID_PPV_ARGS(&state.copyCommandList));
        if (SUCCEEDED(hr)) {
            state.copyCommandList->Close();
        }
    }
    if (SUCCEEDED(hr)) {
        hr = pD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&state.copyFence));
    }
    if (SUCCEEDED(hr)) {
        state.copyFenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    }
    if (FAILED(hr) || !state.copyFenceEvent) {
        std::cerr << "error: failed to set up the D3D11On12 present-time copy command list (0x"
                  << std::hex << (unsigned long)hr << std::dec << ")\n";
        pIDeviceState->Release();
        pDevice11->Release();
        if (pContext11) {
            pContext11->Release();
        }
        return FAILED(hr) ? hr : E_FAIL;
    }

    pDevice11->SetPrivateDataInterface(GUID_D3D11On12DeviceState, pIDeviceState);
    pIDeviceState->Release();

    if (ppDevice) {
        *ppDevice = pDevice11;
    } else if (pDevice11) {
        pDevice11->Release();
    }
    if (ppImmediateContext) {
        *ppImmediateContext = pContext11;
    } else if (pContext11) {
        pContext11->Release();
    }

    return hr;
}


HRESULT
createDeviceAndSwapChain(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
                         const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
                         const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain,
                         ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext)
{
    ID3D11Device *pDevice11 = nullptr;
    ID3D11DeviceContext *pContext11 = nullptr;
    HRESULT hr = createDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
                              SDKVersion, &pDevice11, pFeatureLevel, &pContext11);
    if (FAILED(hr)) {
        return hr;
    }

    if (pSwapChainDesc && ppSwapChain) {
        DeviceState *pDevState = getDeviceState(pDevice11, GUID_D3D11On12DeviceState);
        assert(pDevState);

        ComPtr<IDXGIFactory1> pFactory;
        hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
        if (SUCCEEDED(hr)) {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = *pSwapChainDesc;
            DXGI_USAGE appBufferUsage = swapChainDesc.BufferUsage;
            DXGI_SAMPLE_DESC appSampleDesc = swapChainDesc.SampleDesc;
            fixupSwapChainDesc(&swapChainDesc.SwapEffect, &swapChainDesc.BufferCount, &swapChainDesc.BufferDesc.Format, &swapChainDesc.BufferUsage, &swapChainDesc.SampleDesc, &swapChainDesc.Flags);
            IDXGISwapChain *pSwapChain = nullptr;
            hr = pFactory->CreateSwapChain(pDevState->commandQueue.Get(), &swapChainDesc, &pSwapChain);
            if (SUCCEEDED(hr)) {
                *ppSwapChain = wrapSwapChain(pSwapChain, pDevice11, appBufferUsage, appSampleDesc);
            }
        }
    }

    if (FAILED(hr)) {
        pDevice11->Release();
        if (pContext11) {
            pContext11->Release();
        }
        return hr;
    }

    if (ppDevice) {
        *ppDevice = pDevice11;
    } else {
        pDevice11->Release();
    }
    if (ppImmediateContext) {
        *ppImmediateContext = pContext11;
    } else if (pContext11) {
        pContext11->Release();
    }

    return hr;
}


void
fixupSwapChainDesc(DXGI_SWAP_EFFECT *pSwapEffect, UINT *pBufferCount, DXGI_FORMAT *pFormat, DXGI_USAGE *pBufferUsage, DXGI_SAMPLE_DESC *pSampleDesc, UINT *pFlags)
{
    if (*pSwapEffect == DXGI_SWAP_EFFECT_DISCARD || *pSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL) {
        /*
         * D3D12 only supports flip swap effects, so we need to update D3D11
         * traces' legacy bitblt swap effects to flip modes. Correspondingly,
         * we create two buffers and mimic the flip effects by rotating between
         * the two buffers on every Present(). Otherwise, these bitblit D3D11
         * traces will always display a stale image drawn by the first Present().
         */
        std::cerr << "warning: --driver=d3d11on12: updating bitblt swap effects to the flip modes which are required by D3D12\n";
        if (*pSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL) {
            *pSwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        } else {
            *pSwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        }

        if (pFormat) {
            *pFormat = stripSrgb(*pFormat);
        }

        if (pBufferUsage) {
            /*
             * D3D11on12 creates only flip swapchain which might not be compatible with all of the app's
             * original BufferUsage (e.g. DXGI_USAGE_UNORDERED_ACCESS causes DXGI_ERROR_INVALID_CALL).
             * Considering the created real swapchain (m_pSwapChain) only ever gets presented and blitted
             * into, we only need to make its BufferUsage DXGI_USAGE_RENDER_TARGET_OUTPUT.
             * Whatever usage bits the app asked for beyond that (UNORDERED_ACCESS, SHADER_INPUT) are
             * honored on the private back buffer (CDXGISwapChainD3D11On12::GetBuffer).
             */
            *pBufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        }

        if (pSampleDesc && pSampleDesc->Count > 1) {
            /*
             * Flip model swapchains can't be multisampled either, unlike legacy
             * bitblt ones. As with BufferUsage above, the real swapchain
             * (m_pSwapChain) needs to drop this. The app's original sample counts
             * are honored by the private back buffers. And
             * CDXGISwapChainD3D11On12::blitPresentingBuffer resolves it down
             * into the (single-sampled) real back buffer at Present time.
             */
            std::cerr << "warning: --driver=d3d11on12: downgrading multisampled swap chain to single-sample, which is required by the flip presentation model\n";
            pSampleDesc->Count = 1;
            pSampleDesc->Quality = 0;
        }

        if (pFlags && (*pFlags & DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE)) {
            /*
             * GDI-compatible swapchains are incompatible with the flip
             * presentation model too. Unlike BufferUsage/SampleDesc above,
             * there's no equivalent way to honor this on the private back
             * buffer. If the app actually calls IDXGISurface1::GetDC()
             * on it, that call will fail.
             */
            std::cerr << "warning: --driver=d3d11on12: dropping DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE, which is incompatible with the flip presentation model\n";
            *pFlags &= ~DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
        }
    }
    if (*pBufferCount < 2) {
        /*
         * The flip model requires at least 2 buffers.
         */
        *pBufferCount = 2;
    }
}


IDXGISwapChain *
wrapSwapChain(IDXGISwapChain *pSwapChain, IUnknown *pDevice, DXGI_USAGE bufferUsage, DXGI_SAMPLE_DESC sampleDesc)
{
    if (!pSwapChain) {
        return pSwapChain;
    }

    ComPtr<ID3D11Device> pDevice11;
    if (!pDevice || FAILED(pDevice->QueryInterface(IID_PPV_ARGS(&pDevice11))) ||
        !getDeviceState(pDevice11.Get(), GUID_D3D11On12DeviceState)) {
        /*
         * Not a device created via --driver=d3d11on12.
         */
        return pSwapChain;
    }

    ComPtr<IDXGISwapChain3> pSwapChain3;
    if (FAILED(pSwapChain->QueryInterface(IID_PPV_ARGS(&pSwapChain3)))) {
        std::cerr << "error: swapchain doesn't support IDXGISwapChain3, required for --driver=d3d11on12\n";
        return pSwapChain;
    }
    pSwapChain->Release();

    DeviceState *pDevState = getDeviceState(pDevice11.Get(), GUID_D3D11On12DeviceState);
    if (pDevState && !pDevState->factory) {
        pSwapChain3->GetParent(IID_PPV_ARGS(&pDevState->factory));
    }

    return new CDXGISwapChainD3D11On12(pSwapChain3.Detach(), pDevice11.Get(), bufferUsage, sampleDesc);
}


IUnknown *
getCommandQueue(IUnknown *pDevice)
{
    ComPtr<ID3D11Device> pDevice11;
    if (pDevice && SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&pDevice11)))) {
        DeviceState *pDevState = getDeviceState(pDevice11.Get(), GUID_D3D11On12DeviceState);
        if (pDevState) {
            return pDevState->commandQueue.Get();
        }
    }
    return pDevice;
}


bool
isDevice(IUnknown *pDevice)
{
    ComPtr<ID3D11Device> pDevice11;
    if (pDevice && SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&pDevice11)))) {
        return getDeviceState(pDevice11.Get(), GUID_D3D11On12DeviceState) != nullptr;
    }
    return false;
}


} /* namespace d3d11on12 */
} /* namespace d3dretrace */

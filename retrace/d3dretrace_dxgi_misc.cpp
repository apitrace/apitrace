/**************************************************************************
 *
 * Copyright 2015 VMware Inc
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

#include "d3d10imports.hpp"
#include "d3d11imports.hpp"
#include "d3dretrace.hpp"

#include "com_ptr.hpp"


namespace d3dretrace {


HRESULT
createAdapter(IDXGIFactory *pFactory, REFIID riid, void **ppvAdapter)
{
    assert(retrace::driver != retrace::DRIVER_DEFAULT);
    assert(pFactory != nullptr);
    assert(ppvAdapter != nullptr);

    DXGI_GPU_PREFERENCE GpuPreference = DXGI_GPU_PREFERENCE_UNSPECIFIED;
    const char *szSoftware = nullptr;
    switch (retrace::driver) {
    case retrace::DRIVER_INTEGRATED:
        GpuPreference = DXGI_GPU_PREFERENCE_MINIMUM_POWER;
        break;
    case retrace::DRIVER_DISCRETE:
        GpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
        break;
    case retrace::DRIVER_REFERENCE:
        szSoftware = "d3d11ref.dll";
        break;
    case retrace::DRIVER_SOFTWARE:
        szSoftware = "d3d10warp.dll";
        break;
    case retrace::DRIVER_MODULE:
        szSoftware = retrace::driverModule;
        break;
    case retrace::DRIVER_DEFAULT:
    default:
        break;
    }

    IDXGIAdapter *pAdapter = nullptr;
    HRESULT hr;

    if (szSoftware) {
        HMODULE hSoftware = nullptr;
        hSoftware = LoadLibraryA(szSoftware);
        if (!hSoftware) {
            std::cerr << "error: failed to load " << szSoftware << "\n";
            _exit(EXIT_FAILURE);
        }
        hr = pFactory->CreateSoftwareAdapter(hSoftware, &pAdapter);
    } else if (GpuPreference != DXGI_GPU_PREFERENCE_UNSPECIFIED) {
        com_ptr<IDXGIFactory6> pFactory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_IDXGIFactory6, (void **)&pFactory6))) {
            hr = pFactory6->EnumAdapterByGpuPreference(0, GpuPreference, IID_IDXGIAdapter1, (void **)&pAdapter);
        } else {
            hr = DXGI_ERROR_NOT_FOUND;
        }
    } else {
        hr = pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter **>(ppvAdapter));
    }

    if (SUCCEEDED(hr)) {
        DXGI_ADAPTER_DESC AdapterDesc;
        if (SUCCEEDED(pAdapter->GetDesc(&AdapterDesc))) {
            std::wcerr << L"info: using " << AdapterDesc.Description << std::endl;
        }
    }

    *ppvAdapter = pAdapter;

    return hr;
}


static const DWORD
Checker[8][8] = {
    {  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
    { ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U },
    {  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
    { ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U },
    {  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
    { ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U },
    {  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
    { ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U }
};


HRESULT
createSharedResource(ID3D10Device *pDevice, REFIID ReturnedInterface, void **ppResource)
{
    D3D10_TEXTURE2D_DESC Desc;
    memset(&Desc, 0, sizeof Desc);
    Desc.Width = 8;
    Desc.Height = 8;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;
    Desc.Usage = D3D10_USAGE_DEFAULT;
    Desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    Desc.CPUAccessFlags = 0x0;
    Desc.MiscFlags = 0 /* D3D10_RESOURCE_MISC_SHARED */;

    static const D3D10_SUBRESOURCE_DATA InitialData = {
        Checker, sizeof Checker[0], sizeof Checker
    };

    com_ptr<ID3D10Texture2D> pResource;
    HRESULT hr = pDevice->CreateTexture2D(&Desc, &InitialData, &pResource);
    if (SUCCEEDED(hr)) {
        hr = pResource->QueryInterface(ReturnedInterface, ppResource);
    }

    return hr;
}


HRESULT
createSharedResource(ID3D11Device *pDevice, REFIID ReturnedInterface, void **ppResource)
{
    D3D11_TEXTURE2D_DESC Desc;
    memset(&Desc, 0, sizeof Desc);
    Desc.Width = 8;
    Desc.Height = 8;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    Desc.CPUAccessFlags = 0x0;
    Desc.MiscFlags = 0 /* D3D11_RESOURCE_MISC_SHARED */;
    //Desc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    static const D3D11_SUBRESOURCE_DATA InitialData = {
        Checker, sizeof Checker[0], sizeof Checker
    };

    com_ptr<ID3D11Texture2D> pResource;
    HRESULT hr = pDevice->CreateTexture2D(&Desc, &InitialData, &pResource);
    if (SUCCEEDED(hr)) {
        hr = pResource->QueryInterface(ReturnedInterface, ppResource);
    }

    return hr;
}


void
deviceRemoved(trace::Call &call, ID3D11Device *pDevice)
{
    HRESULT hrReason = pDevice->GetDeviceRemovedReason();
    retrace::failed(call, hrReason);
    exit(EXIT_FAILURE);
}

void
deviceRemoved(trace::Call &call, ID3D10Device *pDevice)
{
    HRESULT hrReason = pDevice->GetDeviceRemovedReason();
    retrace::failed(call, hrReason);
    exit(EXIT_FAILURE);
}

void
deviceRemoved(trace::Call &call, ID3D11DeviceChild *pDeviceChild)
{
    com_ptr<ID3D11Device> pDevice;
    pDeviceChild->GetDevice(&pDevice);
    deviceRemoved(call, pDevice);
}

void
deviceRemoved(trace::Call &call, ID3D10DeviceChild *pDeviceChild)
{
    com_ptr<ID3D10Device> pDevice;
    pDeviceChild->GetDevice(&pDevice);
    deviceRemoved(call, pDevice);
}

void
deviceRemoved(trace::Call &call, IDXGISwapChain *pSwapChain)
{
    com_ptr<ID3D11Device> pDevice11;
    if (SUCCEEDED(pSwapChain->GetDevice(IID_ID3D11Device, (void **)&pDevice11))) {
        deviceRemoved(call, pDevice11);
        return;
    }

    com_ptr<ID3D10Device> pDevice10;
    if (SUCCEEDED(pSwapChain->GetDevice(IID_ID3D10Device, (void **)&pDevice10))) {
        deviceRemoved(call, pDevice10);
        return;
    }

    exit(EXIT_FAILURE);
}

} /* namespace d3dretrace */

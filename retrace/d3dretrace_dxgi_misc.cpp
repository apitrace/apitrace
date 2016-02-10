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

#include "com_ptr.hpp"


namespace d3dretrace {


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


} /* namespace d3dretrace */

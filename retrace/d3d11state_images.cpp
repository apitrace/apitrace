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


#include <assert.h>

#include <iostream>
#include <algorithm>

#include "image.hpp"
#include "d3d11imports.hpp"


namespace d3dstate {

static HRESULT
stageResource(ID3D11DeviceContext *pDeviceContext,
              ID3D11Resource *pResource,
              ID3D11Resource **ppStagingResource,
              UINT *pWidth, UINT *pHeight, UINT *pDepth) {
    D3D11_USAGE Usage = D3D11_USAGE_STAGING;
    UINT BindFlags = 0;
    UINT CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    UINT MiscFlags = 0;
    union {
         ID3D11Resource *pStagingResource;
         ID3D11Buffer *pStagingBuffer;
         ID3D11Texture1D *pStagingTexture1D;
         ID3D11Texture2D *pStagingTexture2D;
         ID3D11Texture3D *pStagingTexture3D;
    };
    HRESULT hr;

    ID3D11Device *pDevice = NULL;
    pDeviceContext->GetDevice(&pDevice);

    D3D11_RESOURCE_DIMENSION Type = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    switch (Type) {
    case D3D11_RESOURCE_DIMENSION_BUFFER:
        {
            D3D11_BUFFER_DESC Desc;
            static_cast<ID3D11Buffer *>(pResource)->GetDesc(&Desc);
            Desc.Usage = Usage;
            Desc.BindFlags = BindFlags;
            Desc.CPUAccessFlags = CPUAccessFlags;
            Desc.MiscFlags = MiscFlags;

            *pWidth = Desc.ByteWidth;
            *pHeight = 1;
            *pDepth = 1;

            hr = pDevice->CreateBuffer(&Desc, NULL, &pStagingBuffer);
        }
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        {
            D3D11_TEXTURE1D_DESC Desc;
            static_cast<ID3D11Texture1D *>(pResource)->GetDesc(&Desc);
            Desc.Usage = Usage;
            Desc.BindFlags = BindFlags;
            Desc.CPUAccessFlags = CPUAccessFlags;
            Desc.MiscFlags = MiscFlags;

            *pWidth = Desc.Width;
            *pHeight = 1;
            *pDepth = 1;

            hr = pDevice->CreateTexture1D(&Desc, NULL, &pStagingTexture1D);
        }
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        {
            D3D11_TEXTURE2D_DESC Desc;
            static_cast<ID3D11Texture2D *>(pResource)->GetDesc(&Desc);
            Desc.Usage = Usage;
            Desc.BindFlags = BindFlags;
            Desc.CPUAccessFlags = CPUAccessFlags;
            Desc.MiscFlags &= ~D3D11_RESOURCE_MISC_TEXTURECUBE;

            *pWidth = Desc.Width;
            *pHeight = Desc.Height;
            *pDepth = 1;

            hr = pDevice->CreateTexture2D(&Desc, NULL, &pStagingTexture2D);
        }
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        {
            D3D11_TEXTURE3D_DESC Desc;
            static_cast<ID3D11Texture3D *>(pResource)->GetDesc(&Desc);
            Desc.Usage = Usage;
            Desc.BindFlags = BindFlags;
            Desc.CPUAccessFlags = CPUAccessFlags;
            Desc.MiscFlags = MiscFlags;

            *pWidth = Desc.Width;
            *pHeight = Desc.Height;
            *pDepth = Desc.Depth;

            hr = pDevice->CreateTexture3D(&Desc, NULL, &pStagingTexture3D);
        }
        break;
    default:
        assert(0);
        hr = E_NOTIMPL;
        break;
    }

    if (SUCCEEDED(hr)) {
        *ppStagingResource = pStagingResource;
        pDeviceContext->CopyResource(pStagingResource, pResource);
    }
    
    pDevice->Release();

    return hr;
}

image::Image *
getRenderTargetImage(ID3D11DeviceContext *pDevice) {
    image::Image *image = NULL;
    ID3D11RenderTargetView *pRenderTargetView = NULL;
    D3D11_RENDER_TARGET_VIEW_DESC Desc;
    ID3D11Resource *pResource = NULL;
    ID3D11Resource *pStagingResource = NULL;
    UINT Width, Height, Depth;
    UINT MipSlice;
    UINT Subresource;
    D3D11_MAPPED_SUBRESOURCE MappedSubresource;
    HRESULT hr;
    const unsigned char *src;
    unsigned char *dst;

    pDevice->OMGetRenderTargets(1, &pRenderTargetView, NULL);
    if (!pRenderTargetView) {
        goto no_rendertarget;
    }

    pRenderTargetView->GetResource(&pResource);
    assert(pResource);

    pRenderTargetView->GetDesc(&Desc);
    if (Desc.Format != DXGI_FORMAT_R8G8B8A8_UNORM &&
        Desc.Format != DXGI_FORMAT_R32G32B32A32_FLOAT) {
        std::cerr << "warning: unsupported DXGI format " << Desc.Format << "\n";
        goto no_staging;
    }

    hr = stageResource(pDevice, pResource, &pStagingResource, &Width, &Height, &Depth);
    if (FAILED(hr)) {
        goto no_staging;
    }

    // TODO: Take the slice in consideration
    switch (Desc.ViewDimension) {
    case D3D11_RTV_DIMENSION_BUFFER:
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MipSlice;
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE3D:
        MipSlice = Desc.Texture3D.MipSlice;
        break;
    case D3D11_SRV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        goto no_map;
    }
    Subresource = MipSlice;

    Width  = std::max(Width  >> MipSlice, 1U);
    Height = std::max(Height >> MipSlice, 1U);
    Depth  = std::max(Depth  >> MipSlice, 1U);

    hr = pDevice->Map(pStagingResource, Subresource, D3D11_MAP_READ, 0, &MappedSubresource);
    if (FAILED(hr)) {
        goto no_map;
    }

    image = new image::Image(Width, Height, 4, true);
    if (!image) {
        goto no_image;
    }

    dst = image->start();
    src = (const unsigned char *)MappedSubresource.pData;
    for (unsigned y = 0; y < Height; ++y) {
        if (Desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM) {
            memcpy(dst, src, Width * 4);
        } else if (Desc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT) {
            float scale = 1.0f/255.0f;
            for (unsigned x = 0; x < Width; ++x) {
                dst[4*x + 0] = ((float *)src)[4*x + 0] * scale;
                dst[4*x + 1] = ((float *)src)[4*x + 1] * scale;
                dst[4*x + 2] = ((float *)src)[4*x + 2] * scale;
                dst[4*x + 3] = ((float *)src)[4*x + 3] * scale;
            }
        } else {
            assert(0);
        }
        src += MappedSubresource.RowPitch;
        dst += image->stride();
    }

no_image:
    pDevice->Unmap(pStagingResource, Subresource);
no_map:
    if (pStagingResource) {
        pStagingResource->Release();
    }
no_staging:
    if (pResource) {
        pResource->Release();
    }
    if (pRenderTargetView) {
        pRenderTargetView->Release();
    }
no_rendertarget:
    return image;
}


} /* namespace d3dstate */

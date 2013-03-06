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
#include "json.hpp"
#include "d3d10imports.hpp"
#include "d3dstate.hpp"
#include "dxgistate.hpp"


namespace d3dstate {

static HRESULT
stageResource(ID3D10Device *pDevice,
              ID3D10Resource *pResource,
              ID3D10Resource **ppStagingResource,
              UINT *pWidth, UINT *pHeight, UINT *pDepth) {
    D3D10_USAGE Usage = D3D10_USAGE_STAGING;
    UINT BindFlags = 0;
    UINT CPUAccessFlags = D3D10_CPU_ACCESS_READ;
    UINT MiscFlags = 0;
    union {
         ID3D10Resource *pStagingResource;
         ID3D10Buffer *pStagingBuffer;
         ID3D10Texture1D *pStagingTexture1D;
         ID3D10Texture2D *pStagingTexture2D;
         ID3D10Texture3D *pStagingTexture3D;
    };
    HRESULT hr;

    D3D10_RESOURCE_DIMENSION Type = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    switch (Type) {
    case D3D10_RESOURCE_DIMENSION_BUFFER:
        {
            D3D10_BUFFER_DESC Desc;
            static_cast<ID3D10Buffer *>(pResource)->GetDesc(&Desc);
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
    case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
        {
            D3D10_TEXTURE1D_DESC Desc;
            static_cast<ID3D10Texture1D *>(pResource)->GetDesc(&Desc);
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
    case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
        {
            D3D10_TEXTURE2D_DESC Desc;
            static_cast<ID3D10Texture2D *>(pResource)->GetDesc(&Desc);
            Desc.Usage = Usage;
            Desc.BindFlags = BindFlags;
            Desc.CPUAccessFlags = CPUAccessFlags;
            Desc.MiscFlags &= D3D10_RESOURCE_MISC_TEXTURECUBE;

            *pWidth = Desc.Width;
            *pHeight = Desc.Height;
            *pDepth = 1;

            hr = pDevice->CreateTexture2D(&Desc, NULL, &pStagingTexture2D);
        }
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
        {
            D3D10_TEXTURE3D_DESC Desc;
            static_cast<ID3D10Texture3D *>(pResource)->GetDesc(&Desc);
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
        pDevice->CopyResource(pStagingResource, pResource);
    }
    
    return hr;
}

static HRESULT
mapResource(ID3D10Resource *pResource,
            UINT Subresource, D3D10_MAP MapType, UINT MapFlags,
            D3D10_MAPPED_TEXTURE3D *pMappedSubresource) {
    D3D10_RESOURCE_DIMENSION Type = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    switch (Type) {
    case D3D10_RESOURCE_DIMENSION_BUFFER:
        assert(Subresource == 0);
        return static_cast<ID3D10Buffer *>(pResource)->Map(MapType, MapFlags, &pMappedSubresource->pData);
    case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
        return static_cast<ID3D10Texture1D *>(pResource)->Map(Subresource, MapType, MapFlags, &pMappedSubresource->pData);
    case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
        return static_cast<ID3D10Texture2D *>(pResource)->Map(Subresource, MapType, MapFlags, reinterpret_cast<D3D10_MAPPED_TEXTURE2D *>(pMappedSubresource));
    case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
        return static_cast<ID3D10Texture3D *>(pResource)->Map(Subresource, MapType, MapFlags, pMappedSubresource);
    default:
        assert(0);
        return E_NOTIMPL;
    }
}

static void
unmapResource(ID3D10Resource *pResource, UINT Subresource) {
    D3D10_RESOURCE_DIMENSION Type = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    switch (Type) {
    case D3D10_RESOURCE_DIMENSION_BUFFER:
        assert(Subresource == 0);
        static_cast<ID3D10Buffer *>(pResource)->Unmap();
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
        static_cast<ID3D10Texture1D *>(pResource)->Unmap(Subresource);
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
        static_cast<ID3D10Texture2D *>(pResource)->Unmap(Subresource);
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
        static_cast<ID3D10Texture3D *>(pResource)->Unmap(Subresource);
        break;
    default:
        assert(0);
    }
}

static image::Image *
getRenderTargetViewImage(ID3D10Device *pDevice,
                         ID3D10RenderTargetView *pRenderTargetView) {
    image::Image *image = NULL;
    D3D10_RENDER_TARGET_VIEW_DESC Desc;
    ID3D10Resource *pResource = NULL;
    ID3D10Resource *pStagingResource = NULL;
    UINT Width, Height, Depth;
    UINT MipSlice;
    UINT Subresource;
    D3D10_MAPPED_TEXTURE3D MappedSubresource;
    HRESULT hr;

    if (!pRenderTargetView) {
        return NULL;
    }

    pRenderTargetView->GetResource(&pResource);
    assert(pResource);

    pRenderTargetView->GetDesc(&Desc);

    hr = stageResource(pDevice, pResource, &pStagingResource, &Width, &Height, &Depth);
    if (FAILED(hr)) {
        goto no_staging;
    }

    // TODO: Take the slice in consideration
    switch (Desc.ViewDimension) {
    case D3D10_RTV_DIMENSION_BUFFER:
        MipSlice = 0;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MipSlice;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MipSlice;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MipSlice;
        MipSlice = 0;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MipSlice;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D10_RTV_DIMENSION_TEXTURE3D:
        MipSlice = Desc.Texture3D.MipSlice;
        break;
    case D3D10_SRV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        goto no_map;
    }
    Subresource = MipSlice;

    Width  = std::max(Width  >> MipSlice, 1U);
    Height = std::max(Height >> MipSlice, 1U);
    Depth  = std::max(Depth  >> MipSlice, 1U);

    hr = mapResource(pStagingResource, Subresource, D3D10_MAP_READ, 0, &MappedSubresource);
    if (FAILED(hr)) {
        goto no_map;
    }

    image = new image::Image(Width, Height, 4);
    if (!image) {
        goto no_image;
    }
    assert(image->stride() > 0);

    hr = ConvertFormat(Desc.Format,
                       MappedSubresource.pData,
                       MappedSubresource.RowPitch,
                       DXGI_FORMAT_R8G8B8A8_UNORM,
                       image->start(),
                       image->stride(),
                       Width, Height);
    if (FAILED(hr)) {
        delete image;
        image = NULL;
    }

no_image:
    unmapResource(pStagingResource, Subresource);
no_map:
    if (pStagingResource) {
        pStagingResource->Release();
    }
no_staging:
    if (pResource) {
        pResource->Release();
    }
    return image;
}


image::Image *
getRenderTargetImage(ID3D10Device *pDevice) {
    ID3D10RenderTargetView *pRenderTargetView = NULL;
    pDevice->OMGetRenderTargets(1, &pRenderTargetView, NULL);

    image::Image *image = NULL;
    if (pRenderTargetView) {
        image = getRenderTargetViewImage(pDevice, pRenderTargetView);
        pRenderTargetView->Release();
    }

    return image;
}


void
dumpFramebuffer(JSONWriter &json, ID3D10Device *pDevice)
{
    json.beginMember("framebuffer");
    json.beginObject();

    ID3D10RenderTargetView *pRenderTargetViews[D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT];
    pDevice->OMGetRenderTargets(D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT, pRenderTargetViews, NULL);

    for (UINT i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        if (!pRenderTargetViews[i]) {
            continue;
        }

        image::Image *image;
        image = getRenderTargetViewImage(pDevice, pRenderTargetViews[i]);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label, "RENDER_TARGET_%u", i);
            json.beginMember(label);
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // RENDER_TARGET_*
        }

        pRenderTargetViews[i]->Release();
    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */

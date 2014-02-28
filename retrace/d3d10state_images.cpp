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

#include "os.hpp"
#include "json.hpp"
#include "image.hpp"
#include "d3d10imports.hpp"
#include "d3dstate.hpp"
#include "dxgistate.hpp"


namespace d3dstate {

static HRESULT
stageResource(ID3D10Device *pDevice,
              ID3D10Resource *pResource,
              ID3D10Resource **ppStagingResource,
              UINT *pWidth, UINT *pHeight, UINT *pDepth,
              UINT *pMipLevels) {
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
            *pMipLevels = 1;

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
            *pMipLevels = Desc.MipLevels;

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
            *pMipLevels = Desc.MipLevels;

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
            *pMipLevels = Desc.MipLevels;

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
            UINT SubResource, D3D10_MAP MapType, UINT MapFlags,
            D3D10_MAPPED_TEXTURE3D *pMappedSubResource) {
    D3D10_RESOURCE_DIMENSION Type = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    switch (Type) {
    case D3D10_RESOURCE_DIMENSION_BUFFER:
        assert(SubResource == 0);
        return static_cast<ID3D10Buffer *>(pResource)->Map(MapType, MapFlags, &pMappedSubResource->pData);
    case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
        return static_cast<ID3D10Texture1D *>(pResource)->Map(SubResource, MapType, MapFlags, &pMappedSubResource->pData);
    case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
        return static_cast<ID3D10Texture2D *>(pResource)->Map(SubResource, MapType, MapFlags, reinterpret_cast<D3D10_MAPPED_TEXTURE2D *>(pMappedSubResource));
    case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
        return static_cast<ID3D10Texture3D *>(pResource)->Map(SubResource, MapType, MapFlags, pMappedSubResource);
    default:
        assert(0);
        return E_NOTIMPL;
    }
}

static void
unmapResource(ID3D10Resource *pResource, UINT SubResource) {
    D3D10_RESOURCE_DIMENSION Type = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    switch (Type) {
    case D3D10_RESOURCE_DIMENSION_BUFFER:
        assert(SubResource == 0);
        static_cast<ID3D10Buffer *>(pResource)->Unmap();
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
        static_cast<ID3D10Texture1D *>(pResource)->Unmap(SubResource);
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
        static_cast<ID3D10Texture2D *>(pResource)->Unmap(SubResource);
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
        static_cast<ID3D10Texture3D *>(pResource)->Unmap(SubResource);
        break;
    default:
        assert(0);
    }
}


image::Image *
getSubResourceImage(ID3D10Device *pDevice,
                    ID3D10Resource *pResource,
                    DXGI_FORMAT Format,
                    UINT ArraySlice,
                    UINT MipSlice)
{
    image::Image *image = NULL;
    ID3D10Resource *pStagingResource = NULL;
    UINT Width, Height, Depth;
    UINT MipLevels;
    UINT SubResource;
    D3D10_MAPPED_TEXTURE3D MappedSubResource;
    HRESULT hr;

    if (!pResource) {
        return NULL;
    }

    hr = stageResource(pDevice, pResource, &pStagingResource, &Width, &Height, &Depth, &MipLevels);
    if (FAILED(hr)) {
        goto no_staging;
    }

    SubResource = ArraySlice*MipLevels + MipSlice;

    Width  = std::max(Width  >> MipSlice, 1U);
    Height = std::max(Height >> MipSlice, 1U);
    Depth  = std::max(Depth  >> MipSlice, 1U);

    hr = mapResource(pStagingResource, SubResource, D3D10_MAP_READ, 0, &MappedSubResource);
    if (FAILED(hr)) {
        goto no_map;
    }

    image = ConvertImage(Format,
                         MappedSubResource.pData,
                         MappedSubResource.RowPitch,
                         Width, Height);

    unmapResource(pStagingResource, SubResource);
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


static void
dumpShaderResourceViewImage(JSONWriter &json,
                            ID3D10Device *pDevice,
                            ID3D10ShaderResourceView *pShaderResourceView,
                            const char *shader,
                            UINT stage) {
    D3D10_SHADER_RESOURCE_VIEW_DESC Desc;
    ID3D10Resource *pResource = NULL;

    if (!pShaderResourceView) {
        return;
    }

    pShaderResourceView->GetResource(&pResource);
    assert(pResource);

    pShaderResourceView->GetDesc(&Desc);

    UINT MipSlice = 0;
    UINT FirstArraySlice = 0;
    UINT ArraySize = 1;

    // TODO: Take the slice in consideration
    switch (Desc.ViewDimension) {
    case D3D10_SRV_DIMENSION_BUFFER:
        break;
    case D3D10_SRV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MostDetailedMip;
        break;
    case D3D10_SRV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MostDetailedMip;
        break;
    case D3D10_SRV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MostDetailedMip;
        break;
    case D3D10_SRV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MostDetailedMip;
        break;
    case D3D10_SRV_DIMENSION_TEXTURE2DMS:
        break;
    case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY:
        break;
    case D3D10_SRV_DIMENSION_TEXTURE3D:
        MipSlice = Desc.Texture3D.MostDetailedMip;
        break;
    case D3D10_SRV_DIMENSION_TEXTURECUBE:
        MipSlice = Desc.TextureCube.MostDetailedMip;
        ArraySize = 6;
        break;
    case D3D10_SRV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return;
    }

    for (UINT ArraySlice = FirstArraySlice; ArraySlice < FirstArraySlice + ArraySize; ++ArraySlice) {

        image::Image *image;
        image = getSubResourceImage(pDevice, pResource, Desc.Format, ArraySlice, MipSlice);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label,
                      "%s_RESOURCE_%u_ARRAY_%u_LEVEL_%u",
                      shader, stage, ArraySlice, MipSlice);
            json.beginMember(label);
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // *_RESOURCE_*
            delete image;
        }

    }
}


static image::Image *
getRenderTargetViewImage(ID3D10Device *pDevice,
                         ID3D10RenderTargetView *pRenderTargetView) {
    D3D10_RENDER_TARGET_VIEW_DESC Desc;
    ID3D10Resource *pResource = NULL;
    UINT MipSlice;

    if (!pRenderTargetView) {
        return NULL;
    }

    pRenderTargetView->GetResource(&pResource);
    assert(pResource);

    pRenderTargetView->GetDesc(&Desc);

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
    case D3D10_RTV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, 0, MipSlice);
}


static image::Image *
getDepthStencilViewImage(ID3D10Device *pDevice,
                         ID3D10DepthStencilView *pDepthStencilView) {
    D3D10_DEPTH_STENCIL_VIEW_DESC Desc;
    ID3D10Resource *pResource = NULL;
    UINT MipSlice;

    if (!pDepthStencilView) {
        return NULL;
    }

    pDepthStencilView->GetResource(&pResource);
    assert(pResource);

    pDepthStencilView->GetDesc(&Desc);

    // TODO: Take the slice in consideration
    switch (Desc.ViewDimension) {
    case D3D10_DSV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MipSlice;
        break;
    case D3D10_DSV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MipSlice;
        break;
    case D3D10_DSV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MipSlice;
        break;
    case D3D10_DSV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MipSlice;
        break;
    case D3D10_DSV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D10_DSV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, 0, MipSlice);
}


static void
dumpStageTextures(JSONWriter &json, ID3D10Device *pDevice, const char *stageName,
                  UINT NumViews,
                  ID3D10ShaderResourceView **ppShaderResourceViews)
{
    for (UINT i = 0; i < NumViews; ++i) {
        if (!ppShaderResourceViews[i]) {
            continue;
        }

        dumpShaderResourceViewImage(json, pDevice, ppShaderResourceViews[i], stageName, i);

        ppShaderResourceViews[i]->Release();
    }
}


void
dumpTextures(JSONWriter &json, ID3D10Device *pDevice)
{
    json.beginMember("textures");
    json.beginObject();

    ID3D10ShaderResourceView *pShaderResourceViews[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    pDevice->PSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);
    dumpStageTextures(json, pDevice, "PS", ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    pDevice->VSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);
    dumpStageTextures(json, pDevice, "VS", ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    pDevice->GSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);
    dumpStageTextures(json, pDevice, "GS", ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    json.endObject();
    json.endMember(); // textures
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
    ID3D10DepthStencilView *pDepthStencilView;
    pDevice->OMGetRenderTargets(ARRAYSIZE(pRenderTargetViews), pRenderTargetViews,
                                &pDepthStencilView);

    for (UINT i = 0; i < ARRAYSIZE(pRenderTargetViews); ++i) {
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
            delete image;
        }

        pRenderTargetViews[i]->Release();
    }

    if (pDepthStencilView) {
        image::Image *image;
        image = getDepthStencilViewImage(pDevice, pDepthStencilView);
        if (image) {
            json.beginMember("DEPTH_STENCIL");
            json.writeImage(image, "UNKNOWN");
            json.endMember();
            delete image;
        }

        pDepthStencilView->Release();

    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */

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
#include "d3d11imports.hpp"
#include "d3d10state.hpp"
#include "dxgistate.hpp"


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
            Desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;

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

static image::Image *
getSubResourceImage(ID3D11DeviceContext *pDevice,
                    ID3D11Resource *pResource,
                    DXGI_FORMAT Format,
                    UINT MipSlice)
{
    image::Image *image = NULL;
    ID3D11Resource *pStagingResource = NULL;
    UINT Width, Height, Depth;
    UINT SubResource = MipSlice;
    D3D11_MAPPED_SUBRESOURCE MappedSubResource;
    HRESULT hr;

    if (!pResource) {
        return NULL;
    }

    hr = stageResource(pDevice, pResource, &pStagingResource, &Width, &Height, &Depth);
    if (FAILED(hr)) {
        goto no_staging;
    }

    Width  = std::max(Width  >> MipSlice, 1U);
    Height = std::max(Height >> MipSlice, 1U);
    Depth  = std::max(Depth  >> MipSlice, 1U);

    hr = pDevice->Map(pStagingResource, SubResource, D3D11_MAP_READ, 0, &MappedSubResource);
    if (FAILED(hr)) {
        goto no_map;
    }

    image = ConvertImage(Format,
                         MappedSubResource.pData,
                         MappedSubResource.RowPitch,
                         Width, Height);

    pDevice->Unmap(pStagingResource, SubResource);
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


static image::Image *
getShaderResourceViewImage(ID3D11DeviceContext *pDevice,
                           ID3D11ShaderResourceView *pShaderResourceView) {
    D3D11_SHADER_RESOURCE_VIEW_DESC Desc;
    ID3D11Resource *pResource = NULL;
    UINT MipSlice;

    if (!pShaderResourceView) {
        return NULL;
    }

    pShaderResourceView->GetResource(&pResource);
    assert(pResource);

    pShaderResourceView->GetDesc(&Desc);

    // TODO: Take the slice in consideration
    switch (Desc.ViewDimension) {
    case D3D11_SRV_DIMENSION_BUFFER:
        MipSlice = 0;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MostDetailedMip;
        MipSlice = 0;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE3D:
        MipSlice = Desc.Texture3D.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURECUBE:
        MipSlice = Desc.TextureCube.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, MipSlice);
}


static image::Image *
getRenderTargetViewImage(ID3D11DeviceContext *pDevice,
                         ID3D11RenderTargetView *pRenderTargetView) {
    D3D11_RENDER_TARGET_VIEW_DESC Desc;
    ID3D11Resource *pResource = NULL;
    UINT MipSlice;

    if (!pRenderTargetView) {
        return NULL;
    }

    pRenderTargetView->GetResource(&pResource);
    assert(pResource);

    pRenderTargetView->GetDesc(&Desc);

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
    case D3D11_RTV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, MipSlice);
}


static image::Image *
getDepthStencilViewImage(ID3D11DeviceContext *pDevice,
                         ID3D11DepthStencilView *pDepthStencilView) {
    D3D11_DEPTH_STENCIL_VIEW_DESC Desc;
    ID3D11Resource *pResource = NULL;
    UINT MipSlice;

    if (!pDepthStencilView) {
        return NULL;
    }

    pDepthStencilView->GetResource(&pResource);
    assert(pResource);

    pDepthStencilView->GetDesc(&Desc);

    // TODO: Take the slice in consideration
    switch (Desc.ViewDimension) {
    case D3D11_DSV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MipSlice;
        MipSlice = 0;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D11_DSV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, MipSlice);
}


void
dumpTextures(JSONWriter &json, ID3D11DeviceContext *pDevice)
{
    json.beginMember("textures");
    json.beginObject();

    ID3D11ShaderResourceView *pShaderResourceViews[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
    pDevice->PSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    for (UINT i = 0; i < ARRAYSIZE(pShaderResourceViews); ++i) {
        if (!pShaderResourceViews[i]) {
            continue;
        }

        image::Image *image;
        image = getShaderResourceViewImage(pDevice, pShaderResourceViews[i]);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label, "PS_RESOURCE_%u", i);
            json.beginMember(label);
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // PS_RESOURCE_*
        }

        pShaderResourceViews[i]->Release();
    }

    json.endObject();
    json.endMember(); // textures
}


image::Image *
getRenderTargetImage(ID3D11DeviceContext *pDevice) {
    ID3D11RenderTargetView *pRenderTargetView = NULL;
    pDevice->OMGetRenderTargets(1, &pRenderTargetView, NULL);

    image::Image *image = NULL;
    if (pRenderTargetView) {
        image = getRenderTargetViewImage(pDevice, pRenderTargetView);
        pRenderTargetView->Release();
    }

    return image;
}


void
dumpFramebuffer(JSONWriter &json, ID3D11DeviceContext *pDevice)
{
    json.beginMember("framebuffer");
    json.beginObject();

    ID3D11RenderTargetView *pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    ID3D11DepthStencilView *pDepthStencilView;
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
        }

        pDepthStencilView->Release();

    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */

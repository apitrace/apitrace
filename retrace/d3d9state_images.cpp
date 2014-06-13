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
#include <stdint.h>
#include <stdio.h>

#include "image.hpp"
#include "json.hpp"
#include "com_ptr.hpp"
#include "d3d9imports.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


static image::Image *
getSurfaceImage(IDirect3DDevice9 *pDevice,
                IDirect3DSurface9 *pSurface) {
    image::Image *image = NULL;
    const unsigned char *src;
    unsigned char *dst;
    HRESULT hr;

    if (!pSurface) {
        return NULL;
    }

    D3DSURFACE_DESC Desc;
    hr = pSurface->GetDesc(&Desc);
    assert(SUCCEEDED(hr));

    unsigned numChannels;
    image::ChannelType channelType;
    switch (Desc.Format) {
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_R5G6B5:
        numChannels = 3;
        channelType = image::TYPE_UNORM8;
        break;
    case D3DFMT_D16:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D32F_LOCKABLE:
        numChannels = 1;
        channelType = image::TYPE_FLOAT;
        break;
    default:
        std::cerr << "warning: unsupported D3DFORMAT " << Desc.Format << "\n";
        return NULL;
    }

    D3DLOCKED_RECT LockedRect;
    hr = pSurface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
    if (FAILED(hr)) {
        return NULL;
    }

    image = new image::Image(Desc.Width, Desc.Height, numChannels, true, channelType);
    if (!image) {
        goto no_image;
    }

    dst = image->start();
    src = (const unsigned char *)LockedRect.pBits;
    for (unsigned y = 0; y < Desc.Height; ++y) {
        switch (Desc.Format) {
        case D3DFMT_R5G6B5:
            for (unsigned x = 0; x < Desc.Width; ++x) {
                uint32_t pixel = ((const uint16_t *)src)[x];
                dst[3*x + 0] = (( pixel        & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
                dst[3*x + 1] = (((pixel >>  5) & 0x3f) * (2*0xff) + 0x3f) / (2*0x3f);
                dst[3*x + 2] = (( pixel >> 11        ) * (2*0xff) + 0x1f) / (2*0x1f);
            }
            break;
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:
            for (unsigned x = 0; x < Desc.Width; ++x) {
                dst[3*x + 0] = src[4*x + 2];
                dst[3*x + 1] = src[4*x + 1];
                dst[3*x + 2] = src[4*x + 0];
            }
            break;
        case D3DFMT_D16:
        case D3DFMT_D16_LOCKABLE:
            for (unsigned x = 0; x < Desc.Width; ++x) {
                ((float *)dst)[x] = ((const uint16_t *)src)[x] * (1.0f / 0xffff);
            }
            break;
        case D3DFMT_D32F_LOCKABLE:
            memcpy(dst, src, Desc.Width * sizeof(float));
            break;
        default:
            assert(0);
            break;
        }

        src += LockedRect.Pitch;
        dst += image->stride();
    }

no_image:
    pSurface->UnlockRect();
    return image;
}


static image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice,
                     IDirect3DSurface9 *pRenderTarget) {
    HRESULT hr;

    if (!pRenderTarget) {
        return NULL;
    }

    D3DSURFACE_DESC Desc;
    hr = pRenderTarget->GetDesc(&Desc);
    assert(SUCCEEDED(hr));

    com_ptr<IDirect3DSurface9> pStagingSurface;
    hr = pDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, D3DPOOL_SYSTEMMEM, &pStagingSurface, NULL);
    if (FAILED(hr)) {
        return NULL;
    }

    hr = pDevice->GetRenderTargetData(pRenderTarget, pStagingSurface);
    if (FAILED(hr)) {
        std::cerr << "warning: GetRenderTargetData failed\n";
        return NULL;
    }

    return getSurfaceImage(pDevice, pStagingSurface);
}


image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice) {
    HRESULT hr;

    com_ptr<IDirect3DSurface9> pRenderTarget;
    hr = pDevice->GetRenderTarget(0, &pRenderTarget);
    if (FAILED(hr)) {
        return NULL;
    }
    assert(pRenderTarget);

    return getRenderTargetImage(pDevice, pRenderTarget);
}


static image::Image *
getTextureImage(IDirect3DDevice9 *pDevice,
                IDirect3DBaseTexture9 *pTexture,
                D3DCUBEMAP_FACES FaceType,
                UINT Level)
{
    image::Image *image = NULL;
    HRESULT hr;

    if (!pTexture) {
        return NULL;
    }

    com_ptr<IDirect3DSurface9> pSurface;

    D3DRESOURCETYPE Type = pTexture->GetType();
    switch (Type) {
    case D3DRTYPE_TEXTURE:
        assert(FaceType == D3DCUBEMAP_FACE_POSITIVE_X);
        hr = reinterpret_cast<IDirect3DTexture9 *>(pTexture)->GetSurfaceLevel(Level, &pSurface);
        break;
    case D3DRTYPE_CUBETEXTURE:
        hr = reinterpret_cast<IDirect3DCubeTexture9 *>(pTexture)->GetCubeMapSurface(FaceType, Level, &pSurface);
        break;
    default:
        /* TODO: support volume textures */
        return NULL;
    }
    if (FAILED(hr) || !pSurface) {
        return NULL;
    }

    D3DSURFACE_DESC Desc;
    hr = pSurface->GetDesc(&Desc);
    assert(SUCCEEDED(hr));

    if (Desc.Pool == D3DPOOL_DEFAULT) {
        com_ptr<IDirect3DSurface9> pRenderTarget;
        hr = pDevice->CreateRenderTarget(Desc.Width, Desc.Height, Desc.Format,
                                         D3DMULTISAMPLE_NONE, 0, FALSE,
                                         &pRenderTarget, NULL);
        if (FAILED(hr)) {
            std::cerr << "warning: CreateRenderTarget failed\n";
        } else {
            hr = pDevice->StretchRect(pSurface, NULL, pRenderTarget, NULL, D3DTEXF_NONE);
            if (FAILED(hr)) {
                std::cerr << "warning: StretchRect failed\n";
            } else {
                image = getRenderTargetImage(pDevice, pRenderTarget);
            }
        }
    } else {
        image = getSurfaceImage(pDevice, pSurface);
    }

    return image;
}

void
dumpTextures(JSONWriter &json, IDirect3DDevice9 *pDevice)
{
    HRESULT hr;

    json.beginMember("textures");
    json.beginObject();

    for (DWORD Stage = 0; Stage < 16; ++Stage) {
        com_ptr<IDirect3DBaseTexture9> pTexture;
        hr = pDevice->GetTexture(Stage, &pTexture);
        if (FAILED(hr)) {
            continue;
        }

        if (!pTexture) {
            continue;
        }

        D3DRESOURCETYPE Type = pTexture->GetType();

        DWORD NumFaces = Type == D3DRTYPE_CUBETEXTURE ? 6 : 1;
        DWORD NumLevels = pTexture->GetLevelCount();

        for (DWORD Face = 0; Face < NumFaces; ++Face) {
            for (DWORD Level = 0; Level < NumLevels; ++Level) {
                image::Image *image;
                image = getTextureImage(pDevice, pTexture, static_cast<D3DCUBEMAP_FACES>(Face), Level);
                if (image) {
                    char label[128];
                    if (Type == D3DRTYPE_CUBETEXTURE) {
                        _snprintf(label, sizeof label, "PS_RESOURCE_%lu_FACE_%lu_LEVEL_%lu", Stage, Face, Level);
                    } else {
                        _snprintf(label, sizeof label, "PS_RESOURCE_%lu_LEVEL_%lu", Stage, Level);
                    }
                    json.beginMember(label);
                    json.writeImage(image, "UNKNOWN");
                    json.endMember(); // PS_RESOURCE_*
                }
            }
        }
    }

    json.endObject();
    json.endMember(); // textures
}


void
dumpFramebuffer(JSONWriter &json, IDirect3DDevice9 *pDevice)
{
    HRESULT hr;

    json.beginMember("framebuffer");
    json.beginObject();

    D3DCAPS9 Caps;
    pDevice->GetDeviceCaps(&Caps);

    for (UINT i = 0; i < Caps.NumSimultaneousRTs; ++i) {
        com_ptr<IDirect3DSurface9> pRenderTarget;
        hr = pDevice->GetRenderTarget(i, &pRenderTarget);
        if (FAILED(hr)) {
            continue;
        }

        if (!pRenderTarget) {
            continue;
        }

        image::Image *image;
        image = getRenderTargetImage(pDevice, pRenderTarget);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label, "RENDER_TARGET_%u", i);
            json.beginMember(label);
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // RENDER_TARGET_*
        }
    }

    com_ptr<IDirect3DSurface9> pDepthStencil;
    hr = pDevice->GetDepthStencilSurface(&pDepthStencil);
    if (SUCCEEDED(hr) && pDepthStencil) {
        image::Image *image;
        image = getSurfaceImage(pDevice, pDepthStencil);
        if (image) {
            json.beginMember("DEPTH_STENCIL");
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // RENDER_TARGET_*
        }
    }


    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */

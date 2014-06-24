/**************************************************************************
 *
 * Copyright 2011-2012 VMware, Inc.
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

#include "image.hpp"
#include "json.hpp"
#include "com_ptr.hpp"
#include "d3d8imports.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


static image::Image *
getSurfaceImage(IDirect3DDevice8 *pDevice,
                IDirect3DSurface8 *pSurface) {
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
getRenderTargetImage(IDirect3DDevice8 *pDevice,
                     IDirect3DSurface8 *pRenderTarget) {
    HRESULT hr;

    if (!pRenderTarget) {
        return NULL;
    }

    D3DSURFACE_DESC Desc;
    hr = pRenderTarget->GetDesc(&Desc);
    assert(SUCCEEDED(hr));

    com_ptr<IDirect3DSurface8> pStagingSurface;
    hr = pDevice->CreateImageSurface(Desc.Width, Desc.Height, Desc.Format, &pStagingSurface);
    if (FAILED(hr)) {
        return NULL;
    }

    hr = pDevice->CopyRects(pRenderTarget, NULL, 0, pStagingSurface, NULL);
    if (FAILED(hr)) {
        std::cerr << "warning: GetRenderTargetData failed\n";
        return NULL;
    }

    return getSurfaceImage(pDevice, pStagingSurface);
}


image::Image *
getRenderTargetImage(IDirect3DDevice8 *pDevice) {
    HRESULT hr;

    com_ptr<IDirect3DSurface8> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (FAILED(hr)) {
        return NULL;
    }
    assert(pRenderTarget);

    return getRenderTargetImage(pDevice, pRenderTarget);
}


static image::Image *
getTextureImage(IDirect3DDevice8 *pDevice,
                IDirect3DBaseTexture8 *pTexture,
                D3DCUBEMAP_FACES FaceType,
                UINT Level)
{
    HRESULT hr;

    if (!pTexture) {
        return NULL;
    }

    com_ptr<IDirect3DSurface8> pSurface;

    D3DRESOURCETYPE Type = pTexture->GetType();
    switch (Type) {
    case D3DRTYPE_TEXTURE:
        assert(FaceType == D3DCUBEMAP_FACE_POSITIVE_X);
        hr = reinterpret_cast<IDirect3DTexture8 *>(pTexture)->GetSurfaceLevel(Level, &pSurface);
        break;
    case D3DRTYPE_CUBETEXTURE:
        hr = reinterpret_cast<IDirect3DCubeTexture8 *>(pTexture)->GetCubeMapSurface(FaceType, Level, &pSurface);
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

    if (Desc.Pool != D3DPOOL_DEFAULT ||
        Desc.Usage & D3DUSAGE_DYNAMIC) {
        // Lockable texture
        return getSurfaceImage(pDevice, pSurface);
    } else if (Desc.Usage & D3DUSAGE_RENDERTARGET) {
        // Rendertarget texture
        return getRenderTargetImage(pDevice, pSurface);
    } else {
        // D3D constraints are such there is not much else that can be done.
        return NULL;
    }
}


void
dumpTextures(JSONWriter &json, IDirect3DDevice8 *pDevice)
{
    HRESULT hr;

    json.beginMember("textures");
    json.beginObject();

    for (DWORD Stage = 0; Stage < 8; ++Stage) {
        com_ptr<IDirect3DBaseTexture8> pTexture;
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
dumpFramebuffer(JSONWriter &json, IDirect3DDevice8 *pDevice)
{
    HRESULT hr;

    json.beginMember("framebuffer");
    json.beginObject();

    com_ptr<IDirect3DSurface8> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (SUCCEEDED(hr) && pRenderTarget) {
        image::Image *image;
        image = getRenderTargetImage(pDevice, pRenderTarget);
        if (image) {
            json.beginMember("RENDER_TARGET_0");
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // RENDER_TARGET_*
        }
    }

    com_ptr<IDirect3DSurface8> pDepthStencil;
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

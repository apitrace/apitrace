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
#include <stdio.h>

#include "image.hpp"
#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3d8imports.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


image::Image *
ConvertImage(D3DFORMAT SrcFormat,
             void *SrcData,
             INT SrcPitch,
             UINT Width, UINT Height);


static image::Image *
getSurfaceImage(IDirect3DDevice8 *pDevice,
                IDirect3DSurface8 *pSurface) {
    image::Image *image = NULL;
    HRESULT hr;

    if (!pSurface) {
        return NULL;
    }

    D3DSURFACE_DESC Desc;
    hr = pSurface->GetDesc(&Desc);
    assert(SUCCEEDED(hr));

    if (Desc.Format == D3DFMT_NULL) {
        // dummy rendertarget
        return NULL;
    }

    D3DLOCKED_RECT LockedRect;
    hr = pSurface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK);
    if (FAILED(hr)) {
        return NULL;
    }

    image = ConvertImage(Desc.Format, LockedRect.pBits, LockedRect.Pitch, Desc.Width, Desc.Height);

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

    if (Desc.Format == D3DFMT_NULL) {
        // dummy rendertarget
        return NULL;
    }

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
dumpTextures(StateWriter &writer, IDirect3DDevice8 *pDevice)
{
    HRESULT hr;

    writer.beginMember("textures");
    writer.beginObject();

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
                    writer.beginMember(label);
                    writer.writeImage(image);
                    writer.endMember(); // PS_RESOURCE_*
                    delete image;
                }
            }
        }
    }

    writer.endObject();
    writer.endMember(); // textures
}


void
dumpFramebuffer(StateWriter &writer, IDirect3DDevice8 *pDevice)
{
    HRESULT hr;

    writer.beginMember("framebuffer");
    writer.beginObject();

    com_ptr<IDirect3DSurface8> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (SUCCEEDED(hr) && pRenderTarget) {
        image::Image *image;
        image = getRenderTargetImage(pDevice, pRenderTarget);
        if (image) {
            writer.beginMember("RENDER_TARGET_0");
            writer.writeImage(image);
            writer.endMember(); // RENDER_TARGET_*
            delete image;
        }
    }

    com_ptr<IDirect3DSurface8> pDepthStencil;
    hr = pDevice->GetDepthStencilSurface(&pDepthStencil);
    if (SUCCEEDED(hr) && pDepthStencil) {
        image::Image *image;
        image = getSurfaceImage(pDevice, pDepthStencil);
        if (image) {
            writer.beginMember("DEPTH_STENCIL");
            writer.writeImage(image);
            writer.endMember(); // RENDER_TARGET_*
            delete image;
        }
    }

    writer.endObject();
    writer.endMember(); // framebuffer
}


} /* namespace d3dstate */

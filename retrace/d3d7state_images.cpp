/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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
#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3dimports.hpp"
#include "d3dstate.hpp"


typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN              =  0,

    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,

    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_D16                  = 80,

    D3DFMT_D32F_LOCKABLE        = 82,
    D3DFMT_D24FS8               = 83,

    D3DFMT_FORCE_DWORD          = 0x7fffffff
} D3DFORMAT;


namespace d3dstate {


image::Image *
ConvertImage(D3DFORMAT SrcFormat,
             void *SrcData,
             INT SrcPitch,
             UINT Width, UINT Height);


static D3DFORMAT
convertFormat(const DDPIXELFORMAT & ddpfPixelFormat)
{
    if (ddpfPixelFormat.dwSize != sizeof ddpfPixelFormat) {
        return D3DFMT_UNKNOWN;
    }

    switch (ddpfPixelFormat.dwFlags) {
    case DDPF_RGB:
        switch (ddpfPixelFormat.dwRGBBitCount) {
        case 32:
            if (ddpfPixelFormat.dwRBitMask == 0xff0000 &&
                ddpfPixelFormat.dwGBitMask == 0x00ff00 &&
                ddpfPixelFormat.dwBBitMask == 0x0000ff) {
                return D3DFMT_X8R8G8B8;
            }
            break;
        }
        break;
    case DDPF_ZBUFFER:
    case DDPF_ZBUFFER | DDPF_STENCILBUFFER:
        switch (ddpfPixelFormat.dwZBufferBitDepth) {
        case 16:
            if (ddpfPixelFormat.dwZBitMask == 0x0000ffff) {
                return D3DFMT_D16;
            }
            break;
        case 32:
            if (ddpfPixelFormat.dwZBitMask == 0x00ffffff) {
                return D3DFMT_D24X8;
            }
            break;
        }
        break;
    }

    return D3DFMT_UNKNOWN;
}

static image::Image *
getSurfaceImage(IDirect3DDevice7 *pDevice,
                     IDirectDrawSurface7 *pSurface)
{
    HRESULT hr;

    DDSURFACEDESC2 Desc;
    ZeroMemory(&Desc, sizeof Desc);
    Desc.dwSize = sizeof Desc;

    hr = pSurface->Lock(NULL, &Desc, DDLOCK_WAIT | DDLOCK_READONLY | DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK, NULL);
    if (FAILED(hr)) {
        std::cerr << "warning: IDirectDrawSurface7::Lock failed\n";
        return NULL;
    }

    image::Image *image = NULL;
    D3DFORMAT Format = convertFormat(Desc.ddpfPixelFormat);
    if (Format == D3DFMT_UNKNOWN) {
        std::cerr << "warning: unsupported DDPIXELFORMAT\n";
    } else {
        image = ConvertImage(Format, Desc.lpSurface, Desc.lPitch, Desc.dwWidth, Desc.dwHeight);
    }

    pSurface->Unlock(NULL);

    return image;
}


image::Image *
getRenderTargetImage(IDirect3DDevice7 *pDevice) {
    HRESULT hr;

    com_ptr<IDirectDrawSurface7> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (FAILED(hr)) {
        return NULL;
    }
    assert(pRenderTarget);

    return getSurfaceImage(pDevice, pRenderTarget);
}


void
dumpTextures(StateWriter &writer, IDirect3DDevice7 *pDevice)
{
    writer.beginMember("textures");
    writer.beginObject();
    writer.endObject();
    writer.endMember(); // textures
}


void
dumpFramebuffer(StateWriter &writer, IDirect3DDevice7 *pDevice)
{
    HRESULT hr;

    writer.beginMember("framebuffer");
    writer.beginObject();

    com_ptr<IDirectDrawSurface7> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (SUCCEEDED(hr) && pRenderTarget) {
        image::Image *image;
        image = getSurfaceImage(pDevice, pRenderTarget);
        if (image) {
            writer.beginMember("RENDER_TARGET_0");
            writer.writeImage(image);
            writer.endMember(); // RENDER_TARGET_*
            delete image;
        }

        // Search for a depth-stencil attachment
        DDSCAPS2 ddsCaps;
        ZeroMemory(&ddsCaps, sizeof ddsCaps);
        ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
        com_ptr<IDirectDrawSurface7> pDepthStencil;
        hr = pRenderTarget->GetAttachedSurface(&ddsCaps, &pDepthStencil);
        if (SUCCEEDED(hr) && pDepthStencil) {
            std::cerr << "found ZS!!\n";
            image = getSurfaceImage(pDevice, pDepthStencil);
            if (image) {
                writer.beginMember("DEPTH_STENCIL");
                writer.writeImage(image);
                writer.endMember(); // DEPTH_STENCIL
                delete image;
            }
        }
    }

    writer.endObject();
    writer.endMember(); // framebuffer
}


} /* namespace d3dstate */

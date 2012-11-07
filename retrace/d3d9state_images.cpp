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

#include "image.hpp"
#include "d3d9imports.hpp"


namespace d3dstate {


image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice) {
    image::Image *image = NULL;
    IDirect3DSurface9 *pRenderTarget = NULL;
    D3DSURFACE_DESC Desc;
    IDirect3DSurface9 *pStagingSurface = NULL;
    D3DLOCKED_RECT LockedRect;
    const unsigned char *src;
    unsigned char *dst;
    HRESULT hr;

    hr = pDevice->GetRenderTarget(0, &pRenderTarget);
    if (FAILED(hr)) {
        goto no_rendertarget;
    }
    assert(pRenderTarget);

    hr = pRenderTarget->GetDesc(&Desc);
    assert(SUCCEEDED(hr));
    assert(Desc.Format == D3DFMT_X8R8G8B8 || Desc.Format == D3DFMT_A8R8G8B8);

    hr = pDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, D3DPOOL_SYSTEMMEM, &pStagingSurface, NULL);
    if (FAILED(hr)) {
        goto no_staging;
    }

    hr = pDevice->GetRenderTargetData(pRenderTarget, pStagingSurface);
    if (FAILED(hr)) {
        goto no_rendertargetdata;
    }

    hr = pStagingSurface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
    if (FAILED(hr)) {
        goto no_rendertargetdata;
    }

    image = new image::Image(Desc.Width, Desc.Height, 3, true);
    if (!image) {
        goto no_image;
    }

    dst = image->start();
    src = (const unsigned char *)LockedRect.pBits;
    for (unsigned y = 0; y < Desc.Height; ++y) {
        for (unsigned x = 0; x < Desc.Width; ++x) {
            dst[3*x + 0] = src[4*x + 2];
            dst[3*x + 1] = src[4*x + 1];
            dst[3*x + 2] = src[4*x + 0];
        }
        src += LockedRect.Pitch;
        dst += image->stride();
    }

no_image:
    pStagingSurface->UnlockRect();
no_rendertargetdata:
    pStagingSurface->Release();
no_staging:
    pRenderTarget->Release();
no_rendertarget:
    return image;
}


} /* namespace d3dstate */

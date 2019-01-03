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
#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3d9state.hpp"
#include "d3dstate.hpp"
#include "d3d9size.hpp"

#include <vector>

namespace d3dstate {


static image::Image *
getSurfaceImage(IDirect3DDevice9 *pDevice,
                IDirect3DSurface9 *pSurface,
                struct StateWriter::ImageDesc &imageDesc) {
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
    hr = pSurface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
    if (FAILED(hr)) {
        return NULL;
    }

    image = ConvertImage(Desc.Format, LockedRect.pBits, LockedRect.Pitch, Desc.Width, Desc.Height);

    pSurface->UnlockRect();

    imageDesc.format = formatToString(Desc.Format);

    return image;
}


static image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice,
                     IDirect3DSurface9 *pRenderTarget,
                     struct StateWriter::ImageDesc &imageDesc) {
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

    com_ptr<IDirect3DSurface9> pResolveSurface;
    if (Desc.MultiSampleType == D3DMULTISAMPLE_NONE) {
        pResolveSurface = pRenderTarget;
    } else {
        hr = pDevice->CreateRenderTarget(Desc.Width, Desc.Height, Desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pResolveSurface, nullptr);
        if (FAILED(hr)) {
            std::cerr << "warning: failed to create resolve rendertarget\n";
            return nullptr;
        }
        hr = pDevice->StretchRect(pRenderTarget, nullptr, pResolveSurface, nullptr, D3DTEXF_NONE);
        if (FAILED(hr)) {
            std::cerr << "warning: failed to resolve render target\n";
            return nullptr;
        }
    }

    com_ptr<IDirect3DSurface9> pStagingSurface;
    hr = pDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, D3DPOOL_SYSTEMMEM, &pStagingSurface, NULL);
    if (FAILED(hr)) {
        return NULL;
    }

    hr = pDevice->GetRenderTargetData(pResolveSurface, pStagingSurface);
    if (FAILED(hr)) {
        std::cerr << "warning: GetRenderTargetData failed\n";
        return NULL;
    }

    return getSurfaceImage(pDevice, pStagingSurface, imageDesc);
}


image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice) {
    HRESULT hr;
    struct StateWriter::ImageDesc imageDesc;

    com_ptr<IDirect3DSurface9> pRenderTarget;
    hr = pDevice->GetRenderTarget(0, &pRenderTarget);
    if (FAILED(hr)) {
        return NULL;
    }
    assert(pRenderTarget);

    return getRenderTargetImage(pDevice, pRenderTarget, imageDesc);
}


image::Image *
getRenderTargetImage(IDirect3DSwapChain9 *pSwapChain) {
    HRESULT hr;
    struct StateWriter::ImageDesc imageDesc;

    com_ptr<IDirect3DDevice9> pDevice;
    hr = pSwapChain->GetDevice(&pDevice);
    if (FAILED(hr)) {
        return NULL;
    }

    // TODO: Use GetFrontBufferData instead??
    com_ptr<IDirect3DSurface9> pBackBuffer;
    hr = pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    if (FAILED(hr)) {
        return NULL;
    }
    assert(pBackBuffer);

    return getRenderTargetImage(pDevice, pBackBuffer, imageDesc);
}


static HRESULT
getLockableRenderTargetForDepth(IDirect3DDevice9 *pDevice,
                                int w,
                                int h,
                                com_ptr<IDirect3DSurface9> &pRenderTarget)
{
    HRESULT hr;

    std::vector<D3DFORMAT> fmts ({D3DFMT_R32F, D3DFMT_G32R32F, D3DFMT_R16F, D3DFMT_X8R8G8B8});

    pRenderTarget = NULL;
    for (UINT i = 0; i < fmts.size(); i++) {
        hr = pDevice->CreateRenderTarget(w, h, fmts[i], D3DMULTISAMPLE_NONE,
                 0, TRUE, &pRenderTarget, NULL);
        if (SUCCEEDED(hr))
            break;
    }
    return hr;
}


static HRESULT
blitTexturetoRendertarget(IDirect3DDevice9 *pDevice,
                          IDirect3DBaseTexture9 *pSourceTex,
                          IDirect3DSurface9 *pRenderTarget)
{
    com_ptr<IDirect3DPixelShader9> pPS;
    HRESULT hr;

    /* state */
    com_ptr<IDirect3DSurface9> pOldRenderTarget;
    com_ptr<IDirect3DSurface9> pOldDepthStencil;
    D3DVIEWPORT9 oldViewport;
    D3DMATRIX oldProj, oldView, oldWorld;
    com_ptr<IDirect3DPixelShader9> pOldPixelShader;
    com_ptr<IDirect3DVertexShader9> pOldVertexShader;
    DWORD oldFVF;
    com_ptr<IDirect3DBaseTexture9> pOldTexture;
    com_ptr<IDirect3DStateBlock9> pState;

    static const DWORD ps_code[] =
    {
        0xffff0200,                                                             /* ps_2_0                       */
        0x0200001f, 0x90000000, 0xa00f0800,                                     /* dcl_2d s0                    */
        0x0200001f, 0x80000000, 0xb00f0000,                                     /* dcl t0                       */
        0x05000051, 0xa00f0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, /* def c0, 0.0, 0.0, 0.0, 0.0   */
        0x02000001, 0x800f0000, 0xa0e40000,                                     /* mov r0, c0                   */
        0x03000042, 0x800f0000, 0xb0e40000, 0xa0e40800,                         /* texld r0, t0, s0             */
        0x02000001, 0x800f0800, 0x80e40000,                                     /* mov oC0, r0                  */
        0x0000ffff,                                                             /* end                          */
    };

    static const struct
    {
        float x, y, z;
        float s, t, p, q;
    }
    quad[] =
    {
        { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f},
        {  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.5f},
        { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f},
        {  1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f},
    };

    hr = pDevice->CreatePixelShader(ps_code, &pPS);
    if (!SUCCEEDED(hr))
        return hr;

    /* Store state */
    pDevice->GetTransform(D3DTS_PROJECTION, &oldProj);
    pDevice->GetTransform(D3DTS_VIEW, &oldView);
    pDevice->GetTransform(D3DTS_WORLD, &oldWorld);
    pDevice->GetPixelShader(&pOldPixelShader);
    pDevice->GetVertexShader(&pOldVertexShader);
    pDevice->GetFVF(&oldFVF);
    pDevice->GetViewport(&oldViewport);
    pDevice->GetTexture(0, &pOldTexture);
    pDevice->GetRenderTarget(0, &pOldRenderTarget);
    pDevice->GetDepthStencilSurface(&pOldDepthStencil);
    /* Store samplerstates and texture and sampler states */
    pDevice->CreateStateBlock(D3DSBT_ALL, &pState);

    /* Set source */
    pDevice->SetTexture(0, pSourceTex);

    /* Set destination */
    pDevice->SetRenderTarget(0, pRenderTarget);

    pDevice->SetDepthStencilSurface(NULL);

    pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
    pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
    pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
    pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    pDevice->SetRenderState(D3DRS_STENCILREF, 0);

    D3DSURFACE_DESC desc;
    pRenderTarget->GetDesc(&desc);

    D3DVIEWPORT9 vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Height = desc.Height;
    vp.Width = desc.Width;
    vp.MaxZ = 1.0f;
    vp.MinZ = 0.0f;
    pDevice->SetViewport(&vp);

    static const D3DMATRIX identity =
    {{{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }}};

    pDevice->SetTransform(D3DTS_PROJECTION, &identity);
    pDevice->SetTransform(D3DTS_VIEW, &identity);
    pDevice->SetTransform(D3DTS_WORLD, &identity);

    pDevice->SetPixelShader(pPS);
    pDevice->SetVertexShader(NULL);
    pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0));

    pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 0);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    pDevice->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 0);

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

    /* Blit texture to rendertarget */
    pDevice->BeginScene();
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(*quad));
    pDevice->EndScene();

    /* Restore state */
    pDevice->SetTransform(D3DTS_PROJECTION, &oldProj);
    pDevice->SetTransform(D3DTS_VIEW, &oldView);
    pDevice->SetTransform(D3DTS_WORLD, &oldWorld);

    pDevice->SetPixelShader(pOldPixelShader);
    pDevice->SetVertexShader(pOldVertexShader);

    pDevice->SetFVF(oldFVF);
    pDevice->SetViewport(&oldViewport);

    pDevice->SetTexture(0, pOldTexture);
    pDevice->SetRenderTarget(0, pOldRenderTarget);

    pDevice->SetDepthStencilSurface(pOldDepthStencil);

    pState->Apply();

    return D3D_OK;
}


static bool
isDepthSamplerFormat(D3DFORMAT fmt)
{
    return (fmt == D3DFMT_DF24 ||
            fmt == D3DFMT_DF16 ||
            fmt == D3DFMT_INTZ);
}


static image::Image *
getTextureImage(IDirect3DDevice9 *pDevice,
                IDirect3DBaseTexture9 *pTexture,
                D3DCUBEMAP_FACES FaceType,
                UINT Level,
                struct StateWriter::ImageDesc &imageDesc)
{
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

    if (Type == D3DRTYPE_TEXTURE && isDepthSamplerFormat(Desc.Format)) {
        /* Special case for depth sampling formats.
         * Blit to temporary rendertarget and dump it.
         * Finally replace the format string to avoid confusion.
         */
        com_ptr<IDirect3DSurface9> pTempSurf;

        hr = getLockableRenderTargetForDepth(pDevice, Desc.Width, Desc.Height, pTempSurf);
        if (FAILED(hr) || !pTempSurf) {
            return NULL;
        }

        hr = blitTexturetoRendertarget(pDevice, pTexture, pTempSurf);
        if (FAILED(hr)) {
            return NULL;
        }

        image::Image *image;
        image = getRenderTargetImage(pDevice, pTempSurf, imageDesc);

        /* Replace rendertarget format with depth format */
        imageDesc.format = formatToString(Desc.Format);

        return image;
    } else if (Desc.Pool != D3DPOOL_DEFAULT ||
        Desc.Usage & D3DUSAGE_DYNAMIC) {
        // Lockable texture
        return getSurfaceImage(pDevice, pSurface, imageDesc);
    } else if (Desc.Usage & D3DUSAGE_RENDERTARGET) {
        // Rendertarget texture
        return getRenderTargetImage(pDevice, pSurface, imageDesc);
    } else {
        // D3D constraints are such there is not much else that can be done.
        return NULL;
    }
}


void
dumpTextures(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    HRESULT hr;

    writer.beginMember("textures");
    writer.beginObject();

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
                struct StateWriter::ImageDesc imageDesc;
                image = getTextureImage(pDevice, pTexture, static_cast<D3DCUBEMAP_FACES>(Face), Level, imageDesc);
                if (image) {
                    char label[128];
                    if (Type == D3DRTYPE_CUBETEXTURE) {
                        _snprintf(label, sizeof label, "PS_RESOURCE_%lu_FACE_%lu_LEVEL_%lu", Stage, Face, Level);
                    } else {
                        _snprintf(label, sizeof label, "PS_RESOURCE_%lu_LEVEL_%lu", Stage, Level);
                    }
                    writer.beginMember(label);
                    writer.writeImage(image, imageDesc);
                    writer.endMember(); // PS_RESOURCE_*
                    delete image;
                }
            }
        }
    }

    writer.endObject();
    writer.endMember(); // textures
}


static void
dumpRenderTargets(StateWriter &writer,
                 IDirect3DDevice9 *pDevice)
{
    HRESULT hr;
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
        struct StateWriter::ImageDesc imageDesc;
        image = getRenderTargetImage(pDevice, pRenderTarget, imageDesc);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label, "RENDER_TARGET_%u", i);
            writer.beginMember(label);
            writer.writeImage(image, imageDesc);
            writer.endMember(); // RENDER_TARGET_*
            delete image;
        }
    }
}


static void
dumpDepthStencil(StateWriter &writer,
                 IDirect3DDevice9 *pDevice,
                 com_ptr<IDirect3DSurface9> &pDepthStencil,
                 D3DFORMAT format)
{
    image::Image *image;
    struct StateWriter::ImageDesc imageDesc;

    if (!pDepthStencil)
        return;

    D3DSURFACE_DESC Desc;
    pDepthStencil->GetDesc(&Desc);

    if (Desc.Usage & D3DUSAGE_RENDERTARGET) {
        /* RESZ hack: depth has been uploaded to rendertarget */
        image = getRenderTargetImage(pDevice, pDepthStencil, imageDesc);
        imageDesc.format = formatToString(format);
    } else {
        image = getSurfaceImage(pDevice, pDepthStencil, imageDesc);
    }

    if (image) {
        writer.beginMember("DEPTH_STENCIL");
        writer.writeImage(image, imageDesc);
        writer.endMember(); // DEPTH_STENCIL
        delete image;
    }
}


static bool
isLockableFormat(D3DFORMAT fmt)
{
    return (fmt == D3DFMT_D16_LOCKABLE ||
            fmt == D3DFMT_D32F_LOCKABLE ||
            fmt == D3DFMT_S8_LOCKABLE ||
            fmt == D3DFMT_D32_LOCKABLE);
}


static bool
isRESZSupported(IDirect3DDevice9 *pDevice)
{
    com_ptr<IDirect3D9> pD3D9;
    HRESULT hr;

    pDevice->GetDirect3D(&pD3D9);

    hr = pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
            D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3DFMT_RESZ);

    return SUCCEEDED(hr);
}


static HRESULT
blitDepthRESZ(IDirect3DDevice9 *pDevice, com_ptr<IDirect3DTexture9> &pDestTex)
{
    D3DVECTOR vDummyPoint = {0.0f, 0.0f, 0.0f};
    HRESULT hr;
    DWORD oldZEnable, oldZWriteEnable, oldColorWriteEnable;
    com_ptr<IDirect3DBaseTexture9> pOldTex;

    /* Store current state */
    pDevice->GetTexture(0, &pOldTex);
    pDevice->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
    pDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
    pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &oldColorWriteEnable);

    pDevice->SetTexture(0, pDestTex);

    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

    /* Dummy draw call */
    hr = pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &vDummyPoint, sizeof(vDummyPoint));

    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZEnable);
    pDevice->SetRenderState(D3DRS_ZENABLE, oldZWriteEnable);
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, oldColorWriteEnable);

    /* Trigger RESZ hack, upload depth to texture bound at sampler 0 */
    pDevice->SetRenderState(D3DRS_POINTSIZE, RESZ_CODE);
    pDevice->SetRenderState(D3DRS_POINTSIZE, 0);

    /* Restore state */
    pDevice->SetTexture(0, pOldTex);

    return hr;
}


static HRESULT
getTextureforDepthAsTexture(IDirect3DDevice9 *pDevice, int w, int h, com_ptr<IDirect3DTexture9> &pDestTex)
{
    HRESULT hr;

    std::vector<D3DFORMAT> fmts ({D3DFMT_INTZ, D3DFMT_DF24, D3DFMT_DF16});

    for (int i = 0; i < fmts.size(); i++) {
        hr = pDevice->CreateTexture(w, h, 1, D3DUSAGE_DEPTHSTENCIL,
                fmts[i], D3DPOOL_DEFAULT, &pDestTex, NULL);
        if (SUCCEEDED(hr))
            break;
    }
    return hr;
}


void
dumpFramebuffer(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    HRESULT hr;

    writer.beginMember("framebuffer");
    writer.beginObject();

    dumpRenderTargets(writer, pDevice);

    com_ptr<IDirect3DSurface9> pDepthStencil;
    hr = pDevice->GetDepthStencilSurface(&pDepthStencil);
    if (SUCCEEDED(hr)) {
        D3DSURFACE_DESC Desc;
        pDepthStencil->GetDesc(&Desc);

        if (isLockableFormat(Desc.Format)) {
            dumpDepthStencil(writer, pDevice, pDepthStencil, Desc.Format);
        } else if(isRESZSupported(pDevice)) {
            /* Use RESZ hack to copy depth to a readable format.
             * This is working on AMD, Intel and WINE.
             * 1. Copy to depth sampling format using RESZ hack.
             * 2. Blit to temporary rendertarget and dump it.
             */
            com_ptr<IDirect3DTexture9> pTextureDepthSampler;

            hr = getTextureforDepthAsTexture(pDevice, Desc.Width, Desc.Height, pTextureDepthSampler);
            if (SUCCEEDED(hr)) {
                hr = blitDepthRESZ(pDevice, pTextureDepthSampler);
                if (SUCCEEDED(hr)) {
                    com_ptr<IDirect3DSurface9> pTempSurf;

                    hr = getLockableRenderTargetForDepth(pDevice, Desc.Width, Desc.Height, pTempSurf);

                    if (SUCCEEDED(hr)) {
                        hr = blitTexturetoRendertarget(pDevice, pTextureDepthSampler, pTempSurf);

                        if (SUCCEEDED(hr)) {
                            dumpDepthStencil(writer, pDevice, pTempSurf, Desc.Format);
                        }
                    }
                }
            }
        } else {
            // Can't to anything about it
        }
    }

    writer.endObject();
    writer.endMember(); // framebuffer
}


} /* namespace d3dstate */

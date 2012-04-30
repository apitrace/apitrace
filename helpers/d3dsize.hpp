/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * AUTHORS,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **************************************************************************/


/*
 * Auxiliary functions to compute the size of array/blob arguments.
 */

#ifndef _D3D_SIZE_HPP_
#define _D3D_SIZE_HPP_


/* We purposedly don't include any D3D header, so that this header can be used
 * with all D3D versions. */

#include <assert.h>

#include "os.hpp"


static inline size_t
_vertexCount(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount)
{
    switch (PrimitiveType) {
    case D3DPT_POINTLIST:
        return PrimitiveCount;
    case D3DPT_LINELIST:
        return PrimitiveCount*2;
    case D3DPT_LINESTRIP:
        return PrimitiveCount + 1;
    case D3DPT_TRIANGLELIST:
        return PrimitiveCount * 3;
    case D3DPT_TRIANGLESTRIP:
        return PrimitiveCount + 2;
    case D3DPT_TRIANGLEFAN:
        return PrimitiveCount + 1;
    default:
        os::log("apitrace: warning: %s: unknown D3DPRIMITIVETYPE %u\n", __FUNCTION__, PrimitiveType);
        return 0;
    }
}


static inline size_t
_vertexDataSize(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, UINT VertexStride) {
    return _vertexCount(PrimitiveType, PrimitiveCount) * VertexStride;
}


static inline size_t
_indexDataSize(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, D3DFORMAT IndexDataFormat) {
    UINT IndexStride;
    switch (IndexDataFormat) {
    case D3DFMT_INDEX16:
        IndexStride = 2;
        break;
    case D3DFMT_INDEX32:
        IndexStride = 4;
        break;
    default:
        os::log("apitrace: warning: %s: unexpected index D3DFORMAT %u\n", __FUNCTION__, IndexDataFormat);
        return 0;
    }
    return _vertexCount(PrimitiveType, PrimitiveCount) * IndexStride;
}


#if DIRECT3D_VERSION >= 0x0800

/*
 * Return the number of tokens for a given shader.
 */
static inline size_t
_shaderSize(const DWORD *pFunction)
{
    DWORD dwLength = 0;

    while (true) {
        DWORD dwToken = pFunction[dwLength++];

        switch (dwToken & D3DSI_OPCODE_MASK) {
        case D3DSIO_COMMENT:
            dwLength += (dwToken & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
            break;

        case D3DSIO_END:
            if (dwToken != D3DSIO_END) {
                os::log("apitrace: warning: %s: malformed END token\n", __FUNCTION__);
            }
            return dwLength * sizeof *pFunction;
        }
    }
}


static size_t
_formatSize(D3DFORMAT Format, UINT Width, UINT Height, INT Pitch) {
    switch ((DWORD)Format) {
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
    case D3DFMT_ATI1:
    case D3DFMT_ATI2:
        Width /= 4;
        Height /= 4;
        break;

    case D3DFMT_UYVY:
    case D3DFMT_R8G8_B8G8:
    case D3DFMT_YUY2:
    case D3DFMT_G8R8_G8B8:
        Width /= 2;
        break;

    case D3DFMT_NV12:
        return (Height + ((Height + 1) / 2)) * Pitch;

    case D3DFMT_NULL:
        return 0;

    default:
        break;
    }

    (void)Width;

    return Height * Pitch;
}


#endif /* DIRECT3D_VERSION >= 0x0800 */


#if DIRECT3D_VERSION >= 0x0900


static inline size_t
_getLockSize(IDirect3DSurface9 *pSurface, const D3DLOCKED_RECT *pLockedRect, const RECT *pRect) {
    HRESULT hr;

    D3DSURFACE_DESC Desc;
    hr = pSurface->GetDesc(&Desc);
    if (FAILED(hr)) {
        return 0;
    } 

    UINT Width;
    UINT Height;
    if (pRect) {
        Width  = pRect->right  - pRect->left;
        Height = pRect->bottom - pRect->top;
    } else {
        Width  = Desc.Width;
        Height = Desc.Height;
    }

    return _formatSize(Desc.Format, Width, Height, pLockedRect->Pitch);
}


static inline size_t
_getLockSize(IDirect3DTexture9 *pTexture, UINT Level, const D3DLOCKED_RECT *pLockedRect, const RECT *pRect) {
    HRESULT hr;

    D3DSURFACE_DESC Desc;
    hr = pTexture->GetLevelDesc(Level, &Desc);
    if (FAILED(hr)) {
        return 0;
    }

    UINT Width;
    UINT Height;
    if (pRect) {
        Width  = pRect->right  - pRect->left;
        Height = pRect->bottom - pRect->top;
    } else {
        Width  = Desc.Width;
        Height = Desc.Height;
    }

    return _formatSize(Desc.Format, Width, Height, pLockedRect->Pitch);
}


static inline size_t
_getLockSize(IDirect3DCubeTexture9 *pTexture, D3DCUBEMAP_FACES FaceType, UINT Level, const D3DLOCKED_RECT *pLockedRect, const RECT *pRect) {
    HRESULT hr;

    (void)FaceType;

    D3DSURFACE_DESC Desc;
    hr = pTexture->GetLevelDesc(Level, &Desc);
    if (FAILED(hr)) {
        return 0;
    }

    UINT Width;
    UINT Height;
    if (pRect) {
        Width  = pRect->right  - pRect->left;
        Height = pRect->bottom - pRect->top;
    } else {
        Width  = Desc.Width;
        Height = Desc.Height;
    }

    return _formatSize(Desc.Format, Width, Height, pLockedRect->Pitch);
}


#endif /* DIRECT3D_VERSION >= 0x0900 */


#endif /* _D3D_SIZE_HPP_ */

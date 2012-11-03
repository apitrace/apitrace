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

#ifndef _D3D10SIZE_HPP_
#define _D3D10SIZE_HPP_


/* We purposedly don't include any D3D header, so that this header can be used
 * with all D3D versions. */

#include <assert.h>


static size_t
_getMapSize(DXGI_FORMAT Format, UINT Width, UINT Height, INT RowPitch, UINT Depth = 1, INT DepthPitch = 0) {
    if (Width == 0 || Height == 0 || Depth == 0) {
        return 0;
    }

    if (RowPitch < 0) {
        os::log("apitrace: warning: %s: negative row pitch %i\n", __FUNCTION__, RowPitch);
        return 0;
    }

    if (DepthPitch < 0) {
        os::log("apitrace: warning: %s: negative slice pitch %i\n", __FUNCTION__, DepthPitch);
        return 0;
    }

    switch (Format) {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        Width  = (Width  + 3) / 4;
        Height = (Height + 3) / 4;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        Width = (Width + 1) / 2;
        break;

    case DXGI_FORMAT_UNKNOWN:
        return 0;

    default:
        break;
    }

    /* FIXME */
    (void)Width;

    size_t size = Height * RowPitch;

    if (Depth > 1) {
        size += (Depth - 1) * DepthPitch;
    }

    return size;
}

static inline void
_getMapInfo(ID3D10Buffer *pResource, D3D10_MAP MapType, UINT MapFlags, void * * ppData,
            void * & pMappedData, size_t & MappedSize) {
    pMappedData = 0;
    MappedSize = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_BUFFER_DESC Desc;
    pResource->GetDesc(&Desc);

    pMappedData = *ppData;
    MappedSize = Desc.ByteWidth;
}

static inline void
_getMapInfo(ID3D10Texture1D *pResource, UINT Subresource, D3D10_MAP MapType, UINT MapFlags, void * * ppData,
            void * & pMappedData, size_t & MappedSize) {
    pMappedData = 0;
    MappedSize = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_TEXTURE1D_DESC Desc;
    pResource->GetDesc(&Desc);

    pMappedData = *ppData;
    MappedSize = _getMapSize(Desc.Format, Desc.Width, 1, 0);
}

static inline void
_getMapInfo(ID3D10Texture2D *pResource, UINT Subresource, D3D10_MAP MapType, UINT MapFlags, D3D10_MAPPED_TEXTURE2D * pMappedTex2D,
            void * & pMappedData, size_t & MappedSize) {
    pMappedData = 0;
    MappedSize = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_TEXTURE2D_DESC Desc;
    pResource->GetDesc(&Desc);

    pMappedData = pMappedTex2D->pData;
    MappedSize = _getMapSize(Desc.Format, Desc.Width, Desc.Height, pMappedTex2D->RowPitch);
}

static inline void
_getMapInfo(ID3D10Texture3D *pResource, UINT Subresource, D3D10_MAP MapType, UINT MapFlags, D3D10_MAPPED_TEXTURE3D * pMappedTex3D,
            void * & pMappedData, size_t & MappedSize) {
    pMappedData = 0;
    MappedSize = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_TEXTURE3D_DESC Desc;
    pResource->GetDesc(&Desc);

    pMappedData = pMappedTex3D->pData;
    MappedSize = _getMapSize(Desc.Format, Desc.Width, Desc.Height, pMappedTex3D->RowPitch, Desc.Depth, pMappedTex3D->DepthPitch);
}


static inline void
_getMapInfo(IDXGISurface *pResource, DXGI_MAPPED_RECT * pLockedRect, UINT MapFlags,
            void * & pMappedData, size_t & MappedSize) {
    pMappedData = 0;
    MappedSize = 0;

    if (!(MapFlags & DXGI_MAP_WRITE)) {
        return;
    }

    DXGI_SURFACE_DESC Desc;
    HRESULT hr = pResource->GetDesc(&Desc);
    if (FAILED(hr)) {
        return;
    }

    pMappedData = pLockedRect->pBits;
    MappedSize = _getMapSize(Desc.Format, Desc.Width, Desc.Height, pLockedRect->Pitch);
}


#endif /* _D3D10SIZE_HPP_ */

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

#include <algorithm>

#include "dxgisize.hpp"


inline UINT
_getNumMipLevels(const D3D10_BUFFER_DESC *pDesc) {
    return 1;
}

inline UINT
_getNumMipLevels(const D3D10_TEXTURE1D_DESC *pDesc) {
    return pDesc->MipLevels != 0 ? pDesc->MipLevels : _getNumMipLevels(pDesc->Width);
}

inline UINT
_getNumMipLevels(const D3D10_TEXTURE2D_DESC *pDesc) {
    return pDesc->MipLevels != 0 ? pDesc->MipLevels : _getNumMipLevels(pDesc->Width, pDesc->Height);
}

inline UINT
_getNumMipLevels(const D3D10_TEXTURE3D_DESC *pDesc) {
    return pDesc->MipLevels != 0 ? pDesc->MipLevels : _getNumMipLevels(pDesc->Width, pDesc->Height, pDesc->Depth);
}

inline UINT
_getNumSubResources(const D3D10_BUFFER_DESC *pDesc) {
    return 1;
}

inline UINT
_getNumSubResources(const D3D10_TEXTURE1D_DESC *pDesc) {
    return _getNumMipLevels(pDesc) * pDesc->ArraySize;
}

inline UINT
_getNumSubResources(const D3D10_TEXTURE2D_DESC *pDesc) {
    return _getNumMipLevels(pDesc) * pDesc->ArraySize;
}

inline UINT
_getNumSubResources(const D3D10_TEXTURE3D_DESC *pDesc) {
    return _getNumMipLevels(pDesc);
}

static inline size_t
_calcSubresourceSize(const D3D10_BUFFER_DESC *pDesc, UINT Subresource, UINT RowPitch = 0, UINT SlicePitch = 0) {
    return pDesc->ByteWidth;
}

static inline size_t
_calcSubresourceSize(const D3D10_TEXTURE1D_DESC *pDesc, UINT Subresource, UINT RowPitch = 0, UINT SlicePitch = 0) {
    UINT MipLevel = Subresource % _getNumMipLevels(pDesc);
    return _calcMipDataSize(MipLevel, pDesc->Format, pDesc->Width, 1, RowPitch, 1, SlicePitch);
}

static inline size_t
_calcSubresourceSize(const D3D10_TEXTURE2D_DESC *pDesc, UINT Subresource, UINT RowPitch, UINT SlicePitch = 0) {
    UINT MipLevel = Subresource % _getNumMipLevels(pDesc);
    return _calcMipDataSize(MipLevel, pDesc->Format, pDesc->Width, pDesc->Height, RowPitch, 1, SlicePitch);
}

static inline size_t
_calcSubresourceSize(const D3D10_TEXTURE3D_DESC *pDesc, UINT Subresource, UINT RowPitch, UINT SlicePitch) {
    UINT MipLevel = Subresource;
    return _calcMipDataSize(MipLevel, pDesc->Format, pDesc->Width, pDesc->Height, RowPitch, pDesc->Depth, SlicePitch);
}

static inline void
_getMapDesc(ID3D10Buffer *pResource, D3D10_MAP MapType, UINT MapFlags, void * * ppData,
            _MAP_DESC & MapDesc) {
    MapDesc.pData = 0;
    MapDesc.Size = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_BUFFER_DESC Desc;
    pResource->GetDesc(&Desc);

    MapDesc.pData = *ppData;
    MapDesc.Size = Desc.ByteWidth;
}

static inline void
_getMapDesc(ID3D10Texture1D *pResource, UINT Subresource, D3D10_MAP MapType, UINT MapFlags, void * * ppData,
            _MAP_DESC & MapDesc) {
    MapDesc.pData = 0;
    MapDesc.Size = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_TEXTURE1D_DESC Desc;
    pResource->GetDesc(&Desc);

    MapDesc.pData = *ppData;
    MapDesc.Size = _calcSubresourceSize(&Desc, Subresource);
}

static inline void
_getMapDesc(ID3D10Texture2D *pResource, UINT Subresource, D3D10_MAP MapType, UINT MapFlags, D3D10_MAPPED_TEXTURE2D * pMappedTex2D,
            _MAP_DESC & MapDesc) {
    MapDesc.pData = 0;
    MapDesc.Size = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_TEXTURE2D_DESC Desc;
    pResource->GetDesc(&Desc);

    MapDesc.pData = pMappedTex2D->pData;
    MapDesc.Size = _calcSubresourceSize(&Desc, Subresource, pMappedTex2D->RowPitch);
}

static inline void
_getMapDesc(ID3D10Texture3D *pResource, UINT Subresource, D3D10_MAP MapType, UINT MapFlags, D3D10_MAPPED_TEXTURE3D * pMappedTex3D,
            _MAP_DESC & MapDesc) {
    MapDesc.pData = 0;
    MapDesc.Size = 0;

    if (MapType == D3D10_MAP_READ) {
        return;
    }

    D3D10_TEXTURE3D_DESC Desc;
    pResource->GetDesc(&Desc);

    MapDesc.pData = pMappedTex3D->pData;
    MapDesc.Size = _calcSubresourceSize(&Desc, Subresource, pMappedTex3D->RowPitch, pMappedTex3D->DepthPitch);
}


static inline size_t
_calcSubresourceSize(ID3D10Resource *pDstResource, UINT DstSubresource, const D3D10_BOX *pDstBox, UINT SrcRowPitch, UINT SrcDepthPitch) {
    if (pDstBox &&
        (pDstBox->left  >= pDstBox->right ||
         pDstBox->top   >= pDstBox->bottom ||
         pDstBox->front >= pDstBox->back)) {
        return 0;
    }

    D3D10_RESOURCE_DIMENSION Type = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pDstResource->GetType(&Type);

    DXGI_FORMAT Format;
    UINT Width;
    UINT Height = 1;
    UINT Depth = 1;
    UINT MipLevel = 0;

    switch (Type) {
    case D3D10_RESOURCE_DIMENSION_BUFFER:
        if (pDstBox) {
            return pDstBox->right - pDstBox->left;
        } else {
            D3D10_BUFFER_DESC Desc;
            static_cast<ID3D10Buffer *>(pDstResource)->GetDesc(&Desc);
            return Desc.ByteWidth;
        }
    case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
        {
            D3D10_TEXTURE1D_DESC Desc;
            static_cast<ID3D10Texture1D *>(pDstResource)->GetDesc(&Desc);
            Format = Desc.Format;
            Width = Desc.Width;
            MipLevel = DstSubresource % Desc.MipLevels;
        }
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
        {
            D3D10_TEXTURE2D_DESC Desc;
            static_cast<ID3D10Texture2D *>(pDstResource)->GetDesc(&Desc);
            Format = Desc.Format;
            Width = Desc.Width;
            Height = Desc.Height;
            MipLevel = DstSubresource % Desc.MipLevels;
        }
        break;
    case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
        {
            D3D10_TEXTURE3D_DESC Desc;
            static_cast<ID3D10Texture3D *>(pDstResource)->GetDesc(&Desc);
            Format = Desc.Format;
            Width = Desc.Width;
            Height = Desc.Height;
            Depth = Desc.Depth;
        }
        break;
    case D3D10_RESOURCE_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return 0;
    }

    if (pDstBox) {
        Width  = pDstBox->right  - pDstBox->left;
        Height = pDstBox->bottom - pDstBox->top;
        Depth  = pDstBox->back   - pDstBox->front;
    }

    return _calcMipDataSize(MipLevel, Format, Width, Height, SrcRowPitch, Depth, SrcDepthPitch);
}


#endif /* _D3D10SIZE_HPP_ */

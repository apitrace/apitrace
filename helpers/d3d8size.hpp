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

#pragma once


#include "d3dcommonsize.hpp"


static inline size_t
_declCount(const DWORD *pDeclaration) {
    size_t count = 0;
    while (pDeclaration[count++] != D3DVSD_END())
        ;
    return count;
}


static inline void
_getFormatSize(D3DFORMAT Format, size_t & BlockSize, UINT & BlockWidth, UINT & BlockHeight)
{
    BlockSize = 0;
    BlockWidth = 1;
    BlockHeight = 1;

    switch ((DWORD)Format) {

    case D3DFMT_R3G3B2:
    case D3DFMT_A8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_A4L4:
        BlockSize = 8;
        break;

    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_A8P8:
    case D3DFMT_A8L8:
    case D3DFMT_V8U8:
    case D3DFMT_L6V5U5:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D15S1:
    case D3DFMT_D16:
    case D3DFMT_INDEX16:
    case D3DFMT_R16:
        BlockSize = 16;
        break;

    case D3DFMT_R8G8B8:
        BlockSize = 24;
        break;

    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_G16R16:
    case D3DFMT_X8L8V8U8:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
    case D3DFMT_W11V11U10:
    case D3DFMT_A2W10V10U10:
    case D3DFMT_D32:
    case D3DFMT_D24S8:
    case D3DFMT_D24X8:
    case D3DFMT_D24X4S4:
    case D3DFMT_INDEX32:
    case D3DFMT_AL16:
        BlockSize = 32;
        break;

    case D3DFMT_UYVY:
    case D3DFMT_YUY2:
        BlockWidth = 2;
        BlockSize = 32;
        break;

    case D3DFMT_DXT1:
        BlockHeight = BlockWidth = 4;
        BlockSize = 64;
        break;

    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
        BlockHeight = BlockWidth = 4;
        BlockSize = 128;
        break;

    case D3DFMT_UNKNOWN:
    case D3DFMT_VERTEXDATA:
        os::log("apitrace: warning: %s: unexpected D3DFMT %u\n", __FUNCTION__, Format);
        BlockSize = 0;
        break;

    default:
        os::log("apitrace: warning: %s: unknown D3DFMT %u\n", __FUNCTION__, Format);
        BlockWidth = 0;
        break;
    }
}


static inline void
_getMapInfo(IDirect3DVertexBuffer8 *pBuffer, UINT OffsetToLock, UINT SizeToLock, BYTE ** ppbData,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = *ppbData;
    MappedSize = 0;

    if (SizeToLock == 0) {
        D3DVERTEXBUFFER_DESC Desc;
        HRESULT hr = pBuffer->GetDesc(&Desc);
        if (FAILED(hr)) {
            return;
        }
        MappedSize = Desc.Size;
    } else {
        MappedSize = SizeToLock;
    }
}


static inline void
_getMapInfo(IDirect3DIndexBuffer8 *pBuffer, UINT OffsetToLock, UINT SizeToLock, BYTE ** ppbData,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = *ppbData;
    MappedSize = 0;

    if (SizeToLock == 0) {
        D3DINDEXBUFFER_DESC Desc;
        HRESULT hr = pBuffer->GetDesc(&Desc);
        if (FAILED(hr)) {
            return;
        }
        MappedSize = Desc.Size;
    } else {
        MappedSize = SizeToLock;
    }
}


static inline void
_getMapInfo(IDirect3DSurface8 *pSurface, const D3DLOCKED_RECT *pLockedRect, const RECT *pRect,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = pLockedRect->pBits;
    MappedSize = 0;

    HRESULT hr;

    D3DSURFACE_DESC Desc;
    hr = pSurface->GetDesc(&Desc);
    if (FAILED(hr)) {
        return;
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

    MappedSize = _getLockSize(Desc.Format, pRect, Width, Height, pLockedRect->Pitch);
}


static inline void
_getMapInfo(IDirect3DTexture8 *pTexture, UINT Level, const D3DLOCKED_RECT *pLockedRect, const RECT *pRect,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = pLockedRect->pBits;
    MappedSize = 0;

    HRESULT hr;

    D3DSURFACE_DESC Desc;
    hr = pTexture->GetLevelDesc(Level, &Desc);
    if (FAILED(hr)) {
        return;
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

    MappedSize = _getLockSize(Desc.Format, pRect, Width, Height, pLockedRect->Pitch);
}


static inline void
_getMapInfo(IDirect3DCubeTexture8 *pTexture, D3DCUBEMAP_FACES FaceType, UINT Level, const D3DLOCKED_RECT *pLockedRect, const RECT *pRect,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = pLockedRect->pBits;
    MappedSize = 0;

    HRESULT hr;

    (void)FaceType;

    D3DSURFACE_DESC Desc;
    hr = pTexture->GetLevelDesc(Level, &Desc);
    if (FAILED(hr)) {
        return;
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

    MappedSize = _getLockSize(Desc.Format, pRect, Width, Height, pLockedRect->Pitch);
}


static inline void
_getMapInfo(IDirect3DVolume8 *pVolume, const D3DLOCKED_BOX *pLockedVolume, const D3DBOX *pBox,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = pLockedVolume->pBits;
    MappedSize = 0;

    HRESULT hr;

    D3DVOLUME_DESC Desc;
    hr = pVolume->GetDesc(&Desc);
    if (FAILED(hr)) {
        return;
    }

    UINT Width;
    UINT Height;
    UINT Depth;
    if (pBox) {
        Width  = pBox->Right  - pBox->Left;
        Height = pBox->Bottom - pBox->Top;
        Depth  = pBox->Back   - pBox->Front;
    } else {
        Width  = Desc.Width;
        Height = Desc.Height;
        Depth  = Desc.Depth;
    }

    MappedSize = _getLockSize(Desc.Format, pBox, Width, Height, pLockedVolume->RowPitch, Depth, pLockedVolume->SlicePitch);
}


static inline void
_getMapInfo(IDirect3DVolumeTexture8 *pTexture, UINT Level, const D3DLOCKED_BOX *pLockedVolume, const D3DBOX *pBox,
             void * & pLockedData, size_t & MappedSize) {
    pLockedData = pLockedVolume->pBits;
    MappedSize = 0;

    HRESULT hr;

    D3DVOLUME_DESC Desc;
    hr = pTexture->GetLevelDesc(Level, &Desc);
    if (FAILED(hr)) {
        return;
    }

    UINT Width;
    UINT Height;
    UINT Depth;
    if (pBox) {
        Width  = pBox->Right  - pBox->Left;
        Height = pBox->Bottom - pBox->Top;
        Depth  = pBox->Back   - pBox->Front;
    } else {
        Width  = Desc.Width;
        Height = Desc.Height;
        Depth  = Desc.Depth;
    }

    MappedSize = _getLockSize(Desc.Format, pBox, Width, Height, pLockedVolume->RowPitch, Depth, pLockedVolume->SlicePitch);
}



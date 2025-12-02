/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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


static inline size_t
_getVertexSize(DWORD dwFVF) {
    size_t size = 0;

    assert((dwFVF & D3DFVF_RESERVED0) == 0);

    switch (dwFVF & D3DFVF_POSITION_MASK) {
    case D3DFVF_XYZ:
        size += 3 * sizeof(FLOAT);
        break;
    case D3DFVF_XYZRHW:
        size += 4 * sizeof(FLOAT);
        break;
    case D3DFVF_XYZB1:
        size += (3 + 1) * sizeof(FLOAT);
        break;
    case D3DFVF_XYZB2:
        size += (3 + 2) * sizeof(FLOAT);
        break;
    case D3DFVF_XYZB3:
        size += (3 + 3) * sizeof(FLOAT);
        break;
    case D3DFVF_XYZB4:
        size += (3 + 4) * sizeof(FLOAT);
        break;
    case D3DFVF_XYZB5:
        size += (3 + 5) * sizeof(FLOAT);
        break;
#if DIRECT3D_VERSION >= 0x0900
    case D3DFVF_XYZW:
        size += 4 * sizeof(FLOAT);
        break;
#endif
    }

    if (dwFVF & D3DFVF_NORMAL) {
        size += 3 * sizeof(FLOAT);
    }
#if DIRECT3D_VERSION >= 0x0800
    if (dwFVF & D3DFVF_PSIZE) {
        size += sizeof(FLOAT);
    }
#else
    if (dwFVF & D3DFVF_RESERVED1) {
        // D3DLVERTEX
        size += sizeof(DWORD);
    }
#endif
    if (dwFVF & D3DFVF_DIFFUSE) {
        size += sizeof(D3DCOLOR);
    }
    if (dwFVF & D3DFVF_SPECULAR) {
        size += sizeof(D3DCOLOR);
    }

    DWORD dwNumTextures = (dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    for (DWORD CoordIndex = 0; CoordIndex < dwNumTextures; ++CoordIndex) {
        // See D3DFVF_TEXCOORDSIZE*
        DWORD dwTexCoordSize = (dwFVF >> (CoordIndex*2 + 16)) & 3;
        switch (dwTexCoordSize) {
        case D3DFVF_TEXTUREFORMAT2:
            size += 2 * sizeof(FLOAT);
            break;
        case D3DFVF_TEXTUREFORMAT1:
            size += 1 * sizeof(FLOAT);
            break;
        case D3DFVF_TEXTUREFORMAT3:
            size += 3 * sizeof(FLOAT);
            break;
        case D3DFVF_TEXTUREFORMAT4:
            size += 4 * sizeof(FLOAT);
            break;
        }
    }

    assert((dwFVF & D3DFVF_RESERVED2) == 0);

    return size;
}

static inline void
_getFormatSize(LPDDPIXELFORMAT fmt, size_t& BlockSize, UINT& BlockWidth, UINT& BlockHeight) {
    BlockSize = 0;
    BlockWidth = 1;
    BlockHeight = 1;

    if (fmt->dwFlags & DDPF_RGB) {
        BlockSize = fmt->dwRGBBitCount;
    }
    else if ((fmt->dwFlags & DDPF_ZBUFFER)) {
        BlockSize = fmt->dwZBufferBitDepth;
    }
    else if ((fmt->dwFlags & DDPF_LUMINANCE)) {
        BlockSize = fmt->dwLuminanceBitCount;
    }
    else if ((fmt->dwFlags & DDPF_BUMPDUDV)) {
        BlockSize = fmt->dwBumpBitCount;
    }
    else if ((fmt->dwFlags & DDPF_FOURCC)) {
        switch (fmt->dwFourCC) {
        case MAKEFOURCC('D', 'X', 'T', '1'):
            BlockHeight = BlockWidth = 4;
            BlockSize = 64;
            break;
        case MAKEFOURCC('D', 'X', 'T', '2'):
        case MAKEFOURCC('D', 'X', 'T', '3'):
        case MAKEFOURCC('D', 'X', 'T', '4'):
        case MAKEFOURCC('D', 'X', 'T', '5'):
            BlockHeight = BlockWidth = 4;
            BlockSize = 128;
            break;
        default:
            os::log("apitrace: warning: %s: unknown FOURCC DDPIXELFORMAT %lu\n", __FUNCTION__, fmt->dwFlags);
            BlockSize = 0;
            break;
        }
    }
    else {
        os::log("apitrace: warning: %s: unknown DDPIXELFORMAT %lu\n", __FUNCTION__, fmt->dwFlags);
        BlockSize = 0;
    }
}

#define PACKED_PITCH 0x7ffffff

static inline size_t
_getLockSize(LPDDPIXELFORMAT Format, bool Partial, UINT Width, UINT Height, INT RowPitch = PACKED_PITCH, UINT Depth = 1, INT SlicePitch = 0) {
    if (Width == 0 || Height == 0 || Depth == 0) {
        return 0;
    }

    if (RowPitch < 0) {
        os::log("apitrace: warning: %s: negative row pitch %i\n", __FUNCTION__, RowPitch);
        return 0;
    }

    size_t size;
    {
        size_t BlockSize;
        UINT BlockWidth;
        UINT BlockHeight;
        _getFormatSize(Format, BlockSize, BlockWidth, BlockHeight);
        assert(BlockHeight);
        Height = (Height + BlockHeight - 1) / BlockHeight;

        if (RowPitch == PACKED_PITCH) {
            RowPitch = ((Width + BlockWidth - 1) / BlockWidth * BlockSize + 7) / 8;
        }

        size = Height * RowPitch;

        if (Partial || Height == 1) {
            // Must take pixel size in consideration
            if (BlockWidth) {
                Width = (Width + BlockWidth - 1) / BlockWidth;
                size = (Width * BlockSize + 7) / 8;
                if (Height > 1) {
                    size += (Height - 1) * RowPitch;
                }
            }
        }
    }

    if (Depth > 1) {
        if (SlicePitch < 0) {
            os::log("apitrace: warning: %s: negative slice pitch %i\n", __FUNCTION__, SlicePitch);
            return 0;
        }

        size += (Depth - 1) * SlicePitch;
    }

    return size;
}

static inline void
_getMapInfo(IDirectDrawSurface* pSurface, RECT* pRect, DDSURFACEDESC* pDesc,
    void*& pLockedData, size_t& MappedSize) {
    MappedSize = 0;
    pLockedData = nullptr;

    UINT Width;
    UINT Height;
    if (pRect) {
        Width = pRect->right - pRect->left;
        Height = pRect->bottom - pRect->top;
    }
    else {
        Width = pDesc->dwWidth;
        Height = pDesc->dwHeight;
    }

    pLockedData = pDesc->lpSurface;
    MappedSize = _getLockSize(&pDesc->ddpfPixelFormat, pRect, Width, Height, pDesc->lPitch);
}

static inline void
_getMapInfo(IDirectDrawSurface2* pSurface, RECT* pRect, DDSURFACEDESC* pDesc,
    void*& pLockedData, size_t& MappedSize) {
    MappedSize = 0;
    pLockedData = nullptr;

    UINT Width;
    UINT Height;
    if (pRect) {
        Width = pRect->right - pRect->left;
        Height = pRect->bottom - pRect->top;
    }
    else {
        Width = pDesc->dwWidth;
        Height = pDesc->dwHeight;
    }

    pLockedData = pDesc->lpSurface;
    MappedSize = _getLockSize(&pDesc->ddpfPixelFormat, pRect, Width, Height, pDesc->lPitch);
}

static inline void
_getMapInfo(IDirectDrawSurface3* pSurface, RECT* pRect, DDSURFACEDESC* pDesc,
    void*& pLockedData, size_t& MappedSize) {
    MappedSize = 0;
    pLockedData = nullptr;

    UINT Width;
    UINT Height;
    if (pRect) {
        Width = pRect->right - pRect->left;
        Height = pRect->bottom - pRect->top;
    }
    else {
        Width = pDesc->dwWidth;
        Height = pDesc->dwHeight;
    }

    pLockedData = pDesc->lpSurface;
    MappedSize = _getLockSize(&pDesc->ddpfPixelFormat, pRect, Width, Height, pDesc->lPitch);
}

static inline void
_getMapInfo(IDirectDrawSurface4* pSurface, RECT* pRect, DDSURFACEDESC2* pDesc,
    void*& pLockedData, size_t& MappedSize) {
    MappedSize = 0;
    pLockedData = nullptr;

    UINT Width;
    UINT Height;
    if (pRect) {
        Width = pRect->right - pRect->left;
        Height = pRect->bottom - pRect->top;
    }
    else {
        Width = pDesc->dwWidth;
        Height = pDesc->dwHeight;
    }

    pLockedData = pDesc->lpSurface;
    MappedSize = _getLockSize(&pDesc->ddpfPixelFormat, pRect, Width, Height, pDesc->lPitch);
}

static inline void
_getMapInfo(IDirectDrawSurface7* pSurface, RECT* pRect, DDSURFACEDESC2* pDesc,
    void*& pLockedData, size_t& MappedSize) {
    MappedSize = 0;
    pLockedData = nullptr;

    UINT Width;
    UINT Height;
    if (pRect) {
        Width = pRect->right - pRect->left;
        Height = pRect->bottom - pRect->top;
    }
    else {
        Width = pDesc->dwWidth;
        Height = pDesc->dwHeight;
    }

    pLockedData = pDesc->lpSurface;
    MappedSize = _getLockSize(&pDesc->ddpfPixelFormat, pRect, Width, Height, pDesc->lPitch);
}

static inline void
_getMapInfo(IDirect3DVertexBuffer* pBuffer, void** ppbData,
    void*& pLockedData, size_t& MappedSize) {
    pLockedData = nullptr;
    MappedSize = 0;

    D3DVERTEXBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    HRESULT hr = pBuffer->GetVertexBufferDesc(&desc);
    if (FAILED(hr)) {
        return;
    }

    pLockedData = *ppbData;
    MappedSize = _getVertexSize(desc.dwFVF) * desc.dwNumVertices;
}

static inline void
_getMapInfo(IDirect3DVertexBuffer7* pBuffer, void** ppbData,
    void*& pLockedData, size_t& MappedSize) {
    pLockedData = nullptr;
    MappedSize = 0;

    D3DVERTEXBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    HRESULT hr = pBuffer->GetVertexBufferDesc(&desc);
    if (FAILED(hr)) {
        return;
    }

    if (ppbData && *ppbData) {
        pLockedData = *ppbData;
        MappedSize = _getVertexSize(desc.dwFVF) * desc.dwNumVertices;
    }
}

static inline void
_getMapInfo(IDirect3DExecuteBuffer* pBuffer, D3DEXECUTEBUFFERDESC* pDesc,
    void*& pLockedData, size_t& MappedSize) {

    pLockedData = pDesc->lpData;
    MappedSize = pDesc->dwBufferSize;
}


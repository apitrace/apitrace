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


/* We purposedly don't include any D3D header, so that this header can be used
 * with all D3D versions. */

#include <assert.h>

#include <algorithm>

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
        return PrimitiveCount + 2;
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
            if (dwToken == D3DSIO_END) {
                return dwLength * sizeof *pFunction;
            }
            break;
        }
    }
}


static inline void
_getFormatSize(D3DFORMAT Format, size_t & BlockSize, UINT & BlockWidth, UINT & BlockHeight);

#define PACKED_PITCH 0x7ffffff

static inline size_t
_getLockSize(D3DFORMAT Format, bool Partial, UINT Width, UINT Height, INT RowPitch = PACKED_PITCH, UINT Depth = 1, INT SlicePitch = 0) {
    if (Width == 0 || Height == 0 || Depth == 0) {
        return 0;
    }

    if (RowPitch < 0) {
        os::log("apitrace: warning: %s: negative row pitch %i\n", __FUNCTION__, RowPitch);
        return 0;
    }

    size_t size;
    if (Format == MAKEFOURCC('N','V','1','2') ||
        Format == MAKEFOURCC('Y','V','1','2')) {
        // Planar YUV

        if (RowPitch == PACKED_PITCH) {
            RowPitch = Width;
        }

        size = (Height + (Height + 1)/2) * RowPitch;
    } else if (Format == MAKEFOURCC('A','T','I','1')) {
        // 64 bits per 4x4 block, but limited to height*pitch

        if (RowPitch == PACKED_PITCH) {
            RowPitch = Width;
        }

        size = std::min(Height * RowPitch,  ((Height + 3)/4) * ((Width + 3)/4) * (64 / 8));
    } else if (Format == MAKEFOURCC('A','T','I','2')) {
        // 128 bits per 4x4 block, but limited to height*pitch

        if (RowPitch == PACKED_PITCH) {
            RowPitch = Width;
        }

        size = std::min(Height * RowPitch,  ((Height + 3)/4) * ((Width + 3)/4) * (128 / 8));
    } else {
        size_t BlockSize;
        UINT BlockWidth;
        UINT BlockHeight;
        _getFormatSize(Format, BlockSize, BlockWidth, BlockHeight);
        assert(BlockHeight);
        Height = (Height + BlockHeight - 1) / BlockHeight;

        if (RowPitch == PACKED_PITCH) {
            RowPitch = ((Width + BlockWidth  - 1) / BlockWidth * BlockSize + 7) / 8;
        }

        size = Height * RowPitch;

        if (Partial || Height == 1) {
            // Must take pixel size in consideration
            if (BlockWidth) {
                Width = (Width + BlockWidth  - 1) / BlockWidth;
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



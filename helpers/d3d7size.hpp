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





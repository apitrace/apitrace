/**************************************************************************
 *
 * Copyright 2020 Joshua Ashton for Valve Software
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

#include <assert.h>

#include <algorithm>

#include "dxgisize.hpp"
#include "com_ptr.hpp"

template<typename T, typename U = T>
constexpr T align(T what, U to) {
    return (what + to - 1) & ~(to - 1);
}

static inline size_t
_calcSubresourceSize12(ID3D12Resource *pDstResource, UINT DstSubresource, const D3D12_BOX *pDstBox, UINT SrcRowPitch, UINT SrcDepthPitch)
{
    if (pDstBox &&
        (pDstBox->left  >= pDstBox->right ||
         pDstBox->top   >= pDstBox->bottom ||
         pDstBox->front >= pDstBox->back)) {
        return 0;
    }

    D3D12_RESOURCE_DESC Desc = pDstResource->GetDesc();

    UINT64 Width;
    UINT Height;
    UINT16 Depth;
    if (pDstBox) {
        Width  = pDstBox->right  - pDstBox->left;
        Height = pDstBox->bottom - pDstBox->top;
        Depth  = pDstBox->back   - pDstBox->front;
    } else {
        assert(Desc.Width  > 0);
        assert(Desc.Height > 0);
        assert(Desc.DepthOrArraySize > 0);
        UINT16 MipLevel = DstSubresource % Desc.MipLevels;
        Width  = std::max(Desc.Width  >> MipLevel, UINT64(1));
        Height = std::max(Desc.Height >> MipLevel, UINT(1));
        if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D) {
            Depth = std::max(UINT16(Desc.DepthOrArraySize >> MipLevel), UINT16(1));
        } else {
            Depth = 1;
        }
    }

    return _calcDataSize(Desc.Format, Width, Height, SrcRowPitch, Depth, SrcDepthPitch);
}


static inline UINT64
_getMapSize(ID3D12Resource* pResource)
{
    D3D12_RESOURCE_DESC Desc = pResource->GetDesc();

    if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
        return Desc.Width;

    com_ptr<ID3D12Device> pDevice;
    pResource->GetDevice(IID_ID3D12Device, (void **) &pDevice);

    D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = { Desc.Format, 0 };
    if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_INFO, &formatInfo, sizeof(formatInfo))))
        return 0;

    UINT64 TotalBytes = 0;
    pDevice->GetCopyableFootprints(&Desc, 0, formatInfo.PlaneCount * Desc.MipLevels * Desc.DepthOrArraySize, 0, nullptr, nullptr, nullptr, &TotalBytes);

    return TotalBytes;
}

static inline D3D12_PIPELINE_STATE_SUBOBJECT_TYPE
_getStateStreamType(char *pStream)
{
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE value = *reinterpret_cast<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE*>(pStream);
    return value;
}
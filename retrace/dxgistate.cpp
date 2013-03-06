/**************************************************************************
 *
 * Copyright 2013 VMware, Inc.
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


#include "dxgistate.hpp"

#ifdef __MINGW32__
#define nullptr NULL
#endif
#include "DirectXTex.h"


namespace d3dstate {


/**
 * Convert between DXGI formats.
 *
 */
HRESULT
ConvertFormat(DXGI_FORMAT SrcFormat,
              void *SrcData,
              UINT SrcPitch,
              DXGI_FORMAT DstFormat,
              void *DstData,
              UINT DstPitch,
              UINT Width, UINT Height)
{
    HRESULT hr;

    DirectX::Image SrcImage;
    DirectX::Image DstImage;

    SrcImage.width = Width;
    SrcImage.height = Height;
    SrcImage.format = SrcFormat;
    SrcImage.rowPitch = SrcPitch;
    SrcImage.slicePitch = Height * SrcPitch;
    SrcImage.pixels = (uint8_t*)SrcData;

    DstImage.width = Width;
    DstImage.height = Height;
    DstImage.format = DstFormat;
    DstImage.rowPitch = DstPitch;
    DstImage.slicePitch = Height * DstPitch;
    DstImage.pixels = (uint8_t*)DstData;

    DirectX::Rect rect(0, 0, Width, Height);

    if (SrcFormat != DstFormat) {
        DirectX::ScratchImage ScratchImage;
        ScratchImage.Initialize2D(DstFormat, Width, Height, 1, 1);

        hr = DirectX::Convert(SrcImage, DstFormat, DirectX::TEX_FILTER_DEFAULT, 0.0f, ScratchImage);
        if (SUCCEEDED(hr)) {
            hr = CopyRectangle(*ScratchImage.GetImage(0, 0, 0), rect, DstImage, DirectX::TEX_FILTER_DEFAULT, 0, 0);
        }
    } else {
        hr = CopyRectangle(SrcImage, rect, DstImage, DirectX::TEX_FILTER_DEFAULT, 0, 0);
    }

    return hr;
}


} /* namespace d3dstate */

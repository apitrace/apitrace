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

#pragma once


#include <windows.h>

#include "winsdk_compat.h"

#include <dxgi.h>


#ifndef ARRAYSIZE
#define ARRAYSIZE(_x) (sizeof(_x)/sizeof((_x)[0]))
#endif


namespace image {
    class Image;
}


namespace d3dstate {


HRESULT
ConvertFormat(DXGI_FORMAT SrcFormat,
              void *SrcData,
              UINT SrcPitch,
              DXGI_FORMAT DstFormat,
              void *DstData,
              UINT DstPitch,
              UINT Width, UINT Height);

image::Image *
ConvertImage(DXGI_FORMAT SrcFormat,
             void *SrcData,
             UINT SrcPitch,
             UINT Width, UINT Height);


image::Image *
getRenderTargetImage(IDXGISwapChain *pSwapChain);

std::string
getDXGIFormatName(DXGI_FORMAT format);


} /* namespace d3dstate */



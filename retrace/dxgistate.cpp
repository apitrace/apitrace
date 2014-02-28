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


#include <assert.h>

#include <iostream>

#include "image.hpp"
#include "json.hpp"

#include "dxgistate.hpp"
#include "d3d10state.hpp"

#ifdef __MINGW32__
#define nullptr NULL
#endif
#include "DirectXTex.h"


namespace d3dstate {


static DXGI_FORMAT
ChooseConversionFormat(DXGI_FORMAT Format, unsigned &numChannels, image::ChannelType &channelType)
{
    numChannels = 4;
    channelType = image::TYPE_UNORM8;

    switch (Format) {

    // Float
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        channelType = image::TYPE_FLOAT;
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    // Unsigned normalized
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R1_UNORM:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_BC7_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    // SRGB
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    // Signed normalized
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return DXGI_FORMAT_R8G8B8A8_SNORM;

    // Unsigned
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;

    // Signed
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R32G32B32_SINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_SINT:
        return DXGI_FORMAT_R8G8B8A8_SINT;

    // Depth
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_D16_UNORM:
        numChannels = 1;
        channelType = image::TYPE_FLOAT;
        return DXGI_FORMAT_D32_FLOAT;

    // Typeless
    case DXGI_FORMAT_UNKNOWN:
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return DXGI_FORMAT_UNKNOWN;

    default:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
}


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

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));
    if (FAILED(hr)) {
        return hr;
    }

    if (SrcFormat != DstFormat) {
        DirectX::ScratchImage ScratchImage;
        ScratchImage.Initialize2D(DstFormat, Width, Height, 1, 1);

        if (DirectX::IsCompressed(SrcFormat)) {
            hr = DirectX::Decompress(SrcImage, DstFormat, ScratchImage);
        } else {
            hr = DirectX::Convert(SrcImage, DstFormat, DirectX::TEX_FILTER_DEFAULT, 0.0f, ScratchImage);
        }

        if (SUCCEEDED(hr)) {
            hr = CopyRectangle(*ScratchImage.GetImage(0, 0, 0), rect, DstImage, DirectX::TEX_FILTER_DEFAULT, 0, 0);
        }
    } else {
        hr = CopyRectangle(SrcImage, rect, DstImage, DirectX::TEX_FILTER_DEFAULT, 0, 0);
    }

    return hr;
}


image::Image *
ConvertImage(DXGI_FORMAT SrcFormat,
             void *SrcData,
             UINT SrcPitch,
             UINT Width, UINT Height)
{
    unsigned numChannels;
    image::ChannelType channelType;

    DXGI_FORMAT DstFormat = ChooseConversionFormat(SrcFormat, numChannels, channelType);
    if (DstFormat == DXGI_FORMAT_UNKNOWN) {
        return NULL;
    }

    image::Image *image = new image::Image(Width, Height, numChannels, false, channelType);
    if (!image) {
        return NULL;
    }
    assert(image->stride() > 0);

    HRESULT hr;
    hr = ConvertFormat(SrcFormat,
                       SrcData, SrcPitch,
                       DstFormat,
                       image->start(), image->stride(),
                       Width, Height);
    if (FAILED(hr)) {
        std::cerr << "warning: failed to convert from format " << SrcFormat << " to format " << DstFormat << "\n";
        delete image;
        image = NULL;
    }

    return image;
}


image::Image *
getRenderTargetImage(IDXGISwapChain *pSwapChain)
{
    HRESULT hr;

    assert(pSwapChain);
    if (!pSwapChain) {
        return NULL;
    }

    DXGI_SWAP_CHAIN_DESC Desc;
    hr = pSwapChain->GetDesc(&Desc);
    assert(SUCCEEDED(hr));

    /*
     * There is a IDXGISurface::Map method, but swapchains are not normally mappable,
     * and there is no way to copy into a staging resource, which effectively means there
     * is no way to read a IDXGISwapChain using DXGI interfaces alone.
     *
     * We must figure out
     * the appropriate D3D10/D3D11 interfaces, and use them instead.
     */

    ID3D10Resource *pD3D10Resource = NULL;
    hr = pSwapChain->GetBuffer(0, IID_ID3D10Resource, (void **)&pD3D10Resource);
    if (SUCCEEDED(hr)) {
        ID3D10Device * pD3D10Device;
        hr = pSwapChain->GetDevice(IID_ID3D10Device, (void **)&pD3D10Device);
        assert(SUCCEEDED(hr));
        if (FAILED(hr)) {
            return NULL;
        }

        DXGI_FORMAT Format = Desc.BufferDesc.Format;

        return getSubResourceImage(pD3D10Device, pD3D10Resource, Format, 0, 0);
    }

    /* FIXME: Handle D3D11 too. */

    return NULL;
}


void
dumpDevice(std::ostream &os, IDXGISwapChain *pSwapChain)
{
    JSONWriter json(os);

    json.beginMember("framebuffer");
    json.beginObject();

    if (pSwapChain) {
        image::Image *image;
        image = getRenderTargetImage(pSwapChain);
        if (image) {
            json.beginMember("SWAP_CHAIN");
            json.writeImage(image, "UNKNOWN");
            json.endMember();
        }
    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */

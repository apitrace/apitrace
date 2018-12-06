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
#include "state_writer.hpp"
#include "com_ptr.hpp"

#include "dxgistate.hpp"
#include "d3d10state.hpp"
#include "d3d11state.hpp"
#include "d3d11imports.hpp"

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
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    // SRGB
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

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

    // 8-bit unsigned
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;

    // 4-channel integers
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R10G10B10A2_UINT:
        numChannels = 4;
        channelType = image::TYPE_FLOAT;
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    // 3-channel integers
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        numChannels = 3;
        channelType = image::TYPE_FLOAT;
        return DXGI_FORMAT_R32G32B32_FLOAT;

    // 2-channel integers
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R8G8_SINT:
        numChannels = 2;
        channelType = image::TYPE_FLOAT;
        return DXGI_FORMAT_R32G32_FLOAT;

    // 1-channel integers
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_SINT:
        numChannels = 1;
        channelType = image::TYPE_FLOAT;
        return DXGI_FORMAT_R32_FLOAT;

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

    /*
     * Avoid WIC, as it's not reliable (e.g, R32_FLOAT ->
     * R32G32B32A32_FLOAT will fail with E_INVALIDARG.)
     */
    DWORD filter = DirectX::TEX_FILTER_FORCE_NON_WIC;

    if (SrcFormat != DstFormat) {
        DirectX::ScratchImage ScratchImage;
        ScratchImage.Initialize2D(DstFormat, Width, Height, 1, 1);

        if (DirectX::IsCompressed(SrcFormat)) {
            hr = DirectX::Decompress(SrcImage, DstFormat, ScratchImage);
        } else {
            hr = DirectX::Convert(SrcImage, DstFormat, filter, 0.0f, ScratchImage);
        }

        if (SUCCEEDED(hr)) {
            hr = DirectX::CopyRectangle(*ScratchImage.GetImage(0, 0, 0), rect, DstImage, filter, 0, 0);
        }
    } else {
        hr = DirectX::CopyRectangle(SrcImage, rect, DstImage, filter, 0, 0);
    }

    return hr;
}


image::Image *
ConvertImage(DXGI_FORMAT SrcFormat,
             void *SrcData,
             UINT SrcPitch,
             UINT Width, UINT Height,
             bool isDepthStencil)
{
    unsigned numChannels;
    image::ChannelType channelType;

    // DirectXTex doesn't support the typeless depth-stencil/uav variants
    switch (SrcFormat) {
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        SrcFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;

    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        // FIXME: This will dump the depth-channel.  It would be more
        // adequate to dump the stencil channel, but we can't achieve
        // that with DirectXTex.
        SrcFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;

    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        SrcFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        break;

    case DXGI_FORMAT_R32_TYPELESS:
        SrcFormat = isDepthStencil ? DXGI_FORMAT_D32_FLOAT : DXGI_FORMAT_R32_UINT;
        break;

    default:
        break;
    }

    DXGI_FORMAT DstFormat = ChooseConversionFormat(SrcFormat, numChannels, channelType);
    if (DstFormat == DXGI_FORMAT_UNKNOWN) {
        std::cerr << "warning: don't know how to convert format " << getDXGIFormatName(SrcFormat) << "\n";
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
        std::cerr << "warning: failed to convert from format " << getDXGIFormatName(SrcFormat)
                  << " to format " << getDXGIFormatName(DstFormat) << "\n";
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

    DXGI_FORMAT Format = Desc.BufferDesc.Format;

    /*
     * There is a IDXGISurface::Map method, but swapchains are not normally mappable,
     * and there is no way to copy into a staging resource, which effectively means there
     * is no way to read a IDXGISwapChain using DXGI interfaces alone.
     *
     * We must figure out
     * the appropriate D3D10/D3D11 interfaces, and use them instead.
     */

    com_ptr<ID3D11Device1> pD3D11Device1;
    hr = pSwapChain->GetDevice(IID_ID3D11Device1, (void **)&pD3D11Device1);
    if (SUCCEEDED(hr)) {
        /*
         * With D3D11.1 runtime (that comes with Windows 8 and Windows 7
         * Platform Update), we can no longer guess the device interface based
         * on the success of GetDevice, since it will always succeed regardless
         * the requested device interface, but the returned device maight be
         * inactive/lockedout, and attempt to use it will cause the call to be
         * ignored, though a debug runtime will issue a
         * DEVICE_LOCKEDOUT_INTERFACE warning.
         *
         * See also:
         * - https://msdn.microsoft.com/en-us/library/windows/desktop/hh404583.aspx
         * - https://social.msdn.microsoft.com/Forums/en-US/c482bcec-b094-4df0-9253-9c496b115beb/d3d11-runtime-assertion-when-using-graphics-debugger-and-other-applications-may-be-intercepting?forum=wingameswithdirectx
         */
        com_ptr<ID3D11Resource> pD3D11Resource;
        hr = pSwapChain->GetBuffer(0, IID_ID3D11Resource, (void **)&pD3D11Resource);
        assert(SUCCEEDED(hr));
        if (FAILED(hr)) {
            return NULL;
        }

        com_ptr<ID3D11DeviceContext1> pD3D11DeviceContext;
        pD3D11Device1->GetImmediateContext1(&pD3D11DeviceContext);

        UINT Flags = 0;
        UINT DeviceFlags = pD3D11Device1->GetCreationFlags();
        if (DeviceFlags & D3D11_CREATE_DEVICE_SINGLETHREADED) {
            Flags |= D3D11_1_CREATE_DEVICE_CONTEXT_STATE_SINGLETHREADED;
        }
        D3D_FEATURE_LEVEL FeatureLevel = pD3D11Device1->GetFeatureLevel();

        com_ptr<ID3DDeviceContextState> pState;
        hr = pD3D11Device1->CreateDeviceContextState(Flags, &FeatureLevel, 1, D3D11_SDK_VERSION, IID_ID3D11Device1, NULL, &pState);
        if (SUCCEEDED(hr)) {
            com_ptr<ID3DDeviceContextState> pPreviousState;
            pD3D11DeviceContext->SwapDeviceContextState(pState, &pPreviousState);

            image::Image *image;
            image = getSubResourceImage(pD3D11DeviceContext, pD3D11Resource, Format, 0, 0);

            pD3D11DeviceContext->SwapDeviceContextState(pPreviousState, NULL);

            return image;
        }
    }

    com_ptr<ID3D11Device> pD3D11Device;
    hr = pSwapChain->GetDevice(IID_ID3D11Device, (void **)&pD3D11Device);
    if (SUCCEEDED(hr)) {
        com_ptr<ID3D11Resource> pD3D11Resource;
        hr = pSwapChain->GetBuffer(0, IID_ID3D11Resource, (void **)&pD3D11Resource);
        assert(SUCCEEDED(hr));
        if (FAILED(hr)) {
            return NULL;
        }

        com_ptr<ID3D11DeviceContext> pD3D11DeviceContext;
        pD3D11Device->GetImmediateContext(&pD3D11DeviceContext);

        return getSubResourceImage(pD3D11DeviceContext, pD3D11Resource, Format, 0, 0);
    }

    com_ptr<ID3D10Device> pD3D10Device;
    hr = pSwapChain->GetDevice(IID_ID3D10Device, (void **)&pD3D10Device);
    if (SUCCEEDED(hr)) {
        com_ptr<ID3D10Resource> pD3D10Resource;
        hr = pSwapChain->GetBuffer(0, IID_ID3D10Resource, (void **)&pD3D10Resource);
        assert(SUCCEEDED(hr));
        if (FAILED(hr)) {
            return NULL;
        }

        return getSubResourceImage(pD3D10Device, pD3D10Resource, Format, 0, 0);
    }

    return NULL;
}


void
dumpDevice(StateWriter &writer, IDXGISwapChain *pSwapChain)
{
    HRESULT hr;

    if (pSwapChain) {
        com_ptr<ID3D10Device> pD3D10Device;
        hr = pSwapChain->GetDevice(IID_ID3D10Device, (void **)&pD3D10Device);
        if (SUCCEEDED(hr)) {
             dumpDevice(writer, pD3D10Device);
             return;
        }

        com_ptr<ID3D11Device> pD3D11Device;
        hr = pSwapChain->GetDevice(IID_ID3D11Device, (void **)&pD3D11Device);
        if (SUCCEEDED(hr)) {
            com_ptr<ID3D11DeviceContext> pD3D11DeviceContext;
            pD3D11Device->GetImmediateContext(&pD3D11DeviceContext);
            dumpDevice(writer, pD3D11DeviceContext);
            return;
        }
    }

    // Fallback -- this should really never happen.
    assert(0);
    writer.beginMember("framebuffer");
    writer.beginObject();
    if (pSwapChain) {
        image::Image *image;
        image = getRenderTargetImage(pSwapChain);
        if (image) {
            writer.beginMember("SWAP_CHAIN");
            writer.writeImage(image);
            writer.endMember();
            delete image;
        }
    }
    writer.endObject();
    writer.endMember(); // framebuffer
}


std::string
getDXGIFormatName(DXGI_FORMAT format)
{
   switch (format) {
   case DXGI_FORMAT_UNKNOWN:
      return "DXGI_FORMAT_UNKNOWN";
   case DXGI_FORMAT_R32G32B32A32_TYPELESS:
      return "DXGI_FORMAT_R32G32B32A32_TYPELESS";
   case DXGI_FORMAT_R32G32B32A32_FLOAT:
      return "DXGI_FORMAT_R32G32B32A32_FLOAT";
   case DXGI_FORMAT_R32G32B32A32_UINT:
      return "DXGI_FORMAT_R32G32B32A32_UINT";
   case DXGI_FORMAT_R32G32B32A32_SINT:
      return "DXGI_FORMAT_R32G32B32A32_SINT";
   case DXGI_FORMAT_R32G32B32_TYPELESS:
      return "DXGI_FORMAT_R32G32B32_TYPELESS";
   case DXGI_FORMAT_R32G32B32_FLOAT:
      return "DXGI_FORMAT_R32G32B32_FLOAT";
   case DXGI_FORMAT_R32G32B32_UINT:
      return "DXGI_FORMAT_R32G32B32_UINT";
   case DXGI_FORMAT_R32G32B32_SINT:
      return "DXGI_FORMAT_R32G32B32_SINT";
   case DXGI_FORMAT_R16G16B16A16_TYPELESS:
      return "DXGI_FORMAT_R16G16B16A16_TYPELESS";
   case DXGI_FORMAT_R16G16B16A16_FLOAT:
      return "DXGI_FORMAT_R16G16B16A16_FLOAT";
   case DXGI_FORMAT_R16G16B16A16_UNORM:
      return "DXGI_FORMAT_R16G16B16A16_UNORM";
   case DXGI_FORMAT_R16G16B16A16_UINT:
      return "DXGI_FORMAT_R16G16B16A16_UINT";
   case DXGI_FORMAT_R16G16B16A16_SNORM:
      return "DXGI_FORMAT_R16G16B16A16_SNORM";
   case DXGI_FORMAT_R16G16B16A16_SINT:
      return "DXGI_FORMAT_R16G16B16A16_SINT";
   case DXGI_FORMAT_R32G32_TYPELESS:
      return "DXGI_FORMAT_R32G32_TYPELESS";
   case DXGI_FORMAT_R32G32_FLOAT:
      return "DXGI_FORMAT_R32G32_FLOAT";
   case DXGI_FORMAT_R32G32_UINT:
      return "DXGI_FORMAT_R32G32_UINT";
   case DXGI_FORMAT_R32G32_SINT:
      return "DXGI_FORMAT_R32G32_SINT";
   case DXGI_FORMAT_R32G8X24_TYPELESS:
      return "DXGI_FORMAT_R32G8X24_TYPELESS";
   case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
      return "DXGI_FORMAT_D32_FLOAT_S8X24_UINT";
   case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
      return "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS";
   case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
      return "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT";
   case DXGI_FORMAT_R10G10B10A2_TYPELESS:
      return "DXGI_FORMAT_R10G10B10A2_TYPELESS";
   case DXGI_FORMAT_R10G10B10A2_UNORM:
      return "DXGI_FORMAT_R10G10B10A2_UNORM";
   case DXGI_FORMAT_R10G10B10A2_UINT:
      return "DXGI_FORMAT_R10G10B10A2_UINT";
   case DXGI_FORMAT_R11G11B10_FLOAT:
      return "DXGI_FORMAT_R11G11B10_FLOAT";
   case DXGI_FORMAT_R8G8B8A8_TYPELESS:
      return "DXGI_FORMAT_R8G8B8A8_TYPELESS";
   case DXGI_FORMAT_R8G8B8A8_UNORM:
      return "DXGI_FORMAT_R8G8B8A8_UNORM";
   case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
      return "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB";
   case DXGI_FORMAT_R8G8B8A8_UINT:
      return "DXGI_FORMAT_R8G8B8A8_UINT";
   case DXGI_FORMAT_R8G8B8A8_SNORM:
      return "DXGI_FORMAT_R8G8B8A8_SNORM";
   case DXGI_FORMAT_R8G8B8A8_SINT:
      return "DXGI_FORMAT_R8G8B8A8_SINT";
   case DXGI_FORMAT_R16G16_TYPELESS:
      return "DXGI_FORMAT_R16G16_TYPELESS";
   case DXGI_FORMAT_R16G16_FLOAT:
      return "DXGI_FORMAT_R16G16_FLOAT";
   case DXGI_FORMAT_R16G16_UNORM:
      return "DXGI_FORMAT_R16G16_UNORM";
   case DXGI_FORMAT_R16G16_UINT:
      return "DXGI_FORMAT_R16G16_UINT";
   case DXGI_FORMAT_R16G16_SNORM:
      return "DXGI_FORMAT_R16G16_SNORM";
   case DXGI_FORMAT_R16G16_SINT:
      return "DXGI_FORMAT_R16G16_SINT";
   case DXGI_FORMAT_R32_TYPELESS:
      return "DXGI_FORMAT_R32_TYPELESS";
   case DXGI_FORMAT_D32_FLOAT:
      return "DXGI_FORMAT_D32_FLOAT";
   case DXGI_FORMAT_R32_FLOAT:
      return "DXGI_FORMAT_R32_FLOAT";
   case DXGI_FORMAT_R32_UINT:
      return "DXGI_FORMAT_R32_UINT";
   case DXGI_FORMAT_R32_SINT:
      return "DXGI_FORMAT_R32_SINT";
   case DXGI_FORMAT_R24G8_TYPELESS:
      return "DXGI_FORMAT_R24G8_TYPELESS";
   case DXGI_FORMAT_D24_UNORM_S8_UINT:
      return "DXGI_FORMAT_D24_UNORM_S8_UINT";
   case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
      return "DXGI_FORMAT_R24_UNORM_X8_TYPELESS";
   case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
      return "DXGI_FORMAT_X24_TYPELESS_G8_UINT";
   case DXGI_FORMAT_R8G8_TYPELESS:
      return "DXGI_FORMAT_R8G8_TYPELESS";
   case DXGI_FORMAT_R8G8_UNORM:
      return "DXGI_FORMAT_R8G8_UNORM";
   case DXGI_FORMAT_R8G8_UINT:
      return "DXGI_FORMAT_R8G8_UINT";
   case DXGI_FORMAT_R8G8_SNORM:
      return "DXGI_FORMAT_R8G8_SNORM";
   case DXGI_FORMAT_R8G8_SINT:
      return "DXGI_FORMAT_R8G8_SINT";
   case DXGI_FORMAT_R16_TYPELESS:
      return "DXGI_FORMAT_R16_TYPELESS";
   case DXGI_FORMAT_R16_FLOAT:
      return "DXGI_FORMAT_R16_FLOAT";
   case DXGI_FORMAT_D16_UNORM:
      return "DXGI_FORMAT_D16_UNORM";
   case DXGI_FORMAT_R16_UNORM:
      return "DXGI_FORMAT_R16_UNORM";
   case DXGI_FORMAT_R16_UINT:
      return "DXGI_FORMAT_R16_UINT";
   case DXGI_FORMAT_R16_SNORM:
      return "DXGI_FORMAT_R16_SNORM";
   case DXGI_FORMAT_R16_SINT:
      return "DXGI_FORMAT_R16_SINT";
   case DXGI_FORMAT_R8_TYPELESS:
      return "DXGI_FORMAT_R8_TYPELESS";
   case DXGI_FORMAT_R8_UNORM:
      return "DXGI_FORMAT_R8_UNORM";
   case DXGI_FORMAT_R8_UINT:
      return "DXGI_FORMAT_R8_UINT";
   case DXGI_FORMAT_R8_SNORM:
      return "DXGI_FORMAT_R8_SNORM";
   case DXGI_FORMAT_R8_SINT:
      return "DXGI_FORMAT_R8_SINT";
   case DXGI_FORMAT_A8_UNORM:
      return "DXGI_FORMAT_A8_UNORM";
   case DXGI_FORMAT_R1_UNORM:
      return "DXGI_FORMAT_R1_UNORM";
   case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
      return "DXGI_FORMAT_R9G9B9E5_SHAREDEXP";
   case DXGI_FORMAT_R8G8_B8G8_UNORM:
      return "DXGI_FORMAT_R8G8_B8G8_UNORM";
   case DXGI_FORMAT_G8R8_G8B8_UNORM:
      return "DXGI_FORMAT_G8R8_G8B8_UNORM";
   case DXGI_FORMAT_BC1_TYPELESS:
      return "DXGI_FORMAT_BC1_TYPELESS";
   case DXGI_FORMAT_BC1_UNORM:
      return "DXGI_FORMAT_BC1_UNORM";
   case DXGI_FORMAT_BC1_UNORM_SRGB:
      return "DXGI_FORMAT_BC1_UNORM_SRGB";
   case DXGI_FORMAT_BC2_TYPELESS:
      return "DXGI_FORMAT_BC2_TYPELESS";
   case DXGI_FORMAT_BC2_UNORM:
      return "DXGI_FORMAT_BC2_UNORM";
   case DXGI_FORMAT_BC2_UNORM_SRGB:
      return "DXGI_FORMAT_BC2_UNORM_SRGB";
   case DXGI_FORMAT_BC3_TYPELESS:
      return "DXGI_FORMAT_BC3_TYPELESS";
   case DXGI_FORMAT_BC3_UNORM:
      return "DXGI_FORMAT_BC3_UNORM";
   case DXGI_FORMAT_BC3_UNORM_SRGB:
      return "DXGI_FORMAT_BC3_UNORM_SRGB";
   case DXGI_FORMAT_BC4_TYPELESS:
      return "DXGI_FORMAT_BC4_TYPELESS";
   case DXGI_FORMAT_BC4_UNORM:
      return "DXGI_FORMAT_BC4_UNORM";
   case DXGI_FORMAT_BC4_SNORM:
      return "DXGI_FORMAT_BC4_SNORM";
   case DXGI_FORMAT_BC5_TYPELESS:
      return "DXGI_FORMAT_BC5_TYPELESS";
   case DXGI_FORMAT_BC5_UNORM:
      return "DXGI_FORMAT_BC5_UNORM";
   case DXGI_FORMAT_BC5_SNORM:
      return "DXGI_FORMAT_BC5_SNORM";
   case DXGI_FORMAT_B5G6R5_UNORM:
      return "DXGI_FORMAT_B5G6R5_UNORM";
   case DXGI_FORMAT_B5G5R5A1_UNORM:
      return "DXGI_FORMAT_B5G5R5A1_UNORM";
   case DXGI_FORMAT_B8G8R8A8_UNORM:
      return "DXGI_FORMAT_B8G8R8A8_UNORM";
   case DXGI_FORMAT_B8G8R8X8_UNORM:
      return "DXGI_FORMAT_B8G8R8X8_UNORM";
   case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
      return "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM";
   case DXGI_FORMAT_B8G8R8A8_TYPELESS:
      return "DXGI_FORMAT_B8G8R8A8_TYPELESS";
   case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
      return "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB";
   case DXGI_FORMAT_B8G8R8X8_TYPELESS:
      return "DXGI_FORMAT_B8G8R8X8_TYPELESS";
   case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
      return "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB";
   case DXGI_FORMAT_BC6H_TYPELESS:
      return "DXGI_FORMAT_BC6H_TYPELESS";
   case DXGI_FORMAT_BC6H_UF16:
      return "DXGI_FORMAT_BC6H_UF16";
   case DXGI_FORMAT_BC6H_SF16:
      return "DXGI_FORMAT_BC6H_SF16";
   case DXGI_FORMAT_BC7_TYPELESS:
      return "DXGI_FORMAT_BC7_TYPELESS";
   case DXGI_FORMAT_BC7_UNORM:
      return "DXGI_FORMAT_BC7_UNORM";
   case DXGI_FORMAT_BC7_UNORM_SRGB:
      return "DXGI_FORMAT_BC7_UNORM_SRGB";
   case DXGI_FORMAT_AYUV:
      return "DXGI_FORMAT_AYUV";
   case DXGI_FORMAT_Y410:
      return "DXGI_FORMAT_Y410";
   case DXGI_FORMAT_Y416:
      return "DXGI_FORMAT_Y416";
   case DXGI_FORMAT_NV12:
      return "DXGI_FORMAT_NV12";
   case DXGI_FORMAT_P010:
      return "DXGI_FORMAT_P010";
   case DXGI_FORMAT_P016:
      return "DXGI_FORMAT_P016";
   case DXGI_FORMAT_420_OPAQUE:
      return "DXGI_FORMAT_420_OPAQUE";
   case DXGI_FORMAT_YUY2:
      return "DXGI_FORMAT_YUY2";
   case DXGI_FORMAT_Y210:
      return "DXGI_FORMAT_Y210";
   case DXGI_FORMAT_Y216:
      return "DXGI_FORMAT_Y216";
   case DXGI_FORMAT_NV11:
      return "DXGI_FORMAT_NV11";
   case DXGI_FORMAT_AI44:
      return "DXGI_FORMAT_AI44";
   case DXGI_FORMAT_IA44:
      return "DXGI_FORMAT_IA44";
   case DXGI_FORMAT_P8:
      return "DXGI_FORMAT_P8";
   case DXGI_FORMAT_A8P8:
      return "DXGI_FORMAT_A8P8";
   case DXGI_FORMAT_B4G4R4A4_UNORM:
      return "DXGI_FORMAT_B4G4R4A4_UNORM";
   case DXGI_FORMAT_FORCE_UINT:
      return "DXGI_FORMAT_FORCE_UINT";
   default:
      return "DXGI_ERROR_FORMAT";
   }
}

} /* namespace d3dstate */

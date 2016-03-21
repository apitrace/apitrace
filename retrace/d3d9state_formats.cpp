/**************************************************************************
 *
 * Copyright 2014 VMware, Inc
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


#include "d3d9state.hpp"

#include <assert.h>
#include <stdint.h>

#include "image.hpp"

#ifdef HAVE_DXGI
#include "dxgistate.hpp"
#endif


std::ostream &
operator << (std::ostream &os, D3DFORMAT Format) {
    if (Format >= 0 && Format < 256) {
        return os << (unsigned)Format;
    } else {
        char ch0 = Format;
        char ch1 = Format >> 8;
        char ch2 = Format >> 16;
        char ch3 = Format >> 24;
        return os << "'" << ch0 << ch1 << ch2 << ch3 << "'";
    }
    return os;
}


namespace d3dstate {


#ifdef HAVE_DXGI

/**
 * http://msdn.microsoft.com/en-us/library/windows/desktop/cc308051.aspx
 */
static DXGI_FORMAT
convertFormatDXGI(D3DFORMAT Format)
{
    switch ((int)Format) {
    case D3DFMT_UNKNOWN:
        return DXGI_FORMAT_UNKNOWN;
    case D3DFMT_A8R8G8B8:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case D3DFMT_X8R8G8B8:
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    case D3DFMT_R5G6B5:
        return DXGI_FORMAT_B5G6R5_UNORM;
    case D3DFMT_A1R5G5B5:
        return DXGI_FORMAT_B5G5R5A1_UNORM;
    case D3DFMT_A4R4G4B4:
        return DXGI_FORMAT_B4G4R4A4_UNORM;
    case D3DFMT_A8:
        return DXGI_FORMAT_A8_UNORM;
    case D3DFMT_A2B10G10R10:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case D3DFMT_A8B8G8R8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case D3DFMT_G16R16:
        return DXGI_FORMAT_R16G16_UNORM;
    case D3DFMT_A16B16G16R16:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    case D3DFMT_L8:
        return DXGI_FORMAT_R8_UNORM;
    case D3DFMT_V8U8:
        return DXGI_FORMAT_R8G8_SNORM;
    case D3DFMT_Q8W8V8U8:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    case D3DFMT_V16U16:
        return DXGI_FORMAT_R16G16_SNORM;
    case D3DFMT_R8G8_B8G8:
        return DXGI_FORMAT_G8R8_G8B8_UNORM;
    case D3DFMT_G8R8_G8B8:
        return DXGI_FORMAT_R8G8_B8G8_UNORM;
    case D3DFMT_DXT1:
        return DXGI_FORMAT_BC1_UNORM;
    case D3DFMT_DXT2:
        return DXGI_FORMAT_BC2_UNORM;
    case D3DFMT_DXT3:
        return DXGI_FORMAT_BC2_UNORM;
    case D3DFMT_DXT4:
        return DXGI_FORMAT_BC3_UNORM;
    case D3DFMT_DXT5:
        return DXGI_FORMAT_BC3_UNORM;
    case D3DFMT_D16:
    case D3DFMT_D16_LOCKABLE:
        return DXGI_FORMAT_D16_UNORM;
    case D3DFMT_D32F_LOCKABLE:
        return DXGI_FORMAT_D32_FLOAT;
    case D3DFMT_D24S8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case D3DFMT_L16:
        return DXGI_FORMAT_R16_UNORM;
    case D3DFMT_INDEX16:
        return DXGI_FORMAT_R16_UINT;
    case D3DFMT_INDEX32:
        return DXGI_FORMAT_R32_UINT;
    case D3DFMT_Q16W16V16U16:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
    case D3DFMT_R16F:
        return DXGI_FORMAT_R16_FLOAT;
    case D3DFMT_G16R16F:
        return DXGI_FORMAT_R16G16_FLOAT;
    case D3DFMT_A16B16G16R16F:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case D3DFMT_R32F:
        return DXGI_FORMAT_R32_FLOAT;
    case D3DFMT_G32R32F:
        return DXGI_FORMAT_R32G32_FLOAT;
    case D3DFMT_A32B32G32R32F:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case D3DFMT_ATI1N:
        return DXGI_FORMAT_BC4_UNORM;
    case D3DFMT_ATI2N:
        return DXGI_FORMAT_BC5_UNORM;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}


image::Image *
ConvertImageDXGI(D3DFORMAT SrcFormat,
                 void *SrcData,
                 INT SrcPitch,
                 UINT Width, UINT Height)
{
    DXGI_FORMAT Format = convertFormatDXGI(SrcFormat);
    if (Format == DXGI_FORMAT_UNKNOWN) {
        return NULL;
    }

    assert(SrcPitch >= 0);

    return ConvertImage(Format, SrcData, SrcPitch, Width, Height);
}

#endif /* HAVE_DXGI */


image::Image *
ConvertImage(D3DFORMAT SrcFormat,
             void *SrcData,
             INT SrcPitch,
             UINT Width, UINT Height)
{
    image::Image *image;

    unsigned numChannels;
    image::ChannelType channelType;
    switch (SrcFormat) {
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_R5G6B5:
        numChannels = 3;
        channelType = image::TYPE_UNORM8;
        break;
    case D3DFMT_D16:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D24S8:
    case D3DFMT_D24X8:
    case D3DFMT_D32F_LOCKABLE:
    case D3DFMT_DF16:
    case D3DFMT_DF24:
    case D3DFMT_INTZ:
        numChannels = 1;
        channelType = image::TYPE_FLOAT;
        break;
    default:
#ifdef HAVE_DXGI
        image = ConvertImageDXGI(SrcFormat, SrcData, SrcPitch, Width, Height);
        if (image) {
            return image;
        }
#endif

        std::cerr << "warning: unsupported D3DFORMAT " << SrcFormat << "\n";
        return NULL;
    }

    image = new image::Image(Width, Height, numChannels, true, channelType);
    if (!image) {
        return NULL;
    }

    const unsigned char *src;
    unsigned char *dst;

    dst = image->start();
    src = (const unsigned char *)SrcData;
    for (unsigned y = 0; y < Height; ++y) {
        switch (SrcFormat) {
        case D3DFMT_R5G6B5:
            for (unsigned x = 0; x < Width; ++x) {
                uint32_t pixel = ((const uint16_t *)src)[x];
                dst[3*x + 0] = (( pixel        & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
                dst[3*x + 1] = (((pixel >>  5) & 0x3f) * (2*0xff) + 0x3f) / (2*0x3f);
                dst[3*x + 2] = (( pixel >> 11        ) * (2*0xff) + 0x1f) / (2*0x1f);
            }
            break;
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:
            for (unsigned x = 0; x < Width; ++x) {
                dst[3*x + 0] = src[4*x + 2];
                dst[3*x + 1] = src[4*x + 1];
                dst[3*x + 2] = src[4*x + 0];
            }
            break;
        case D3DFMT_D16:
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_DF16:
            for (unsigned x = 0; x < Width; ++x) {
                ((float *)dst)[x] = ((const uint16_t *)src)[x] * (1.0f / 0xffff);
            }
            break;
        case D3DFMT_D24S8:
        case D3DFMT_D24X8:
        case D3DFMT_DF24:
        case D3DFMT_INTZ:
            for (unsigned x = 0; x < Width; ++x) {
                ((float *)dst)[x] = ((const uint32_t *)src)[x] * (1.0 / 0xffffff);
            }
            break;
        case D3DFMT_D32F_LOCKABLE:
            memcpy(dst, src, Width * sizeof(float));
            break;
        default:
            assert(0);
            break;
        }

        src += SrcPitch;
        dst += image->stride();
    }

    return image;
}


} /* namespace d3dstate */

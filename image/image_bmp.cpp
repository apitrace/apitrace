/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright 2008-2010 VMware, Inc.
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
#include <stdint.h>

#include <fstream>

#include "image.hpp"


namespace image {


#pragma pack(push,2)
struct FileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};
#pragma pack(pop)

struct InfoHeader {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

struct Pixel {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbAlpha;
};


bool
Image::writeBMP(const char *filename) const {
    assert(channels == 4);
    assert(channelType == TYPE_UNORM8);

    struct FileHeader bmfh;
    struct InfoHeader bmih;
    unsigned x, y;

    bmfh.bfType = 0x4d42;
    bmfh.bfSize = 14 + 40 + height*width*4;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = 14 + 40;

    bmih.biSize = 40;
    bmih.biWidth = width;
    bmih.biHeight = height;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = 0;
    bmih.biSizeImage = height*width*4;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;

    std::ofstream stream(filename, std::ofstream::binary);

    if (!stream) {
        return false;
    }

    stream.write((const char *)&bmfh, 14);
    stream.write((const char *)&bmih, 40);

    unsigned stride = width*4;

    if (flipped) {
        for (y = 0; y < height; ++y) {
            const unsigned char *ptr = pixels + y * stride;
            for (x = 0; x < width; ++x) {
                struct Pixel pixel;
                pixel.rgbRed   = ptr[x*4 + 0];
                pixel.rgbGreen = ptr[x*4 + 1];
                pixel.rgbBlue  = ptr[x*4 + 2];
                pixel.rgbAlpha = ptr[x*4 + 3];
                stream.write((const char *)&pixel, 4);
            }
        }
    } else {
        y = height;
        while (y--) {
            const unsigned char *ptr = pixels + y * stride;
            for (x = 0; x < width; ++x) {
                struct Pixel pixel;
                pixel.rgbRed   = ptr[x*4 + 0];
                pixel.rgbGreen = ptr[x*4 + 1];
                pixel.rgbBlue  = ptr[x*4 + 2];
                pixel.rgbAlpha = ptr[x*4 + 3];
                stream.write((const char *)&pixel, 4);
            }
        }
    }

    stream.close();

    return true;
}


} /* namespace image */

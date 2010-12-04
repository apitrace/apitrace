/**************************************************************************
 *
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


#include <png.h>

#include <stdint.h>

#include <fstream>

#include "image.hpp"


namespace Image {


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

    if (!stream)
        return false;

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

bool
Image::writePNG(const char *filename) const {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;

    /* Open the file */
    fp = fopen(filename, "wb");
    if (!fp)
        goto no_fp;

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        goto no_png;

    /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr,  NULL);
        goto no_png;
    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
     * error handling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        goto no_png;
    }

#if 1
    png_init_io(png_ptr, fp);
#else
    png_set_write_fn(png_ptr, (void *)user_io_ptr, user_write_fn,
        user_IO_flush_function);
#endif

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    png_write_info(png_ptr, info_ptr);

    if (!flipped) {
        for (unsigned y = 0; y < height; ++y) {
            png_byte *row = (png_byte *)(pixels + y*width*4);
            png_write_rows(png_ptr, &row, 1);
        }
    } else {
        unsigned y = height;
        while (y--) {
            png_byte *row = (png_byte *)(pixels + y*width*4);
            png_write_rows(png_ptr, &row, 1);
        }
    }

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
    return true;

no_png:
    fclose(fp);
no_fp:
    return false;
}

} /* namespace Image */

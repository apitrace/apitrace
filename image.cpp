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

#include <math.h>
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

    fp = fopen(filename, "wb");
    if (!fp)
        goto no_fp;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        goto no_png;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr,  NULL);
        goto no_png;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        goto no_png;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    png_write_info(png_ptr, info_ptr);

    if (!flipped) {
        for (unsigned y = 0; y < height; ++y) {
            png_bytep row = (png_bytep)(pixels + y*width*4);
            png_write_rows(png_ptr, &row, 1);
        }
    } else {
        unsigned y = height;
        while (y--) {
            png_bytep row = (png_bytep)(pixels + y*width*4);
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


Image *
readPNG(const char *filename)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info;
    Image *image;

    fp = fopen(filename, "rb");
    if (!fp)
        goto no_fp;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        goto no_png;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        goto no_png;
    }

    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        goto no_png;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        goto no_png;
    }

    png_init_io(png_ptr, fp);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, compression_type, filter_method;

    png_get_IHDR(png_ptr, info_ptr,
                 &width, &height,
                 &bit_depth, &color_type, &interlace_type,
                 &compression_type, &filter_method);

    image = new Image(width, height);
    if (!image)
        goto no_image;

    /* Convert to RGBA8 */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    for (unsigned y = 0; y < height; ++y) {
        png_bytep row = (png_bytep)(image->pixels + y*width*4);
        png_read_row(png_ptr, row, NULL);
    }

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return image;

no_image:
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
no_png:
    fclose(fp);
no_fp:
    return NULL;
}


double Image::compare(Image &ref)
{
    if (width != ref.width ||
        height != ref.height) {
        return 0.0;
    }

    const unsigned char *pSrc = start();
    const unsigned char *pRef = ref.start();

    unsigned long long error = 0;
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned  x = 0; x < width*4; ++x) {
            int delta = pSrc[x] - pRef[x];
            error += delta*delta;
        }

        pSrc += stride();
        pRef += ref.stride();
    }

    double numerator = error*2 + 1;
    double denominator = height*width*4ULL*255ULL*255ULL*2;
    double quotient = numerator/denominator;

    // Precision in bits
    double precision = -log(quotient)/log(2.0);

    return precision;
}



} /* namespace Image */

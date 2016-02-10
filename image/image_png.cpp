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


#include <zlib.h>
#include <png.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <fstream>

#include "image.hpp"


namespace image {


static const int png_compression_level = Z_BEST_SPEED;


static void
pngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    std::ostream *os = (std::ostream *) png_get_io_ptr(png_ptr);
    os->write((const char *)data, length);
}


static inline uint8_t
floatToUnorm8(float c)
{
    if (c <= 0.0f) {
        return 0;
    }
    if (c >= 1.0f) {
        return 255;
    }
    return c * 255.0f + 0.5f;
}


static inline uint8_t
floatToSRGB(float c)
{
    if (c <= 0.0f) {
        return 0;
    }
    if (c >= 1.0f) {
        return 255;
    }
    if (c <= 0.0031308f) {
        c *= 12.92f;
    } else {
        c = 1.055f * powf(c, 1.0f/2.4f) - 0.055f;
    }
    return c * 255.0f + 0.5f;
}


bool
Image::writePNG(std::ostream &os, bool strip_alpha) const
{
    png_structp png_ptr;
    png_infop info_ptr;
    int color_type;

    switch (channels) {
    case 4:
        color_type = strip_alpha ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;
        break;
    case 3:
        color_type = PNG_COLOR_TYPE_RGB;
        break;
    case 2:
        color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
        break;
    case 1:
        color_type = PNG_COLOR_TYPE_GRAY;
        break;
    default:
        assert(0);
        goto no_png;
    }

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

    png_set_write_fn(png_ptr, &os, pngWriteCallback, NULL);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                 color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_compression_level(png_ptr, png_compression_level);

    png_write_info(png_ptr, info_ptr);

    if (channels == 4 && strip_alpha) {
        png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
    }

    switch (channelType) {
    case TYPE_UNORM8:
        for (const unsigned char *row = start(); row != end(); row += stride()) {
            png_write_rows(png_ptr, (png_bytepp) &row, 1);
        }
        break;
    case TYPE_FLOAT:
        png_bytep rowUnorm8 = new png_byte[width * channels];
        for (const unsigned char *row = start(); row != end(); row += stride()) {
            const float *rowFloat = (const float *)row;
            for (unsigned x = 0, i = 0; x < width; ++x) {
                for (unsigned channel = 0; channel < channels; ++channel, ++i) {
                    float c = rowFloat[i];
                    bool srgb = channels >= 3 && channel < 3;
                    rowUnorm8[i] = srgb ? floatToSRGB(c) : floatToUnorm8(c);
                }
            }
            png_write_rows(png_ptr, (png_bytepp) &rowUnorm8, 1);
        }
        delete [] rowUnorm8;
        break;
    }

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return true;

no_png:
    return false;
}


bool
Image::writePNG(const char *filename, bool strip_alpha) const
{
    std::ofstream os(filename, std::ofstream::binary);
    if (!os) {
        return false;
    }
    return writePNG(os, strip_alpha);
}


static void
pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    std::istream *os = (std::istream *) png_get_io_ptr(png_ptr);
    os->read((char *)data, length);
}


Image *
readPNG(std::istream &is)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info;
    unsigned channels;
    Image *image;

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

    png_set_read_fn(png_ptr, &is, pngReadCallback);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, compression_type, filter_method;

    png_get_IHDR(png_ptr, info_ptr,
                 &width, &height,
                 &bit_depth, &color_type, &interlace_type,
                 &compression_type, &filter_method);

    /* Convert to RGBA8 */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    channels = png_get_channels(png_ptr, info_ptr);
    image = new Image(width, height, channels);
    if (!image)
        goto no_image;

    assert(png_get_rowbytes(png_ptr, info_ptr) == width*channels);
    for (unsigned y = 0; y < height; ++y) {
        png_bytep row = (png_bytep)(image->pixels + y*width*channels);
        png_read_row(png_ptr, row, NULL);
    }

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    return image;

no_image:
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
no_png:
    return NULL;
}

Image *
readPNG(const char *filename)
{
    std::ifstream is(filename, std::ifstream::binary);
    if (!is) {
        return NULL;
    }
    return readPNG(filename);
}


} /* namespace image */

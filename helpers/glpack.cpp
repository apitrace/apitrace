/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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

/*
 * Pixel pack state for readbacks.
 */

#include "glproc.hpp"
#include "glpack.hpp"


static const struct {
    GLenum pname;
    GLfloat identity;
} pixelTransferParams[] = {
    { GL_MAP_COLOR, GL_FALSE },
    { GL_MAP_STENCIL, GL_FALSE },
    { GL_INDEX_SHIFT, 0.0f },
    { GL_INDEX_OFFSET, 0.0f },
    { GL_RED_SCALE, 1.0f },
    { GL_RED_BIAS, 0.0f },
    { GL_GREEN_SCALE, 1.0f },
    { GL_GREEN_BIAS, 0.0f },
    { GL_BLUE_SCALE, 1.0f },
    { GL_BLUE_BIAS, 0.0f },
    { GL_ALPHA_SCALE, 1.0f },
    { GL_ALPHA_BIAS, 0.0f },
    { GL_DEPTH_SCALE, 1.0f },
    { GL_DEPTH_BIAS, 0.0f },
};


void
PixelTransferState::save(const glfeatures::Features &features)
{
    static_assert(sizeof pixelTransferParams / sizeof pixelTransferParams[0] == numParams,
                  "one value per parameter");

    enabled = features.pixel_transfer;
    if (!enabled) {
        return;
    }
    for (unsigned i = 0; i < numParams; ++i) {
        values[i] = pixelTransferParams[i].identity;
        _glGetFloatv(pixelTransferParams[i].pname, &values[i]);
    }
}

void
PixelTransferState::reset(Setter set) const
{
    if (!enabled) {
        return;
    }
    for (unsigned i = 0; i < numParams; ++i) {
        if (values[i] != pixelTransferParams[i].identity) {
            set(pixelTransferParams[i].pname, pixelTransferParams[i].identity);
        }
    }
}

void
PixelTransferState::restore(Setter set) const
{
    if (!enabled) {
        return;
    }
    for (unsigned i = 0; i < numParams; ++i) {
        if (values[i] != pixelTransferParams[i].identity) {
            set(pixelTransferParams[i].pname, values[i]);
        }
    }
}


PixelPackState::PixelPackState(const glfeatures::Features &features)
{
    desktop = !features.ES;
    subimage = features.pack_subimage;
    texture_3d = features.texture_3d;
    pixel_buffer_object = features.pixel_buffer_object;
    color_buffer_float = features.ARB_color_buffer_float;

    // Start with default state
    pack_alignment = 4;
    pack_image_height = 0;
    pack_lsb_first = GL_FALSE;
    pack_row_length = 0;
    pack_skip_images = 0;
    pack_skip_pixels = 0;
    pack_skip_rows = 0;
    pack_swap_bytes = GL_FALSE;
    pixel_pack_buffer_binding = 0;
    clamp_read_color = GL_FIXED_ONLY;

    // Get current state
    _glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
    if (subimage) {
        _glGetIntegerv(GL_PACK_ROW_LENGTH, &pack_row_length);
        _glGetIntegerv(GL_PACK_SKIP_PIXELS, &pack_skip_pixels);
        _glGetIntegerv(GL_PACK_SKIP_ROWS, &pack_skip_rows);
    }
    if (desktop) {
        _glGetIntegerv(GL_PACK_LSB_FIRST, &pack_lsb_first);
        _glGetIntegerv(GL_PACK_SWAP_BYTES, &pack_swap_bytes);
        if (texture_3d) {
            _glGetIntegerv(GL_PACK_IMAGE_HEIGHT, &pack_image_height);
            _glGetIntegerv(GL_PACK_SKIP_IMAGES, &pack_skip_images);
        }
    }
    if (pixel_buffer_object) {
        _glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pixel_pack_buffer_binding);
    }
    if (color_buffer_float) {
        _glGetIntegerv(GL_CLAMP_READ_COLOR, &clamp_read_color);
    }
    pixel_transfer.save(features);

    // Reset state for compact images
    _glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if (subimage) {
        _glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        _glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        _glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    }
    if (desktop) {
        _glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
        _glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
        if (texture_3d) {
            _glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
            _glPixelStorei(GL_PACK_SKIP_IMAGES, 0);
        }
    }
    if (pixel_buffer_object) {
        _glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }
    if (color_buffer_float) {
        _glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    }
    pixel_transfer.reset(_glPixelTransferf);
}

PixelPackState::~PixelPackState() {
    _glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment);
    if (subimage) {
        _glPixelStorei(GL_PACK_ROW_LENGTH, pack_row_length);
        _glPixelStorei(GL_PACK_SKIP_PIXELS, pack_skip_pixels);
        _glPixelStorei(GL_PACK_SKIP_ROWS, pack_skip_rows);
    }
    if (desktop) {
        _glPixelStorei(GL_PACK_LSB_FIRST, pack_lsb_first);
        _glPixelStorei(GL_PACK_SWAP_BYTES, pack_swap_bytes);
        if (texture_3d) {
            _glPixelStorei(GL_PACK_IMAGE_HEIGHT, pack_image_height);
            _glPixelStorei(GL_PACK_SKIP_IMAGES, pack_skip_images);
        }
    }
    if (pixel_buffer_object) {
        _glBindBuffer(GL_PIXEL_PACK_BUFFER, pixel_pack_buffer_binding);
    }
    if (color_buffer_float) {
        _glClampColor(GL_CLAMP_READ_COLOR, clamp_read_color);
    }
    pixel_transfer.restore(_glPixelTransferf);
}

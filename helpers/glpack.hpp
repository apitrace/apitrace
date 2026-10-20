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

#pragma once

#include "glimports.hpp"
#include "glfeatures.hpp"


/*
 * Tightly packed pack state, outside any pack buffer and without read color
 * clamping, for reading pixels back into client memory.  The previous state
 * is restored at the end of the scope.
 */
class PixelPackState
{
public:
    PixelPackState(const glfeatures::Features &features);

    ~PixelPackState();

private:
    bool desktop;
    bool subimage;
    bool texture_3d;
    bool pixel_buffer_object;
    bool color_buffer_float;

    // Pack state
    GLint pack_alignment;
    GLint pack_image_height;
    GLint pack_lsb_first;
    GLint pack_row_length;
    GLint pack_skip_images;
    GLint pack_skip_pixels;
    GLint pack_skip_rows;
    GLint pack_swap_bytes;
    GLint pixel_pack_buffer_binding;

    GLint clamp_read_color;
};

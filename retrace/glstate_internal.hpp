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

#ifndef _GLSTATE_INTERNAL_HPP_
#define _GLSTATE_INTERNAL_HPP_


#include "glimports.hpp"


class JSONWriter;


namespace glstate {


struct Context
{
    bool ES;

    bool ARB_draw_buffers;

    Context(void);

    void
    resetPixelPackState(void);

    void
    restorePixelPackState(void);

private:
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
};


void dumpBoolean(JSONWriter &json, GLboolean value);

void dumpEnum(JSONWriter &json, GLenum pname);

void dumpParameters(JSONWriter &json, Context &context);

void dumpShadersUniforms(JSONWriter &json, Context &context);

void dumpTextures(JSONWriter &json, Context &context);

void dumpFramebuffer(JSONWriter &json, Context &context);


} /* namespace glstate */


#endif /* _GLSTATE_INTERNAL_HPP_ */

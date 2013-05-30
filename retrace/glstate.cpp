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


#include <string.h>

#include <algorithm>
#include <iostream>

#include "image.hpp"
#include "json.hpp"
#include "glproc.hpp"
#include "glsize.hpp"
#include "glstate.hpp"
#include "glstate_internal.hpp"


namespace glstate {


Context::Context(void) {
    memset(this, 0, sizeof *this);

    const char *version = (const char *)glGetString(GL_VERSION);
    if (version) {
        if (version[0] == 'O' &&
            version[1] == 'p' &&
            version[2] == 'e' &&
            version[3] == 'n' &&
            version[4] == 'G' &&
            version[5] == 'L' &&
            version[6] == ' ' &&
            version[7] == 'E' &&
            version[8] == 'S' &&
            (version[9] == ' ' || version[9] == '-')) {
            ES = true;
        }
    }

    ARB_draw_buffers = !ES;

    // TODO: Check extensions we use below
}

void
Context::resetPixelPackState(void) {
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

    // Get current state
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
    if (!ES) {
        glGetIntegerv(GL_PACK_IMAGE_HEIGHT, &pack_image_height);
        glGetIntegerv(GL_PACK_LSB_FIRST, &pack_lsb_first);
        glGetIntegerv(GL_PACK_ROW_LENGTH, &pack_row_length);
        glGetIntegerv(GL_PACK_SKIP_IMAGES, &pack_skip_images);
        glGetIntegerv(GL_PACK_SKIP_PIXELS, &pack_skip_pixels);
        glGetIntegerv(GL_PACK_SKIP_ROWS, &pack_skip_rows);
        glGetIntegerv(GL_PACK_SWAP_BYTES, &pack_swap_bytes);
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pixel_pack_buffer_binding);
    }

    // Reset state for compact images
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if (!ES) {
        glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
        glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_IMAGES, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }
}

void
Context::restorePixelPackState(void) {
    glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment);
    if (!ES) {
        glPixelStorei(GL_PACK_IMAGE_HEIGHT, pack_image_height);
        glPixelStorei(GL_PACK_LSB_FIRST, pack_lsb_first);
        glPixelStorei(GL_PACK_ROW_LENGTH, pack_row_length);
        glPixelStorei(GL_PACK_SKIP_IMAGES, pack_skip_images);
        glPixelStorei(GL_PACK_SKIP_PIXELS, pack_skip_pixels);
        glPixelStorei(GL_PACK_SKIP_ROWS, pack_skip_rows);
        glPixelStorei(GL_PACK_SWAP_BYTES, pack_swap_bytes);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pixel_pack_buffer_binding);
    }
}


static const GLenum bindings[] = {
    GL_DRAW_BUFFER,
    GL_READ_BUFFER,
    GL_PIXEL_PACK_BUFFER_BINDING,
    GL_PIXEL_UNPACK_BUFFER_BINDING,
    GL_TEXTURE_BINDING_1D,
    GL_TEXTURE_BINDING_1D_ARRAY,
    GL_TEXTURE_BINDING_2D,
    GL_TEXTURE_BINDING_2D_ARRAY,
    GL_TEXTURE_BINDING_2D_MULTISAMPLE,
    GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
    GL_TEXTURE_BINDING_3D,
    GL_TEXTURE_BINDING_RECTANGLE,
    GL_TEXTURE_BINDING_CUBE_MAP,
    GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,
    GL_DRAW_FRAMEBUFFER_BINDING,
    GL_READ_FRAMEBUFFER_BINDING,
    GL_RENDERBUFFER_BINDING,
    GL_DRAW_BUFFER0,
    GL_DRAW_BUFFER1,
    GL_DRAW_BUFFER2,
    GL_DRAW_BUFFER3,
    GL_DRAW_BUFFER4,
    GL_DRAW_BUFFER5,
    GL_DRAW_BUFFER6,
    GL_DRAW_BUFFER7,
};


#define NUM_BINDINGS sizeof(bindings)/sizeof(bindings[0])


void dumpCurrentContext(std::ostream &os)
{
    JSONWriter json(os);

#ifndef NDEBUG
    GLint old_bindings[NUM_BINDINGS];
    for (unsigned i = 0; i < NUM_BINDINGS; ++i) {
        old_bindings[i] = 0;
        glGetIntegerv(bindings[i], &old_bindings[i]);
    }
#endif

    Context context;

    dumpParameters(json, context);
    dumpShadersUniforms(json, context);
    dumpTextures(json, context);
    dumpFramebuffer(json, context);

#ifndef NDEBUG
    for (unsigned i = 0; i < NUM_BINDINGS; ++i) {
        GLint new_binding = 0;
        glGetIntegerv(bindings[i], &new_binding);
        if (new_binding != old_bindings[i]) {
            std::cerr << "warning: " << enumToString(bindings[i]) << " was clobbered\n";
        }
    }
#endif

}


} /* namespace glstate */

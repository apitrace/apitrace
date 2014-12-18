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
#include "glproc.hpp"
#include "image.hpp"


class JSONWriter;


namespace glstate {


struct Context
{
    bool ES;
    bool core;

    bool ARB_draw_buffers;
    bool ARB_sampler_objects;
    bool KHR_debug;
    bool EXT_debug_label;

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


static inline void
flushErrors(void) {
    while (glGetError() != GL_NO_ERROR) {
    }
}



const char *
formatToString(GLenum internalFormat);


struct InternalFormatDesc
{
    GLenum internalFormat;

    /* The external format/type that matches the internalFormat exactly, or GL_NONE. */
    GLenum format;
    GLenum type;
};


const InternalFormatDesc &
getInternalFormatDesc(GLenum internalFormat);

void
chooseReadBackFormat(const InternalFormatDesc &formatDesc, GLenum &format, GLenum &type);

void
getImageFormat(GLenum format, GLenum type,
               GLuint &channels, image::ChannelType &channelType);


bool
isGeometryShaderBound(Context &context);


void dumpBoolean(JSONWriter &json, GLboolean value);

void dumpEnum(JSONWriter &json, GLenum pname);

char *
getObjectLabel(Context &context, GLenum identifier, GLuint name);

void dumpObjectLabel(JSONWriter &json, Context &context, GLenum identifier, GLuint name, const char *member);

void dumpParameters(JSONWriter &json, Context &context);

void dumpShadersUniforms(JSONWriter &json, Context &context);

void dumpTextures(JSONWriter &json, Context &context);

void dumpFramebuffer(JSONWriter &json, Context &context);


} /* namespace glstate */


#endif /* _GLSTATE_INTERNAL_HPP_ */

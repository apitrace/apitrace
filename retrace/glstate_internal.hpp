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

#pragma once


#include <stdint.h>

#include "glimports.hpp"
#include "glproc.hpp"
#include "image.hpp"


class StateWriter;


namespace glstate {


struct Context
{
    unsigned ES:1;
    unsigned core:1;

    unsigned ARB_draw_buffers:1;
    unsigned ARB_sampler_objects:1;
    unsigned ARB_get_program_binary:1;
    unsigned KHR_debug:1;
    unsigned EXT_debug_label:1;

    Context(void);
};


class PixelPackState
{
public:
    PixelPackState(const Context & context);

    ~PixelPackState();

private:
    bool ES;

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

    /* The appropriate read type for dumping. */
    GLenum readType;
};


const InternalFormatDesc &
getInternalFormatDesc(GLenum internalFormat);

void
chooseReadBackFormat(const InternalFormatDesc &formatDesc, GLenum &format, GLenum &type);

void
getImageFormat(GLenum format, GLenum type,
               GLuint &channels, image::ChannelType &channelType);


// Abstract base class for pixel format conversion
class PixelFormat
{
public:
    virtual ~PixelFormat() {}

    // Size in bytes
    virtual size_t
    size(void) const = 0;

    // Unpack a span of pixels
    virtual void
    unpackSpan(const uint8_t *inSpan, float *outSpan, unsigned width) const = 0;
};


const PixelFormat *
getPixelFormat(GLenum internalFormat);


/**
 * Helper class to temporarily bind a buffer to the specified target until
 * control leaves the declaration scope.
 */
class BufferBinding
{
private:
    GLenum target;
    GLuint buffer;
    GLuint prevBuffer;

public:
    BufferBinding(GLenum _target, GLuint _buffer);

    ~BufferBinding();
};


/**
 * Helper class to temporarily map a buffer (if necessary), and unmap when
 * destroyed.
 */
class BufferMapping
{
    GLuint target;
    GLuint buffer;
    GLvoid *map_pointer;
    bool unmap;

public:
    BufferMapping();

    GLvoid *
    map(GLenum _target, GLuint _buffer);

    ~BufferMapping();
};


bool
isGeometryShaderBound(Context &context);


void dumpBoolean(StateWriter &writer, GLboolean value);

void dumpEnum(StateWriter &writer, GLenum pname);

char *
getObjectLabel(Context &context, GLenum identifier, GLuint name);

void dumpObjectLabel(StateWriter &writer, Context &context, GLenum identifier, GLuint name, const char *member);

void dumpParameters(StateWriter &writer, Context &context);

void dumpShadersUniforms(StateWriter &writer, Context &context);

void dumpTextures(StateWriter &writer, Context &context);

void dumpFramebuffer(StateWriter &writer, Context &context);


} /* namespace glstate */



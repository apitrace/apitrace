/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright 2010 VMware, Inc.
 * Copyright 2004 IBM Corporation
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * AUTHORS,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **************************************************************************/

/*
 * Auxiliary functions to compute the size of array/blob arguments, depending.
 */

#ifndef _GL_SIZE_HPP_
#define _GL_SIZE_HPP_


#include <string.h>

#include <algorithm>

#include "os.hpp"
#include "glimports.hpp"


static inline size_t
_gl_type_size(GLenum type)
{
    switch (type) {
    case GL_BOOL:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_2_BYTES:
    case GL_HALF_FLOAT:
        return 2;
    case GL_3_BYTES:
        return 3;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
    case GL_4_BYTES:
    case GL_FIXED:
        return 4;
    case GL_DOUBLE:
        return 8;
    default:
        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
        return 0;
    }
}

static inline void
_gl_uniform_size(GLenum type, GLenum &elemType, GLint &numCols, GLint &numRows) {
    numCols = 1;
    numRows = 1;

    switch (type) {
    case GL_FLOAT:
        elemType = GL_FLOAT;
        break;
    case GL_FLOAT_VEC2:
        elemType = GL_FLOAT;
        numCols = 2;
        break;
    case GL_FLOAT_VEC3:
        elemType = GL_FLOAT;
        numCols = 3;
        break;
    case GL_FLOAT_VEC4:
        elemType = GL_FLOAT;
        numCols = 4;
        break;
    case GL_DOUBLE:
        elemType = GL_DOUBLE;
        break;
    case GL_DOUBLE_VEC2:
        elemType = GL_DOUBLE;
        numCols = 2;
        break;
    case GL_DOUBLE_VEC3:
        elemType = GL_DOUBLE;
        numCols = 3;
        break;
    case GL_DOUBLE_VEC4:
        elemType = GL_DOUBLE;
        numCols = 4;
        break;
    case GL_INT:
        elemType = GL_INT;
        break;
    case GL_INT_VEC2:
        elemType = GL_INT;
        numCols = 2;
        break;
    case GL_INT_VEC3:
        elemType = GL_INT;
        numCols = 3;
        break;
    case GL_INT_VEC4:
        elemType = GL_INT;
        numCols = 4;
        break;
    case GL_UNSIGNED_INT:
        elemType = GL_UNSIGNED_INT;
        break;
    case GL_UNSIGNED_INT_VEC2:
        elemType = GL_UNSIGNED_INT;
        numCols = 2;
        break;
    case GL_UNSIGNED_INT_VEC3:
        elemType = GL_UNSIGNED_INT;
        numCols = 3;
        break;
    case GL_UNSIGNED_INT_VEC4:
        elemType = GL_UNSIGNED_INT;
        numCols = 4;
        break;
    case GL_BOOL:
        elemType = GL_BOOL;
        break;
    case GL_BOOL_VEC2:
        elemType = GL_BOOL;
        numCols = 2;
        break;
    case GL_BOOL_VEC3:
        elemType = GL_BOOL;
        numCols = 3;
        break;
    case GL_BOOL_VEC4:
        elemType = GL_BOOL;
        numCols = 4;
        break;
    case GL_FLOAT_MAT2:
        elemType = GL_FLOAT;
        numCols = 2;
        numRows = 2;
        break;
    case GL_FLOAT_MAT3:
        elemType = GL_FLOAT;
        numCols = 3;
        numRows = 3;
        break;
    case GL_FLOAT_MAT4:
        elemType = GL_FLOAT;
        numCols = 4;
        numRows = 4;
        break;
    case GL_FLOAT_MAT2x3:
        elemType = GL_FLOAT;
        numCols = 2;
        numRows = 3;
        break;
    case GL_FLOAT_MAT2x4:
        elemType = GL_FLOAT;
        numCols = 2;
        numRows = 4;
        break;
    case GL_FLOAT_MAT3x2:
        elemType = GL_FLOAT;
        numCols = 3;
        numRows = 2;
        break;
    case GL_FLOAT_MAT3x4:
        elemType = GL_FLOAT;
        numCols = 3;
        numRows = 4;
        break;
    case GL_FLOAT_MAT4x2:
        elemType = GL_FLOAT;
        numCols = 4;
        numRows = 2;
        break;
    case GL_FLOAT_MAT4x3:
        elemType = GL_FLOAT;
        numCols = 4;
        numRows = 3;
        break;
    case GL_DOUBLE_MAT2:
        elemType = GL_DOUBLE;
        numCols = 2;
        numRows = 2;
        break;
    case GL_DOUBLE_MAT3:
        elemType = GL_DOUBLE;
        numCols = 3;
        numRows = 3;
        break;
    case GL_DOUBLE_MAT4:
        elemType = GL_DOUBLE;
        numCols = 4;
        numRows = 4;
        break;
    case GL_DOUBLE_MAT2x3:
        elemType = GL_DOUBLE;
        numCols = 2;
        numRows = 3;
        break;
    case GL_DOUBLE_MAT2x4:
        elemType = GL_DOUBLE;
        numCols = 2;
        numRows = 4;
        break;
    case GL_DOUBLE_MAT3x2:
        elemType = GL_DOUBLE;
        numCols = 3;
        numRows = 2;
        break;
    case GL_DOUBLE_MAT3x4:
        elemType = GL_DOUBLE;
        numCols = 3;
        numRows = 4;
        break;
    case GL_DOUBLE_MAT4x2:
        elemType = GL_DOUBLE;
        numCols = 4;
        numRows = 2;
        break;
    case GL_DOUBLE_MAT4x3:
        elemType = GL_DOUBLE;
        numCols = 4;
        numRows = 3;
        break;
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_CUBE_MAP_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        elemType = GL_INT;
        break;
    default:
        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
        elemType = GL_NONE;
        numCols = 0;
        numRows = 0;
        return;
    }
}
    
static inline size_t
_glArrayPointer_size(GLint size, GLenum type, GLsizei stride, GLsizei count)
{
    if (!count) {
        return 0;
    }

    if (size == GL_BGRA) {
        size = 4; 
    }

    if (size > 4) {
        os::log("apitrace: warning: %s: unexpected size 0x%04X\n", __FUNCTION__, size);
    }

    size_t elementSize = size*_gl_type_size(type);
    if (!stride) {
        stride = (GLsizei)elementSize;
    }

    return stride*(count - 1) + elementSize;
}

#define _glVertexPointer_size(size, type, stride, count) _glArrayPointer_size(size, type, stride, count)
#define _glNormalPointer_size(type, stride, count) _glArrayPointer_size(3, type, stride, count)
#define _glColorPointer_size(size, type, stride, count) _glArrayPointer_size(size, type, stride, count)
#define _glIndexPointer_size(type, stride, count) _glArrayPointer_size(1, type, stride, count)
#define _glTexCoordPointer_size(size, type, stride, count) _glArrayPointer_size(size, type, stride, count)
#define _glEdgeFlagPointer_size(stride, count) _glArrayPointer_size(1, GL_BOOL, stride, count)
#define _glFogCoordPointer_size(type, stride, count) _glArrayPointer_size(1, type, stride, count)
#define _glSecondaryColorPointer_size(size, type, stride, count) _glArrayPointer_size(size, type, stride, count)
#define _glVertexAttribPointer_size(size, type, normalized, stride, count) _glArrayPointer_size(size, type, stride, count)
#define _glVertexAttribPointerARB_size(size, type, normalized, stride, count) _glArrayPointer_size(size, type, stride, count)
#define _glVertexAttribPointerNV_size(size, type, stride, count) _glArrayPointer_size(size, type, stride, count)

/**
 * Same as glGetIntegerv, but passing the result in the return value.
 */
static inline GLint
_glGetInteger(GLenum pname) {
    GLint param = 0;
    _glGetIntegerv(pname, &param);
    return param;
}

static inline GLint
_element_array_buffer_binding(void) {
    return _glGetInteger(GL_ELEMENT_ARRAY_BUFFER_BINDING);
}

static inline GLuint
_glDrawArrays_count(GLint first, GLsizei count)
{
    if (!count) {
        return 0;
    }
    return first + count;
}

#define _glDrawArraysEXT_count _glDrawArrays_count

/* Forward declaration for definition in gltrace.py */
void
_shadow_glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size,
                              GLvoid *data);

static inline GLuint
_glDrawElementsBaseVertex_count(GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
{
    GLvoid *temp = 0;

    if (!count) {
        return 0;
    }

    GLint element_array_buffer = _element_array_buffer_binding();
    if (element_array_buffer) {
        // Read indices from index buffer object
        GLintptr offset = (GLintptr)indices;
        GLsizeiptr size = count*_gl_type_size(type);
        GLvoid *temp = malloc(size);
        if (!temp) {
            return 0;
        }
        memset(temp, 0, size);
        _shadow_glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, temp);
        indices = temp;
    } else {
        if (!indices) {
            return 0;
        }
    }

    GLuint maxindex = 0;
    GLsizei i;
    if (type == GL_UNSIGNED_BYTE) {
        const GLubyte *p = (const GLubyte *)indices;
        for (i = 0; i < count; ++i) {
            if (p[i] > maxindex) {
                maxindex = p[i];
            }
        }
    } else if (type == GL_UNSIGNED_SHORT) {
        const GLushort *p = (const GLushort *)indices;
        for (i = 0; i < count; ++i) {
            if (p[i] > maxindex) {
                maxindex = p[i];
            }
        }
    } else if (type == GL_UNSIGNED_INT) {
        const GLuint *p = (const GLuint *)indices;
        for (i = 0; i < count; ++i) {
            if (p[i] > maxindex) {
                maxindex = p[i];
            }
        }
    } else {
        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
    }

    if (element_array_buffer) {
        free(temp);
    }

    maxindex += basevertex;

    return maxindex + 1;
}

#define _glDrawRangeElementsBaseVertex_count(start, end, count, type, indices, basevertex) _glDrawElementsBaseVertex_count(count, type, indices, basevertex)

#define _glDrawElements_count(count, type, indices) _glDrawElementsBaseVertex_count(count, type, indices, 0);
#define _glDrawRangeElements_count(start, end, count, type, indices) _glDrawElements_count(count, type, indices)
#define _glDrawRangeElementsEXT_count _glDrawRangeElements_count

/* FIXME take in consideration instancing */
#define _glDrawArraysInstanced_count(first, count, primcount) _glDrawArrays_count(first, count)
#define _glDrawElementsInstanced_count(count, type, indices, primcount) _glDrawElements_count(count, type, indices)
#define _glDrawElementsInstancedBaseVertex_count(count, type, indices, primcount, basevertex) _glDrawElementsBaseVertex_count(count, type, indices, basevertex)
#define _glDrawRangeElementsInstanced_count(start, end, count, type, indices, primcount) _glDrawRangeElements_count(start, end, count, type, indices)
#define _glDrawRangeElementsInstancedBaseVertex_count(start, end, count, type, indices, primcount, basevertex) _glDrawRangeElementsBaseVertex_count(start, end, count, type, indices, basevertex)

#define _glDrawArraysInstancedBaseInstance_count(first, count, primcount, baseinstance) _glDrawArrays_count(first, count)
#define _glDrawElementsInstancedBaseInstance_count(count, type, indices, primcount, baseinstance) _glDrawElements_count(count, type, indices)
#define _glDrawElementsInstancedBaseVertexBaseInstance_count(count, type, indices, primcount, basevertex, baseinstance) _glDrawElementsBaseVertex_count(count, type, indices, basevertex)

#define _glDrawArraysInstancedARB_count _glDrawArraysInstanced_count
#define _glDrawElementsInstancedARB_count _glDrawElementsInstanced_count
#define _glDrawArraysInstancedEXT_count _glDrawArraysInstanced_count
#define _glDrawElementsInstancedEXT_count _glDrawElementsInstanced_count

static inline GLuint
_glDrawArraysIndirect_count(const GLvoid *indirect) {
    os::log("apitrace: warning: %s: unsupported\n", __FUNCTION__);
    return 0;
}

static inline GLuint
_glDrawElementsIndirect_count(GLenum type, const GLvoid *indirect) {
    os::log("apitrace: warning: %s: unsupported\n", __FUNCTION__);
    return 0;
}

static inline GLuint
_glMultiDrawArrays_count(const GLint *first, const GLsizei *count, GLsizei drawcount) {
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < drawcount; ++draw) {
        GLuint _count_draw = _glDrawArrays_count(first[draw], count[draw]);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}

static inline GLuint
_glMultiDrawElements_count(const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount) {
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < drawcount; ++draw) {
        GLuint _count_draw = _glDrawElements_count(count[draw], type, indices[draw]);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}

static inline GLuint
_glMultiDrawElementsBaseVertex_count(const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount, const GLint * basevertex) {
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < drawcount; ++draw) {
        GLuint _count_draw = _glDrawElementsBaseVertex_count(count[draw], type, indices[draw], basevertex[draw]);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}

#define _glMultiDrawArraysEXT_count _glMultiDrawArrays_count
#define _glMultiDrawElementsEXT_count _glMultiDrawElements_count

#define _glMultiModeDrawArraysIBM_count(first, count, drawcount, modestride) _glMultiDrawArrays_count(first, count, drawcount)
#define _glMultiModeDrawElementsIBM_count(count, type, indices, drawcount, modestride) _glMultiDrawElements_count(count, type, (const GLvoid **)indices, drawcount)


static inline size_t
_glCallLists_size(GLsizei n, GLenum type)
{
    return n*_gl_type_size(type);
}

static inline size_t
_glMap1d_size(GLenum target, GLint stride, GLint order)
{
    if (order < 1) {
        return 0;
    }

    GLint channels;
    switch (target) {
    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
        channels = 1;
        break;
    case GL_MAP1_TEXTURE_COORD_2:
        channels = 2;
        break;
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
    case GL_MAP1_VERTEX_3:
        channels = 3;
        break;
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
    case GL_MAP1_VERTEX_4:
        channels = 4;
        break;
    default:
        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, target);
        return 0;
    }

    if (stride < channels) {
        return 0;
    }

    return channels + stride * (order - 1);
}

#define _glMap1f_size _glMap1d_size

static inline size_t
_glMap2d_size(GLenum target, GLint ustride, GLint uorder, GLint vstride, GLint vorder)
{
    if (uorder < 1 || vorder < 1) {
        return 0;
    }

    GLint channels;
    switch (target) {
    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        channels = 1;
        break;
    case GL_MAP2_TEXTURE_COORD_2:
        channels = 2;
        break;
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
    case GL_MAP2_VERTEX_3:
        channels = 3;
        break;
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
    case GL_MAP2_VERTEX_4:
        channels = 4;
        break;
    default:
        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, target);
        return 0;
    }

    if (ustride < channels || vstride < channels) {
        return 0;
    }

    return channels + 
           ustride * (uorder - 1) +
           vstride * (vorder - 1);
}

#define _glMap2f_size _glMap2d_size

static inline unsigned
_gl_format_channels(GLenum format) {
    switch (format) {
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_INTENSITY:
    case GL_LUMINANCE:
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
        return 1;
    case GL_DEPTH_STENCIL:
    case GL_LUMINANCE_ALPHA:
    case GL_RG:
    case GL_HILO_NV:
    case GL_DSDT_NV:
        return 2;
    case GL_RGB:
    case GL_BGR:
    case GL_DSDT_MAG_NV:
        return 3;
    case GL_RGBA:
    case GL_BGRA:
    case GL_ABGR_EXT:
    case GL_CMYK_EXT:
    case GL_DSDT_MAG_VIB_NV:
        return 4;
    case GL_CMYKA_EXT:
        return 5;
    default:
        os::log("apitrace: warning: %s: unexpected format GLenum 0x%04X\n", __FUNCTION__, format);
        return 0;
    }
}

template<class X>
static inline bool
_is_pot(X x) {
    return (x & (x - 1)) == 0;
}

template<class X, class Y>
static inline X
_align(X x, Y y) {
    return (x + (y - 1)) & ~(y - 1);
}

static inline void
_gl_format_size(GLenum format, GLenum type,
                unsigned & bits_per_element, unsigned & bits_per_pixel)
{
    unsigned num_channels = _gl_format_channels(format);

    switch (type) {
    case GL_BITMAP:
        bits_per_pixel = bits_per_element = 1;
        break;
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        bits_per_element = 8;
        bits_per_pixel = bits_per_element * num_channels;
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
        bits_per_element = 16;
        bits_per_pixel = bits_per_element * num_channels;
        break;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        bits_per_element = 32;
        bits_per_pixel = bits_per_element * num_channels;
        break;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
        bits_per_pixel = bits_per_element = 8;
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_8_8_MESA:
    case GL_UNSIGNED_SHORT_8_8_REV_MESA:
        bits_per_pixel = bits_per_element = 16;
        break;
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_24_8:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_S8_S8_8_8_NV:
    case GL_UNSIGNED_INT_8_8_S8_S8_REV_NV:
        bits_per_pixel = bits_per_element = 32;
        break;
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        bits_per_pixel = bits_per_element = 64;
        break;
    default:
        os::log("apitrace: warning: %s: unexpected type GLenum 0x%04X\n", __FUNCTION__, type);
        bits_per_pixel = bits_per_element = 0;
        break;
    }
}

static inline size_t
_glClearBufferData_size(GLenum format, GLenum type) {
    unsigned bits_per_element;
    unsigned bits_per_pixel;
    _gl_format_size(format, type, bits_per_element, bits_per_pixel);
    return (bits_per_pixel + 7)/8;
}

static inline size_t
_gl_image_size(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth, GLboolean has_unpack_subimage) {

    unsigned bits_per_element;
    unsigned bits_per_pixel;
    _gl_format_size(format, type, bits_per_element, bits_per_pixel);

    GLint alignment = 4;
    GLint row_length = 0;
    GLint image_height = 0;
    GLint skip_rows = 0;
    GLint skip_pixels = 0;
    GLint skip_images = 0;

    _glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
    if (has_unpack_subimage) {
        _glGetIntegerv(GL_UNPACK_ROW_LENGTH,   &row_length);
        _glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT, &image_height);
        _glGetIntegerv(GL_UNPACK_SKIP_ROWS,    &skip_rows);
        _glGetIntegerv(GL_UNPACK_SKIP_PIXELS,  &skip_pixels);
        _glGetIntegerv(GL_UNPACK_SKIP_IMAGES,  &skip_images);
    }

    if (row_length <= 0) {
        row_length = width;
    }

    size_t row_stride = (row_length*bits_per_pixel + 7)/8;

    if ((bits_per_element == 1*8 ||
         bits_per_element == 2*8 ||
         bits_per_element == 4*8 ||
         bits_per_element == 8*8) &&
        (GLint)bits_per_element < alignment*8) {
        row_stride = _align(row_stride, alignment);
    }

    if (image_height <= 0) {
        image_height = height;
    }

    /* XXX: GL_UNPACK_IMAGE_HEIGHT and GL_UNPACK_SKIP_IMAGES should probably
     * not be considered for pixel rectangles. */

    size_t image_stride = image_height*row_stride;

    size_t size = depth*image_stride;

    size += (skip_pixels*bits_per_pixel + 7)/8;
    size += skip_rows*row_stride;
    size += skip_images*image_stride;

    return size;
}

// note that can_unpack_subimage() is generated by gltrace.py
#define _glTexImage3D_size(format, type, width, height, depth) _gl_image_size(format, type, width, height, depth, can_unpack_subimage())
#define _glTexImage2D_size(format, type, width, height)        _gl_image_size(format, type, width, height, 1, can_unpack_subimage())
#define _glTexImage1D_size(format, type, width)                _gl_image_size(format, type, width, 1, 1, can_unpack_subimage())

#define _glTexSubImage3D_size(format, type, width, height, depth) _glTexImage3D_size(format, type, width, height, depth)
#define _glTexSubImage2D_size(format, type, width, height)        _glTexImage2D_size(format, type, width, height)
#define _glTexSubImage1D_size(format, type, width)                _glTexImage1D_size(format, type, width)

#define _glTexImage3DEXT_size _glTexImage3D_size
#define _glTexImage2DEXT_size _glTexImage2D_size
#define _glTexImage1DEXT_size _glTexImage1D_size
#define _glTexSubImage3DEXT_size _glTexSubImage3D_size
#define _glTexSubImage2DEXT_size _glTexSubImage2D_size
#define _glTexSubImage1DEXT_size _glTexSubImage1D_size

#define _glTextureImage3DEXT_size _glTexImage3D_size
#define _glTextureImage2DEXT_size _glTexImage2D_size
#define _glTextureImage1DEXT_size _glTexImage1D_size
#define _glTextureSubImage3DEXT_size _glTexSubImage3D_size
#define _glTextureSubImage2DEXT_size _glTexSubImage2D_size
#define _glTextureSubImage1DEXT_size _glTexSubImage1D_size

#define _glMultiTexImage3DEXT_size _glTexImage3D_size
#define _glMultiTexImage2DEXT_size _glTexImage2D_size
#define _glMultiTexImage1DEXT_size _glTexImage1D_size
#define _glMultiTexSubImage3DEXT_size _glTexSubImage3D_size
#define _glMultiTexSubImage2DEXT_size _glTexSubImage2D_size
#define _glMultiTexSubImage1DEXT_size _glTexSubImage1D_size

#define _glDrawPixels_size(format, type, width, height) _glTexImage2D_size(format, type, width, height)
#define _glConvolutionFilter1D_size(format, type, width) _glTexImage1D_size(format, type, width)
#define _glConvolutionFilter2D_size(format, type, width, height) _glTexImage2D_size(format, type, width, height)
#define _glColorTable_size(format, type, width) _glTexImage1D_size(format, type, width)
#define _glColorSubTable_size(format, type, count) _glColorTable_size(format, type, count)

#define _glBitmap_size(width, height) _glTexImage2D_size(GL_COLOR_INDEX, GL_BITMAP, width, height)
#define _glPolygonStipple_size() _glBitmap_size(32, 32)

static inline size_t
_glClearBuffer_size(GLenum buffer)
{
    switch (buffer) {
    case GL_COLOR:
    case GL_FRONT:
    case GL_BACK:
    case GL_LEFT:
    case GL_RIGHT:
    case GL_FRONT_AND_BACK:
        return 4;
    case GL_DEPTH:
    case GL_STENCIL:
        return 1;
    default:
        os::log("apitrace: warning: %s: unexpected buffer GLenum 0x%04X\n", __FUNCTION__, buffer);
        return 0;
    }
}

/* 
 * attribute list, terminated by the given terminator.
 */
template<class T>
static inline size_t
_AttribList_size(const T *pAttribList, const T terminator = static_cast<T>(0))
{
    size_t size = 0;

    if (pAttribList) {
        do {
            ++size;
        } while (*pAttribList++ != terminator);
    }

    return size;
}


/*
 * (key, value) attribute list, terminated by the given terminator.
 */
template<class T>
static inline size_t
_AttribPairList_size(const T *pAttribList, const T terminator = static_cast<T>(0))
{
    size_t size = 0;

    if (pAttribList) {
        while (pAttribList[size] != terminator) {
            size += 2;
        }
        // terminator also counts
        ++size;
    }

    return size;
}


#endif /* _GL_SIZE_HPP_ */

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
#include <assert.h>

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

/**
 * Same as glGetVertexAttribiv, but passing the result in the return value.
 */
static inline GLint
_glGetVertexAttribi(GLuint index, GLenum pname) {
    GLint param = 0;
    _glGetVertexAttribiv(index, pname, &param);
    return param;
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

typedef struct {
    GLuint count;
    GLuint primCount;
    GLuint first;
    GLuint baseInstance;
} DrawArraysIndirectCommand;

static inline GLuint
_glMultiDrawArraysIndirect_count(const GLvoid *indirect, GLsizei drawcount, GLsizei stride) {
    const DrawArraysIndirectCommand *cmd;
    GLvoid *temp = 0;

    if (drawcount <= 0) {
        return 0;
    }

    if (stride == 0) {
        stride = sizeof *cmd;
    }

    GLint draw_indirect_buffer = _glGetInteger(GL_DRAW_INDIRECT_BUFFER_BINDING);
    if (draw_indirect_buffer) {
        // Read commands from indirect buffer object
        GLintptr offset = (GLintptr)indirect;
        GLsizeiptr size = sizeof *cmd + (drawcount - 1) * stride;
        GLvoid *temp = malloc(size);
        if (!temp) {
            return 0;
        }
        memset(temp, 0, size);
        _glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, offset, size, temp);
        indirect = temp;
    } else {
        if (!indirect) {
            return 0;
        }
    }

    GLuint count = 0;
    for (GLsizei i = 0; i < drawcount; ++i) {
        cmd = (const DrawArraysIndirectCommand *)((const GLbyte *)indirect + i * stride);

        GLuint count_i = _glDrawArraysInstancedBaseInstance_count(
            cmd->first,
            cmd->count,
            cmd->primCount,
            cmd->baseInstance
        );

        count = std::max(count, count_i);
    }

    if (draw_indirect_buffer) {
        free(temp);
    }

    return count;
}

static inline GLuint
_glDrawArraysIndirect_count(const GLvoid *indirect) {
    return _glMultiDrawArraysIndirect_count(indirect, 1, 0);
}

typedef struct {
    GLuint count;
    GLuint primCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
} DrawElementsIndirectCommand;

static inline GLuint
_glMultiDrawElementsIndirect_count(GLenum type, const GLvoid *indirect, GLsizei drawcount, GLsizei stride) {
    const DrawElementsIndirectCommand *cmd;
    GLvoid *temp = 0;

    if (drawcount <= 0) {
        return 0;
    }

    if (stride == 0) {
        stride = sizeof *cmd;
    }

    GLint draw_indirect_buffer = _glGetInteger(GL_DRAW_INDIRECT_BUFFER_BINDING);
    if (draw_indirect_buffer) {
        // Read commands from indirect buffer object
        GLintptr offset = (GLintptr)indirect;
        GLsizeiptr size = sizeof *cmd + (drawcount - 1) * stride;
        GLvoid *temp = malloc(size);
        if (!temp) {
            return 0;
        }
        memset(temp, 0, size);
        _glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, offset, size, temp);
        indirect = temp;
    } else {
        if (!indirect) {
            return 0;
        }
    }

    cmd = (const DrawElementsIndirectCommand *)indirect;

    GLuint count = 0;
    for (GLsizei i = 0; i < drawcount; ++i) {
        cmd = (const DrawElementsIndirectCommand *)((const GLbyte *)indirect + i * stride);

        GLuint count_i = _glDrawElementsInstancedBaseVertexBaseInstance_count(
            cmd->count,
            type,
            (GLvoid *)(uintptr_t)(cmd->firstIndex * _gl_type_size(type)),
            cmd->primCount,
            cmd->baseVertex,
            cmd->baseInstance
        );

        count = std::max(count, count_i);
    }

    if (draw_indirect_buffer) {
        free(temp);
    }

    return count;
}

static inline GLuint
_glDrawElementsIndirect_count(GLenum type, const GLvoid *indirect) {
    return _glMultiDrawElementsIndirect_count(type, indirect, 1, 0);
}

#define _glMultiDrawArraysIndirectAMD_count _glMultiDrawArraysIndirect_count
#define _glMultiDrawElementsIndirectAMD_count _glMultiDrawElementsIndirect_count

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

/**
 * Number of channels in this format.
 *
 * That is, the number of elements per pixel when this format is passed with a
 * to DrawPixels, ReadPixels, TexImage*, TexSubImage*, GetTexImage, etc.
 */
static inline unsigned
_gl_format_channels(GLenum format) {
    switch (format) {
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_RED_INTEGER:
    case GL_GREEN:
    case GL_GREEN_INTEGER:
    case GL_BLUE:
    case GL_BLUE_INTEGER:
    case GL_ALPHA:
    case GL_ALPHA_INTEGER:
    case GL_INTENSITY:
    case GL_LUMINANCE:
    case GL_LUMINANCE_INTEGER_EXT:
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
        return 1;
    case GL_DEPTH_STENCIL:
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE_ALPHA_INTEGER_EXT:
    case GL_RG:
    case GL_RG_INTEGER:
    case GL_422_EXT: // (luminance, chrominance)
    case GL_422_REV_EXT: // (luminance, chrominance)
    case GL_422_AVERAGE_EXT: // (luminance, chrominance)
    case GL_422_REV_AVERAGE_EXT: // (luminance, chrominance)
    case GL_HILO_NV: // (hi, lo)
    case GL_DSDT_NV: // (ds, dt)
    case GL_YCBCR_422_APPLE: // (luminance, chroma)
    case GL_RGB_422_APPLE: // (G, B) on even pixels, (G, R) on odd pixels
    case GL_YCRCB_422_SGIX: // (Y, [Cb,Cr])
        return 2;
    case GL_RGB:
    case GL_RGB_INTEGER:
    case GL_BGR:
    case GL_BGR_INTEGER:
    case GL_DSDT_MAG_NV: // (ds, dt, magnitude)
    case GL_YCRCB_444_SGIX: // (Cb, Y, Cr)
        return 3;
    case GL_RGBA:
    case GL_RGBA_INTEGER:
    case GL_BGRA:
    case GL_BGRA_INTEGER:
    case GL_ABGR_EXT:
    case GL_CMYK_EXT:
    case GL_DSDT_MAG_VIB_NV: // (ds, dt, magnitude, vibrance)
        return 4;
    case GL_CMYKA_EXT:
        return 5;
    case GL_FORMAT_SUBSAMPLE_24_24_OML:
    case GL_FORMAT_SUBSAMPLE_244_244_OML:
        // requires UNSIGNED_INT_10_10_10_2, so this value will be ignored
        return 0;
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

    size_t image_stride = image_height*row_stride;

    /*
     * We can't just do
     *
     *   size = depth*image_stride
     *
     * here as that could result in reading beyond the end of the buffer when
     * selecting sub-rectangles via GL_UNPACK_SKIP_*.
     */
    size_t size = (width*bits_per_pixel + 7)/8;
    if (height > 1) {
        size += (height - 1)*row_stride;
    }
    if (depth > 1) {
        size += (depth - 1)*image_stride;
    }

    /* XXX: GL_UNPACK_IMAGE_HEIGHT and GL_UNPACK_SKIP_IMAGES should probably
     * not be considered for pixel rectangles. */

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

static inline size_t
_glPath_coords_size(GLsizei numCoords, GLenum coordType)
{
    switch (coordType) {
    case GL_BYTE:
        return numCoords * sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:
        return numCoords * sizeof(GLubyte);
    case GL_SHORT:
        return numCoords * sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
        return numCoords * sizeof(GLushort);
    case GL_FLOAT:
        return numCoords * sizeof(GLfloat);
    default:
        return 0;
    }
}

static inline size_t
_glPath_fontName_size(GLenum fontTarget, const void *fontName)
{
    switch (fontTarget) {
    case GL_STANDARD_FONT_NAME_NV:
    case GL_SYSTEM_FONT_NAME_NV:
    case GL_FILE_NAME_NV:
        {
            // Include +1 to copy nul terminator.
            GLsizei size = GLsizei(strlen(reinterpret_cast<const char*>(fontName))+1);
            return size;
        }
    default:
        return 0;
    }
}

static inline size_t
_glPath_chardcodes_size(GLsizei numGlyphs, GLenum type)
{
    GLsizei bytes_per_glyph;
    switch (type) {
    case GL_FLOAT:
    case GL_INT:
        bytes_per_glyph = 4;
        break;
    case GL_BYTE:
        bytes_per_glyph = 1;
        break;
    case GL_SHORT:
        bytes_per_glyph = 2;
        break;
    default:
        return 0;
    }
    return bytes_per_glyph*numGlyphs;
}

static GLsizei floatPerTransformList(GLenum transformType)
{
    switch (transformType) {
    case GL_NONE:
        return 0;
    case GL_TRANSLATE_X_NV:
    case GL_TRANSLATE_Y_NV:
        return 1;
    case GL_TRANSLATE_2D_NV:
        return 2;
    case GL_TRANSLATE_3D_NV:
        return 3;
    case GL_AFFINE_2D_NV:
    case GL_TRANSPOSE_AFFINE_2D_NV:
        return 6;
    case GL_PROJECTIVE_2D_NV:
    case GL_TRANSPOSE_PROJECTIVE_2D_NV:
        return 9;
    case GL_AFFINE_3D_NV:
    case GL_TRANSPOSE_AFFINE_3D_NV:
        return 12;
    case GL_PROJECTIVE_3D_NV:
    case GL_TRANSPOSE_PROJECTIVE_3D_NV:
        return 16;
    default:
        return 0;
    }
}

static inline size_t
_gl_transformType_size(GLenum transformType)
{
    return floatPerTransformList(transformType)*sizeof(GLfloat);
}

static inline size_t
_gl_transformType_size(GLsizei numPaths, GLenum transformType)
{
    return numPaths*floatPerTransformList(transformType)*sizeof(GLfloat);
}

static size_t valuesPerPathParameter(GLenum pname)
{
    switch (pname) {
    case GL_PATH_FILL_MODE_NV:
    case GL_PATH_FILL_MASK_NV:
    case GL_PATH_FILL_COVER_MODE_NV:
    case GL_PATH_STROKE_WIDTH_NV:
    case GL_PATH_INITIAL_END_CAP_NV:
    case GL_PATH_TERMINAL_END_CAP_NV:
    case GL_PATH_JOIN_STYLE_NV:
    case GL_PATH_MITER_LIMIT_NV:
    case GL_PATH_INITIAL_DASH_CAP_NV:
    case GL_PATH_TERMINAL_DASH_CAP_NV:
    case GL_PATH_DASH_OFFSET_NV:
    case GL_PATH_DASH_OFFSET_RESET_NV:
    case GL_PATH_CLIENT_LENGTH_NV:
    case GL_PATH_STROKE_COVER_MODE_NV:
    case GL_PATH_STROKE_MASK_NV:
    case GL_PATH_STROKE_OVERSAMPLE_COUNT_NV:
    case GL_PATH_SAMPLE_QUALITY_NV:
    case GL_PATH_END_CAPS_NV:  // not valid for glGetPathParameter
    case GL_PATH_DASH_CAPS_NV: // not valid for glGetPathParameter
        return 1;
    default:
        return 0;
    }
}

static inline size_t
_gl_PathParameter_size(GLenum pname)
{
    return valuesPerPathParameter(pname);
}

// See RFC-3629 "UTF-8, a transformation format of ISO 10646"
// http://www.rfc-editor.org/rfc/rfc3629.txt
// http://rfc-ref.org/RFC-TEXTS/3629/index.html
static bool
__glPathGetCodePointUTF8(const void *&utf_string,
                         GLuint &code_point)
{
    const GLubyte *p = reinterpret_cast<const GLubyte*>(utf_string);
    GLubyte c0 = p[0];
    if ((c0 & 0x80) == 0x00) {
        // Zero continuation (0 to 127)
        code_point = c0;
        assert(code_point <= 127);
        p += 1;
    } else {
        GLubyte c1 = p[1];
        if ((c1 & 0xC0) != 0x80) {
            // Stop processing the UTF byte sequence early.
            return false;
        }
        if ((c0 & 0xE0) == 0xC0) {
            // One contination (128 to 2047)
            code_point = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
            if (code_point < 128) {
                return false;
            }
            assert(code_point >= 128 && code_point <= 2047);
            p += 2;
        } else {
            GLubyte c2 = p[2];
            if ((c2 & 0xC0) != 0x80) {
                // Stop processing the UTF byte sequence early.
                return false;
            }
            if ((c0 & 0xF0) == 0xE0) {
                // Two continuation (2048 to 55295 and 57344 to 65535)
                code_point = ((c0 & 0x0F) << 12) |
                             ((c1 & 0x3F) << 6) |
                              (c2 & 0x3F);
                // "The definition of UTF-8 prohibits encoding character numbers between
                // U+D800 and U+DFFF, which are reserved for use with the UTF-16
                // encoding form (as surrogate pairs) and do not directly represent
                // characters."
                // 0xD800 = 55,296
                // 0xDFFF = 57,343
                if ((code_point >= 55296) && (code_point <= 57343)) {
                    // Stop processing the UTF byte sequence early.
                    return false;
                }
                if (code_point < 2048) {
                    return false;
                }
                assert(code_point >= 2048 && code_point <= 65535);
                assert(code_point < 55296 || code_point > 57343);
                p += 3;
            } else {
                GLubyte c3 = p[3];
                if ((c3 & 0xC0) != 0x80) {
                    // Stop processing the UTF byte sequence early.
                    return false;
                }
                if ((c0 & 0xF8) == 0xF0) {
                    // Three continuation (65536 to 1114111)
                    code_point = ((c0 & 0x07) << 18) |
                                 ((c1 & 0x3F) << 12) | 
                                 ((c2 & 0x3F) << 6) |
                                  (c3 & 0x3F);
                    if (code_point < 65536 && code_point > 1114111) {
                        return false;
                    }
                    assert(code_point >= 65536 && code_point <= 1114111);
                    p += 4;
                } else {
                    // Skip invalid or restricted encodings.
                    // Stop processing the UTF byte sequence early.
                    return false;
                }
            }
        }
    }
    utf_string = p;
    return true;
}

// See RFC-2781 "UTF-16, a transformation format of ISO 10646"
// http://rfc-ref.org/RFC-TEXTS/2781/index.html
// http://www.rfc-editor.org/rfc/rfc2781.txt
static bool
__glPathGetCodePointUTF16(const void *&utf_string,
                          GLuint &code_point)
{
    // Section 2.2 (Decoding UTF-16) of http://www.rfc-editor.org/rfc/rfc2781.txt
    // "Decoding of a single character from UTF-16 to an ISO 10646 character
    // value proceeds as follows."
    const GLushort *p = reinterpret_cast<const GLushort*>(utf_string);

    // "Let W1 be the next 16-bit integer in the
    // sequence of integers representing the text."
    GLushort W1 = p[0];
    // "1) If W1 < 0xD800 or W1 > 0xDFFF, the character value U is the value
    // of W1. Terminate."
    if ((W1 < 0xDB00) || (W1 > 0xDFFF)) {
        code_point = W1;
        p += 1;
    } else {
        // "2) Determine if W1 is between 0xD800 and 0xDBFF."
        bool between1 = (W1 >= 0xDB00) && (W1 <= 0xDBFF);
        if (!between1) {
            // "If not, the sequence
            // is in error and no valid character can be obtained using W1.
            // Terminate."
            return false;
        }
        // "Let W2 be the (eventual) next integer following W1."
        GLushort W2 = p[1];
        // DOES NOT APPLY because API provides character (not byte) count.
        // "3) If there is no W2 (that is, the sequence ends with W1), [Terminate]"

        // "... or if W2 is not between 0xDC00 and 0xDFFF, the sequence
        // is in error.  Terminate."
        bool between2 = (W2 >= 0xDC00) && (W2 <= 0xDFFF);
        if (!between2) {
            return false;
        }
        // "4) Construct a 20-bit unsigned integer U', taking the 10 low-order
        // bits of W1 as its 10 high-order bits and the 10 low-order bits of
        // W2 as its 10 low-order bits."
        code_point = ((W1 & 0x3FF) << 10) |
                      (W2 & 0x3FF);
        // "5) Add 0x10000 to U' to obtain the character value U. Terminate."
        code_point += 0x10000;
        p += 2;
    }
    utf_string = p;
    return true;
}

static size_t bytesOfSequence(GLsizei count, GLenum type, const GLvoid *sequence)
{
    GLsizei bytes_per_element;
    switch (type) {
    case GL_BYTE:
        bytes_per_element = sizeof(GLbyte);
        break;
    case GL_UNSIGNED_BYTE:
        bytes_per_element = sizeof(GLubyte);
        break;
    case GL_SHORT:
        bytes_per_element = sizeof(GLshort);
        break;
    case GL_UNSIGNED_SHORT:
        bytes_per_element = sizeof(GLushort);
        break;
    case GL_INT:
        bytes_per_element = sizeof(GLint);
        break;
    case GL_UNSIGNED_INT:
        bytes_per_element = sizeof(GLuint);
        break;
    case GL_FLOAT:
        bytes_per_element = sizeof(GLfloat);
        break;
    case GL_2_BYTES:
        bytes_per_element = 2*sizeof(GLubyte);
        break;
    case GL_3_BYTES:
        bytes_per_element = 3*sizeof(GLubyte);
        break;
    case GL_4_BYTES:
        bytes_per_element = 4*sizeof(GLubyte);
        break;
    case GL_UTF8_NV:
        {
            const void *utf_string = sequence;
            for (GLsizei i=0; i<count; i++) {
                GLuint code_point;  // ignored
                bool ok = __glPathGetCodePointUTF8(utf_string, code_point);
                if (!ok) {
                    break;
                }
            }
            const char *start = reinterpret_cast<const char*>(sequence);
            const char *end = reinterpret_cast<const char*>(utf_string);
            return end - start;
        }
    case GL_UTF16_NV:
        {
            const void *utf_string = sequence;
            for (GLsizei i=0; i<count; i++) {
                GLuint code_point;  // ignored
                bool ok = __glPathGetCodePointUTF16(utf_string, code_point);
                if (!ok) {
                    break;
                }
            }
            const char *start = reinterpret_cast<const char*>(sequence);
            const char *end = reinterpret_cast<const char*>(utf_string);
            return end - start;
        }
    default:  // generate INVALID_ENUM
        return 0;
    }
    if (count > 0) {
        return count * bytes_per_element;
    } else {
        return 0;
    }
}

static inline size_t
_gl_Paths_size(GLsizei numPaths, GLenum pathNameType, const GLvoid *paths)
{
    return bytesOfSequence(numPaths, pathNameType, paths);
}

static inline size_t
_gl_PathColorGen_size(GLenum genMode, GLenum colorFormat)
{
    GLsizei coeffsPerComponent;
    switch (genMode) {
    case GL_NONE:
        coeffsPerComponent = 0;
        break;
    case GL_OBJECT_LINEAR:
    case GL_PATH_OBJECT_BOUNDING_BOX_NV:
        coeffsPerComponent = 3;
        break;
    case GL_EYE_LINEAR:
        coeffsPerComponent = 4;
        break;
    default:
        return 0;
    }

    GLsizei components;
    switch (colorFormat) {
    case GL_LUMINANCE:
    case GL_ALPHA:
    case GL_INTENSITY:
        components = 1;
        break;
    case GL_LUMINANCE_ALPHA:
        components = 2;
        break;
    case GL_RGB:
        components = 3;
        break;
    case GL_RGBA:
        components = 4;
        break;
    default:
        return 0;
    }

    GLsizei numCoeffs = components * coeffsPerComponent;
    return numCoeffs*sizeof(GLfloat);
}

static inline size_t
_gl_PathTexGen_size(GLenum genMode, GLsizei components)
{
    GLsizei coeffsPerComponent;
    switch (genMode) {
    case GL_NONE:
        return 0;
    case GL_OBJECT_LINEAR:
    case GL_PATH_OBJECT_BOUNDING_BOX_NV:
        coeffsPerComponent = 3;
        break;
    case GL_EYE_LINEAR:
        coeffsPerComponent = 4;
        break;
    default:
        return 0;
    }

    if (components < 1 || components > 4) {
        return 0;
    }

    GLsizei numCoeffs = components * coeffsPerComponent;
    return numCoeffs*sizeof(GLfloat);
}

static size_t valuesPerGetPathParameter(GLenum pname)
{
    switch (pname) {
    case GL_PATH_FILL_MODE_NV:
    case GL_PATH_FILL_MASK_NV:
    case GL_PATH_FILL_COVER_MODE_NV:
    case GL_PATH_STROKE_WIDTH_NV:
    case GL_PATH_INITIAL_END_CAP_NV:
    case GL_PATH_TERMINAL_END_CAP_NV:
    case GL_PATH_JOIN_STYLE_NV:
    case GL_PATH_MITER_LIMIT_NV:
    case GL_PATH_INITIAL_DASH_CAP_NV:
    case GL_PATH_TERMINAL_DASH_CAP_NV:
    case GL_PATH_DASH_OFFSET_NV:
    case GL_PATH_DASH_OFFSET_RESET_NV:
    case GL_PATH_CLIENT_LENGTH_NV:
    case GL_PATH_STROKE_COVER_MODE_NV:
    case GL_PATH_STROKE_MASK_NV:
    case GL_PATH_STROKE_OVERSAMPLE_COUNT_NV:
    case GL_PATH_SAMPLE_QUALITY_NV:
        return 1;
    default:
        return 0;
    }
}

static inline size_t
_gl_GetPathParameter_size(GLenum pname)
{
    return valuesPerGetPathParameter(pname);
}

static inline size_t
_gl_GetPathSpacing(GLsizei numPaths, GLenum transformType)
{
    switch (transformType) {
    case GL_TRANSLATE_X_NV:
        return (numPaths-1)*1;
    case GL_TRANSLATE_2D_NV:
        return (numPaths-1)*2;
    default:
        return 0;
    }
}

/**
 * Helper function for determining the string lengths for glShaderSource and
 * glShaderSourceARB, which is a tad too complex to inline in the specs.
 */
template<class Char>
static inline size_t
_glShaderSource_length(const Char * const * string, const GLint *length, GLsizei index)
{
    if (length != NULL && length[index] >= 0) {
        return (size_t)length[index];
    } else {
        return strlen(string[index]);
    }
}

/**
 * Helper function for determining the string lengths for glGetDebugMessageLog*.
 */
template<class Char>
static inline size_t
_glGetDebugMessageLog_length(const Char * string, const GLsizei *lengths, GLuint count)
{
    size_t size = 0;
    GLuint index;
    if (lengths) {
        for (index = 0; index < count; ++index) {
            size += lengths[index];
        }
    } else {
        for (index = 0; index < count; ++index) {
            size += strlen(&string[size]) + 1;
        }
    }
    if (size) {
        // Remove the last null terminator
        --size;
    }
    return size;
}

#endif /* _GL_SIZE_HPP_ */

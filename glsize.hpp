/**************************************************************************
 *
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

#include "os.hpp"
#include "glimports.hpp"


static inline size_t
__gl_type_size(GLenum type)
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
        return 4;
    case GL_DOUBLE:
        return 8;
    default:
        OS::DebugMessage("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
        return 0;
    }
}

static inline size_t
__glArrayPointer_size(GLint size, GLenum type, GLsizei stride, GLsizei maxIndex)
{
    size_t elementSize = size*__gl_type_size(type);
    if (!stride) {
        stride = (GLsizei)elementSize;
    }
    return stride*maxIndex + elementSize;
}

#define __glVertexPointer_size(size, type, stride, maxIndex) __glArrayPointer_size(size, type, stride, maxIndex)
#define __glNormalPointer_size(type, stride, maxIndex) __glArrayPointer_size(3, type, stride, maxIndex)
#define __glColorPointer_size(size, type, stride, maxIndex) __glArrayPointer_size(size, type, stride, maxIndex)
#define __glIndexPointer_size(type, stride, maxIndex) __glArrayPointer_size(1, type, stride, maxIndex)
#define __glTexCoordPointer_size(size, type, stride, maxIndex) __glArrayPointer_size(size, type, stride, maxIndex)
#define __glEdgeFlagPointer_size(stride, maxIndex) __glArrayPointer_size(1, GL_BOOL, stride, maxIndex)
#define __glFogCoordPointer_size(type, stride, maxIndex) __glArrayPointer_size(1, type, stride, maxIndex)
#define __glSecondaryColorPointer_size(size, type, stride, maxIndex) __glArrayPointer_size(size, type, stride, maxIndex)
#define __glVertexAttribPointer_size(size, type, normalized, stride, maxIndex) __glArrayPointer_size(size, type, stride, maxIndex)

static inline GLuint
__glDrawArrays_maxindex(GLint first, GLsizei count)
{
    if (!count) {
        return 0;
    }
    return first + count - 1;
}

#define __glDrawArraysEXT_maxindex __glDrawArrays_maxindex

static inline GLuint
__glDrawElementsBaseVertex_maxindex(GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
{
    GLvoid *temp = 0;
    GLint __element_array_buffer = 0;

    if (!count) {
        return 0;
    }
    __glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__element_array_buffer);
    if (__element_array_buffer) {
        // Read indices from index buffer object
        GLintptr offset = (GLintptr)indices;
        GLsizeiptr size = count*__gl_type_size(type);
        GLvoid *temp = malloc(size);
        if (!temp) {
            return 0;
        }
        memset(temp, 0, size);
        __glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, temp);
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
        OS::DebugMessage("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
    }

    if (__element_array_buffer) {
        free(temp);
    }

    maxindex += basevertex;

    return maxindex;
}

#define __glDrawRangeElementsBaseVertex_maxindex(start, end, count, type, indices, basevertex) __glDrawElementsBaseVertex_maxindex(count, type, indices, basevertex)

#define __glDrawElements_maxindex(count, type, indices) __glDrawElementsBaseVertex_maxindex(count, type, indices, 0);
#define __glDrawRangeElements_maxindex(start, end, count, type, indices) __glDrawElements_maxindex(count, type, indices)
#define __glDrawRangeElementsEXT_maxindex __glDrawRangeElements_maxindex

#define __glDrawArraysInstanced_maxindex(first, count, primcount) __glDrawArrays_maxindex(first, count)
#define __glDrawElementsInstanced_maxindex(count, type, indices, primcount) __glDrawElements_maxindex(count, type, indices)
#define __glDrawElementsInstancedBaseVertex_maxindex(count, type, indices, primcount, basevertex) __glDrawElementsBaseVertex_maxindex(count, type, indices, basevertex)
#define __glDrawRangeElementsInstanced_maxindex(start, end, count, type, indices, primcount) __glDrawRangeElements_maxindex(start, end, count, type, indices)
#define __glDrawRangeElementsInstancedBaseVertex_maxindex(start, end, count, type, indices, primcount, basevertex) __glDrawRangeElementsBaseVertex_maxindex(start, end, count, type, indices, basevertex)

#define __glDrawArraysInstancedARB_maxindex __glDrawArraysInstanced_maxindex
#define __glDrawElementsInstancedARB_maxindex __glDrawElementsInstanced_maxindex
#define __glDrawArraysInstancedEXT_maxindex __glDrawArraysInstanced_maxindex
#define __glDrawElementsInstancedEXT_maxindex __glDrawElementsInstanced_maxindex

static inline GLuint
__glDrawArraysIndirect_maxindex(const GLvoid *indirect) {
    OS::DebugMessage("apitrace: warning: %s: unsupported\n", __FUNCTION__);
    return 0;
}

static inline GLuint
__glDrawElementsIndirect_maxindex(GLenum type, const GLvoid *indirect) {
    OS::DebugMessage("apitrace: warning: %s: unsupported\n", __FUNCTION__);
    return 0;
}

static inline GLuint
__glMultiDrawArrays_maxindex(const GLint *first, const GLsizei *count, GLsizei primcount) {
    GLuint maxindex = 0;
    for (GLsizei prim = 0; prim < primcount; ++prim) {
        GLuint maxindex_prim = __glDrawArrays_maxindex(first[prim], count[prim]);
        maxindex = std::max(maxindex, maxindex_prim);
    }
    return maxindex;
}

static inline GLuint
__glMultiDrawElements_maxindex(const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount) {
    GLuint maxindex = 0;
    for (GLsizei prim = 0; prim < primcount; ++prim) {
        GLuint maxindex_prim = __glDrawElements_maxindex(count[prim], type, indices[prim]);
        maxindex = std::max(maxindex, maxindex_prim);
    }
    return maxindex;
}

static inline GLuint
__glMultiDrawElementsBaseVertex_maxindex(const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount, const GLint * basevertex) {
    GLuint maxindex = 0;
    for (GLsizei prim = 0; prim < primcount; ++prim) {
        GLuint maxindex_prim = __glDrawElementsBaseVertex_maxindex(count[prim], type, indices[prim], basevertex[prim]);
        maxindex = std::max(maxindex, maxindex_prim);
    }
    return maxindex;
}

#define __glMultiDrawArraysEXT_maxindex __glMultiDrawArrays_maxindex
#define __glMultiDrawElementsEXT_maxindex __glMultiDrawElements_maxindex

#define __glMultiModeDrawArraysIBM_maxindex(first, count, primcount, modestride) __glMultiDrawArrays_maxindex(first, count, primcount)
#define __glMultiModeDrawElementsIBM_maxindex(count, type, indices, primcount, modestride) __glMultiDrawElements_maxindex(count, type, (const GLvoid **)indices, primcount)


static inline size_t
__glCallLists_size(GLsizei n, GLenum type)
{
    return n*__gl_type_size(type);
}

#define __glFogfv_size __gl_param_size
#define __glFogiv_size __gl_param_size

#define __glLightfv_size __gl_param_size
#define __glLightiv_size __gl_param_size

#define __glLightModelfv_size __gl_param_size
#define __glLightModeliv_size __glLightModelfv_size

#define __glMaterialfv_size __gl_param_size
#define __glMaterialiv_size __glMaterialfv_size


static inline size_t
__glMap1d_size(GLenum target, GLint stride, GLint order)
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
        OS::DebugMessage("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, target);
        return 0;
    }

    if (stride < channels) {
        return 0;
    }

    return channels + stride * (order - 1);
}

#define __glMap1f_size __glMap1d_size

static inline size_t
__glMap2d_size(GLenum target, GLint ustride, GLint uorder, GLint vstride, GLint vorder)
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
        OS::DebugMessage("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, target);
        return 0;
    }

    if (ustride < channels || vstride < channels) {
        return 0;
    }

    return channels + 
           ustride * (uorder - 1) +
           vstride * (vorder - 1);
}

#define __glMap2f_size __glMap2d_size

#define __glGetBooleanv_size __gl_param_size
#define __glGetDoublev_size __glGetBooleanv_size
#define __glGetFloatv_size __glGetBooleanv_size
#define __glGetIntegerv_size __glGetBooleanv_size
#define __glGetInteger64v_size __glGetBooleanv_size
#define __glGetBooleani_v_size __gl_param_size
#define __glGetDoublei_v_size __glGetBooleanv_size
#define __glGetFloati_v_size __glGetBooleanv_size
#define __glGetIntegeri_v_size __glGetBooleani_v_size
#define __glGetInteger64i_v_size __glGetBooleani_v_size

#define __glGetLightfv_size __glLightfv_size
#define __glGetLightiv_size __glLightfv_size

#define __glGetMaterialfv_size __glMaterialfv_size
#define __glGetMaterialiv_size __glMaterialfv_size


#define __glColorTableParameterfv_size __gl_param_size
#define __glColorTableParameteriv_size __gl_param_size
#define __glGetColorTableParameterfv_size __gl_param_size
#define __glGetColorTableParameteriv_size __gl_param_size

#define __glConvolutionParameterfv_size __gl_param_size
#define __glConvolutionParameteriv_size __gl_param_size
#define __glGetConvolutionParameterfv_size __gl_param_size
#define __glGetConvolutionParameteriv_size __gl_param_size

#define __glGetHistogramParameterfv_size __gl_param_size
#define __glGetHistogramParameteriv_size __gl_param_size

#define __glGetMinmaxParameterfv_size __gl_param_size
#define __glGetMinmaxParameteriv_size __gl_param_size

#define __glGetProgramivARB_size __gl_param_size
#define __glGetProgramivNV_size __gl_param_size

#define __glGetVertexAttribdvARB_size __gl_param_size
#define __glGetVertexAttribfvARB_size __gl_param_size
#define __glGetVertexAttribivARB_size __gl_param_size
#define __glGetVertexAttribdvNV_size __gl_param_size
#define __glGetVertexAttribfvNV_size __gl_param_size
#define __glGetVertexAttribivNV_size __gl_param_size

#define __glGetQueryObjectivARB_size __gl_param_size
#define __glGetQueryObjectuivARB_size __glGetQueryObjectivARB_size
#define __glGetQueryivARB_size __gl_param_size

#define __glPointParameterfv_size __glPointParameterfvEXT_size
#define __glPointParameteriv_size __glPointParameterfvEXT_size
#define __glPointParameterfvARB_size __glPointParameterfvEXT_size
#define __glPointParameterfvEXT_size __gl_param_size
#define __glPointParameterivNV_size __glPointParameterfvEXT_size

#define __glGetFramebufferAttachmentParameteriv_size __gl_param_size
#define __glGetFramebufferAttachmentParameterivEXT_size __gl_param_size

static inline size_t
__gl_format_channels(GLenum format) {
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
        return 2;
    case GL_RGB:
    case GL_BGR:
        return 3;
    case GL_RGBA:
    case GL_BGRA:
    case GL_ABGR_EXT:
    case GL_CMYK_EXT:
        return 4;
    case GL_CMYKA_EXT:
        return 5;
    default:
        OS::DebugMessage("apitrace: warning: %s: unexpected format GLenum 0x%04X\n", __FUNCTION__, format);
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

static inline size_t
__gl_image_size(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth) {
    size_t num_channels = __gl_format_channels(format);

    size_t bits_per_pixel;
    switch (type) {
    case GL_BITMAP:
        bits_per_pixel = 1;
        break;
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        bits_per_pixel = 8 * num_channels;
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
        bits_per_pixel = 16 * num_channels;
        break;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        bits_per_pixel = 32 * num_channels;
        break;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
        bits_per_pixel = 8;
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_8_8_MESA:
    case GL_UNSIGNED_SHORT_8_8_REV_MESA:
        bits_per_pixel = 16;
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
        bits_per_pixel = 32;
        break;
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        bits_per_pixel = 64;
        break;
    default:
        OS::DebugMessage("apitrace: warning: %s: unexpected type GLenum 0x%04X\n", __FUNCTION__, type);
        bits_per_pixel = 0;
        break;
    }

    GLint alignment = 4;
    GLint row_length = 0;
    GLint image_height = 0;
    GLint skip_rows = 0;
    GLint skip_pixels = 0;
    GLint skip_images = 0;

    __glGetIntegerv(GL_UNPACK_ALIGNMENT,    &alignment);
    __glGetIntegerv(GL_UNPACK_ROW_LENGTH,   &row_length);
    __glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT, &image_height);
    __glGetIntegerv(GL_UNPACK_SKIP_ROWS,    &skip_rows);
    __glGetIntegerv(GL_UNPACK_SKIP_PIXELS,  &skip_pixels);
    __glGetIntegerv(GL_UNPACK_SKIP_IMAGES,  &skip_images);

    if (row_length <= 0) {
        row_length = width;
    }

    size_t row_stride = (row_length*bits_per_pixel + 7)/8;

    if (bits_per_pixel < alignment*8 &&
        (bits_per_pixel & 7) == 0 &&
        _is_pot(bits_per_pixel)) {
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

#define __glTexParameterfv_size __gl_param_size
#define __glTexParameteriv_size __gl_param_size
#define __glGetTexParameterfv_size __gl_param_size
#define __glGetTexParameteriv_size __gl_param_size
#define __glGetTexLevelParameterfv_size __gl_param_size
#define __glGetTexLevelParameteriv_size __gl_param_size
#define __glTexParameterIiv_size __gl_param_size
#define __glTexParameterIuiv_size __gl_param_size
#define __glGetTexParameterIiv_size __gl_param_size
#define __glGetTexParameterIuiv_size __gl_param_size

#define __glTexEnvfv_size __gl_param_size
#define __glTexEnviv_size __gl_param_size
#define __glGetTexEnvfv_size __gl_param_size
#define __glGetTexEnviv_size __gl_param_size

#define __glTexGendv_size __gl_param_size
#define __glTexGenfv_size __gl_param_size
#define __glTexGeniv_size __gl_param_size
#define __glGetTexGendv_size __gl_param_size
#define __glGetTexGenfv_size __gl_param_size
#define __glGetTexGeniv_size __gl_param_size

#define __glTexImage3D_size(format, type, width, height, depth) __gl_image_size(format, type, width, height, depth)
#define __glTexImage2D_size(format, type, width, height)        __gl_image_size(format, type, width, height, 1)
#define __glTexImage1D_size(format, type, width)                __gl_image_size(format, type, width, 1, 1)

#define __glTexSubImage3D_size(format, type, width, height, depth) __glTexImage3D_size(format, type, width, height, depth)
#define __glTexSubImage2D_size(format, type, width, height)        __glTexImage2D_size(format, type, width, height)
#define __glTexSubImage1D_size(format, type, width)                __glTexImage1D_size(format, type, width)

#define __glTexImage3DEXT_size __glTexImage3D_size
#define __glTexImage2DEXT_size __glTexImage2D_size
#define __glTexImage1DEXT_size __glTexImage1D_size
#define __glTexSubImage3DEXT_size __glTexSubImage3D_size
#define __glTexSubImage2DEXT_size __glTexSubImage2D_size
#define __glTexSubImage1DEXT_size __glTexSubImage1D_size

#define __glTextureImage3DEXT_size __glTexImage3D_size
#define __glTextureImage2DEXT_size __glTexImage2D_size
#define __glTextureImage1DEXT_size __glTexImage1D_size
#define __glTextureSubImage3DEXT_size __glTexSubImage3D_size
#define __glTextureSubImage2DEXT_size __glTexSubImage2D_size
#define __glTextureSubImage1DEXT_size __glTexSubImage1D_size

#define __glMultiTexImage3DEXT_size __glTexImage3D_size
#define __glMultiTexImage2DEXT_size __glTexImage2D_size
#define __glMultiTexImage1DEXT_size __glTexImage1D_size
#define __glMultiTexSubImage3DEXT_size __glTexSubImage3D_size
#define __glMultiTexSubImage2DEXT_size __glTexSubImage2D_size
#define __glMultiTexSubImage1DEXT_size __glTexSubImage1D_size

#define __glDrawPixels_size(format, type, width, height) __glTexImage2D_size(format, type, width, height)
#define __glConvolutionFilter1D_size(format, type, width) __glTexImage1D_size(format, type, width)
#define __glConvolutionFilter2D_size(format, type, width, height) __glTexImage2D_size(format, type, width, height)
#define __glColorTable_size(format, type, width) __glTexImage1D_size(format, type, width)
#define __glColorSubTable_size(format, type, count) __glColorTable_size(format, type, count)

#define __glBitmap_size(width, height) __glTexImage2D_size(GL_COLOR_INDEX, GL_BITMAP, width, height)
#define __glPolygonStipple_size() __glBitmap_size(32, 32)

static inline size_t
__glClearBuffer_size(GLenum buffer)
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
        OS::DebugMessage("apitrace: warning: %s: unexpected buffer GLenum 0x%04X\n", __FUNCTION__, buffer);
        return 0;
    }
}

/* 
 * 0 terminated integer/float attribute list.
 */
template<class T>
static inline size_t
__AttribList_size(const T *pAttribList)
{
    size_t size = 0;

    if (pAttribList) {
        do {
            ++size;
        } while (*pAttribList++);
    }

    return size;
}


#endif /* _GL_SIZE_HPP_ */

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
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
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
__glDrawElements_maxindex(GLsizei count, GLenum type, const GLvoid *indices)
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
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
    }

    if (__element_array_buffer) {
        free(temp);
    }

    return maxindex;
}

static inline GLuint
__glDrawRangeElements_maxindex(GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    (void)start;
    (void)end;
    return __glDrawElements_maxindex(count, type, indices);
}

#define __glDrawRangeElementsEXT_maxindex __glDrawRangeElements_maxindex

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
__glMap1d_size(GLenum pname)
{
    switch (pname) {
    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
        return 1;
    case GL_MAP1_TEXTURE_COORD_2:
        return 2;
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
    case GL_MAP1_VERTEX_3:
        return 3;
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
    case GL_MAP1_VERTEX_4:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glMap1f_size __glMap1d_size

static inline size_t
__glMap2d_size(GLenum pname)
{
    switch (pname) {
    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        return 1;
    case GL_MAP2_TEXTURE_COORD_2:
        return 2;
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
    case GL_MAP2_VERTEX_3:
        return 3;
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
    case GL_MAP2_VERTEX_4:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glMap2f_size __glMap2d_size

#define __glGetBooleanv_size __gl_param_size
#define __glGetDoublev_size __glGetBooleanv_size
#define __glGetFloatv_size __glGetBooleanv_size
#define __glGetIntegerv_size __glGetBooleanv_size
#define __glGetInteger64v_size __glGetBooleanv_size

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
        return 4;
    default:
        OS::DebugMessage("warning: %s: unexpected format GLenum 0x%04X\n", __FUNCTION__, format);
        return 0;
    }
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
        OS::DebugMessage("warning: %s: unexpected type GLenum 0x%04X\n", __FUNCTION__, type);
        bits_per_pixel = 0;
        break;
    }

    /* FIXME: consider glPixelStore settings */

    size_t row_stride = (width*bits_per_pixel + 7)/8;

    size_t slice_stride = height*row_stride;

    return depth*slice_stride;
}

#define __glTexParameterfv_size __gl_param_size
#define __glTexParameteriv_size __gl_param_size
#define __glGetTexParameterfv_size __gl_param_size
#define __glGetTexParameteriv_size __gl_param_size
#define __glGetTexLevelParameterfv_size __gl_param_size
#define __glGetTexLevelParameteriv_size __gl_param_size

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

#define __glBitmap_size(width, height) __glTexImage2D_size(GL_COLOR_INDEX, GL_BITMAP, width, height)
#define __glPolygonStipple_size() __glBitmap_size(32, 32)


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

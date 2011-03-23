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
 *
 * Some of these functions were semi-automatically generated with
 * apigen/glsize.py script from Mesa's XML description of the GL enums.
 *
 * Other functions were done by hand.
 */

#ifndef _GL_HELPERS_HPP_
#define _GL_HELPERS_HPP_


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
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_2_BYTES:
    case GL_HALF_FLOAT:
        return 2;
        break;
    case GL_3_BYTES:
        return 3;
        break;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
    case GL_4_BYTES:
        return 4;
        break;
    case GL_DOUBLE:
        return 8;
        break;
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

static inline GLuint
__glDrawArrays_maxindex(GLint first, GLsizei count)
{
    return first + count - 1;
}

static inline GLuint
__glDrawElements_maxindex(GLsizei count, GLenum type, const GLvoid *indices)
{
    GLvoid *temp = 0;
    GLint __element_array_buffer = 0;
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

static inline size_t
__glCallLists_size(GLsizei n, GLenum type)
{
    return n*__gl_type_size(type);
}

static inline size_t
__glFogfv_size(GLenum pname)
{
    switch (pname) {
    case GL_FOG_INDEX:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
    case GL_FOG_MODE:
    case GL_FOG_OFFSET_VALUE_SGIX:
    case GL_FOG_DISTANCE_MODE_NV:
        return 1;
    case GL_FOG_COLOR:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glFogiv_size __glFogfv_size

static inline size_t
__glLightfv_size(GLenum pname)
{
    switch (pname) {
    case GL_SPOT_EXPONENT:
    case GL_SPOT_CUTOFF:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
        return 1;
    case GL_SPOT_DIRECTION:
        return 3;
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_POSITION:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glLightiv_size __glLightfv_size

static inline size_t
__glLightModelfv_size(GLenum pname)
{
    switch (pname) {
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_LIGHT_MODEL_COLOR_CONTROL:
/*  case GL_LIGHT_MODEL_COLOR_CONTROL_EXT:*/
        return 1;
    case GL_LIGHT_MODEL_AMBIENT:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glLightModeliv_size __glLightModelfv_size

static inline size_t
__glMaterialfv_size(GLenum pname)
{
    switch (pname) {
    case GL_SHININESS:
        return 1;
    case GL_COLOR_INDEXES:
        return 3;
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_AMBIENT_AND_DIFFUSE:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glMaterialiv_size __glMaterialfv_size

static inline size_t
__glTexParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_PRIORITY:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_FAIL_VALUE_ARB:
/*  case GL_SHADOW_AMBIENT_SGIX:*/
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_CLIPMAP_FRAME_SGIX:
    case GL_TEXTURE_LOD_BIAS_S_SGIX:
    case GL_TEXTURE_LOD_BIAS_T_SGIX:
    case GL_TEXTURE_LOD_BIAS_R_SGIX:
    case GL_GENERATE_MIPMAP:
/*  case GL_GENERATE_MIPMAP_SGIS:*/
    case GL_TEXTURE_COMPARE_SGIX:
    case GL_TEXTURE_COMPARE_OPERATOR_SGIX:
    case GL_TEXTURE_MAX_CLAMP_S_SGIX:
    case GL_TEXTURE_MAX_CLAMP_T_SGIX:
    case GL_TEXTURE_MAX_CLAMP_R_SGIX:
    case GL_TEXTURE_MAX_ANISOTROPY_EXT:
    case GL_TEXTURE_LOD_BIAS:
/*  case GL_TEXTURE_LOD_BIAS_EXT:*/
    case GL_TEXTURE_STORAGE_HINT_APPLE:
    case GL_STORAGE_PRIVATE_APPLE:
    case GL_STORAGE_CACHED_APPLE:
    case GL_STORAGE_SHARED_APPLE:
    case GL_DEPTH_TEXTURE_MODE:
/*  case GL_DEPTH_TEXTURE_MODE_ARB:*/
    case GL_TEXTURE_COMPARE_MODE:
/*  case GL_TEXTURE_COMPARE_MODE_ARB:*/
    case GL_TEXTURE_COMPARE_FUNC:
/*  case GL_TEXTURE_COMPARE_FUNC_ARB:*/
    case GL_TEXTURE_UNSIGNED_REMAP_MODE_NV:
        return 1;
    case GL_TEXTURE_CLIPMAP_CENTER_SGIX:
    case GL_TEXTURE_CLIPMAP_OFFSET_SGIX:
        return 2;
    case GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX:
        return 3;
    case GL_TEXTURE_BORDER_COLOR:
    case GL_POST_TEXTURE_FILTER_BIAS_SGIX:
    case GL_POST_TEXTURE_FILTER_SCALE_SGIX:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glTexParameteriv_size __glTexParameterfv_size

static inline size_t
__glTexEnvfv_size(GLenum pname)
{
    switch (pname) {
    case GL_ALPHA_SCALE:
    case GL_TEXTURE_ENV_MODE:
    case GL_TEXTURE_LOD_BIAS:
    case GL_COMBINE_RGB:
    case GL_COMBINE_ALPHA:
    case GL_RGB_SCALE:
    case GL_SOURCE0_RGB:
    case GL_SOURCE1_RGB:
    case GL_SOURCE2_RGB:
    case GL_SOURCE3_RGB_NV:
    case GL_SOURCE0_ALPHA:
    case GL_SOURCE1_ALPHA:
    case GL_SOURCE2_ALPHA:
    case GL_SOURCE3_ALPHA_NV:
    case GL_OPERAND0_RGB:
    case GL_OPERAND1_RGB:
    case GL_OPERAND2_RGB:
    case GL_OPERAND3_RGB_NV:
    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_ALPHA:
    case GL_OPERAND3_ALPHA_NV:
    case GL_BUMP_TARGET_ATI:
    case GL_COORD_REPLACE_ARB:
/*  case GL_COORD_REPLACE_NV:*/
        return 1;
    case GL_TEXTURE_ENV_COLOR:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glTexEnviv_size __glTexEnvfv_size

static inline size_t
__glTexGendv_size(GLenum pname)
{
    switch (pname) {
    case GL_TEXTURE_GEN_MODE:
        return 1;
    case GL_OBJECT_PLANE:
    case GL_EYE_PLANE:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glTexGenfv_size __glTexGendv_size

#define __glTexGeniv_size __glTexGendv_size

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

static inline size_t
__glGetBooleanv_size(GLenum pname)
{
    switch (pname) {
    case GL_CURRENT_INDEX:
    case GL_CURRENT_RASTER_INDEX:
    case GL_CURRENT_RASTER_POSITION_VALID:
    case GL_CURRENT_RASTER_DISTANCE:
    case GL_POINT_SMOOTH:
    case GL_POINT_SIZE:
    case GL_SMOOTH_POINT_SIZE_GRANULARITY:
    case GL_LINE_SMOOTH:
    case GL_LINE_WIDTH:
    case GL_LINE_WIDTH_GRANULARITY:
    case GL_LINE_STIPPLE:
    case GL_LINE_STIPPLE_PATTERN:
    case GL_LINE_STIPPLE_REPEAT:
    case GL_LIST_MODE:
    case GL_MAX_LIST_NESTING:
    case GL_LIST_BASE:
    case GL_LIST_INDEX:
    case GL_POLYGON_SMOOTH:
    case GL_POLYGON_STIPPLE:
    case GL_EDGE_FLAG:
    case GL_CULL_FACE:
    case GL_CULL_FACE_MODE:
    case GL_FRONT_FACE:
    case GL_LIGHTING:
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_SHADE_MODEL:
    case GL_COLOR_MATERIAL_FACE:
    case GL_COLOR_MATERIAL_PARAMETER:
    case GL_COLOR_MATERIAL:
    case GL_FOG:
    case GL_FOG_INDEX:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
    case GL_FOG_MODE:
    case GL_DEPTH_TEST:
    case GL_DEPTH_WRITEMASK:
    case GL_DEPTH_CLEAR_VALUE:
    case GL_DEPTH_FUNC:
    case GL_STENCIL_TEST:
    case GL_STENCIL_CLEAR_VALUE:
    case GL_STENCIL_FUNC:
    case GL_STENCIL_VALUE_MASK:
    case GL_STENCIL_FAIL:
    case GL_STENCIL_PASS_DEPTH_FAIL:
    case GL_STENCIL_PASS_DEPTH_PASS:
    case GL_STENCIL_REF:
    case GL_STENCIL_WRITEMASK:
    case GL_MATRIX_MODE:
    case GL_NORMALIZE:
    case GL_MODELVIEW_STACK_DEPTH:
    case GL_PROJECTION_STACK_DEPTH:
    case GL_TEXTURE_STACK_DEPTH:
    case GL_ATTRIB_STACK_DEPTH:
    case GL_CLIENT_ATTRIB_STACK_DEPTH:
    case GL_ALPHA_TEST:
    case GL_ALPHA_TEST_FUNC:
    case GL_ALPHA_TEST_REF:
    case GL_DITHER:
    case GL_BLEND_DST:
    case GL_BLEND_SRC:
    case GL_BLEND:
    case GL_LOGIC_OP_MODE:
    case GL_LOGIC_OP:
    case GL_AUX_BUFFERS:
    case GL_DRAW_BUFFER:
    case GL_READ_BUFFER:
    case GL_SCISSOR_TEST:
    case GL_INDEX_CLEAR_VALUE:
    case GL_INDEX_WRITEMASK:
    case GL_INDEX_MODE:
    case GL_RGBA_MODE:
    case GL_DOUBLEBUFFER:
    case GL_STEREO:
    case GL_RENDER_MODE:
    case GL_PERSPECTIVE_CORRECTION_HINT:
    case GL_POINT_SMOOTH_HINT:
    case GL_LINE_SMOOTH_HINT:
    case GL_POLYGON_SMOOTH_HINT:
    case GL_FOG_HINT:
    case GL_TEXTURE_GEN_S:
    case GL_TEXTURE_GEN_T:
    case GL_TEXTURE_GEN_R:
    case GL_TEXTURE_GEN_Q:
    case GL_PIXEL_MAP_I_TO_I:
    case GL_PIXEL_MAP_I_TO_I_SIZE:
    case GL_PIXEL_MAP_S_TO_S_SIZE:
    case GL_PIXEL_MAP_I_TO_R_SIZE:
    case GL_PIXEL_MAP_I_TO_G_SIZE:
    case GL_PIXEL_MAP_I_TO_B_SIZE:
    case GL_PIXEL_MAP_I_TO_A_SIZE:
    case GL_PIXEL_MAP_R_TO_R_SIZE:
    case GL_PIXEL_MAP_G_TO_G_SIZE:
    case GL_PIXEL_MAP_B_TO_B_SIZE:
    case GL_PIXEL_MAP_A_TO_A_SIZE:
    case GL_UNPACK_SWAP_BYTES:
    case GL_UNPACK_LSB_FIRST:
    case GL_UNPACK_ROW_LENGTH:
    case GL_UNPACK_SKIP_ROWS:
    case GL_UNPACK_SKIP_PIXELS:
    case GL_UNPACK_ALIGNMENT:
    case GL_PACK_SWAP_BYTES:
    case GL_PACK_LSB_FIRST:
    case GL_PACK_ROW_LENGTH:
    case GL_PACK_SKIP_ROWS:
    case GL_PACK_SKIP_PIXELS:
    case GL_PACK_ALIGNMENT:
    case GL_MAP_COLOR:
    case GL_MAP_STENCIL:
    case GL_INDEX_SHIFT:
    case GL_INDEX_OFFSET:
    case GL_RED_SCALE:
    case GL_RED_BIAS:
    case GL_ZOOM_X:
    case GL_ZOOM_Y:
    case GL_GREEN_SCALE:
    case GL_GREEN_BIAS:
    case GL_BLUE_SCALE:
    case GL_BLUE_BIAS:
    case GL_ALPHA_SCALE:
    case GL_ALPHA_BIAS:
    case GL_DEPTH_SCALE:
    case GL_DEPTH_BIAS:
    case GL_MAX_EVAL_ORDER:
    case GL_MAX_LIGHTS:
    case GL_MAX_CLIP_PLANES:
    case GL_MAX_TEXTURE_SIZE:
    case GL_MAX_PIXEL_MAP_TABLE:
    case GL_MAX_ATTRIB_STACK_DEPTH:
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MAX_NAME_STACK_DEPTH:
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
    case GL_SUBPIXEL_BITS:
    case GL_INDEX_BITS:
    case GL_RED_BITS:
    case GL_GREEN_BITS:
    case GL_BLUE_BITS:
    case GL_ALPHA_BITS:
    case GL_DEPTH_BITS:
    case GL_STENCIL_BITS:
    case GL_ACCUM_RED_BITS:
    case GL_ACCUM_GREEN_BITS:
    case GL_ACCUM_BLUE_BITS:
    case GL_ACCUM_ALPHA_BITS:
    case GL_NAME_STACK_DEPTH:
    case GL_AUTO_NORMAL:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_INDEX:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_1:
    case GL_MAP1_TEXTURE_COORD_2:
    case GL_MAP1_TEXTURE_COORD_3:
    case GL_MAP1_TEXTURE_COORD_4:
    case GL_MAP1_VERTEX_3:
    case GL_MAP1_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_INDEX:
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_1:
    case GL_MAP2_TEXTURE_COORD_2:
    case GL_MAP2_TEXTURE_COORD_3:
    case GL_MAP2_TEXTURE_COORD_4:
    case GL_MAP2_VERTEX_3:
    case GL_MAP2_VERTEX_4:
    case GL_MAP1_GRID_SEGMENTS:
    case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
    case GL_POLYGON_OFFSET_UNITS:
    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
    case GL_LIGHT4:
    case GL_LIGHT5:
    case GL_LIGHT6:
    case GL_LIGHT7:
    case GL_BLEND_EQUATION:
/*  case GL_BLEND_EQUATION_EXT:*/
    case GL_CONVOLUTION_1D:
    case GL_CONVOLUTION_2D:
    case GL_SEPARABLE_2D:
    case GL_MAX_CONVOLUTION_WIDTH:
/*  case GL_MAX_CONVOLUTION_WIDTH_EXT:*/
    case GL_MAX_CONVOLUTION_HEIGHT:
/*  case GL_MAX_CONVOLUTION_HEIGHT_EXT:*/
    case GL_POST_CONVOLUTION_RED_SCALE:
/*  case GL_POST_CONVOLUTION_RED_SCALE_EXT:*/
    case GL_POST_CONVOLUTION_GREEN_SCALE:
/*  case GL_POST_CONVOLUTION_GREEN_SCALE_EXT:*/
    case GL_POST_CONVOLUTION_BLUE_SCALE:
/*  case GL_POST_CONVOLUTION_BLUE_SCALE_EXT:*/
    case GL_POST_CONVOLUTION_ALPHA_SCALE:
/*  case GL_POST_CONVOLUTION_ALPHA_SCALE_EXT:*/
    case GL_POST_CONVOLUTION_RED_BIAS:
/*  case GL_POST_CONVOLUTION_RED_BIAS_EXT:*/
    case GL_POST_CONVOLUTION_GREEN_BIAS:
/*  case GL_POST_CONVOLUTION_GREEN_BIAS_EXT:*/
    case GL_POST_CONVOLUTION_BLUE_BIAS:
/*  case GL_POST_CONVOLUTION_BLUE_BIAS_EXT:*/
    case GL_POST_CONVOLUTION_ALPHA_BIAS:
/*  case GL_POST_CONVOLUTION_ALPHA_BIAS_EXT:*/
    case GL_HISTOGRAM:
    case GL_MINMAX:
    case GL_POLYGON_OFFSET_FACTOR:
    case GL_RESCALE_NORMAL:
/*  case GL_RESCALE_NORMAL_EXT:*/
    case GL_TEXTURE_BINDING_1D:
    case GL_TEXTURE_BINDING_2D:
    case GL_TEXTURE_BINDING_3D:
    case GL_PACK_SKIP_IMAGES:
    case GL_PACK_IMAGE_HEIGHT:
    case GL_UNPACK_SKIP_IMAGES:
    case GL_UNPACK_IMAGE_HEIGHT:
    case GL_TEXTURE_3D:
    case GL_MAX_3D_TEXTURE_SIZE:
    case GL_VERTEX_ARRAY:
    case GL_NORMAL_ARRAY:
    case GL_COLOR_ARRAY:
    case GL_INDEX_ARRAY:
    case GL_TEXTURE_COORD_ARRAY:
    case GL_EDGE_FLAG_ARRAY:
    case GL_VERTEX_ARRAY_SIZE:
    case GL_VERTEX_ARRAY_TYPE:
    case GL_VERTEX_ARRAY_STRIDE:
    case GL_NORMAL_ARRAY_TYPE:
    case GL_NORMAL_ARRAY_STRIDE:
    case GL_COLOR_ARRAY_SIZE:
    case GL_COLOR_ARRAY_TYPE:
    case GL_COLOR_ARRAY_STRIDE:
    case GL_INDEX_ARRAY_TYPE:
    case GL_INDEX_ARRAY_STRIDE:
    case GL_TEXTURE_COORD_ARRAY_SIZE:
    case GL_TEXTURE_COORD_ARRAY_TYPE:
    case GL_TEXTURE_COORD_ARRAY_STRIDE:
    case GL_EDGE_FLAG_ARRAY_STRIDE:
    case GL_MULTISAMPLE:
/*  case GL_MULTISAMPLE_ARB:*/
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
/*  case GL_SAMPLE_ALPHA_TO_COVERAGE_ARB:*/
    case GL_SAMPLE_ALPHA_TO_ONE:
/*  case GL_SAMPLE_ALPHA_TO_ONE_ARB:*/
    case GL_SAMPLE_COVERAGE:
/*  case GL_SAMPLE_COVERAGE_ARB:*/
    case GL_SAMPLE_BUFFERS:
/*  case GL_SAMPLE_BUFFERS_ARB:*/
    case GL_SAMPLES:
/*  case GL_SAMPLES_ARB:*/
    case GL_SAMPLE_COVERAGE_VALUE:
/*  case GL_SAMPLE_COVERAGE_VALUE_ARB:*/
    case GL_SAMPLE_COVERAGE_INVERT:
/*  case GL_SAMPLE_COVERAGE_INVERT_ARB:*/
    case GL_COLOR_MATRIX_STACK_DEPTH:
    case GL_MAX_COLOR_MATRIX_STACK_DEPTH:
    case GL_POST_COLOR_MATRIX_RED_SCALE:
    case GL_POST_COLOR_MATRIX_GREEN_SCALE:
    case GL_POST_COLOR_MATRIX_BLUE_SCALE:
    case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
    case GL_POST_COLOR_MATRIX_RED_BIAS:
    case GL_POST_COLOR_MATRIX_GREEN_BIAS:
    case GL_POST_COLOR_MATRIX_BLUE_BIAS:
    case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
    case GL_BLEND_DST_RGB:
    case GL_BLEND_SRC_RGB:
    case GL_BLEND_DST_ALPHA:
    case GL_BLEND_SRC_ALPHA:
    case GL_COLOR_TABLE:
    case GL_POST_CONVOLUTION_COLOR_TABLE:
    case GL_POST_COLOR_MATRIX_COLOR_TABLE:
    case GL_MAX_ELEMENTS_VERTICES:
    case GL_MAX_ELEMENTS_INDICES:
    case GL_CLIP_VOLUME_CLIPPING_HINT_EXT:
    case GL_POINT_SIZE_MIN:
    case GL_POINT_SIZE_MAX:
    case GL_POINT_FADE_THRESHOLD_SIZE:
    case GL_OCCLUSION_TEST_HP:
    case GL_OCCLUSION_TEST_RESULT_HP:
    case GL_LIGHT_MODEL_COLOR_CONTROL:
    case GL_CURRENT_FOG_COORD:
    case GL_FOG_COORDINATE_ARRAY_TYPE:
    case GL_FOG_COORDINATE_ARRAY_STRIDE:
    case GL_FOG_COORD_ARRAY:
    case GL_COLOR_SUM_ARB:
    case GL_SECONDARY_COLOR_ARRAY_SIZE:
    case GL_SECONDARY_COLOR_ARRAY_TYPE:
    case GL_SECONDARY_COLOR_ARRAY_STRIDE:
    case GL_SECONDARY_COLOR_ARRAY:
    case GL_ACTIVE_TEXTURE:
/*  case GL_ACTIVE_TEXTURE_ARB:*/
    case GL_CLIENT_ACTIVE_TEXTURE:
/*  case GL_CLIENT_ACTIVE_TEXTURE_ARB:*/
    case GL_MAX_TEXTURE_UNITS:
/*  case GL_MAX_TEXTURE_UNITS_ARB:*/
    case GL_MAX_RENDERBUFFER_SIZE:
/*  case GL_MAX_RENDERBUFFER_SIZE_EXT:*/
    case GL_TEXTURE_COMPRESSION_HINT:
/*  case GL_TEXTURE_COMPRESSION_HINT_ARB:*/
    case GL_TEXTURE_RECTANGLE_ARB:
/*  case GL_TEXTURE_RECTANGLE_NV:*/
    case GL_TEXTURE_BINDING_RECTANGLE_ARB:
/*  case GL_TEXTURE_BINDING_RECTANGLE_NV:*/
    case GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB:
/*  case GL_MAX_RECTANGLE_TEXTURE_SIZE_NV:*/
    case GL_MAX_TEXTURE_LOD_BIAS:
    case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
    case GL_MAX_SHININESS_NV:
    case GL_MAX_SPOT_EXPONENT_NV:
    case GL_TEXTURE_CUBE_MAP:
/*  case GL_TEXTURE_CUBE_MAP_ARB:*/
    case GL_TEXTURE_BINDING_CUBE_MAP:
/*  case GL_TEXTURE_BINDING_CUBE_MAP_ARB:*/
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
/*  case GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB:*/
    case GL_MULTISAMPLE_FILTER_HINT_NV:
    case GL_FOG_DISTANCE_MODE_NV:
    case GL_VERTEX_PROGRAM_ARB:
    case GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB:
    case GL_MAX_PROGRAM_MATRICES_ARB:
    case GL_CURRENT_MATRIX_STACK_DEPTH_ARB:
    case GL_VERTEX_PROGRAM_POINT_SIZE_ARB:
    case GL_VERTEX_PROGRAM_TWO_SIDE_ARB:
    case GL_PROGRAM_ERROR_POSITION_ARB:
    case GL_DEPTH_CLAMP:
/*  case GL_DEPTH_CLAMP_NV:*/
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
/*  case GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB:*/
    case GL_MAX_VERTEX_UNITS_ARB:
    case GL_ACTIVE_VERTEX_UNITS_ARB:
    case GL_WEIGHT_SUM_UNITY_ARB:
    case GL_VERTEX_BLEND_ARB:
    case GL_CURRENT_WEIGHT_ARB:
    case GL_WEIGHT_ARRAY_TYPE_ARB:
    case GL_WEIGHT_ARRAY_STRIDE_ARB:
    case GL_WEIGHT_ARRAY_SIZE_ARB:
    case GL_WEIGHT_ARRAY_ARB:
    case GL_PACK_INVERT_MESA:
    case GL_STENCIL_BACK_FUNC_ATI:
    case GL_STENCIL_BACK_FAIL_ATI:
    case GL_STENCIL_BACK_PASS_DEPTH_FAIL_ATI:
    case GL_STENCIL_BACK_PASS_DEPTH_PASS_ATI:
    case GL_FRAGMENT_PROGRAM_ARB:
    case GL_MAX_DRAW_BUFFERS_ARB:
/*  case GL_MAX_DRAW_BUFFERS_ATI:*/
    case GL_DRAW_BUFFER0_ARB:
/*  case GL_DRAW_BUFFER0_ATI:*/
    case GL_DRAW_BUFFER1_ARB:
/*  case GL_DRAW_BUFFER1_ATI:*/
    case GL_DRAW_BUFFER2_ARB:
/*  case GL_DRAW_BUFFER2_ATI:*/
    case GL_DRAW_BUFFER3_ARB:
/*  case GL_DRAW_BUFFER3_ATI:*/
    case GL_DRAW_BUFFER4_ARB:
/*  case GL_DRAW_BUFFER4_ATI:*/
    case GL_DRAW_BUFFER5_ARB:
/*  case GL_DRAW_BUFFER5_ATI:*/
    case GL_DRAW_BUFFER6_ARB:
/*  case GL_DRAW_BUFFER6_ATI:*/
    case GL_DRAW_BUFFER7_ARB:
/*  case GL_DRAW_BUFFER7_ATI:*/
    case GL_DRAW_BUFFER8_ARB:
/*  case GL_DRAW_BUFFER8_ATI:*/
    case GL_DRAW_BUFFER9_ARB:
/*  case GL_DRAW_BUFFER9_ATI:*/
    case GL_DRAW_BUFFER10_ARB:
/*  case GL_DRAW_BUFFER10_ATI:*/
    case GL_DRAW_BUFFER11_ARB:
/*  case GL_DRAW_BUFFER11_ATI:*/
    case GL_DRAW_BUFFER12_ARB:
/*  case GL_DRAW_BUFFER12_ATI:*/
    case GL_DRAW_BUFFER13_ARB:
/*  case GL_DRAW_BUFFER13_ATI:*/
    case GL_DRAW_BUFFER14_ARB:
/*  case GL_DRAW_BUFFER14_ATI:*/
    case GL_DRAW_BUFFER15_ARB:
/*  case GL_DRAW_BUFFER15_ATI:*/
    case GL_BLEND_EQUATION_ALPHA_EXT:
    case GL_MATRIX_PALETTE_ARB:
    case GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB:
    case GL_MAX_PALETTE_MATRICES_ARB:
    case GL_CURRENT_PALETTE_MATRIX_ARB:
    case GL_MATRIX_INDEX_ARRAY_ARB:
    case GL_CURRENT_MATRIX_INDEX_ARB:
    case GL_MATRIX_INDEX_ARRAY_SIZE_ARB:
    case GL_MATRIX_INDEX_ARRAY_TYPE_ARB:
    case GL_MATRIX_INDEX_ARRAY_STRIDE_ARB:
    case GL_COMPARE_REF_DEPTH_TO_TEXTURE_EXT:
    case GL_POINT_SPRITE_ARB:
/*  case GL_POINT_SPRITE_NV:*/
    case GL_POINT_SPRITE_R_MODE_NV:
    case GL_MAX_VERTEX_ATTRIBS_ARB:
    case GL_MAX_TEXTURE_COORDS_ARB:
    case GL_MAX_TEXTURE_IMAGE_UNITS_ARB:
    case GL_DEPTH_BOUNDS_TEST_EXT:
    case GL_ARRAY_BUFFER_BINDING_ARB:
    case GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB:
    case GL_PIXEL_PACK_BUFFER_BINDING:
    case GL_PIXEL_UNPACK_BUFFER_BINDING:
    case GL_VERTEX_ARRAY_BUFFER_BINDING_ARB:
    case GL_NORMAL_ARRAY_BUFFER_BINDING_ARB:
    case GL_COLOR_ARRAY_BUFFER_BINDING_ARB:
    case GL_INDEX_ARRAY_BUFFER_BINDING_ARB:
    case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB:
    case GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB:
    case GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB:
    case GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB:
    case GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB:
    case GL_TEXTURE_CUBE_MAP_SEAMLESS:
    case GL_MAX_ARRAY_TEXTURE_LAYERS_EXT:
    case GL_STENCIL_TEST_TWO_SIDE_EXT:
    case GL_ACTIVE_STENCIL_FACE_EXT:
    case GL_TEXTURE_BINDING_1D_ARRAY_EXT:
    case GL_TEXTURE_BINDING_2D_ARRAY_EXT:
    case GL_FRAMEBUFFER_BINDING:
/*  case GL_DRAW_FRAMEBUFFER_BINDING_EXT:*/
    case GL_RENDERBUFFER_BINDING:
/*  case GL_RENDERBUFFER_BINDING_EXT:*/
    case GL_READ_FRAMEBUFFER_BINDING:
/*  case GL_READ_FRAMEBUFFER_BINDING_EXT:*/
    case GL_MAX_COLOR_ATTACHMENTS:
/*  case GL_MAX_COLOR_ATTACHMENTS_EXT:*/
    case GL_MAX_SAMPLES:
/*  case GL_MAX_SAMPLES_EXT:*/
    case GL_MAX_SERVER_WAIT_TIMEOUT:
    case GL_RASTER_POSITION_UNCLIPPED_IBM:
        return 1;
    case GL_SMOOTH_POINT_SIZE_RANGE:
    case GL_LINE_WIDTH_RANGE:
    case GL_POLYGON_MODE:
    case GL_DEPTH_RANGE:
    case GL_MAX_VIEWPORT_DIMS:
    case GL_MAP1_GRID_DOMAIN:
    case GL_MAP2_GRID_SEGMENTS:
    case GL_ALIASED_POINT_SIZE_RANGE:
    case GL_ALIASED_LINE_WIDTH_RANGE:
    case GL_DEPTH_BOUNDS_EXT:
        return 2;
    case GL_CURRENT_NORMAL:
    case GL_POINT_DISTANCE_ATTENUATION:
        return 3;
    case GL_CURRENT_COLOR:
    case GL_CURRENT_TEXTURE_COORDS:
    case GL_CURRENT_RASTER_COLOR:
    case GL_CURRENT_RASTER_TEXTURE_COORDS:
    case GL_CURRENT_RASTER_POSITION:
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_FOG_COLOR:
    case GL_ACCUM_CLEAR_VALUE:
    case GL_VIEWPORT:
    case GL_SCISSOR_BOX:
    case GL_COLOR_CLEAR_VALUE:
    case GL_COLOR_WRITEMASK:
    case GL_MAP2_GRID_DOMAIN:
    case GL_BLEND_COLOR:
/*  case GL_BLEND_COLOR_EXT:*/
    case GL_CURRENT_SECONDARY_COLOR:
        return 4;
    case GL_MODELVIEW_MATRIX:
    case GL_PROJECTION_MATRIX:
    case GL_TEXTURE_MATRIX:
    case GL_MODELVIEW0_ARB:
    case GL_COLOR_MATRIX:
    case GL_MODELVIEW1_ARB:
    case GL_CURRENT_MATRIX_ARB:
    case GL_MODELVIEW2_ARB:
    case GL_MODELVIEW3_ARB:
    case GL_MODELVIEW4_ARB:
    case GL_MODELVIEW5_ARB:
    case GL_MODELVIEW6_ARB:
    case GL_MODELVIEW7_ARB:
    case GL_MODELVIEW8_ARB:
    case GL_MODELVIEW9_ARB:
    case GL_MODELVIEW10_ARB:
    case GL_MODELVIEW11_ARB:
    case GL_MODELVIEW12_ARB:
    case GL_MODELVIEW13_ARB:
    case GL_MODELVIEW14_ARB:
    case GL_MODELVIEW15_ARB:
    case GL_MODELVIEW16_ARB:
    case GL_MODELVIEW17_ARB:
    case GL_MODELVIEW18_ARB:
    case GL_MODELVIEW19_ARB:
    case GL_MODELVIEW20_ARB:
    case GL_MODELVIEW21_ARB:
    case GL_MODELVIEW22_ARB:
    case GL_MODELVIEW23_ARB:
    case GL_MODELVIEW24_ARB:
    case GL_MODELVIEW25_ARB:
    case GL_MODELVIEW26_ARB:
    case GL_MODELVIEW27_ARB:
    case GL_MODELVIEW28_ARB:
    case GL_MODELVIEW29_ARB:
    case GL_MODELVIEW30_ARB:
    case GL_MODELVIEW31_ARB:
    case GL_TRANSPOSE_CURRENT_MATRIX_ARB:
        return 16;
#if 0
    case GL_FOG_COORDINATE_SOURCE:
    case GL_COMPRESSED_TEXTURE_FORMATS:
    case GL_MIN_PROGRAM_TEXEL_OFFSET_EXT:
    case GL_MAX_PROGRAM_TEXEL_OFFSET_EXT:
    case GL_RGBA_INTEGER_MODE_EXT:
        return __glGetBooleanv_variable_size(pname);
#endif
    case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
    case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
    case GL_MAX_VARYING_FLOATS:
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
    case GL_MAX_GEOMETRY_UNIFORM_COMPONENTS:
    case GL_MAX_GEOMETRY_OUTPUT_VERTICES:
    case GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetDoublev_size __glGetBooleanv_size

#define __glGetFloatv_size __glGetBooleanv_size

#define __glGetIntegerv_size __glGetBooleanv_size

#define __glGetLightfv_size __glLightfv_size

#define __glGetLightiv_size __glLightfv_size

#define __glGetMaterialfv_size __glMaterialfv_size

#define __glGetMaterialiv_size __glMaterialfv_size

#define __glGetTexEnvfv_size __glTexEnvfv_size

#define __glGetTexEnviv_size __glTexEnvfv_size

#define __glGetTexGendv_size __glTexGendv_size

#define __glGetTexGenfv_size __glTexGendv_size

#define __glGetTexGeniv_size __glTexGendv_size

static inline size_t
__glGetTexParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_PRIORITY:
    case GL_TEXTURE_RESIDENT:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_FAIL_VALUE_ARB:
/*  case GL_SHADOW_AMBIENT_SGIX:*/
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_CLIPMAP_FRAME_SGIX:
    case GL_TEXTURE_LOD_BIAS_S_SGIX:
    case GL_TEXTURE_LOD_BIAS_T_SGIX:
    case GL_TEXTURE_LOD_BIAS_R_SGIX:
    case GL_GENERATE_MIPMAP:
/*  case GL_GENERATE_MIPMAP_SGIS:*/
    case GL_TEXTURE_COMPARE_SGIX:
    case GL_TEXTURE_COMPARE_OPERATOR_SGIX:
    case GL_TEXTURE_MAX_CLAMP_S_SGIX:
    case GL_TEXTURE_MAX_CLAMP_T_SGIX:
    case GL_TEXTURE_MAX_CLAMP_R_SGIX:
    case GL_TEXTURE_MAX_ANISOTROPY_EXT:
    case GL_TEXTURE_LOD_BIAS:
/*  case GL_TEXTURE_LOD_BIAS_EXT:*/
    case GL_TEXTURE_RANGE_LENGTH_APPLE:
    case GL_TEXTURE_STORAGE_HINT_APPLE:
    case GL_DEPTH_TEXTURE_MODE:
/*  case GL_DEPTH_TEXTURE_MODE_ARB:*/
    case GL_TEXTURE_COMPARE_MODE:
/*  case GL_TEXTURE_COMPARE_MODE_ARB:*/
    case GL_TEXTURE_COMPARE_FUNC:
/*  case GL_TEXTURE_COMPARE_FUNC_ARB:*/
    case GL_TEXTURE_UNSIGNED_REMAP_MODE_NV:
        return 1;
    case GL_TEXTURE_CLIPMAP_CENTER_SGIX:
    case GL_TEXTURE_CLIPMAP_OFFSET_SGIX:
        return 2;
    case GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX:
        return 3;
    case GL_TEXTURE_BORDER_COLOR:
    case GL_POST_TEXTURE_FILTER_BIAS_SGIX:
    case GL_POST_TEXTURE_FILTER_SCALE_SGIX:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetTexParameteriv_size __glGetTexParameterfv_size

static inline size_t
__glGetTexLevelParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_TEXTURE_WIDTH:
    case GL_TEXTURE_HEIGHT:
    case GL_TEXTURE_COMPONENTS:
    case GL_TEXTURE_BORDER:
    case GL_TEXTURE_RED_SIZE:
/*  case GL_TEXTURE_RED_SIZE_EXT:*/
    case GL_TEXTURE_GREEN_SIZE:
/*  case GL_TEXTURE_GREEN_SIZE_EXT:*/
    case GL_TEXTURE_BLUE_SIZE:
/*  case GL_TEXTURE_BLUE_SIZE_EXT:*/
    case GL_TEXTURE_ALPHA_SIZE:
/*  case GL_TEXTURE_ALPHA_SIZE_EXT:*/
    case GL_TEXTURE_LUMINANCE_SIZE:
/*  case GL_TEXTURE_LUMINANCE_SIZE_EXT:*/
    case GL_TEXTURE_INTENSITY_SIZE:
/*  case GL_TEXTURE_INTENSITY_SIZE_EXT:*/
    case GL_TEXTURE_DEPTH:
    case GL_TEXTURE_INDEX_SIZE_EXT:
    case GL_TEXTURE_COMPRESSED_IMAGE_SIZE:
/*  case GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB:*/
    case GL_TEXTURE_COMPRESSED:
/*  case GL_TEXTURE_COMPRESSED_ARB:*/
    case GL_TEXTURE_DEPTH_SIZE:
/*  case GL_TEXTURE_DEPTH_SIZE_ARB:*/
    case GL_TEXTURE_STENCIL_SIZE:
/*  case GL_TEXTURE_STENCIL_SIZE_EXT:*/
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetTexLevelParameteriv_size __glGetTexLevelParameterfv_size

static inline size_t
__glColorTableParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_COLOR_TABLE_SCALE:
    case GL_COLOR_TABLE_BIAS:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glColorTableParameteriv_size __glColorTableParameterfv_size

static inline size_t
__glGetColorTableParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_COLOR_TABLE_FORMAT:
/*  case GL_COLOR_TABLE_FORMAT_EXT:*/
    case GL_COLOR_TABLE_WIDTH:
/*  case GL_COLOR_TABLE_WIDTH_EXT:*/
    case GL_COLOR_TABLE_RED_SIZE:
/*  case GL_COLOR_TABLE_RED_SIZE_EXT:*/
    case GL_COLOR_TABLE_GREEN_SIZE:
/*  case GL_COLOR_TABLE_GREEN_SIZE_EXT:*/
    case GL_COLOR_TABLE_BLUE_SIZE:
/*  case GL_COLOR_TABLE_BLUE_SIZE_EXT:*/
    case GL_COLOR_TABLE_ALPHA_SIZE:
/*  case GL_COLOR_TABLE_ALPHA_SIZE_EXT:*/
    case GL_COLOR_TABLE_LUMINANCE_SIZE:
/*  case GL_COLOR_TABLE_LUMINANCE_SIZE_EXT:*/
    case GL_COLOR_TABLE_INTENSITY_SIZE:
/*  case GL_COLOR_TABLE_INTENSITY_SIZE_EXT:*/
        return 1;
    case GL_COLOR_TABLE_SCALE:
    case GL_COLOR_TABLE_BIAS:
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetColorTableParameteriv_size __glGetColorTableParameterfv_size

static inline size_t
__glConvolutionParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_CONVOLUTION_BORDER_MODE:
/*  case GL_CONVOLUTION_BORDER_MODE_EXT:*/
        return 1;
    case GL_CONVOLUTION_FILTER_SCALE:
/*  case GL_CONVOLUTION_FILTER_SCALE_EXT:*/
    case GL_CONVOLUTION_FILTER_BIAS:
/*  case GL_CONVOLUTION_FILTER_BIAS_EXT:*/
    case GL_CONVOLUTION_BORDER_COLOR:
/*  case GL_CONVOLUTION_BORDER_COLOR_HP:*/
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glConvolutionParameteriv_size __glConvolutionParameterfv_size

static inline size_t
__glGetConvolutionParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_CONVOLUTION_BORDER_MODE:
/*  case GL_CONVOLUTION_BORDER_MODE_EXT:*/
    case GL_CONVOLUTION_FORMAT:
/*  case GL_CONVOLUTION_FORMAT_EXT:*/
    case GL_CONVOLUTION_WIDTH:
/*  case GL_CONVOLUTION_WIDTH_EXT:*/
    case GL_CONVOLUTION_HEIGHT:
/*  case GL_CONVOLUTION_HEIGHT_EXT:*/
    case GL_MAX_CONVOLUTION_WIDTH:
/*  case GL_MAX_CONVOLUTION_WIDTH_EXT:*/
    case GL_MAX_CONVOLUTION_HEIGHT:
/*  case GL_MAX_CONVOLUTION_HEIGHT_EXT:*/
        return 1;
    case GL_CONVOLUTION_FILTER_SCALE:
/*  case GL_CONVOLUTION_FILTER_SCALE_EXT:*/
    case GL_CONVOLUTION_FILTER_BIAS:
/*  case GL_CONVOLUTION_FILTER_BIAS_EXT:*/
    case GL_CONVOLUTION_BORDER_COLOR:
/*  case GL_CONVOLUTION_BORDER_COLOR_HP:*/
        return 4;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetConvolutionParameteriv_size __glGetConvolutionParameterfv_size

static inline size_t
__glGetHistogramParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_HISTOGRAM_WIDTH:
    case GL_HISTOGRAM_FORMAT:
    case GL_HISTOGRAM_RED_SIZE:
    case GL_HISTOGRAM_GREEN_SIZE:
    case GL_HISTOGRAM_BLUE_SIZE:
    case GL_HISTOGRAM_ALPHA_SIZE:
    case GL_HISTOGRAM_LUMINANCE_SIZE:
    case GL_HISTOGRAM_SINK:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetHistogramParameteriv_size __glGetHistogramParameterfv_size

static inline size_t
__glGetMinmaxParameterfv_size(GLenum pname)
{
    switch (pname) {
    case GL_MINMAX_FORMAT:
    case GL_MINMAX_SINK:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetMinmaxParameteriv_size __glGetMinmaxParameterfv_size

static inline size_t
__glGetProgramivARB_size(GLenum pname)
{
    switch (pname) {
    case GL_PROGRAM_LENGTH_ARB:
    case GL_PROGRAM_BINDING_ARB:
    case GL_PROGRAM_ALU_INSTRUCTIONS_ARB:
    case GL_PROGRAM_TEX_INSTRUCTIONS_ARB:
    case GL_PROGRAM_TEX_INDIRECTIONS_ARB:
    case GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB:
    case GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB:
    case GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB:
    case GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB:
    case GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB:
    case GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB:
    case GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB:
    case GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB:
    case GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB:
    case GL_PROGRAM_FORMAT_ARB:
    case GL_PROGRAM_INSTRUCTIONS_ARB:
    case GL_MAX_PROGRAM_INSTRUCTIONS_ARB:
    case GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB:
    case GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB:
    case GL_PROGRAM_TEMPORARIES_ARB:
    case GL_MAX_PROGRAM_TEMPORARIES_ARB:
    case GL_PROGRAM_NATIVE_TEMPORARIES_ARB:
    case GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB:
    case GL_PROGRAM_PARAMETERS_ARB:
    case GL_MAX_PROGRAM_PARAMETERS_ARB:
    case GL_PROGRAM_NATIVE_PARAMETERS_ARB:
    case GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB:
    case GL_PROGRAM_ATTRIBS_ARB:
    case GL_MAX_PROGRAM_ATTRIBS_ARB:
    case GL_PROGRAM_NATIVE_ATTRIBS_ARB:
    case GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB:
    case GL_PROGRAM_ADDRESS_REGISTERS_ARB:
    case GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB:
    case GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB:
    case GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB:
    case GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB:
    case GL_MAX_PROGRAM_ENV_PARAMETERS_ARB:
    case GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB:
    case GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV:
    case GL_MAX_PROGRAM_CALL_DEPTH_NV:
    case GL_MAX_PROGRAM_IF_DEPTH_NV:
    case GL_MAX_PROGRAM_LOOP_DEPTH_NV:
    case GL_MAX_PROGRAM_LOOP_COUNT_NV:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

static inline size_t
__glGetVertexAttribdvARB_size(GLenum pname)
{
    switch (pname) {
    case GL_VERTEX_PROGRAM_ARB:
    case GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB:
    case GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB:
    case GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB:
    case GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB:
    case GL_CURRENT_VERTEX_ATTRIB_ARB:
    case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetVertexAttribfvARB_size __glGetVertexAttribdvARB_size

#define __glGetVertexAttribivARB_size __glGetVertexAttribdvARB_size

static inline size_t
__glGetQueryObjectivARB_size(GLenum pname)
{
    switch (pname) {
    case GL_QUERY_RESULT_ARB:
    case GL_QUERY_RESULT_AVAILABLE_ARB:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetQueryObjectuivARB_size __glGetQueryObjectivARB_size

static inline size_t
__glGetQueryivARB_size(GLenum pname)
{
    switch (pname) {
    case GL_QUERY_COUNTER_BITS_ARB:
    case GL_CURRENT_QUERY_ARB:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

static inline size_t
__glPointParameterfvEXT_size(GLenum pname)
{
    switch (pname) {
    case GL_POINT_SIZE_MIN:
/*  case GL_POINT_SIZE_MIN_ARB:*/
/*  case GL_POINT_SIZE_MIN_SGIS:*/
    case GL_POINT_SIZE_MAX:
/*  case GL_POINT_SIZE_MAX_ARB:*/
/*  case GL_POINT_SIZE_MAX_SGIS:*/
    case GL_POINT_FADE_THRESHOLD_SIZE:
/*  case GL_POINT_FADE_THRESHOLD_SIZE_ARB:*/
/*  case GL_POINT_FADE_THRESHOLD_SIZE_SGIS:*/
    case GL_POINT_SPRITE_R_MODE_NV:
    case GL_POINT_SPRITE_COORD_ORIGIN:
        return 1;
    case GL_POINT_DISTANCE_ATTENUATION:
/*  case GL_POINT_DISTANCE_ATTENUATION_ARB:*/
/*  case GL_POINT_DISTANCE_ATTENUATION_SGIS:*/
        return 3;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glPointParameterfv_size __glPointParameterfvEXT_size
#define __glPointParameteriv_size __glPointParameterfvEXT_size
#define __glPointParameterfvARB_size __glPointParameterfvEXT_size
#define __glPointParameterivNV_size __glPointParameterfvEXT_size

static inline size_t
__glGetProgramivNV_size(GLenum pname)
{
    switch (pname) {
    case GL_PROGRAM_LENGTH_NV:
    case GL_PROGRAM_TARGET_NV:
    case GL_PROGRAM_RESIDENT_NV:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

static inline size_t
__glGetVertexAttribdvNV_size(GLenum pname)
{
    switch (pname) {
    case GL_ATTRIB_ARRAY_SIZE_NV:
    case GL_ATTRIB_ARRAY_STRIDE_NV:
    case GL_ATTRIB_ARRAY_TYPE_NV:
    case GL_CURRENT_ATTRIB_NV:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetVertexAttribfvNV_size __glGetVertexAttribdvNV_size

#define __glGetVertexAttribivNV_size __glGetVertexAttribdvNV_size

static inline size_t
__glGetFramebufferAttachmentParameterivEXT_size(GLenum pname)
{
    switch (pname) {
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT:
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT:
        return 1;
    default:
        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);
        return 1;
    }
}

#define __glGetFramebufferAttachmentParameteriv_size __glGetFramebufferAttachmentParameterivEXT_size

static inline size_t
__glTexImage3D_size(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth, GLint border) {
    size_t num_channels;
    switch (format) {
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_INTENSITY:
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
        num_channels = 1;
        break;
    case GL_LUMINANCE_ALPHA:
        num_channels = 2;
        break;
    case GL_RGB:
    case GL_BGR:
        num_channels = 3;
        break;
    case GL_RGBA:
    case GL_BGRA:
        num_channels = 4;
        break;
    default:
        OS::DebugMessage("warning: %s: unexpected GLenum 0x%04X\n", __FUNCTION__, format);
        num_channels = 0;
        break;
    }

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
        bits_per_pixel = 16 * num_channels;
        break;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        bits_per_pixel = 32 * num_channels;
        break;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
        bits_per_pixel = 8;
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
        bits_per_pixel = 16;
        break;
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        bits_per_pixel = 32;
        break;
    default:
        OS::DebugMessage("warning: %s: unexpected GLenum 0x%04X\n", __FUNCTION__, type);
        bits_per_pixel = 0;
        break;
    }

    /* FIXME: consider glPixelStore settings */

    size_t row_stride = (width*bits_per_pixel + 7)/8;

    size_t slice_stride = height*row_stride;

    return depth*slice_stride;
}

#define __glTexImage2D_size(format, type, width, height, border) __glTexImage3D_size(format, type, width, height, 1, border)
#define __glTexImage1D_size(format, type, width, border)         __glTexImage3D_size(format, type, width, 1, 1, border)

#define __glTexSubImage3D_size(format, type, width, height, depth) __glTexImage3D_size(format, type, width, height, depth, 0)
#define __glTexSubImage2D_size(format, type, width, height)        __glTexImage2D_size(format, type, width, height, 0)
#define __glTexSubImage1D_size(format, type, width)                __glTexImage1D_size(format, type, width, 0)

#define __glTexImage3DEXT_size __glTexImage3D_size
#define __glTexImage2DEXT_size __glTexImage2D_size
#define __glTexImage1DEXT_size __glTexImage1D_size
#define __glTexSubImage3DEXT_size __glTexSubImage3D_size
#define __glTexSubImage2DEXT_size __glTexSubImage2D_size
#define __glTexSubImage1DEXT_size __glTexSubImage1D_size

#define __glDrawPixels_size(format, type, width, height) __glTexImage2D_size(format, type, width, height, 0)

#define __glBitmap_size(width, height) __glTexImage2D_size(GL_COLOR_INDEX, GL_BITMAP, width, height, 0)
#define __glPolygonStipple_size() __glBitmap_size(32, 32)

#endif /* _GL_HELPERS_HPP_ */

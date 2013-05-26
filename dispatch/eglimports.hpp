/**************************************************************************
 *
 * Copyright 2011 LunarG, Inc.
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
 * Central place for all EGL and client API includes.  It can be included
 * directly, or indirectly through glimports.h.
 */

#ifndef _EGLIMPORTS_HPP_
#define _EGLIMPORTS_HPP_


#include <KHR/khrplatform.h>

#ifdef _GDI32_
/* Prevent __declspec(dllimport) attribute */
#undef KHRONOS_APICALL
#define KHRONOS_APICALL
#endif

// EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
/* Prevent collision with Trace::Bool */
#undef Bool


// OpenGL ES
#include <GLES/glplatform.h>
#include <GLES2/gl2platform.h>


// OpenGL ES 2.0

// avoid conflicting with GL_NV_multisample_coverage
#define GL_NV_coverage_sample
#include "GLES2/gl2ext.h"


// OpenGL ES 1.1
typedef int32_t  GLfixed;
typedef int32_t  GLclampx;

#ifndef GL_OES_compressed_paletted_texture
#define GL_PALETTE4_RGB8_OES                                    0x8B90
#define GL_PALETTE4_RGBA8_OES                                   0x8B91
#define GL_PALETTE4_R5_G6_B5_OES                                0x8B92
#define GL_PALETTE4_RGBA4_OES                                   0x8B93
#define GL_PALETTE4_RGB5_A1_OES                                 0x8B94
#define GL_PALETTE8_RGB8_OES                                    0x8B95
#define GL_PALETTE8_RGBA8_OES                                   0x8B96
#define GL_PALETTE8_R5_G6_B5_OES                                0x8B97
#define GL_PALETTE8_RGBA4_OES                                   0x8B98
#define GL_PALETTE8_RGB5_A1_OES                                 0x8B99
#endif

#ifndef GL_OES_point_size_array
#define GL_POINT_SIZE_ARRAY_OES                                 0x8B9C
#define GL_POINT_SIZE_ARRAY_TYPE_OES                            0x898A
#define GL_POINT_SIZE_ARRAY_STRIDE_OES                          0x898B
#define GL_POINT_SIZE_ARRAY_POINTER_OES                         0x898C
#define GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES                  0x8B9F
#endif

#undef _glext_h_
#include "GLES/glext.h"


#endif /* _EGLIMPORTS_HPP_ */

/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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
 * Central place for all GL includes, and respective OS dependent headers.
 */

#ifndef _GLIMPORTS_HPP_
#define _GLIMPORTS_HPP_


#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN 1
#  endif
#  include <windows.h>
#elif defined(__APPLE__)
#elif defined(HAVE_X11)
#  include <X11/Xlib.h>
#endif /* !_WIN32 */


#include <GL/gl.h>
#include <GL/glext.h>


// Windows 8 GL headers define GL_EXT_paletted_texture but not
// GL_TEXTURE_INDEX_SIZE_EXT, and due to the way we include DirectX headers, it
// ends up taking precedence over the ones we bundle...
#if defined(GL_EXT_paletted_texture) && !defined(GL_TEXTURE_INDEX_SIZE_EXT)
#define GL_TEXTURE_INDEX_SIZE_EXT 0x80ED
#endif


// GL_NVX_gpu_memory_info
#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B


// Stuff that got removed from official Khronos headers.
// XXX: Consider removing all our references to it too.
#define GL_PIXEL_TEX_GEN_Q_CEILING_SGIX   0x8184
#define GL_PIXEL_TEX_GEN_Q_ROUND_SGIX     0x8185
#define GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX     0x8186
#define GL_PIXEL_TEX_GEN_ALPHA_REPLACE_SGIX 0x8187
#define GL_PIXEL_TEX_GEN_ALPHA_NO_REPLACE_SGIX 0x8188
#define GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX    0x8189
#define GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX    0x818A
#define GL_FOG_SCALE_SGIX                 0x81FC
#define GL_FOG_SCALE_VALUE_SGIX           0x81FD
#define GL_DEPTH_PASS_INSTRUMENT_SGIX     0x8310
#define GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX 0x8311
#define GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX 0x8312
#define GL_PATH_SAMPLE_QUALITY_NV         0x9085
#define GL_PATH_STROKE_OVERSAMPLE_COUNT_NV 0x9087
#define GL_MULTI_HULLS_NV                 0x908C
#define GL_PROJECTIVE_2D_NV               0x9093
#define GL_PROJECTIVE_3D_NV               0x9095
#define GL_TRANSPOSE_PROJECTIVE_2D_NV     0x9097
#define GL_TRANSPOSE_PROJECTIVE_3D_NV     0x9099


#if defined(_WIN32)

#include <GL/wglext.h>

#ifndef PFD_SUPPORT_DIRECTDRAW
#define PFD_SUPPORT_DIRECTDRAW 0x00002000
#endif
#ifndef PFD_SUPPORT_COMPOSITION
#define PFD_SUPPORT_COMPOSITION 0x00008000
#endif

#ifndef WGL_SWAPMULTIPLE_MAX

extern "C"
typedef struct _WGLSWAP
{
    HDC hdc;
    UINT uiFlags;
} WGLSWAP, *PWGLSWAP, FAR *LPWGLSWAP;

#define WGL_SWAPMULTIPLE_MAX 16

#endif /* !WGL_SWAPMULTIPLE_MAX */

#elif defined(__APPLE__)

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
#elif TARGET_OS_MAC
#include <AvailabilityMacros.h>

/* Silence deprecated OpenGL warnings. */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1090
#include <OpenGL/OpenGLAvailability.h>
#undef OPENGL_DEPRECATED
#undef OPENGL_DEPRECATED_MSG
#define OPENGL_DEPRECATED(from, to)
#define OPENGL_DEPRECATED_MSG(from, to, msg)
#endif

#include <OpenGL/OpenGL.h>

#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1060
#include <OpenGL/CGLIOSurface.h>
#include <OpenGL/CGLDevice.h>
#else
#define kCGLPFAAcceleratedCompute 97
#define kCGLRPAcceleratedCompute 130
typedef void *CGLShareGroupObj;
typedef struct __IOSurface *IOSurfaceRef;
#endif

#if MAC_OS_X_VERSION_MIN_REQUIRED < 1070
#define kCGLPFATripleBuffer 3
#define kCGLPFAOpenGLProfile  99
#define kCGLRPVideoMemoryMegabytes 131
#define kCGLRPTextureMemoryMegabytes 132
#define kCGLCECrashOnRemovedFunctions 316
#define kCGLOGLPVersion_Legacy 0x1000
#define kCGLOGLPVersion_3_2_Core 0x3200
#define kCGLOGLPVersion_GL3_Core 0x3200
#endif

#if MAC_OS_X_VERSION_MIN_REQUIRED < 1080
#define kCGLPFABackingVolatile 77
#define kCGLPFASupportsAutomaticGraphicsSwitching 101
#endif

#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
#define kCGLOGLPVersion_GL3_Core 0x3200
#define kCGLOGLPVersion_GL4_Core 0x4100
#define kCGLRPMajorGLVersion 133
#endif

extern "C" {

// From http://www.opensource.apple.com/source/gdb/gdb-954/libcheckpoint/cpcg.c
typedef void * CGSConnectionID;
typedef int CGSWindowID;
typedef int CGSSurfaceID;

CGLError CGLSetSurface(CGLContextObj ctx, CGSConnectionID cid, CGSWindowID wid, CGSSurfaceID sid);
CGLError CGLGetSurface(CGLContextObj ctx, CGSConnectionID* cid, CGSWindowID* wid, CGSSurfaceID* sid);
CGLError CGLUpdateContext(CGLContextObj ctx);

}
#endif

#else

#ifdef HAVE_X11
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

/* Prevent collision with trace::Bool */
#undef Bool

#endif


#include "eglimports.hpp"


#endif /* _GLIMPORTS_HPP_ */

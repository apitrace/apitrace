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


// Prevent including system's glext.h
#define __glext_h_


// Some functions take GLenum disguised as GLint.  Apple noticed and fixed it
// in the Mac OS X 10.6.x gl.h headers.  Regardless, C++ typechecking rules
// force the wrappers to match the prototype precisely.
#if defined(__APPLE__)
#  include <AvailabilityMacros.h> // for MAC_OS_X_VERSION_10_7
#  if defined(MAC_OS_X_VERSION_10_7)
#    define GLenum_int GLint
#  else
#    define GLenum_int GLenum
#  endif
#else
#  define GLenum_int GLint
#endif


#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <windows.h>
#include <GL/gl.h>

#elif defined(__APPLE__)

#include <OpenGL/gl.h>

#else

#ifdef HAVE_X11
#include <X11/Xlib.h>
#endif

#include <GL/gl.h>

#endif /* !_WIN32 */


// Include our own glext.h
#undef __glext_h_
#include "glext/glext.h"


#ifndef GL_TEXTURE_INDEX_SIZE_EXT
#define GL_TEXTURE_INDEX_SIZE_EXT         0x80ED
#endif


#if defined(_WIN32)

#include "glext/wglext.h"

#define GLAPIENTRY __stdcall

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

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLIOSurface.h>
#include <OpenGL/CGLDevice.h>

#ifndef CGL_VERSION_1_3
#define kCGLPFAOpenGLProfile 99
#define kCGLOGLPVersion_Legacy 0x1000
#define kCGLOGLPVersion_3_2_Core 0x3200
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

#else

#ifdef HAVE_X11
#include <GL/glx.h>
#include "glext/glxext.h"
#endif

/* Prevent collision with trace::Bool */
#undef Bool

#endif


#include "eglimports.hpp"


#endif /* _GLIMPORTS_HPP_ */

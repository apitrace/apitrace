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

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <windows.h>

#else /* !_WIN32 */

#include <X11/Xlib.h>

#endif /* !_WIN32 */

#include <GL/gl.h>

#include <GL/glext.h>

#ifndef GL_TEXTURE_INDEX_SIZE_EXT
#define GL_TEXTURE_INDEX_SIZE_EXT         0x80ED
#endif

#ifdef _WIN32

#include <GL/wglext.h>

#define GLAPIENTRY __stdcall

#ifndef PFD_SUPPORT_DIRECTDRAW
#define PFD_SUPPORT_DIRECTDRAW 0x00002000
#endif
#ifndef PFD_SUPPORT_COMPOSITION
#define PFD_SUPPORT_COMPOSITION 0x00008000
#endif

#ifdef __MINGW32__

extern "C"
typedef struct _WGLSWAP
{
    HDC hdc;
    UINT uiFlags;
} WGLSWAP, *PWGLSWAP, FAR *LPWGLSWAP;

#define WGL_SWAPMULTIPLE_MAX 16

#endif /* __MINGW32__ */

#else /* !_WIN32 */

#include <GL/glx.h>
#include <GL/glext.h>

/* Prevent collision with Trace::Bool */
#undef Bool

#endif /* !_WIN32 */

#endif /* _GLIMPORTS_HPP_ */

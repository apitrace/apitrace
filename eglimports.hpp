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


// OpenGL
#ifndef __gl_h_

// Prevent including system's glext.h
#define __glext_h_

#include <GL/gl.h>

// Include our own glext.h
#undef __glext_h_
#include "glext/glext.h"

#ifndef GL_TEXTURE_INDEX_SIZE_EXT
#define GL_TEXTURE_INDEX_SIZE_EXT         0x80ED
#endif

#endif /* __gl_h_ */


#ifdef HAVE_EGL

// EGL
#include "EGL/egl.h"
#include "EGL/eglext.h"
/* Prevent collision with Trace::Bool */
#undef Bool

#endif /* HAVE_EGL */


#endif /* _EGLIMPORTS_HPP_ */

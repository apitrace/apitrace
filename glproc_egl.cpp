/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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


#include "glproc.hpp"


#if !defined(_WIN32)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for dladdr
#endif
#include <dlfcn.h>
#endif


/*
 * Handle to the true OpenGL library.
 */
#if defined(_WIN32)
HINSTANCE __libGlHandle = NULL;
#else
void *__libGlHandle = RTLD_NEXT;
#endif



#if defined(_WIN32)

#error Unsupported

#elif defined(__APPLE__)

#error Unsupported

#else

/*
 * Lookup a EGL or GLES symbol
 */
static inline void *
__libegl_sym(const char *symbol)
{
    void *proc;

    /* Always try dlsym before eglGetProcAddress as spec 3.10 says
     * implementation may choose to also export extension functions
     * publicly.
     */
    proc = dlsym(__libGlHandle, symbol);
    if (!proc && symbol[0] == 'g' && symbol[1] == 'l')
        proc = (void *) __eglGetProcAddress(symbol);

    return proc;
}

void *
__getPublicProcAddress(const char *procName)
{
    return __libegl_sym(procName);
}

void *
__getPrivateProcAddress(const char *procName)
{
    return __libegl_sym(procName);
}

#endif

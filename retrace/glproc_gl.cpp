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
#include "os.hpp"
#include "os_string.hpp"


#if !defined(_WIN32)
#include <dlfcn.h>
#endif


/*
 * Handle to the true OpenGL library.
 */
#if defined(_WIN32)
HMODULE _libGlHandle = NULL;
#else
void *_libGlHandle = NULL;
#endif



#if defined(_WIN32)

void *
_getPublicProcAddress(const char *procName)
{
    if (!_libGlHandle) {
        const char *szDll = "opengl32.dll";
        
        _libGlHandle = LoadLibraryA(szDll);
        if (!_libGlHandle) {
            os::log("apitrace: error: couldn't load %s\n", szDll);
            return NULL;
        }
    }
        
    return (void *)GetProcAddress(_libGlHandle, procName);
}


void *
_getPrivateProcAddress(const char *procName) {
    return (void *)_wglGetProcAddress(procName);
}


#elif defined(__APPLE__)


/*
 * Path to the true OpenGL framework
 */
static const char *libgl_filename = "/System/Library/Frameworks/OpenGL.framework/OpenGL";


/*
 * Lookup a libGL symbol
 */
void * _libgl_sym(const char *symbol)
{
    if (!_libGlHandle) {
        _libGlHandle = dlopen(libgl_filename, RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);

        if (!_libGlHandle) {
            os::log("apitrace: error: couldn't load %s\n", libgl_filename);
            os::abort();
            return NULL;
        }
    }

    return dlsym(_libGlHandle, symbol);
}


void *
_getPublicProcAddress(const char *procName)
{
    return _libgl_sym(procName);
}

void *
_getPrivateProcAddress(const char *procName)
{
    return _libgl_sym(procName);
}


#else


/*
 * Lookup a libGL symbol
 */
void * _libgl_sym(const char *symbol)
{
    if (!_libGlHandle) {
        const char * libgl_filename = "libGL.so.1";

        _libGlHandle = dlopen(libgl_filename, RTLD_GLOBAL | RTLD_LAZY);
        if (!_libGlHandle) {
            os::log("apitrace: error: couldn't find libGL.so\n");
            return NULL;
        }
    }

    return dlsym(_libGlHandle, symbol);
}


void *
_getPublicProcAddress(const char *procName)
{
    return _libgl_sym(procName);
}

void *
_getPrivateProcAddress(const char *procName)
{
    return (void *)_glXGetProcAddressARB((const GLubyte *)procName);
}


#endif 


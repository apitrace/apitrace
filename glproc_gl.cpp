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
#include <unistd.h> // for symlink
#include <dlfcn.h>
#endif


/*
 * Handle to the true OpenGL library.
 */
#if defined(_WIN32)
HINSTANCE __libGlHandle = NULL;
#else
void *__libGlHandle = NULL;
#endif



#if defined(_WIN32)

void *
__getPublicProcAddress(const char *procName)
{
    if (!__libGlHandle) {
        char szDll[MAX_PATH] = {0};
        
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        
        strcat(szDll, "\\\\opengl32.dll");
        
        __libGlHandle = LoadLibraryA(szDll);
        if (!__libGlHandle) {
            os::log("apitrace: error: couldn't load %s\n", szDll);
            return NULL;
        }
    }
        
    return (void *)GetProcAddress(__libGlHandle, procName);
}


void *
__getPrivateProcAddress(const char *procName) {
    return (void *)__wglGetProcAddress(procName);
}


#elif defined(__APPLE__)


/*
 * Path to the true OpenGL framework
 */
static const char *libgl_filename = "/System/Library/Frameworks/OpenGL.framework/OpenGL";


/*
 * Lookup a libGL symbol
 */
void * __libgl_sym(const char *symbol)
{
    void *result;

    if (!__libGlHandle) {
        /* 
         * Unfortunately we can't just dlopen the true dynamic library because
         * DYLD_LIBRARY_PATH/DYLD_FRAMEWORK_PATH take precedence, even for
         * absolute paths.  So we create a temporary symlink, and dlopen that
         * instead.
         */

        char temp_filename[] = "/tmp/tmp.XXXXXX";

        if (mktemp(temp_filename) != NULL) {
            if (symlink(libgl_filename, temp_filename) == 0) {
                __libGlHandle = dlopen(temp_filename, RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);
                remove(temp_filename);
            }
        }

        if (!__libGlHandle) {
            os::log("apitrace: error: couldn't load %s\n", libgl_filename);
            os::abort();
            return NULL;
        }
    }

    result = dlsym(__libGlHandle, symbol);

#ifndef RETRACE
    if (result == dlsym(RTLD_SELF, symbol)) {
        os::log("apitrace: error: symbol lookup recursion\n");
        os::abort();
        return NULL;
    }
#endif

    return result;
}


void *
__getPublicProcAddress(const char *procName)
{
    return __libgl_sym(procName);
}

void *
__getPrivateProcAddress(const char *procName)
{
    return __libgl_sym(procName);
}


#else


/*
 * Invoke the true dlopen() function.
 */
static void *
__dlopen(const char *filename, int flag)
{
    typedef void * (*PFNDLOPEN)(const char *, int);
    static PFNDLOPEN dlopen_ptr = NULL;

    if (!dlopen_ptr) {
        dlopen_ptr = (PFNDLOPEN)dlsym(RTLD_NEXT, "dlopen");
        if (!dlopen_ptr) {
            os::log("apitrace: error: dlsym(RTLD_NEXT, \"dlopen\") failed\n");
            return NULL;
        }
    }

    return dlopen_ptr(filename, flag);
}


/*
 * Lookup a libGL symbol
 */
void * __libgl_sym(const char *symbol)
{
    void *result;

    if (!__libGlHandle) {
        /*
         * The app doesn't directly link against libGL.so, nor does it directly
         * dlopen it.  So we have to load it ourselves.
         */

        const char * libgl_filename = getenv("TRACE_LIBGL");

        if (!libgl_filename) {
            /*
             * Try to use whatever libGL.so the library is linked against.
             */

            result = dlsym(RTLD_NEXT, symbol);
            if (result) {
                __libGlHandle = RTLD_NEXT;
                return result;
            }

            libgl_filename = "libGL.so.1";
        }

        /*
         * It would have been preferable to use RTLD_LOCAL to ensure that the
         * application can never access libGL.so symbols directly, but this
         * won't work, given libGL.so often loads a driver specific SO and
         * exposes symbols to it.
         */

        __libGlHandle = __dlopen(libgl_filename, RTLD_GLOBAL | RTLD_LAZY);
        if (!__libGlHandle) {
            os::log("apitrace: error: couldn't find libGL.so\n");
            return NULL;
        }
    }

    return dlsym(__libGlHandle, symbol);
}


void *
__getPublicProcAddress(const char *procName)
{
    return __libgl_sym(procName);
}

void *
__getPrivateProcAddress(const char *procName)
{
    return (void *)__glXGetProcAddressARB((const GLubyte *)procName);
}


#endif 


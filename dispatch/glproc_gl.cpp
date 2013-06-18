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
#include "dlopen.hpp"
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
        char szDll[MAX_PATH] = {0};
        
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        
        strcat(szDll, "\\\\opengl32.dll");
        
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
    void *result;

    if (!_libGlHandle) {
        /* 
         * Unfortunately we can't just dlopen the true dynamic library because
         * DYLD_LIBRARY_PATH/DYLD_FRAMEWORK_PATH take precedence, even for
         * absolute paths.  So we create a temporary symlink, and dlopen that
         * instead.
         */

        char temp_filename[] = "/tmp/tmp.XXXXXX";

        if (mktemp(temp_filename) != NULL) {
            if (symlink(libgl_filename, temp_filename) == 0) {
                _libGlHandle = dlopen(temp_filename, RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);
                remove(temp_filename);
            }
        }

        if (!_libGlHandle) {
            os::log("apitrace: error: couldn't load %s\n", libgl_filename);
            os::abort();
            return NULL;
        }
    }

    result = dlsym(_libGlHandle, symbol);

#if 0
    if (result && result == dlsym(RTLD_SELF, symbol)) {
        os::log("apitrace: error: symbol lookup recursion\n");
        os::abort();
        return NULL;
    }
#endif

    return result;
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


static inline void
logSymbol(const char *name, void *ptr) {
    if (0) {
        if (ptr) {
            Dl_info info;
            if (ptr && dladdr(ptr, &info)) {
                os::log("apitrace: %s -> \"%s\"\n", name, info.dli_fname);
            }
        } else {
            os::log("apitrace: %s -> NULL\n", name);
        }
    }
}


#ifdef __GLIBC__
extern "C" void * __libc_dlsym(void *, const char *);
#endif


/*
 * Protect against dlsym interception.
 *
 * We implement the whole API, so we don't need to intercept dlsym -- dlopen is
 * enough. However we need to protect against other dynamic libraries
 * intercepting dlsym, to prevent infinite recursion,
 *
 * In particular the Steam Community Overlay advertises dlsym. See also
 * http://lists.freedesktop.org/archives/apitrace/2013-March/000573.html
 */
static inline void *
_dlsym(void *handle, const char *symbol)
{
    void *result;

#ifdef __GLIBC__
    /*
     * We rely on glibc's internal __libc_dlsym.  See also
     * http://www.linuxforu.com/2011/08/lets-hook-a-library-function/
     *
     * Use use it to obtain the true dlsym.  We don't use __libc_dlsym directly
     * because it does not support things such as RTLD_NEXT.
     */
    typedef void * (*PFN_DLSYM)(void *, const char *);
    static PFN_DLSYM dlsym_ptr = NULL;
    if (!dlsym_ptr) {
        void *libdl_handle = _dlopen("libdl.so.2", RTLD_LOCAL | RTLD_NOW);
        if (libdl_handle) {
            dlsym_ptr = (PFN_DLSYM)__libc_dlsym(libdl_handle, "dlsym");
        }
        if (!dlsym_ptr) {
            os::log("apitrace: error: failed to look up real dlsym\n");
            return NULL;
        }

        logSymbol("dlsym", (void*)dlsym_ptr);
    }

    result = dlsym_ptr(handle, symbol);
#else
    result = dlsym(handle, symbol);
#endif

    return result;
}


/*
 * Lookup a libGL symbol
 */
void * _libgl_sym(const char *symbol)
{
    void *result;

    if (!_libGlHandle) {
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
                _libGlHandle = RTLD_NEXT;
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

        _libGlHandle = _dlopen(libgl_filename, RTLD_GLOBAL | RTLD_LAZY);
        if (!_libGlHandle) {
            os::log("apitrace: error: couldn't find libGL.so\n");
            return NULL;
        }
    }

    result = _dlsym(_libGlHandle, symbol);

    logSymbol(symbol, result);

    return result;
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


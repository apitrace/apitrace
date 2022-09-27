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
        
        strcat(szDll, "\\opengl32.dll");
        
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


#include <sys/stat.h>


/*
 * Path to the true OpenGL framework
 */
static const char *libgl_filename = "/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL";


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
         * absolute paths.
         *
         * We used to overcome this by creating a temporary symlink, and dlopen'ing
         * that instead.
         *
         * However on newer macOS versions the OpenGL framework and dynamic
         * libraries don't exist on the filesystem, but rather in the dyld
         * shared system cache, so this trick no longer works.
         */

        struct stat st;
        int err = stat(libgl_filename, &st);
        if (err != 0) {
            os::log("apitrace: error: %s does not exist;"
                    " see https://github.com/apitrace/apitrace/issues/826\n",
                    libgl_filename);
            os::abort();
        } else {
            os::String temp_template = os::getTemporaryDirectoryPath();
            temp_template.append("tmp.XXXXXX");
            char *temp_filename = temp_template.buf(temp_template.length() + 1);

            if (mktemp(temp_filename) == NULL) {
                os::log("apitrace: error: mktemp failed\n");
                os::abort();
            }

            if (symlink(libgl_filename, temp_filename) != 0) {
                os::log("apitrace: error: symlink failed\n");
                remove(temp_filename);
                os::abort();
            }

            _libGlHandle = dlopen(temp_filename, RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);
            if (!_libGlHandle) {
                os::log("apitrace: error: couldn't load %s -> %s\n", temp_filename, libgl_filename);
                os::abort();
            }

            remove(temp_filename);
        }
    }

    result = dlsym(_libGlHandle, symbol);

    if (result && result == dlsym(RTLD_SELF, symbol)) {
        os::log("apitrace: error: symbol lookup recursion\n");
        os::abort();
        return NULL;
    }

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

#ifndef RTLD_DEEPBIND
#define RTLD_DEEPBIND 0
#endif

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

        _libGlHandle = _dlopen(libgl_filename, RTLD_GLOBAL | RTLD_LAZY | RTLD_DEEPBIND);
        if (!_libGlHandle) {
            os::log("apitrace: error: couldn't find libGL.so\n");
            return NULL;
        }
    }

    result = dlsym(_libGlHandle, symbol);

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


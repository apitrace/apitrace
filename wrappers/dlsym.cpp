/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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


#include <assert.h>
#include <stdlib.h>
#include <libgen.h>

#include <memory>

#include "os.hpp"


#if defined(__GLIBC__) && !defined(__UCLIBC__) && __GLIBC__ == 2 && __GLIBC_MINOR__ < 34


#include <dlfcn.h>

extern "C" void * __libc_dlopen_mode(const char * filename, int flag);
extern "C" void * __libc_dlsym(void * handle, const char * symbol);


/*
 * Protect against dlsym interception.
 *
 * We implement the whole API, so we don't need to intercept dlsym -- dlopen is
 * enough. However we need to protect against other dynamic libraries
 * intercepting dlsym, to prevent infinite recursion,
 *
 * In particular the Steam Community Overlay exports dlsym.  See also
 * http://lists.freedesktop.org/archives/apitrace/2013-March/000573.html
 */
PRIVATE
void *
dlsym(void * handle, const char * symbol)
{
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
        void *libdl_handle = __libc_dlopen_mode("libdl.so.2", RTLD_LOCAL | RTLD_NOW);
        if (libdl_handle) {
            dlsym_ptr = (PFN_DLSYM)__libc_dlsym(libdl_handle, "dlsym");
        }
        if (!dlsym_ptr) {
            os::log("apitrace: error: failed to look up real dlsym\n");
            return NULL;
        }
    }

    return dlsym_ptr(handle, symbol);
}



#endif /* __GLIBC__ */


#include "dlopen.hpp"


extern void * _libGlHandle;



enum LibClass {
    LIB_UNKNOWN = 0,
    LIB_GL,
    LIB_GLX,
    LIB_OPENGL,
    LIB_EGL,
    LIB_GLES1,
    LIB_GLES2,
};


inline LibClass
classifyLibrary(const char *pathname)
{
    std::unique_ptr<char, decltype(free) *> dupname { strdup(pathname), free };

    char *filename = basename(dupname.get());
    assert(filename);

    if (strcmp(filename, "libGL.so") == 0 ||
        strcmp(filename, "libGL.so.1") == 0) {
        return LIB_GL;
    }

    if (strcmp(filename, "libGLX.so") == 0 ||
        strcmp(filename, "libGLX.so.0") == 0) {
        return LIB_GLX;
    }

    if (strcmp(filename, "libOpenGL.so") == 0 ||
        strcmp(filename, "libOpenGL.so.0") == 0) {
        return LIB_OPENGL;
    }

#ifdef EGLTRACE
    if (strcmp(filename, "libEGL.so") == 0 ||
        strcmp(filename, "libEGL.so.1") == 0) {
        return LIB_EGL;
    }

    if  (strcmp(filename, "libGLESv1_CM.so") == 0 ||
         strcmp(filename, "libGLESv1_CM.so.1") == 0) {
        return LIB_GLES1;
    }

    if (strcmp(filename, "libGLESv2.so") == 0 ||
        strcmp(filename, "libGLESv2.so.2") == 0) {
        return LIB_GLES2;
    }
#endif

    /*
     * TODO: Identify driver SOs (e.g, *_dri.so), to prevent intercepting
     * dlopen calls from them.
     *
     * Another alternative is to ignore dlopen calls when inside wrapped calls.
     */

    return LIB_UNKNOWN;
}


/*
 * Several applications, such as Quake3, use dlopen("libGL.so.1"), but
 * LD_PRELOAD does not intercept symbols obtained via dlopen/dlsym, therefore
 * we need to intercept the dlopen() call here, and redirect to our wrapper
 * shared object.
 */
extern "C" PUBLIC
void * dlopen(const char *filename, int flag)
{
    void *handle;

    if (!filename) {
        return _dlopen(filename, flag);
    }

    LibClass libClass = classifyLibrary(filename);
    bool intercept = libClass != LIB_UNKNOWN;

    if (intercept) {
        void *caller = __builtin_return_address(0);
        Dl_info info;
        const char *caller_module = "<unknown>";
        if (dladdr(caller, &info)) {
            caller_module = info.dli_fname;
            intercept = classifyLibrary(caller_module) == LIB_UNKNOWN;
        }

        const char * libgl_filename = getenv("TRACE_LIBGL");
        if (libgl_filename) {
            // Don't intercept when using LD_LIBRARY_PATH instead of LD_PRELOAD
            intercept = false;
        }

        os::log("apitrace: %s dlopen(\"%s\", 0x%x) from %s\n",
                intercept ? "redirecting" : "ignoring",
                filename, flag, caller_module);
    }

#ifdef EGLTRACE

    if (intercept) {
        /* The current dispatch implementation relies on core entry-points to be globally available, so force this.
         *
         * TODO: A better approach would be note down the entry points here and
         * use them latter. Another alternative would be to reopen the library
         * with RTLD_NOLOAD | RTLD_GLOBAL.
         */
        flag &= ~RTLD_LOCAL;
        flag |= RTLD_GLOBAL;
    }

#endif

    handle = _dlopen(filename, flag);
    if (!handle) {
        return handle;
    }

    if (intercept) {
        if (libClass == LIB_GL) {
            // Use the true libGL.so handle instead of RTLD_NEXT from now on
            _libGlHandle = handle;
        }

        // Get the file path for our shared object, and use it instead
        static int dummy = 0xdeedbeef;
        Dl_info info;
        if (dladdr(&dummy, &info)) {
            handle = _dlopen(info.dli_fname, flag);
        } else {
            os::log("apitrace: warning: dladdr() failed\n");
        }

#ifdef EGLTRACE
        // SDL will skip dlopen'ing libEGL.so after it spots EGL symbols on our
        // wrapper, so force loading it here.
        // (https://github.com/apitrace/apitrace/issues/291#issuecomment-59734022)
        if (strcmp(filename, "libEGL.so") != 0 &&
            strcmp(filename, "libEGL.so.1") != 0) {
            _dlopen("libEGL.so.1", RTLD_GLOBAL | RTLD_LAZY);
        }
#endif
    }

    return handle;
}


#ifdef __linux__

#include "trace_writer_local.hpp"

/*
 * Intercept _exit so we can flush our trace even when the app (eg. Wine)
 * aborts.
 *
 * TODO: Currently we dispatch to _Exit, but for completness we should
 * intercept _Exit too.
 */
void
_exit(int status)
{
    trace::localWriter.flush();
    _Exit(status);
}

#endif /* __linux__ */

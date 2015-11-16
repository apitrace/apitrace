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


/*
 * Provides access to real dlopen, as tracing libraries interpose it.
 */


#include "os.hpp"
#include "dlopen.hpp"


#ifdef __GLIBC__


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


/*
 * Invoke the true dlopen() function.
 */
void *
_dlopen(const char *filename, int flag)
{
    typedef void * (*PFN_DLOPEN)(const char *, int);
    static PFN_DLOPEN dlopen_ptr = NULL;

    if (!dlopen_ptr) {
#if defined(ANDROID)
        /* Android does not have dlopen in libdl.so; instead, the
         * implementation is available in the dynamic linker itself.
         * Oddly enough, we still can interpose it, but need to use
         * RTLD_DEFAULT to get pointer to the original function.  */
        dlopen_ptr = (PFN_DLOPEN)dlsym(RTLD_DEFAULT, "dlopen");
#elif defined(__GLIBC__)
        void *libdl_handle = __libc_dlopen_mode("libdl.so.2", RTLD_LOCAL | RTLD_NOW);
        dlopen_ptr = (PFN_DLOPEN)__libc_dlsym(libdl_handle, "dlopen");
#else
        dlopen_ptr = (PFN_DLOPEN)dlsym(RTLD_NEXT, "dlopen");
#endif
        if (!dlopen_ptr) {
            os::log("apitrace: error: failed to look up real dlopen\n");
            return NULL;
        }
    }

    return dlopen_ptr(filename, flag);
}

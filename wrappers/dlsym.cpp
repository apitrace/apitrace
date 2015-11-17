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
 * Intercept dlsym or dlopen.
 */


#include "os.hpp"


#if !defined(_WIN32) && !defined(__APPLE__)

#include <dlfcn.h>
#include <execinfo.h>


#if defined(__GLIBC__)
extern "C" void * __libc_dlopen_mode(const char * filename, int flag);
extern "C" void * __libc_dlsym(void * handle, const char * symbol);
#endif


static const int verbosity = 2;


extern "C" char _begin[]; // obtained via -Wl,-defsym=_begin=0
extern "C" char _etext[]; // builtin


/*
 * Whether the given address belongs to this shared object or not.
 */
static inline bool
isInternalAddress(void *addr)
{
    return _begin <= addr && addr < _etext;
}



#define DLSYM_INTERCEPTION 1


#if DLSYM_INTERCEPTION && defined(__GLIBC__)

#include <link.h> // for link_map


static void * self_handle = NULL;


// Try to get GCC to do tail-call optimization
// XXX: Unfortunately not effective with calls to function pointers
#pragma GCC optimize ("omit-frame-pointer")
#pragma GCC optimize ("optimize-sibling-calls")


/*
 * Intercept dlsym.
 *
 * Intercepting dlopen has several disadvantages:
 * 
 */
PUBLIC
void *
dlsym(void * handle, const char * symbol)
{
    void * returnAddr = __builtin_extract_return_addr(__builtin_return_address(0));

    bool internal = isInternalAddress(returnAddr);

    /*
     * We rely on glibc's internal __libc_dlsym.  See also
     * http://www.linuxforu.com/2011/08/lets-hook-a-library-function/
     *
     * Use use it to obtain the true dlsym.  We don't use __libc_dlsym directly
     * because it does not support things such as RTLD_NEXT.
     */
    typedef void * (*PFN_DLSYM)(void *, const char *);
    static PFN_DLSYM dlsym_ptr = NULL;
    static void *libdl_handle;
    if (!dlsym_ptr) {
        libdl_handle = __libc_dlopen_mode("libdl.so.2", RTLD_LOCAL | RTLD_NOW);
        if (libdl_handle) {
            dlsym_ptr = (PFN_DLSYM)__libc_dlsym(libdl_handle, "dlsym");
        }
        if (!dlsym_ptr) {
            os::log("apitrace: error: failed to look up real dlsym\n");
            os::abort();
        }
    }

    const char *s = symbol;
    bool intercept = (s[0] == 'g' && s[1] == 'l' && (s[2] >= 'A' && s[2] <= 'Z')) ||
                     (s[0] == 'e' && s[1] == 'g' && s[2] == 'l' && (s[3] >= 'A' && s[3] <= 'Z'));

    void *addr = NULL;
    if (!internal) {

        if (intercept) {
            // Get our own handle
            if (!self_handle) {
                static const int dummy = 0xdeadbeef;
                Dl_info info;
                if (!dladdr(&dummy, &info)) {
                    os::log("apitrace: error: dladdr() failed\n");
                    os::abort();
                }
                self_handle = __libc_dlopen_mode(info.dli_fname, RTLD_LAZY | RTLD_NOLOAD);
                if (!self_handle) {
                    os::log("apitrace: error: dlopen(\"%s\") failed\n", info.dli_fname);
                    os::abort();
                }
            }

            addr = __libc_dlsym(self_handle, symbol);
        }

        if (verbosity > 1 ||
            (verbosity == 1 && addr)) {
            const char *filename = NULL;
            const char *quote = "";
            if (handle == RTLD_DEFAULT) {
                filename = "RTLD_DEFAULT";
            } else if (handle == RTLD_NEXT) {
                filename = "RTLD_NEXT";
            } else if (handle == libdl_handle) {
                filename = "libdl.so.2";
            } else {
                struct link_map *map;
                if (dlinfo(handle, RTLD_DI_LINKMAP, &map) == 0) {
                    filename = map->l_name;
                    if (filename && filename[0] == '\0') {
                        filename = NULL;
                    } else {
                        quote = "\"";
                    }
                }
            }
            const char *verb = addr ? "intercepting" : "ignoring";
            if (filename) {
                os::log("apitrace: %s dlsym(%s%s%s, \"%s\")\n", verb, quote, filename, quote, symbol);
            } else {
                os::log("apitrace: %s dlsym(%p, \"%s\")\n", verb, handle, symbol);
            }
        }

        if (addr) {
            return addr;
        }
    }

    addr = dlsym_ptr(handle, symbol);

    if (addr && isInternalAddress(addr)) {
        os::log("apitrace: error: infinite recursing looking up \"%s\"\n", symbol);
        os::abort();
    }

    return addr;
}


#else // !DLSYM_INTERCEPTION


#ifdef __GLIBC__

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
    os::log("apitrace: info: _dlsym(..., \"%s\")\n", symbol);
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

    void *addr = dlsym_ptr(handle, symbol);

    if (addr && isInternalAddress(addr)) {
        os::log("apitrace: error: infinite recursing looking up \"%s\"\n", symbol);
        os::abort();
    }

    return addr;
}

#endif /* __GLIBC__ */


extern void * _libGlHandle;


/*
 * Invoke the true dlopen() function.
 */
static void *
real_dlopen(const char *filename, int flag)
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


/*
 * Several applications, such as Quake3, use dlopen("libGL.so.1"), but
 * LD_PRELOAD does not intercept symbols obtained via dlopen/dlsym, therefore
 * we need to intercept the dlopen() call here, and redirect to our wrapper
 * shared object.
 */
static void *
fake_dlopen(const char *filename, int flag)
{
    bool intercept = false;

    os::log("apitrace: info: dlopen(\"%s\", 0x%x)\n", filename, flag);

    if (filename) {
        intercept =
            strcmp(filename, "libEGL.so") == 0 ||
            strcmp(filename, "libEGL.so.1") == 0 ||
            strcmp(filename, "libGLESv1_CM.so") == 0 ||
            strcmp(filename, "libGLESv1_CM.so.1") == 0 ||
            strcmp(filename, "libGLESv2.so") == 0 ||
            strcmp(filename, "libGLESv2.so.2") == 0 ||
            strcmp(filename, "libGL.so") == 0 ||
            strcmp(filename, "libGL.so.1") == 0;

        if (intercept) {
            os::log("apitrace: redirecting dlopen(\"%s\", 0x%x)\n", filename, flag);

            /* The current dispatch implementation relies on core entry-points
             * to be globally available, so force this.
             *
             * TODO: A better approach would be note down the entry points here
             * and use them latter. Another alternative would be to reopen the
             * library with RTLD_NOLOAD | RTLD_GLOBAL.
             */
            flag &= ~RTLD_LOCAL;
            flag |= RTLD_GLOBAL;
        }
    }

    void *handle = real_dlopen(filename, flag);

    if (intercept) {

        // FIXME: handle absolute paths and other versions
        if (strcmp(filename, "libGL.so") == 0 ||
            strcmp(filename, "libGL.so.1") == 0) {

            // Use the true libGL.so handle instead of RTLD_NEXT from now on
            _libGlHandle = handle;
        }

        // Get the file path for our shared object, and use it instead
        static const int dummy = 0xdeadbeef;
        Dl_info info;
        if (dladdr(&dummy, &info)) {
            handle = real_dlopen(info.dli_fname, flag);
        } else {
            os::log("apitrace: warning: dladdr() failed\n");
        }

        // SDL will skip dlopen'ing libEGL.so after it spots EGL symbols on our
        // wrapper, so force loading it here.
        // (https://github.com/apitrace/apitrace/issues/291#issuecomment-59734022)
        if (strcmp(filename, "libEGL.so") != 0 &&
            strcmp(filename, "libEGL.so.1") != 0) {
            real_dlopen("libEGL.so.1", RTLD_GLOBAL | RTLD_LAZY);
        }
    }

    return handle;
}


extern "C" PUBLIC
void *
dlopen(const char *filename, int flag)
{
    void * returnAddr = __builtin_return_address(0);
    bool internal = isInternalAddress(returnAddr);
    if (internal) {
        return real_dlopen(filename, flag);
    } else {
        return fake_dlopen(filename, flag);
    }
}


#endif // !DLSYM_INTERCEPTION

#endif // !defined(_WIN32) && !defined(__APPLE__)

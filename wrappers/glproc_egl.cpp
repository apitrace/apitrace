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
#include "dlopen.hpp"
#endif


/*
 * Handle to the true OpenGL library.
 * XXX: Not really used yet.
 */
#if defined(_WIN32)
HMODULE _libGlHandle = NULL;
#else
void *_libGlHandle = NULL;
#endif



#if defined(_WIN32)

#error Unsupported

#elif defined(__APPLE__)

#error Unsupported

#else

#ifndef RTLD_DEEPBIND
#define RTLD_DEEPBIND 0
#endif

/*
 * Lookup a public EGL/GL/GLES symbol
 *
 * The spec states that eglGetProcAddress should only be used for non-core
 * (extensions) entry-points.  Core entry-points should be taken directly from
 * the API specific libraries.
 *
 */
void *
_getPublicProcAddress(const char *procName)
{
    void *proc;

    /*
     * We rely on dlsym(RTLD_NEXT, ...) whenever we can, because certain gl*
     * symbols are exported by multiple APIs/SOs, and it's not trivial to
     * determine which API/SO we should get the current symbol from.
     */
    proc = dlsym(RTLD_NEXT, procName);
    if (proc) {
        return proc;
    }

    /*
     * dlsym(RTLD_NEXT, ...) will fail when the SO containing the symbol was
     * loaded with RTLD_LOCAL.  We try to override RTLD_LOCAL with RTLD_GLOBAL
     * when tracing but this doesn't always work.  So we try to guess and load
     * the shared object directly.
     *
     * See https://github.com/apitrace/apitrace/issues/349#issuecomment-113316990
     */

    if (procName[0] == 'e' && procName[1] == 'g' && procName[2] == 'l') {
        static void *libEGL = NULL;
        if (!libEGL) {
            libEGL = _dlopen("libEGL.so.1", RTLD_LOCAL | RTLD_LAZY | RTLD_DEEPBIND);
            if (!libEGL) {
                return NULL;
            }
        }
        return dlsym(libEGL, procName);
    }

    /*
     * TODO: We could further mitigate against using the wrong SO by:
     * - using RTLD_NOLOAD to ensure we only use an existing SO
     * - the determine the right SO via eglQueryAPI and glGetString(GL_VERSION)
     */

    if (procName[0] == 'g' && procName[1] == 'l') {
        /* TODO: Use GLESv1/GLESv2 on a per-context basis. */

        static void *libGLESv2 = NULL;
        if (!libGLESv2) {
            libGLESv2 = _dlopen("libGLESv2.so.2", RTLD_LOCAL | RTLD_LAZY | RTLD_DEEPBIND);
        }
        if (libGLESv2) {
            proc = dlsym(libGLESv2, procName);
            if (proc) {
                return proc;
            }
        }

        static void *libGLESv1 = NULL;
        if (!libGLESv1) {
            libGLESv1 = _dlopen("libGLESv1_CM.so.1", RTLD_LOCAL | RTLD_LAZY | RTLD_DEEPBIND);
        }
        if (libGLESv1) {
            proc = dlsym(libGLESv1, procName);
            if (proc) {
                return proc;
            }
        }

        static void *libGL = NULL;
        if (!libGL) {
            libGL = _dlopen("libGL.so.1", RTLD_LOCAL | RTLD_LAZY | RTLD_DEEPBIND);
        }
        if (libGL) {
            proc = dlsym(libGL, procName);
            if (proc) {
                return proc;
            }
        }
    }

    return NULL;
}

/*
 * Lookup a private EGL/GL/GLES symbol
 *
 * Private symbols should always be available through eglGetProcAddress, and
 * they are guaranteed to work with any context bound (regardless of the API).
 *
 * However, per issue#57, eglGetProcAddress returns garbage on some
 * implementations, and the spec states that implementations may choose to also
 * export extension functions publicly, so we always attempt dlsym before
 * eglGetProcAddress to mitigate that.
 */
void *
_getPrivateProcAddress(const char *procName)
{
    void *proc;
    proc = _getPublicProcAddress(procName);
    if (!proc &&
        ((procName[0] == 'e' && procName[1] == 'g' && procName[2] == 'l') ||
         (procName[0] == 'g' && procName[1] == 'l'))) {
        proc = (void *) _eglGetProcAddress(procName);
    }

    return proc;
}

#endif

##########################################################################
#
# Copyright 2011 LunarG, Inc.
# All Rights Reserved.
#
# Based on glxtrace.py, which has
#
#   Copyright 2011 Jose Fonseca
#   Copyright 2008-2010 VMware, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


"""EGL tracing generator."""


from specs.stdapi import API
from specs.glapi import glapi
from specs.eglapi import eglapi
from specs.glesapi import glesapi
from gltrace import GlTracer
from dispatch import function_pointer_type, function_pointer_value


class EglTracer(GlTracer):

    def isFunctionPublic(self, function):
        # The symbols visible in libEGL.so can vary, so expose them all
        return True

    def traceFunctionImplBody(self, function):
        GlTracer.traceFunctionImplBody(self, function)

        if function.name == 'eglMakeCurrent':
            print '    // update the profile'
            print '    if (ctx != EGL_NO_CONTEXT) {'
            print '        EGLint api = EGL_OPENGL_ES_API, version = 1;'
            print '        gltrace::Context *tr = gltrace::getContext();'
            print '        __eglQueryContext(dpy, ctx, EGL_CONTEXT_CLIENT_TYPE, &api);'
            print '        __eglQueryContext(dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &version);'
            print '        if (api == EGL_OPENGL_API)'
            print '            tr->profile = gltrace::PROFILE_COMPAT;'
            print '        else if (version == 1)'
            print '            tr->profile = gltrace::PROFILE_ES1;'
            print '        else'
            print '            tr->profile = gltrace::PROFILE_ES2;'
            print '    }'

    def wrapRet(self, function, instance):
        GlTracer.wrapRet(self, function, instance)

        if function.name == "eglGetProcAddress":
            print '    %s = __unwrap_proc_addr(procname, %s);' % (instance, instance)


if __name__ == '__main__':
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print '#include <dlfcn.h>'
    print
    print '#include "trace_writer_local.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print '#define EGL_EGLEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print
    print 'static __eglMustCastToProperFunctionPointerType __unwrap_proc_addr(const char * procname, __eglMustCastToProperFunctionPointerType procPtr);'
    print

    api = API()
    api.addApi(eglapi)
    api.addApi(glapi)
    api.addApi(glesapi)
    tracer = EglTracer()
    tracer.trace_api(api)

    print 'static __eglMustCastToProperFunctionPointerType __unwrap_proc_addr(const char * procname, __eglMustCastToProperFunctionPointerType procPtr) {'
    print '    if (!procPtr) {'
    print '        return procPtr;'
    print '    }'
    for f in api.functions:
        ptype = function_pointer_type(f)
        pvalue = function_pointer_value(f)
        print '    if (!strcmp("%s", procname)) {' % f.name
        print '        %s = (%s)procPtr;' % (pvalue, ptype)
        print '        return (__eglMustCastToProperFunctionPointerType)&%s;' % (f.name,)
        print '    }'
    print '    os::log("apitrace: warning: unknown function \\"%s\\"\\n", procname);'
    print '    return procPtr;'
    print '}'
    print
    print r'''


/*
 * Android does not support LD_PRELOAD.
 */
#if !defined(ANDROID)


/*
 * Invoke the true dlopen() function.
 */
static void *__dlopen(const char *filename, int flag)
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
 * Several applications, such as Quake3, use dlopen("libGL.so.1"), but
 * LD_PRELOAD does not intercept symbols obtained via dlopen/dlsym, therefore
 * we need to intercept the dlopen() call here, and redirect to our wrapper
 * shared object.
 */
extern "C" PUBLIC
void * dlopen(const char *filename, int flag)
{
    bool intercept = false;

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

            /* The current dispatch implementation relies on core entry-points to be globally available, so force this.
             *
             * TODO: A better approach would be note down the entry points here and
             * use them latter. Another alternative would be to reopen the library
             * with RTLD_NOLOAD | RTLD_GLOBAL.
             */
            flag &= ~RTLD_LOCAL;
            flag |= RTLD_GLOBAL;
        }
    }

    void *handle = __dlopen(filename, flag);

    if (intercept) {
        // Get the file path for our shared object, and use it instead
        static int dummy = 0xdeedbeef;
        Dl_info info;
        if (dladdr(&dummy, &info)) {
            handle = __dlopen(info.dli_fname, flag);
        } else {
            os::log("apitrace: warning: dladdr() failed\n");
        }
    }

    return handle;
}


#endif /* !ANDROID */



'''

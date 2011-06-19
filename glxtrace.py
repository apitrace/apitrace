##########################################################################
#
# Copyright 2011 Jose Fonseca
# Copyright 2008-2010 VMware, Inc.
# All Rights Reserved.
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


"""GLX tracing generator."""


from stdapi import API
from glapi import glapi
from glxapi import glxapi
from gltrace import GlTracer
from dispatch import function_pointer_type, function_pointer_value


class GlxTracer(GlTracer):

    def is_public_function(self, function):
        # The symbols visible in libGL.so can vary, so expose them all
        return True

    def trace_function_impl_body(self, function):
        GlTracer.trace_function_impl_body(self, function)

        # Take snapshots
        if function.name == 'glXSwapBuffers':
            print '    glsnapshot::snapshot(__call);'
        if function.name in ('glFinish', 'glFlush'):
            print '    GLint __draw_framebuffer = 0;'
            print '    __glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &__draw_framebuffer);'
            print '    if (__draw_framebuffer == 0) {'
            print '        GLint __draw_buffer = GL_NONE;'
            print '        __glGetIntegerv(GL_DRAW_BUFFER, &__draw_buffer);'
            print '        if (__draw_buffer == GL_FRONT) {'
            print '             glsnapshot::snapshot(__call);'
            print '        }'
            print '    }'

    def wrap_ret(self, function, instance):
        GlTracer.wrap_ret(self, function, instance)

        if function.name in ("glXGetProcAddress", "glXGetProcAddressARB"):
            print '    %s = __unwrap_proc_addr(procName, %s);' % (instance, instance)


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print
    print '#ifndef _GNU_SOURCE'
    print '#define _GNU_SOURCE // for dladdr'
    print '#endif'
    print '#include <dlfcn.h>'
    print
    print '#include "trace_writer.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print '#define GLX_GLXEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print '#include "glsnapshot.hpp"'
    print
    print 'static __GLXextFuncPtr __unwrap_proc_addr(const GLubyte * procName, __GLXextFuncPtr procPtr);'
    print

    api = API()
    api.add_api(glxapi)
    api.add_api(glapi)
    tracer = GlxTracer()
    tracer.trace_api(api)

    print 'static __GLXextFuncPtr __unwrap_proc_addr(const GLubyte * procName, __GLXextFuncPtr procPtr) {'
    print '    if (!procPtr) {'
    print '        return procPtr;'
    print '    }'
    for f in api.functions:
        ptype = function_pointer_type(f)
        pvalue = function_pointer_value(f)
        print '    if (!strcmp("%s", (const char *)procName)) {' % f.name
        print '        %s = (%s)procPtr;' % (pvalue, ptype)
        print '        return (__GLXextFuncPtr)&%s;' % (f.name,)
        print '    }'
    print '    OS::DebugMessage("apitrace: warning: unknown function \\"%s\\"\\n", (const char *)procName);'
    print '    return procPtr;'
    print '}'
    print
    print r'''


/*
 * Handle to the true libGL.so
 */
static void *libgl_handle = NULL;


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
            OS::DebugMessage("apitrace: error: dlsym(RTLD_NEXT, \"dlopen\") failed\n");
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
    void *handle;

    handle = __dlopen(filename, flag);

    const char * libgl_filename = getenv("TRACE_LIBGL");

    if (filename && handle && !libgl_filename) {
        if (0) {
            OS::DebugMessage("apitrace: warning: dlopen(\"%s\", 0x%x)\n", filename, flag);
        }

        // FIXME: handle absolute paths and other versions
        if (strcmp(filename, "libGL.so") == 0 ||
            strcmp(filename, "libGL.so.1") == 0) {

            // Use the true libGL.so handle instead of RTLD_NEXT from now on
            libgl_handle = handle;

            // Get the file path for our shared object, and use it instead
            static int dummy = 0xdeedbeef;
            Dl_info info;
            if (dladdr(&dummy, &info)) {
                OS::DebugMessage("apitrace: redirecting dlopen(\"%s\", 0x%x)\n", filename, flag);
                handle = __dlopen(info.dli_fname, flag);
            } else {
                OS::DebugMessage("apitrace: warning: dladdr() failed\n");
            }
        }
    }

    return handle;
}


/*
 * Lookup a libGL symbol
 */
void * __libgl_sym(const char *symbol)
{
    void *result;

    if (!libgl_handle) {
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
                libgl_handle = RTLD_NEXT;
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

        libgl_handle = __dlopen(libgl_filename, RTLD_GLOBAL | RTLD_LAZY);
        if (!libgl_handle) {
            OS::DebugMessage("apitrace: error: couldn't find libGL.so\n");
            return NULL;
        }
    }

    return dlsym(libgl_handle, symbol);
}


'''

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

    def is_public_function(self, function):
        # The symbols visible in libEGL.so can vary, so expose them all
        return True

    def trace_function_impl_body(self, function):
        GlTracer.trace_function_impl_body(self, function)

        # Take snapshots
        if function.name == 'eglSwapBuffers':
            print '    glsnapshot::snapshot(__call);'
        if function.name in ('glFinish', 'glFlush'):
            print '    tracer_context *ctx = __get_context();'
            print '    GLint __draw_framebuffer = 0;'
            print '    __glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &__draw_framebuffer);'
            print '    if (__draw_framebuffer == 0 && ctx->profile == PROFILE_COMPAT) {'
            print '        GLint __draw_buffer = GL_NONE;'
            print '        __glGetIntegerv(GL_DRAW_BUFFER, &__draw_buffer);'
            print '        if (__draw_buffer == GL_FRONT) {'
            print '             glsnapshot::snapshot(__call);'
            print '        }'
            print '    }'
        if function.name == 'eglMakeCurrent':
            print '    // update the profile'
            print '    if (ctx != EGL_NO_CONTEXT) {'
            print '        EGLint api = EGL_OPENGL_ES_API, version = 1;'
            print '        tracer_context *tr = __get_context();'
            print '        __eglQueryContext(dpy, ctx, EGL_CONTEXT_CLIENT_TYPE, &api);'
            print '        __eglQueryContext(dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &version);'
            print '        if (api == EGL_OPENGL_API)'
            print '            tr->profile = PROFILE_COMPAT;'
            print '        else if (version == 1)'
            print '            tr->profile = PROFILE_ES1;'
            print '        else'
            print '            tr->profile = PROFILE_ES2;'
            print '    }'

    def wrap_ret(self, function, instance):
        GlTracer.wrap_ret(self, function, instance)

        if function.name == "eglGetProcAddress":
            print '    %s = __unwrap_proc_addr(procname, %s);' % (instance, instance)


if __name__ == '__main__':
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print '#include <dlfcn.h>'
    print
    print '#include "trace_writer.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print '#define EGL_EGLEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print '#include "glsnapshot.hpp"'
    print
    print 'static __eglMustCastToProperFunctionPointerType __unwrap_proc_addr(const char * procname, __eglMustCastToProperFunctionPointerType procPtr);'
    print

    api = API()
    api.add_api(eglapi)
    api.add_api(glapi)
    api.add_api(glesapi)
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
 * Lookup a EGL or GLES symbol
 */
void * __libegl_sym(const char *symbol, bool pub)
{
    void *proc;

    /*
     * Public symbols are EGL core functions and those defined in dekstop GL
     * ABI.  Troubles come from the latter.
     */
    if (pub) {
        proc = dlsym(RTLD_NEXT, symbol);
        if (!proc && symbol[0] == 'g' && symbol[1] == 'l')
            proc = (void *) __eglGetProcAddress(symbol);
    }
    else {
        proc = (void *) __eglGetProcAddress(symbol);
        if (!proc && symbol[0] == 'g' && symbol[1] == 'l')
            proc = dlsym(RTLD_NEXT, symbol);
    }

    return proc;
}
'''

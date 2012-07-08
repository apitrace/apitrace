##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
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


"""WGL tracing code generator."""


from gltrace import GlTracer
from specs.stdapi import API
from specs.glapi import glapi
from specs.wglapi import wglapi


class WglTracer(GlTracer):

    getProcAddressFunctionNames = [
        "wglGetProcAddress",
    ]

    def traceFunctionImplBody(self, function):
        GlTracer.traceFunctionImplBody(self, function)

        if function.name == 'wglCreateContext':
            print '    if (_result)'
            print '        gltrace::createContext((uintptr_t)_result);'

        if function.name == 'wglMakeCurrent':
            print '    if (_result) {'
            print '        if (hglrc != NULL)'
            print '            gltrace::setContext((uintptr_t)hglrc);'
            print '        else'
            print '            gltrace::clearContext();'
            print '    }'

        if function.name == 'wglDeleteContext':
            # Unlike other GL APIs like EGL or GLX, WGL will make the context
            # inactive if it's currently the active context.
            print '    if (_wglGetCurrentContext() == hglrc) {'
            print '        gltrace::clearContext();'
            print '    }'
            print '    gltrace::destroyContext((uintptr_t)hglrc);'


if __name__ == '__main__':
    print
    print '#define _GDI32_'
    print
    print '#include <string.h>'
    print '#include <windows.h>'
    print
    print '#include "trace_writer_local.hpp"'
    print '#include "os.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print '#define WGL_GLXEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print
    api = API()
    api.addApi(glapi)
    api.addApi(wglapi)
    tracer = WglTracer()
    tracer.traceApi(api)

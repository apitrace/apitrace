##########################################################################
#
# Copyright 2011 VMware, Inc.
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


"""CGL tracing generator."""


from gltrace import GlTracer
from specs.stdapi import Module, API
from specs.glapi import glapi
from specs.cglapi import cglapi


class CglTracer(GlTracer):

    def isFunctionPublic(self, function):
        # all OpenGL symbols are visible on MacOSX
        return True

    def traceFunctionImplBody(self, function):
        if function.name == 'CGLReleaseContext':
            # Unlike other GL APIs like EGL or GLX, CGL will make the context
            # not current if it's the current context.
            print '    if (_CGLGetContextRetainCount(ctx) == 1) {'
            print '        if (gltrace::releaseContext((uintptr_t)ctx)) {'
            print '            if (_CGLGetCurrentContext() == ctx) {'
            print '                gltrace::clearContext();'
            print '            }'
            print '        }'
            print '    }'

        if function.name == 'CGLDestroyContext':
            # The same rule applies here about the  as for CGLReleaseContext.
            print '    if (gltrace::releaseContext((uintptr_t)ctx)) {'
            print '        if (_CGLGetCurrentContext() == ctx) {'
            print '            gltrace::clearContext();'
            print '        }'
            print '    }'

        GlTracer.traceFunctionImplBody(self, function)

        if function.name == 'CGLCreateContext':
            print '    if (_result == kCGLNoError) {'
            print '        gltrace::createContext((uintptr_t)*ctx);'
            print '    }'

        if function.name == 'CGLSetCurrentContext':
            print '    if (_result == kCGLNoError) {'
            print '        if (ctx != NULL) {'
            print '            gltrace::setContext((uintptr_t)ctx);'
            print '        } else {'
            print '            gltrace::clearContext();'
            print '        }'
            print '    }'

        if function.name == 'CGLRetainContext':
            print '    gltrace::retainContext((uintptr_t)ctx);'


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print
    print '#include "trace_writer_local.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print

    module = Module()
    module.mergeModule(cglapi)
    module.mergeModule(glapi)
    api = API()
    api.addModule(module)
    tracer = CglTracer()
    tracer.traceApi(api)

    print r'''

PUBLIC
void * gll_noop = 0;

'''

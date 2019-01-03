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


from gltrace import GlTracer
from specs.stdapi import Module, API
from specs.glapi import glapi
from specs.glxapi import glxapi


class GlxTracer(GlTracer):

    def isFunctionPublic(self, function):
        # The symbols visible in libGL.so can vary, so expose them all
        return True

    getProcAddressFunctionNames = [
        "glXGetProcAddress",
        "glXGetProcAddressARB",
    ]

    createContextFunctionNames = [
        'glXCreateContext',
        'glXCreateContextAttribsARB',
        'glXCreateContextWithConfigSGIX',
        'glXCreateNewContext',
    ]

    destroyContextFunctionNames = [
        'glXDestroyContext',
    ]

    makeCurrentFunctionNames = [
        'glXMakeCurrent',
        'glXMakeContextCurrent',
        'glXMakeCurrentReadSGI',
    ]

    def traceFunctionImplBody(self, function):
        if function.name in self.destroyContextFunctionNames:
            print('    gltrace::releaseContext((uintptr_t)ctx);')

        GlTracer.traceFunctionImplBody(self, function)

        if function.name in self.createContextFunctionNames:
            print('    if (_result != NULL)')
            print('        gltrace::createContext((uintptr_t)_result);')

        if function.name in self.makeCurrentFunctionNames:
            print('    if (_result) {')
            print('        if (ctx != NULL)')
            print('            gltrace::setContext((uintptr_t)ctx);')
            print('        else')
            print('            gltrace::clearContext();')
            print('    }')

        if function.name == 'glXBindTexImageEXT':
            # FIXME: glXBindTexImageEXT gets called frequently, so we should
            # avoid recording the same data over and over again somehow, e.g.:
            # - get the pixels before and after glXBindTexImageEXT, and only
            #   emit emitFakeTexture2D when it changes
            # - keep a global hash of the pixels
            # FIXME: Handle mipmaps
            print(r'''
                unsigned glx_target = 0;
                _glXQueryDrawable(display, drawable, GLX_TEXTURE_TARGET_EXT, &glx_target);
                GLenum target;
                switch (glx_target) {
                // FIXME
                //case GLX_TEXTURE_1D_EXT:
                //    target = GL_TEXTURE_1D;
                //    break;
                case GLX_TEXTURE_2D_EXT:
                    target = GL_TEXTURE_2D;
                    break;
                case GLX_TEXTURE_RECTANGLE_EXT:
                    target = GL_TEXTURE_RECTANGLE;
                    break;
                default:
                    os::log("apitrace: warning: %s: unsupported GLX_TEXTURE_TARGET_EXT 0x%u\n", __FUNCTION__, glx_target);
                    target = GL_NONE;
                    break;
                }
                GLint level = 0;
                GLint internalformat = GL_NONE;
                _glGetTexLevelParameteriv(target, level, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
                // XXX: GL_TEXTURE_INTERNAL_FORMAT cannot be trusted on NVIDIA
                // -- it sometimes returns GL_BGRA, even though GL_BGR/BGRA is
                // not a valid internal format.
                switch (internalformat) {
                case GL_BGR:
                    internalformat = GL_RGB;
                    break;
                case GL_BGRA:
                    internalformat = GL_RGBA;
                    break;
                }
                GLint width = 0;
                _glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);
                GLint height = 0;
                _glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);
                GLint border = 0;
                // XXX: We always use GL_RGBA format to read the pixels because:
                // - some implementations (Mesa) seem to return bogus results
                //   for GLX_TEXTURE_FORMAT_EXT
                // - hardware usually stores GL_RGB with 32bpp, so it should be
                //   faster to read/write
                // - it is more robust against GL_(UN)PACK_ALIGNMENT state
                //   changes
                // The drawback is that traces will be slightly bigger.
                GLenum format = GL_RGBA;
                GLenum type = GL_UNSIGNED_BYTE;
                if (target && internalformat && height && width) {
                    // FIXME: This assumes (UN)PACK state (in particular
                    // GL_(UN)PACK_ROW_LENGTH) is set to its defaults. We
                    // really should temporarily reset the state here (and emit
                    // according fake calls) to cope when its not. At very
                    // least we need a heads up warning that this will cause
                    // problems.
                    GLint alignment = 4;
                    GLint row_stride = _align(width * 4, alignment);
                    std::unique_ptr<GLbyte[]> data(new GLbyte[height * row_stride]);
                    GLvoid *pixels = data.get();
                    _glGetTexImage(target, level, format, type, pixels);
            ''')
            self.emitFakeTexture2D()
            print(r'''
                }
            ''')


if __name__ == '__main__':
    print()
    print('#include <stdlib.h>')
    print('#include <string.h>')
    print()
    print('#include <memory>')
    print()
    print('#include "trace_writer_local.hpp"')
    print()
    print('// To validate our prototypes')
    print('#define GL_GLEXT_PROTOTYPES')
    print('#define GLX_GLXEXT_PROTOTYPES')
    print()
    print('#include "glproc.hpp"')
    print('#include "glsize.hpp"')
    print()

    module = Module()
    module.mergeModule(glxapi)
    module.mergeModule(glapi)
    api = API()
    api.addModule(module)
    tracer = GlxTracer()
    tracer.traceApi(api)

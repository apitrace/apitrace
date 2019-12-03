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


from gltrace import GlTracer
from specs.stdapi import Module, API
from specs.glapi import glapi
from specs.eglapi import eglapi


class EglTracer(GlTracer):

    def isFunctionPublic(self, function):
        # The symbols visible in libEGL.so can vary, so expose them all
        return True

    getProcAddressFunctionNames = [
        "eglGetProcAddress",
    ]

    def traceFunctionImplBody(self, function):
        GlTracer.traceFunctionImplBody(self, function)

        if function.name == 'eglCreateContext':
            print('    if (_result != EGL_NO_CONTEXT)')
            print('        gltrace::createContext((uintptr_t)_result, (uintptr_t)share_context);')

        if function.name == 'eglMakeCurrent':
            print(r'    if (_result) {')
            print(r'        // update the profile')
            print(r'        if (ctx != EGL_NO_CONTEXT) {')
            print(r'            gltrace::setContext((uintptr_t)ctx);')
            print(r'            gltrace::Context *tr = gltrace::getContext();')
            print(r'            EGLint api = EGL_OPENGL_ES_API;')
            print(r'            _eglQueryContext(dpy, ctx, EGL_CONTEXT_CLIENT_TYPE, &api);')
            print(r'            if (api == EGL_OPENGL_API) {')
            print(r'                assert(tr->profile.api == glfeatures::API_GL);')
            print(r'            } else if (api == EGL_OPENGL_ES_API) {')
            print(r'                EGLint client_version = 1;')
            print(r'                _eglQueryContext(dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &client_version);')
            print(r'                if (tr->profile.api != glfeatures::API_GLES ||')
            print(r'                    tr->profile.major < client_version) {')
            print(r'                    std::string version = tr->profile.str();')
            print(r'                    os::log("apitrace: warning: eglMakeCurrent: expected OpenGL ES %i.x context, but got %s\n",')
            print(r'                            client_version, version.c_str());')
            print(r'                }')
            print(r'            } else {')
            print(r'                assert(0);')
            print(r'            }')
            print(r'        } else {')
            print(r'            gltrace::clearContext();')
            print(r'        }')
            print(r'    }')

        if function.name == 'eglDestroyContext':
            print('    if (_result) {')
            print('        gltrace::releaseContext((uintptr_t)ctx);')
            print('    }')

        if function.name == 'glEGLImageTargetTexture2DOES':
            print('    image_info *info = _EGLImageKHR_get_image_info(target, image);')
            print('    if (info) {')
            print('        GLint level = 0;')
            print('        GLint internalformat = info->internalformat;')
            print('        GLsizei width = info->width;')
            print('        GLsizei height = info->height;')
            print('        GLint border = 0;')
            print('        GLenum format = info->format;')
            print('        GLenum type = info->type;')
            print('        const GLvoid * pixels = info->pixels;')
            self.emitFakeTexture2D()
            print('        _EGLImageKHR_free_image_info(info);')
            print('    }')


if __name__ == '__main__':
    print('#include <stdlib.h>')
    print('#include <string.h>')
    print()
    print('#include "trace_writer_local.hpp"')
    print()
    print('// To validate our prototypes')
    print('#define GL_GLEXT_PROTOTYPES')
    print('#define EGL_EGLEXT_PROTOTYPES')
    print()
    print('#include "glproc.hpp"')
    print('#include "glsize.hpp"')
    print('#include "eglsize.hpp"')
    print()
    
    module = Module()
    module.mergeModule(eglapi)
    module.mergeModule(glapi)
    api = API()
    api.addModule(module)
    tracer = EglTracer()
    tracer.traceApi(api)

    print(r'''
#if defined(ANDROID)

/*
 * Undocumented Android extensions used by the wrappers which have bound
 * information passed to it, but is currently ignored, so probably unreliable.
 *
 * See:
 * https://github.com/android/platform_frameworks_base/search?q=glVertexPointerBounds
 */

extern "C" PUBLIC
void APIENTRY glColorPointerBounds(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer, GLsizei count) {
    (void)count;
    glColorPointer(size, type, stride, pointer);
}

extern "C" PUBLIC
void APIENTRY glNormalPointerBounds(GLenum type, GLsizei stride, const GLvoid * pointer, GLsizei count) {
    (void)count;
    glNormalPointer(type, stride, pointer);
}

extern "C" PUBLIC
void APIENTRY glTexCoordPointerBounds(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer, GLsizei count) {
    (void)count;
    glTexCoordPointer(size, type, stride, pointer);
}

extern "C" PUBLIC
void APIENTRY glVertexPointerBounds(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer, GLsizei count) {
    (void)count;
    glVertexPointer(size, type, stride, pointer);
}

extern "C" PUBLIC
void GL_APIENTRY glPointSizePointerOESBounds(GLenum type, GLsizei stride, const GLvoid *pointer, GLsizei count) {
    (void)count;
    glPointSizePointerOES(type, stride, pointer);
}

extern "C" PUBLIC
void APIENTRY glMatrixIndexPointerOESBounds(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLsizei count) {
    (void)count;
    glMatrixIndexPointerOES(size, type, stride, pointer);
}

extern "C" PUBLIC
void APIENTRY glWeightPointerOESBounds(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLsizei count) {
    (void)count;
    glWeightPointerOES(size, type, stride, pointer);
}

/*
 * There is also a glVertexAttribPointerBounds in
 * https://github.com/android/platform_frameworks_base/blob/master/opengl/tools/glgen/stubs/gles11/GLES20cHeader.cpp
 * but is it not exported.
 */

#endif /* ANDROID */
''')

##########################################################################
#
# Copyright 2010 VMware, Inc.
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


"""Generated an header, glproc.hpp, which does pretty much what GLEW does, but
covers all the functions we support.
""" 


import sys

from dispatch import Dispatcher
import specs.stdapi as stdapi
from specs.glapi import glapi
from specs.glxapi import glxapi
from specs.wglapi import wglapi
from specs.cglapi import cglapi
from specs.eglapi import eglapi
from specs.glesapi import glesapi


# See http://www.opengl.org/registry/ABI/
public_symbols = set([
    # GL 1.2 and ARB_multitexture
    "glAccum",
    "glAlphaFunc",
    "glAreTexturesResident",
    "glArrayElement",
    "glBegin",
    "glBindTexture",
    "glBitmap",
    "glBlendFunc",
    "glCallList",
    "glCallLists",
    "glClear",
    "glClearAccum",
    "glClearColor",
    "glClearDepth",
    "glClearIndex",
    "glClearStencil",
    "glClipPlane",
    "glColor3b",
    "glColor3bv",
    "glColor3d",
    "glColor3dv",
    "glColor3f",
    "glColor3fv",
    "glColor3i",
    "glColor3iv",
    "glColor3s",
    "glColor3sv",
    "glColor3ub",
    "glColor3ubv",
    "glColor3ui",
    "glColor3uiv",
    "glColor3us",
    "glColor3usv",
    "glColor4b",
    "glColor4bv",
    "glColor4d",
    "glColor4dv",
    "glColor4f",
    "glColor4fv",
    "glColor4i",
    "glColor4iv",
    "glColor4s",
    "glColor4sv",
    "glColor4ub",
    "glColor4ubv",
    "glColor4ui",
    "glColor4uiv",
    "glColor4us",
    "glColor4usv",
    "glColorMask",
    "glColorMaterial",
    "glColorPointer",
    "glCopyPixels",
    "glCopyTexImage1D",
    "glCopyTexImage2D",
    "glCopyTexSubImage1D",
    "glCopyTexSubImage2D",
    "glCullFace",
    "glDeleteLists",
    "glDeleteTextures",
    "glDepthFunc",
    "glDepthMask",
    "glDepthRange",
    "glDisable",
    "glDisableClientState",
    "glDrawArrays",
    "glDrawBuffer",
    "glDrawElements",
    "glDrawPixels",
    "glEdgeFlag",
    "glEdgeFlagPointer",
    "glEdgeFlagv",
    "glEnable",
    "glEnableClientState",
    "glEnd",
    "glEndList",
    "glEvalCoord1d",
    "glEvalCoord1dv",
    "glEvalCoord1f",
    "glEvalCoord1fv",
    "glEvalCoord2d",
    "glEvalCoord2dv",
    "glEvalCoord2f",
    "glEvalCoord2fv",
    "glEvalMesh1",
    "glEvalMesh2",
    "glEvalPoint1",
    "glEvalPoint2",
    "glFeedbackBuffer",
    "glFinish",
    "glFlush",
    "glFogf",
    "glFogfv",
    "glFogi",
    "glFogiv",
    "glFrontFace",
    "glFrustum",
    "glGenLists",
    "glGenTextures",
    "glGetBooleanv",
    "glGetClipPlane",
    "glGetDoublev",
    "glGetError",
    "glGetFloatv",
    "glGetIntegerv",
    "glGetLightfv",
    "glGetLightiv",
    "glGetMapdv",
    "glGetMapfv",
    "glGetMapiv",
    "glGetMaterialfv",
    "glGetMaterialiv",
    "glGetPixelMapfv",
    "glGetPixelMapuiv",
    "glGetPixelMapusv",
    "glGetPointerv",
    "glGetPolygonStipple",
    "glGetString",
    "glGetTexEnvfv",
    "glGetTexEnviv",
    "glGetTexGendv",
    "glGetTexGenfv",
    "glGetTexGeniv",
    "glGetTexImage",
    "glGetTexLevelParameterfv",
    "glGetTexLevelParameteriv",
    "glGetTexParameterfv",
    "glGetTexParameteriv",
    "glHint",
    "glIndexMask",
    "glIndexPointer",
    "glIndexd",
    "glIndexdv",
    "glIndexf",
    "glIndexfv",
    "glIndexi",
    "glIndexiv",
    "glIndexs",
    "glIndexsv",
    "glIndexub",
    "glIndexubv",
    "glInitNames",
    "glInterleavedArrays",
    "glIsEnabled",
    "glIsList",
    "glIsTexture",
    "glLightModelf",
    "glLightModelfv",
    "glLightModeli",
    "glLightModeliv",
    "glLightf",
    "glLightfv",
    "glLighti",
    "glLightiv",
    "glLineStipple",
    "glLineWidth",
    "glListBase",
    "glLoadIdentity",
    "glLoadMatrixd",
    "glLoadMatrixf",
    "glLoadName",
    "glLogicOp",
    "glMap1d",
    "glMap1f",
    "glMap2d",
    "glMap2f",
    "glMapGrid1d",
    "glMapGrid1f",
    "glMapGrid2d",
    "glMapGrid2f",
    "glMaterialf",
    "glMaterialfv",
    "glMateriali",
    "glMaterialiv",
    "glMatrixMode",
    "glMultMatrixd",
    "glMultMatrixf",
    "glNewList",
    "glNormal3b",
    "glNormal3bv",
    "glNormal3d",
    "glNormal3dv",
    "glNormal3f",
    "glNormal3fv",
    "glNormal3i",
    "glNormal3iv",
    "glNormal3s",
    "glNormal3sv",
    "glNormalPointer",
    "glOrtho",
    "glPassThrough",
    "glPixelMapfv",
    "glPixelMapuiv",
    "glPixelMapusv",
    "glPixelStoref",
    "glPixelStorei",
    "glPixelTransferf",
    "glPixelTransferi",
    "glPixelZoom",
    "glPointSize",
    "glPolygonMode",
    "glPolygonOffset",
    "glPolygonStipple",
    "glPopAttrib",
    "glPopClientAttrib",
    "glPopMatrix",
    "glPopName",
    "glPrioritizeTextures",
    "glPushAttrib",
    "glPushClientAttrib",
    "glPushMatrix",
    "glPushName",
    "glRasterPos2d",
    "glRasterPos2dv",
    "glRasterPos2f",
    "glRasterPos2fv",
    "glRasterPos2i",
    "glRasterPos2iv",
    "glRasterPos2s",
    "glRasterPos2sv",
    "glRasterPos3d",
    "glRasterPos3dv",
    "glRasterPos3f",
    "glRasterPos3fv",
    "glRasterPos3i",
    "glRasterPos3iv",
    "glRasterPos3s",
    "glRasterPos3sv",
    "glRasterPos4d",
    "glRasterPos4dv",
    "glRasterPos4f",
    "glRasterPos4fv",
    "glRasterPos4i",
    "glRasterPos4iv",
    "glRasterPos4s",
    "glRasterPos4sv",
    "glReadBuffer",
    "glReadPixels",
    "glRectd",
    "glRectdv",
    "glRectf",
    "glRectfv",
    "glRecti",
    "glRectiv",
    "glRects",
    "glRectsv",
    "glRenderMode",
    "glRotated",
    "glRotatef",
    "glScaled",
    "glScalef",
    "glScissor",
    "glSelectBuffer",
    "glShadeModel",
    "glStencilFunc",
    "glStencilMask",
    "glStencilOp",
    "glTexCoord1d",
    "glTexCoord1dv",
    "glTexCoord1f",
    "glTexCoord1fv",
    "glTexCoord1i",
    "glTexCoord1iv",
    "glTexCoord1s",
    "glTexCoord1sv",
    "glTexCoord2d",
    "glTexCoord2dv",
    "glTexCoord2f",
    "glTexCoord2fv",
    "glTexCoord2i",
    "glTexCoord2iv",
    "glTexCoord2s",
    "glTexCoord2sv",
    "glTexCoord3d",
    "glTexCoord3dv",
    "glTexCoord3f",
    "glTexCoord3fv",
    "glTexCoord3i",
    "glTexCoord3iv",
    "glTexCoord3s",
    "glTexCoord3sv",
    "glTexCoord4d",
    "glTexCoord4dv",
    "glTexCoord4f",
    "glTexCoord4fv",
    "glTexCoord4i",
    "glTexCoord4iv",
    "glTexCoord4s",
    "glTexCoord4sv",
    "glTexCoordPointer",
    "glTexEnvf",
    "glTexEnvfv",
    "glTexEnvi",
    "glTexEnviv",
    "glTexGend",
    "glTexGendv",
    "glTexGenf",
    "glTexGenfv",
    "glTexGeni",
    "glTexGeniv",
    "glTexImage1D",
    "glTexImage2D",
    "glTexParameterf",
    "glTexParameterfv",
    "glTexParameteri",
    "glTexParameteriv",
    "glTexSubImage1D",
    "glTexSubImage2D",
    "glTranslated",
    "glTranslatef",
    "glVertex2d",
    "glVertex2dv",
    "glVertex2f",
    "glVertex2fv",
    "glVertex2i",
    "glVertex2iv",
    "glVertex2s",
    "glVertex2sv",
    "glVertex3d",
    "glVertex3dv",
    "glVertex3f",
    "glVertex3fv",
    "glVertex3i",
    "glVertex3iv",
    "glVertex3s",
    "glVertex3sv",
    "glVertex4d",
    "glVertex4dv",
    "glVertex4f",
    "glVertex4fv",
    "glVertex4i",
    "glVertex4iv",
    "glVertex4s",
    "glVertex4sv",
    "glVertexPointer",
    "glViewport",

    # GLX 1.3 and GLX_ARB_get_proc_address
    "glXChooseVisual",
    "glXCreateContext",
    "glXDestroyContext",
    "glXMakeCurrent",
    "glXCopyContext",
    "glXSwapBuffers",
    "glXCreateGLXPixmap",
    "glXDestroyGLXPixmap",
    "glXQueryExtension",
    "glXQueryVersion",
    "glXIsDirect",
    "glXGetConfig",
    "glXGetCurrentContext",
    "glXGetCurrentDrawable",
    "glXWaitGL",
    "glXWaitX",
    "glXUseXFont",
    "glXQueryExtensionsString",
    "glXQueryServerString",
    "glXGetClientString",
    "glXGetCurrentDisplay",
    "glXChooseFBConfig",
    "glXGetFBConfigAttrib",
    "glXGetFBConfigs",
    "glXGetVisualFromFBConfig",
    "glXCreateWindow",
    "glXDestroyWindow",
    "glXCreatePixmap",
    "glXDestroyPixmap",
    "glXCreatePbuffer",
    "glXDestroyPbuffer",
    "glXQueryDrawable",
    "glXCreateNewContext",
    "glXMakeContextCurrent",
    "glXGetCurrentReadDrawable",
    "glXQueryContext",
    "glXSelectEvent",
    "glXGetSelectedEvent",
    "glXGetProcAddressARB",
    "glXGetProcAddress",
    
    # WGL
    #"glDebugEntry",
    "wglChoosePixelFormat",
    "wglCopyContext",
    "wglCreateContext",
    "wglCreateLayerContext",
    "wglDeleteContext",
    "wglDescribeLayerPlane",
    "wglDescribePixelFormat",
    "wglGetCurrentContext",
    "wglGetCurrentDC",
    "wglGetDefaultProcAddress",
    "wglGetLayerPaletteEntries",
    "wglGetPixelFormat",
    "wglGetProcAddress",
    "wglMakeCurrent",
    "wglRealizeLayerPalette",
    "wglSetLayerPaletteEntries",
    "wglSetPixelFormat",
    "wglShareLists",
    "wglSwapBuffers",
    "wglSwapLayerBuffers",
    "wglSwapMultipleBuffers",
    "wglUseFontBitmapsA",
    "wglUseFontBitmapsW",
    "wglUseFontOutlinesA",
    "wglUseFontOutlinesW",

])

# EGL 1.4
public_symbols.update([
   "eglBindAPI",
   "eglBindTexImage",
   "eglChooseConfig",
   "eglCopyBuffers",
   "eglCreateContext",
   "eglCreatePbufferFromClientBuffer",
   "eglCreatePbufferSurface",
   "eglCreatePixmapSurface",
   "eglCreateWindowSurface",
   "eglDestroyContext",
   "eglDestroySurface",
   "eglGetConfigAttrib",
   "eglGetConfigs",
   "eglGetCurrentContext",
   "eglGetCurrentDisplay",
   "eglGetCurrentSurface",
   "eglGetDisplay",
   "eglGetError",
   "eglGetProcAddress",
   "eglInitialize",
   "eglMakeCurrent",
   "eglQueryAPI",
   "eglQueryContext",
   "eglQueryString",
   "eglQuerySurface",
   "eglReleaseTexImage",
   "eglReleaseThread",
   "eglSurfaceAttrib",
   "eglSwapBuffers",
   "eglSwapInterval",
   "eglTerminate",
   "eglWaitClient",
   "eglWaitGL",
   "eglWaitNative",
])

class GlDispatcher(Dispatcher):

    def isFunctionPublic(self, module, function):
        return function.name in public_symbols or function.name.startswith('CGL')

    def getProcAddressName(self, module, function):
        if self.isFunctionPublic(module, function):
            return '_getPublicProcAddress'
        else:
            return '_getPrivateProcAddress'

    def failFunction(self, function):
        # We fake these when they are not available
        if function.name in (
            # GL_KHR_debug
            'glDebugMessageControl',
            'glDebugMessageInsert',
            'glDebugMessageCallback',
            'glPushDebugGroup',
            'glPopDebugGroup',
            'glObjectLabel',
            'glObjectPtrLabel',
            # GL_ARB_debug_output
            'glDebugMessageControlARB',
            'glDebugMessageInsertARB',
            'glDebugMessageCallbackARB',
            # GL_AMD_debug_output
            'glDebugMessageEnableAMD',
            'glDebugMessageInsertAMD',
            'glDebugMessageCallbackAMD',
            # GL_EXT_debug_label
            'glLabelObjectEXT',
            # GL_EXT_debug_marker
            'glInsertEventMarkerEXT',
            'glPushGroupMarkerEXT',
            'glPopGroupMarkerEXT',
        ):
            return
        if function.name in ('glGetObjectLabel', 'glGetObjectPtrLabel', 'glGetObjectLabelEXT'):
            print r'    if (length != 0) *length = 0;'
            print r'    if (label != 0 && bufSize > 0) *label = 0;'
            return
        if function.name in ('glGetDebugMessageLog', 'glGetDebugMessageLogARB'):
            print r'    if (sources != 0) *sources = 0;'
            print r'    if (types != 0) *types = 0;'
            print r'    if (ids != 0) *ids = 0;'
            print r'    if (severities != 0) *severities = 0;'
            print r'    if (lengths != 0) *lengths = 0;'
            print r'    if (messageLog != 0 && bufsize > 0) *messageLog = 0;'
            print r'    return 0;'
            return
        if function.name in ('glGetDebugMessageLogAMD'):
            print r'    if (categories != 0) *categories = 0;'
            print r'    if (ids != 0) *ids = 0;'
            print r'    if (severities != 0) *severities = 0;'
            print r'    if (lengths != 0) *lengths = 0;'
            print r'    if (message != 0 && bufsize > 0) *message = 0;'
            print r'    return 0;'
            return

        Dispatcher.failFunction(self, function)


if __name__ == '__main__':
    decl, impl = sys.argv[1:]

    sys.stdout = open(decl, 'wt')
    print
    print '#ifndef _GLPROC_HPP_'
    print '#define _GLPROC_HPP_'
    print
    print '#include "glimports.hpp"'
    print
    print '#if defined(_WIN32)'
    print 'extern HMODULE _libGlHandle;'
    print '#else'
    print 'extern void * _libGlHandle;'
    print '#endif'
    print
    print 'void * _getPublicProcAddress(const char *procName);'
    print 'void * _getPrivateProcAddress(const char *procName);'
    print
    dispatcher = GlDispatcher()
    print
    dispatcher.dispatchModuleDecl(eglapi)
    print
    print '#if defined(_WIN32)'
    print
    dispatcher.dispatchModuleDecl(wglapi)
    print
    print '#elif defined(__APPLE__)'
    print
    dispatcher.dispatchModuleDecl(cglapi)
    print
    print '#elif defined(HAVE_X11)'
    print
    dispatcher.dispatchModuleDecl(glxapi)
    print
    print '#endif'
    print
    dispatcher.dispatchModuleDecl(glapi)
    print
    dispatcher.dispatchModuleDecl(glesapi)
    print
    print '#endif /* !_GLPROC_HPP_ */'
    print

    sys.stdout = open(impl, 'wt')
    print
    print '#include "glproc.hpp"'
    print '#include "os.hpp"'
    print
    dispatcher = GlDispatcher()
    print
    dispatcher.dispatchModuleImpl(eglapi)
    print
    print '#if defined(_WIN32)'
    print
    dispatcher.dispatchModuleImpl(wglapi)
    print
    print '#elif defined(__APPLE__)'
    print
    dispatcher.dispatchModuleImpl(cglapi)
    print
    print '#elif defined(HAVE_X11)'
    print
    dispatcher.dispatchModuleImpl(glxapi)
    print
    print '#endif'
    print
    dispatcher.dispatchModuleImpl(glapi)
    print
    dispatcher.dispatchModuleImpl(glesapi)
    print

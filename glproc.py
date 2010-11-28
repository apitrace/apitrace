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


import stdapi
from glapi import glapi
from glxapi import glxapi
from wglapi import wglapi


public_symbols = set([
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
#	"glDebugEntry",
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


class Dispatcher:

    def function_pointer_type(self, function):
        return '__PFN' + function.name.upper()

    def function_pointer_value(self, function):
        return '__' + function.name + '_ptr'

    def dispatch_function(self, function):
        if function.name in public_symbols:
            return

        ptype = self.function_pointer_type(function)
        pvalue = self.function_pointer_value(function)
        print 'typedef ' + function.prototype('* %s' % ptype) + ';'
        print 'static %s %s = NULL;' % (ptype, pvalue)
        print
        print 'static inline ' + function.prototype('__' + function.name) + ' {'
        if function.type is stdapi.Void:
            ret = ''
        else:
            ret = 'return '
        self.get_true_pointer(function)
        pvalue = self.function_pointer_value(function)
        print '    %s%s(%s);' % (ret, pvalue, ', '.join([str(arg.name) for arg in function.args]))
        print '}'
        print
        print '#define %s __%s' % (function.name, function.name)

    def get_true_pointer(self, function):
        ptype = self.function_pointer_type(function)
        pvalue = self.function_pointer_value(function)
        if function.name in public_symbols:
            get_proc_address = '__getProcAddress'
        else:
            get_proc_address = '__glGetProcAddress'
        print '    if (!%s) {' % (pvalue,)
        print '        %s = (%s)%s("%s");' % (pvalue, ptype, get_proc_address, function.name)
        print '        if (!%s) {' % (pvalue,)
        self.fail_function(function)
        print '        }'
        print '    }'

    def fail_function(self, function):
        print '    std::cerr << "error: unavailable function \\"%s\\"\\n";' % function.name
        if function.fail is not None:
            if function.type is stdapi.Void:
                assert function.fail == ''
                print '            return;' 
            else:
                assert function.fail != ''
                print '            return %s;' % function.fail
        else:
            print '            abort();'


if __name__ == '__main__':
    print
    print '#include "glimports.hpp"'
    print '#include "os.hpp"'
    print
    if 0:
        print 'typedef void (*__PROC)(void);'
        print
        print 'static __PROC __getProcAddress(const char *name);'
        print
        print 'static __PROC __glGetProcAddress(const char *name);'
    else:
        print '#ifdef WIN32'
        print '#define __glGetProcAddress wglGetProcAddress'
        print '#else'
        print '#define __glGetProcAddress(name) glXGetProcAddress((const GLubyte *)(name))'
        print '#endif'
    print
    dispatcher = Dispatcher()
    for function in glapi.functions:
        dispatcher.dispatch_function(function)
    print
    if 0:
        print '''

#ifdef WIN32

static HINSTANCE g_hDll = NULL;

static __PROC
__getProcAddress(const char *name)
{
    if (!g_hDll) {
        char szDll[MAX_PATH] = {0};
        
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        
        strcat(szDll, "\\\\opengl32.dll");
        
        g_hDll = LoadLibraryA(szDll);
        if (!g_hDll) {
            return NULL;
        }
    }
        
    return GetProcAddress(g_hDll, name);
}

static inline __PROC
__glGetProcAddress(const char *name) {
    return __wglGetProcAddress(name);
}

#else

static void g_module = NULL;

static __PROC
__getProcAddress(const char *name)
{
    if (!g_module) {
        g_module = dlopen("libGL.so", RTLD_LAZY);
        if (!g_module) {
            return NULL;
        }
    }
        
    return (__PROC)dlsym(g_module, name);
}

static inline __PROC
__glGetProcAddress(const char *name) {
    return __glXGetProcAddress(name);
}

#endif

    '''

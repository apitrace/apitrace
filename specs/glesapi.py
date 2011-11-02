##########################################################################
#
# Copyright 2011 LunarG, Inc.
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


"""GLES API description.
"""


from gltypes import *


GLfixed = Alias("GLfixed", Int32)
GLclampx = Alias("GLclampx", Int32)


def GlFunction(*args, **kwargs):
    kwargs.setdefault('call', 'GL_APIENTRY')
    return Function(*args, **kwargs)


glesapi = API('GLES')


# OpenGL ES specific functions
glesapi.add_functions([
    # GL_VERSION_ES_CM_1_1: GL_OES_single_precision
    GlFunction(Void, "glFrustumf", [(GLfloat, "left"), (GLfloat, "right"), (GLfloat, "bottom"), (GLfloat, "top"), (GLfloat, "zNear"), (GLfloat, "zFar")]),
    GlFunction(Void, "glOrthof", [(GLfloat, "left"), (GLfloat, "right"), (GLfloat, "bottom"), (GLfloat, "top"), (GLfloat, "zNear"), (GLfloat, "zFar")]),

    GlFunction(Void, "glClipPlanef", [(GLenum, "plane"), (Array(Const(GLfloat), 4), "equation")]),
    GlFunction(Void, "glGetClipPlanef", [(GLenum, "plane"), Out(Array(GLfloat, 4), "equation")], sideeffects=False),

    # GL_VERSION_ES_CM_1_1: GL_OES_fixed_point
    GlFunction(Void, "glAlphaFuncx", [(GLenum, "func"), (GLclampx, "ref")]),
    GlFunction(Void, "glClearColorx", [(GLclampx, "red"), (GLclampx, "green"), (GLclampx, "blue"), (GLclampx, "alpha")]),
    GlFunction(Void, "glClearDepthx", [(GLclampx, "depth")]),
    GlFunction(Void, "glColor4x", [(GLfixed, "red"), (GLfixed, "green"), (GLfixed, "blue"), (GLfixed, "alpha")]),
    GlFunction(Void, "glDepthRangex", [(GLclampx, "zNear"), (GLclampx, "zFar")]),
    GlFunction(Void, "glFogx", [(GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glFogxv", [(GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glFrustumx", [(GLfixed, "left"), (GLfixed, "right"), (GLfixed, "bottom"), (GLfixed, "top"), (GLfixed, "zNear"), (GLfixed, "zFar")]),
    GlFunction(Void, "glLightModelx", [(GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glLightModelxv", [(GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glLightx", [(GLenum, "light"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glLightxv", [(GLenum, "light"), (GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glLineWidthx", [(GLfixed, "width")]),
    GlFunction(Void, "glLoadMatrixx", [(Array(Const(GLfixed), 16), "m")]),
    GlFunction(Void, "glMaterialx", [(GLenum, "face"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glMaterialxv", [(GLenum, "face"), (GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glMultMatrixx", [(Array(Const(GLfixed), 16), "m")]),
    GlFunction(Void, "glMultiTexCoord4x", [(GLenum, "target"), (GLfixed, "s"), (GLfixed, "t"), (GLfixed, "r"), (GLfixed, "q")]),
    GlFunction(Void, "glNormal3x", [(GLfixed, "nx"), (GLfixed, "ny"), (GLfixed, "nz")]),
    GlFunction(Void, "glOrthox", [(GLfixed, "left"), (GLfixed, "right"), (GLfixed, "bottom"), (GLfixed, "top"), (GLfixed, "zNear"), (GLfixed, "zFar")]),
    GlFunction(Void, "glPointSizex", [(GLfixed, "size")]),
    GlFunction(Void, "glPolygonOffsetx", [(GLfixed, "factor"), (GLfixed, "units")]),
    GlFunction(Void, "glRotatex", [(GLfixed, "angle"), (GLfixed, "x"), (GLfixed, "y"), (GLfixed, "z")]),
    GlFunction(Void, "glSampleCoveragex", [(GLclampx, "value"), (GLboolean, "invert")]),
    GlFunction(Void, "glScalex", [(GLfixed, "x"), (GLfixed, "y"), (GLfixed, "z")]),
    GlFunction(Void, "glTexEnvx", [(GLenum, "target"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glTexEnvxv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glTexParameterx", [(GLenum, "target"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glTranslatex", [(GLfixed, "x"), (GLfixed, "y"), (GLfixed, "z")]),

    GlFunction(Void, "glClipPlanex", [(GLenum, "plane"), (Array(Const(GLfixed), 4), "equation")]),
    GlFunction(Void, "glGetClipPlanex", [(GLenum, "plane"), Out(Array(GLfixed, 4), "equation")], sideeffects=False),
    GlFunction(Void, "glGetFixedv", [(GLenum, "pname"), Out(Array(GLfixed, "__gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetLightxv", [(GLenum, "light"), (GLenum, "pname"), Out(Array(GLfixed, "__gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetMaterialxv", [(GLenum, "face"), (GLenum, "pname"), Out(Array(GLfixed, "__gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetTexEnvxv", [(GLenum, "target"), (GLenum, "pname"), Out(Array(GLfixed, "__gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetTexParameterxv", [(GLenum, "target"), (GLenum, "pname"), Out(Array(GLfixed, "__gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glPointParameterx", [(GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glPointParameterxv", [(GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glTexParameterxv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLfixed), "__gl_param_size(pname)"), "params")]),

    # GL_ES_VERSION_2_0

    # all functions are already defined in OpenGL thanks to
    # GL_ARB_ES2_compatibility
])

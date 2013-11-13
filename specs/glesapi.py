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


glesapi = Module('GLES')


# OpenGL ES specific functions
glesapi.addFunctions([
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
    GlFunction(Void, "glFogxv", [(GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glFrustumx", [(GLfixed, "left"), (GLfixed, "right"), (GLfixed, "bottom"), (GLfixed, "top"), (GLfixed, "zNear"), (GLfixed, "zFar")]),
    GlFunction(Void, "glLightModelx", [(GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glLightModelxv", [(GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glLightx", [(GLenum, "light"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glLightxv", [(GLenum, "light"), (GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glLineWidthx", [(GLfixed, "width")]),
    GlFunction(Void, "glLoadMatrixx", [(Array(Const(GLfixed), 16), "m")]),
    GlFunction(Void, "glMaterialx", [(GLenum, "face"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glMaterialxv", [(GLenum, "face"), (GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
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
    GlFunction(Void, "glTexEnvxv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glTexParameterx", [(GLenum, "target"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glTranslatex", [(GLfixed, "x"), (GLfixed, "y"), (GLfixed, "z")]),

    GlFunction(Void, "glClipPlanex", [(GLenum, "plane"), (Array(Const(GLfixed), 4), "equation")]),
    GlFunction(Void, "glGetClipPlanex", [(GLenum, "plane"), Out(Array(GLfixed, 4), "equation")], sideeffects=False),
    GlFunction(Void, "glGetFixedv", [(GLenum, "pname"), Out(Array(GLfixed, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetLightxv", [(GLenum, "light"), (GLenum, "pname"), Out(Array(GLfixed, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetMaterialxv", [(GLenum, "face"), (GLenum, "pname"), Out(Array(GLfixed, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetTexEnvxv", [(GLenum, "target"), (GLenum, "pname"), Out(Array(GLfixed, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetTexParameterxv", [(GLenum, "target"), (GLenum, "pname"), Out(Array(GLfixed, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glPointParameterx", [(GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glPointParameterxv", [(GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glTexParameterxv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),

    # GL_ES_VERSION_2_0

    # all functions are already defined in OpenGL thanks to
    # GL_ARB_ES2_compatibility

    # GL_OES_blend_equation_separate
    GlFunction(Void, "glBlendEquationSeparateOES", [(GLenum, "modeRGB"), (GLenum, "modeAlpha")]),

    # GL_OES_blend_func_separate
    GlFunction(Void, "glBlendFuncSeparateOES", [(GLenum, "sfactorRGB"), (GLenum, "dfactorRGB"), (GLenum, "sfactorAlpha"), (GLenum, "dfactorAlpha")]),

    # GL_OES_blend_subtract
    GlFunction(Void, "glBlendEquationOES", [(GLenum, "mode")]),

    # GL_OES_framebuffer_object
    GlFunction(GLboolean, "glIsRenderbufferOES", [(GLrenderbuffer, "renderbuffer")], sideeffects=False),
    GlFunction(Void, "glBindRenderbufferOES", [(GLenum, "target"), (GLrenderbuffer, "renderbuffer")]),
    GlFunction(Void, "glDeleteRenderbuffersOES", [(GLsizei, "n"), (Array(Const(GLrenderbuffer), "n"), "renderbuffers")]),
    GlFunction(Void, "glGenRenderbuffersOES", [(GLsizei, "n"), Out(Array(GLrenderbuffer, "n"), "renderbuffers")]),
    GlFunction(Void, "glRenderbufferStorageOES", [(GLenum, "target"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
    GlFunction(Void, "glGetRenderbufferParameterivOES", [(GLenum, "target"), (GLenum, "pname"), (Array(GLint, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(GLboolean, "glIsFramebufferOES", [(GLframebuffer, "framebuffer")], sideeffects=False),
    GlFunction(Void, "glBindFramebufferOES", [(GLenum, "target"), (GLframebuffer, "framebuffer")]),
    GlFunction(Void, "glDeleteFramebuffersOES", [(GLsizei, "n"), (Array(Const(GLframebuffer), "n"), "framebuffers")]),
    GlFunction(Void, "glGenFramebuffersOES", [(GLsizei, "n"), Out(Array(GLframebuffer, "n"), "framebuffers")]),
    GlFunction(GLenum, "glCheckFramebufferStatusOES", [(GLenum, "target")]),
    GlFunction(Void, "glFramebufferTexture2DOES", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLtexture, "texture"), (GLint, "level")]),
    GlFunction(Void, "glFramebufferRenderbufferOES", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "renderbuffertarget"), (GLuint, "renderbuffer")]),
    GlFunction(Void, "glGetFramebufferAttachmentParameterivOES", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "pname"), Out(Array(GLint, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGenerateMipmapOES", [(GLenum, "target")]),

    # GL_OES_matrix_palette
    GlFunction(Void, "glCurrentPaletteMatrixOES", [(GLuint, "index")]),
    GlFunction(Void, "glLoadPaletteFromModelViewMatrixOES", []),
    GlFunction(Void, "glMatrixIndexPointerOES", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (GLpointerConst, "pointer")]),
    GlFunction(Void, "glWeightPointerOES", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (GLpointerConst, "pointer")]),

    # GL_OES_point_size_array
    GlFunction(Void, "glPointSizePointerOES", [(GLenum, "type"), (GLsizei, "stride"), (GLpointerConst, "ptr")]),

    # GL_OES_query_matrix
    GlFunction(GLbitfield, "glQueryMatrixxOES", [(Array(GLfixed, 16), "mantissa"), (Array(GLint, 16), "exponent")]),

    # GL_OES_texture_cube_map
    GlFunction(Void, "glTexGenfOES", [(GLenum, "coord"), (GLenum, "pname"), (GLfloat, "param")]),
    GlFunction(Void, "glTexGenfvOES", [(GLenum, "coord"), (GLenum, "pname"), (Array(Const(GLfloat), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glTexGeniOES", [(GLenum, "coord"), (GLenum, "pname"), (GLint, "param")]),
    GlFunction(Void, "glTexGenivOES", [(GLenum, "coord"), (GLenum, "pname"), (Array(Const(GLint), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glTexGenxOES", [(GLenum, "coord"), (GLenum, "pname"), (GLfixed, "param")]),
    GlFunction(Void, "glTexGenxvOES", [(GLenum, "coord"), (GLenum, "pname"), (Array(Const(GLfixed), "_gl_param_size(pname)"), "params")]),
    GlFunction(Void, "glGetTexGenfvOES", [(GLenum, "coord"), (GLenum, "pname"), Out(Array(GLfloat, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetTexGenivOES", [(GLenum, "coord"), (GLenum, "pname"), Out(Array(GLint, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetTexGenxvOES", [(GLenum, "coord"), (GLenum, "pname"), Out(Array(GLfixed, "_gl_param_size(pname)"), "params")], sideeffects=False),

    # GL_OES_mapbuffer
    GlFunction(Void, "glGetBufferPointervOES", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLpointer), "params")], sideeffects=False),
    GlFunction(GLmap, "glMapBufferOES", [(GLenum, "target"), (GLenum, "access")]),
    GlFunction(GLboolean, "glUnmapBufferOES", [(GLenum, "target")]),

    # GL_OES_texture_3D
    GlFunction(Void, "glTexImage3DOES", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLint, "border"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "_glTexImage3D_size(format, type, width, height, depth)"), "pixels")]),
    GlFunction(Void, "glTexSubImage3DOES", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "zoffset"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "_glTexSubImage3D_size(format, type, width, height, depth)"), "pixels")]),
    GlFunction(Void, "glCopyTexSubImage3DOES", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "zoffset"), (GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height")]),
    GlFunction(Void, "glCompressedTexImage3DOES", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLint, "border"), (GLsizei, "imageSize"), (Blob(Const(GLvoid), "imageSize"), "data")]),
    GlFunction(Void, "glCompressedTexSubImage3DOES", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "zoffset"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLenum, "format"), (GLsizei, "imageSize"), (Blob(Const(GLvoid), "imageSize"), "data")]),
    GlFunction(Void, "glFramebufferTexture3DOES", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLtexture, "texture"), (GLint, "level"), (GLint, "zoffset")]),

    # GL_OES_get_program_binary
    GlFunction(Void, "glGetProgramBinaryOES", [(GLprogram, "program"), (GLsizei, "bufSize"), Out(Pointer(GLsizei), "length"), Out(Pointer(GLenum), "binaryFormat"), Out(OpaqueBlob(GLvoid, "length ? *length : bufSize"), "binary")], sideeffects=False),
    GlFunction(Void, "glProgramBinaryOES", [(GLprogram, "program"), (GLenum, "binaryFormat"), (Blob(Const(GLvoid), "length"), "binary"), (GLsizei, "length")]),

    # GL_OES_draw_texture
    GlFunction(Void, "glDrawTexfOES", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "width"), (GLfloat, "height")]),
    GlFunction(Void, "glDrawTexfvOES", [(Array(Const(GLfloat), 4), "coords")]),
    GlFunction(Void, "glDrawTexiOES", [(GLint, "x"), (GLint, "y"), (GLint, "z"), (GLint, "width"), (GLint, "height")]),
    GlFunction(Void, "glDrawTexivOES", [(Array(Const(GLint), 4), "coords")]),
    GlFunction(Void, "glDrawTexsOES", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z"), (GLshort, "width"), (GLshort, "height")]),
    GlFunction(Void, "glDrawTexsvOES", [(Array(Const(GLshort), 4), "coords")]),

    # GL_EXT_discard_framebuffer
    GlFunction(Void, "glDiscardFramebufferEXT", [(GLenum, "target"), (GLsizei, "numAttachments"), (Array(Const(GLenum), "numAttachments"), "attachments")]),

    # GL_OES_vertex_array_object
    GlFunction(Void, "glBindVertexArrayOES", [(GLarray, "array")]),
    GlFunction(Void, "glDeleteVertexArraysOES", [(GLsizei, "n"), (Array(Const(GLarray), "n"), "arrays")]),
    GlFunction(Void, "glGenVertexArraysOES", [(GLsizei, "n"), Out(Array(GLarray, "n"), "arrays")]),
    GlFunction(GLboolean, "glIsVertexArrayOES", [(GLarray, "array")], sideeffects=False),

    # GL_NV_coverage_sample
    GlFunction(Void, "glCoverageMaskNV", [(GLboolean, "mask")]),
    GlFunction(Void, "glCoverageOperationNV", [(GLenum, "operation")]),

    # GL_IMG_multisampled_render_to_texture
    GlFunction(Void, "glRenderbufferStorageMultisampleIMG", [(GLenum, "target"), (GLsizei, "samples"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
    GlFunction(Void, "glFramebufferTexture2DMultisampleIMG", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLtexture, "texture"), (GLint, "level"), (GLsizei, "samples")]),

    # GL_APPLE_framebuffer_multisample
    GlFunction(Void, "glRenderbufferStorageMultisampleAPPLE", [(GLenum, "target"), (GLsizei, "samples"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
    GlFunction(Void, "glResolveMultisampleFramebufferAPPLE", []),

    # GL_ANGLE_framebuffer_blit
    GlFunction(Void, "glBlitFramebufferANGLE", [(GLint, "srcX0"), (GLint, "srcY0"), (GLint, "srcX1"), (GLint, "srcY1"), (GLint, "dstX0"), (GLint, "dstY0"), (GLint, "dstX1"), (GLint, "dstY1"), (GLbitfield_attrib, "mask"), (GLenum, "filter")]),

    # GL_ANGLE_framebuffer_multisample
    GlFunction(Void, "glRenderbufferStorageMultisampleANGLE", [(GLenum, "target"), (GLsizei, "samples"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),

    # GL_NV_draw_buffers
    GlFunction(Void, "glDrawBuffersNV", [(GLsizei, "n"), (Array(Const(GLenum), "n"), "bufs")]),

    # GL_NV_read_buffer
    GlFunction(Void, "glReadBufferNV", [(GLenum, "mode")]),

    # GL_EXT_debug_label
    GlFunction(Void, "glLabelObjectEXT", [(GLenum, "type"), (GLuint, "object"), (GLsizei, "length"), (GLstringConst, "label")]),
    GlFunction(Void, "glGetObjectLabelEXT", [(GLenum, "type"), (GLuint, "object"), (GLsizei, "bufSize"), Out(Pointer(GLsizei), "length"), Out(GLstring, "label")], sideeffects=False),

    # GL_EXT_debug_marker
    GlFunction(Void, "glInsertEventMarkerEXT", [(GLsizei, "length"), (String(Const(GLchar), "length ? length : strlen(marker)"), "marker")], sideeffects=False),
    GlFunction(Void, "glPushGroupMarkerEXT", [(GLsizei, "length"), (String(Const(GLchar), "length ? length : strlen(marker)"), "marker")], sideeffects=False),
    GlFunction(Void, "glPopGroupMarkerEXT", [], sideeffects=False),

    # GL_EXT_occlusion_query_boolean
    GlFunction(Void, "glGenQueriesEXT", [(GLsizei, "n"), Out(Array(GLquery, "n"), "ids")]),
    GlFunction(Void, "glDeleteQueriesEXT", [(GLsizei, "n"), (Array(Const(GLquery), "n"), "ids")]),
    GlFunction(GLboolean, "glIsQueryEXT", [(GLquery, "id")], sideeffects=False),
    GlFunction(Void, "glBeginQueryEXT", [(GLenum, "target"), (GLquery, "id")]),
    GlFunction(Void, "glEndQueryEXT", [(GLenum, "target")]),
    GlFunction(Void, "glGetQueryivEXT", [(GLenum, "target"), (GLenum, "pname"), Out(Array(GLint, "_gl_param_size(pname)"), "params")], sideeffects=False),
    GlFunction(Void, "glGetQueryObjectuivEXT", [(GLquery, "id"), (GLenum, "pname"), Out(Array(GLuint, "_gl_param_size(pname)"), "params")], sideeffects=False),

    # GL_EXT_separate_shader_objects
    GlFunction(Void, "glUseProgramStagesEXT", [(GLpipeline, "pipeline"), (GLbitfield_shader, "stages"), (GLprogram, "program")]),
    GlFunction(Void, "glActiveShaderProgramEXT", [(GLpipeline, "pipeline"), (GLprogram, "program")]),
    GlFunction(GLprogram, "glCreateShaderProgramvEXT", [(GLenum, "type"), (GLsizei, "count"), (Const(Array(GLstringConst, "count")), "strings")]),
    GlFunction(Void, "glBindProgramPipelineEXT", [(GLpipeline, "pipeline")]),
    GlFunction(Void, "glDeleteProgramPipelinesEXT", [(GLsizei, "n"), (Array(Const(GLuint), "n"), "pipelines")]),
    GlFunction(Void, "glGenProgramPipelinesEXT", [(GLsizei, "n"), Out(Array(GLpipeline, "n"), "pipelines")]),
    GlFunction(GLboolean, "glIsProgramPipelineEXT", [(GLpipeline, "pipeline")], sideeffects=False),
    #GlFunction(Void, "glProgramParameteriEXT", [(GLprogram, "program"), (GLenum, "pname"), (GLint, "value")]),
    GlFunction(Void, "glGetProgramPipelineivEXT", [(GLpipeline, "pipeline"), (GLenum, "pname"), Out(Array(GLint, "_gl_param_size(pname)"), "params")], sideeffects=False),
    #GlFunction(Void, "glProgramUniform1iEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLint, "x")]),
    #GlFunction(Void, "glProgramUniform2iEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLint, "x"), (GLint, "y")]),
    #GlFunction(Void, "glProgramUniform3iEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLint, "x"), (GLint, "y"), (GLint, "z")]),
    #GlFunction(Void, "glProgramUniform4iEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLint, "x"), (GLint, "y"), (GLint, "z"), (GLint, "w")]),
    #GlFunction(Void, "glProgramUniform1fEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLfloat, "x")]),
    #GlFunction(Void, "glProgramUniform2fEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLfloat, "x"), (GLfloat, "y")]),
    #GlFunction(Void, "glProgramUniform3fEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
    #GlFunction(Void, "glProgramUniform4fEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
    #GlFunction(Void, "glProgramUniform1ivEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLint), "count"), "value")]),
    #GlFunction(Void, "glProgramUniform2ivEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLint), "count*2"), "value")]),
    #GlFunction(Void, "glProgramUniform3ivEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLint), "count*3"), "value")]),
    #GlFunction(Void, "glProgramUniform4ivEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLint), "count*4"), "value")]),
    #GlFunction(Void, "glProgramUniform1fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLfloat), "count"), "value")]),
    #GlFunction(Void, "glProgramUniform2fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLfloat), "count*2"), "value")]),
    #GlFunction(Void, "glProgramUniform3fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLfloat), "count*3"), "value")]),
    #GlFunction(Void, "glProgramUniform4fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (Array(Const(GLfloat), "count*4"), "value")]),
    #GlFunction(Void, "glProgramUniformMatrix2fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (Array(Const(GLfloat), "count*2*2"), "value")]),
    #GlFunction(Void, "glProgramUniformMatrix3fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (Array(Const(GLfloat), "count*3*3"), "value")]),
    #GlFunction(Void, "glProgramUniformMatrix4fvEXT", [(GLprogram, "program"), (GLlocation, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (Array(Const(GLfloat), "count*4*4"), "value")]),
    GlFunction(Void, "glValidateProgramPipelineEXT", [(GLpipeline, "pipeline")]),
    GlFunction(Void, "glGetProgramPipelineInfoLogEXT", [(GLpipeline, "pipeline"), (GLsizei, "bufSize"), Out(Pointer(GLsizei), "length"), Out(GLstring, "infoLog")], sideeffects=False),

    # GL_EXT_multisampled_render_to_texture
    #GlFunction(Void, "glRenderbufferStorageMultisampleEXT", [(GLenum, "target"), (GLsizei, "samples"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
    GlFunction(Void, "glFramebufferTexture2DMultisampleEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLtexture, "texture"), (GLint, "level"), (GLsizei, "samples")]),

    # GL_EXT_texture_storage
    GlFunction(Void, "glTexStorage1DEXT", [(GLenum, "target"), (GLsizei, "levels"), (GLenum, "internalformat"), (GLsizei, "width")]),
    GlFunction(Void, "glTexStorage2DEXT", [(GLenum, "target"), (GLsizei, "levels"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
    GlFunction(Void, "glTexStorage3DEXT", [(GLenum, "target"), (GLsizei, "levels"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth")]),
    #GlFunction(Void, "glTextureStorage1DEXT", [(GLtexture, "texture"), (GLenum, "target"), (GLsizei, "levels"), (GLenum, "internalformat"), (GLsizei, "width")]),
    #GlFunction(Void, "glTextureStorage2DEXT", [(GLtexture, "texture"), (GLenum, "target"), (GLsizei, "levels"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
    #GlFunction(Void, "glTextureStorage3DEXT", [(GLtexture, "texture"), (GLenum, "target"), (GLsizei, "levels"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth")]),
])

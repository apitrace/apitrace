##########################################################################
#
# Copyright 2011 Jose Fonseca
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


'''Describe GL types.'''


import platform

from stdapi import *


GLboolean = Alias("GLboolean", Bool)
GLvoid = Alias("GLvoid", Void)
GLbyte = Alias("GLbyte", SChar)
GLshort = Alias("GLshort", Short)
GLint = Alias("GLint", Int)
GLint64 = Alias("GLint64", Int64)
GLubyte = Alias("GLubyte", UChar)
GLushort = Alias("GLushort", UShort)
GLuint = Alias("GLuint", UInt)
GLuint64 = Alias("GLuint64", UInt64)
GLsizei = Alias("GLsizei", Int)
GLintptr = Alias("GLintptr", Int)
GLsizeiptr = Alias("GLsizeiptr", Int)
GLfloat = Alias("GLfloat", Float)
GLclampf = Alias("GLclampf", Float)
GLdouble = Alias("GLdouble", Double)
GLclampd = Alias("GLclampd", Double)
GLchar = Alias("GLchar", SChar)
GLstring = String("GLchar *")

GLcharARB = Alias("GLcharARB", SChar)
GLstringARB = String("GLcharARB *")
GLintptrARB = Alias("GLintptrARB", Int)
GLsizeiptrARB = Alias("GLsizeiptrARB", Int)
GLhandleARB = Handle("handleARB", Alias("GLhandleARB", UInt))
GLhalfARB = Alias("GLhalfARB", UShort)
GLhalfNV = Alias("GLhalfNV", UShort)
GLint64EXT = Alias("GLint64EXT", Int64)
GLuint64EXT = Alias("GLuint64EXT", UInt64)
GLDEBUGPROCARB = Opaque("GLDEBUGPROCARB")
GLDEBUGPROCAMD = Opaque("GLDEBUGPROCAMD")

GLlist = Handle("list", GLuint)
GLtexture = Handle("texture", GLuint)
GLbuffer = Handle("buffer", GLuint)
GLquery = Handle("query", GLuint)
GLfenceNV = Handle("fenceNV", GLuint)
GLprogram = Handle("program", GLuint)
GLshader = Handle("shader", GLuint)
GLlocation = Handle("location", GLint, key=('program', GLuint))
GLlocationARB = Handle("locationARB", GLint, key=('programObj', GLhandleARB))
GLprogramARB = Handle("programARB", GLuint)
GLframebuffer = Handle("framebuffer", GLuint)
GLrenderbuffer = Handle("renderbuffer", GLuint)
GLfragmentShaderATI = Handle("fragmentShaderATI", GLuint)
GLarray = Handle("array", GLuint)
GLregion = Handle("region", GLuint)
GLmap = Handle("map", OpaquePointer(GLvoid))
GLpipeline = Handle("pipeline", GLuint)
GLsampler = Handle("sampler", GLuint)

GLsync_ = Opaque("GLsync")
GLsync = Handle("sync", GLsync_)

GLenum = Enum("GLenum", [
    # Parameters are added later from glparams.py's parameter table
])

# Some functions take GLenum disguised as GLint.  Apple noticed and fixed it in
# the gl.h header.  Regardless, C++ typechecking rules force the wrappers to
# match the prototype precisely.
if platform.system() == 'Darwin':
    GLenum_int = GLenum
else:
    GLenum_int = Alias("GLint", GLenum)

GLenum_mode = FakeEnum(GLenum, [
    "GL_POINTS",                         # 0x0000
    "GL_LINES",                          # 0x0001
    "GL_LINE_LOOP",                      # 0x0002
    "GL_LINE_STRIP",                     # 0x0003
    "GL_TRIANGLES",                      # 0x0004
    "GL_TRIANGLE_STRIP",                 # 0x0005
    "GL_TRIANGLE_FAN",                   # 0x0006
    "GL_QUADS",                          # 0x0007
    "GL_QUAD_STRIP",                     # 0x0008
    "GL_POLYGON",                        # 0x0009
    "GL_LINES_ADJACENCY",                # 0x000A
    "GL_LINE_STRIP_ADJACENCY",           # 0x000B
    "GL_TRIANGLES_ADJACENCY",            # 0x000C
    "GL_TRIANGLE_STRIP_ADJACENCY",       # 0x000D
    "GL_PATCHES",                        # 0x000E
])

GLenum_error = FakeEnum(GLenum, [
    "GL_NO_ERROR",                       # 0x0
    "GL_INVALID_ENUM",                   # 0x0500
    "GL_INVALID_VALUE",                  # 0x0501
    "GL_INVALID_OPERATION",              # 0x0502
    "GL_STACK_OVERFLOW",                 # 0x0503
    "GL_STACK_UNDERFLOW",                # 0x0504
    "GL_OUT_OF_MEMORY",                  # 0x0505
    "GL_INVALID_FRAMEBUFFER_OPERATION",  # 0x0506
    "GL_TABLE_TOO_LARGE",                # 0x8031
])

GLbitfield = Alias("GLbitfield", UInt)

GLbitfield_attrib = Flags(GLbitfield, [
    "GL_ALL_ATTRIB_BITS",     # 0x000FFFFF
    "GL_CURRENT_BIT",         # 0x00000001
    "GL_POINT_BIT",           # 0x00000002
    "GL_LINE_BIT",            # 0x00000004
    "GL_POLYGON_BIT",         # 0x00000008
    "GL_POLYGON_STIPPLE_BIT", # 0x00000010
    "GL_PIXEL_MODE_BIT",      # 0x00000020
    "GL_LIGHTING_BIT",        # 0x00000040
    "GL_FOG_BIT",             # 0x00000080
    "GL_DEPTH_BUFFER_BIT",    # 0x00000100
    "GL_ACCUM_BUFFER_BIT",    # 0x00000200
    "GL_STENCIL_BUFFER_BIT",  # 0x00000400
    "GL_VIEWPORT_BIT",        # 0x00000800
    "GL_TRANSFORM_BIT",       # 0x00001000
    "GL_ENABLE_BIT",          # 0x00002000
    "GL_COLOR_BUFFER_BIT",    # 0x00004000
    "GL_HINT_BIT",            # 0x00008000
    "GL_EVAL_BIT",            # 0x00010000
    "GL_LIST_BIT",            # 0x00020000
    "GL_TEXTURE_BIT",         # 0x00040000
    "GL_SCISSOR_BIT",         # 0x00080000
    "GL_MULTISAMPLE_BIT",     # 0x20000000
])

GLbitfield_client_attrib = Flags(GLbitfield, [
    "GL_CLIENT_ALL_ATTRIB_BITS",  # 0xFFFFFFFF
    "GL_CLIENT_PIXEL_STORE_BIT",  # 0x00000001
    "GL_CLIENT_VERTEX_ARRAY_BIT", # 0x00000002
])

GLbitfield_shader = Flags(GLbitfield, [
    "GL_ALL_SHADER_BITS",                        # 0xFFFFFFFF
    "GL_VERTEX_SHADER_BIT",                      # 0x00000001
    "GL_FRAGMENT_SHADER_BIT",                    # 0x00000002
    "GL_GEOMETRY_SHADER_BIT",                    # 0x00000004
    "GL_TESS_CONTROL_SHADER_BIT",                # 0x00000008
    "GL_TESS_EVALUATION_SHADER_BIT",             # 0x00000010
])

GLbitfield_access = Flags(GLbitfield, [
    "GL_MAP_READ_BIT",                # 0x0001
    "GL_MAP_WRITE_BIT",               # 0x0002
    "GL_MAP_INVALIDATE_RANGE_BIT",    # 0x0004
    "GL_MAP_INVALIDATE_BUFFER_BIT",   # 0x0008
    "GL_MAP_FLUSH_EXPLICIT_BIT",      # 0x0010
    "GL_MAP_UNSYNCHRONIZED_BIT",      # 0x0020
])

GLbitfield_sync_flush = Flags(GLbitfield, [
    "GL_SYNC_FLUSH_COMMANDS_BIT",				# 0x00000001
])

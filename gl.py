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

from base import *

GLboolean = Alias("GLboolean", Bool)
GLvoid = Alias("GLvoid", Void)
GLbyte = Alias("GLbyte", SChar)
GLshort = Alias("GLshort", Short)
GLint = Alias("GLint", Int)
GLubyte = Alias("GLubyte", UChar)
GLushort = Alias("GLushort", UShort)
GLuint = Alias("GLuint", UInt)
GLsizei = Alias("GLsizei", Int)
GLfloat = Alias("GLfloat", Float)
GLclampf = Alias("GLclampf", Float)
GLdouble = Alias("GLdouble", Double)
GLclampd = Alias("GLclampd", Double)
GLbitfield = Alias("GLbitfield", UInt)

GLchar = Alias("GLchar", SChar)
GLcharARB = Alias("GLcharARB", SChar)
GLintptr = Alias("GLintptr", Int)
GLsizeiptr = Alias("GLsizeiptr", Int)
GLintptrARB = Alias("GLintptrARB", Int)
GLsizeiptrARB = Alias("GLsizeiptrARB", Int)
GLhandleARB = Alias("GLhandleARB", UInt)
GLhalfARB = Alias("GLhalfARB", UShort)
GLhalfNV = Alias("GLhalfNV", UShort)
GLint64EXT = Alias("GLint64EXT", Long) # FIXME
GLuint64EXT = Alias("GLuint64EXT", ULong) # FIXME

GLstring = Alias("const GLchar *", String)
GLstringARB = Alias("const GLcharARB *", String)

GLenum = Enum("GLenum", [
    #"GL_NO_ERROR",                       # 0x0
    #"GL_NONE",                           # 0x0
    "GL_ZERO",                           # 0x0
    "GL_ONE",                            # 0x1

    "GL_ACCUM",                          # 0x0100
    "GL_LOAD",                           # 0x0101
    "GL_RETURN",                         # 0x0102
    "GL_MULT",                           # 0x0103
    "GL_ADD",                            # 0x0104
    "GL_NEVER",                          # 0x0200
    "GL_LESS",                           # 0x0201
    "GL_EQUAL",                          # 0x0202
    "GL_LEQUAL",                         # 0x0203
    "GL_GREATER",                        # 0x0204
    "GL_NOTEQUAL",                       # 0x0205
    "GL_GEQUAL",                         # 0x0206
    "GL_ALWAYS",                         # 0x0207
    "GL_SRC_COLOR",                      # 0x0300
    "GL_ONE_MINUS_SRC_COLOR",            # 0x0301
    "GL_SRC_ALPHA",                      # 0x0302
    "GL_ONE_MINUS_SRC_ALPHA",            # 0x0303
    "GL_DST_ALPHA",                      # 0x0304
    "GL_ONE_MINUS_DST_ALPHA",            # 0x0305
    "GL_DST_COLOR",                      # 0x0306
    "GL_ONE_MINUS_DST_COLOR",            # 0x0307
    "GL_SRC_ALPHA_SATURATE",             # 0x0308
    "GL_FRONT_LEFT",                     # 0x0400
    "GL_FRONT_RIGHT",                    # 0x0401
    "GL_BACK_LEFT",                      # 0x0402
    "GL_BACK_RIGHT",                     # 0x0403
    "GL_FRONT",                          # 0x0404
    "GL_BACK",                           # 0x0405
    "GL_LEFT",                           # 0x0406
    "GL_RIGHT",                          # 0x0407
    "GL_FRONT_AND_BACK",                 # 0x0408
    "GL_AUX0",                           # 0x0409
    "GL_AUX1",                           # 0x040A
    "GL_AUX2",                           # 0x040B
    "GL_AUX3",                           # 0x040C
    "GL_INVALID_ENUM",                   # 0x0500
    "GL_INVALID_VALUE",                  # 0x0501
    "GL_INVALID_OPERATION",              # 0x0502
    "GL_STACK_OVERFLOW",                 # 0x0503
    "GL_STACK_UNDERFLOW",                # 0x0504
    "GL_OUT_OF_MEMORY",                  # 0x0505
    "GL_INVALID_FRAMEBUFFER_OPERATION",  # 0x0506
    "GL_2D",                             # 0x0600
    "GL_3D",                             # 0x0601
    "GL_3D_COLOR",                       # 0x0602
    "GL_3D_COLOR_TEXTURE",               # 0x0603
    "GL_4D_COLOR_TEXTURE",               # 0x0604
    "GL_PASS_THROUGH_TOKEN",             # 0x0700
    "GL_POINT_TOKEN",                    # 0x0701
    "GL_LINE_TOKEN",                     # 0x0702
    "GL_POLYGON_TOKEN",                  # 0x0703
    "GL_BITMAP_TOKEN",                   # 0x0704
    "GL_DRAW_PIXEL_TOKEN",               # 0x0705
    "GL_COPY_PIXEL_TOKEN",               # 0x0706
    "GL_LINE_RESET_TOKEN",               # 0x0707
    "GL_EXP",                            # 0x0800
    "GL_EXP2",                           # 0x0801
    "GL_CW",                             # 0x0900
    "GL_CCW",                            # 0x0901
    "GL_COEFF",                          # 0x0A00
    "GL_ORDER",                          # 0x0A01
    "GL_DOMAIN",                         # 0x0A02
    "GL_CURRENT_COLOR",                  # 0x0B00
    "GL_CURRENT_INDEX",                  # 0x0B01
    "GL_CURRENT_NORMAL",                 # 0x0B02
    "GL_CURRENT_TEXTURE_COORDS",         # 0x0B03
    "GL_CURRENT_RASTER_COLOR",           # 0x0B04
    "GL_CURRENT_RASTER_INDEX",           # 0x0B05
    "GL_CURRENT_RASTER_TEXTURE_COORDS",  # 0x0B06
    "GL_CURRENT_RASTER_POSITION",        # 0x0B07
    "GL_CURRENT_RASTER_POSITION_VALID",  # 0x0B08
    "GL_CURRENT_RASTER_DISTANCE",        # 0x0B09
    "GL_POINT_SMOOTH",                   # 0x0B10
    "GL_POINT_SIZE",                     # 0x0B11
    "GL_POINT_SIZE_RANGE",               # 0x0B12
    "GL_POINT_SIZE_GRANULARITY",         # 0x0B13
    "GL_LINE_SMOOTH",                    # 0x0B20
    "GL_LINE_WIDTH",                     # 0x0B21
    "GL_LINE_WIDTH_RANGE",               # 0x0B22
    "GL_LINE_WIDTH_GRANULARITY",         # 0x0B23
    "GL_LINE_STIPPLE",                   # 0x0B24
    "GL_LINE_STIPPLE_PATTERN",           # 0x0B25
    "GL_LINE_STIPPLE_REPEAT",            # 0x0B26
    "GL_LIST_MODE",                      # 0x0B30
    "GL_MAX_LIST_NESTING",               # 0x0B31
    "GL_LIST_BASE",                      # 0x0B32
    "GL_LIST_INDEX",                     # 0x0B33
    "GL_POLYGON_MODE",                   # 0x0B40
    "GL_POLYGON_SMOOTH",                 # 0x0B41
    "GL_POLYGON_STIPPLE",                # 0x0B42
    "GL_EDGE_FLAG",                      # 0x0B43
    "GL_CULL_FACE",                      # 0x0B44
    "GL_CULL_FACE_MODE",                 # 0x0B45
    "GL_FRONT_FACE",                     # 0x0B46
    "GL_LIGHTING",                       # 0x0B50
    "GL_LIGHT_MODEL_LOCAL_VIEWER",       # 0x0B51
    "GL_LIGHT_MODEL_TWO_SIDE",           # 0x0B52
    "GL_LIGHT_MODEL_AMBIENT",            # 0x0B53
    "GL_SHADE_MODEL",                    # 0x0B54
    "GL_COLOR_MATERIAL_FACE",            # 0x0B55
    "GL_COLOR_MATERIAL_PARAMETER",       # 0x0B56
    "GL_COLOR_MATERIAL",                 # 0x0B57
    "GL_FOG",                            # 0x0B60
    "GL_FOG_INDEX",                      # 0x0B61
    "GL_FOG_DENSITY",                    # 0x0B62
    "GL_FOG_START",                      # 0x0B63
    "GL_FOG_END",                        # 0x0B64
    "GL_FOG_MODE",                       # 0x0B65
    "GL_FOG_COLOR",                      # 0x0B66
    "GL_DEPTH_RANGE",                    # 0x0B70
    "GL_DEPTH_TEST",                     # 0x0B71
    "GL_DEPTH_WRITEMASK",                # 0x0B72
    "GL_DEPTH_CLEAR_VALUE",              # 0x0B73
    "GL_DEPTH_FUNC",                     # 0x0B74
    "GL_ACCUM_CLEAR_VALUE",              # 0x0B80
    "GL_STENCIL_TEST",                   # 0x0B90
    "GL_STENCIL_CLEAR_VALUE",            # 0x0B91
    "GL_STENCIL_FUNC",                   # 0x0B92
    "GL_STENCIL_VALUE_MASK",             # 0x0B93
    "GL_STENCIL_FAIL",                   # 0x0B94
    "GL_STENCIL_PASS_DEPTH_FAIL",        # 0x0B95
    "GL_STENCIL_PASS_DEPTH_PASS",        # 0x0B96
    "GL_STENCIL_REF",                    # 0x0B97
    "GL_STENCIL_WRITEMASK",              # 0x0B98
    "GL_MATRIX_MODE",                    # 0x0BA0
    "GL_NORMALIZE",                      # 0x0BA1
    "GL_VIEWPORT",                       # 0x0BA2
    "GL_MODELVIEW_STACK_DEPTH",          # 0x0BA3
    "GL_PROJECTION_STACK_DEPTH",         # 0x0BA4
    "GL_TEXTURE_STACK_DEPTH",            # 0x0BA5
    "GL_MODELVIEW_MATRIX",               # 0x0BA6
    "GL_PROJECTION_MATRIX",              # 0x0BA7
    "GL_TEXTURE_MATRIX",                 # 0x0BA8
    "GL_ATTRIB_STACK_DEPTH",             # 0x0BB0
    "GL_CLIENT_ATTRIB_STACK_DEPTH",      # 0x0BB1
    "GL_ALPHA_TEST",                     # 0x0BC0
    "GL_ALPHA_TEST_FUNC",                # 0x0BC1
    "GL_ALPHA_TEST_REF",                 # 0x0BC2
    "GL_DITHER",                         # 0x0BD0
    "GL_BLEND_DST",                      # 0x0BE0
    "GL_BLEND_SRC",                      # 0x0BE1
    "GL_BLEND",                          # 0x0BE2
    "GL_LOGIC_OP_MODE",                  # 0x0BF0
    "GL_INDEX_LOGIC_OP",                 # 0x0BF1
    "GL_COLOR_LOGIC_OP",                 # 0x0BF2
    "GL_AUX_BUFFERS",                    # 0x0C00
    "GL_DRAW_BUFFER",                    # 0x0C01
    "GL_READ_BUFFER",                    # 0x0C02
    "GL_SCISSOR_BOX",                    # 0x0C10
    "GL_SCISSOR_TEST",                   # 0x0C11
    "GL_INDEX_CLEAR_VALUE",              # 0x0C20
    "GL_INDEX_WRITEMASK",                # 0x0C21
    "GL_COLOR_CLEAR_VALUE",              # 0x0C22
    "GL_COLOR_WRITEMASK",                # 0x0C23
    "GL_INDEX_MODE",                     # 0x0C30
    "GL_RGBA_MODE",                      # 0x0C31
    "GL_DOUBLEBUFFER",                   # 0x0C32
    "GL_STEREO",                         # 0x0C33
    "GL_RENDER_MODE",                    # 0x0C40
    "GL_PERSPECTIVE_CORRECTION_HINT",    # 0x0C50
    "GL_POINT_SMOOTH_HINT",              # 0x0C51
    "GL_LINE_SMOOTH_HINT",               # 0x0C52
    "GL_POLYGON_SMOOTH_HINT",            # 0x0C53
    "GL_FOG_HINT",                       # 0x0C54
    "GL_TEXTURE_GEN_S",                  # 0x0C60
    "GL_TEXTURE_GEN_T",                  # 0x0C61
    "GL_TEXTURE_GEN_R",                  # 0x0C62
    "GL_TEXTURE_GEN_Q",                  # 0x0C63
    "GL_PIXEL_MAP_I_TO_I",               # 0x0C70
    "GL_PIXEL_MAP_S_TO_S",               # 0x0C71
    "GL_PIXEL_MAP_I_TO_R",               # 0x0C72
    "GL_PIXEL_MAP_I_TO_G",               # 0x0C73
    "GL_PIXEL_MAP_I_TO_B",               # 0x0C74
    "GL_PIXEL_MAP_I_TO_A",               # 0x0C75
    "GL_PIXEL_MAP_R_TO_R",               # 0x0C76
    "GL_PIXEL_MAP_G_TO_G",               # 0x0C77
    "GL_PIXEL_MAP_B_TO_B",               # 0x0C78
    "GL_PIXEL_MAP_A_TO_A",               # 0x0C79
    "GL_PIXEL_MAP_I_TO_I_SIZE",          # 0x0CB0
    "GL_PIXEL_MAP_S_TO_S_SIZE",          # 0x0CB1
    "GL_PIXEL_MAP_I_TO_R_SIZE",          # 0x0CB2
    "GL_PIXEL_MAP_I_TO_G_SIZE",          # 0x0CB3
    "GL_PIXEL_MAP_I_TO_B_SIZE",          # 0x0CB4
    "GL_PIXEL_MAP_I_TO_A_SIZE",          # 0x0CB5
    "GL_PIXEL_MAP_R_TO_R_SIZE",          # 0x0CB6
    "GL_PIXEL_MAP_G_TO_G_SIZE",          # 0x0CB7
    "GL_PIXEL_MAP_B_TO_B_SIZE",          # 0x0CB8
    "GL_PIXEL_MAP_A_TO_A_SIZE",          # 0x0CB9
    "GL_UNPACK_SWAP_BYTES",              # 0x0CF0
    "GL_UNPACK_LSB_FIRST",               # 0x0CF1
    "GL_UNPACK_ROW_LENGTH",              # 0x0CF2
    "GL_UNPACK_SKIP_ROWS",               # 0x0CF3
    "GL_UNPACK_SKIP_PIXELS",             # 0x0CF4
    "GL_UNPACK_ALIGNMENT",               # 0x0CF5
    "GL_PACK_SWAP_BYTES",                # 0x0D00
    "GL_PACK_LSB_FIRST",                 # 0x0D01
    "GL_PACK_ROW_LENGTH",                # 0x0D02
    "GL_PACK_SKIP_ROWS",                 # 0x0D03
    "GL_PACK_SKIP_PIXELS",               # 0x0D04
    "GL_PACK_ALIGNMENT",                 # 0x0D05
    "GL_MAP_COLOR",                      # 0x0D10
    "GL_MAP_STENCIL",                    # 0x0D11
    "GL_INDEX_SHIFT",                    # 0x0D12
    "GL_INDEX_OFFSET",                   # 0x0D13
    "GL_RED_SCALE",                      # 0x0D14
    "GL_RED_BIAS",                       # 0x0D15
    "GL_ZOOM_X",                         # 0x0D16
    "GL_ZOOM_Y",                         # 0x0D17
    "GL_GREEN_SCALE",                    # 0x0D18
    "GL_GREEN_BIAS",                     # 0x0D19
    "GL_BLUE_SCALE",                     # 0x0D1A
    "GL_BLUE_BIAS",                      # 0x0D1B
    "GL_ALPHA_SCALE",                    # 0x0D1C
    "GL_ALPHA_BIAS",                     # 0x0D1D
    "GL_DEPTH_SCALE",                    # 0x0D1E
    "GL_DEPTH_BIAS",                     # 0x0D1F
    "GL_MAX_EVAL_ORDER",                 # 0x0D30
    "GL_MAX_LIGHTS",                     # 0x0D31
    "GL_MAX_CLIP_PLANES",                # 0x0D32
    "GL_MAX_TEXTURE_SIZE",               # 0x0D33
    "GL_MAX_PIXEL_MAP_TABLE",            # 0x0D34
    "GL_MAX_ATTRIB_STACK_DEPTH",         # 0x0D35
    "GL_MAX_MODELVIEW_STACK_DEPTH",      # 0x0D36
    "GL_MAX_NAME_STACK_DEPTH",           # 0x0D37
    "GL_MAX_PROJECTION_STACK_DEPTH",     # 0x0D38
    "GL_MAX_TEXTURE_STACK_DEPTH",        # 0x0D39
    "GL_MAX_VIEWPORT_DIMS",              # 0x0D3A
    "GL_MAX_CLIENT_ATTRIB_STACK_DEPTH",  # 0x0D3B
    "GL_SUBPIXEL_BITS",                  # 0x0D50
    "GL_INDEX_BITS",                     # 0x0D51
    "GL_RED_BITS",                       # 0x0D52
    "GL_GREEN_BITS",                     # 0x0D53
    "GL_BLUE_BITS",                      # 0x0D54
    "GL_ALPHA_BITS",                     # 0x0D55
    "GL_DEPTH_BITS",                     # 0x0D56
    "GL_STENCIL_BITS",                   # 0x0D57
    "GL_ACCUM_RED_BITS",                 # 0x0D58
    "GL_ACCUM_GREEN_BITS",               # 0x0D59
    "GL_ACCUM_BLUE_BITS",                # 0x0D5A
    "GL_ACCUM_ALPHA_BITS",               # 0x0D5B
    "GL_NAME_STACK_DEPTH",               # 0x0D70
    "GL_AUTO_NORMAL",                    # 0x0D80
    "GL_MAP1_COLOR_4",                   # 0x0D90
    "GL_MAP1_INDEX",                     # 0x0D91
    "GL_MAP1_NORMAL",                    # 0x0D92
    "GL_MAP1_TEXTURE_COORD_1",           # 0x0D93
    "GL_MAP1_TEXTURE_COORD_2",           # 0x0D94
    "GL_MAP1_TEXTURE_COORD_3",           # 0x0D95
    "GL_MAP1_TEXTURE_COORD_4",           # 0x0D96
    "GL_MAP1_VERTEX_3",                  # 0x0D97
    "GL_MAP1_VERTEX_4",                  # 0x0D98
    "GL_MAP2_COLOR_4",                   # 0x0DB0
    "GL_MAP2_INDEX",                     # 0x0DB1
    "GL_MAP2_NORMAL",                    # 0x0DB2
    "GL_MAP2_TEXTURE_COORD_1",           # 0x0DB3
    "GL_MAP2_TEXTURE_COORD_2",           # 0x0DB4
    "GL_MAP2_TEXTURE_COORD_3",           # 0x0DB5
    "GL_MAP2_TEXTURE_COORD_4",           # 0x0DB6
    "GL_MAP2_VERTEX_3",                  # 0x0DB7
    "GL_MAP2_VERTEX_4",                  # 0x0DB8
    "GL_MAP1_GRID_DOMAIN",               # 0x0DD0
    "GL_MAP1_GRID_SEGMENTS",             # 0x0DD1
    "GL_MAP2_GRID_DOMAIN",               # 0x0DD2
    "GL_MAP2_GRID_SEGMENTS",             # 0x0DD3
    "GL_TEXTURE_1D",                     # 0x0DE0
    "GL_TEXTURE_2D",                     # 0x0DE1
    "GL_FEEDBACK_BUFFER_POINTER",        # 0x0DF0
    "GL_FEEDBACK_BUFFER_SIZE",           # 0x0DF1
    "GL_FEEDBACK_BUFFER_TYPE",           # 0x0DF2
    "GL_SELECTION_BUFFER_POINTER",       # 0x0DF3
    "GL_SELECTION_BUFFER_SIZE",          # 0x0DF4
    "GL_TEXTURE_WIDTH",                  # 0x1000
    "GL_TEXTURE_HEIGHT",                 # 0x1001
    "GL_TEXTURE_COMPONENTS",             # 0x1003
    #"GL_TEXTURE_INTERNAL_FORMAT",       # 0x1003
    "GL_TEXTURE_BORDER_COLOR",           # 0x1004
    "GL_TEXTURE_BORDER",                 # 0x1005
    "GL_DONT_CARE",                      # 0x1100
    "GL_FASTEST",                        # 0x1101
    "GL_NICEST",                         # 0x1102
    "GL_AMBIENT",                        # 0x1200
    "GL_DIFFUSE",                        # 0x1201
    "GL_SPECULAR",                       # 0x1202
    "GL_POSITION",                       # 0x1203
    "GL_SPOT_DIRECTION",                 # 0x1204
    "GL_SPOT_EXPONENT",                  # 0x1205
    "GL_SPOT_CUTOFF",                    # 0x1206
    "GL_CONSTANT_ATTENUATION",           # 0x1207
    "GL_LINEAR_ATTENUATION",             # 0x1208
    "GL_QUADRATIC_ATTENUATION",          # 0x1209
    "GL_COMPILE",                        # 0x1300
    "GL_COMPILE_AND_EXECUTE",            # 0x1301
    "GL_BYTE",                           # 0x1400
    "GL_UNSIGNED_BYTE",                  # 0x1401
    "GL_SHORT",                          # 0x1402
    "GL_UNSIGNED_SHORT",                 # 0x1403
    "GL_INT",                            # 0x1404
    "GL_UNSIGNED_INT",                   # 0x1405
    "GL_FLOAT",                          # 0x1406
    "GL_2_BYTES",                        # 0x1407
    "GL_3_BYTES",                        # 0x1408
    "GL_4_BYTES",                        # 0x1409
    "GL_DOUBLE",                         # 0x140A
    "GL_HALF_FLOAT",                     # 0x140B
    "GL_CLEAR",                          # 0x1500
    "GL_AND",                            # 0x1501
    "GL_AND_REVERSE",                    # 0x1502
    "GL_COPY",                           # 0x1503
    "GL_AND_INVERTED",                   # 0x1504
    "GL_NOOP",                           # 0x1505
    "GL_XOR",                            # 0x1506
    "GL_OR",                             # 0x1507
    "GL_NOR",                            # 0x1508
    "GL_EQUIV",                          # 0x1509
    "GL_INVERT",                         # 0x150A
    "GL_OR_REVERSE",                     # 0x150B
    "GL_COPY_INVERTED",                  # 0x150C
    "GL_OR_INVERTED",                    # 0x150D
    "GL_NAND",                           # 0x150E
    "GL_SET",                            # 0x150F
    "GL_EMISSION",                       # 0x1600
    "GL_SHININESS",                      # 0x1601
    "GL_AMBIENT_AND_DIFFUSE",            # 0x1602
    "GL_COLOR_INDEXES",                  # 0x1603
    "GL_MODELVIEW",                      # 0x1700
    "GL_PROJECTION",                     # 0x1701
    "GL_TEXTURE",                        # 0x1702
    "GL_COLOR",                          # 0x1800
    "GL_DEPTH",                          # 0x1801
    "GL_STENCIL",                        # 0x1802
    "GL_COLOR_INDEX",                    # 0x1900
    "GL_STENCIL_INDEX",                  # 0x1901
    "GL_DEPTH_COMPONENT",                # 0x1902
    "GL_RED",                            # 0x1903
    "GL_GREEN",                          # 0x1904
    "GL_BLUE",                           # 0x1905
    "GL_ALPHA",                          # 0x1906
    "GL_RGB",                            # 0x1907
    "GL_RGBA",                           # 0x1908
    "GL_LUMINANCE",                      # 0x1909
    "GL_LUMINANCE_ALPHA",                # 0x190A
    "GL_BITMAP",                         # 0x1A00
    "GL_POINT",                          # 0x1B00
    "GL_LINE",                           # 0x1B01
    "GL_FILL",                           # 0x1B02
    "GL_RENDER",                         # 0x1C00
    "GL_FEEDBACK",                       # 0x1C01
    "GL_SELECT",                         # 0x1C02
    "GL_FLAT",                           # 0x1D00
    "GL_SMOOTH",                         # 0x1D01
    "GL_KEEP",                           # 0x1E00
    "GL_REPLACE",                        # 0x1E01
    "GL_INCR",                           # 0x1E02
    "GL_DECR",                           # 0x1E03
    "GL_VENDOR",                         # 0x1F00
    "GL_RENDERER",                       # 0x1F01
    "GL_VERSION",                        # 0x1F02
    "GL_EXTENSIONS",                     # 0x1F03
    "GL_S",                              # 0x2000
    "GL_T",                              # 0x2001
    "GL_R",                              # 0x2002
    "GL_Q",                              # 0x2003
    "GL_MODULATE",                       # 0x2100
    "GL_DECAL",                          # 0x2101
    "GL_TEXTURE_ENV_MODE",               # 0x2200
    "GL_TEXTURE_ENV_COLOR",              # 0x2201
    "GL_TEXTURE_ENV",                    # 0x2300
    "GL_EYE_LINEAR",                     # 0x2400
    "GL_OBJECT_LINEAR",                  # 0x2401
    "GL_SPHERE_MAP",                     # 0x2402
    "GL_TEXTURE_GEN_MODE",               # 0x2500
    "GL_OBJECT_PLANE",                   # 0x2501
    "GL_EYE_PLANE",                      # 0x2502
    "GL_NEAREST",                        # 0x2600
    "GL_LINEAR",                         # 0x2601
    "GL_NEAREST_MIPMAP_NEAREST",         # 0x2700
    "GL_LINEAR_MIPMAP_NEAREST",          # 0x2701
    "GL_NEAREST_MIPMAP_LINEAR",          # 0x2702
    "GL_LINEAR_MIPMAP_LINEAR",           # 0x2703
    "GL_TEXTURE_MAG_FILTER",             # 0x2800
    "GL_TEXTURE_MIN_FILTER",             # 0x2801
    "GL_TEXTURE_WRAP_S",                 # 0x2802
    "GL_TEXTURE_WRAP_T",                 # 0x2803
    "GL_CLAMP",                          # 0x2900
    "GL_REPEAT",                         # 0x2901
    "GL_POLYGON_OFFSET_UNITS",           # 0x2A00
    "GL_POLYGON_OFFSET_POINT",           # 0x2A01
    "GL_POLYGON_OFFSET_LINE",            # 0x2A02
    "GL_R3_G3_B2",                       # 0x2A10
    "GL_V2F",                            # 0x2A20
    "GL_V3F",                            # 0x2A21
    "GL_C4UB_V2F",                       # 0x2A22
    "GL_C4UB_V3F",                       # 0x2A23
    "GL_C3F_V3F",                        # 0x2A24
    "GL_N3F_V3F",                        # 0x2A25
    "GL_C4F_N3F_V3F",                    # 0x2A26
    "GL_T2F_V3F",                        # 0x2A27
    "GL_T4F_V4F",                        # 0x2A28
    "GL_T2F_C4UB_V3F",                   # 0x2A29
    "GL_T2F_C3F_V3F",                    # 0x2A2A
    "GL_T2F_N3F_V3F",                    # 0x2A2B
    "GL_T2F_C4F_N3F_V3F",                # 0x2A2C
    "GL_T4F_C4F_N3F_V4F",                # 0x2A2D
    "GL_CLIP_PLANE0",                    # 0x3000
    "GL_CLIP_PLANE1",                    # 0x3001
    "GL_CLIP_PLANE2",                    # 0x3002
    "GL_CLIP_PLANE3",                    # 0x3003
    "GL_CLIP_PLANE4",                    # 0x3004
    "GL_CLIP_PLANE5",                    # 0x3005
    "GL_LIGHT0",                         # 0x4000
    "GL_LIGHT1",                         # 0x4001
    "GL_LIGHT2",                         # 0x4002
    "GL_LIGHT3",                         # 0x4003
    "GL_LIGHT4",                         # 0x4004
    "GL_LIGHT5",                         # 0x4005
    "GL_LIGHT6",                         # 0x4006
    "GL_LIGHT7",                         # 0x4007
    "GL_ABGR_EXT",                       # 0x8000
    "GL_CONSTANT_COLOR",                 # 0x8001
    "GL_ONE_MINUS_CONSTANT_COLOR",       # 0x8002
    "GL_CONSTANT_ALPHA",                 # 0x8003
    "GL_ONE_MINUS_CONSTANT_ALPHA",       # 0x8004
    "GL_BLEND_COLOR",                    # 0x8005
    "GL_FUNC_ADD",                       # 0x8006
    "GL_MIN",                            # 0x8007
    "GL_MAX",                            # 0x8008
    "GL_BLEND_EQUATION",                 # 0x8009
    "GL_FUNC_SUBTRACT",                  # 0x800A
    "GL_FUNC_REVERSE_SUBTRACT",          # 0x800B
    "GL_CMYK_EXT",                       # 0x800C
    "GL_CMYKA_EXT",                      # 0x800D
    "GL_PACK_CMYK_HINT_EXT",             # 0x800E
    "GL_UNPACK_CMYK_HINT_EXT",           # 0x800F
    "GL_CONVOLUTION_1D",                 # 0x8010
    "GL_CONVOLUTION_2D",                 # 0x8011
    "GL_SEPARABLE_2D",                   # 0x8012
    "GL_CONVOLUTION_BORDER_MODE",        # 0x8013
    "GL_CONVOLUTION_FILTER_SCALE",       # 0x8014
    "GL_CONVOLUTION_FILTER_BIAS",        # 0x8015
    "GL_REDUCE",                         # 0x8016
    "GL_CONVOLUTION_FORMAT",             # 0x8017
    "GL_CONVOLUTION_WIDTH",              # 0x8018
    "GL_CONVOLUTION_HEIGHT",             # 0x8019
    "GL_MAX_CONVOLUTION_WIDTH",          # 0x801A
    "GL_MAX_CONVOLUTION_HEIGHT",         # 0x801B
    "GL_POST_CONVOLUTION_RED_SCALE",     # 0x801C
    "GL_POST_CONVOLUTION_GREEN_SCALE",   # 0x801D
    "GL_POST_CONVOLUTION_BLUE_SCALE",    # 0x801E
    "GL_POST_CONVOLUTION_ALPHA_SCALE",   # 0x801F
    "GL_POST_CONVOLUTION_RED_BIAS",      # 0x8020
    "GL_POST_CONVOLUTION_GREEN_BIAS",    # 0x8021
    "GL_POST_CONVOLUTION_BLUE_BIAS",     # 0x8022
    "GL_POST_CONVOLUTION_ALPHA_BIAS",    # 0x8023
    "GL_HISTOGRAM",                      # 0x8024
    "GL_PROXY_HISTOGRAM",                # 0x8025
    "GL_HISTOGRAM_WIDTH",                # 0x8026
    "GL_HISTOGRAM_FORMAT",               # 0x8027
    "GL_HISTOGRAM_RED_SIZE",             # 0x8028
    "GL_HISTOGRAM_GREEN_SIZE",           # 0x8029
    "GL_HISTOGRAM_BLUE_SIZE",            # 0x802A
    "GL_HISTOGRAM_ALPHA_SIZE",           # 0x802B
    "GL_HISTOGRAM_LUMINANCE_SIZE",       # 0x802C
    "GL_HISTOGRAM_SINK",                 # 0x802D
    "GL_MINMAX",                         # 0x802E
    "GL_MINMAX_FORMAT",                  # 0x802F
    "GL_MINMAX_SINK",                    # 0x8030
    "GL_TABLE_TOO_LARGE",                # 0x8031
    "GL_UNSIGNED_BYTE_3_3_2",            # 0x8032
    "GL_UNSIGNED_SHORT_4_4_4_4",         # 0x8033
    "GL_UNSIGNED_SHORT_5_5_5_1",         # 0x8034
    "GL_UNSIGNED_INT_8_8_8_8",           # 0x8035
    "GL_UNSIGNED_INT_10_10_10_2",        # 0x8036
    "GL_POLYGON_OFFSET_FILL",            # 0x8037
    "GL_POLYGON_OFFSET_FACTOR",          # 0x8038
    "GL_POLYGON_OFFSET_BIAS_EXT",        # 0x8039
    "GL_RESCALE_NORMAL",                 # 0x803A
    "GL_ALPHA4",                         # 0x803B
    "GL_ALPHA8",                         # 0x803C
    "GL_ALPHA12",                        # 0x803D
    "GL_ALPHA16",                        # 0x803E
    "GL_LUMINANCE4",                     # 0x803F
    "GL_LUMINANCE8",                     # 0x8040
    "GL_LUMINANCE12",                    # 0x8041
    "GL_LUMINANCE16",                    # 0x8042
    "GL_LUMINANCE4_ALPHA4",              # 0x8043
    "GL_LUMINANCE6_ALPHA2",              # 0x8044
    "GL_LUMINANCE8_ALPHA8",              # 0x8045
    "GL_LUMINANCE12_ALPHA4",             # 0x8046
    "GL_LUMINANCE12_ALPHA12",            # 0x8047
    "GL_LUMINANCE16_ALPHA16",            # 0x8048
    "GL_INTENSITY",                      # 0x8049
    "GL_INTENSITY4",                     # 0x804A
    "GL_INTENSITY8",                     # 0x804B
    "GL_INTENSITY12",                    # 0x804C
    "GL_INTENSITY16",                    # 0x804D
    "GL_RGB2_EXT",                       # 0x804E
    "GL_RGB4",                           # 0x804F
    "GL_RGB5",                           # 0x8050
    "GL_RGB8",                           # 0x8051
    "GL_RGB10",                          # 0x8052
    "GL_RGB12",                          # 0x8053
    "GL_RGB16",                          # 0x8054
    "GL_RGBA2",                          # 0x8055
    "GL_RGBA4",                          # 0x8056
    "GL_RGB5_A1",                        # 0x8057
    "GL_RGBA8",                          # 0x8058
    "GL_RGB10_A2",                       # 0x8059
    "GL_RGBA12",                         # 0x805A
    "GL_RGBA16",                         # 0x805B
    "GL_TEXTURE_RED_SIZE",               # 0x805C
    "GL_TEXTURE_GREEN_SIZE",             # 0x805D
    "GL_TEXTURE_BLUE_SIZE",              # 0x805E
    "GL_TEXTURE_ALPHA_SIZE",             # 0x805F
    "GL_TEXTURE_LUMINANCE_SIZE",         # 0x8060
    "GL_TEXTURE_INTENSITY_SIZE",         # 0x8061
    "GL_REPLACE_EXT",                    # 0x8062
    "GL_PROXY_TEXTURE_1D",               # 0x8063
    "GL_PROXY_TEXTURE_2D",               # 0x8064
    "GL_TEXTURE_TOO_LARGE_EXT",          # 0x8065
    "GL_TEXTURE_PRIORITY",               # 0x8066
    "GL_TEXTURE_RESIDENT",               # 0x8067
    "GL_TEXTURE_BINDING_1D",             # 0x8068
    "GL_TEXTURE_BINDING_2D",             # 0x8069
    "GL_TEXTURE_BINDING_3D",             # 0x806A
    "GL_PACK_SKIP_IMAGES",               # 0x806B
    "GL_PACK_IMAGE_HEIGHT",              # 0x806C
    "GL_UNPACK_SKIP_IMAGES",             # 0x806D
    "GL_UNPACK_IMAGE_HEIGHT",            # 0x806E
    "GL_TEXTURE_3D",                     # 0x806F
    "GL_PROXY_TEXTURE_3D",               # 0x8070
    "GL_TEXTURE_DEPTH",                  # 0x8071
    "GL_TEXTURE_WRAP_R",                 # 0x8072
    "GL_MAX_3D_TEXTURE_SIZE",            # 0x8073
    "GL_VERTEX_ARRAY",                   # 0x8074
    "GL_NORMAL_ARRAY",                   # 0x8075
    "GL_COLOR_ARRAY",                    # 0x8076
    "GL_INDEX_ARRAY",                    # 0x8077
    "GL_TEXTURE_COORD_ARRAY",            # 0x8078
    "GL_EDGE_FLAG_ARRAY",                # 0x8079
    "GL_VERTEX_ARRAY_SIZE",              # 0x807A
    "GL_VERTEX_ARRAY_TYPE",              # 0x807B
    "GL_VERTEX_ARRAY_STRIDE",            # 0x807C
    "GL_VERTEX_ARRAY_COUNT_EXT",         # 0x807D
    "GL_NORMAL_ARRAY_TYPE",              # 0x807E
    "GL_NORMAL_ARRAY_STRIDE",            # 0x807F
    "GL_NORMAL_ARRAY_COUNT_EXT",         # 0x8080
    "GL_COLOR_ARRAY_SIZE",               # 0x8081
    "GL_COLOR_ARRAY_TYPE",               # 0x8082
    "GL_COLOR_ARRAY_STRIDE",             # 0x8083
    "GL_COLOR_ARRAY_COUNT_EXT",          # 0x8084
    "GL_INDEX_ARRAY_TYPE",               # 0x8085
    "GL_INDEX_ARRAY_STRIDE",             # 0x8086
    "GL_INDEX_ARRAY_COUNT_EXT",          # 0x8087
    "GL_TEXTURE_COORD_ARRAY_SIZE",       # 0x8088
    "GL_TEXTURE_COORD_ARRAY_TYPE",       # 0x8089
    "GL_TEXTURE_COORD_ARRAY_STRIDE",     # 0x808A
    "GL_TEXTURE_COORD_ARRAY_COUNT_EXT",  # 0x808B
    "GL_EDGE_FLAG_ARRAY_STRIDE",         # 0x808C
    "GL_EDGE_FLAG_ARRAY_COUNT_EXT",      # 0x808D
    "GL_VERTEX_ARRAY_POINTER",           # 0x808E
    "GL_NORMAL_ARRAY_POINTER",           # 0x808F
    "GL_COLOR_ARRAY_POINTER",            # 0x8090
    "GL_INDEX_ARRAY_POINTER",            # 0x8091
    "GL_TEXTURE_COORD_ARRAY_POINTER",    # 0x8092
    "GL_EDGE_FLAG_ARRAY_POINTER",        # 0x8093
    "GL_INTERLACE_SGIX",                 # 0x8094
    "GL_DETAIL_TEXTURE_2D_SGIS",         # 0x8095
    "GL_DETAIL_TEXTURE_2D_BINDING_SGIS", # 0x8096
    "GL_LINEAR_DETAIL_SGIS",             # 0x8097
    "GL_LINEAR_DETAIL_ALPHA_SGIS",       # 0x8098
    "GL_LINEAR_DETAIL_COLOR_SGIS",       # 0x8099
    "GL_DETAIL_TEXTURE_LEVEL_SGIS",      # 0x809A
    "GL_DETAIL_TEXTURE_MODE_SGIS",       # 0x809B
    "GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS",# 0x809C
    "GL_MULTISAMPLE",                    # 0x809D
    "GL_SAMPLE_ALPHA_TO_COVERAGE",       # 0x809E
    "GL_SAMPLE_ALPHA_TO_ONE",            # 0x809F
    "GL_SAMPLE_COVERAGE",                # 0x80A0
    "GL_1PASS_EXT",                      # 0x80A1
    "GL_2PASS_0_EXT",                    # 0x80A2
    "GL_2PASS_1_EXT",                    # 0x80A3
    "GL_4PASS_0_EXT",                    # 0x80A4
    "GL_4PASS_1_EXT",                    # 0x80A5
    "GL_4PASS_2_EXT",                    # 0x80A6
    "GL_4PASS_3_EXT",                    # 0x80A7
    "GL_SAMPLE_BUFFERS",                 # 0x80A8
    "GL_SAMPLES",                        # 0x80A9
    "GL_SAMPLE_COVERAGE_VALUE",          # 0x80AA
    "GL_SAMPLE_COVERAGE_INVERT",         # 0x80AB
    "GL_SAMPLE_PATTERN_EXT",             # 0x80AC
    "GL_LINEAR_SHARPEN_SGIS",            # 0x80AD
    "GL_LINEAR_SHARPEN_ALPHA_SGIS",      # 0x80AE
    "GL_LINEAR_SHARPEN_COLOR_SGIS",      # 0x80AF
    "GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS", # 0x80B0
    "GL_COLOR_MATRIX",                   # 0x80B1
    "GL_COLOR_MATRIX_STACK_DEPTH",       # 0x80B2
    "GL_MAX_COLOR_MATRIX_STACK_DEPTH",   # 0x80B3
    "GL_POST_COLOR_MATRIX_RED_SCALE",    # 0x80B4
    "GL_POST_COLOR_MATRIX_GREEN_SCALE",  # 0x80B5
    "GL_POST_COLOR_MATRIX_BLUE_SCALE",   # 0x80B6
    "GL_POST_COLOR_MATRIX_ALPHA_SCALE",  # 0x80B7
    "GL_POST_COLOR_MATRIX_RED_BIAS",     # 0x80B8
    "GL_POST_COLOR_MATRIX_GREEN_BIAS",   # 0x80B9
    "GL_POST_COLOR_MATRIX_BLUE_BIAS",    # 0x80BA
    "GL_POST_COLOR_MATRIX_ALPHA_BIAS",   # 0x80BB
    "GL_TEXTURE_COLOR_TABLE_SGI",        # 0x80BC
    "GL_PROXY_TEXTURE_COLOR_TABLE_SGI",  # 0x80BD
    "GL_TEXTURE_ENV_BIAS_SGIX",          # 0x80BE
    "GL_TEXTURE_COMPARE_FAIL_VALUE_ARB", # 0x80BF
    "GL_BLEND_DST_RGB",                  # 0x80C8
    "GL_BLEND_SRC_RGB",                  # 0x80C9
    "GL_BLEND_DST_ALPHA",                # 0x80CA
    "GL_BLEND_SRC_ALPHA",                # 0x80CB
    "GL_422_EXT",                        # 0x80CC
    "GL_422_REV_EXT",                    # 0x80CD
    "GL_422_AVERAGE_EXT",                # 0x80CE
    "GL_422_REV_AVERAGE_EXT",            # 0x80CF
    "GL_COLOR_TABLE",                    # 0x80D0
    "GL_POST_CONVOLUTION_COLOR_TABLE",   # 0x80D1
    "GL_POST_COLOR_MATRIX_COLOR_TABLE",  # 0x80D2
    "GL_PROXY_COLOR_TABLE",              # 0x80D3
    "GL_PROXY_POST_CONVOLUTION_COLOR_TABLE", # 0x80D4
    "GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE", # 0x80D5
    "GL_COLOR_TABLE_SCALE",              # 0x80D6
    "GL_COLOR_TABLE_BIAS",               # 0x80D7
    "GL_COLOR_TABLE_FORMAT",             # 0x80D8
    "GL_COLOR_TABLE_WIDTH",              # 0x80D9
    "GL_COLOR_TABLE_RED_SIZE",           # 0x80DA
    "GL_COLOR_TABLE_GREEN_SIZE",         # 0x80DB
    "GL_COLOR_TABLE_BLUE_SIZE",          # 0x80DC
    "GL_COLOR_TABLE_ALPHA_SIZE",         # 0x80DD
    "GL_COLOR_TABLE_LUMINANCE_SIZE",     # 0x80DE
    "GL_COLOR_TABLE_INTENSITY_SIZE",     # 0x80DF
    "GL_BGR",                            # 0x80E0
    "GL_BGRA",                           # 0x80E1
    "GL_COLOR_INDEX1_EXT",               # 0x80E2
    "GL_COLOR_INDEX2_EXT",               # 0x80E3
    "GL_COLOR_INDEX4_EXT",               # 0x80E4
    "GL_COLOR_INDEX8_EXT",               # 0x80E5
    "GL_COLOR_INDEX12_EXT",              # 0x80E6
    "GL_COLOR_INDEX16_EXT",              # 0x80E7
    "GL_MAX_ELEMENTS_VERTICES",          # 0x80E8
    "GL_MAX_ELEMENTS_INDICES",           # 0x80E9
    "GL_PHONG_WIN",                      # 0x80EA
    "GL_PHONG_HINT_WIN",                 # 0x80EB
    "GL_FOG_SPECULAR_TEXTURE_WIN",       # 0x80EC
    #"GL_TEXTURE_INDEX_SIZE_EXT",         # 0x80ED
    "GL_CLIP_VOLUME_CLIPPING_HINT_EXT",  # 0x80F0
    "GL_DUAL_ALPHA4_SGIS",               # 0x8110
    "GL_DUAL_ALPHA8_SGIS",               # 0x8111
    "GL_DUAL_ALPHA12_SGIS",              # 0x8112
    "GL_DUAL_ALPHA16_SGIS",              # 0x8113
    "GL_DUAL_LUMINANCE4_SGIS",           # 0x8114
    "GL_DUAL_LUMINANCE8_SGIS",           # 0x8115
    "GL_DUAL_LUMINANCE12_SGIS",          # 0x8116
    "GL_DUAL_LUMINANCE16_SGIS",          # 0x8117
    "GL_DUAL_INTENSITY4_SGIS",           # 0x8118
    "GL_DUAL_INTENSITY8_SGIS",           # 0x8119
    "GL_DUAL_INTENSITY12_SGIS",          # 0x811A
    "GL_DUAL_INTENSITY16_SGIS",          # 0x811B
    "GL_DUAL_LUMINANCE_ALPHA4_SGIS",     # 0x811C
    "GL_DUAL_LUMINANCE_ALPHA8_SGIS",     # 0x811D
    "GL_QUAD_ALPHA4_SGIS",               # 0x811E
    "GL_QUAD_ALPHA8_SGIS",               # 0x811F
    "GL_QUAD_LUMINANCE4_SGIS",           # 0x8120
    "GL_QUAD_LUMINANCE8_SGIS",           # 0x8121
    "GL_QUAD_INTENSITY4_SGIS",           # 0x8122
    "GL_QUAD_INTENSITY8_SGIS",           # 0x8123
    "GL_DUAL_TEXTURE_SELECT_SGIS",       # 0x8124
    "GL_QUAD_TEXTURE_SELECT_SGIS",       # 0x8125
    "GL_POINT_SIZE_MIN",                 # 0x8126
    "GL_POINT_SIZE_MAX",                 # 0x8127
    "GL_POINT_FADE_THRESHOLD_SIZE",      # 0x8128
    "GL_POINT_DISTANCE_ATTENUATION",     # 0x8129
    "GL_FOG_FUNC_SGIS",                  # 0x812A
    "GL_FOG_FUNC_POINTS_SGIS",           # 0x812B
    "GL_MAX_FOG_FUNC_POINTS_SGIS",       # 0x812C
    "GL_CLAMP_TO_BORDER",                # 0x812D
    "GL_CLAMP_TO_EDGE",                  # 0x812F
    "GL_PACK_SKIP_VOLUMES_SGIS",         # 0x8130
    "GL_PACK_IMAGE_DEPTH_SGIS",          # 0x8131
    "GL_UNPACK_SKIP_VOLUMES_SGIS",       # 0x8132
    "GL_UNPACK_IMAGE_DEPTH_SGIS",        # 0x8133
    "GL_TEXTURE_4D_SGIS",                # 0x8134
    "GL_PROXY_TEXTURE_4D_SGIS",          # 0x8135
    "GL_TEXTURE_4DSIZE_SGIS",            # 0x8136
    "GL_TEXTURE_WRAP_Q_SGIS",            # 0x8137
    "GL_MAX_4D_TEXTURE_SIZE_SGIS",       # 0x8138
    "GL_PIXEL_TEX_GEN_SGIX",             # 0x8139
    "GL_TEXTURE_MIN_LOD",                # 0x813A
    "GL_TEXTURE_MAX_LOD",                # 0x813B
    "GL_TEXTURE_BASE_LEVEL",             # 0x813C
    "GL_TEXTURE_MAX_LEVEL",              # 0x813D
    "GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX", # 0x813E
    "GL_PIXEL_TILE_CACHE_INCREMENT_SGIX",# 0x813F
    "GL_PIXEL_TILE_WIDTH_SGIX",          # 0x8140
    "GL_PIXEL_TILE_HEIGHT_SGIX",         # 0x8141
    "GL_PIXEL_TILE_GRID_WIDTH_SGIX",     # 0x8142
    "GL_PIXEL_TILE_GRID_HEIGHT_SGIX",    # 0x8143
    "GL_PIXEL_TILE_GRID_DEPTH_SGIX",     # 0x8144
    "GL_PIXEL_TILE_CACHE_SIZE_SGIX",     # 0x8145
    "GL_FILTER4_SGIS",                   # 0x8146
    "GL_TEXTURE_FILTER4_SIZE_SGIS",      # 0x8147
    "GL_SPRITE_SGIX",                    # 0x8148
    "GL_SPRITE_MODE_SGIX",               # 0x8149
    "GL_SPRITE_AXIS_SGIX",               # 0x814A
    "GL_SPRITE_TRANSLATION_SGIX",        # 0x814B
    "GL_SPRITE_AXIAL_SGIX",              # 0x814C
    "GL_SPRITE_OBJECT_ALIGNED_SGIX",     # 0x814D
    "GL_SPRITE_EYE_ALIGNED_SGIX",        # 0x814E
    "GL_TEXTURE_4D_BINDING_SGIS",        # 0x814F
    "GL_IGNORE_BORDER_HP",               # 0x8150
    "GL_CONSTANT_BORDER",                # 0x8151
    "GL_REPLICATE_BORDER",               # 0x8153
    "GL_CONVOLUTION_BORDER_COLOR",       # 0x8154
    "GL_IMAGE_SCALE_X_HP",               # 0x8155
    "GL_IMAGE_SCALE_Y_HP",               # 0x8156
    "GL_IMAGE_TRANSLATE_X_HP",           # 0x8157
    "GL_IMAGE_TRANSLATE_Y_HP",           # 0x8158
    "GL_IMAGE_ROTATE_ANGLE_HP",          # 0x8159
    "GL_IMAGE_ROTATE_ORIGIN_X_HP",       # 0x815A
    "GL_IMAGE_ROTATE_ORIGIN_Y_HP",       # 0x815B
    "GL_IMAGE_MAG_FILTER_HP",            # 0x815C
    "GL_IMAGE_MIN_FILTER_HP",            # 0x815D
    "GL_IMAGE_CUBIC_WEIGHT_HP",          # 0x815E
    "GL_CUBIC_HP",                       # 0x815F
    "GL_AVERAGE_HP",                     # 0x8160
    "GL_IMAGE_TRANSFORM_2D_HP",          # 0x8161
    "GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP", # 0x8162
    "GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP", # 0x8163
    "GL_OCCLUSION_TEST_HP",              # 0x8165
    "GL_OCCLUSION_TEST_RESULT_HP",       # 0x8166
    "GL_TEXTURE_LIGHTING_MODE_HP",       # 0x8167
    "GL_TEXTURE_POST_SPECULAR_HP",       # 0x8168
    "GL_TEXTURE_PRE_SPECULAR_HP",        # 0x8169
    "GL_LINEAR_CLIPMAP_LINEAR_SGIX",     # 0x8170
    "GL_TEXTURE_CLIPMAP_CENTER_SGIX",    # 0x8171
    "GL_TEXTURE_CLIPMAP_FRAME_SGIX",     # 0x8172
    "GL_TEXTURE_CLIPMAP_OFFSET_SGIX",    # 0x8173
    "GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX", # 0x8174
    "GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX",# 0x8175
    "GL_TEXTURE_CLIPMAP_DEPTH_SGIX",     # 0x8176
    "GL_MAX_CLIPMAP_DEPTH_SGIX",         # 0x8177
    "GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX", # 0x8178
    "GL_POST_TEXTURE_FILTER_BIAS_SGIX",  # 0x8179
    "GL_POST_TEXTURE_FILTER_SCALE_SGIX", # 0x817A
    "GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX", # 0x817B
    "GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX", # 0x817C
    "GL_REFERENCE_PLANE_SGIX",           # 0x817D
    "GL_REFERENCE_PLANE_EQUATION_SGIX",  # 0x817E
    "GL_IR_INSTRUMENT1_SGIX",            # 0x817F
    "GL_INSTRUMENT_BUFFER_POINTER_SGIX", # 0x8180
    "GL_INSTRUMENT_MEASUREMENTS_SGIX",   # 0x8181
    "GL_LIST_PRIORITY_SGIX",             # 0x8182
    "GL_CALLIGRAPHIC_FRAGMENT_SGIX",     # 0x8183
    "GL_PIXEL_TEX_GEN_Q_CEILING_SGIX",   # 0x8184
    "GL_PIXEL_TEX_GEN_Q_ROUND_SGIX",     # 0x8185
    "GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX",     # 0x8186
    "GL_PIXEL_TEX_GEN_ALPHA_REPLACE_SGIX", # 0x8187
    "GL_PIXEL_TEX_GEN_ALPHA_NO_REPLACE_SGIX", # 0x8188
    "GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX",    # 0x8189
    "GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX",    # 0x818A
    "GL_FRAMEZOOM_SGIX",                 # 0x818B
    "GL_FRAMEZOOM_FACTOR_SGIX",          # 0x818C
    "GL_MAX_FRAMEZOOM_FACTOR_SGIX",      # 0x818D
    "GL_TEXTURE_LOD_BIAS_S_SGIX",        # 0x818E
    "GL_TEXTURE_LOD_BIAS_T_SGIX",        # 0x818F
    "GL_TEXTURE_LOD_BIAS_R_SGIX",        # 0x8190
    "GL_GENERATE_MIPMAP",                # 0x8191
    "GL_GENERATE_MIPMAP_HINT",           # 0x8192
    "GL_GEOMETRY_DEFORMATION_SGIX",      # 0x8194
    "GL_TEXTURE_DEFORMATION_SGIX",       # 0x8195
    "GL_DEFORMATIONS_MASK_SGIX",         # 0x8196
    "GL_MAX_DEFORMATION_ORDER_SGIX",     # 0x8197
    "GL_FOG_OFFSET_SGIX",                # 0x8198
    "GL_FOG_OFFSET_VALUE_SGIX",          # 0x8199
    "GL_TEXTURE_COMPARE_SGIX",           # 0x819A
    "GL_TEXTURE_COMPARE_OPERATOR_SGIX",  # 0x819B
    "GL_TEXTURE_LEQUAL_R_SGIX",          # 0x819C
    "GL_TEXTURE_GEQUAL_R_SGIX",          # 0x819D
    "GL_DEPTH_COMPONENT16",              # 0x81A5
    "GL_DEPTH_COMPONENT24",              # 0x81A6
    "GL_DEPTH_COMPONENT32",              # 0x81A7
    "GL_ARRAY_ELEMENT_LOCK_FIRST_EXT",   # 0x81A8
    "GL_ARRAY_ELEMENT_LOCK_COUNT_EXT",   # 0x81A9
    "GL_CULL_VERTEX_EXT",                # 0x81AA
    "GL_CULL_VERTEX_EYE_POSITION_EXT",   # 0x81AB
    "GL_CULL_VERTEX_OBJECT_POSITION_EXT",# 0x81AC
    "GL_IUI_V2F_EXT",                    # 0x81AD
    "GL_IUI_V3F_EXT",                    # 0x81AE
    "GL_IUI_N3F_V2F_EXT",                # 0x81AF
    "GL_IUI_N3F_V3F_EXT",                # 0x81B0
    "GL_T2F_IUI_V2F_EXT",                # 0x81B1
    "GL_T2F_IUI_V3F_EXT",                # 0x81B2
    "GL_T2F_IUI_N3F_V2F_EXT",            # 0x81B3
    "GL_T2F_IUI_N3F_V3F_EXT",            # 0x81B4
    "GL_INDEX_TEST_EXT",                 # 0x81B5
    "GL_INDEX_TEST_FUNC_EXT",            # 0x81B6
    "GL_INDEX_TEST_REF_EXT",             # 0x81B7
    "GL_INDEX_MATERIAL_EXT",             # 0x81B8
    "GL_INDEX_MATERIAL_PARAMETER_EXT",   # 0x81B9
    "GL_INDEX_MATERIAL_FACE_EXT",        # 0x81BA
    "GL_YCRCB_422_SGIX",                 # 0x81BB
    "GL_YCRCB_444_SGIX",                 # 0x81BC
    "GL_WRAP_BORDER_SUN",                # 0x81D4
    "GL_UNPACK_CONSTANT_DATA_SUNX",      # 0x81D5
    "GL_TEXTURE_CONSTANT_DATA_SUNX",     # 0x81D6
    "GL_TRIANGLE_LIST_SUN",              # 0x81D7
    "GL_REPLACEMENT_CODE_SUN",           # 0x81D8
    "GL_GLOBAL_ALPHA_SUN",               # 0x81D9
    "GL_GLOBAL_ALPHA_FACTOR_SUN",        # 0x81DA
    "GL_TEXTURE_COLOR_WRITEMASK_SGIS",   # 0x81EF
    "GL_EYE_DISTANCE_TO_POINT_SGIS",     # 0x81F0
    "GL_OBJECT_DISTANCE_TO_POINT_SGIS",  # 0x81F1
    "GL_EYE_DISTANCE_TO_LINE_SGIS",      # 0x81F2
    "GL_OBJECT_DISTANCE_TO_LINE_SGIS",   # 0x81F3
    "GL_EYE_POINT_SGIS",                 # 0x81F4
    "GL_OBJECT_POINT_SGIS",              # 0x81F5
    "GL_EYE_LINE_SGIS",                  # 0x81F6
    "GL_OBJECT_LINE_SGIS",               # 0x81F7
    "GL_LIGHT_MODEL_COLOR_CONTROL",      # 0x81F8
    "GL_SINGLE_COLOR",                   # 0x81F9
    "GL_SEPARATE_SPECULAR_COLOR",        # 0x81FA
    "GL_SHARED_TEXTURE_PALETTE_EXT",     # 0x81FB
    "GL_FOG_SCALE_SGIX",                 # 0x81FC
    "GL_FOG_SCALE_VALUE_SGIX",           # 0x81FD
    "GL_TEXT_FRAGMENT_SHADER_ATI",       # 0x8200
    "GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING", # 0x8210
    "GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE", # 0x8211
    "GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE",# 0x8212
    "GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE", # 0x8213
    "GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE", # 0x8214
    "GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE", # 0x8215
    "GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE", # 0x8216
    "GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE", # 0x8217
    "GL_FRAMEBUFFER_DEFAULT",            # 0x8218
    "GL_FRAMEBUFFER_UNDEFINED",          # 0x8219
    "GL_DEPTH_STENCIL_ATTACHMENT",       # 0x821A
    "GL_MAJOR_VERSION",                  # 0x821B
    "GL_MINOR_VERSION",                  # 0x821C
    "GL_NUM_EXTENSIONS",                 # 0x821D
    "GL_CONTEXT_FLAGS",                  # 0x821E
    "GL_INDEX",                          # 0x8222
    "GL_DEPTH_BUFFER",                   # 0x8223
    "GL_STENCIL_BUFFER",                 # 0x8224
    "GL_COMPRESSED_RED",                 # 0x8225
    "GL_COMPRESSED_RG",                  # 0x8226
    "GL_RG",                             # 0x8227
    "GL_RG_INTEGER",                     # 0x8228
    "GL_R8",                             # 0x8229
    "GL_R16",                            # 0x822A
    "GL_RG8",                            # 0x822B
    "GL_RG16",                           # 0x822C
    "GL_R16F",                           # 0x822D
    "GL_R32F",                           # 0x822E
    "GL_RG16F",                          # 0x822F
    "GL_RG32F",                          # 0x8230
    "GL_R8I",                            # 0x8231
    "GL_R8UI",                           # 0x8232
    "GL_R16I",                           # 0x8233
    "GL_R16UI",                          # 0x8234
    "GL_R32I",                           # 0x8235
    "GL_R32UI",                          # 0x8236
    "GL_RG8I",                           # 0x8237
    "GL_RG8UI",                          # 0x8238
    "GL_RG16I",                          # 0x8239
    "GL_RG16UI",                         # 0x823A
    "GL_RG32I",                          # 0x823B
    "GL_RG32UI",                         # 0x823C
    "GL_DEPTH_PASS_INSTRUMENT_SGIX",     # 0x8310
    "GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX", # 0x8311
    "GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX", # 0x8312
    "GL_CONVOLUTION_HINT_SGIX",          # 0x8316
    "GL_YCRCB_SGIX",                     # 0x8318
    "GL_YCRCBA_SGIX",                    # 0x8319
    "GL_ALPHA_MIN_SGIX",                 # 0x8320
    "GL_ALPHA_MAX_SGIX",                 # 0x8321
    "GL_SCALEBIAS_HINT_SGIX",            # 0x8322
    "GL_ASYNC_MARKER_SGIX",              # 0x8329
    "GL_PIXEL_TEX_GEN_MODE_SGIX",        # 0x832B
    "GL_ASYNC_HISTOGRAM_SGIX",           # 0x832C
    "GL_MAX_ASYNC_HISTOGRAM_SGIX",       # 0x832D
    "GL_PIXEL_TRANSFORM_2D_EXT",         # 0x8330
    "GL_PIXEL_MAG_FILTER_EXT",           # 0x8331
    "GL_PIXEL_MIN_FILTER_EXT",           # 0x8332
    "GL_PIXEL_CUBIC_WEIGHT_EXT",         # 0x8333
    "GL_CUBIC_EXT",                      # 0x8334
    "GL_AVERAGE_EXT",                    # 0x8335
    "GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT", # 0x8336
    "GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT", # 0x8337
    "GL_PIXEL_TRANSFORM_2D_MATRIX_EXT",  # 0x8338
    "GL_FRAGMENT_MATERIAL_EXT",          # 0x8349
    "GL_FRAGMENT_NORMAL_EXT",            # 0x834A
    "GL_FRAGMENT_COLOR_EXT",             # 0x834C
    "GL_ATTENUATION_EXT",                # 0x834D
    "GL_SHADOW_ATTENUATION_EXT",         # 0x834E
    "GL_TEXTURE_APPLICATION_MODE_EXT",   # 0x834F
    "GL_TEXTURE_LIGHT_EXT",              # 0x8350
    "GL_TEXTURE_MATERIAL_FACE_EXT",      # 0x8351
    "GL_TEXTURE_MATERIAL_PARAMETER_EXT", # 0x8352
    "GL_PIXEL_TEXTURE_SGIS",             # 0x8353
    "GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS", # 0x8354
    "GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS", # 0x8355
    "GL_PIXEL_GROUP_COLOR_SGIS",         # 0x8356
    "GL_ASYNC_TEX_IMAGE_SGIX",           # 0x835C
    "GL_ASYNC_DRAW_PIXELS_SGIX",         # 0x835D
    "GL_ASYNC_READ_PIXELS_SGIX",         # 0x835E
    "GL_MAX_ASYNC_TEX_IMAGE_SGIX",       # 0x835F
    "GL_MAX_ASYNC_DRAW_PIXELS_SGIX",     # 0x8360
    "GL_MAX_ASYNC_READ_PIXELS_SGIX",     # 0x8361
    "GL_UNSIGNED_BYTE_2_3_3_REV",        # 0x8362
    "GL_UNSIGNED_SHORT_5_6_5",           # 0x8363
    "GL_UNSIGNED_SHORT_5_6_5_REV",       # 0x8364
    "GL_UNSIGNED_SHORT_4_4_4_4_REV",     # 0x8365
    "GL_UNSIGNED_SHORT_1_5_5_5_REV",     # 0x8366
    "GL_UNSIGNED_INT_8_8_8_8_REV",       # 0x8367
    "GL_UNSIGNED_INT_2_10_10_10_REV",    # 0x8368
    "GL_TEXTURE_MAX_CLAMP_S_SGIX",       # 0x8369
    "GL_TEXTURE_MAX_CLAMP_T_SGIX",       # 0x836A
    "GL_TEXTURE_MAX_CLAMP_R_SGIX",       # 0x836B
    "GL_MIRRORED_REPEAT",                # 0x8370
    "GL_RGB_S3TC",                       # 0x83A0
    "GL_RGB4_S3TC",                      # 0x83A1
    "GL_RGBA_S3TC",                      # 0x83A2
    "GL_RGBA4_S3TC",                     # 0x83A3
    "GL_VERTEX_PRECLIP_SGIX",            # 0x83EE
    "GL_VERTEX_PRECLIP_HINT_SGIX",       # 0x83EF
    "GL_COMPRESSED_RGB_S3TC_DXT1_EXT",   # 0x83F0
    "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT",  # 0x83F1
    "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT",  # 0x83F2
    "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT",  # 0x83F3
    "GL_PARALLEL_ARRAYS_INTEL",          # 0x83F4
    "GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL", # 0x83F5
    "GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL", # 0x83F6
    "GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL", # 0x83F7
    "GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL", # 0x83F8
    "GL_FRAGMENT_LIGHTING_SGIX",         # 0x8400
    "GL_FRAGMENT_COLOR_MATERIAL_SGIX",   # 0x8401
    "GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX", # 0x8402
    "GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX", # 0x8403
    "GL_MAX_FRAGMENT_LIGHTS_SGIX",       # 0x8404
    "GL_MAX_ACTIVE_LIGHTS_SGIX",         # 0x8405
    "GL_CURRENT_RASTER_NORMAL_SGIX",     # 0x8406
    "GL_LIGHT_ENV_MODE_SGIX",            # 0x8407
    "GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX", # 0x8408
    "GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX", # 0x8409
    "GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX", # 0x840A
    "GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX", # 0x840B
    "GL_FRAGMENT_LIGHT0_SGIX",           # 0x840C
    "GL_FRAGMENT_LIGHT1_SGIX",           # 0x840D
    "GL_FRAGMENT_LIGHT2_SGIX",           # 0x840E
    "GL_FRAGMENT_LIGHT3_SGIX",           # 0x840F
    "GL_FRAGMENT_LIGHT4_SGIX",           # 0x8410
    "GL_FRAGMENT_LIGHT5_SGIX",           # 0x8411
    "GL_FRAGMENT_LIGHT6_SGIX",           # 0x8412
    "GL_FRAGMENT_LIGHT7_SGIX",           # 0x8413
    "GL_PACK_RESAMPLE_SGIX",             # 0x842C
    "GL_UNPACK_RESAMPLE_SGIX",           # 0x842D
    "GL_RESAMPLE_REPLICATE_SGIX",        # 0x842E
    "GL_RESAMPLE_ZERO_FILL_SGIX",        # 0x842F
    "GL_RESAMPLE_DECIMATE_SGIX",         # 0x8430
    "GL_TANGENT_ARRAY_EXT",              # 0x8439
    "GL_BINORMAL_ARRAY_EXT",             # 0x843A
    "GL_CURRENT_TANGENT_EXT",            # 0x843B
    "GL_CURRENT_BINORMAL_EXT",           # 0x843C
    "GL_TANGENT_ARRAY_TYPE_EXT",         # 0x843E
    "GL_TANGENT_ARRAY_STRIDE_EXT",       # 0x843F
    "GL_BINORMAL_ARRAY_TYPE_EXT",        # 0x8440
    "GL_BINORMAL_ARRAY_STRIDE_EXT",      # 0x8441
    "GL_TANGENT_ARRAY_POINTER_EXT",      # 0x8442
    "GL_BINORMAL_ARRAY_POINTER_EXT",     # 0x8443
    "GL_MAP1_TANGENT_EXT",               # 0x8444
    "GL_MAP2_TANGENT_EXT",               # 0x8445
    "GL_MAP1_BINORMAL_EXT",              # 0x8446
    "GL_MAP2_BINORMAL_EXT",              # 0x8447
    "GL_NEAREST_CLIPMAP_NEAREST_SGIX",   # 0x844D
    "GL_NEAREST_CLIPMAP_LINEAR_SGIX",    # 0x844E
    "GL_LINEAR_CLIPMAP_NEAREST_SGIX",    # 0x844F
    "GL_FOG_COORD_SRC",                  # 0x8450
    "GL_FOG_COORD",                      # 0x8451
    "GL_FRAGMENT_DEPTH",                 # 0x8452
    "GL_CURRENT_FOG_COORD",              # 0x8453
    "GL_FOG_COORD_ARRAY_TYPE",           # 0x8454
    "GL_FOG_COORD_ARRAY_STRIDE",         # 0x8455
    "GL_FOG_COORD_ARRAY_POINTER",        # 0x8456
    "GL_FOG_COORD_ARRAY",                # 0x8457
    "GL_COLOR_SUM",                      # 0x8458
    "GL_CURRENT_SECONDARY_COLOR",        # 0x8459
    "GL_SECONDARY_COLOR_ARRAY_SIZE",     # 0x845A
    "GL_SECONDARY_COLOR_ARRAY_TYPE",     # 0x845B
    "GL_SECONDARY_COLOR_ARRAY_STRIDE",   # 0x845C
    "GL_SECONDARY_COLOR_ARRAY_POINTER",  # 0x845D
    "GL_SECONDARY_COLOR_ARRAY",          # 0x845E
    "GL_CURRENT_RASTER_SECONDARY_COLOR", # 0x845F
    "GL_ALIASED_POINT_SIZE_RANGE",       # 0x846D
    "GL_ALIASED_LINE_WIDTH_RANGE",       # 0x846E
    "GL_SCREEN_COORDINATES_REND",        # 0x8490
    "GL_INVERTED_SCREEN_W_REND",         # 0x8491
    "GL_TEXTURE0",                       # 0x84C0
    "GL_TEXTURE1",                       # 0x84C1
    "GL_TEXTURE2",                       # 0x84C2
    "GL_TEXTURE3",                       # 0x84C3
    "GL_TEXTURE4",                       # 0x84C4
    "GL_TEXTURE5",                       # 0x84C5
    "GL_TEXTURE6",                       # 0x84C6
    "GL_TEXTURE7",                       # 0x84C7
    "GL_TEXTURE8",                       # 0x84C8
    "GL_TEXTURE9",                       # 0x84C9
    "GL_TEXTURE10",                      # 0x84CA
    "GL_TEXTURE11",                      # 0x84CB
    "GL_TEXTURE12",                      # 0x84CC
    "GL_TEXTURE13",                      # 0x84CD
    "GL_TEXTURE14",                      # 0x84CE
    "GL_TEXTURE15",                      # 0x84CF
    "GL_TEXTURE16",                      # 0x84D0
    "GL_TEXTURE17",                      # 0x84D1
    "GL_TEXTURE18",                      # 0x84D2
    "GL_TEXTURE19",                      # 0x84D3
    "GL_TEXTURE20",                      # 0x84D4
    "GL_TEXTURE21",                      # 0x84D5
    "GL_TEXTURE22",                      # 0x84D6
    "GL_TEXTURE23",                      # 0x84D7
    "GL_TEXTURE24",                      # 0x84D8
    "GL_TEXTURE25",                      # 0x84D9
    "GL_TEXTURE26",                      # 0x84DA
    "GL_TEXTURE27",                      # 0x84DB
    "GL_TEXTURE28",                      # 0x84DC
    "GL_TEXTURE29",                      # 0x84DD
    "GL_TEXTURE30",                      # 0x84DE
    "GL_TEXTURE31",                      # 0x84DF
    "GL_ACTIVE_TEXTURE",                 # 0x84E0
    "GL_CLIENT_ACTIVE_TEXTURE",          # 0x84E1
    "GL_MAX_TEXTURE_UNITS",              # 0x84E2
    "GL_TRANSPOSE_MODELVIEW_MATRIX",     # 0x84E3
    "GL_TRANSPOSE_PROJECTION_MATRIX",    # 0x84E4
    "GL_TRANSPOSE_TEXTURE_MATRIX",       # 0x84E5
    "GL_TRANSPOSE_COLOR_MATRIX",         # 0x84E6
    "GL_SUBTRACT",                       # 0x84E7
    "GL_MAX_RENDERBUFFER_SIZE",          # 0x84E8
    "GL_COMPRESSED_ALPHA",               # 0x84E9
    "GL_COMPRESSED_LUMINANCE",           # 0x84EA
    "GL_COMPRESSED_LUMINANCE_ALPHA",     # 0x84EB
    "GL_COMPRESSED_INTENSITY",           # 0x84EC
    "GL_COMPRESSED_RGB",                 # 0x84ED
    "GL_COMPRESSED_RGBA",                # 0x84EE
    "GL_TEXTURE_COMPRESSION_HINT",       # 0x84EF
    "GL_ALL_COMPLETED_NV",               # 0x84F2
    "GL_FENCE_STATUS_NV",                # 0x84F3
    "GL_FENCE_CONDITION_NV",             # 0x84F4
    "GL_TEXTURE_RECTANGLE",              # 0x84F5
    "GL_TEXTURE_BINDING_RECTANGLE",      # 0x84F6
    "GL_PROXY_TEXTURE_RECTANGLE",        # 0x84F7
    "GL_MAX_RECTANGLE_TEXTURE_SIZE",     # 0x84F8
    "GL_DEPTH_STENCIL",                  # 0x84F9
    "GL_UNSIGNED_INT_24_8",              # 0x84FA
    "GL_MAX_TEXTURE_LOD_BIAS",           # 0x84FD
    "GL_TEXTURE_MAX_ANISOTROPY_EXT",     # 0x84FE
    "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT", # 0x84FF
    "GL_TEXTURE_FILTER_CONTROL",         # 0x8500
    "GL_TEXTURE_LOD_BIAS",               # 0x8501
    "GL_MODELVIEW1_STACK_DEPTH_EXT",     # 0x8502
    "GL_COMBINE4_NV",                    # 0x8503
    "GL_MAX_SHININESS_NV",               # 0x8504
    "GL_MAX_SPOT_EXPONENT_NV",           # 0x8505
    "GL_MODELVIEW1_MATRIX_EXT",          # 0x8506
    "GL_INCR_WRAP",                      # 0x8507
    "GL_DECR_WRAP",                      # 0x8508
    "GL_VERTEX_WEIGHTING_EXT",           # 0x8509
    "GL_MODELVIEW1_ARB",                 # 0x850A
    "GL_CURRENT_VERTEX_WEIGHT_EXT",      # 0x850B
    "GL_VERTEX_WEIGHT_ARRAY_EXT",        # 0x850C
    "GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT",   # 0x850D
    "GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT",   # 0x850E
    "GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT", # 0x850F
    "GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT",# 0x8510
    "GL_NORMAL_MAP",                     # 0x8511
    "GL_REFLECTION_MAP",                 # 0x8512
    "GL_TEXTURE_CUBE_MAP",               # 0x8513
    "GL_TEXTURE_BINDING_CUBE_MAP",       # 0x8514
    "GL_TEXTURE_CUBE_MAP_POSITIVE_X",    # 0x8515
    "GL_TEXTURE_CUBE_MAP_NEGATIVE_X",    # 0x8516
    "GL_TEXTURE_CUBE_MAP_POSITIVE_Y",    # 0x8517
    "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y",    # 0x8518
    "GL_TEXTURE_CUBE_MAP_POSITIVE_Z",    # 0x8519
    "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z",    # 0x851A
    "GL_PROXY_TEXTURE_CUBE_MAP",         # 0x851B
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",      # 0x851C
    "GL_VERTEX_ARRAY_RANGE_NV",          # 0x851D
    "GL_VERTEX_ARRAY_RANGE_LENGTH_NV",   # 0x851E
    "GL_VERTEX_ARRAY_RANGE_VALID_NV",    # 0x851F
    "GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV", # 0x8520
    "GL_VERTEX_ARRAY_RANGE_POINTER_NV",  # 0x8521
    "GL_REGISTER_COMBINERS_NV",          # 0x8522
    "GL_VARIABLE_A_NV",                  # 0x8523
    "GL_VARIABLE_B_NV",                  # 0x8524
    "GL_VARIABLE_C_NV",                  # 0x8525
    "GL_VARIABLE_D_NV",                  # 0x8526
    "GL_VARIABLE_E_NV",                  # 0x8527
    "GL_VARIABLE_F_NV",                  # 0x8528
    "GL_VARIABLE_G_NV",                  # 0x8529
    "GL_CONSTANT_COLOR0_NV",             # 0x852A
    "GL_CONSTANT_COLOR1_NV",             # 0x852B
    "GL_PRIMARY_COLOR_NV",               # 0x852C
    "GL_SECONDARY_COLOR_NV",             # 0x852D
    "GL_SPARE0_NV",                      # 0x852E
    "GL_SPARE1_NV",                      # 0x852F
    "GL_DISCARD_NV",                     # 0x8530
    "GL_E_TIMES_F_NV",                   # 0x8531
    "GL_SPARE0_PLUS_SECONDARY_COLOR_NV", # 0x8532
    "GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV", # 0x8533
    "GL_MULTISAMPLE_FILTER_HINT_NV",     # 0x8534
    "GL_PER_STAGE_CONSTANTS_NV",         # 0x8535
    "GL_UNSIGNED_IDENTITY_NV",           # 0x8536
    "GL_UNSIGNED_INVERT_NV",             # 0x8537
    "GL_EXPAND_NORMAL_NV",               # 0x8538
    "GL_EXPAND_NEGATE_NV",               # 0x8539
    "GL_HALF_BIAS_NORMAL_NV",            # 0x853A
    "GL_HALF_BIAS_NEGATE_NV",            # 0x853B
    "GL_SIGNED_IDENTITY_NV",             # 0x853C
    "GL_SIGNED_NEGATE_NV",               # 0x853D
    "GL_SCALE_BY_TWO_NV",                # 0x853E
    "GL_SCALE_BY_FOUR_NV",               # 0x853F
    "GL_SCALE_BY_ONE_HALF_NV",           # 0x8540
    "GL_BIAS_BY_NEGATIVE_ONE_HALF_NV",   # 0x8541
    "GL_COMBINER_INPUT_NV",              # 0x8542
    "GL_COMBINER_MAPPING_NV",            # 0x8543
    "GL_COMBINER_COMPONENT_USAGE_NV",    # 0x8544
    "GL_COMBINER_AB_DOT_PRODUCT_NV",     # 0x8545
    "GL_COMBINER_CD_DOT_PRODUCT_NV",     # 0x8546
    "GL_COMBINER_MUX_SUM_NV",            # 0x8547
    "GL_COMBINER_SCALE_NV",              # 0x8548
    "GL_COMBINER_BIAS_NV",               # 0x8549
    "GL_COMBINER_AB_OUTPUT_NV",          # 0x854A
    "GL_COMBINER_CD_OUTPUT_NV",          # 0x854B
    "GL_COMBINER_SUM_OUTPUT_NV",         # 0x854C
    "GL_MAX_GENERAL_COMBINERS_NV",       # 0x854D
    "GL_NUM_GENERAL_COMBINERS_NV",       # 0x854E
    "GL_COLOR_SUM_CLAMP_NV",             # 0x854F
    "GL_COMBINER0_NV",                   # 0x8550
    "GL_COMBINER1_NV",                   # 0x8551
    "GL_COMBINER2_NV",                   # 0x8552
    "GL_COMBINER3_NV",                   # 0x8553
    "GL_COMBINER4_NV",                   # 0x8554
    "GL_COMBINER5_NV",                   # 0x8555
    "GL_COMBINER6_NV",                   # 0x8556
    "GL_COMBINER7_NV",                   # 0x8557
    "GL_PRIMITIVE_RESTART_NV",           # 0x8558
    "GL_PRIMITIVE_RESTART_INDEX_NV",     # 0x8559
    "GL_FOG_DISTANCE_MODE_NV",           # 0x855A
    "GL_EYE_RADIAL_NV",                  # 0x855B
    "GL_EYE_PLANE_ABSOLUTE_NV",          # 0x855C
    "GL_EMBOSS_LIGHT_NV",                # 0x855D
    "GL_EMBOSS_CONSTANT_NV",             # 0x855E
    "GL_EMBOSS_MAP_NV",                  # 0x855F
    "GL_RED_MIN_CLAMP_INGR",             # 0x8560
    "GL_GREEN_MIN_CLAMP_INGR",           # 0x8561
    "GL_BLUE_MIN_CLAMP_INGR",            # 0x8562
    "GL_ALPHA_MIN_CLAMP_INGR",           # 0x8563
    "GL_RED_MAX_CLAMP_INGR",             # 0x8564
    "GL_GREEN_MAX_CLAMP_INGR",           # 0x8565
    "GL_BLUE_MAX_CLAMP_INGR",            # 0x8566
    "GL_ALPHA_MAX_CLAMP_INGR",           # 0x8567
    "GL_INTERLACE_READ_INGR",            # 0x8568
    "GL_COMBINE",                        # 0x8570
    "GL_COMBINE_RGB",                    # 0x8571
    "GL_COMBINE_ALPHA",                  # 0x8572
    "GL_RGB_SCALE",                      # 0x8573
    "GL_ADD_SIGNED",                     # 0x8574
    "GL_INTERPOLATE",                    # 0x8575
    "GL_CONSTANT",                       # 0x8576
    "GL_PRIMARY_COLOR",                  # 0x8577
    "GL_PREVIOUS",                       # 0x8578
    "GL_SOURCE0_RGB",                    # 0x8580
    "GL_SOURCE1_RGB",                    # 0x8581
    "GL_SOURCE2_RGB",                    # 0x8582
    "GL_SOURCE3_RGB_NV",                 # 0x8583
    "GL_SOURCE0_ALPHA",                  # 0x8588
    "GL_SOURCE1_ALPHA",                  # 0x8589
    "GL_SOURCE2_ALPHA",                  # 0x858A
    "GL_OPERAND0_RGB",                   # 0x8590
    "GL_OPERAND1_RGB",                   # 0x8591
    "GL_OPERAND2_RGB",                   # 0x8592
    "GL_OPERAND3_RGB_NV",                # 0x8593
    "GL_OPERAND0_ALPHA",                 # 0x8598
    "GL_OPERAND1_ALPHA",                 # 0x8599
    "GL_OPERAND2_ALPHA",                 # 0x859A
    "GL_OPERAND3_ALPHA_NV",              # 0x859B
    "GL_PACK_SUBSAMPLE_RATE_SGIX",       # 0x85A0
    "GL_UNPACK_SUBSAMPLE_RATE_SGIX",     # 0x85A1
    "GL_PIXEL_SUBSAMPLE_4444_SGIX",      # 0x85A2
    "GL_PIXEL_SUBSAMPLE_2424_SGIX",      # 0x85A3
    "GL_PIXEL_SUBSAMPLE_4242_SGIX",      # 0x85A4
    "GL_PERTURB_EXT",                    # 0x85AE
    "GL_TEXTURE_NORMAL_EXT",             # 0x85AF
    "GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE", # 0x85B0
    "GL_TRANSFORM_HINT_APPLE",           # 0x85B1
    "GL_UNPACK_CLIENT_STORAGE_APPLE",    # 0x85B2
    "GL_VERTEX_ARRAY_BINDING",           # 0x85B5
    "GL_YCBCR_422_APPLE",                # 0x85B9
    "GL_UNSIGNED_SHORT_8_8_MESA",        # 0x85BA
    "GL_UNSIGNED_SHORT_8_8_REV_MESA",    # 0x85BB
    "GL_STORAGE_CACHED_APPLE",           # 0x85BE
    "GL_STORAGE_SHARED_APPLE",           # 0x85BF
    "GL_REPLACEMENT_CODE_ARRAY_SUN",     # 0x85C0
    "GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN",# 0x85C1
    "GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN", # 0x85C2
    "GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN", # 0x85C3
    "GL_R1UI_V3F_SUN",                   # 0x85C4
    "GL_R1UI_C4UB_V3F_SUN",              # 0x85C5
    "GL_R1UI_C3F_V3F_SUN",               # 0x85C6
    "GL_R1UI_N3F_V3F_SUN",               # 0x85C7
    "GL_R1UI_C4F_N3F_V3F_SUN",           # 0x85C8
    "GL_R1UI_T2F_V3F_SUN",               # 0x85C9
    "GL_R1UI_T2F_N3F_V3F_SUN",           # 0x85CA
    "GL_R1UI_T2F_C4F_N3F_V3F_SUN",       # 0x85CB
    "GL_SLICE_ACCUM_SUN",                # 0x85CC
    "GL_QUAD_MESH_SUN",                  # 0x8614
    "GL_TRIANGLE_MESH_SUN",              # 0x8615
    "GL_VERTEX_PROGRAM_NV",              # 0x8620
    "GL_VERTEX_STATE_PROGRAM_NV",        # 0x8621
    "GL_VERTEX_ATTRIB_ARRAY_ENABLED",    # 0x8622
    "GL_VERTEX_ATTRIB_ARRAY_SIZE",       # 0x8623
    "GL_VERTEX_ATTRIB_ARRAY_STRIDE",     # 0x8624
    "GL_VERTEX_ATTRIB_ARRAY_TYPE",       # 0x8625
    "GL_CURRENT_VERTEX_ATTRIB",          # 0x8626
    "GL_PROGRAM_LENGTH_ARB",             # 0x8627
    "GL_PROGRAM_STRING_ARB",             # 0x8628
    "GL_MODELVIEW_PROJECTION_NV",        # 0x8629
    "GL_IDENTITY_NV",                    # 0x862A
    "GL_INVERSE_NV",                     # 0x862B
    "GL_TRANSPOSE_NV",                   # 0x862C
    "GL_INVERSE_TRANSPOSE_NV",           # 0x862D
    "GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB", # 0x862E
    "GL_MAX_PROGRAM_MATRICES_ARB",       # 0x862F
    "GL_MATRIX0_NV",                     # 0x8630
    "GL_MATRIX1_NV",                     # 0x8631
    "GL_MATRIX2_NV",                     # 0x8632
    "GL_MATRIX3_NV",                     # 0x8633
    "GL_MATRIX4_NV",                     # 0x8634
    "GL_MATRIX5_NV",                     # 0x8635
    "GL_MATRIX6_NV",                     # 0x8636
    "GL_MATRIX7_NV",                     # 0x8637
    "GL_CURRENT_MATRIX_STACK_DEPTH_ARB", # 0x8640
    "GL_CURRENT_MATRIX_ARB",             # 0x8641
    "GL_VERTEX_PROGRAM_POINT_SIZE",      # 0x8642
    "GL_VERTEX_PROGRAM_TWO_SIDE",        # 0x8643
    "GL_PROGRAM_PARAMETER_NV",           # 0x8644
    "GL_VERTEX_ATTRIB_ARRAY_POINTER",    # 0x8645
    "GL_PROGRAM_TARGET_NV",              # 0x8646
    "GL_PROGRAM_RESIDENT_NV",            # 0x8647
    "GL_TRACK_MATRIX_NV",                # 0x8648
    "GL_TRACK_MATRIX_TRANSFORM_NV",      # 0x8649
    "GL_VERTEX_PROGRAM_BINDING_NV",      # 0x864A
    "GL_PROGRAM_ERROR_POSITION_ARB",     # 0x864B
    "GL_OFFSET_TEXTURE_RECTANGLE_NV",    # 0x864C
    "GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV", # 0x864D
    "GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV", # 0x864E
    "GL_DEPTH_CLAMP_NV",                 # 0x864F
    "GL_VERTEX_ATTRIB_ARRAY0_NV",        # 0x8650
    "GL_VERTEX_ATTRIB_ARRAY1_NV",        # 0x8651
    "GL_VERTEX_ATTRIB_ARRAY2_NV",        # 0x8652
    "GL_VERTEX_ATTRIB_ARRAY3_NV",        # 0x8653
    "GL_VERTEX_ATTRIB_ARRAY4_NV",        # 0x8654
    "GL_VERTEX_ATTRIB_ARRAY5_NV",        # 0x8655
    "GL_VERTEX_ATTRIB_ARRAY6_NV",        # 0x8656
    "GL_VERTEX_ATTRIB_ARRAY7_NV",        # 0x8657
    "GL_VERTEX_ATTRIB_ARRAY8_NV",        # 0x8658
    "GL_VERTEX_ATTRIB_ARRAY9_NV",        # 0x8659
    "GL_VERTEX_ATTRIB_ARRAY10_NV",       # 0x865A
    "GL_VERTEX_ATTRIB_ARRAY11_NV",       # 0x865B
    "GL_VERTEX_ATTRIB_ARRAY12_NV",       # 0x865C
    "GL_VERTEX_ATTRIB_ARRAY13_NV",       # 0x865D
    "GL_VERTEX_ATTRIB_ARRAY14_NV",       # 0x865E
    "GL_VERTEX_ATTRIB_ARRAY15_NV",       # 0x865F
    "GL_MAP1_VERTEX_ATTRIB0_4_NV",       # 0x8660
    "GL_MAP1_VERTEX_ATTRIB1_4_NV",       # 0x8661
    "GL_MAP1_VERTEX_ATTRIB2_4_NV",       # 0x8662
    "GL_MAP1_VERTEX_ATTRIB3_4_NV",       # 0x8663
    "GL_MAP1_VERTEX_ATTRIB4_4_NV",       # 0x8664
    "GL_MAP1_VERTEX_ATTRIB5_4_NV",       # 0x8665
    "GL_MAP1_VERTEX_ATTRIB6_4_NV",       # 0x8666
    "GL_MAP1_VERTEX_ATTRIB7_4_NV",       # 0x8667
    "GL_MAP1_VERTEX_ATTRIB8_4_NV",       # 0x8668
    "GL_MAP1_VERTEX_ATTRIB9_4_NV",       # 0x8669
    "GL_MAP1_VERTEX_ATTRIB10_4_NV",      # 0x866A
    "GL_MAP1_VERTEX_ATTRIB11_4_NV",      # 0x866B
    "GL_MAP1_VERTEX_ATTRIB12_4_NV",      # 0x866C
    "GL_MAP1_VERTEX_ATTRIB13_4_NV",      # 0x866D
    "GL_MAP1_VERTEX_ATTRIB14_4_NV",      # 0x866E
    "GL_MAP1_VERTEX_ATTRIB15_4_NV",      # 0x866F
    "GL_MAP2_VERTEX_ATTRIB0_4_NV",       # 0x8670
    "GL_MAP2_VERTEX_ATTRIB1_4_NV",       # 0x8671
    "GL_MAP2_VERTEX_ATTRIB2_4_NV",       # 0x8672
    "GL_MAP2_VERTEX_ATTRIB3_4_NV",       # 0x8673
    "GL_MAP2_VERTEX_ATTRIB4_4_NV",       # 0x8674
    "GL_MAP2_VERTEX_ATTRIB5_4_NV",       # 0x8675
    "GL_MAP2_VERTEX_ATTRIB6_4_NV",       # 0x8676
    "GL_MAP2_VERTEX_ATTRIB7_4_NV",       # 0x8677
    "GL_MAP2_VERTEX_ATTRIB8_4_NV",       # 0x8678
    "GL_MAP2_VERTEX_ATTRIB9_4_NV",       # 0x8679
    "GL_MAP2_VERTEX_ATTRIB10_4_NV",      # 0x867A
    "GL_MAP2_VERTEX_ATTRIB11_4_NV",      # 0x867B
    "GL_MAP2_VERTEX_ATTRIB12_4_NV",      # 0x867C
    "GL_MAP2_VERTEX_ATTRIB13_4_NV",      # 0x867D
    "GL_MAP2_VERTEX_ATTRIB14_4_NV",      # 0x867E
    "GL_MAP2_VERTEX_ATTRIB15_4_NV",      # 0x867F
    "GL_TEXTURE_COMPRESSED_IMAGE_SIZE",  # 0x86A0
    "GL_TEXTURE_COMPRESSED",             # 0x86A1
    "GL_NUM_COMPRESSED_TEXTURE_FORMATS", # 0x86A2
    "GL_COMPRESSED_TEXTURE_FORMATS",     # 0x86A3
    "GL_MAX_VERTEX_UNITS_ARB",           # 0x86A4
    "GL_ACTIVE_VERTEX_UNITS_ARB",        # 0x86A5
    "GL_WEIGHT_SUM_UNITY_ARB",           # 0x86A6
    "GL_VERTEX_BLEND_ARB",               # 0x86A7
    "GL_CURRENT_WEIGHT_ARB",             # 0x86A8
    "GL_WEIGHT_ARRAY_TYPE_ARB",          # 0x86A9
    "GL_WEIGHT_ARRAY_STRIDE_ARB",        # 0x86AA
    "GL_WEIGHT_ARRAY_SIZE_ARB",          # 0x86AB
    "GL_WEIGHT_ARRAY_POINTER_ARB",       # 0x86AC
    "GL_WEIGHT_ARRAY_ARB",               # 0x86AD
    "GL_DOT3_RGB",                       # 0x86AE
    "GL_DOT3_RGBA",                      # 0x86AF
    "GL_COMPRESSED_RGB_FXT1_3DFX",       # 0x86B0
    "GL_COMPRESSED_RGBA_FXT1_3DFX",      # 0x86B1
    "GL_MULTISAMPLE_3DFX",               # 0x86B2
    "GL_SAMPLE_BUFFERS_3DFX",            # 0x86B3
    "GL_SAMPLES_3DFX",                   # 0x86B4
    "GL_EVAL_2D_NV",                     # 0x86C0
    "GL_EVAL_TRIANGULAR_2D_NV",          # 0x86C1
    "GL_MAP_TESSELLATION_NV",            # 0x86C2
    "GL_MAP_ATTRIB_U_ORDER_NV",          # 0x86C3
    "GL_MAP_ATTRIB_V_ORDER_NV",          # 0x86C4
    "GL_EVAL_FRACTIONAL_TESSELLATION_NV",# 0x86C5
    "GL_EVAL_VERTEX_ATTRIB0_NV",         # 0x86C6
    "GL_EVAL_VERTEX_ATTRIB1_NV",         # 0x86C7
    "GL_EVAL_VERTEX_ATTRIB2_NV",         # 0x86C8
    "GL_EVAL_VERTEX_ATTRIB3_NV",         # 0x86C9
    "GL_EVAL_VERTEX_ATTRIB4_NV",         # 0x86CA
    "GL_EVAL_VERTEX_ATTRIB5_NV",         # 0x86CB
    "GL_EVAL_VERTEX_ATTRIB6_NV",         # 0x86CC
    "GL_EVAL_VERTEX_ATTRIB7_NV",         # 0x86CD
    "GL_EVAL_VERTEX_ATTRIB8_NV",         # 0x86CE
    "GL_EVAL_VERTEX_ATTRIB9_NV",         # 0x86CF
    "GL_EVAL_VERTEX_ATTRIB10_NV",        # 0x86D0
    "GL_EVAL_VERTEX_ATTRIB11_NV",        # 0x86D1
    "GL_EVAL_VERTEX_ATTRIB12_NV",        # 0x86D2
    "GL_EVAL_VERTEX_ATTRIB13_NV",        # 0x86D3
    "GL_EVAL_VERTEX_ATTRIB14_NV",        # 0x86D4
    "GL_EVAL_VERTEX_ATTRIB15_NV",        # 0x86D5
    "GL_MAX_MAP_TESSELLATION_NV",        # 0x86D6
    "GL_MAX_RATIONAL_EVAL_ORDER_NV",     # 0x86D7
    "GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV", # 0x86D9
    "GL_UNSIGNED_INT_S8_S8_8_8_NV",      # 0x86DA
    "GL_UNSIGNED_INT_8_8_S8_S8_REV_NV",  # 0x86DB
    "GL_DSDT_MAG_INTENSITY_NV",          # 0x86DC
    "GL_SHADER_CONSISTENT_NV",           # 0x86DD
    "GL_TEXTURE_SHADER_NV",              # 0x86DE
    "GL_SHADER_OPERATION_NV",            # 0x86DF
    "GL_CULL_MODES_NV",                  # 0x86E0
    "GL_OFFSET_TEXTURE_MATRIX_NV",       # 0x86E1
    "GL_OFFSET_TEXTURE_SCALE_NV",        # 0x86E2
    "GL_OFFSET_TEXTURE_BIAS_NV",         # 0x86E3
    "GL_PREVIOUS_TEXTURE_INPUT_NV",      # 0x86E4
    "GL_CONST_EYE_NV",                   # 0x86E5
    "GL_PASS_THROUGH_NV",                # 0x86E6
    "GL_CULL_FRAGMENT_NV",               # 0x86E7
    "GL_OFFSET_TEXTURE_2D_NV",           # 0x86E8
    "GL_DEPENDENT_AR_TEXTURE_2D_NV",     # 0x86E9
    "GL_DEPENDENT_GB_TEXTURE_2D_NV",     # 0x86EA
    "GL_DOT_PRODUCT_NV",                 # 0x86EC
    "GL_DOT_PRODUCT_DEPTH_REPLACE_NV",   # 0x86ED
    "GL_DOT_PRODUCT_TEXTURE_2D_NV",      # 0x86EE
    "GL_DOT_PRODUCT_TEXTURE_3D_NV",      # 0x86EF
    "GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV",# 0x86F0
    "GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV",# 0x86F1
    "GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV",# 0x86F2
    "GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV", # 0x86F3
    "GL_HILO_NV",                        # 0x86F4
    "GL_DSDT_NV",                        # 0x86F5
    "GL_DSDT_MAG_NV",                    # 0x86F6
    "GL_DSDT_MAG_VIB_NV",                # 0x86F7
    "GL_HILO16_NV",                      # 0x86F8
    "GL_SIGNED_HILO_NV",                 # 0x86F9
    "GL_SIGNED_HILO16_NV",               # 0x86FA
    "GL_SIGNED_RGBA_NV",                 # 0x86FB
    "GL_SIGNED_RGBA8_NV",                # 0x86FC
    "GL_SIGNED_RGB_NV",                  # 0x86FE
    "GL_SIGNED_RGB8_NV",                 # 0x86FF
    "GL_SIGNED_LUMINANCE_NV",            # 0x8701
    "GL_SIGNED_LUMINANCE8_NV",           # 0x8702
    "GL_SIGNED_LUMINANCE_ALPHA_NV",      # 0x8703
    "GL_SIGNED_LUMINANCE8_ALPHA8_NV",    # 0x8704
    "GL_SIGNED_ALPHA_NV",                # 0x8705
    "GL_SIGNED_ALPHA8_NV",               # 0x8706
    "GL_SIGNED_INTENSITY_NV",            # 0x8707
    "GL_SIGNED_INTENSITY8_NV",           # 0x8708
    "GL_DSDT8_NV",                       # 0x8709
    "GL_DSDT8_MAG8_NV",                  # 0x870A
    "GL_DSDT8_MAG8_INTENSITY8_NV",       # 0x870B
    "GL_SIGNED_RGB_UNSIGNED_ALPHA_NV",   # 0x870C
    "GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV", # 0x870D
    "GL_HI_SCALE_NV",                    # 0x870E
    "GL_LO_SCALE_NV",                    # 0x870F
    "GL_DS_SCALE_NV",                    # 0x8710
    "GL_DT_SCALE_NV",                    # 0x8711
    "GL_MAGNITUDE_SCALE_NV",             # 0x8712
    "GL_VIBRANCE_SCALE_NV",              # 0x8713
    "GL_HI_BIAS_NV",                     # 0x8714
    "GL_LO_BIAS_NV",                     # 0x8715
    "GL_DS_BIAS_NV",                     # 0x8716
    "GL_DT_BIAS_NV",                     # 0x8717
    "GL_MAGNITUDE_BIAS_NV",              # 0x8718
    "GL_VIBRANCE_BIAS_NV",               # 0x8719
    "GL_TEXTURE_BORDER_VALUES_NV",       # 0x871A
    "GL_TEXTURE_HI_SIZE_NV",             # 0x871B
    "GL_TEXTURE_LO_SIZE_NV",             # 0x871C
    "GL_TEXTURE_DS_SIZE_NV",             # 0x871D
    "GL_TEXTURE_DT_SIZE_NV",             # 0x871E
    "GL_TEXTURE_MAG_SIZE_NV",            # 0x871F
    "GL_MODELVIEW2_ARB",                 # 0x8722
    "GL_MODELVIEW3_ARB",                 # 0x8723
    "GL_MODELVIEW4_ARB",                 # 0x8724
    "GL_MODELVIEW5_ARB",                 # 0x8725
    "GL_MODELVIEW6_ARB",                 # 0x8726
    "GL_MODELVIEW7_ARB",                 # 0x8727
    "GL_MODELVIEW8_ARB",                 # 0x8728
    "GL_MODELVIEW9_ARB",                 # 0x8729
    "GL_MODELVIEW10_ARB",                # 0x872A
    "GL_MODELVIEW11_ARB",                # 0x872B
    "GL_MODELVIEW12_ARB",                # 0x872C
    "GL_MODELVIEW13_ARB",                # 0x872D
    "GL_MODELVIEW14_ARB",                # 0x872E
    "GL_MODELVIEW15_ARB",                # 0x872F
    "GL_MODELVIEW16_ARB",                # 0x8730
    "GL_MODELVIEW17_ARB",                # 0x8731
    "GL_MODELVIEW18_ARB",                # 0x8732
    "GL_MODELVIEW19_ARB",                # 0x8733
    "GL_MODELVIEW20_ARB",                # 0x8734
    "GL_MODELVIEW21_ARB",                # 0x8735
    "GL_MODELVIEW22_ARB",                # 0x8736
    "GL_MODELVIEW23_ARB",                # 0x8737
    "GL_MODELVIEW24_ARB",                # 0x8738
    "GL_MODELVIEW25_ARB",                # 0x8739
    "GL_MODELVIEW26_ARB",                # 0x873A
    "GL_MODELVIEW27_ARB",                # 0x873B
    "GL_MODELVIEW28_ARB",                # 0x873C
    "GL_MODELVIEW29_ARB",                # 0x873D
    "GL_MODELVIEW30_ARB",                # 0x873E
    "GL_MODELVIEW31_ARB",                # 0x873F
    "GL_DOT3_RGB_EXT",                   # 0x8740
    "GL_DOT3_RGBA_EXT",                  # 0x8741
    "GL_MIRROR_CLAMP_ATI",               # 0x8742
    "GL_MIRROR_CLAMP_TO_EDGE_ATI",       # 0x8743
    "GL_MODULATE_ADD_ATI",               # 0x8744
    "GL_MODULATE_SIGNED_ADD_ATI",        # 0x8745
    "GL_MODULATE_SUBTRACT_ATI",          # 0x8746
    "GL_YCBCR_MESA",                     # 0x8757
    "GL_PACK_INVERT_MESA",               # 0x8758
    "GL_TEXTURE_1D_STACK_MESAX",         # 0x8759
    "GL_TEXTURE_2D_STACK_MESAX",         # 0x875A
    "GL_PROXY_TEXTURE_1D_STACK_MESAX",   # 0x875B
    "GL_PROXY_TEXTURE_2D_STACK_MESAX",   # 0x875C
    "GL_TEXTURE_1D_STACK_BINDING_MESAX", # 0x875D
    "GL_TEXTURE_2D_STACK_BINDING_MESAX", # 0x875E
    "GL_STATIC_ATI",                     # 0x8760
    "GL_DYNAMIC_ATI",                    # 0x8761
    "GL_PRESERVE_ATI",                   # 0x8762
    "GL_DISCARD_ATI",                    # 0x8763
    "GL_BUFFER_SIZE",                    # 0x8764
    "GL_BUFFER_USAGE",                   # 0x8765
    "GL_ARRAY_OBJECT_BUFFER_ATI",        # 0x8766
    "GL_ARRAY_OBJECT_OFFSET_ATI",        # 0x8767
    "GL_ELEMENT_ARRAY_ATI",              # 0x8768
    "GL_ELEMENT_ARRAY_TYPE_ATI",         # 0x8769
    "GL_ELEMENT_ARRAY_POINTER_ATI",      # 0x876A
    "GL_MAX_VERTEX_STREAMS_ATI",         # 0x876B
    "GL_VERTEX_STREAM0_ATI",             # 0x876C
    "GL_VERTEX_STREAM1_ATI",             # 0x876D
    "GL_VERTEX_STREAM2_ATI",             # 0x876E
    "GL_VERTEX_STREAM3_ATI",             # 0x876F
    "GL_VERTEX_STREAM4_ATI",             # 0x8770
    "GL_VERTEX_STREAM5_ATI",             # 0x8771
    "GL_VERTEX_STREAM6_ATI",             # 0x8772
    "GL_VERTEX_STREAM7_ATI",             # 0x8773
    "GL_VERTEX_SOURCE_ATI",              # 0x8774
    "GL_BUMP_ROT_MATRIX_ATI",            # 0x8775
    "GL_BUMP_ROT_MATRIX_SIZE_ATI",       # 0x8776
    "GL_BUMP_NUM_TEX_UNITS_ATI",         # 0x8777
    "GL_BUMP_TEX_UNITS_ATI",             # 0x8778
    "GL_DUDV_ATI",                       # 0x8779
    "GL_DU8DV8_ATI",                     # 0x877A
    "GL_BUMP_ENVMAP_ATI",                # 0x877B
    "GL_BUMP_TARGET_ATI",                # 0x877C
    "GL_VERTEX_SHADER_EXT",              # 0x8780
    "GL_VERTEX_SHADER_BINDING_EXT",      # 0x8781
    "GL_OP_INDEX_EXT",                   # 0x8782
    "GL_OP_NEGATE_EXT",                  # 0x8783
    "GL_OP_DOT3_EXT",                    # 0x8784
    "GL_OP_DOT4_EXT",                    # 0x8785
    "GL_OP_MUL_EXT",                     # 0x8786
    "GL_OP_ADD_EXT",                     # 0x8787
    "GL_OP_MADD_EXT",                    # 0x8788
    "GL_OP_FRAC_EXT",                    # 0x8789
    "GL_OP_MAX_EXT",                     # 0x878A
    "GL_OP_MIN_EXT",                     # 0x878B
    "GL_OP_SET_GE_EXT",                  # 0x878C
    "GL_OP_SET_LT_EXT",                  # 0x878D
    "GL_OP_CLAMP_EXT",                   # 0x878E
    "GL_OP_FLOOR_EXT",                   # 0x878F
    "GL_OP_ROUND_EXT",                   # 0x8790
    "GL_OP_EXP_BASE_2_EXT",              # 0x8791
    "GL_OP_LOG_BASE_2_EXT",              # 0x8792
    "GL_OP_POWER_EXT",                   # 0x8793
    "GL_OP_RECIP_EXT",                   # 0x8794
    "GL_OP_RECIP_SQRT_EXT",              # 0x8795
    "GL_OP_SUB_EXT",                     # 0x8796
    "GL_OP_CROSS_PRODUCT_EXT",           # 0x8797
    "GL_OP_MULTIPLY_MATRIX_EXT",         # 0x8798
    "GL_OP_MOV_EXT",                     # 0x8799
    "GL_OUTPUT_VERTEX_EXT",              # 0x879A
    "GL_OUTPUT_COLOR0_EXT",              # 0x879B
    "GL_OUTPUT_COLOR1_EXT",              # 0x879C
    "GL_OUTPUT_TEXTURE_COORD0_EXT",      # 0x879D
    "GL_OUTPUT_TEXTURE_COORD1_EXT",      # 0x879E
    "GL_OUTPUT_TEXTURE_COORD2_EXT",      # 0x879F
    "GL_OUTPUT_TEXTURE_COORD3_EXT",      # 0x87A0
    "GL_OUTPUT_TEXTURE_COORD4_EXT",      # 0x87A1
    "GL_OUTPUT_TEXTURE_COORD5_EXT",      # 0x87A2
    "GL_OUTPUT_TEXTURE_COORD6_EXT",      # 0x87A3
    "GL_OUTPUT_TEXTURE_COORD7_EXT",      # 0x87A4
    "GL_OUTPUT_TEXTURE_COORD8_EXT",      # 0x87A5
    "GL_OUTPUT_TEXTURE_COORD9_EXT",      # 0x87A6
    "GL_OUTPUT_TEXTURE_COORD10_EXT",     # 0x87A7
    "GL_OUTPUT_TEXTURE_COORD11_EXT",     # 0x87A8
    "GL_OUTPUT_TEXTURE_COORD12_EXT",     # 0x87A9
    "GL_OUTPUT_TEXTURE_COORD13_EXT",     # 0x87AA
    "GL_OUTPUT_TEXTURE_COORD14_EXT",     # 0x87AB
    "GL_OUTPUT_TEXTURE_COORD15_EXT",     # 0x87AC
    "GL_OUTPUT_TEXTURE_COORD16_EXT",     # 0x87AD
    "GL_OUTPUT_TEXTURE_COORD17_EXT",     # 0x87AE
    "GL_OUTPUT_TEXTURE_COORD18_EXT",     # 0x87AF
    "GL_OUTPUT_TEXTURE_COORD19_EXT",     # 0x87B0
    "GL_OUTPUT_TEXTURE_COORD20_EXT",     # 0x87B1
    "GL_OUTPUT_TEXTURE_COORD21_EXT",     # 0x87B2
    "GL_OUTPUT_TEXTURE_COORD22_EXT",     # 0x87B3
    "GL_OUTPUT_TEXTURE_COORD23_EXT",     # 0x87B4
    "GL_OUTPUT_TEXTURE_COORD24_EXT",     # 0x87B5
    "GL_OUTPUT_TEXTURE_COORD25_EXT",     # 0x87B6
    "GL_OUTPUT_TEXTURE_COORD26_EXT",     # 0x87B7
    "GL_OUTPUT_TEXTURE_COORD27_EXT",     # 0x87B8
    "GL_OUTPUT_TEXTURE_COORD28_EXT",     # 0x87B9
    "GL_OUTPUT_TEXTURE_COORD29_EXT",     # 0x87BA
    "GL_OUTPUT_TEXTURE_COORD30_EXT",     # 0x87BB
    "GL_OUTPUT_TEXTURE_COORD31_EXT",     # 0x87BC
    "GL_OUTPUT_FOG_EXT",                 # 0x87BD
    "GL_SCALAR_EXT",                     # 0x87BE
    "GL_VECTOR_EXT",                     # 0x87BF
    "GL_MATRIX_EXT",                     # 0x87C0
    "GL_VARIANT_EXT",                    # 0x87C1
    "GL_INVARIANT_EXT",                  # 0x87C2
    "GL_LOCAL_CONSTANT_EXT",             # 0x87C3
    "GL_LOCAL_EXT",                      # 0x87C4
    "GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT", # 0x87C5
    "GL_MAX_VERTEX_SHADER_VARIANTS_EXT", # 0x87C6
    "GL_MAX_VERTEX_SHADER_INVARIANTS_EXT", # 0x87C7
    "GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT", # 0x87C8
    "GL_MAX_VERTEX_SHADER_LOCALS_EXT",   # 0x87C9
    "GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT", # 0x87CA
    "GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT", # 0x87CB
    "GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT", # 0x87CC
    "GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT", # 0x87CD
    "GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT", # 0x87CE
    "GL_VERTEX_SHADER_INSTRUCTIONS_EXT", # 0x87CF
    "GL_VERTEX_SHADER_VARIANTS_EXT",     # 0x87D0
    "GL_VERTEX_SHADER_INVARIANTS_EXT",   # 0x87D1
    "GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT", # 0x87D2
    "GL_VERTEX_SHADER_LOCALS_EXT",       # 0x87D3
    "GL_VERTEX_SHADER_OPTIMIZED_EXT",    # 0x87D4
    "GL_X_EXT",                          # 0x87D5
    "GL_Y_EXT",                          # 0x87D6
    "GL_Z_EXT",                          # 0x87D7
    "GL_W_EXT",                          # 0x87D8
    "GL_NEGATIVE_X_EXT",                 # 0x87D9
    "GL_NEGATIVE_Y_EXT",                 # 0x87DA
    "GL_NEGATIVE_Z_EXT",                 # 0x87DB
    "GL_NEGATIVE_W_EXT",                 # 0x87DC
    "GL_ZERO_EXT",                       # 0x87DD
    "GL_ONE_EXT",                        # 0x87DE
    "GL_NEGATIVE_ONE_EXT",               # 0x87DF
    "GL_NORMALIZED_RANGE_EXT",           # 0x87E0
    "GL_FULL_RANGE_EXT",                 # 0x87E1
    "GL_CURRENT_VERTEX_EXT",             # 0x87E2
    "GL_MVP_MATRIX_EXT",                 # 0x87E3
    "GL_VARIANT_VALUE_EXT",              # 0x87E4
    "GL_VARIANT_DATATYPE_EXT",           # 0x87E5
    "GL_VARIANT_ARRAY_STRIDE_EXT",       # 0x87E6
    "GL_VARIANT_ARRAY_TYPE_EXT",         # 0x87E7
    "GL_VARIANT_ARRAY_EXT",              # 0x87E8
    "GL_VARIANT_ARRAY_POINTER_EXT",      # 0x87E9
    "GL_INVARIANT_VALUE_EXT",            # 0x87EA
    "GL_INVARIANT_DATATYPE_EXT",         # 0x87EB
    "GL_LOCAL_CONSTANT_VALUE_EXT",       # 0x87EC
    "GL_LOCAL_CONSTANT_DATATYPE_EXT",    # 0x87ED
    "GL_PN_TRIANGLES_ATI",               # 0x87F0
    "GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI", # 0x87F1
    "GL_PN_TRIANGLES_POINT_MODE_ATI",    # 0x87F2
    "GL_PN_TRIANGLES_NORMAL_MODE_ATI",   # 0x87F3
    "GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI", # 0x87F4
    "GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI", # 0x87F5
    "GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI", # 0x87F6
    "GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI", # 0x87F7
    "GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI", # 0x87F8
    "GL_VBO_FREE_MEMORY_ATI",            # 0x87FB
    "GL_TEXTURE_FREE_MEMORY_ATI",        # 0x87FC
    "GL_RENDERBUFFER_FREE_MEMORY_ATI",   # 0x87FD
    "GL_STENCIL_BACK_FUNC",              # 0x8800
    "GL_STENCIL_BACK_FAIL",              # 0x8801
    "GL_STENCIL_BACK_PASS_DEPTH_FAIL",   # 0x8802
    "GL_STENCIL_BACK_PASS_DEPTH_PASS",   # 0x8803
    "GL_FRAGMENT_PROGRAM_ARB",           # 0x8804
    "GL_PROGRAM_ALU_INSTRUCTIONS_ARB",   # 0x8805
    "GL_PROGRAM_TEX_INSTRUCTIONS_ARB",   # 0x8806
    "GL_PROGRAM_TEX_INDIRECTIONS_ARB",   # 0x8807
    "GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB", # 0x8808
    "GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB", # 0x8809
    "GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB", # 0x880A
    "GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB", # 0x880B
    "GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB", # 0x880C
    "GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB", # 0x880D
    "GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB", # 0x880E
    "GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB", # 0x880F
    "GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB", # 0x8810
    "GL_RGBA32F",                        # 0x8814
    "GL_RGB32F",                         # 0x8815
    "GL_ALPHA32F_ARB",                   # 0x8816
    "GL_INTENSITY32F_ARB",               # 0x8817
    "GL_LUMINANCE32F_ARB",               # 0x8818
    "GL_LUMINANCE_ALPHA32F_ARB",         # 0x8819
    "GL_RGBA16F",                        # 0x881A
    "GL_RGB16F",                         # 0x881B
    "GL_ALPHA16F_ARB",                   # 0x881C
    "GL_INTENSITY16F_ARB",               # 0x881D
    "GL_LUMINANCE16F_ARB",               # 0x881E
    "GL_LUMINANCE_ALPHA16F_ARB",         # 0x881F
    "GL_RGBA_FLOAT_MODE_ARB",            # 0x8820
    "GL_MAX_DRAW_BUFFERS",               # 0x8824
    "GL_DRAW_BUFFER0",                   # 0x8825
    "GL_DRAW_BUFFER1",                   # 0x8826
    "GL_DRAW_BUFFER2",                   # 0x8827
    "GL_DRAW_BUFFER3",                   # 0x8828
    "GL_DRAW_BUFFER4",                   # 0x8829
    "GL_DRAW_BUFFER5",                   # 0x882A
    "GL_DRAW_BUFFER6",                   # 0x882B
    "GL_DRAW_BUFFER7",                   # 0x882C
    "GL_DRAW_BUFFER8",                   # 0x882D
    "GL_DRAW_BUFFER9",                   # 0x882E
    "GL_DRAW_BUFFER10",                  # 0x882F
    "GL_DRAW_BUFFER11",                  # 0x8830
    "GL_DRAW_BUFFER12",                  # 0x8831
    "GL_DRAW_BUFFER13",                  # 0x8832
    "GL_DRAW_BUFFER14",                  # 0x8833
    "GL_DRAW_BUFFER15",                  # 0x8834
    "GL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI",# 0x8835
    "GL_BLEND_EQUATION_ALPHA",           # 0x883D
    "GL_MATRIX_PALETTE_ARB",             # 0x8840
    "GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB", # 0x8841
    "GL_MAX_PALETTE_MATRICES_ARB",       # 0x8842
    "GL_CURRENT_PALETTE_MATRIX_ARB",     # 0x8843
    "GL_MATRIX_INDEX_ARRAY_ARB",         # 0x8844
    "GL_CURRENT_MATRIX_INDEX_ARB",       # 0x8845
    "GL_MATRIX_INDEX_ARRAY_SIZE_ARB",    # 0x8846
    "GL_MATRIX_INDEX_ARRAY_TYPE_ARB",    # 0x8847
    "GL_MATRIX_INDEX_ARRAY_STRIDE_ARB",  # 0x8848
    "GL_MATRIX_INDEX_ARRAY_POINTER_ARB", # 0x8849
    "GL_TEXTURE_DEPTH_SIZE",             # 0x884A
    "GL_DEPTH_TEXTURE_MODE",             # 0x884B
    "GL_TEXTURE_COMPARE_MODE",           # 0x884C
    "GL_TEXTURE_COMPARE_FUNC",           # 0x884D
    "GL_COMPARE_REF_TO_TEXTURE",         # 0x884E
    "GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV",# 0x8850
    "GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV", # 0x8851
    "GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV", # 0x8852
    "GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV", # 0x8853
    "GL_OFFSET_HILO_TEXTURE_2D_NV",      # 0x8854
    "GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV", # 0x8855
    "GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV", # 0x8856
    "GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV", # 0x8857
    "GL_DEPENDENT_HILO_TEXTURE_2D_NV",   # 0x8858
    "GL_DEPENDENT_RGB_TEXTURE_3D_NV",    # 0x8859
    "GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV", # 0x885A
    "GL_DOT_PRODUCT_PASS_THROUGH_NV",    # 0x885B
    "GL_DOT_PRODUCT_TEXTURE_1D_NV",      # 0x885C
    "GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV", # 0x885D
    "GL_HILO8_NV",                       # 0x885E
    "GL_SIGNED_HILO8_NV",                # 0x885F
    "GL_FORCE_BLUE_TO_ONE_NV",           # 0x8860
    "GL_POINT_SPRITE",                   # 0x8861
    "GL_COORD_REPLACE",                  # 0x8862
    "GL_POINT_SPRITE_R_MODE_NV",         # 0x8863
    "GL_QUERY_COUNTER_BITS",             # 0x8864
    "GL_CURRENT_QUERY",                  # 0x8865
    "GL_QUERY_RESULT",                   # 0x8866
    "GL_QUERY_RESULT_AVAILABLE",         # 0x8867
    "GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV", # 0x8868
    "GL_MAX_VERTEX_ATTRIBS",             # 0x8869
    "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED", # 0x886A
    "GL_DEPTH_STENCIL_TO_RGBA_NV",       # 0x886E
    "GL_DEPTH_STENCIL_TO_BGRA_NV",       # 0x886F
    "GL_FRAGMENT_PROGRAM_NV",            # 0x8870
    "GL_MAX_TEXTURE_COORDS",             # 0x8871
    "GL_MAX_TEXTURE_IMAGE_UNITS",        # 0x8872
    "GL_FRAGMENT_PROGRAM_BINDING_NV",    # 0x8873
    "GL_PROGRAM_ERROR_STRING_ARB",       # 0x8874
    "GL_PROGRAM_FORMAT_ASCII_ARB",       # 0x8875
    "GL_PROGRAM_FORMAT_ARB",             # 0x8876
    "GL_WRITE_PIXEL_DATA_RANGE_NV",      # 0x8878
    "GL_READ_PIXEL_DATA_RANGE_NV",       # 0x8879
    "GL_WRITE_PIXEL_DATA_RANGE_LENGTH_NV", # 0x887A
    "GL_READ_PIXEL_DATA_RANGE_LENGTH_NV",# 0x887B
    "GL_WRITE_PIXEL_DATA_RANGE_POINTER_NV", # 0x887C
    "GL_READ_PIXEL_DATA_RANGE_POINTER_NV", # 0x887D
    "GL_FLOAT_R_NV",                     # 0x8880
    "GL_FLOAT_RG_NV",                    # 0x8881
    "GL_FLOAT_RGB_NV",                   # 0x8882
    "GL_FLOAT_RGBA_NV",                  # 0x8883
    "GL_FLOAT_R16_NV",                   # 0x8884
    "GL_FLOAT_R32_NV",                   # 0x8885
    "GL_FLOAT_RG16_NV",                  # 0x8886
    "GL_FLOAT_RG32_NV",                  # 0x8887
    "GL_FLOAT_RGB16_NV",                 # 0x8888
    "GL_FLOAT_RGB32_NV",                 # 0x8889
    "GL_FLOAT_RGBA16_NV",                # 0x888A
    "GL_FLOAT_RGBA32_NV",                # 0x888B
    "GL_TEXTURE_FLOAT_COMPONENTS_NV",    # 0x888C
    "GL_FLOAT_CLEAR_COLOR_VALUE_NV",     # 0x888D
    "GL_FLOAT_RGBA_MODE_NV",             # 0x888E
    "GL_TEXTURE_UNSIGNED_REMAP_MODE_NV", # 0x888F
    "GL_DEPTH_BOUNDS_TEST_EXT",          # 0x8890
    "GL_DEPTH_BOUNDS_EXT",               # 0x8891
    "GL_ARRAY_BUFFER",                   # 0x8892
    "GL_ELEMENT_ARRAY_BUFFER",           # 0x8893
    "GL_ARRAY_BUFFER_BINDING",           # 0x8894
    "GL_ELEMENT_ARRAY_BUFFER_BINDING",   # 0x8895
    "GL_VERTEX_ARRAY_BUFFER_BINDING",    # 0x8896
    "GL_NORMAL_ARRAY_BUFFER_BINDING",    # 0x8897
    "GL_COLOR_ARRAY_BUFFER_BINDING",     # 0x8898
    "GL_INDEX_ARRAY_BUFFER_BINDING",     # 0x8899
    "GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING", # 0x889A
    "GL_EDGE_FLAG_ARRAY_BUFFER_BINDING", # 0x889B
    "GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING", # 0x889C
    "GL_FOG_COORD_ARRAY_BUFFER_BINDING", # 0x889D
    "GL_WEIGHT_ARRAY_BUFFER_BINDING",    # 0x889E
    "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING", # 0x889F
    "GL_PROGRAM_INSTRUCTIONS_ARB",       # 0x88A0
    "GL_MAX_PROGRAM_INSTRUCTIONS_ARB",   # 0x88A1
    "GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB",# 0x88A2
    "GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB", # 0x88A3
    "GL_PROGRAM_TEMPORARIES_ARB",        # 0x88A4
    "GL_MAX_PROGRAM_TEMPORARIES_ARB",    # 0x88A5
    "GL_PROGRAM_NATIVE_TEMPORARIES_ARB", # 0x88A6
    "GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB", # 0x88A7
    "GL_PROGRAM_PARAMETERS_ARB",         # 0x88A8
    "GL_MAX_PROGRAM_PARAMETERS_ARB",     # 0x88A9
    "GL_PROGRAM_NATIVE_PARAMETERS_ARB",  # 0x88AA
    "GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB", # 0x88AB
    "GL_PROGRAM_ATTRIBS_ARB",            # 0x88AC
    "GL_MAX_PROGRAM_ATTRIBS_ARB",        # 0x88AD
    "GL_PROGRAM_NATIVE_ATTRIBS_ARB",     # 0x88AE
    "GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB", # 0x88AF
    "GL_PROGRAM_ADDRESS_REGISTERS_ARB",  # 0x88B0
    "GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB", # 0x88B1
    "GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB", # 0x88B2
    "GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB", # 0x88B3
    "GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB", # 0x88B4
    "GL_MAX_PROGRAM_ENV_PARAMETERS_ARB", # 0x88B5
    "GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB",# 0x88B6
    "GL_TRANSPOSE_CURRENT_MATRIX_ARB",   # 0x88B7
    "GL_READ_ONLY",                      # 0x88B8
    "GL_WRITE_ONLY",                     # 0x88B9
    "GL_READ_WRITE",                     # 0x88BA
    "GL_BUFFER_ACCESS",                  # 0x88BB
    "GL_BUFFER_MAPPED",                  # 0x88BC
    "GL_BUFFER_MAP_POINTER",             # 0x88BD
    "GL_TIME_ELAPSED_EXT",               # 0x88BF
    "GL_MATRIX0_ARB",                    # 0x88C0
    "GL_MATRIX1_ARB",                    # 0x88C1
    "GL_MATRIX2_ARB",                    # 0x88C2
    "GL_MATRIX3_ARB",                    # 0x88C3
    "GL_MATRIX4_ARB",                    # 0x88C4
    "GL_MATRIX5_ARB",                    # 0x88C5
    "GL_MATRIX6_ARB",                    # 0x88C6
    "GL_MATRIX7_ARB",                    # 0x88C7
    "GL_MATRIX8_ARB",                    # 0x88C8
    "GL_MATRIX9_ARB",                    # 0x88C9
    "GL_MATRIX10_ARB",                   # 0x88CA
    "GL_MATRIX11_ARB",                   # 0x88CB
    "GL_MATRIX12_ARB",                   # 0x88CC
    "GL_MATRIX13_ARB",                   # 0x88CD
    "GL_MATRIX14_ARB",                   # 0x88CE
    "GL_MATRIX15_ARB",                   # 0x88CF
    "GL_MATRIX16_ARB",                   # 0x88D0
    "GL_MATRIX17_ARB",                   # 0x88D1
    "GL_MATRIX18_ARB",                   # 0x88D2
    "GL_MATRIX19_ARB",                   # 0x88D3
    "GL_MATRIX20_ARB",                   # 0x88D4
    "GL_MATRIX21_ARB",                   # 0x88D5
    "GL_MATRIX22_ARB",                   # 0x88D6
    "GL_MATRIX23_ARB",                   # 0x88D7
    "GL_MATRIX24_ARB",                   # 0x88D8
    "GL_MATRIX25_ARB",                   # 0x88D9
    "GL_MATRIX26_ARB",                   # 0x88DA
    "GL_MATRIX27_ARB",                   # 0x88DB
    "GL_MATRIX28_ARB",                   # 0x88DC
    "GL_MATRIX29_ARB",                   # 0x88DD
    "GL_MATRIX30_ARB",                   # 0x88DE
    "GL_MATRIX31_ARB",                   # 0x88DF
    "GL_STREAM_DRAW",                    # 0x88E0
    "GL_STREAM_READ",                    # 0x88E1
    "GL_STREAM_COPY",                    # 0x88E2
    "GL_STATIC_DRAW",                    # 0x88E4
    "GL_STATIC_READ",                    # 0x88E5
    "GL_STATIC_COPY",                    # 0x88E6
    "GL_DYNAMIC_DRAW",                   # 0x88E8
    "GL_DYNAMIC_READ",                   # 0x88E9
    "GL_DYNAMIC_COPY",                   # 0x88EA
    "GL_PIXEL_PACK_BUFFER",              # 0x88EB
    "GL_PIXEL_UNPACK_BUFFER",            # 0x88EC
    "GL_PIXEL_PACK_BUFFER_BINDING",      # 0x88ED
    "GL_PIXEL_UNPACK_BUFFER_BINDING",    # 0x88EF
    "GL_DEPTH24_STENCIL8",               # 0x88F0
    "GL_TEXTURE_STENCIL_SIZE",           # 0x88F1
    "GL_STENCIL_TAG_BITS_EXT",           # 0x88F2
    "GL_STENCIL_CLEAR_TAG_VALUE_EXT",    # 0x88F3
    "GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV", # 0x88F4
    "GL_MAX_PROGRAM_CALL_DEPTH_NV",      # 0x88F5
    "GL_MAX_PROGRAM_IF_DEPTH_NV",        # 0x88F6
    "GL_MAX_PROGRAM_LOOP_DEPTH_NV",      # 0x88F7
    "GL_MAX_PROGRAM_LOOP_COUNT_NV",      # 0x88F8
    "GL_VERTEX_ATTRIB_ARRAY_INTEGER",    # 0x88FD
    "GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ARB",# 0x88FE
    "GL_MAX_ARRAY_TEXTURE_LAYERS",       # 0x88FF
    "GL_MIN_PROGRAM_TEXEL_OFFSET",       # 0x8904
    "GL_MAX_PROGRAM_TEXEL_OFFSET",       # 0x8905
    "GL_PROGRAM_ATTRIB_COMPONENTS_NV",   # 0x8906
    "GL_PROGRAM_RESULT_COMPONENTS_NV",   # 0x8907
    "GL_MAX_PROGRAM_ATTRIB_COMPONENTS_NV", # 0x8908
    "GL_MAX_PROGRAM_RESULT_COMPONENTS_NV", # 0x8909
    "GL_STENCIL_TEST_TWO_SIDE_EXT",      # 0x8910
    "GL_ACTIVE_STENCIL_FACE_EXT",        # 0x8911
    "GL_MIRROR_CLAMP_TO_BORDER_EXT",     # 0x8912
    "GL_SAMPLES_PASSED",                 # 0x8914
    "GL_CLAMP_VERTEX_COLOR",             # 0x891A
    "GL_CLAMP_FRAGMENT_COLOR",           # 0x891B
    "GL_CLAMP_READ_COLOR",               # 0x891C
    "GL_FIXED_ONLY",                     # 0x891D
    "GL_FRAGMENT_SHADER_ATI",            # 0x8920
    "GL_REG_0_ATI",                      # 0x8921
    "GL_REG_1_ATI",                      # 0x8922
    "GL_REG_2_ATI",                      # 0x8923
    "GL_REG_3_ATI",                      # 0x8924
    "GL_REG_4_ATI",                      # 0x8925
    "GL_REG_5_ATI",                      # 0x8926
    "GL_REG_6_ATI",                      # 0x8927
    "GL_REG_7_ATI",                      # 0x8928
    "GL_REG_8_ATI",                      # 0x8929
    "GL_REG_9_ATI",                      # 0x892A
    "GL_REG_10_ATI",                     # 0x892B
    "GL_REG_11_ATI",                     # 0x892C
    "GL_REG_12_ATI",                     # 0x892D
    "GL_REG_13_ATI",                     # 0x892E
    "GL_REG_14_ATI",                     # 0x892F
    "GL_REG_15_ATI",                     # 0x8930
    "GL_REG_16_ATI",                     # 0x8931
    "GL_REG_17_ATI",                     # 0x8932
    "GL_REG_18_ATI",                     # 0x8933
    "GL_REG_19_ATI",                     # 0x8934
    "GL_REG_20_ATI",                     # 0x8935
    "GL_REG_21_ATI",                     # 0x8936
    "GL_REG_22_ATI",                     # 0x8937
    "GL_REG_23_ATI",                     # 0x8938
    "GL_REG_24_ATI",                     # 0x8939
    "GL_REG_25_ATI",                     # 0x893A
    "GL_REG_26_ATI",                     # 0x893B
    "GL_REG_27_ATI",                     # 0x893C
    "GL_REG_28_ATI",                     # 0x893D
    "GL_REG_29_ATI",                     # 0x893E
    "GL_REG_30_ATI",                     # 0x893F
    "GL_REG_31_ATI",                     # 0x8940
    "GL_CON_0_ATI",                      # 0x8941
    "GL_CON_1_ATI",                      # 0x8942
    "GL_CON_2_ATI",                      # 0x8943
    "GL_CON_3_ATI",                      # 0x8944
    "GL_CON_4_ATI",                      # 0x8945
    "GL_CON_5_ATI",                      # 0x8946
    "GL_CON_6_ATI",                      # 0x8947
    "GL_CON_7_ATI",                      # 0x8948
    "GL_CON_8_ATI",                      # 0x8949
    "GL_CON_9_ATI",                      # 0x894A
    "GL_CON_10_ATI",                     # 0x894B
    "GL_CON_11_ATI",                     # 0x894C
    "GL_CON_12_ATI",                     # 0x894D
    "GL_CON_13_ATI",                     # 0x894E
    "GL_CON_14_ATI",                     # 0x894F
    "GL_CON_15_ATI",                     # 0x8950
    "GL_CON_16_ATI",                     # 0x8951
    "GL_CON_17_ATI",                     # 0x8952
    "GL_CON_18_ATI",                     # 0x8953
    "GL_CON_19_ATI",                     # 0x8954
    "GL_CON_20_ATI",                     # 0x8955
    "GL_CON_21_ATI",                     # 0x8956
    "GL_CON_22_ATI",                     # 0x8957
    "GL_CON_23_ATI",                     # 0x8958
    "GL_CON_24_ATI",                     # 0x8959
    "GL_CON_25_ATI",                     # 0x895A
    "GL_CON_26_ATI",                     # 0x895B
    "GL_CON_27_ATI",                     # 0x895C
    "GL_CON_28_ATI",                     # 0x895D
    "GL_CON_29_ATI",                     # 0x895E
    "GL_CON_30_ATI",                     # 0x895F
    "GL_CON_31_ATI",                     # 0x8960
    "GL_MOV_ATI",                        # 0x8961
    "GL_ADD_ATI",                        # 0x8963
    "GL_MUL_ATI",                        # 0x8964
    "GL_SUB_ATI",                        # 0x8965
    "GL_DOT3_ATI",                       # 0x8966
    "GL_DOT4_ATI",                       # 0x8967
    "GL_MAD_ATI",                        # 0x8968
    "GL_LERP_ATI",                       # 0x8969
    "GL_CND_ATI",                        # 0x896A
    "GL_CND0_ATI",                       # 0x896B
    "GL_DOT2_ADD_ATI",                   # 0x896C
    "GL_SECONDARY_INTERPOLATOR_ATI",     # 0x896D
    "GL_NUM_FRAGMENT_REGISTERS_ATI",     # 0x896E
    "GL_NUM_FRAGMENT_CONSTANTS_ATI",     # 0x896F
    "GL_NUM_PASSES_ATI",                 # 0x8970
    "GL_NUM_INSTRUCTIONS_PER_PASS_ATI",  # 0x8971
    "GL_NUM_INSTRUCTIONS_TOTAL_ATI",     # 0x8972
    "GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI", # 0x8973
    "GL_NUM_LOOPBACK_COMPONENTS_ATI",    # 0x8974
    "GL_COLOR_ALPHA_PAIRING_ATI",        # 0x8975
    "GL_SWIZZLE_STR_ATI",                # 0x8976
    "GL_SWIZZLE_STQ_ATI",                # 0x8977
    "GL_SWIZZLE_STR_DR_ATI",             # 0x8978
    "GL_SWIZZLE_STQ_DQ_ATI",             # 0x8979
    "GL_SWIZZLE_STRQ_ATI",               # 0x897A
    "GL_SWIZZLE_STRQ_DQ_ATI",            # 0x897B
    "GL_INTERLACE_OML",                  # 0x8980
    "GL_INTERLACE_READ_OML",             # 0x8981
    "GL_FORMAT_SUBSAMPLE_24_24_OML",     # 0x8982
    "GL_FORMAT_SUBSAMPLE_244_244_OML",   # 0x8983
    "GL_PACK_RESAMPLE_OML",              # 0x8984
    "GL_UNPACK_RESAMPLE_OML",            # 0x8985
    "GL_RESAMPLE_REPLICATE_OML",         # 0x8986
    "GL_RESAMPLE_ZERO_FILL_OML",         # 0x8987
    "GL_RESAMPLE_AVERAGE_OML",           # 0x8988
    "GL_RESAMPLE_DECIMATE_OML",          # 0x8989
    "GL_DRAW_PIXELS_APPLE",              # 0x8A0A
    "GL_FENCE_APPLE",                    # 0x8A0B
    "GL_UNIFORM_BUFFER",                 # 0x8A11
    "GL_BUFFER_SERIALIZED_MODIFY_APPLE", # 0x8A12
    "GL_BUFFER_FLUSHING_UNMAP_APPLE",    # 0x8A13
    "GL_UNIFORM_BUFFER_BINDING",         # 0x8A28
    "GL_UNIFORM_BUFFER_START",           # 0x8A29
    "GL_UNIFORM_BUFFER_SIZE",            # 0x8A2A
    "GL_MAX_VERTEX_UNIFORM_BLOCKS",      # 0x8A2B
    "GL_MAX_GEOMETRY_UNIFORM_BLOCKS",    # 0x8A2C
    "GL_MAX_FRAGMENT_UNIFORM_BLOCKS",    # 0x8A2D
    "GL_MAX_COMBINED_UNIFORM_BLOCKS",    # 0x8A2E
    "GL_MAX_UNIFORM_BUFFER_BINDINGS",    # 0x8A2F
    "GL_MAX_UNIFORM_BLOCK_SIZE",         # 0x8A30
    "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", # 0x8A31
    "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", # 0x8A32
    "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", # 0x8A33
    "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT",# 0x8A34
    "GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH", # 0x8A35
    "GL_ACTIVE_UNIFORM_BLOCKS",          # 0x8A36
    "GL_UNIFORM_TYPE",                   # 0x8A37
    "GL_UNIFORM_SIZE",                   # 0x8A38
    "GL_UNIFORM_NAME_LENGTH",            # 0x8A39
    "GL_UNIFORM_BLOCK_INDEX",            # 0x8A3A
    "GL_UNIFORM_OFFSET",                 # 0x8A3B
    "GL_UNIFORM_ARRAY_STRIDE",           # 0x8A3C
    "GL_UNIFORM_MATRIX_STRIDE",          # 0x8A3D
    "GL_UNIFORM_IS_ROW_MAJOR",           # 0x8A3E
    "GL_UNIFORM_BLOCK_BINDING",          # 0x8A3F
    "GL_UNIFORM_BLOCK_DATA_SIZE",        # 0x8A40
    "GL_UNIFORM_BLOCK_NAME_LENGTH",      # 0x8A41
    "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS",  # 0x8A42
    "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES", # 0x8A43
    "GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER", # 0x8A44
    "GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER", # 0x8A45
    "GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER", # 0x8A46
    "GL_FRAGMENT_SHADER",                # 0x8B30
    "GL_VERTEX_SHADER",                  # 0x8B31
    "GL_PROGRAM_OBJECT_ARB",             # 0x8B40
    "GL_SHADER_OBJECT_ARB",              # 0x8B48
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",# 0x8B49
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",  # 0x8B4A
    "GL_MAX_VARYING_COMPONENTS",         # 0x8B4B
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", # 0x8B4C
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", # 0x8B4D
    "GL_OBJECT_TYPE_ARB",                # 0x8B4E
    "GL_SHADER_TYPE",                    # 0x8B4F
    "GL_FLOAT_VEC2",                     # 0x8B50
    "GL_FLOAT_VEC3",                     # 0x8B51
    "GL_FLOAT_VEC4",                     # 0x8B52
    "GL_INT_VEC2",                       # 0x8B53
    "GL_INT_VEC3",                       # 0x8B54
    "GL_INT_VEC4",                       # 0x8B55
    "GL_BOOL",                           # 0x8B56
    "GL_BOOL_VEC2",                      # 0x8B57
    "GL_BOOL_VEC3",                      # 0x8B58
    "GL_BOOL_VEC4",                      # 0x8B59
    "GL_FLOAT_MAT2",                     # 0x8B5A
    "GL_FLOAT_MAT3",                     # 0x8B5B
    "GL_FLOAT_MAT4",                     # 0x8B5C
    "GL_SAMPLER_1D",                     # 0x8B5D
    "GL_SAMPLER_2D",                     # 0x8B5E
    "GL_SAMPLER_3D",                     # 0x8B5F
    "GL_SAMPLER_CUBE",                   # 0x8B60
    "GL_SAMPLER_1D_SHADOW",              # 0x8B61
    "GL_SAMPLER_2D_SHADOW",              # 0x8B62
    "GL_SAMPLER_2D_RECT",                # 0x8B63
    "GL_SAMPLER_2D_RECT_SHADOW",         # 0x8B64
    "GL_FLOAT_MAT2x3",                   # 0x8B65
    "GL_FLOAT_MAT2x4",                   # 0x8B66
    "GL_FLOAT_MAT3x2",                   # 0x8B67
    "GL_FLOAT_MAT3x4",                   # 0x8B68
    "GL_FLOAT_MAT4x2",                   # 0x8B69
    "GL_FLOAT_MAT4x3",                   # 0x8B6A
    "GL_DELETE_STATUS",                  # 0x8B80
    "GL_COMPILE_STATUS",                 # 0x8B81
    "GL_LINK_STATUS",                    # 0x8B82
    "GL_VALIDATE_STATUS",                # 0x8B83
    "GL_INFO_LOG_LENGTH",                # 0x8B84
    "GL_ATTACHED_SHADERS",               # 0x8B85
    "GL_ACTIVE_UNIFORMS",                # 0x8B86
    "GL_ACTIVE_UNIFORM_MAX_LENGTH",      # 0x8B87
    "GL_SHADER_SOURCE_LENGTH",           # 0x8B88
    "GL_ACTIVE_ATTRIBUTES",              # 0x8B89
    "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH",    # 0x8B8A
    "GL_FRAGMENT_SHADER_DERIVATIVE_HINT",# 0x8B8B
    "GL_SHADING_LANGUAGE_VERSION",       # 0x8B8C
    "GL_CURRENT_PROGRAM",                # 0x8B8D
    "GL_IMPLEMENTATION_COLOR_READ_TYPE_OES", # 0x8B9A
    "GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES", # 0x8B9B
    "GL_COUNTER_TYPE_AMD",               # 0x8BC0
    "GL_COUNTER_RANGE_AMD",              # 0x8BC1
    "GL_UNSIGNED_INT64_AMD",             # 0x8BC2
    "GL_PERCENTAGE_AMD",                 # 0x8BC3
    "GL_PERFMON_RESULT_AVAILABLE_AMD",   # 0x8BC4
    "GL_PERFMON_RESULT_SIZE_AMD",        # 0x8BC5
    "GL_PERFMON_RESULT_AMD",             # 0x8BC6
    "GL_TEXTURE_RED_TYPE",               # 0x8C10
    "GL_TEXTURE_GREEN_TYPE",             # 0x8C11
    "GL_TEXTURE_BLUE_TYPE",              # 0x8C12
    "GL_TEXTURE_ALPHA_TYPE",             # 0x8C13
    "GL_TEXTURE_LUMINANCE_TYPE",         # 0x8C14
    "GL_TEXTURE_INTENSITY_TYPE",         # 0x8C15
    "GL_TEXTURE_DEPTH_TYPE",             # 0x8C16
    "GL_UNSIGNED_NORMALIZED",            # 0x8C17
    "GL_TEXTURE_1D_ARRAY",               # 0x8C18
    "GL_PROXY_TEXTURE_1D_ARRAY",         # 0x8C19
    "GL_TEXTURE_2D_ARRAY",               # 0x8C1A
    "GL_PROXY_TEXTURE_2D_ARRAY",         # 0x8C1B
    "GL_TEXTURE_BINDING_1D_ARRAY",       # 0x8C1C
    "GL_TEXTURE_BINDING_2D_ARRAY",       # 0x8C1D
    "GL_GEOMETRY_PROGRAM_NV",            # 0x8C26
    "GL_MAX_PROGRAM_OUTPUT_VERTICES_NV", # 0x8C27
    "GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV", # 0x8C28
    "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB", # 0x8C29
    "GL_TEXTURE_BUFFER",                 # 0x8C2A
    "GL_MAX_TEXTURE_BUFFER_SIZE",        # 0x8C2B
    "GL_TEXTURE_BINDING_BUFFER",         # 0x8C2C
    "GL_TEXTURE_BUFFER_DATA_STORE_BINDING", # 0x8C2D
    "GL_TEXTURE_BUFFER_FORMAT",          # 0x8C2E
    "GL_R11F_G11F_B10F",                 # 0x8C3A
    "GL_UNSIGNED_INT_10F_11F_11F_REV",   # 0x8C3B
    "GL_RGBA_SIGNED_COMPONENTS_EXT",     # 0x8C3C
    "GL_RGB9_E5",                        # 0x8C3D
    "GL_UNSIGNED_INT_5_9_9_9_REV",       # 0x8C3E
    "GL_TEXTURE_SHARED_SIZE",            # 0x8C3F
    "GL_SRGB",                           # 0x8C40
    "GL_SRGB8",                          # 0x8C41
    "GL_SRGB_ALPHA",                     # 0x8C42
    "GL_SRGB8_ALPHA8",                   # 0x8C43
    "GL_SLUMINANCE_ALPHA",               # 0x8C44
    "GL_SLUMINANCE8_ALPHA8",             # 0x8C45
    "GL_SLUMINANCE",                     # 0x8C46
    "GL_SLUMINANCE8",                    # 0x8C47
    "GL_COMPRESSED_SRGB",                # 0x8C48
    "GL_COMPRESSED_SRGB_ALPHA",          # 0x8C49
    "GL_COMPRESSED_SLUMINANCE",          # 0x8C4A
    "GL_COMPRESSED_SLUMINANCE_ALPHA",    # 0x8C4B
    "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT",  # 0x8C4C
    "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT", # 0x8C4D
    "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT", # 0x8C4E
    "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT", # 0x8C4F
    "GL_COMPRESSED_LUMINANCE_LATC1_EXT", # 0x8C70
    "GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT", # 0x8C71
    "GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT", # 0x8C72
    "GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT", # 0x8C73
    "GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH", # 0x8C76
    "GL_BACK_PRIMARY_COLOR_NV",          # 0x8C77
    "GL_BACK_SECONDARY_COLOR_NV",        # 0x8C78
    "GL_TEXTURE_COORD_NV",               # 0x8C79
    "GL_CLIP_DISTANCE_NV",               # 0x8C7A
    "GL_VERTEX_ID_NV",                   # 0x8C7B
    "GL_PRIMITIVE_ID_NV",                # 0x8C7C
    "GL_GENERIC_ATTRIB_NV",              # 0x8C7D
    "GL_TRANSFORM_FEEDBACK_ATTRIBS_NV",  # 0x8C7E
    "GL_TRANSFORM_FEEDBACK_BUFFER_MODE", # 0x8C7F
    "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS", # 0x8C80
    "GL_ACTIVE_VARYINGS_NV",             # 0x8C81
    "GL_ACTIVE_VARYING_MAX_LENGTH_NV",   # 0x8C82
    "GL_TRANSFORM_FEEDBACK_VARYINGS",    # 0x8C83
    "GL_TRANSFORM_FEEDBACK_BUFFER_START",# 0x8C84
    "GL_TRANSFORM_FEEDBACK_BUFFER_SIZE", # 0x8C85
    "GL_TRANSFORM_FEEDBACK_RECORD_NV",   # 0x8C86
    "GL_PRIMITIVES_GENERATED",           # 0x8C87
    "GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN", # 0x8C88
    "GL_RASTERIZER_DISCARD",             # 0x8C89
    "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_ATTRIBS_NV", # 0x8C8A
    "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS", # 0x8C8B
    "GL_INTERLEAVED_ATTRIBS",            # 0x8C8C
    "GL_SEPARATE_ATTRIBS",               # 0x8C8D
    "GL_TRANSFORM_FEEDBACK_BUFFER",      # 0x8C8E
    "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING", # 0x8C8F
    "GL_POINT_SPRITE_COORD_ORIGIN",      # 0x8CA0
    "GL_LOWER_LEFT",                     # 0x8CA1
    "GL_UPPER_LEFT",                     # 0x8CA2
    "GL_STENCIL_BACK_REF",               # 0x8CA3
    "GL_STENCIL_BACK_VALUE_MASK",        # 0x8CA4
    "GL_STENCIL_BACK_WRITEMASK",         # 0x8CA5
    "GL_FRAMEBUFFER_BINDING",            # 0x8CA6
    "GL_RENDERBUFFER_BINDING",           # 0x8CA7
    "GL_READ_FRAMEBUFFER",               # 0x8CA8
    "GL_DRAW_FRAMEBUFFER",               # 0x8CA9
    "GL_READ_FRAMEBUFFER_BINDING",       # 0x8CAA
    "GL_RENDERBUFFER_SAMPLES",           # 0x8CAB
    "GL_DEPTH_COMPONENT32F",             # 0x8CAC
    "GL_DEPTH32F_STENCIL8",              # 0x8CAD
    "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE", # 0x8CD0
    "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME", # 0x8CD1
    "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL", # 0x8CD2
    "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE", # 0x8CD3
    "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT", # 0x8CD4
    "GL_FRAMEBUFFER_COMPLETE",           # 0x8CD5
    "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", # 0x8CD6
    "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", # 0x8CD7
    "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT", # 0x8CD9
    "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT", # 0x8CDA
    "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", # 0x8CDB
    "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", # 0x8CDC
    "GL_FRAMEBUFFER_UNSUPPORTED",        # 0x8CDD
    "GL_MAX_COLOR_ATTACHMENTS",          # 0x8CDF
    "GL_COLOR_ATTACHMENT0",              # 0x8CE0
    "GL_COLOR_ATTACHMENT1",              # 0x8CE1
    "GL_COLOR_ATTACHMENT2",              # 0x8CE2
    "GL_COLOR_ATTACHMENT3",              # 0x8CE3
    "GL_COLOR_ATTACHMENT4",              # 0x8CE4
    "GL_COLOR_ATTACHMENT5",              # 0x8CE5
    "GL_COLOR_ATTACHMENT6",              # 0x8CE6
    "GL_COLOR_ATTACHMENT7",              # 0x8CE7
    "GL_COLOR_ATTACHMENT8",              # 0x8CE8
    "GL_COLOR_ATTACHMENT9",              # 0x8CE9
    "GL_COLOR_ATTACHMENT10",             # 0x8CEA
    "GL_COLOR_ATTACHMENT11",             # 0x8CEB
    "GL_COLOR_ATTACHMENT12",             # 0x8CEC
    "GL_COLOR_ATTACHMENT13",             # 0x8CED
    "GL_COLOR_ATTACHMENT14",             # 0x8CEE
    "GL_COLOR_ATTACHMENT15",             # 0x8CEF
    "GL_DEPTH_ATTACHMENT",               # 0x8D00
    "GL_STENCIL_ATTACHMENT",             # 0x8D20
    "GL_FRAMEBUFFER",                    # 0x8D40
    "GL_RENDERBUFFER",                   # 0x8D41
    "GL_RENDERBUFFER_WIDTH",             # 0x8D42
    "GL_RENDERBUFFER_HEIGHT",            # 0x8D43
    "GL_RENDERBUFFER_INTERNAL_FORMAT",   # 0x8D44
    "GL_STENCIL_INDEX1",                 # 0x8D46
    "GL_STENCIL_INDEX4",                 # 0x8D47
    "GL_STENCIL_INDEX8",                 # 0x8D48
    "GL_STENCIL_INDEX16",                # 0x8D49
    "GL_RENDERBUFFER_RED_SIZE",          # 0x8D50
    "GL_RENDERBUFFER_GREEN_SIZE",        # 0x8D51
    "GL_RENDERBUFFER_BLUE_SIZE",         # 0x8D52
    "GL_RENDERBUFFER_ALPHA_SIZE",        # 0x8D53
    "GL_RENDERBUFFER_DEPTH_SIZE",        # 0x8D54
    "GL_RENDERBUFFER_STENCIL_SIZE",      # 0x8D55
    "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", # 0x8D56
    "GL_MAX_SAMPLES_EXT",                # 0x8D57
    "GL_RGBA32UI",                       # 0x8D70
    "GL_RGB32UI",                        # 0x8D71
    "GL_ALPHA32UI_EXT",                  # 0x8D72
    "GL_INTENSITY32UI_EXT",              # 0x8D73
    "GL_LUMINANCE32UI_EXT",              # 0x8D74
    "GL_LUMINANCE_ALPHA32UI_EXT",        # 0x8D75
    "GL_RGBA16UI",                       # 0x8D76
    "GL_RGB16UI",                        # 0x8D77
    "GL_ALPHA16UI_EXT",                  # 0x8D78
    "GL_INTENSITY16UI_EXT",              # 0x8D79
    "GL_LUMINANCE16UI_EXT",              # 0x8D7A
    "GL_LUMINANCE_ALPHA16UI_EXT",        # 0x8D7B
    "GL_RGBA8UI",                        # 0x8D7C
    "GL_RGB8UI",                         # 0x8D7D
    "GL_ALPHA8UI_EXT",                   # 0x8D7E
    "GL_INTENSITY8UI_EXT",               # 0x8D7F
    "GL_LUMINANCE8UI_EXT",               # 0x8D80
    "GL_LUMINANCE_ALPHA8UI_EXT",         # 0x8D81
    "GL_RGBA32I",                        # 0x8D82
    "GL_RGB32I",                         # 0x8D83
    "GL_ALPHA32I_EXT",                   # 0x8D84
    "GL_INTENSITY32I_EXT",               # 0x8D85
    "GL_LUMINANCE32I_EXT",               # 0x8D86
    "GL_LUMINANCE_ALPHA32I_EXT",         # 0x8D87
    "GL_RGBA16I",                        # 0x8D88
    "GL_RGB16I",                         # 0x8D89
    "GL_ALPHA16I_EXT",                   # 0x8D8A
    "GL_INTENSITY16I_EXT",               # 0x8D8B
    "GL_LUMINANCE16I_EXT",               # 0x8D8C
    "GL_LUMINANCE_ALPHA16I_EXT",         # 0x8D8D
    "GL_RGBA8I",                         # 0x8D8E
    "GL_RGB8I",                          # 0x8D8F
    "GL_ALPHA8I_EXT",                    # 0x8D90
    "GL_INTENSITY8I_EXT",                # 0x8D91
    "GL_LUMINANCE8I_EXT",                # 0x8D92
    "GL_LUMINANCE_ALPHA8I_EXT",          # 0x8D93
    "GL_RED_INTEGER",                    # 0x8D94
    "GL_GREEN_INTEGER",                  # 0x8D95
    "GL_BLUE_INTEGER",                   # 0x8D96
    "GL_ALPHA_INTEGER",                  # 0x8D97
    "GL_RGB_INTEGER",                    # 0x8D98
    "GL_RGBA_INTEGER",                   # 0x8D99
    "GL_BGR_INTEGER",                    # 0x8D9A
    "GL_BGRA_INTEGER",                   # 0x8D9B
    "GL_LUMINANCE_INTEGER_EXT",          # 0x8D9C
    "GL_LUMINANCE_ALPHA_INTEGER_EXT",    # 0x8D9D
    "GL_RGBA_INTEGER_MODE_EXT",          # 0x8D9E
    "GL_MAX_PROGRAM_PARAMETER_BUFFER_BINDINGS_NV", # 0x8DA0
    "GL_MAX_PROGRAM_PARAMETER_BUFFER_SIZE_NV", # 0x8DA1
    "GL_VERTEX_PROGRAM_PARAMETER_BUFFER_NV", # 0x8DA2
    "GL_GEOMETRY_PROGRAM_PARAMETER_BUFFER_NV", # 0x8DA3
    "GL_FRAGMENT_PROGRAM_PARAMETER_BUFFER_NV", # 0x8DA4
    "GL_MAX_PROGRAM_GENERIC_ATTRIBS_NV", # 0x8DA5
    "GL_MAX_PROGRAM_GENERIC_RESULTS_NV", # 0x8DA6
    "GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB", # 0x8DA7
    "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB", # 0x8DA8
    "GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB", # 0x8DA9
    "GL_DEPTH_COMPONENT32F_NV",          # 0x8DAB
    "GL_DEPTH32F_STENCIL8_NV",           # 0x8DAC
    "GL_FLOAT_32_UNSIGNED_INT_24_8_REV", # 0x8DAD
    "GL_DEPTH_BUFFER_FLOAT_MODE_NV",     # 0x8DAF
    "GL_FRAMEBUFFER_SRGB",               # 0x8DB9
    "GL_FRAMEBUFFER_SRGB_CAPABLE_EXT",   # 0x8DBA
    "GL_COMPRESSED_RED_RGTC1",           # 0x8DBB
    "GL_COMPRESSED_SIGNED_RED_RGTC1",    # 0x8DBC
    "GL_COMPRESSED_RG_RGTC2",            # 0x8DBD
    "GL_COMPRESSED_SIGNED_RG_RGTC2",     # 0x8DBE
    "GL_SAMPLER_1D_ARRAY",               # 0x8DC0
    "GL_SAMPLER_2D_ARRAY",               # 0x8DC1
    "GL_SAMPLER_BUFFER",                 # 0x8DC2
    "GL_SAMPLER_1D_ARRAY_SHADOW",        # 0x8DC3
    "GL_SAMPLER_2D_ARRAY_SHADOW",        # 0x8DC4
    "GL_SAMPLER_CUBE_SHADOW",            # 0x8DC5
    "GL_UNSIGNED_INT_VEC2",              # 0x8DC6
    "GL_UNSIGNED_INT_VEC3",              # 0x8DC7
    "GL_UNSIGNED_INT_VEC4",              # 0x8DC8
    "GL_INT_SAMPLER_1D",                 # 0x8DC9
    "GL_INT_SAMPLER_2D",                 # 0x8DCA
    "GL_INT_SAMPLER_3D",                 # 0x8DCB
    "GL_INT_SAMPLER_CUBE",               # 0x8DCC
    "GL_INT_SAMPLER_2D_RECT",            # 0x8DCD
    "GL_INT_SAMPLER_1D_ARRAY",           # 0x8DCE
    "GL_INT_SAMPLER_2D_ARRAY",           # 0x8DCF
    "GL_INT_SAMPLER_BUFFER",             # 0x8DD0
    "GL_UNSIGNED_INT_SAMPLER_1D",        # 0x8DD1
    "GL_UNSIGNED_INT_SAMPLER_2D",        # 0x8DD2
    "GL_UNSIGNED_INT_SAMPLER_3D",        # 0x8DD3
    "GL_UNSIGNED_INT_SAMPLER_CUBE",      # 0x8DD4
    "GL_UNSIGNED_INT_SAMPLER_2D_RECT",   # 0x8DD5
    "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY",  # 0x8DD6
    "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY",  # 0x8DD7
    "GL_UNSIGNED_INT_SAMPLER_BUFFER",    # 0x8DD8
    "GL_GEOMETRY_SHADER_ARB",            # 0x8DD9
    "GL_GEOMETRY_VERTICES_OUT_ARB",      # 0x8DDA
    "GL_GEOMETRY_INPUT_TYPE_ARB",        # 0x8DDB
    "GL_GEOMETRY_OUTPUT_TYPE_ARB",       # 0x8DDC
    "GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB", # 0x8DDD
    "GL_MAX_VERTEX_VARYING_COMPONENTS_ARB", # 0x8DDE
    "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB", # 0x8DDF
    "GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB", # 0x8DE0
    "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB", # 0x8DE1
    "GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT", # 0x8DE2
    "GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT", # 0x8DE3
    "GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT", # 0x8DE4
    "GL_MAX_BINDABLE_UNIFORM_SIZE_EXT",  # 0x8DED
    "GL_UNIFORM_BUFFER_EXT",             # 0x8DEE
    "GL_UNIFORM_BUFFER_BINDING_EXT",     # 0x8DEF
    "GL_RENDERBUFFER_COLOR_SAMPLES_NV",  # 0x8E10
    "GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV", # 0x8E11
    "GL_MULTISAMPLE_COVERAGE_MODES_NV",  # 0x8E12
    "GL_QUERY_WAIT",                     # 0x8E13
    "GL_QUERY_NO_WAIT",                  # 0x8E14
    "GL_QUERY_BY_REGION_WAIT",           # 0x8E15
    "GL_QUERY_BY_REGION_NO_WAIT",        # 0x8E16
    "GL_TRANSFORM_FEEDBACK_NV",          # 0x8E22
    "GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED_NV", # 0x8E23
    "GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE_NV", # 0x8E24
    "GL_TRANSFORM_FEEDBACK_BINDING_NV",  # 0x8E25
    "GL_FRAME_NV",                       # 0x8E26
    "GL_FIELDS_NV",                      # 0x8E27
    "GL_CURRENT_TIME_NV",                # 0x8E28
    "GL_NUM_FILL_STREAMS_NV",            # 0x8E29
    "GL_PRESENT_TIME_NV",                # 0x8E2A
    "GL_PRESENT_DURATION_NV",            # 0x8E2B
    "GL_PROGRAM_MATRIX_EXT",             # 0x8E2D
    "GL_TRANSPOSE_PROGRAM_MATRIX_EXT",   # 0x8E2E
    "GL_PROGRAM_MATRIX_STACK_DEPTH_EXT", # 0x8E2F
    "GL_TEXTURE_SWIZZLE_R_EXT",          # 0x8E42
    "GL_TEXTURE_SWIZZLE_G_EXT",          # 0x8E43
    "GL_TEXTURE_SWIZZLE_B_EXT",          # 0x8E44
    "GL_TEXTURE_SWIZZLE_A_EXT",          # 0x8E45
    "GL_TEXTURE_SWIZZLE_RGBA_EXT",       # 0x8E46
    "GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION_EXT", # 0x8E4C
    "GL_FIRST_VERTEX_CONVENTION_EXT",    # 0x8E4D
    "GL_LAST_VERTEX_CONVENTION_EXT",     # 0x8E4E
    "GL_PROVOKING_VERTEX_EXT",           # 0x8E4F
    "GL_SAMPLE_POSITION_NV",             # 0x8E50
    "GL_SAMPLE_MASK_NV",                 # 0x8E51
    "GL_SAMPLE_MASK_VALUE_NV",           # 0x8E52
    "GL_TEXTURE_BINDING_RENDERBUFFER_NV",# 0x8E53
    "GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV", # 0x8E54
    "GL_TEXTURE_RENDERBUFFER_NV",        # 0x8E55
    "GL_SAMPLER_RENDERBUFFER_NV",        # 0x8E56
    "GL_INT_SAMPLER_RENDERBUFFER_NV",    # 0x8E57
    "GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV", # 0x8E58
    "GL_MAX_SAMPLE_MASK_WORDS_NV",       # 0x8E59
    "GL_COPY_READ_BUFFER",               # 0x8F36
    "GL_COPY_WRITE_BUFFER",              # 0x8F37
    "GL_RED_SNORM",                      # 0x8F90
    "GL_RG_SNORM",                       # 0x8F91
    "GL_RGB_SNORM",                      # 0x8F92
    "GL_RGBA_SNORM",                     # 0x8F93
    "GL_R8_SNORM",                       # 0x8F94
    "GL_RG8_SNORM",                      # 0x8F95
    "GL_RGB8_SNORM",                     # 0x8F96
    "GL_RGBA8_SNORM",                    # 0x8F97
    "GL_R16_SNORM",                      # 0x8F98
    "GL_RG16_SNORM",                     # 0x8F99
    "GL_RGB16_SNORM",                    # 0x8F9A
    "GL_RGBA16_SNORM",                   # 0x8F9B
    "GL_SIGNED_NORMALIZED",              # 0x8F9C
    "GL_PRIMITIVE_RESTART",              # 0x8F9D
    "GL_PRIMITIVE_RESTART_INDEX",        # 0x8F9E
    "GL_SAMPLER_BUFFER_AMD",             # 0x9001
    "GL_INT_SAMPLER_BUFFER_AMD",         # 0x9002
    "GL_UNSIGNED_INT_SAMPLER_BUFFER_AMD",# 0x9003
    "GL_TESSELLATION_MODE_AMD",          # 0x9004
    "GL_TESSELLATION_FACTOR_AMD",        # 0x9005
    "GL_DISCRETE_AMD",                   # 0x9006
    "GL_CONTINUOUS_AMD",                 # 0x9007
    "GL_RASTER_POSITION_UNCLIPPED_IBM",  # 0x19262
    "GL_PREFER_DOUBLEBUFFER_HINT_PGI",   # 0x1A1F8
    "GL_CONSERVE_MEMORY_HINT_PGI",       # 0x1A1FD
    "GL_RECLAIM_MEMORY_HINT_PGI",        # 0x1A1FE
    "GL_NATIVE_GRAPHICS_HANDLE_PGI",     # 0x1A202
    "GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI", # 0x1A203
    "GL_NATIVE_GRAPHICS_END_HINT_PGI",   # 0x1A204
    "GL_ALWAYS_FAST_HINT_PGI",           # 0x1A20C
    "GL_ALWAYS_SOFT_HINT_PGI",           # 0x1A20D
    "GL_ALLOW_DRAW_OBJ_HINT_PGI",        # 0x1A20E
    "GL_ALLOW_DRAW_WIN_HINT_PGI",        # 0x1A20F
    "GL_ALLOW_DRAW_FRG_HINT_PGI",        # 0x1A210
    "GL_ALLOW_DRAW_MEM_HINT_PGI",        # 0x1A211
    "GL_STRICT_DEPTHFUNC_HINT_PGI",      # 0x1A216
    "GL_STRICT_LIGHTING_HINT_PGI",       # 0x1A217
    "GL_STRICT_SCISSOR_HINT_PGI",        # 0x1A218
    "GL_FULL_STIPPLE_HINT_PGI",          # 0x1A219
    "GL_CLIP_NEAR_HINT_PGI",             # 0x1A220
    "GL_CLIP_FAR_HINT_PGI",              # 0x1A221
    "GL_WIDE_LINE_HINT_PGI",             # 0x1A222
    "GL_BACK_NORMALS_HINT_PGI",          # 0x1A223
    "GL_VERTEX_DATA_HINT_PGI",           # 0x1A22A
    "GL_VERTEX_CONSISTENT_HINT_PGI",     # 0x1A22B
    "GL_MATERIAL_SIDE_HINT_PGI",         # 0x1A22C
    "GL_MAX_VERTEX_HINT_PGI",            # 0x1A22D
    "GL_CULL_VERTEX_IBM",                # 103050
    "GL_VERTEX_ARRAY_LIST_IBM",          # 103070
    "GL_NORMAL_ARRAY_LIST_IBM",          # 103071
    "GL_COLOR_ARRAY_LIST_IBM",           # 103072
    "GL_INDEX_ARRAY_LIST_IBM",           # 103073
    "GL_TEXTURE_COORD_ARRAY_LIST_IBM",   # 103074
    "GL_EDGE_FLAG_ARRAY_LIST_IBM",       # 103075
    "GL_FOG_COORDINATE_ARRAY_LIST_IBM",  # 103076
    "GL_SECONDARY_COLOR_ARRAY_LIST_IBM", # 103077
    "GL_VERTEX_ARRAY_LIST_STRIDE_IBM",   # 103080
    "GL_NORMAL_ARRAY_LIST_STRIDE_IBM",   # 103081
    "GL_COLOR_ARRAY_LIST_STRIDE_IBM",    # 103082
    "GL_INDEX_ARRAY_LIST_STRIDE_IBM",    # 103083
    "GL_TEXTURE_COORD_ARRAY_LIST_STRIDE_IBM", # 103084
    "GL_EDGE_FLAG_ARRAY_LIST_STRIDE_IBM",# 103085
    "GL_FOG_COORDINATE_ARRAY_LIST_STRIDE_IBM", # 103086
    "GL_SECONDARY_COLOR_ARRAY_LIST_STRIDE_IBM", # 103087
])

GLprimenum = FakeEnum(GLenum, [
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
    "GL_LINES_ADJACENCY_ARB",            # 0x000A
    "GL_LINE_STRIP_ADJACENCY_ARB",       # 0x000B
    "GL_TRIANGLES_ADJACENCY_ARB",        # 0x000C
    "GL_TRIANGLE_STRIP_ADJACENCY_ARB",   # 0x000D
])

GLbufferbitfield = Flags(GLbitfield, [
    "GL_DEPTH_BUFFER_BIT", # 0x00000100
    "GL_ACCUM_BUFFER_BIT", # 0x00000200
    "GL_STENCIL_BUFFER_BIT", # 0x00000400
    "GL_COLOR_BUFFER_BIT", # 0x00004000
])

[
    "GL_ALL_ATTRIB_BITS", # 0x000FFFFF
    "GL_CURRENT_BIT", # 0x00000001
    "GL_POINT_BIT", # 0x00000002
    "GL_LINE_BIT", # 0x00000004
    "GL_POLYGON_BIT", # 0x00000008
    "GL_POLYGON_STIPPLE_BIT", # 0x00000010
    "GL_PIXEL_MODE_BIT", # 0x00000020
    "GL_LIGHTING_BIT", # 0x00000040
    "GL_FOG_BIT", # 0x00000080
    "GL_VIEWPORT_BIT", # 0x00000800
    "GL_TRANSFORM_BIT", # 0x00001000
    "GL_ENABLE_BIT", # 0x00002000
    "GL_HINT_BIT", # 0x00008000
    "GL_EVAL_BIT", # 0x00010000
    "GL_LIST_BIT", # 0x00020000
    "GL_TEXTURE_BIT", # 0x00040000
    "GL_SCISSOR_BIT", # 0x00080000
    "GL_MULTISAMPLE_BIT", # 0x20000000
]

[
    "GL_CLIENT_PIXEL_STORE_BIT", # 0x00000001
    "GL_CLIENT_VERTEX_ARRAY_BIT", # 0x00000002
    "GL_ALL_CLIENT_ATTRIB_BITS", # 0xFFFFFFFF
    "GL_CLIENT_ALL_ATTRIB_BITS", # 0xFFFFFFFF
]


def basic_functions(Function):
    def F(*args, **kwargs):
        kwargs.setdefault('call', 'GLAPIENTRY')
        return Function(*args, **kwargs)
    return [
        F(Void, "glNewList", [(GLuint, "list"), (GLenum, "mode")]),
        F(Void, "glEndList", []),
        F(Void, "glCallList", [(GLuint, "list")]),
        F(Void, "glCallLists", [(GLsizei, "n"), (GLenum, "type"), (Blob(Const(GLvoid), "__gl_calllists_size(n, type)"), "lists")]),
        F(Void, "glDeleteLists", [(GLuint, "list"), (GLsizei, "range")]),
        F(GLuint, "glGenLists", [(GLsizei, "range")]),
        F(Void, "glListBase", [(GLuint, "base")]),
        F(Void, "glBegin", [(GLprimenum, "mode")]),
        F(Void, "glBitmap", [(GLsizei, "width"), (GLsizei, "height"), (GLfloat, "xorig"), (GLfloat, "yorig"), (GLfloat, "xmove"), (GLfloat, "ymove"), (Blob(Const(GLubyte), "__gl_bitmap_size(width, height)"), "bitmap")]),
        F(Void, "glColor3b", [(GLbyte, "red"), (GLbyte, "green"), (GLbyte, "blue")]),
        F(Void, "glColor3bv", [(Array(Const(GLbyte), "3"), "v")]),
        F(Void, "glColor3d", [(GLdouble, "red"), (GLdouble, "green"), (GLdouble, "blue")]),
        F(Void, "glColor3dv", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glColor3f", [(GLfloat, "red"), (GLfloat, "green"), (GLfloat, "blue")]),
        F(Void, "glColor3fv", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glColor3i", [(GLint, "red"), (GLint, "green"), (GLint, "blue")]),
        F(Void, "glColor3iv", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glColor3s", [(GLshort, "red"), (GLshort, "green"), (GLshort, "blue")]),
        F(Void, "glColor3sv", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glColor3ub", [(GLubyte, "red"), (GLubyte, "green"), (GLubyte, "blue")]),
        F(Void, "glColor3ubv", [(Array(Const(GLubyte), "3"), "v")]),
        F(Void, "glColor3ui", [(GLuint, "red"), (GLuint, "green"), (GLuint, "blue")]),
        F(Void, "glColor3uiv", [(Array(Const(GLuint), "3"), "v")]),
        F(Void, "glColor3us", [(GLushort, "red"), (GLushort, "green"), (GLushort, "blue")]),
        F(Void, "glColor3usv", [(Array(Const(GLushort), "3"), "v")]),
        F(Void, "glColor4b", [(GLbyte, "red"), (GLbyte, "green"), (GLbyte, "blue"), (GLbyte, "alpha")]),
        F(Void, "glColor4bv", [(Array(Const(GLbyte), "4"), "v")]),
        F(Void, "glColor4d", [(GLdouble, "red"), (GLdouble, "green"), (GLdouble, "blue"), (GLdouble, "alpha")]),
        F(Void, "glColor4dv", [(Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glColor4f", [(GLfloat, "red"), (GLfloat, "green"), (GLfloat, "blue"), (GLfloat, "alpha")]),
        F(Void, "glColor4fv", [(Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glColor4i", [(GLint, "red"), (GLint, "green"), (GLint, "blue"), (GLint, "alpha")]),
        F(Void, "glColor4iv", [(Array(Const(GLint), "4"), "v")]),
        F(Void, "glColor4s", [(GLshort, "red"), (GLshort, "green"), (GLshort, "blue"), (GLshort, "alpha")]),
        F(Void, "glColor4sv", [(Array(Const(GLshort), "4"), "v")]),
        F(Void, "glColor4ub", [(GLubyte, "red"), (GLubyte, "green"), (GLubyte, "blue"), (GLubyte, "alpha")]),
        F(Void, "glColor4ubv", [(Array(Const(GLubyte), "4"), "v")]),
        F(Void, "glColor4ui", [(GLuint, "red"), (GLuint, "green"), (GLuint, "blue"), (GLuint, "alpha")]),
        F(Void, "glColor4uiv", [(Array(Const(GLuint), "4"), "v")]),
        F(Void, "glColor4us", [(GLushort, "red"), (GLushort, "green"), (GLushort, "blue"), (GLushort, "alpha")]),
        F(Void, "glColor4usv", [(Array(Const(GLushort), "4"), "v")]),
        F(Void, "glEdgeFlag", [(GLboolean, "flag")]),
        F(Void, "glEdgeFlagv", [(Array(Const(GLboolean), "1"), "flag")]),
        F(Void, "glEnd", []),
        F(Void, "glIndexd", [(GLdouble, "c")]),
        F(Void, "glIndexdv", [(Array(Const(GLdouble), "1"), "c")]),
        F(Void, "glIndexf", [(GLfloat, "c")]),
        F(Void, "glIndexfv", [(Array(Const(GLfloat), "1"), "c")]),
        F(Void, "glIndexi", [(GLint, "c")]),
        F(Void, "glIndexiv", [(Array(Const(GLint), "1"), "c")]),
        F(Void, "glIndexs", [(GLshort, "c")]),
        F(Void, "glIndexsv", [(Array(Const(GLshort), "1"), "c")]),
        F(Void, "glNormal3b", [(GLbyte, "nx"), (GLbyte, "ny"), (GLbyte, "nz")]),
        F(Void, "glNormal3bv", [(Array(Const(GLbyte), "3"), "v")]),
        F(Void, "glNormal3d", [(GLdouble, "nx"), (GLdouble, "ny"), (GLdouble, "nz")]),
        F(Void, "glNormal3dv", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glNormal3f", [(GLfloat, "nx"), (GLfloat, "ny"), (GLfloat, "nz")]),
        F(Void, "glNormal3fv", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glNormal3i", [(GLint, "nx"), (GLint, "ny"), (GLint, "nz")]),
        F(Void, "glNormal3iv", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glNormal3s", [(GLshort, "nx"), (GLshort, "ny"), (GLshort, "nz")]),
        F(Void, "glNormal3sv", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glRasterPos2d", [(GLdouble, "x"), (GLdouble, "y")]),
        F(Void, "glRasterPos2dv", [(Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glRasterPos2f", [(GLfloat, "x"), (GLfloat, "y")]),
        F(Void, "glRasterPos2fv", [(Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glRasterPos2i", [(GLint, "x"), (GLint, "y")]),
        F(Void, "glRasterPos2iv", [(Array(Const(GLint), "2"), "v")]),
        F(Void, "glRasterPos2s", [(GLshort, "x"), (GLshort, "y")]),
        F(Void, "glRasterPos2sv", [(Array(Const(GLshort), "2"), "v")]),
        F(Void, "glRasterPos3d", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glRasterPos3dv", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glRasterPos3f", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glRasterPos3fv", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glRasterPos3i", [(GLint, "x"), (GLint, "y"), (GLint, "z")]),
        F(Void, "glRasterPos3iv", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glRasterPos3s", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z")]),
        F(Void, "glRasterPos3sv", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glRasterPos4d", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glRasterPos4dv", [(Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glRasterPos4f", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glRasterPos4fv", [(Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glRasterPos4i", [(GLint, "x"), (GLint, "y"), (GLint, "z"), (GLint, "w")]),
        F(Void, "glRasterPos4iv", [(Array(Const(GLint), "4"), "v")]),
        F(Void, "glRasterPos4s", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z"), (GLshort, "w")]),
        F(Void, "glRasterPos4sv", [(Array(Const(GLshort), "4"), "v")]),
        F(Void, "glRectd", [(GLdouble, "x1"), (GLdouble, "y1"), (GLdouble, "x2"), (GLdouble, "y2")]),
        F(Void, "glRectdv", [(Array(Const(GLdouble), "2"), "v1"), (Array(Const(GLdouble), "2"), "v2")]),
        F(Void, "glRectf", [(GLfloat, "x1"), (GLfloat, "y1"), (GLfloat, "x2"), (GLfloat, "y2")]),
        F(Void, "glRectfv", [(Array(Const(GLfloat), "2"), "v1"), (Array(Const(GLfloat), "2"), "v2")]),
        F(Void, "glRecti", [(GLint, "x1"), (GLint, "y1"), (GLint, "x2"), (GLint, "y2")]),
        F(Void, "glRectiv", [(Array(Const(GLint), "2"), "v1"), (Array(Const(GLint), "2"), "v2")]),
        F(Void, "glRects", [(GLshort, "x1"), (GLshort, "y1"), (GLshort, "x2"), (GLshort, "y2")]),
        F(Void, "glRectsv", [(Array(Const(GLshort), "2"), "v1"), (Array(Const(GLshort), "2"), "v2")]),
        F(Void, "glTexCoord1d", [(GLdouble, "s")]),
        F(Void, "glTexCoord1dv", [(Array(Const(GLdouble), "1"), "v")]),
        F(Void, "glTexCoord1f", [(GLfloat, "s")]),
        F(Void, "glTexCoord1fv", [(Array(Const(GLfloat), "1"), "v")]),
        F(Void, "glTexCoord1i", [(GLint, "s")]),
        F(Void, "glTexCoord1iv", [(Array(Const(GLint), "1"), "v")]),
        F(Void, "glTexCoord1s", [(GLshort, "s")]),
        F(Void, "glTexCoord1sv", [(Array(Const(GLshort), "1"), "v")]),
        F(Void, "glTexCoord2d", [(GLdouble, "s"), (GLdouble, "t")]),
        F(Void, "glTexCoord2dv", [(Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glTexCoord2f", [(GLfloat, "s"), (GLfloat, "t")]),
        F(Void, "glTexCoord2fv", [(Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glTexCoord2i", [(GLint, "s"), (GLint, "t")]),
        F(Void, "glTexCoord2iv", [(Array(Const(GLint), "2"), "v")]),
        F(Void, "glTexCoord2s", [(GLshort, "s"), (GLshort, "t")]),
        F(Void, "glTexCoord2sv", [(Array(Const(GLshort), "2"), "v")]),
        F(Void, "glTexCoord3d", [(GLdouble, "s"), (GLdouble, "t"), (GLdouble, "r")]),
        F(Void, "glTexCoord3dv", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glTexCoord3f", [(GLfloat, "s"), (GLfloat, "t"), (GLfloat, "r")]),
        F(Void, "glTexCoord3fv", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glTexCoord3i", [(GLint, "s"), (GLint, "t"), (GLint, "r")]),
        F(Void, "glTexCoord3iv", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glTexCoord3s", [(GLshort, "s"), (GLshort, "t"), (GLshort, "r")]),
        F(Void, "glTexCoord3sv", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glTexCoord4d", [(GLdouble, "s"), (GLdouble, "t"), (GLdouble, "r"), (GLdouble, "q")]),
        F(Void, "glTexCoord4dv", [(Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glTexCoord4f", [(GLfloat, "s"), (GLfloat, "t"), (GLfloat, "r"), (GLfloat, "q")]),
        F(Void, "glTexCoord4fv", [(Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glTexCoord4i", [(GLint, "s"), (GLint, "t"), (GLint, "r"), (GLint, "q")]),
        F(Void, "glTexCoord4iv", [(Array(Const(GLint), "4"), "v")]),
        F(Void, "glTexCoord4s", [(GLshort, "s"), (GLshort, "t"), (GLshort, "r"), (GLshort, "q")]),
        F(Void, "glTexCoord4sv", [(Array(Const(GLshort), "4"), "v")]),
        F(Void, "glVertex2d", [(GLdouble, "x"), (GLdouble, "y")]),
        F(Void, "glVertex2dv", [(Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glVertex2f", [(GLfloat, "x"), (GLfloat, "y")]),
        F(Void, "glVertex2fv", [(Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glVertex2i", [(GLint, "x"), (GLint, "y")]),
        F(Void, "glVertex2iv", [(Array(Const(GLint), "2"), "v")]),
        F(Void, "glVertex2s", [(GLshort, "x"), (GLshort, "y")]),
        F(Void, "glVertex2sv", [(Array(Const(GLshort), "2"), "v")]),
        F(Void, "glVertex3d", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glVertex3dv", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glVertex3f", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glVertex3fv", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glVertex3i", [(GLint, "x"), (GLint, "y"), (GLint, "z")]),
        F(Void, "glVertex3iv", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glVertex3s", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z")]),
        F(Void, "glVertex3sv", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glVertex4d", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glVertex4dv", [(Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glVertex4f", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glVertex4fv", [(Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glVertex4i", [(GLint, "x"), (GLint, "y"), (GLint, "z"), (GLint, "w")]),
        F(Void, "glVertex4iv", [(Array(Const(GLint), "4"), "v")]),
        F(Void, "glVertex4s", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z"), (GLshort, "w")]),
        F(Void, "glVertex4sv", [(Array(Const(GLshort), "4"), "v")]),
        F(Void, "glClipPlane", [(GLenum, "plane"), (Array(Const(GLdouble), "4"), "equation")]),
        F(Void, "glColorMaterial", [(GLenum, "face"), (GLenum, "mode")]),
        F(Void, "glCullFace", [(GLenum, "mode")]),
        F(Void, "glFogf", [(GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glFogfv", [(GLenum, "pname"), (Array(Const(GLfloat), "(pname == GL_FOG_COLOR ? 4 : 1)"), "params")]),
        F(Void, "glFogi", [(GLenum, "pname"), (GLint, "param")]),
        F(Void, "glFogiv", [(GLenum, "pname"), (Array(Const(GLint), "(pname == GL_FOG_COLOR ? 4 : 1)"), "params")]),
        F(Void, "glFrontFace", [(GLenum, "mode")]),
        F(Void, "glHint", [(GLenum, "target"), (GLenum, "mode")]),
        F(Void, "glLightf", [(GLenum, "light"), (GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glLightfv", [(GLenum, "light"), (GLenum, "pname"), (Array(Const(GLfloat), "4"), "params")]), # XXX
        F(Void, "glLighti", [(GLenum, "light"), (GLenum, "pname"), (GLint, "param")]),
        F(Void, "glLightiv", [(GLenum, "light"), (GLenum, "pname"), (Array(Const(GLint), "4"), "params")]), # XXX
        F(Void, "glLightModelf", [(GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glLightModelfv", [(GLenum, "pname"), (Array(Const(GLfloat), "4"), "params")]), # XXX
        F(Void, "glLightModeli", [(GLenum, "pname"), (GLint, "param")]),
        F(Void, "glLightModeliv", [(GLenum, "pname"), (Array(Const(GLint), "4"), "params")]), # XXX
        F(Void, "glLineStipple", [(GLint, "factor"), (GLushort, "pattern")]),
        F(Void, "glLineWidth", [(GLfloat, "width")]),
        F(Void, "glMaterialf", [(GLenum, "face"), (GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glMaterialfv", [(GLenum, "face"), (GLenum, "pname"), (Array(Const(GLfloat), "4"), "params")]), # XXX
        F(Void, "glMateriali", [(GLenum, "face"), (GLenum, "pname"), (GLint, "param")]),
        F(Void, "glMaterialiv", [(GLenum, "face"), (GLenum, "pname"), (Array(Const(GLint), "4"), "params")]), # XXX
        F(Void, "glPointSize", [(GLfloat, "size")]),
        F(Void, "glPolygonMode", [(GLenum, "face"), (GLenum, "mode")]),
        F(Void, "glPolygonStipple", [(Blob(Const(GLubyte), "__gl_bitmap_size(32, 32)"), "mask")]),
        F(Void, "glScissor", [(GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glShadeModel", [(GLenum, "mode")]),
        F(Void, "glTexParameterf", [(GLenum, "target"), (GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glTexParameterfv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLfloat), "(pname == GL_TEXTURE_BORDER_COLOR ? 4 : 1)"), "params")]),
        F(Void, "glTexParameteri", [(GLenum, "target"), (GLenum, "pname"), (GLint, "param")]),
        F(Void, "glTexParameteriv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLint), "(pname == GL_TEXTURE_BORDER_COLOR ? 4 : 1)"), "params")]),
        F(Void, "glTexImage1D", [(GLenum, "target"), (GLint, "level"), (GLint, "internalformat"), (GLsizei, "width"), (GLint, "border"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "__gl_image_size(format, type, width, 1, 1, border)"), "pixels")]),
        F(Void, "glTexImage2D", [(GLenum, "target"), (GLint, "level"), (GLint, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLint, "border"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "__gl_image_size(format, type, width, height, 1, border)"), "pixels")]),
        F(Void, "glTexEnvf", [(GLenum, "target"), (GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glTexEnvfv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLfloat), "(pname == GL_TEXTURE_ENV_COLOR ? 4 : 1)"), "params")]),
        F(Void, "glTexEnvi", [(GLenum, "target"), (GLenum, "pname"), (GLint, "param")]),
        F(Void, "glTexEnviv", [(GLenum, "target"), (GLenum, "pname"), (Array(Const(GLint), "(pname == GL_TEXTURE_ENV_COLOR ? 4 : 1)"), "params")]),
        F(Void, "glTexGend", [(GLenum, "coord"), (GLenum, "pname"), (GLdouble, "param")]),
        F(Void, "glTexGendv", [(GLenum, "coord"), (GLenum, "pname"), (Pointer(Const(GLdouble)), "params")]),
        F(Void, "glTexGenf", [(GLenum, "coord"), (GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glTexGenfv", [(GLenum, "coord"), (GLenum, "pname"), (Pointer(Const(GLfloat)), "params")]),
        F(Void, "glTexGeni", [(GLenum, "coord"), (GLenum, "pname"), (GLint, "param")]),
        F(Void, "glTexGeniv", [(GLenum, "coord"), (GLenum, "pname"), (Pointer(Const(GLint)), "params")]),
        F(Void, "glFeedbackBuffer", [(GLsizei, "size"), (GLenum, "type"), Out(Array(GLfloat, "size"), "buffer")]),
        F(Void, "glSelectBuffer", [(GLsizei, "size"), Out(Array(GLuint, "size"), "buffer")]),
        F(GLint, "glRenderMode", [(GLenum, "mode")]),
        F(Void, "glInitNames", []),
        F(Void, "glLoadName", [(GLuint, "name")]),
        F(Void, "glPassThrough", [(GLfloat, "token")]),
        F(Void, "glPopName", []),
        F(Void, "glPushName", [(GLuint, "name")]),
        F(Void, "glDrawBuffer", [(GLenum, "mode")]),
        F(Void, "glClear", [(GLbufferbitfield, "mask")]),
        F(Void, "glClearAccum", [(GLfloat, "red"), (GLfloat, "green"), (GLfloat, "blue"), (GLfloat, "alpha")]),
        F(Void, "glClearIndex", [(GLfloat, "c")]),
        F(Void, "glClearColor", [(GLclampf, "red"), (GLclampf, "green"), (GLclampf, "blue"), (GLclampf, "alpha")]),
        F(Void, "glClearStencil", [(GLint, "s")]),
        F(Void, "glClearDepth", [(GLclampd, "depth")]),
        F(Void, "glStencilMask", [(GLuint, "mask")]),
        F(Void, "glColorMask", [(GLboolean, "red"), (GLboolean, "green"), (GLboolean, "blue"), (GLboolean, "alpha")]),
        F(Void, "glDepthMask", [(GLboolean, "flag")]),
        F(Void, "glIndexMask", [(GLuint, "mask")]),
        F(Void, "glAccum", [(GLenum, "op"), (GLfloat, "value")]),
        F(Void, "glDisable", [(GLenum, "cap")]),
        F(Void, "glEnable", [(GLenum, "cap")]),
        F(Void, "glFinish", []),
        F(Void, "glFlush", []),
        F(Void, "glPopAttrib", []),
        F(Void, "glPushAttrib", [(GLbitfield, "mask")]),
        F(Void, "glMap1d", [(GLenum, "target"), (GLdouble, "u1"), (GLdouble, "u2"), (GLint, "stride"), (GLint, "order"), (OpaquePointer(Const(GLdouble)), "points")]),
        F(Void, "glMap1f", [(GLenum, "target"), (GLfloat, "u1"), (GLfloat, "u2"), (GLint, "stride"), (GLint, "order"), (OpaquePointer(Const(GLfloat)), "points")]),
        F(Void, "glMap2d", [(GLenum, "target"), (GLdouble, "u1"), (GLdouble, "u2"), (GLint, "ustride"), (GLint, "uorder"), (GLdouble, "v1"), (GLdouble, "v2"), (GLint, "vstride"), (GLint, "vorder"), (OpaquePointer(Const(GLdouble)), "points")]),
        F(Void, "glMap2f", [(GLenum, "target"), (GLfloat, "u1"), (GLfloat, "u2"), (GLint, "ustride"), (GLint, "uorder"), (GLfloat, "v1"), (GLfloat, "v2"), (GLint, "vstride"), (GLint, "vorder"), (OpaquePointer(Const(GLfloat)), "points")]),
        F(Void, "glMapGrid1d", [(GLint, "un"), (GLdouble, "u1"), (GLdouble, "u2")]),
        F(Void, "glMapGrid1f", [(GLint, "un"), (GLfloat, "u1"), (GLfloat, "u2")]),
        F(Void, "glMapGrid2d", [(GLint, "un"), (GLdouble, "u1"), (GLdouble, "u2"), (GLint, "vn"), (GLdouble, "v1"), (GLdouble, "v2")]),
        F(Void, "glMapGrid2f", [(GLint, "un"), (GLfloat, "u1"), (GLfloat, "u2"), (GLint, "vn"), (GLfloat, "v1"), (GLfloat, "v2")]),
        F(Void, "glEvalCoord1d", [(GLdouble, "u")]),
        F(Void, "glEvalCoord1dv", [(Array(Const(GLdouble), "1"), "u")]),
        F(Void, "glEvalCoord1f", [(GLfloat, "u")]),
        F(Void, "glEvalCoord1fv", [(Array(Const(GLfloat), "1"), "u")]),
        F(Void, "glEvalCoord2d", [(GLdouble, "u"), (GLdouble, "v")]),
        F(Void, "glEvalCoord2dv", [(Array(Const(GLdouble), "2"), "u")]),
        F(Void, "glEvalCoord2f", [(GLfloat, "u"), (GLfloat, "v")]),
        F(Void, "glEvalCoord2fv", [(Array(Const(GLfloat), "2"), "u")]),
        F(Void, "glEvalMesh1", [(GLenum, "mode"), (GLint, "i1"), (GLint, "i2")]),
        F(Void, "glEvalPoint1", [(GLint, "i")]),
        F(Void, "glEvalMesh2", [(GLenum, "mode"), (GLint, "i1"), (GLint, "i2"), (GLint, "j1"), (GLint, "j2")]),
        F(Void, "glEvalPoint2", [(GLint, "i"), (GLint, "j")]),
        F(Void, "glAlphaFunc", [(GLenum, "func"), (GLclampf, "ref")]),
        F(Void, "glBlendFunc", [(GLenum, "sfactor"), (GLenum, "dfactor")]),
        F(Void, "glLogicOp", [(GLenum, "opcode")]),
        F(Void, "glStencilFunc", [(GLenum, "func"), (GLint, "ref"), (GLuint, "mask")]),
        F(Void, "glStencilOp", [(GLenum, "fail"), (GLenum, "zfail"), (GLenum, "zpass")]),
        F(Void, "glDepthFunc", [(GLenum, "func")]),
        F(Void, "glPixelZoom", [(GLfloat, "xfactor"), (GLfloat, "yfactor")]),
        F(Void, "glPixelTransferf", [(GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glPixelTransferi", [(GLenum, "pname"), (GLint, "param")]),
        F(Void, "glPixelStoref", [(GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glPixelStorei", [(GLenum, "pname"), (GLint, "param")]),
        F(Void, "glPixelMapfv", [(GLenum, "map"), (GLsizei, "mapsize"), (Array(Const(GLfloat), "mapsize"), "values")]),
        F(Void, "glPixelMapuiv", [(GLenum, "map"), (GLsizei, "mapsize"), (Array(Const(GLuint), "mapsize"), "values")]),
        F(Void, "glPixelMapusv", [(GLenum, "map"), (GLsizei, "mapsize"), (Array(Const(GLushort), "mapsize"), "values")]),
        F(Void, "glReadBuffer", [(GLenum, "mode")]),
        F(Void, "glCopyPixels", [(GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "type")]),
        F(Void, "glReadPixels", [(GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), Out(Pointer(GLvoid), "pixels")]),
        F(Void, "glDrawPixels", [(GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "pixels")]),
        F(Void, "glGetBooleanv", [(GLenum, "pname"), Out(Pointer(GLboolean), "params")], sideeffects=False),
        F(Void, "glGetClipPlane", [(GLenum, "plane"), Out(Array(GLdouble, "4"), "equation")], sideeffects=False),
        F(Void, "glGetDoublev", [(GLenum, "pname"), Out(Pointer(GLdouble), "params")], sideeffects=False),
        F(GLenum, "glGetError", [], sideeffects=False),
        F(Void, "glGetFloatv", [(GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetIntegerv", [(GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetLightfv", [(GLenum, "light"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetLightiv", [(GLenum, "light"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetMapdv", [(GLenum, "target"), (GLenum, "query"), Out(Pointer(GLdouble), "v")], sideeffects=False),
        F(Void, "glGetMapfv", [(GLenum, "target"), (GLenum, "query"), Out(Pointer(GLfloat), "v")], sideeffects=False),
        F(Void, "glGetMapiv", [(GLenum, "target"), (GLenum, "query"), Out(Pointer(GLint), "v")], sideeffects=False),
        F(Void, "glGetMaterialfv", [(GLenum, "face"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetMaterialiv", [(GLenum, "face"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetPixelMapfv", [(GLenum, "map"), Out(Pointer(GLfloat), "values")], sideeffects=False),
        F(Void, "glGetPixelMapuiv", [(GLenum, "map"), Out(Pointer(GLuint), "values")], sideeffects=False),
        F(Void, "glGetPixelMapusv", [(GLenum, "map"), Out(Pointer(GLushort), "values")], sideeffects=False),
        F(Void, "glGetPolygonStipple", [Out(Pointer(GLubyte), "mask")], sideeffects=False),
        F(Alias("const GLubyte *", String), "glGetString", [(GLenum, "name")], sideeffects=False),
        F(Void, "glGetTexEnvfv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetTexEnviv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetTexGendv", [(GLenum, "coord"), (GLenum, "pname"), Out(Pointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetTexGenfv", [(GLenum, "coord"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetTexGeniv", [(GLenum, "coord"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetTexImage", [(GLenum, "target"), (GLint, "level"), (GLenum, "format"), (GLenum, "type"), Out(Pointer(GLvoid), "pixels")], sideeffects=False),
        F(Void, "glGetTexParameterfv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetTexParameteriv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetTexLevelParameterfv", [(GLenum, "target"), (GLint, "level"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetTexLevelParameteriv", [(GLenum, "target"), (GLint, "level"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(GLboolean, "glIsEnabled", [(GLenum, "cap")]),
        F(GLboolean, "glIsList", [(GLuint, "list")]),
        F(Void, "glDepthRange", [(GLclampd, "zNear"), (GLclampd, "zFar")]),
        F(Void, "glFrustum", [(GLdouble, "left"), (GLdouble, "right"), (GLdouble, "bottom"), (GLdouble, "top"), (GLdouble, "zNear"), (GLdouble, "zFar")]),
        F(Void, "glLoadIdentity", []),
        F(Void, "glLoadMatrixf", [(Array(Const(GLfloat), "16"), "m")]),
        F(Void, "glLoadMatrixd", [(Array(Const(GLdouble), "16"), "m")]),
        F(Void, "glMatrixMode", [(GLenum, "mode")]),
        F(Void, "glMultMatrixf", [(Array(Const(GLfloat), "16"), "m")]),
        F(Void, "glMultMatrixd", [(Array(Const(GLdouble), "16"), "m")]),
        F(Void, "glOrtho", [(GLdouble, "left"), (GLdouble, "right"), (GLdouble, "bottom"), (GLdouble, "top"), (GLdouble, "zNear"), (GLdouble, "zFar")]),
        F(Void, "glPopMatrix", []),
        F(Void, "glPushMatrix", []),
        F(Void, "glRotated", [(GLdouble, "angle"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glRotatef", [(GLfloat, "angle"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glScaled", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glScalef", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glTranslated", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glTranslatef", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glViewport", [(GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glArrayElement", [(GLint, "i")]),
        F(Void, "glBindTexture", [(GLenum, "target"), (GLuint, "texture")]),
        F(Void, "glColorPointer", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glDisableClientState", [(GLenum, "array")]),
        F(Void, "glDrawArrays", [(GLenum, "mode"), (GLint, "first"), (GLsizei, "count")]),
        F(Void, "glDrawElements", [(GLenum, "mode"), (GLsizei, "count"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "indices")]),
        F(Void, "glEdgeFlagPointer", [(GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glEnableClientState", [(GLenum, "array")]),
        F(Void, "glIndexPointer", [(GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glIndexub", [(GLubyte, "c")]),
        F(Void, "glIndexubv", [(Array(Const(GLubyte), "1"), "c")]),
        F(Void, "glInterleavedArrays", [(GLenum, "format"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glNormalPointer", [(GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glPolygonOffset", [(GLfloat, "factor"), (GLfloat, "units")]),
        F(Void, "glTexCoordPointer", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glVertexPointer", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(GLboolean, "glAreTexturesResident", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "textures"), Out(Pointer(GLboolean), "residences")]),
        F(Void, "glCopyTexImage1D", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLint, "border")]),
        F(Void, "glCopyTexImage2D", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height"), (GLint, "border")]),
        F(Void, "glCopyTexSubImage1D", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "x"), (GLint, "y"), (GLsizei, "width")]),
        F(Void, "glCopyTexSubImage2D", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glDeleteTextures", [(GLsizei, "n"), (Array(Const(GLuint), "n"), "textures")]),
        F(Void, "glGenTextures", [(GLsizei, "n"), Out(Array(GLuint, "n"), "textures")]),
        F(Void, "glGetPointerv", [(GLenum, "pname"), Out(Pointer(OpaquePointer(GLvoid)), "params")], sideeffects=False),
        F(GLboolean, "glIsTexture", [(GLuint, "texture")]),
        F(Void, "glPrioritizeTextures", [(GLsizei, "n"), (Array(Const(GLuint), "n"), "textures"), (Array(Const(GLclampf), "n"), "priorities")]),
        F(Void, "glTexSubImage1D", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLsizei, "width"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "__gl_image_size(format, type, width, 1, 1, 0)"), "pixels")]),
        F(Void, "glTexSubImage2D", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "__gl_image_size(format, type, width, height, 1, 0)"), "pixels")]),
        F(Void, "glPopClientAttrib", []),
        F(Void, "glPushClientAttrib", [(GLbitfield, "mask")]),
        F(Void, "glBlendColor", [(GLclampf, "red"), (GLclampf, "green"), (GLclampf, "blue"), (GLclampf, "alpha")]),
        F(Void, "glBlendEquation", [(GLenum, "mode")]),
        F(Void, "glDrawRangeElements", [(GLenum, "mode"), (GLuint, "start"), (GLuint, "end"), (GLsizei, "count"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "indices")]),
        F(Void, "glColorTable", [(GLenum, "target"), (GLenum, "internalformat"), (GLsizei, "width"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "table")]),
        F(Void, "glColorTableParameterfv", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glColorTableParameteriv", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(Const(GLint)), "params")]),
        F(Void, "glCopyColorTable", [(GLenum, "target"), (GLenum, "internalformat"), (GLint, "x"), (GLint, "y"), (GLsizei, "width")]),
        F(Void, "glGetColorTable", [(GLenum, "target"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(GLvoid), "table")], sideeffects=False),
        F(Void, "glGetColorTableParameterfv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetColorTableParameteriv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glColorSubTable", [(GLenum, "target"), (GLsizei, "start"), (GLsizei, "count"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glCopyColorSubTable", [(GLenum, "target"), (GLsizei, "start"), (GLint, "x"), (GLint, "y"), (GLsizei, "width")]),
        F(Void, "glConvolutionFilter1D", [(GLenum, "target"), (GLenum, "internalformat"), (GLsizei, "width"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "image")]),
        F(Void, "glConvolutionFilter2D", [(GLenum, "target"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "image")]),
        F(Void, "glConvolutionParameterf", [(GLenum, "target"), (GLenum, "pname"), (GLfloat, "params")]),
        F(Void, "glConvolutionParameterfv", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glConvolutionParameteri", [(GLenum, "target"), (GLenum, "pname"), (GLint, "params")]),
        F(Void, "glConvolutionParameteriv", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(Const(GLint)), "params")]),
        F(Void, "glCopyConvolutionFilter1D", [(GLenum, "target"), (GLenum, "internalformat"), (GLint, "x"), (GLint, "y"), (GLsizei, "width")]),
        F(Void, "glCopyConvolutionFilter2D", [(GLenum, "target"), (GLenum, "internalformat"), (GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glGetConvolutionFilter", [(GLenum, "target"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(GLvoid), "image")], sideeffects=False),
        F(Void, "glGetConvolutionParameterfv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetConvolutionParameteriv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetSeparableFilter", [(GLenum, "target"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(GLvoid), "row"), (OpaquePointer(GLvoid), "column"), (OpaquePointer(GLvoid), "span")], sideeffects=False),
        F(Void, "glSeparableFilter2D", [(GLenum, "target"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "row"), (OpaquePointer(Const(GLvoid)), "column")]),
        F(Void, "glGetHistogram", [(GLenum, "target"), (GLboolean, "reset"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(GLvoid), "values")], sideeffects=False),
        F(Void, "glGetHistogramParameterfv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetHistogramParameteriv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetMinmax", [(GLenum, "target"), (GLboolean, "reset"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(GLvoid), "values")], sideeffects=False),
        F(Void, "glGetMinmaxParameterfv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetMinmaxParameteriv", [(GLenum, "target"), (GLenum, "pname"), Out(Pointer(GLint), "params")], sideeffects=False),
        F(Void, "glHistogram", [(GLenum, "target"), (GLsizei, "width"), (GLenum, "internalformat"), (GLboolean, "sink")]),
        F(Void, "glMinmax", [(GLenum, "target"), (GLenum, "internalformat"), (GLboolean, "sink")]),
        F(Void, "glResetHistogram", [(GLenum, "target")]),
        F(Void, "glResetMinmax", [(GLenum, "target")]),
        F(Void, "glTexImage3D", [(GLenum, "target"), (GLint, "level"), (GLint, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLint, "border"), (GLenum, "format"), (GLenum, "type"), (Blob(Const(GLvoid), "__gl_image_size(format, type, width, height, depth, border)"), "pixels")]),
        F(Void, "glTexSubImage3D", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "zoffset"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLenum, "format"), (GLenum, "type"), (OpaquePointer(Const(GLvoid)), "pixels")]),
        F(Void, "glCopyTexSubImage3D", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "zoffset"), (GLint, "x"), (GLint, "y"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glActiveTextureARB", [(GLenum, "texture")]),
        F(Void, "glClientActiveTextureARB", [(GLenum, "texture")]),
        F(Void, "glMultiTexCoord1dARB", [(GLenum, "target"), (GLdouble, "s")]),
        F(Void, "glMultiTexCoord1dvARB", [(GLenum, "target"), (Array(Const(GLdouble), "1"), "v")]),
        F(Void, "glMultiTexCoord1fARB", [(GLenum, "target"), (GLfloat, "s")]),
        F(Void, "glMultiTexCoord1fvARB", [(GLenum, "target"), (Array(Const(GLfloat), "1"), "v")]),
        F(Void, "glMultiTexCoord1iARB", [(GLenum, "target"), (GLint, "s")]),
        F(Void, "glMultiTexCoord1ivARB", [(GLenum, "target"), (Array(Const(GLint), "1"), "v")]),
        F(Void, "glMultiTexCoord1sARB", [(GLenum, "target"), (GLshort, "s")]),
        F(Void, "glMultiTexCoord1svARB", [(GLenum, "target"), (Array(Const(GLshort), "1"), "v")]),
        F(Void, "glMultiTexCoord2dARB", [(GLenum, "target"), (GLdouble, "s"), (GLdouble, "t")]),
        F(Void, "glMultiTexCoord2dvARB", [(GLenum, "target"), (Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glMultiTexCoord2fARB", [(GLenum, "target"), (GLfloat, "s"), (GLfloat, "t")]),
        F(Void, "glMultiTexCoord2fvARB", [(GLenum, "target"), (Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glMultiTexCoord2iARB", [(GLenum, "target"), (GLint, "s"), (GLint, "t")]),
        F(Void, "glMultiTexCoord2ivARB", [(GLenum, "target"), (Array(Const(GLint), "2"), "v")]),
        F(Void, "glMultiTexCoord2sARB", [(GLenum, "target"), (GLshort, "s"), (GLshort, "t")]),
        F(Void, "glMultiTexCoord2svARB", [(GLenum, "target"), (Array(Const(GLshort), "2"), "v")]),
        F(Void, "glMultiTexCoord3dARB", [(GLenum, "target"), (GLdouble, "s"), (GLdouble, "t"), (GLdouble, "r")]),
        F(Void, "glMultiTexCoord3dvARB", [(GLenum, "target"), (Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glMultiTexCoord3fARB", [(GLenum, "target"), (GLfloat, "s"), (GLfloat, "t"), (GLfloat, "r")]),
        F(Void, "glMultiTexCoord3fvARB", [(GLenum, "target"), (Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glMultiTexCoord3iARB", [(GLenum, "target"), (GLint, "s"), (GLint, "t"), (GLint, "r")]),
        F(Void, "glMultiTexCoord3ivARB", [(GLenum, "target"), (Array(Const(GLint), "3"), "v")]),
        F(Void, "glMultiTexCoord3sARB", [(GLenum, "target"), (GLshort, "s"), (GLshort, "t"), (GLshort, "r")]),
        F(Void, "glMultiTexCoord3svARB", [(GLenum, "target"), (Array(Const(GLshort), "3"), "v")]),
        F(Void, "glMultiTexCoord4dARB", [(GLenum, "target"), (GLdouble, "s"), (GLdouble, "t"), (GLdouble, "r"), (GLdouble, "q")]),
        F(Void, "glMultiTexCoord4dvARB", [(GLenum, "target"), (Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glMultiTexCoord4fARB", [(GLenum, "target"), (GLfloat, "s"), (GLfloat, "t"), (GLfloat, "r"), (GLfloat, "q")]),
        F(Void, "glMultiTexCoord4fvARB", [(GLenum, "target"), (Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glMultiTexCoord4iARB", [(GLenum, "target"), (GLint, "s"), (GLint, "t"), (GLint, "r"), (GLint, "q")]),
        F(Void, "glMultiTexCoord4ivARB", [(GLenum, "target"), (Array(Const(GLint), "4"), "v")]),
        F(Void, "glMultiTexCoord4sARB", [(GLenum, "target"), (GLshort, "s"), (GLshort, "t"), (GLshort, "r"), (GLshort, "q")]),
        F(Void, "glMultiTexCoord4svARB", [(GLenum, "target"), (Array(Const(GLshort), "4"), "v")]),
    ]


def extended_functions(Function):
    def F(*args, **kwargs):
        kwargs.setdefault('call', 'GLAPIENTRY')
        return Function(*args, **kwargs)
    return [
        F(Void, "glAttachShader", [(GLuint, "program"), (GLuint, "shader")]),
        F(GLuint, "glCreateProgram", []),
        F(GLuint, "glCreateShader", [(GLenum, "type")]),
        F(Void, "glDeleteProgram", [(GLuint, "program")]),
        F(Void, "glDeleteShader", [(GLuint, "program")]),
        F(Void, "glDetachShader", [(GLuint, "program"), (GLuint, "shader")]),
        F(Void, "glGetAttachedShaders", [(GLuint, "program"), (GLsizei, "maxCount"), (OpaquePointer(GLsizei), "count"), (OpaquePointer(GLuint), "obj")], sideeffects=False),
        F(Void, "glGetProgramInfoLog", [(GLuint, "program"), (GLsizei, "bufSize"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLchar), "infoLog")], sideeffects=False),
        F(Void, "glGetProgramiv", [(GLuint, "program"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetShaderInfoLog", [(GLuint, "shader"), (GLsizei, "bufSize"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLchar), "infoLog")], sideeffects=False),
        F(Void, "glGetShaderiv", [(GLuint, "shader"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(GLboolean, "glIsProgram", [(GLuint, "program")]),
        F(GLboolean, "glIsShader", [(GLuint, "shader")]),
        F(Void, "glStencilFuncSeparate", [(GLenum, "face"), (GLenum, "func"), (GLint, "ref"), (GLuint, "mask")]),
        F(Void, "glStencilMaskSeparate", [(GLenum, "face"), (GLuint, "mask")]),
        F(Void, "glStencilOpSeparate", [(GLenum, "face"), (GLenum, "sfail"), (GLenum, "zfail"), (GLenum, "zpass")]),
        F(Void, "glUniformMatrix2x3fv", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix2x4fv", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix3x2fv", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix3x4fv", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix4x2fv", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix4x3fv", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glLoadTransposeMatrixdARB", [(OpaquePointer(Const(GLdouble)), "m")]),
        F(Void, "glLoadTransposeMatrixfARB", [(OpaquePointer(Const(GLfloat)), "m")]),
        F(Void, "glMultTransposeMatrixdARB", [(OpaquePointer(Const(GLdouble)), "m")]),
        F(Void, "glMultTransposeMatrixfARB", [(OpaquePointer(Const(GLfloat)), "m")]),
        F(Void, "glSampleCoverageARB", [(GLclampf, "value"), (GLboolean, "invert")]),
        F(Void, "glCompressedTexImage1DARB", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLsizei, "width"), (GLint, "border"), (GLsizei, "imageSize"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glCompressedTexImage2DARB", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLint, "border"), (GLsizei, "imageSize"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glCompressedTexImage3DARB", [(GLenum, "target"), (GLint, "level"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLint, "border"), (GLsizei, "imageSize"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glCompressedTexSubImage1DARB", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLsizei, "width"), (GLenum, "format"), (GLsizei, "imageSize"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glCompressedTexSubImage2DARB", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLsizei, "imageSize"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glCompressedTexSubImage3DARB", [(GLenum, "target"), (GLint, "level"), (GLint, "xoffset"), (GLint, "yoffset"), (GLint, "zoffset"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth"), (GLenum, "format"), (GLsizei, "imageSize"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glGetCompressedTexImageARB", [(GLenum, "target"), (GLint, "level"), (OpaquePointer(GLvoid), "img")], sideeffects=False),
        F(Void, "glDisableVertexAttribArrayARB", [(GLuint, "index")]),
        F(Void, "glEnableVertexAttribArrayARB", [(GLuint, "index")]),
        F(Void, "glGetProgramEnvParameterdvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetProgramEnvParameterfvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetProgramLocalParameterdvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetProgramLocalParameterfvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetProgramStringARB", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(GLvoid), "string")], sideeffects=False),
        F(Void, "glGetProgramivARB", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetVertexAttribdvARB", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetVertexAttribfvARB", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetVertexAttribivARB", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glProgramEnvParameter4dARB", [(GLenum, "target"), (GLuint, "index"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glProgramEnvParameter4dvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(Const(GLdouble)), "params")]),
        F(Void, "glProgramEnvParameter4fARB", [(GLenum, "target"), (GLuint, "index"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glProgramEnvParameter4fvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glProgramLocalParameter4dARB", [(GLenum, "target"), (GLuint, "index"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glProgramLocalParameter4dvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(Const(GLdouble)), "params")]),
        F(Void, "glProgramLocalParameter4fARB", [(GLenum, "target"), (GLuint, "index"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glProgramLocalParameter4fvARB", [(GLenum, "target"), (GLuint, "index"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glProgramStringARB", [(GLenum, "target"), (GLenum, "format"), (GLsizei, "len"), (OpaquePointer(Const(GLvoid)), "string")]),
        F(Void, "glVertexAttrib1dARB", [(GLuint, "index"), (GLdouble, "x")]),
        F(Void, "glVertexAttrib1dvARB", [(GLuint, "index"), (Array(Const(GLdouble), "1"), "v")]),
        F(Void, "glVertexAttrib1fARB", [(GLuint, "index"), (GLfloat, "x")]),
        F(Void, "glVertexAttrib1fvARB", [(GLuint, "index"), (Array(Const(GLfloat), "1"), "v")]),
        F(Void, "glVertexAttrib1sARB", [(GLuint, "index"), (GLshort, "x")]),
        F(Void, "glVertexAttrib1svARB", [(GLuint, "index"), (Array(Const(GLshort), "1"), "v")]),
        F(Void, "glVertexAttrib2dARB", [(GLuint, "index"), (GLdouble, "x"), (GLdouble, "y")]),
        F(Void, "glVertexAttrib2dvARB", [(GLuint, "index"), (Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glVertexAttrib2fARB", [(GLuint, "index"), (GLfloat, "x"), (GLfloat, "y")]),
        F(Void, "glVertexAttrib2fvARB", [(GLuint, "index"), (Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glVertexAttrib2sARB", [(GLuint, "index"), (GLshort, "x"), (GLshort, "y")]),
        F(Void, "glVertexAttrib2svARB", [(GLuint, "index"), (Array(Const(GLshort), "2"), "v")]),
        F(Void, "glVertexAttrib3dARB", [(GLuint, "index"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glVertexAttrib3dvARB", [(GLuint, "index"), (Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glVertexAttrib3fARB", [(GLuint, "index"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glVertexAttrib3fvARB", [(GLuint, "index"), (Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glVertexAttrib3sARB", [(GLuint, "index"), (GLshort, "x"), (GLshort, "y"), (GLshort, "z")]),
        F(Void, "glVertexAttrib3svARB", [(GLuint, "index"), (Array(Const(GLshort), "3"), "v")]),
        F(Void, "glVertexAttrib4NbvARB", [(GLuint, "index"), (OpaquePointer(Const(GLbyte)), "v")]),
        F(Void, "glVertexAttrib4NivARB", [(GLuint, "index"), (OpaquePointer(Const(GLint)), "v")]),
        F(Void, "glVertexAttrib4NsvARB", [(GLuint, "index"), (OpaquePointer(Const(GLshort)), "v")]),
        F(Void, "glVertexAttrib4NubARB", [(GLuint, "index"), (GLubyte, "x"), (GLubyte, "y"), (GLubyte, "z"), (GLubyte, "w")]),
        F(Void, "glVertexAttrib4NubvARB", [(GLuint, "index"), (OpaquePointer(Const(GLubyte)), "v")]),
        F(Void, "glVertexAttrib4NuivARB", [(GLuint, "index"), (OpaquePointer(Const(GLuint)), "v")]),
        F(Void, "glVertexAttrib4NusvARB", [(GLuint, "index"), (OpaquePointer(Const(GLushort)), "v")]),
        F(Void, "glVertexAttrib4bvARB", [(GLuint, "index"), (Array(Const(GLbyte), "4"), "v")]),
        F(Void, "glVertexAttrib4dARB", [(GLuint, "index"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glVertexAttrib4dvARB", [(GLuint, "index"), (Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glVertexAttrib4fARB", [(GLuint, "index"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glVertexAttrib4fvARB", [(GLuint, "index"), (Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glVertexAttrib4ivARB", [(GLuint, "index"), (Array(Const(GLint), "4"), "v")]),
        F(Void, "glVertexAttrib4sARB", [(GLuint, "index"), (GLshort, "x"), (GLshort, "y"), (GLshort, "z"), (GLshort, "w")]),
        F(Void, "glVertexAttrib4svARB", [(GLuint, "index"), (Array(Const(GLshort), "4"), "v")]),
        F(Void, "glVertexAttrib4ubvARB", [(GLuint, "index"), (Array(Const(GLubyte), "4"), "v")]),
        F(Void, "glVertexAttrib4uivARB", [(GLuint, "index"), (Array(Const(GLuint), "4"), "v")]),
        F(Void, "glVertexAttrib4usvARB", [(GLuint, "index"), (Array(Const(GLushort), "4"), "v")]),
        F(Void, "glVertexAttribPointerARB", [(GLuint, "index"), (GLint, "size"), (GLenum, "type"), (GLboolean, "normalized"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glBindBufferARB", [(GLenum, "target"), (GLuint, "buffer")]),
        F(Void, "glBufferDataARB", [(GLenum, "target"), (GLsizeiptrARB, "size"), (OpaquePointer(Const(GLvoid)), "data"), (GLenum, "usage")]),
        F(Void, "glBufferSubDataARB", [(GLenum, "target"), (GLintptrARB, "offset"), (GLsizeiptrARB, "size"), (OpaquePointer(Const(GLvoid)), "data")]),
        F(Void, "glDeleteBuffersARB", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "buffer")]),
        F(Void, "glGenBuffersARB", [(GLsizei, "n"), (OpaquePointer(GLuint), "buffer")]),
        F(Void, "glGetBufferParameterivARB", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetBufferPointervARB", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(OpaquePointer(GLvoid)), "params")], sideeffects=False),
        F(Void, "glGetBufferSubDataARB", [(GLenum, "target"), (GLintptrARB, "offset"), (GLsizeiptrARB, "size"), (OpaquePointer(GLvoid), "data")], sideeffects=False),
        F(GLboolean, "glIsBufferARB", [(GLuint, "buffer")]),
        F(OpaquePointer(GLvoid), "glMapBufferARB", [(GLenum, "target"), (GLenum, "access")]),
        F(GLboolean, "glUnmapBufferARB", [(GLenum, "target")]),
        F(Void, "glBeginQueryARB", [(GLenum, "target"), (GLuint, "id")]),
        F(Void, "glDeleteQueriesARB", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "ids")]),
        F(Void, "glEndQueryARB", [(GLenum, "target")]),
        F(Void, "glGenQueriesARB", [(GLsizei, "n"), (OpaquePointer(GLuint), "ids")]),
        F(Void, "glGetQueryObjectivARB", [(GLuint, "id"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetQueryObjectuivARB", [(GLuint, "id"), (GLenum, "pname"), (OpaquePointer(GLuint), "params")], sideeffects=False),
        F(Void, "glGetQueryivARB", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(GLboolean, "glIsQueryARB", [(GLuint, "id")]),
        F(Void, "glAttachObjectARB", [(GLhandleARB, "containerObj"), (GLhandleARB, "obj")]),
        F(Void, "glCompileShaderARB", [(GLhandleARB, "shader")]),
        F(GLhandleARB, "glCreateProgramObjectARB", []),
        F(GLhandleARB, "glCreateShaderObjectARB", [(GLenum, "shaderType")]),
        F(Void, "glDeleteObjectARB", [(GLhandleARB, "obj")]),
        F(Void, "glDetachObjectARB", [(GLhandleARB, "containerObj"), (GLhandleARB, "attachedObj")]),
        F(Void, "glGetActiveUniformARB", [(GLhandleARB, "program"), (GLuint, "index"), (GLsizei, "bufSize"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLint), "size"), (OpaquePointer(GLenum), "type"), (OpaquePointer(GLcharARB), "name")], sideeffects=False),
        F(Void, "glGetAttachedObjectsARB", [(GLhandleARB, "containerObj"), (GLsizei, "maxLength"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLhandleARB), "infoLog")], sideeffects=False),
        F(GLhandleARB, "glGetHandleARB", [(GLenum, "pname")], sideeffects=False),
        F(Void, "glGetInfoLogARB", [(GLhandleARB, "obj"), (GLsizei, "maxLength"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLcharARB), "infoLog")], sideeffects=False),
        F(Void, "glGetObjectParameterfvARB", [(GLhandleARB, "obj"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetObjectParameterivARB", [(GLhandleARB, "obj"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetShaderSourceARB", [(GLhandleARB, "shader"), (GLsizei, "bufSize"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLcharARB), "source")], sideeffects=False),
        F(GLint, "glGetUniformLocationARB", [(GLhandleARB, "program"), (OpaquePointer(Const(GLcharARB)), "name")], sideeffects=False),
        F(Void, "glGetUniformfvARB", [(GLhandleARB, "program"), (GLint, "location"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetUniformivARB", [(GLhandleARB, "program"), (GLint, "location"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glLinkProgramARB", [(GLhandleARB, "program")]),
        F(Void, "glShaderSourceARB", [(GLhandleARB, "shader"), (GLsizei, "count"), (Array(GLstringARB, "count"), "string"), (Array(Const(GLint), "count"), "length")]), # FIXME
        F(Void, "glUniform1fARB", [(GLint, "location"), (GLfloat, "v0")]),
        F(Void, "glUniform1fvARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniform1iARB", [(GLint, "location"), (GLint, "v0")]),
        F(Void, "glUniform1ivARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLint)), "value")]),
        F(Void, "glUniform2fARB", [(GLint, "location"), (GLfloat, "v0"), (GLfloat, "v1")]),
        F(Void, "glUniform2fvARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniform2iARB", [(GLint, "location"), (GLint, "v0"), (GLint, "v1")]),
        F(Void, "glUniform2ivARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLint)), "value")]),
        F(Void, "glUniform3fARB", [(GLint, "location"), (GLfloat, "v0"), (GLfloat, "v1"), (GLfloat, "v2")]),
        F(Void, "glUniform3fvARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniform3iARB", [(GLint, "location"), (GLint, "v0"), (GLint, "v1"), (GLint, "v2")]),
        F(Void, "glUniform3ivARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLint)), "value")]),
        F(Void, "glUniform4fARB", [(GLint, "location"), (GLfloat, "v0"), (GLfloat, "v1"), (GLfloat, "v2"), (GLfloat, "v3")]),
        F(Void, "glUniform4fvARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniform4iARB", [(GLint, "location"), (GLint, "v0"), (GLint, "v1"), (GLint, "v2"), (GLint, "v3")]),
        F(Void, "glUniform4ivARB", [(GLint, "location"), (GLsizei, "count"), (OpaquePointer(Const(GLint)), "value")]),
        F(Void, "glUniformMatrix2fvARB", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix3fvARB", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUniformMatrix4fvARB", [(GLint, "location"), (GLsizei, "count"), (GLboolean, "transpose"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glUseProgramObjectARB", [(GLhandleARB, "program")]),
        F(Void, "glValidateProgramARB", [(GLhandleARB, "program")]),
        F(Void, "glBindAttribLocationARB", [(GLhandleARB, "program"), (GLuint, "index"), (OpaquePointer(Const(GLcharARB)), "name")]),
        F(Void, "glGetActiveAttribARB", [(GLhandleARB, "program"), (GLuint, "index"), (GLsizei, "bufSize"), (OpaquePointer(GLsizei), "length"), (OpaquePointer(GLint), "size"), (OpaquePointer(GLenum), "type"), (OpaquePointer(GLcharARB), "name")], sideeffects=False),
        F(GLint, "glGetAttribLocationARB", [(GLhandleARB, "program"), (OpaquePointer(Const(GLcharARB)), "name")], sideeffects=False),
        F(Void, "glDrawBuffersARB", [(GLsizei, "n"), (OpaquePointer(Const(GLenum)), "bufs")]),
        F(Void, "glRenderbufferStorageMultisample", [(GLenum, "target"), (GLsizei, "samples"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glPolygonOffsetEXT", [(GLfloat, "factor"), (GLfloat, "bias")]),
        #F(Void, "glGetPixelTexGenParameterfvSGIS", [(GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        #F(Void, "glGetPixelTexGenParameterivSGIS", [(GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        #F(Void, "glPixelTexGenParameterfSGIS", [(GLenum, "pname"), (GLfloat, "param")]),
        #F(Void, "glPixelTexGenParameterfvSGIS", [(GLenum, "pname"), (OpaquePointer(Const(GLfloat)), "params")]),
        #F(Void, "glPixelTexGenParameteriSGIS", [(GLenum, "pname"), (GLint, "param")]),
        #F(Void, "glPixelTexGenParameterivSGIS", [(GLenum, "pname"), (OpaquePointer(Const(GLint)), "params")]),
        #F(Void, "glSampleMaskSGIS", [(GLclampf, "value"), (GLboolean, "invert")]),
        #F(Void, "glSamplePatternSGIS", [(GLenum, "pattern")]),
        F(Void, "glColorPointerEXT", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (GLsizei, "count"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glEdgeFlagPointerEXT", [(GLsizei, "stride"), (GLsizei, "count"), (OpaquePointer(Const(GLboolean)), "pointer")]),
        F(Void, "glIndexPointerEXT", [(GLenum, "type"), (GLsizei, "stride"), (GLsizei, "count"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glNormalPointerEXT", [(GLenum, "type"), (GLsizei, "stride"), (GLsizei, "count"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glTexCoordPointerEXT", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (GLsizei, "count"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glVertexPointerEXT", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (GLsizei, "count"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glPointParameterfEXT", [(GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glPointParameterfvEXT", [(GLenum, "pname"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glLockArraysEXT", [(GLint, "first"), (GLsizei, "count")]),
        F(Void, "glUnlockArraysEXT", []),
        F(Void, "glCullParameterdvEXT", [(GLenum, "pname"), (OpaquePointer(GLdouble), "params")]),
        F(Void, "glCullParameterfvEXT", [(GLenum, "pname"), (OpaquePointer(GLfloat), "params")]),
        F(Void, "glSecondaryColor3bEXT", [(GLbyte, "red"), (GLbyte, "green"), (GLbyte, "blue")]),
        F(Void, "glSecondaryColor3bvEXT", [(Array(Const(GLbyte), "3"), "v")]),
        F(Void, "glSecondaryColor3dEXT", [(GLdouble, "red"), (GLdouble, "green"), (GLdouble, "blue")]),
        F(Void, "glSecondaryColor3dvEXT", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glSecondaryColor3fEXT", [(GLfloat, "red"), (GLfloat, "green"), (GLfloat, "blue")]),
        F(Void, "glSecondaryColor3fvEXT", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glSecondaryColor3iEXT", [(GLint, "red"), (GLint, "green"), (GLint, "blue")]),
        F(Void, "glSecondaryColor3ivEXT", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glSecondaryColor3sEXT", [(GLshort, "red"), (GLshort, "green"), (GLshort, "blue")]),
        F(Void, "glSecondaryColor3svEXT", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glSecondaryColor3ubEXT", [(GLubyte, "red"), (GLubyte, "green"), (GLubyte, "blue")]),
        F(Void, "glSecondaryColor3ubvEXT", [(Array(Const(GLubyte), "3"), "v")]),
        F(Void, "glSecondaryColor3uiEXT", [(GLuint, "red"), (GLuint, "green"), (GLuint, "blue")]),
        F(Void, "glSecondaryColor3uivEXT", [(Array(Const(GLuint), "3"), "v")]),
        F(Void, "glSecondaryColor3usEXT", [(GLushort, "red"), (GLushort, "green"), (GLushort, "blue")]),
        F(Void, "glSecondaryColor3usvEXT", [(Array(Const(GLushort), "3"), "v")]),
        F(Void, "glSecondaryColorPointerEXT", [(GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glMultiDrawArraysEXT", [(GLenum, "mode"), (OpaquePointer(GLint), "first"), (OpaquePointer(GLsizei), "count"), (GLsizei, "primcount")]),
        F(Void, "glMultiDrawElementsEXT", [(GLenum, "mode"), (Array(Const(GLsizei), "primcount"), "count"), (GLenum, "type"), (Array(Const(OpaquePointer(Const(GLvoid))), "primcount"), "indices"), (GLsizei, "primcount")]),
        F(Void, "glFogCoordPointerEXT", [(GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glFogCoorddEXT", [(GLdouble, "coord")]),
        F(Void, "glFogCoorddvEXT", [(OpaquePointer(Const(GLdouble)), "coord")]),
        F(Void, "glFogCoordfEXT", [(GLfloat, "coord")]),
        F(Void, "glFogCoordfvEXT", [(OpaquePointer(Const(GLfloat)), "coord")]),
        F(Void, "glPixelTexGenSGIX", [(GLenum, "mode")]),
        F(Void, "glBlendFuncSeparateEXT", [(GLenum, "sfactorRGB"), (GLenum, "dfactorRGB"), (GLenum, "sfactorAlpha"), (GLenum, "dfactorAlpha")]),
        F(Void, "glFlushVertexArrayRangeNV", []),
        F(Void, "glVertexArrayRangeNV", [(GLsizei, "length"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glCombinerInputNV", [(GLenum, "stage"), (GLenum, "portion"), (GLenum, "variable"), (GLenum, "input"), (GLenum, "mapping"), (GLenum, "componentUsage")]),
        F(Void, "glCombinerOutputNV", [(GLenum, "stage"), (GLenum, "portion"), (GLenum, "abOutput"), (GLenum, "cdOutput"), (GLenum, "sumOutput"), (GLenum, "scale"), (GLenum, "bias"), (GLboolean, "abDotProduct"), (GLboolean, "cdDotProduct"), (GLboolean, "muxSum")]),
        F(Void, "glCombinerParameterfNV", [(GLenum, "pname"), (GLfloat, "param")]),
        F(Void, "glCombinerParameterfvNV", [(GLenum, "pname"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glCombinerParameteriNV", [(GLenum, "pname"), (GLint, "param")]),
        F(Void, "glCombinerParameterivNV", [(GLenum, "pname"), (OpaquePointer(Const(GLint)), "params")]),
        F(Void, "glFinalCombinerInputNV", [(GLenum, "variable"), (GLenum, "input"), (GLenum, "mapping"), (GLenum, "componentUsage")]),
        F(Void, "glGetCombinerInputParameterfvNV", [(GLenum, "stage"), (GLenum, "portion"), (GLenum, "variable"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetCombinerInputParameterivNV", [(GLenum, "stage"), (GLenum, "portion"), (GLenum, "variable"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetCombinerOutputParameterfvNV", [(GLenum, "stage"), (GLenum, "portion"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetCombinerOutputParameterivNV", [(GLenum, "stage"), (GLenum, "portion"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetFinalCombinerInputParameterfvNV", [(GLenum, "variable"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetFinalCombinerInputParameterivNV", [(GLenum, "variable"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glResizeBuffersMESA", []),
        F(Void, "glWindowPos2dMESA", [(GLdouble, "x"), (GLdouble, "y")]),
        F(Void, "glWindowPos2dvMESA", [(Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glWindowPos2fMESA", [(GLfloat, "x"), (GLfloat, "y")]),
        F(Void, "glWindowPos2fvMESA", [(Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glWindowPos2iMESA", [(GLint, "x"), (GLint, "y")]),
        F(Void, "glWindowPos2ivMESA", [(Array(Const(GLint), "2"), "v")]),
        F(Void, "glWindowPos2sMESA", [(GLshort, "x"), (GLshort, "y")]),
        F(Void, "glWindowPos2svMESA", [(Array(Const(GLshort), "2"), "v")]),
        F(Void, "glWindowPos3dMESA", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glWindowPos3dvMESA", [(Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glWindowPos3fMESA", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glWindowPos3fvMESA", [(Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glWindowPos3iMESA", [(GLint, "x"), (GLint, "y"), (GLint, "z")]),
        F(Void, "glWindowPos3ivMESA", [(Array(Const(GLint), "3"), "v")]),
        F(Void, "glWindowPos3sMESA", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z")]),
        F(Void, "glWindowPos3svMESA", [(Array(Const(GLshort), "3"), "v")]),
        F(Void, "glWindowPos4dMESA", [(GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glWindowPos4dvMESA", [(Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glWindowPos4fMESA", [(GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glWindowPos4fvMESA", [(Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glWindowPos4iMESA", [(GLint, "x"), (GLint, "y"), (GLint, "z"), (GLint, "w")]),
        F(Void, "glWindowPos4ivMESA", [(Array(Const(GLint), "4"), "v")]),
        F(Void, "glWindowPos4sMESA", [(GLshort, "x"), (GLshort, "y"), (GLshort, "z"), (GLshort, "w")]),
        F(Void, "glWindowPos4svMESA", [(Array(Const(GLshort), "4"), "v")]),
        F(Void, "glMultiModeDrawArraysIBM", [(OpaquePointer(Const(GLenum)), "mode"), (OpaquePointer(Const(GLint)), "first"), (OpaquePointer(Const(GLsizei)), "count"), (GLsizei, "primcount"), (GLint, "modestride")]),
        F(Void, "glMultiModeDrawElementsIBM", [(OpaquePointer(Const(GLenum)), "mode"), (Array(Const(GLsizei), "primcount"), "count"), (GLenum, "type"), (Array(Const(OpaquePointer(Const(GLvoid))), "primcount"), "indices"), (GLsizei, "primcount"), (GLint, "modestride")]),
        F(Void, "glDeleteFencesNV", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "fences")]),
        F(Void, "glFinishFenceNV", [(GLuint, "fence")]),
        F(Void, "glGenFencesNV", [(GLsizei, "n"), (OpaquePointer(GLuint), "fences")]),
        F(Void, "glGetFenceivNV", [(GLuint, "fence"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(GLboolean, "glIsFenceNV", [(GLuint, "fence")]),
        F(Void, "glSetFenceNV", [(GLuint, "fence"), (GLenum, "condition")]),
        F(GLboolean, "glTestFenceNV", [(GLuint, "fence")]),
        F(GLboolean, "glAreProgramsResidentNV", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "ids"), (OpaquePointer(GLboolean), "residences")]),
        F(Void, "glBindProgramNV", [(GLenum, "target"), (GLuint, "program")]),
        F(Void, "glDeleteProgramsNV", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "programs")]),
        F(Void, "glExecuteProgramNV", [(GLenum, "target"), (GLuint, "id"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glGenProgramsNV", [(GLsizei, "n"), (OpaquePointer(GLuint), "programs")]),
        F(Void, "glGetProgramParameterdvNV", [(GLenum, "target"), (GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetProgramParameterfvNV", [(GLenum, "target"), (GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetProgramStringNV", [(GLuint, "id"), (GLenum, "pname"), (OpaquePointer(GLubyte), "program")], sideeffects=False),
        F(Void, "glGetProgramivNV", [(GLuint, "id"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetTrackMatrixivNV", [(GLenum, "target"), (GLuint, "address"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetVertexAttribPointervNV", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(OpaquePointer(GLvoid)), "pointer")], sideeffects=False),
        F(Void, "glGetVertexAttribdvNV", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetVertexAttribfvNV", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glGetVertexAttribivNV", [(GLuint, "index"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(GLboolean, "glIsProgramNV", [(GLuint, "program")]),
        F(Void, "glLoadProgramNV", [(GLenum, "target"), (GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "program")]),
        F(Void, "glProgramParameters4dvNV", [(GLenum, "target"), (GLuint, "index"), (GLuint, "num"), (OpaquePointer(Const(GLdouble)), "params")]),
        F(Void, "glProgramParameters4fvNV", [(GLenum, "target"), (GLuint, "index"), (GLuint, "num"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glRequestResidentProgramsNV", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "ids")]),
        F(Void, "glTrackMatrixNV", [(GLenum, "target"), (GLuint, "address"), (GLenum, "matrix"), (GLenum, "transform")]),
        F(Void, "glVertexAttrib1dNV", [(GLuint, "index"), (GLdouble, "x")]),
        F(Void, "glVertexAttrib1dvNV", [(GLuint, "index"), (Array(Const(GLdouble), "1"), "v")]),
        F(Void, "glVertexAttrib1fNV", [(GLuint, "index"), (GLfloat, "x")]),
        F(Void, "glVertexAttrib1fvNV", [(GLuint, "index"), (Array(Const(GLfloat), "1"), "v")]),
        F(Void, "glVertexAttrib1sNV", [(GLuint, "index"), (GLshort, "x")]),
        F(Void, "glVertexAttrib1svNV", [(GLuint, "index"), (Array(Const(GLshort), "1"), "v")]),
        F(Void, "glVertexAttrib2dNV", [(GLuint, "index"), (GLdouble, "x"), (GLdouble, "y")]),
        F(Void, "glVertexAttrib2dvNV", [(GLuint, "index"), (Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glVertexAttrib2fNV", [(GLuint, "index"), (GLfloat, "x"), (GLfloat, "y")]),
        F(Void, "glVertexAttrib2fvNV", [(GLuint, "index"), (Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glVertexAttrib2sNV", [(GLuint, "index"), (GLshort, "x"), (GLshort, "y")]),
        F(Void, "glVertexAttrib2svNV", [(GLuint, "index"), (Array(Const(GLshort), "2"), "v")]),
        F(Void, "glVertexAttrib3dNV", [(GLuint, "index"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z")]),
        F(Void, "glVertexAttrib3dvNV", [(GLuint, "index"), (Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glVertexAttrib3fNV", [(GLuint, "index"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z")]),
        F(Void, "glVertexAttrib3fvNV", [(GLuint, "index"), (Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glVertexAttrib3sNV", [(GLuint, "index"), (GLshort, "x"), (GLshort, "y"), (GLshort, "z")]),
        F(Void, "glVertexAttrib3svNV", [(GLuint, "index"), (Array(Const(GLshort), "3"), "v")]),
        F(Void, "glVertexAttrib4dNV", [(GLuint, "index"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glVertexAttrib4dvNV", [(GLuint, "index"), (Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glVertexAttrib4fNV", [(GLuint, "index"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glVertexAttrib4fvNV", [(GLuint, "index"), (Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glVertexAttrib4sNV", [(GLuint, "index"), (GLshort, "x"), (GLshort, "y"), (GLshort, "z"), (GLshort, "w")]),
        F(Void, "glVertexAttrib4svNV", [(GLuint, "index"), (Array(Const(GLshort), "4"), "v")]),
        F(Void, "glVertexAttrib4ubNV", [(GLuint, "index"), (GLubyte, "x"), (GLubyte, "y"), (GLubyte, "z"), (GLubyte, "w")]),
        F(Void, "glVertexAttrib4ubvNV", [(GLuint, "index"), (Array(Const(GLubyte), "4"), "v")]),
        F(Void, "glVertexAttribPointerNV", [(GLuint, "index"), (GLint, "size"), (GLenum, "type"), (GLsizei, "stride"), (OpaquePointer(Const(GLvoid)), "pointer")]),
        F(Void, "glVertexAttribs1dvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLdouble), "1"), "v")]),
        F(Void, "glVertexAttribs1fvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLfloat), "1"), "v")]),
        F(Void, "glVertexAttribs1svNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLshort), "1"), "v")]),
        F(Void, "glVertexAttribs2dvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLdouble), "2"), "v")]),
        F(Void, "glVertexAttribs2fvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLfloat), "2"), "v")]),
        F(Void, "glVertexAttribs2svNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLshort), "2"), "v")]),
        F(Void, "glVertexAttribs3dvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLdouble), "3"), "v")]),
        F(Void, "glVertexAttribs3fvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLfloat), "3"), "v")]),
        F(Void, "glVertexAttribs3svNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLshort), "3"), "v")]),
        F(Void, "glVertexAttribs4dvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glVertexAttribs4fvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glVertexAttribs4svNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLshort), "4"), "v")]),
        F(Void, "glVertexAttribs4ubvNV", [(GLuint, "index"), (GLsizei, "n"), (Array(Const(GLubyte), "4"), "v")]),
        F(Void, "glGetTexBumpParameterfvATI", [(GLenum, "pname"), (OpaquePointer(GLfloat), "param")], sideeffects=False),
        F(Void, "glGetTexBumpParameterivATI", [(GLenum, "pname"), (OpaquePointer(GLint), "param")], sideeffects=False),
        F(Void, "glTexBumpParameterfvATI", [(GLenum, "pname"), (OpaquePointer(Const(GLfloat)), "param")]),
        F(Void, "glTexBumpParameterivATI", [(GLenum, "pname"), (OpaquePointer(Const(GLint)), "param")]),
        F(Void, "glAlphaFragmentOp1ATI", [(GLenum, "op"), (GLuint, "dst"), (GLuint, "dstMod"), (GLuint, "arg1"), (GLuint, "arg1Rep"), (GLuint, "arg1Mod")]),
        F(Void, "glAlphaFragmentOp2ATI", [(GLenum, "op"), (GLuint, "dst"), (GLuint, "dstMod"), (GLuint, "arg1"), (GLuint, "arg1Rep"), (GLuint, "arg1Mod"), (GLuint, "arg2"), (GLuint, "arg2Rep"), (GLuint, "arg2Mod")]),
        F(Void, "glAlphaFragmentOp3ATI", [(GLenum, "op"), (GLuint, "dst"), (GLuint, "dstMod"), (GLuint, "arg1"), (GLuint, "arg1Rep"), (GLuint, "arg1Mod"), (GLuint, "arg2"), (GLuint, "arg2Rep"), (GLuint, "arg2Mod"), (GLuint, "arg3"), (GLuint, "arg3Rep"), (GLuint, "arg3Mod")]),
        F(Void, "glBeginFragmentShaderATI", []),
        F(Void, "glBindFragmentShaderATI", [(GLuint, "id")]),
        F(Void, "glColorFragmentOp1ATI", [(GLenum, "op"), (GLuint, "dst"), (GLuint, "dstMask"), (GLuint, "dstMod"), (GLuint, "arg1"), (GLuint, "arg1Rep"), (GLuint, "arg1Mod")]),
        F(Void, "glColorFragmentOp2ATI", [(GLenum, "op"), (GLuint, "dst"), (GLuint, "dstMask"), (GLuint, "dstMod"), (GLuint, "arg1"), (GLuint, "arg1Rep"), (GLuint, "arg1Mod"), (GLuint, "arg2"), (GLuint, "arg2Rep"), (GLuint, "arg2Mod")]),
        F(Void, "glColorFragmentOp3ATI", [(GLenum, "op"), (GLuint, "dst"), (GLuint, "dstMask"), (GLuint, "dstMod"), (GLuint, "arg1"), (GLuint, "arg1Rep"), (GLuint, "arg1Mod"), (GLuint, "arg2"), (GLuint, "arg2Rep"), (GLuint, "arg2Mod"), (GLuint, "arg3"), (GLuint, "arg3Rep"), (GLuint, "arg3Mod")]),
        F(Void, "glDeleteFragmentShaderATI", [(GLuint, "id")]),
        F(Void, "glEndFragmentShaderATI", []),
        F(GLuint, "glGenFragmentShadersATI", [(GLuint, "range")]),
        F(Void, "glPassTexCoordATI", [(GLuint, "dst"), (GLuint, "coord"), (GLenum, "swizzle")]),
        F(Void, "glSampleMapATI", [(GLuint, "dst"), (GLuint, "interp"), (GLenum, "swizzle")]),
        F(Void, "glSetFragmentShaderConstantATI", [(GLuint, "dst"), (OpaquePointer(Const(GLfloat)), "value")]),
        F(Void, "glPointParameteriNV", [(GLenum, "pname"), (GLint, "param")]),
        F(Void, "glPointParameterivNV", [(GLenum, "pname"), (OpaquePointer(Const(GLint)), "params")]),
        F(Void, "glActiveStencilFaceEXT", [(GLenum, "face")]),
        F(Void, "glBindVertexArrayAPPLE", [(GLuint, "array")]),
        F(Void, "glDeleteVertexArraysAPPLE", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "arrays")]),
        F(Void, "glGenVertexArraysAPPLE", [(GLsizei, "n"), (OpaquePointer(GLuint), "arrays")]),
        F(GLboolean, "glIsVertexArrayAPPLE", [(GLuint, "array")]),
        F(Void, "glGetProgramNamedParameterdvNV", [(GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "name"), (OpaquePointer(GLdouble), "params")], sideeffects=False),
        F(Void, "glGetProgramNamedParameterfvNV", [(GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "name"), (OpaquePointer(GLfloat), "params")], sideeffects=False),
        F(Void, "glProgramNamedParameter4dNV", [(GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "name"), (GLdouble, "x"), (GLdouble, "y"), (GLdouble, "z"), (GLdouble, "w")]),
        F(Void, "glProgramNamedParameter4dvNV", [(GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "name"), (Array(Const(GLdouble), "4"), "v")]),
        F(Void, "glProgramNamedParameter4fNV", [(GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "name"), (GLfloat, "x"), (GLfloat, "y"), (GLfloat, "z"), (GLfloat, "w")]),
        F(Void, "glProgramNamedParameter4fvNV", [(GLuint, "id"), (GLsizei, "len"), (OpaquePointer(Const(GLubyte)), "name"), (Array(Const(GLfloat), "4"), "v")]),
        F(Void, "glDepthBoundsEXT", [(GLclampd, "zmin"), (GLclampd, "zmax")]),
        F(Void, "glBlendEquationSeparateEXT", [(GLenum, "modeRGB"), (GLenum, "modeA")]),
        F(Void, "glBindFramebufferEXT", [(GLenum, "target"), (GLuint, "framebuffer")]),
        F(Void, "glBindRenderbufferEXT", [(GLenum, "target"), (GLuint, "renderbuffer")]),
        F(GLenum, "glCheckFramebufferStatusEXT", [(GLenum, "target")]),
        F(Void, "glDeleteFramebuffersEXT", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "framebuffers")]),
        F(Void, "glDeleteRenderbuffersEXT", [(GLsizei, "n"), (OpaquePointer(Const(GLuint)), "renderbuffers")]),
        F(Void, "glFramebufferRenderbufferEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "renderbuffertarget"), (GLuint, "renderbuffer")]),
        F(Void, "glFramebufferTexture1DEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLuint, "texture"), (GLint, "level")]),
        F(Void, "glFramebufferTexture2DEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLuint, "texture"), (GLint, "level")]),
        F(Void, "glFramebufferTexture3DEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "textarget"), (GLuint, "texture"), (GLint, "level"), (GLint, "zoffset")]),
        F(Void, "glGenFramebuffersEXT", [(GLsizei, "n"), (OpaquePointer(GLuint), "framebuffers")]),
        F(Void, "glGenRenderbuffersEXT", [(GLsizei, "n"), (OpaquePointer(GLuint), "renderbuffers")]),
        F(Void, "glGenerateMipmapEXT", [(GLenum, "target")]),
        F(Void, "glGetFramebufferAttachmentParameterivEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(Void, "glGetRenderbufferParameterivEXT", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(GLint), "params")], sideeffects=False),
        F(GLboolean, "glIsFramebufferEXT", [(GLuint, "framebuffer")]),
        F(GLboolean, "glIsRenderbufferEXT", [(GLuint, "renderbuffer")]),
        F(Void, "glRenderbufferStorageEXT", [(GLenum, "target"), (GLenum, "internalformat"), (GLsizei, "width"), (GLsizei, "height")]),
        F(Void, "glBlitFramebufferEXT", [(GLint, "srcX0"), (GLint, "srcY0"), (GLint, "srcX1"), (GLint, "srcY1"), (GLint, "dstX0"), (GLint, "dstY0"), (GLint, "dstX1"), (GLint, "dstY1"), (GLbitfield, "mask"), (GLenum, "filter")]),
        F(Void, "glFramebufferTextureLayerEXT", [(GLenum, "target"), (GLenum, "attachment"), (GLuint, "texture"), (GLint, "level"), (GLint, "layer")]),
        F(Void, "glStencilFuncSeparateATI", [(GLenum, "frontfunc"), (GLenum, "backfunc"), (GLint, "ref"), (GLuint, "mask")]),
        F(Void, "glProgramEnvParameters4fvEXT", [(GLenum, "target"), (GLuint, "index"), (GLsizei, "count"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glProgramLocalParameters4fvEXT", [(GLenum, "target"), (GLuint, "index"), (GLsizei, "count"), (OpaquePointer(Const(GLfloat)), "params")]),
        F(Void, "glGetQueryObjecti64vEXT", [(GLuint, "id"), (GLenum, "pname"), (OpaquePointer(GLint64EXT), "params")], sideeffects=False),
        F(Void, "glGetQueryObjectui64vEXT", [(GLuint, "id"), (GLenum, "pname"), (OpaquePointer(GLuint64EXT), "params")], sideeffects=False),
    ]

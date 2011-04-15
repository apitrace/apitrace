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


from stdapi import *

from glapi import *


X = None # To be determined
B = GLboolean
I = GLint
E = GLenum
F = GLfloat
D = GLdouble
P = OpaquePointer(Void)
S = CString

# TODO: This table and the one on glenum.py should be unified
parameters = [
    # (function, type, count, name) # value
    (None,	X,	1,	"GL_POINTS"),	# 0x0000
    (None,	X,	1,	"GL_LINES"),	# 0x0001
    (None,	X,	1,	"GL_LINE_LOOP"),	# 0x0002
    (None,	X,	1,	"GL_LINE_STRIP"),	# 0x0003
    (None,	X,	1,	"GL_TRIANGLES"),	# 0x0004
    (None,	X,	1,	"GL_TRIANGLE_STRIP"),	# 0x0005
    (None,	X,	1,	"GL_TRIANGLE_FAN"),	# 0x0006
    (None,	X,	1,	"GL_QUADS"),	# 0x0007
    (None,	X,	1,	"GL_QUAD_STRIP"),	# 0x0008
    (None,	X,	1,	"GL_POLYGON"),	# 0x0009
    (None,	X,	1,	"GL_LINES_ADJACENCY"),	# 0x000A
    (None,	X,	1,	"GL_LINE_STRIP_ADJACENCY"),	# 0x000B
    (None,	X,	1,	"GL_TRIANGLES_ADJACENCY"),	# 0x000C
    (None,	X,	1,	"GL_TRIANGLE_STRIP_ADJACENCY"),	# 0x000D
    (None,	X,	1,	"GL_PATCHES"),	# 0x000E
    (None,	X,	1,	"GL_ACCUM"),	# 0x0100
    (None,	X,	1,	"GL_LOAD"),	# 0x0101
    (None,	X,	1,	"GL_RETURN"),	# 0x0102
    (None,	X,	1,	"GL_MULT"),	# 0x0103
    (None,	X,	1,	"GL_ADD"),	# 0x0104
    (None,	X,	1,	"GL_NEVER"),	# 0x0200
    (None,	X,	1,	"GL_LESS"),	# 0x0201
    (None,	X,	1,	"GL_EQUAL"),	# 0x0202
    (None,	X,	1,	"GL_LEQUAL"),	# 0x0203
    (None,	X,	1,	"GL_GREATER"),	# 0x0204
    (None,	X,	1,	"GL_NOTEQUAL"),	# 0x0205
    (None,	X,	1,	"GL_GEQUAL"),	# 0x0206
    (None,	X,	1,	"GL_ALWAYS"),	# 0x0207
    (None,	X,	1,	"GL_SRC_COLOR"),	# 0x0300
    (None,	X,	1,	"GL_ONE_MINUS_SRC_COLOR"),	# 0x0301
    (None,	X,	1,	"GL_SRC_ALPHA"),	# 0x0302
    (None,	X,	1,	"GL_ONE_MINUS_SRC_ALPHA"),	# 0x0303
    (None,	X,	1,	"GL_DST_ALPHA"),	# 0x0304
    (None,	X,	1,	"GL_ONE_MINUS_DST_ALPHA"),	# 0x0305
    (None,	X,	1,	"GL_DST_COLOR"),	# 0x0306
    (None,	X,	1,	"GL_ONE_MINUS_DST_COLOR"),	# 0x0307
    (None,	X,	1,	"GL_SRC_ALPHA_SATURATE"),	# 0x0308
    (None,	X,	1,	"GL_FRONT_LEFT"),	# 0x0400
    (None,	X,	1,	"GL_FRONT_RIGHT"),	# 0x0401
    (None,	X,	1,	"GL_BACK_LEFT"),	# 0x0402
    (None,	X,	1,	"GL_BACK_RIGHT"),	# 0x0403
    (None,	X,	1,	"GL_FRONT"),	# 0x0404
    (None,	X,	1,	"GL_BACK"),	# 0x0405
    (None,	X,	1,	"GL_LEFT"),	# 0x0406
    (None,	X,	1,	"GL_RIGHT"),	# 0x0407
    (None,	X,	1,	"GL_FRONT_AND_BACK"),	# 0x0408
    (None,	X,	1,	"GL_AUX0"),	# 0x0409
    (None,	X,	1,	"GL_AUX1"),	# 0x040A
    (None,	X,	1,	"GL_AUX2"),	# 0x040B
    (None,	X,	1,	"GL_AUX3"),	# 0x040C
    (None,	X,	1,	"GL_INVALID_ENUM"),	# 0x0500
    (None,	X,	1,	"GL_INVALID_VALUE"),	# 0x0501
    (None,	X,	1,	"GL_INVALID_OPERATION"),	# 0x0502
    (None,	X,	1,	"GL_STACK_OVERFLOW"),	# 0x0503
    (None,	X,	1,	"GL_STACK_UNDERFLOW"),	# 0x0504
    (None,	X,	1,	"GL_OUT_OF_MEMORY"),	# 0x0505
    (None,	X,	1,	"GL_INVALID_FRAMEBUFFER_OPERATION"),	# 0x0506
    (None,	X,	1,	"GL_2D"),	# 0x0600
    (None,	X,	1,	"GL_3D"),	# 0x0601
    (None,	X,	1,	"GL_3D_COLOR"),	# 0x0602
    (None,	X,	1,	"GL_3D_COLOR_TEXTURE"),	# 0x0603
    (None,	X,	1,	"GL_4D_COLOR_TEXTURE"),	# 0x0604
    (None,	X,	1,	"GL_PASS_THROUGH_TOKEN"),	# 0x0700
    (None,	X,	1,	"GL_POINT_TOKEN"),	# 0x0701
    (None,	X,	1,	"GL_LINE_TOKEN"),	# 0x0702
    (None,	X,	1,	"GL_POLYGON_TOKEN"),	# 0x0703
    (None,	X,	1,	"GL_BITMAP_TOKEN"),	# 0x0704
    (None,	X,	1,	"GL_DRAW_PIXEL_TOKEN"),	# 0x0705
    (None,	X,	1,	"GL_COPY_PIXEL_TOKEN"),	# 0x0706
    (None,	X,	1,	"GL_LINE_RESET_TOKEN"),	# 0x0707
    (None,	X,	1,	"GL_EXP"),	# 0x0800
    (None,	X,	1,	"GL_EXP2"),	# 0x0801
    (None,	X,	1,	"GL_CW"),	# 0x0900
    (None,	X,	1,	"GL_CCW"),	# 0x0901
    (None,	X,	1,	"GL_COEFF"),	# 0x0A00
    (None,	X,	1,	"GL_ORDER"),	# 0x0A01
    (None,	X,	1,	"GL_DOMAIN"),	# 0x0A02
    ("glGet",	F,	4,	"GL_CURRENT_COLOR"),	# 0x0B00
    ("glGet",	F,	1,	"GL_CURRENT_INDEX"),	# 0x0B01
    ("glGet",	F,	3,	"GL_CURRENT_NORMAL"),	# 0x0B02
    ("glGet",	F,	4,	"GL_CURRENT_TEXTURE_COORDS"),	# 0x0B03
    ("glGet",	F,	4,	"GL_CURRENT_RASTER_COLOR"),	# 0x0B04
    ("glGet",	F,	1,	"GL_CURRENT_RASTER_INDEX"),	# 0x0B05
    ("glGet",	F,	4,	"GL_CURRENT_RASTER_TEXTURE_COORDS"),	# 0x0B06
    ("glGet",	F,	4,	"GL_CURRENT_RASTER_POSITION"),	# 0x0B07
    ("glGet",	I,	1,	"GL_CURRENT_RASTER_POSITION_VALID"),	# 0x0B08
    ("glGet",	F,	1,	"GL_CURRENT_RASTER_DISTANCE"),	# 0x0B09
    ("glGet",	I,	1,	"GL_POINT_SMOOTH"),	# 0x0B10
    ("glGet",	F,	1,	"GL_POINT_SIZE"),	# 0x0B11
    ("glGet",	F,	2,	"GL_POINT_SIZE_RANGE"),	# 0x0B12
    ("glGet",	F,	1,	"GL_POINT_SIZE_GRANULARITY"),	# 0x0B13
    ("glGet",	I,	1,	"GL_LINE_SMOOTH"),	# 0x0B20
    ("glGet",	F,	1,	"GL_LINE_WIDTH"),	# 0x0B21
    ("glGet",	F,	2,	"GL_LINE_WIDTH_RANGE"),	# 0x0B22
    ("glGet",	F,	1,	"GL_LINE_WIDTH_GRANULARITY"),	# 0x0B23
    ("glGet",	I,	1,	"GL_LINE_STIPPLE"),	# 0x0B24
    ("glGet",	I,	1,	"GL_LINE_STIPPLE_PATTERN"),	# 0x0B25
    ("glGet",	I,	1,	"GL_LINE_STIPPLE_REPEAT"),	# 0x0B26
    ("glGet",	E,	1,	"GL_LIST_MODE"),	# 0x0B30
    ("glGet",	I,	1,	"GL_MAX_LIST_NESTING"),	# 0x0B31
    ("glGet",	I,	1,	"GL_LIST_BASE"),	# 0x0B32
    ("glGet",	I,	1,	"GL_LIST_INDEX"),	# 0x0B33
    ("glGet",	E,	2,	"GL_POLYGON_MODE"),	# 0x0B40
    ("glGet",	I,	1,	"GL_POLYGON_SMOOTH"),	# 0x0B41
    ("glGet",	I,	1,	"GL_POLYGON_STIPPLE"),	# 0x0B42
    ("glGet",	I,	1,	"GL_EDGE_FLAG"),	# 0x0B43
    ("glGet",	I,	1,	"GL_CULL_FACE"),	# 0x0B44
    ("glGet",	E,	1,	"GL_CULL_FACE_MODE"),	# 0x0B45
    ("glGet",	E,	1,	"GL_FRONT_FACE"),	# 0x0B46
    ("glGet",	I,	1,	"GL_LIGHTING"),	# 0x0B50
    ("glGet",	I,	1,	"GL_LIGHT_MODEL_LOCAL_VIEWER"),	# 0x0B51
    ("glGet",	I,	1,	"GL_LIGHT_MODEL_TWO_SIDE"),	# 0x0B52
    ("glGet",	F,	4,	"GL_LIGHT_MODEL_AMBIENT"),	# 0x0B53
    ("glGet",	E,	1,	"GL_SHADE_MODEL"),	# 0x0B54
    ("glGet",	E,	1,	"GL_COLOR_MATERIAL_FACE"),	# 0x0B55
    ("glGet",	E,	1,	"GL_COLOR_MATERIAL_PARAMETER"),	# 0x0B56
    ("glGet",	B,	1,	"GL_COLOR_MATERIAL"),	# 0x0B57
    ("glGet",	I,	1,	"GL_FOG"),	# 0x0B60
    ("glGet",	I,	1,	"GL_FOG_INDEX"),	# 0x0B61
    ("glGet",	F,	1,	"GL_FOG_DENSITY"),	# 0x0B62
    ("glGet",	F,	1,	"GL_FOG_START"),	# 0x0B63
    ("glGet",	F,	1,	"GL_FOG_END"),	# 0x0B64
    ("glGet",	E,	1,	"GL_FOG_MODE"),	# 0x0B65
    ("glGet",	F,	4,	"GL_FOG_COLOR"),	# 0x0B66
    ("glGet",	F,	2,	"GL_DEPTH_RANGE"),	# 0x0B70
    ("glGet",	I,	1,	"GL_DEPTH_TEST"),	# 0x0B71
    ("glGet",	I,	1,	"GL_DEPTH_WRITEMASK"),	# 0x0B72
    ("glGet",	F,	1,	"GL_DEPTH_CLEAR_VALUE"),	# 0x0B73
    ("glGet",	E,	1,	"GL_DEPTH_FUNC"),	# 0x0B74
    ("glGet",	F,	4,	"GL_ACCUM_CLEAR_VALUE"),	# 0x0B80
    ("glGet",	I,	1,	"GL_STENCIL_TEST"),	# 0x0B90
    ("glGet",	I,	1,	"GL_STENCIL_CLEAR_VALUE"),	# 0x0B91
    ("glGet",	E,	1,	"GL_STENCIL_FUNC"),	# 0x0B92
    ("glGet",	I,	1,	"GL_STENCIL_VALUE_MASK"),	# 0x0B93
    ("glGet",	E,	1,	"GL_STENCIL_FAIL"),	# 0x0B94
    ("glGet",	E,	1,	"GL_STENCIL_PASS_DEPTH_FAIL"),	# 0x0B95
    ("glGet",	E,	1,	"GL_STENCIL_PASS_DEPTH_PASS"),	# 0x0B96
    ("glGet",	I,	1,	"GL_STENCIL_REF"),	# 0x0B97
    ("glGet",	I,	1,	"GL_STENCIL_WRITEMASK"),	# 0x0B98
    ("glGet",	E,	1,	"GL_MATRIX_MODE"),	# 0x0BA0
    ("glGet",	I,	1,	"GL_NORMALIZE"),	# 0x0BA1
    ("glGet",	I,	4,	"GL_VIEWPORT"),	# 0x0BA2
    ("glGet",	I,	1,	"GL_MODELVIEW_STACK_DEPTH"),	# 0x0BA3
    ("glGet",	I,	1,	"GL_PROJECTION_STACK_DEPTH"),	# 0x0BA4
    ("glGet",	I,	1,	"GL_TEXTURE_STACK_DEPTH"),	# 0x0BA5
    ("glGet",	F,	16,	"GL_MODELVIEW_MATRIX"),	# 0x0BA6
    ("glGet",	F,	16,	"GL_PROJECTION_MATRIX"),	# 0x0BA7
    ("glGet",	F,	16,	"GL_TEXTURE_MATRIX"),	# 0x0BA8
    ("glGet",	I,	1,	"GL_ATTRIB_STACK_DEPTH"),	# 0x0BB0
    ("glGet",	I,	1,	"GL_CLIENT_ATTRIB_STACK_DEPTH"),	# 0x0BB1
    ("glGet",	B,	1,	"GL_ALPHA_TEST"),	# 0x0BC0
    ("glGet",	E,	1,	"GL_ALPHA_TEST_FUNC"),	# 0x0BC1
    ("glGet",	F,	1,	"GL_ALPHA_TEST_REF"),	# 0x0BC2
    ("glGet",	I,	1,	"GL_DITHER"),	# 0x0BD0
    ("glGet",	I,	1,	"GL_BLEND_DST"),	# 0x0BE0
    ("glGet",	I,	1,	"GL_BLEND_SRC"),	# 0x0BE1
    ("glGet",	B,	1,	"GL_BLEND"),	# 0x0BE2
    ("glGet",	E,	1,	"GL_LOGIC_OP_MODE"),	# 0x0BF0
    ("glGet",	I,	1,	"GL_LOGIC_OP"),	# 0x0BF1
    ("glGet",	I,	1,	"GL_COLOR_LOGIC_OP"),	# 0x0BF2
    ("glGet",	I,	1,	"GL_AUX_BUFFERS"),	# 0x0C00
    ("glGet",	E,	1,	"GL_DRAW_BUFFER"),	# 0x0C01
    ("glGet",	E,	1,	"GL_READ_BUFFER"),	# 0x0C02
    ("glGet",	I,	4,	"GL_SCISSOR_BOX"),	# 0x0C10
    ("glGet",	I,	1,	"GL_SCISSOR_TEST"),	# 0x0C11
    ("glGet",	I,	1,	"GL_INDEX_CLEAR_VALUE"),	# 0x0C20
    ("glGet",	I,	1,	"GL_INDEX_WRITEMASK"),	# 0x0C21
    ("glGet",	F,	4,	"GL_COLOR_CLEAR_VALUE"),	# 0x0C22
    ("glGet",	I,	4,	"GL_COLOR_WRITEMASK"),	# 0x0C23
    ("glGet",	I,	1,	"GL_INDEX_MODE"),	# 0x0C30
    ("glGet",	I,	1,	"GL_RGBA_MODE"),	# 0x0C31
    ("glGet",	I,	1,	"GL_DOUBLEBUFFER"),	# 0x0C32
    ("glGet",	I,	1,	"GL_STEREO"),	# 0x0C33
    ("glGet",	E,	1,	"GL_RENDER_MODE"),	# 0x0C40
    ("glGet",	E,	1,	"GL_PERSPECTIVE_CORRECTION_HINT"),	# 0x0C50
    ("glGet",	E,	1,	"GL_POINT_SMOOTH_HINT"),	# 0x0C51
    ("glGet",	E,	1,	"GL_LINE_SMOOTH_HINT"),	# 0x0C52
    ("glGet",	E,	1,	"GL_POLYGON_SMOOTH_HINT"),	# 0x0C53
    ("glGet",	E,	1,	"GL_FOG_HINT"),	# 0x0C54
    ("glGet",	I,	1,	"GL_TEXTURE_GEN_S"),	# 0x0C60
    ("glGet",	I,	1,	"GL_TEXTURE_GEN_T"),	# 0x0C61
    ("glGet",	I,	1,	"GL_TEXTURE_GEN_R"),	# 0x0C62
    ("glGet",	I,	1,	"GL_TEXTURE_GEN_Q"),	# 0x0C63
    ("glGet",	X,	1,	"GL_PIXEL_MAP_I_TO_I"),	# 0x0C70
    ("glGet",	X,	1,	"GL_PIXEL_MAP_S_TO_S"),	# 0x0C71
    ("glGet",	X,	1,	"GL_PIXEL_MAP_I_TO_R"),	# 0x0C72
    ("glGet",	X,	1,	"GL_PIXEL_MAP_I_TO_G"),	# 0x0C73
    ("glGet",	X,	1,	"GL_PIXEL_MAP_I_TO_B"),	# 0x0C74
    ("glGet",	X,	1,	"GL_PIXEL_MAP_I_TO_A"),	# 0x0C75
    ("glGet",	X,	1,	"GL_PIXEL_MAP_R_TO_R"),	# 0x0C76
    ("glGet",	X,	1,	"GL_PIXEL_MAP_G_TO_G"),	# 0x0C77
    ("glGet",	X,	1,	"GL_PIXEL_MAP_B_TO_B"),	# 0x0C78
    ("glGet",	X,	1,	"GL_PIXEL_MAP_A_TO_A"),	# 0x0C79
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_I_SIZE"),	# 0x0CB0
    ("glGet",	I,	1,	"GL_PIXEL_MAP_S_TO_S_SIZE"),	# 0x0CB1
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_R_SIZE"),	# 0x0CB2
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_G_SIZE"),	# 0x0CB3
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_B_SIZE"),	# 0x0CB4
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_A_SIZE"),	# 0x0CB5
    ("glGet",	I,	1,	"GL_PIXEL_MAP_R_TO_R_SIZE"),	# 0x0CB6
    ("glGet",	I,	1,	"GL_PIXEL_MAP_G_TO_G_SIZE"),	# 0x0CB7
    ("glGet",	I,	1,	"GL_PIXEL_MAP_B_TO_B_SIZE"),	# 0x0CB8
    ("glGet",	I,	1,	"GL_PIXEL_MAP_A_TO_A_SIZE"),	# 0x0CB9
    ("glGet",	I,	1,	"GL_UNPACK_SWAP_BYTES"),	# 0x0CF0
    ("glGet",	I,	1,	"GL_UNPACK_LSB_FIRST"),	# 0x0CF1
    ("glGet",	I,	1,	"GL_UNPACK_ROW_LENGTH"),	# 0x0CF2
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_ROWS"),	# 0x0CF3
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_PIXELS"),	# 0x0CF4
    ("glGet",	I,	1,	"GL_UNPACK_ALIGNMENT"),	# 0x0CF5
    ("glGet",	I,	1,	"GL_PACK_SWAP_BYTES"),	# 0x0D00
    ("glGet",	I,	1,	"GL_PACK_LSB_FIRST"),	# 0x0D01
    ("glGet",	I,	1,	"GL_PACK_ROW_LENGTH"),	# 0x0D02
    ("glGet",	I,	1,	"GL_PACK_SKIP_ROWS"),	# 0x0D03
    ("glGet",	I,	1,	"GL_PACK_SKIP_PIXELS"),	# 0x0D04
    ("glGet",	I,	1,	"GL_PACK_ALIGNMENT"),	# 0x0D05
    ("glGet",	I,	1,	"GL_MAP_COLOR"),	# 0x0D10
    ("glGet",	I,	1,	"GL_MAP_STENCIL"),	# 0x0D11
    ("glGet",	I,	1,	"GL_INDEX_SHIFT"),	# 0x0D12
    ("glGet",	I,	1,	"GL_INDEX_OFFSET"),	# 0x0D13
    ("glGet",	F,	1,	"GL_RED_SCALE"),	# 0x0D14
    ("glGet",	F,	1,	"GL_RED_BIAS"),	# 0x0D15
    ("glGet",	F,	1,	"GL_ZOOM_X"),	# 0x0D16
    ("glGet",	F,	1,	"GL_ZOOM_Y"),	# 0x0D17
    ("glGet",	F,	1,	"GL_GREEN_SCALE"),	# 0x0D18
    ("glGet",	F,	1,	"GL_GREEN_BIAS"),	# 0x0D19
    ("glGet",	F,	1,	"GL_BLUE_SCALE"),	# 0x0D1A
    ("glGet",	F,	1,	"GL_BLUE_BIAS"),	# 0x0D1B
    ("glGet",	F,	1,	"GL_ALPHA_SCALE"),	# 0x0D1C
    ("glGet",	F,	1,	"GL_ALPHA_BIAS"),	# 0x0D1D
    ("glGet",	F,	1,	"GL_DEPTH_SCALE"),	# 0x0D1E
    ("glGet",	F,	1,	"GL_DEPTH_BIAS"),	# 0x0D1F
    ("glGet",	I,	1,	"GL_MAX_EVAL_ORDER"),	# 0x0D30
    ("glGet",	I,	1,	"GL_MAX_LIGHTS"),	# 0x0D31
    ("glGet",	I,	1,	"GL_MAX_CLIP_PLANES"),	# 0x0D32
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_SIZE"),	# 0x0D33
    ("glGet",	I,	1,	"GL_MAX_PIXEL_MAP_TABLE"),	# 0x0D34
    ("glGet",	I,	1,	"GL_MAX_ATTRIB_STACK_DEPTH"),	# 0x0D35
    ("glGet",	I,	1,	"GL_MAX_MODELVIEW_STACK_DEPTH"),	# 0x0D36
    ("glGet",	I,	1,	"GL_MAX_NAME_STACK_DEPTH"),	# 0x0D37
    ("glGet",	I,	1,	"GL_MAX_PROJECTION_STACK_DEPTH"),	# 0x0D38
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_STACK_DEPTH"),	# 0x0D39
    ("glGet",	F,	2,	"GL_MAX_VIEWPORT_DIMS"),	# 0x0D3A
    ("glGet",	I,	1,	"GL_MAX_CLIENT_ATTRIB_STACK_DEPTH"),	# 0x0D3B
    ("glGet",	I,	1,	"GL_SUBPIXEL_BITS"),	# 0x0D50
    ("glGet",	I,	1,	"GL_INDEX_BITS"),	# 0x0D51
    ("glGet",	I,	1,	"GL_RED_BITS"),	# 0x0D52
    ("glGet",	I,	1,	"GL_GREEN_BITS"),	# 0x0D53
    ("glGet",	I,	1,	"GL_BLUE_BITS"),	# 0x0D54
    ("glGet",	I,	1,	"GL_ALPHA_BITS"),	# 0x0D55
    ("glGet",	I,	1,	"GL_DEPTH_BITS"),	# 0x0D56
    ("glGet",	I,	1,	"GL_STENCIL_BITS"),	# 0x0D57
    ("glGet",	I,	1,	"GL_ACCUM_RED_BITS"),	# 0x0D58
    ("glGet",	I,	1,	"GL_ACCUM_GREEN_BITS"),	# 0x0D59
    ("glGet",	I,	1,	"GL_ACCUM_BLUE_BITS"),	# 0x0D5A
    ("glGet",	I,	1,	"GL_ACCUM_ALPHA_BITS"),	# 0x0D5B
    ("glGet",	I,	1,	"GL_NAME_STACK_DEPTH"),	# 0x0D70
    ("glGet",	B,	1,	"GL_AUTO_NORMAL"),	# 0x0D80
    ("glGet",	B,	1,	"GL_MAP1_COLOR_4"),	# 0x0D90
    ("glGet",	B,	1,	"GL_MAP1_INDEX"),	# 0x0D91
    ("glGet",	B,	1,	"GL_MAP1_NORMAL"),	# 0x0D92
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_1"),	# 0x0D93
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_2"),	# 0x0D94
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_3"),	# 0x0D95
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_4"),	# 0x0D96
    ("glGet",	B,	1,	"GL_MAP1_VERTEX_3"),	# 0x0D97
    ("glGet",	B,	1,	"GL_MAP1_VERTEX_4"),	# 0x0D98
    ("glGet",	B,	1,	"GL_MAP2_COLOR_4"),	# 0x0DB0
    ("glGet",	B,	1,	"GL_MAP2_INDEX"),	# 0x0DB1
    ("glGet",	B,	1,	"GL_MAP2_NORMAL"),	# 0x0DB2
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_1"),	# 0x0DB3
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_2"),	# 0x0DB4
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_3"),	# 0x0DB5
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_4"),	# 0x0DB6
    ("glGet",	B,	1,	"GL_MAP2_VERTEX_3"),	# 0x0DB7
    ("glGet",	B,	1,	"GL_MAP2_VERTEX_4"),	# 0x0DB8
    ("glGet",	F,	2,	"GL_MAP1_GRID_DOMAIN"),	# 0x0DD0
    ("glGet",	I,	1,	"GL_MAP1_GRID_SEGMENTS"),	# 0x0DD1
    ("glGet",	F,	4,	"GL_MAP2_GRID_DOMAIN"),	# 0x0DD2
    ("glGet",	I,	2,	"GL_MAP2_GRID_SEGMENTS"),	# 0x0DD3
    ("glGet",	B,	1,	"GL_TEXTURE_1D"),	# 0x0DE0
    ("glGet",	B,	1,	"GL_TEXTURE_2D"),	# 0x0DE1
    ("glGet",	P,	1,	"GL_FEEDBACK_BUFFER_POINTER"),	# 0x0DF0
    ("glGet",	I,	1,	"GL_FEEDBACK_BUFFER_SIZE"),	# 0x0DF1
    ("glGet",	E,	1,	"GL_FEEDBACK_BUFFER_TYPE"),	# 0x0DF2
    ("glGet",	P,	1,	"GL_SELECTION_BUFFER_POINTER"),	# 0x0DF3
    ("glGet",	I,	1,	"GL_SELECTION_BUFFER_SIZE"),	# 0x0DF4
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_WIDTH"),	# 0x1000
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_HEIGHT"),	# 0x1001
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_INTERNAL_FORMAT"),	# 0x1003
    ("glGetTexParameter",	F,	4,	"GL_TEXTURE_BORDER_COLOR"),	# 0x1004
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_BORDER"),	# 0x1005
    ("glGet",	X,	1,	"GL_DONT_CARE"),	# 0x1100
    ("glGet",	X,	1,	"GL_FASTEST"),	# 0x1101
    ("glGet",	X,	1,	"GL_NICEST"),	# 0x1102
    ("glGet",	X,	1,	"GL_AMBIENT"),	# 0x1200
    ("glGet",	X,	1,	"GL_DIFFUSE"),	# 0x1201
    ("glGet",	X,	1,	"GL_SPECULAR"),	# 0x1202
    ("glGet",	X,	1,	"GL_POSITION"),	# 0x1203
    ("glGet",	X,	1,	"GL_SPOT_DIRECTION"),	# 0x1204
    ("glGet",	X,	1,	"GL_SPOT_EXPONENT"),	# 0x1205
    ("glGet",	X,	1,	"GL_SPOT_CUTOFF"),	# 0x1206
    ("glGet",	X,	1,	"GL_CONSTANT_ATTENUATION"),	# 0x1207
    ("glGet",	X,	1,	"GL_LINEAR_ATTENUATION"),	# 0x1208
    ("glGet",	X,	1,	"GL_QUADRATIC_ATTENUATION"),	# 0x1209
    ("glGet",	X,	1,	"GL_COMPILE"),	# 0x1300
    ("glGet",	X,	1,	"GL_COMPILE_AND_EXECUTE"),	# 0x1301
    ("glGet",	X,	1,	"GL_BYTE"),	# 0x1400
    ("glGet",	X,	1,	"GL_UNSIGNED_BYTE"),	# 0x1401
    ("glGet",	X,	1,	"GL_SHORT"),	# 0x1402
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT"),	# 0x1403
    ("glGet",	X,	1,	"GL_INT"),	# 0x1404
    ("glGet",	X,	1,	"GL_UNSIGNED_INT"),	# 0x1405
    ("glGet",	X,	1,	"GL_FLOAT"),	# 0x1406
    ("glGet",	X,	1,	"GL_2_BYTES"),	# 0x1407
    ("glGet",	X,	1,	"GL_3_BYTES"),	# 0x1408
    ("glGet",	X,	1,	"GL_4_BYTES"),	# 0x1409
    ("glGet",	X,	1,	"GL_DOUBLE"),	# 0x140A
    ("glGet",	X,	1,	"GL_HALF_FLOAT"),	# 0x140B
    ("glGet",	X,	1,	"GL_FIXED"),	# 0x140C
    ("glGet",	X,	1,	"GL_INT64_NV"),	# 0x140E
    ("glGet",	X,	1,	"GL_UNSIGNED_INT64_NV"),	# 0x140F
    ("glGet",	X,	1,	"GL_CLEAR"),	# 0x1500
    ("glGet",	X,	1,	"GL_AND"),	# 0x1501
    ("glGet",	X,	1,	"GL_AND_REVERSE"),	# 0x1502
    ("glGet",	X,	1,	"GL_COPY"),	# 0x1503
    ("glGet",	X,	1,	"GL_AND_INVERTED"),	# 0x1504
    ("glGet",	X,	1,	"GL_NOOP"),	# 0x1505
    ("glGet",	X,	1,	"GL_XOR"),	# 0x1506
    ("glGet",	X,	1,	"GL_OR"),	# 0x1507
    ("glGet",	X,	1,	"GL_NOR"),	# 0x1508
    ("glGet",	X,	1,	"GL_EQUIV"),	# 0x1509
    ("glGet",	X,	1,	"GL_INVERT"),	# 0x150A
    ("glGet",	X,	1,	"GL_OR_REVERSE"),	# 0x150B
    ("glGet",	X,	1,	"GL_COPY_INVERTED"),	# 0x150C
    ("glGet",	X,	1,	"GL_OR_INVERTED"),	# 0x150D
    ("glGet",	X,	1,	"GL_NAND"),	# 0x150E
    ("glGet",	X,	1,	"GL_SET"),	# 0x150F
    ("glGet",	X,	1,	"GL_EMISSION"),	# 0x1600
    ("glGet",	X,	1,	"GL_SHININESS"),	# 0x1601
    ("glGet",	X,	1,	"GL_AMBIENT_AND_DIFFUSE"),	# 0x1602
    ("glGet",	X,	1,	"GL_COLOR_INDEXES"),	# 0x1603
    ("glGet",	X,	1,	"GL_MODELVIEW"),	# 0x1700
    ("glGet",	X,	1,	"GL_PROJECTION"),	# 0x1701
    ("glGet",	X,	1,	"GL_TEXTURE"),	# 0x1702
    ("glGet",	X,	1,	"GL_COLOR"),	# 0x1800
    ("glGet",	X,	1,	"GL_DEPTH"),	# 0x1801
    ("glGet",	X,	1,	"GL_STENCIL"),	# 0x1802
    ("glGet",	X,	1,	"GL_COLOR_INDEX"),	# 0x1900
    ("glGet",	X,	1,	"GL_STENCIL_INDEX"),	# 0x1901
    ("glGet",	X,	1,	"GL_DEPTH_COMPONENT"),	# 0x1902
    ("glGet",	X,	1,	"GL_RED"),	# 0x1903
    ("glGet",	X,	1,	"GL_GREEN"),	# 0x1904
    ("glGet",	X,	1,	"GL_BLUE"),	# 0x1905
    ("glGet",	X,	1,	"GL_ALPHA"),	# 0x1906
    ("glGet",	X,	1,	"GL_RGB"),	# 0x1907
    ("glGet",	X,	1,	"GL_RGBA"),	# 0x1908
    ("glGet",	X,	1,	"GL_LUMINANCE"),	# 0x1909
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA"),	# 0x190A
    ("glGet",	X,	1,	"GL_RASTER_POSITION_UNCLIPPED_IBM"),	# 0x19262
    ("glGet",	X,	1,	"GL_BITMAP"),	# 0x1A00
    ("glGet",	X,	1,	"GL_PREFER_DOUBLEBUFFER_HINT_PGI"),	# 0x1A1F8
    ("glGet",	X,	1,	"GL_CONSERVE_MEMORY_HINT_PGI"),	# 0x1A1FD
    ("glGet",	X,	1,	"GL_RECLAIM_MEMORY_HINT_PGI"),	# 0x1A1FE
    ("glGet",	X,	1,	"GL_NATIVE_GRAPHICS_HANDLE_PGI"),	# 0x1A202
    ("glGet",	X,	1,	"GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI"),	# 0x1A203
    ("glGet",	X,	1,	"GL_NATIVE_GRAPHICS_END_HINT_PGI"),	# 0x1A204
    ("glGet",	X,	1,	"GL_ALWAYS_FAST_HINT_PGI"),	# 0x1A20C
    ("glGet",	X,	1,	"GL_ALWAYS_SOFT_HINT_PGI"),	# 0x1A20D
    ("glGet",	X,	1,	"GL_ALLOW_DRAW_OBJ_HINT_PGI"),	# 0x1A20E
    ("glGet",	X,	1,	"GL_ALLOW_DRAW_WIN_HINT_PGI"),	# 0x1A20F
    ("glGet",	X,	1,	"GL_ALLOW_DRAW_FRG_HINT_PGI"),	# 0x1A210
    ("glGet",	X,	1,	"GL_ALLOW_DRAW_MEM_HINT_PGI"),	# 0x1A211
    ("glGet",	X,	1,	"GL_STRICT_DEPTHFUNC_HINT_PGI"),	# 0x1A216
    ("glGet",	X,	1,	"GL_STRICT_LIGHTING_HINT_PGI"),	# 0x1A217
    ("glGet",	X,	1,	"GL_STRICT_SCISSOR_HINT_PGI"),	# 0x1A218
    ("glGet",	X,	1,	"GL_FULL_STIPPLE_HINT_PGI"),	# 0x1A219
    ("glGet",	X,	1,	"GL_CLIP_NEAR_HINT_PGI"),	# 0x1A220
    ("glGet",	X,	1,	"GL_CLIP_FAR_HINT_PGI"),	# 0x1A221
    ("glGet",	X,	1,	"GL_WIDE_LINE_HINT_PGI"),	# 0x1A222
    ("glGet",	X,	1,	"GL_BACK_NORMALS_HINT_PGI"),	# 0x1A223
    ("glGet",	X,	1,	"GL_VERTEX_DATA_HINT_PGI"),	# 0x1A22A
    ("glGet",	X,	1,	"GL_VERTEX_CONSISTENT_HINT_PGI"),	# 0x1A22B
    ("glGet",	X,	1,	"GL_MATERIAL_SIDE_HINT_PGI"),	# 0x1A22C
    ("glGet",	X,	1,	"GL_MAX_VERTEX_HINT_PGI"),	# 0x1A22D
    ("glGet",	X,	1,	"GL_POINT"),	# 0x1B00
    ("glGet",	X,	1,	"GL_LINE"),	# 0x1B01
    ("glGet",	X,	1,	"GL_FILL"),	# 0x1B02
    ("glGet",	X,	1,	"GL_RENDER"),	# 0x1C00
    ("glGet",	X,	1,	"GL_FEEDBACK"),	# 0x1C01
    ("glGet",	X,	1,	"GL_SELECT"),	# 0x1C02
    ("glGet",	X,	1,	"GL_FLAT"),	# 0x1D00
    ("glGet",	X,	1,	"GL_SMOOTH"),	# 0x1D01
    ("glGet",	X,	1,	"GL_KEEP"),	# 0x1E00
    ("glGet",	X,	1,	"GL_REPLACE"),	# 0x1E01
    ("glGet",	X,	1,	"GL_INCR"),	# 0x1E02
    ("glGet",	X,	1,	"GL_DECR"),	# 0x1E03
    ("glGet",	S,	1,	"GL_VENDOR"),	# 0x1F00
    ("glGet",	S,	1,	"GL_RENDERER"),	# 0x1F01
    ("glGet",	S,	1,	"GL_VERSION"),	# 0x1F02
    ("glGet",	S,	1,	"GL_EXTENSIONS"),	# 0x1F03
    ("glGet",	X,	1,	"GL_S"),	# 0x2000
    ("glGet",	X,	1,	"GL_T"),	# 0x2001
    ("glGet",	X,	1,	"GL_R"),	# 0x2002
    ("glGet",	X,	1,	"GL_Q"),	# 0x2003
    ("glGet",	X,	1,	"GL_MODULATE"),	# 0x2100
    ("glGet",	X,	1,	"GL_DECAL"),	# 0x2101
    ("glGet",	X,	1,	"GL_TEXTURE_ENV_MODE"),	# 0x2200
    ("glGet",	X,	1,	"GL_TEXTURE_ENV_COLOR"),	# 0x2201
    ("glGet",	X,	1,	"GL_TEXTURE_ENV"),	# 0x2300
    ("glGet",	X,	1,	"GL_EYE_LINEAR"),	# 0x2400
    ("glGet",	X,	1,	"GL_OBJECT_LINEAR"),	# 0x2401
    ("glGet",	X,	1,	"GL_SPHERE_MAP"),	# 0x2402
    ("glGet",	X,	1,	"GL_TEXTURE_GEN_MODE"),	# 0x2500
    ("glGet",	X,	1,	"GL_OBJECT_PLANE"),	# 0x2501
    ("glGet",	X,	1,	"GL_EYE_PLANE"),	# 0x2502
    ("glGet",	X,	1,	"GL_NEAREST"),	# 0x2600
    ("glGet",	X,	1,	"GL_LINEAR"),	# 0x2601
    ("glGet",	X,	1,	"GL_NEAREST_MIPMAP_NEAREST"),	# 0x2700
    ("glGet",	X,	1,	"GL_LINEAR_MIPMAP_NEAREST"),	# 0x2701
    ("glGet",	X,	1,	"GL_NEAREST_MIPMAP_LINEAR"),	# 0x2702
    ("glGet",	X,	1,	"GL_LINEAR_MIPMAP_LINEAR"),	# 0x2703
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_MAG_FILTER"),	# 0x2800
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_MIN_FILTER"),	# 0x2801
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_WRAP_S"),	# 0x2802
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_WRAP_T"),	# 0x2803
    ("glGet",	X,	1,	"GL_CLAMP"),	# 0x2900
    ("glGet",	X,	1,	"GL_REPEAT"),	# 0x2901
    ("glGet",	F,	1,	"GL_POLYGON_OFFSET_UNITS"),	# 0x2A00
    ("glGet",	I,	1,	"GL_POLYGON_OFFSET_POINT"),	# 0x2A01
    ("glGet",	I,	1,	"GL_POLYGON_OFFSET_LINE"),	# 0x2A02
    ("glGet",	X,	1,	"GL_R3_G3_B2"),	# 0x2A10
    ("glGet",	X,	1,	"GL_V2F"),	# 0x2A20
    ("glGet",	X,	1,	"GL_V3F"),	# 0x2A21
    ("glGet",	X,	1,	"GL_C4UB_V2F"),	# 0x2A22
    ("glGet",	X,	1,	"GL_C4UB_V3F"),	# 0x2A23
    ("glGet",	X,	1,	"GL_C3F_V3F"),	# 0x2A24
    ("glGet",	X,	1,	"GL_N3F_V3F"),	# 0x2A25
    ("glGet",	X,	1,	"GL_C4F_N3F_V3F"),	# 0x2A26
    ("glGet",	X,	1,	"GL_T2F_V3F"),	# 0x2A27
    ("glGet",	X,	1,	"GL_T4F_V4F"),	# 0x2A28
    ("glGet",	X,	1,	"GL_T2F_C4UB_V3F"),	# 0x2A29
    ("glGet",	X,	1,	"GL_T2F_C3F_V3F"),	# 0x2A2A
    ("glGet",	X,	1,	"GL_T2F_N3F_V3F"),	# 0x2A2B
    ("glGet",	X,	1,	"GL_T2F_C4F_N3F_V3F"),	# 0x2A2C
    ("glGet",	X,	1,	"GL_T4F_C4F_N3F_V4F"),	# 0x2A2D
    ("glGet",	B,	1,	"GL_CLIP_PLANE0"),	# 0x3000
    ("glGet",	B,	1,	"GL_CLIP_PLANE1"),	# 0x3001
    ("glGet",	B,	1,	"GL_CLIP_PLANE2"),	# 0x3002
    ("glGet",	B,	1,	"GL_CLIP_PLANE3"),	# 0x3003
    ("glGet",	B,	1,	"GL_CLIP_PLANE4"),	# 0x3004
    ("glGet",	B,	1,	"GL_CLIP_PLANE5"),	# 0x3005
    ("glGet",	X,	1,	"GL_CLIP_DISTANCE6"),	# 0x3006
    ("glGet",	X,	1,	"GL_CLIP_DISTANCE7"),	# 0x3007
    ("glGet",	I,	1,	"GL_LIGHT0"),	# 0x4000
    ("glGet",	I,	1,	"GL_LIGHT1"),	# 0x4001
    ("glGet",	I,	1,	"GL_LIGHT2"),	# 0x4002
    ("glGet",	I,	1,	"GL_LIGHT3"),	# 0x4003
    ("glGet",	I,	1,	"GL_LIGHT4"),	# 0x4004
    ("glGet",	I,	1,	"GL_LIGHT5"),	# 0x4005
    ("glGet",	I,	1,	"GL_LIGHT6"),	# 0x4006
    ("glGet",	I,	1,	"GL_LIGHT7"),	# 0x4007
    ("glGet",	X,	1,	"GL_ABGR_EXT"),	# 0x8000
    ("glGet",	X,	1,	"GL_CONSTANT_COLOR"),	# 0x8001
    ("glGet",	X,	1,	"GL_ONE_MINUS_CONSTANT_COLOR"),	# 0x8002
    ("glGet",	X,	1,	"GL_CONSTANT_ALPHA"),	# 0x8003
    ("glGet",	X,	1,	"GL_ONE_MINUS_CONSTANT_ALPHA"),	# 0x8004
    ("glGet",	F,	4,	"GL_BLEND_COLOR"),	# 0x8005
    ("glGet",	X,	1,	"GL_FUNC_ADD"),	# 0x8006
    ("glGet",	X,	1,	"GL_MIN"),	# 0x8007
    ("glGet",	X,	1,	"GL_MAX"),	# 0x8008
    ("glGet",	E,	1,	"GL_BLEND_EQUATION"),	# 0x8009
    ("glGet",	X,	1,	"GL_FUNC_SUBTRACT"),	# 0x800A
    ("glGet",	X,	1,	"GL_FUNC_REVERSE_SUBTRACT"),	# 0x800B
    ("glGet",	X,	1,	"GL_CMYK_EXT"),	# 0x800C
    ("glGet",	X,	1,	"GL_CMYKA_EXT"),	# 0x800D
    ("glGet",	I,	1,	"GL_PACK_CMYK_HINT_EXT"),	# 0x800E
    ("glGet",	I,	1,	"GL_UNPACK_CMYK_HINT_EXT"),	# 0x800F
    ("glGet",	I,	1,	"GL_CONVOLUTION_1D"),	# 0x8010
    ("glGet",	I,	1,	"GL_CONVOLUTION_2D"),	# 0x8011
    ("glGet",	I,	1,	"GL_SEPARABLE_2D"),	# 0x8012
    ("glGet",	X,	1,	"GL_CONVOLUTION_BORDER_MODE"),	# 0x8013
    ("glGet",	X,	1,	"GL_CONVOLUTION_FILTER_SCALE"),	# 0x8014
    ("glGet",	X,	1,	"GL_CONVOLUTION_FILTER_BIAS"),	# 0x8015
    ("glGet",	X,	1,	"GL_REDUCE"),	# 0x8016
    ("glGet",	X,	1,	"GL_CONVOLUTION_FORMAT"),	# 0x8017
    ("glGet",	X,	1,	"GL_CONVOLUTION_WIDTH"),	# 0x8018
    ("glGet",	X,	1,	"GL_CONVOLUTION_HEIGHT"),	# 0x8019
    ("glGet",	X,	1,	"GL_MAX_CONVOLUTION_WIDTH"),	# 0x801A
    ("glGet",	X,	1,	"GL_MAX_CONVOLUTION_HEIGHT"),	# 0x801B
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_RED_SCALE"),	# 0x801C
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_GREEN_SCALE"),	# 0x801D
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_BLUE_SCALE"),	# 0x801E
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_ALPHA_SCALE"),	# 0x801F
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_RED_BIAS"),	# 0x8020
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_GREEN_BIAS"),	# 0x8021
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_BLUE_BIAS"),	# 0x8022
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_ALPHA_BIAS"),	# 0x8023
    ("glGet",	I,	1,	"GL_HISTOGRAM"),	# 0x8024
    ("glGet",	X,	1,	"GL_PROXY_HISTOGRAM"),	# 0x8025
    ("glGet",	X,	1,	"GL_HISTOGRAM_WIDTH"),	# 0x8026
    ("glGet",	X,	1,	"GL_HISTOGRAM_FORMAT"),	# 0x8027
    ("glGet",	X,	1,	"GL_HISTOGRAM_RED_SIZE"),	# 0x8028
    ("glGet",	X,	1,	"GL_HISTOGRAM_GREEN_SIZE"),	# 0x8029
    ("glGet",	X,	1,	"GL_HISTOGRAM_BLUE_SIZE"),	# 0x802A
    ("glGet",	X,	1,	"GL_HISTOGRAM_ALPHA_SIZE"),	# 0x802B
    ("glGet",	X,	1,	"GL_HISTOGRAM_LUMINANCE_SIZE"),	# 0x802C
    ("glGet",	X,	1,	"GL_HISTOGRAM_SINK"),	# 0x802D
    ("glGet",	I,	1,	"GL_MINMAX"),	# 0x802E
    ("glGet",	X,	1,	"GL_MINMAX_FORMAT"),	# 0x802F
    ("glGet",	X,	1,	"GL_MINMAX_SINK"),	# 0x8030
    ("glGet",	X,	1,	"GL_TABLE_TOO_LARGE"),	# 0x8031
    ("glGet",	X,	1,	"GL_UNSIGNED_BYTE_3_3_2"),	# 0x8032
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_4_4_4_4"),	# 0x8033
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_5_5_5_1"),	# 0x8034
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_8_8_8_8"),	# 0x8035
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_10_10_10_2"),	# 0x8036
    ("glGet",	I,	1,	"GL_POLYGON_OFFSET_FILL"),	# 0x8037
    ("glGet",	F,	1,	"GL_POLYGON_OFFSET_FACTOR"),	# 0x8038
    ("glGet",	F,	1,	"GL_POLYGON_OFFSET_BIAS_EXT"),	# 0x8039
    ("glGet",	I,	1,	"GL_RESCALE_NORMAL"),	# 0x803A
    ("glGet",	X,	1,	"GL_ALPHA4"),	# 0x803B
    ("glGet",	X,	1,	"GL_ALPHA8"),	# 0x803C
    ("glGet",	X,	1,	"GL_ALPHA12"),	# 0x803D
    ("glGet",	X,	1,	"GL_ALPHA16"),	# 0x803E
    ("glGet",	X,	1,	"GL_LUMINANCE4"),	# 0x803F
    ("glGet",	X,	1,	"GL_LUMINANCE8"),	# 0x8040
    ("glGet",	X,	1,	"GL_LUMINANCE12"),	# 0x8041
    ("glGet",	X,	1,	"GL_LUMINANCE16"),	# 0x8042
    ("glGet",	X,	1,	"GL_LUMINANCE4_ALPHA4"),	# 0x8043
    ("glGet",	X,	1,	"GL_LUMINANCE6_ALPHA2"),	# 0x8044
    ("glGet",	X,	1,	"GL_LUMINANCE8_ALPHA8"),	# 0x8045
    ("glGet",	X,	1,	"GL_LUMINANCE12_ALPHA4"),	# 0x8046
    ("glGet",	X,	1,	"GL_LUMINANCE12_ALPHA12"),	# 0x8047
    ("glGet",	X,	1,	"GL_LUMINANCE16_ALPHA16"),	# 0x8048
    ("glGet",	X,	1,	"GL_INTENSITY"),	# 0x8049
    ("glGet",	X,	1,	"GL_INTENSITY4"),	# 0x804A
    ("glGet",	X,	1,	"GL_INTENSITY8"),	# 0x804B
    ("glGet",	X,	1,	"GL_INTENSITY12"),	# 0x804C
    ("glGet",	X,	1,	"GL_INTENSITY16"),	# 0x804D
    ("glGet",	X,	1,	"GL_RGB2_EXT"),	# 0x804E
    ("glGet",	X,	1,	"GL_RGB4"),	# 0x804F
    ("glGet",	X,	1,	"GL_RGB5"),	# 0x8050
    ("glGet",	X,	1,	"GL_RGB8"),	# 0x8051
    ("glGet",	X,	1,	"GL_RGB10"),	# 0x8052
    ("glGet",	X,	1,	"GL_RGB12"),	# 0x8053
    ("glGet",	X,	1,	"GL_RGB16"),	# 0x8054
    ("glGet",	X,	1,	"GL_RGBA2"),	# 0x8055
    ("glGet",	X,	1,	"GL_RGBA4"),	# 0x8056
    ("glGet",	X,	1,	"GL_RGB5_A1"),	# 0x8057
    ("glGet",	X,	1,	"GL_RGBA8"),	# 0x8058
    ("glGet",	X,	1,	"GL_RGB10_A2"),	# 0x8059
    ("glGet",	X,	1,	"GL_RGBA12"),	# 0x805A
    ("glGet",	X,	1,	"GL_RGBA16"),	# 0x805B
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_RED_SIZE"),	# 0x805C
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_GREEN_SIZE"),	# 0x805D
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_BLUE_SIZE"),	# 0x805E
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_ALPHA_SIZE"),	# 0x805F
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_LUMINANCE_SIZE"),	# 0x8060
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_INTENSITY_SIZE"),	# 0x8061
    ("glGet",	X,	1,	"GL_REPLACE_EXT"),	# 0x8062
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_1D"),	# 0x8063
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_2D"),	# 0x8064
    ("glGet",	X,	1,	"GL_TEXTURE_TOO_LARGE_EXT"),	# 0x8065
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_PRIORITY"),	# 0x8066
    ("glGetTexParameter",	B,	1,	"GL_TEXTURE_RESIDENT"),	# 0x8067
    ("glGet",	I,	1,	"GL_TEXTURE_BINDING_1D"),	# 0x8068
    ("glGet",	I,	1,	"GL_TEXTURE_BINDING_2D"),	# 0x8069
    ("glGet",	I,	1,	"GL_TEXTURE_BINDING_3D"),	# 0x806A
    ("glGet",	I,	1,	"GL_PACK_SKIP_IMAGES"),	# 0x806B
    ("glGet",	F,	1,	"GL_PACK_IMAGE_HEIGHT"),	# 0x806C
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_IMAGES"),	# 0x806D
    ("glGet",	F,	1,	"GL_UNPACK_IMAGE_HEIGHT"),	# 0x806E
    ("glGet",	B,	1,	"GL_TEXTURE_3D"),	# 0x806F
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_3D"),	# 0x8070
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_DEPTH"),	# 0x8071
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_WRAP_R"),	# 0x8072
    ("glGet",	I,	1,	"GL_MAX_3D_TEXTURE_SIZE"),	# 0x8073
    ("glGet",	B,	1,	"GL_VERTEX_ARRAY"),	# 0x8074
    ("glGet",	B,	1,	"GL_NORMAL_ARRAY"),	# 0x8075
    ("glGet",	B,	1,	"GL_COLOR_ARRAY"),	# 0x8076
    ("glGet",	B,	1,	"GL_INDEX_ARRAY"),	# 0x8077
    ("glGet",	B,	1,	"GL_TEXTURE_COORD_ARRAY"),	# 0x8078
    ("glGet",	B,	1,	"GL_EDGE_FLAG_ARRAY"),	# 0x8079
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_SIZE"),	# 0x807A
    ("glGet",	E,	1,	"GL_VERTEX_ARRAY_TYPE"),	# 0x807B
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_STRIDE"),	# 0x807C
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_COUNT_EXT"),	# 0x807D
    ("glGet",	E,	1,	"GL_NORMAL_ARRAY_TYPE"),	# 0x807E
    ("glGet",	I,	1,	"GL_NORMAL_ARRAY_STRIDE"),	# 0x807F
    ("glGet",	I,	1,	"GL_NORMAL_ARRAY_COUNT_EXT"),	# 0x8080
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_SIZE"),	# 0x8081
    ("glGet",	E,	1,	"GL_COLOR_ARRAY_TYPE"),	# 0x8082
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_STRIDE"),	# 0x8083
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_COUNT_EXT"),	# 0x8084
    ("glGet",	E,	1,	"GL_INDEX_ARRAY_TYPE"),	# 0x8085
    ("glGet",	I,	1,	"GL_INDEX_ARRAY_STRIDE"),	# 0x8086
    ("glGet",	I,	1,	"GL_INDEX_ARRAY_COUNT_EXT"),	# 0x8087
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_SIZE"),	# 0x8088
    ("glGet",	E,	1,	"GL_TEXTURE_COORD_ARRAY_TYPE"),	# 0x8089
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_STRIDE"),	# 0x808A
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_COUNT_EXT"),	# 0x808B
    ("glGet",	I,	1,	"GL_EDGE_FLAG_ARRAY_STRIDE"),	# 0x808C
    ("glGet",	I,	1,	"GL_EDGE_FLAG_ARRAY_COUNT_EXT"),	# 0x808D
    ("glGet",	P,	1,	"GL_VERTEX_ARRAY_POINTER"),	# 0x808E
    ("glGet",	P,	1,	"GL_NORMAL_ARRAY_POINTER"),	# 0x808F
    ("glGet",	P,	1,	"GL_COLOR_ARRAY_POINTER"),	# 0x8090
    ("glGet",	P,	1,	"GL_INDEX_ARRAY_POINTER"),	# 0x8091
    ("glGet",	P,	1,	"GL_TEXTURE_COORD_ARRAY_POINTER"),	# 0x8092
    ("glGet",	P,	1,	"GL_EDGE_FLAG_ARRAY_POINTER"),	# 0x8093
    ("glGet",	I,	1,	"GL_INTERLACE_SGIX"),	# 0x8094
    ("glGet",	X,	1,	"GL_DETAIL_TEXTURE_2D_SGIS"),	# 0x8095
    ("glGet",	I,	1,	"GL_DETAIL_TEXTURE_2D_BINDING_SGIS"),	# 0x8096
    ("glGet",	X,	1,	"GL_LINEAR_DETAIL_SGIS"),	# 0x8097
    ("glGet",	X,	1,	"GL_LINEAR_DETAIL_ALPHA_SGIS"),	# 0x8098
    ("glGet",	X,	1,	"GL_LINEAR_DETAIL_COLOR_SGIS"),	# 0x8099
    ("glGet",	X,	1,	"GL_DETAIL_TEXTURE_LEVEL_SGIS"),	# 0x809A
    ("glGet",	X,	1,	"GL_DETAIL_TEXTURE_MODE_SGIS"),	# 0x809B
    ("glGet",	X,	1,	"GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS"),	# 0x809C
    ("glGet",	I,	1,	"GL_MULTISAMPLE"),	# 0x809D
    ("glGet",	I,	1,	"GL_SAMPLE_ALPHA_TO_COVERAGE"),	# 0x809E
    ("glGet",	I,	1,	"GL_SAMPLE_ALPHA_TO_ONE"),	# 0x809F
    ("glGet",	I,	1,	"GL_SAMPLE_COVERAGE"),	# 0x80A0
    ("glGet",	X,	1,	"GL_1PASS_EXT"),	# 0x80A1
    ("glGet",	X,	1,	"GL_2PASS_0_EXT"),	# 0x80A2
    ("glGet",	X,	1,	"GL_2PASS_1_EXT"),	# 0x80A3
    ("glGet",	X,	1,	"GL_4PASS_0_EXT"),	# 0x80A4
    ("glGet",	X,	1,	"GL_4PASS_1_EXT"),	# 0x80A5
    ("glGet",	X,	1,	"GL_4PASS_2_EXT"),	# 0x80A6
    ("glGet",	X,	1,	"GL_4PASS_3_EXT"),	# 0x80A7
    ("glGet",	I,	1,	"GL_SAMPLE_BUFFERS"),	# 0x80A8
    ("glGet",	I,	1,	"GL_SAMPLES"),	# 0x80A9
    ("glGet",	F,	1,	"GL_SAMPLE_COVERAGE_VALUE"),	# 0x80AA
    ("glGet",	I,	1,	"GL_SAMPLE_COVERAGE_INVERT"),	# 0x80AB
    ("glGet",	I,	1,	"GL_SAMPLE_PATTERN_EXT"),	# 0x80AC
    ("glGet",	X,	1,	"GL_LINEAR_SHARPEN_SGIS"),	# 0x80AD
    ("glGet",	X,	1,	"GL_LINEAR_SHARPEN_ALPHA_SGIS"),	# 0x80AE
    ("glGet",	X,	1,	"GL_LINEAR_SHARPEN_COLOR_SGIS"),	# 0x80AF
    ("glGet",	X,	1,	"GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS"),	# 0x80B0
    ("glGet",	F,	16,	"GL_COLOR_MATRIX"),	# 0x80B1
    ("glGet",	I,	1,	"GL_COLOR_MATRIX_STACK_DEPTH"),	# 0x80B2
    ("glGet",	I,	1,	"GL_MAX_COLOR_MATRIX_STACK_DEPTH"),	# 0x80B3
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_RED_SCALE"),	# 0x80B4
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_GREEN_SCALE"),	# 0x80B5
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_BLUE_SCALE"),	# 0x80B6
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_ALPHA_SCALE"),	# 0x80B7
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_RED_BIAS"),	# 0x80B8
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_GREEN_BIAS"),	# 0x80B9
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_BLUE_BIAS"),	# 0x80BA
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_ALPHA_BIAS"),	# 0x80BB
    ("glGet",	I,	1,	"GL_TEXTURE_COLOR_TABLE_SGI"),	# 0x80BC
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_COLOR_TABLE_SGI"),	# 0x80BD
    ("glGet",	X,	1,	"GL_TEXTURE_ENV_BIAS_SGIX"),	# 0x80BE
    ("glGet",	X,	1,	"GL_TEXTURE_COMPARE_FAIL_VALUE_ARB"),	# 0x80BF
    ("glGet",	X,	1,	"GL_BLEND_DST_RGB"),	# 0x80C8
    ("glGet",	X,	1,	"GL_BLEND_SRC_RGB"),	# 0x80C9
    ("glGet",	X,	1,	"GL_BLEND_DST_ALPHA"),	# 0x80CA
    ("glGet",	X,	1,	"GL_BLEND_SRC_ALPHA"),	# 0x80CB
    ("glGet",	X,	1,	"GL_422_EXT"),	# 0x80CC
    ("glGet",	X,	1,	"GL_422_REV_EXT"),	# 0x80CD
    ("glGet",	X,	1,	"GL_422_AVERAGE_EXT"),	# 0x80CE
    ("glGet",	X,	1,	"GL_422_REV_AVERAGE_EXT"),	# 0x80CF
    ("glGet",	I,	1,	"GL_COLOR_TABLE"),	# 0x80D0
    ("glGet",	I,	1,	"GL_POST_CONVOLUTION_COLOR_TABLE"),	# 0x80D1
    ("glGet",	I,	1,	"GL_POST_COLOR_MATRIX_COLOR_TABLE"),	# 0x80D2
    ("glGet",	X,	1,	"GL_PROXY_COLOR_TABLE"),	# 0x80D3
    ("glGet",	X,	1,	"GL_PROXY_POST_CONVOLUTION_COLOR_TABLE"),	# 0x80D4
    ("glGet",	X,	1,	"GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE"),	# 0x80D5
    ("glGet",	X,	1,	"GL_COLOR_TABLE_SCALE"),	# 0x80D6
    ("glGet",	X,	1,	"GL_COLOR_TABLE_BIAS"),	# 0x80D7
    ("glGet",	X,	1,	"GL_COLOR_TABLE_FORMAT"),	# 0x80D8
    ("glGet",	X,	1,	"GL_COLOR_TABLE_WIDTH"),	# 0x80D9
    ("glGet",	X,	1,	"GL_COLOR_TABLE_RED_SIZE"),	# 0x80DA
    ("glGet",	X,	1,	"GL_COLOR_TABLE_GREEN_SIZE"),	# 0x80DB
    ("glGet",	X,	1,	"GL_COLOR_TABLE_BLUE_SIZE"),	# 0x80DC
    ("glGet",	X,	1,	"GL_COLOR_TABLE_ALPHA_SIZE"),	# 0x80DD
    ("glGet",	X,	1,	"GL_COLOR_TABLE_LUMINANCE_SIZE"),	# 0x80DE
    ("glGet",	X,	1,	"GL_COLOR_TABLE_INTENSITY_SIZE"),	# 0x80DF
    ("glGet",	X,	1,	"GL_BGR"),	# 0x80E0
    ("glGet",	X,	1,	"GL_BGRA"),	# 0x80E1
    ("glGet",	X,	1,	"GL_COLOR_INDEX1_EXT"),	# 0x80E2
    ("glGet",	X,	1,	"GL_COLOR_INDEX2_EXT"),	# 0x80E3
    ("glGet",	X,	1,	"GL_COLOR_INDEX4_EXT"),	# 0x80E4
    ("glGet",	X,	1,	"GL_COLOR_INDEX8_EXT"),	# 0x80E5
    ("glGet",	X,	1,	"GL_COLOR_INDEX12_EXT"),	# 0x80E6
    ("glGet",	X,	1,	"GL_COLOR_INDEX16_EXT"),	# 0x80E7
    ("glGet",	I,	1,	"GL_MAX_ELEMENTS_VERTICES"),	# 0x80E8
    ("glGet",	I,	1,	"GL_MAX_ELEMENTS_INDICES"),	# 0x80E9
    ("glGet",	X,	1,	"GL_PHONG_WIN"),	# 0x80EA
    ("glGet",	X,	1,	"GL_PHONG_HINT_WIN"),	# 0x80EB
    ("glGet",	X,	1,	"GL_FOG_SPECULAR_TEXTURE_WIN"),	# 0x80EC
    ("glGet",	X,	1,	"GL_TEXTURE_INDEX_SIZE_EXT"),	# 0x80ED
    ("glGet",	X,	1,	"GL_CLIP_VOLUME_CLIPPING_HINT_EXT"),	# 0x80F0
    ("glGet",	X,	1,	"GL_DUAL_ALPHA4_SGIS"),	# 0x8110
    ("glGet",	X,	1,	"GL_DUAL_ALPHA8_SGIS"),	# 0x8111
    ("glGet",	X,	1,	"GL_DUAL_ALPHA12_SGIS"),	# 0x8112
    ("glGet",	X,	1,	"GL_DUAL_ALPHA16_SGIS"),	# 0x8113
    ("glGet",	X,	1,	"GL_DUAL_LUMINANCE4_SGIS"),	# 0x8114
    ("glGet",	X,	1,	"GL_DUAL_LUMINANCE8_SGIS"),	# 0x8115
    ("glGet",	X,	1,	"GL_DUAL_LUMINANCE12_SGIS"),	# 0x8116
    ("glGet",	X,	1,	"GL_DUAL_LUMINANCE16_SGIS"),	# 0x8117
    ("glGet",	X,	1,	"GL_DUAL_INTENSITY4_SGIS"),	# 0x8118
    ("glGet",	X,	1,	"GL_DUAL_INTENSITY8_SGIS"),	# 0x8119
    ("glGet",	X,	1,	"GL_DUAL_INTENSITY12_SGIS"),	# 0x811A
    ("glGet",	X,	1,	"GL_DUAL_INTENSITY16_SGIS"),	# 0x811B
    ("glGet",	X,	1,	"GL_DUAL_LUMINANCE_ALPHA4_SGIS"),	# 0x811C
    ("glGet",	X,	1,	"GL_DUAL_LUMINANCE_ALPHA8_SGIS"),	# 0x811D
    ("glGet",	X,	1,	"GL_QUAD_ALPHA4_SGIS"),	# 0x811E
    ("glGet",	X,	1,	"GL_QUAD_ALPHA8_SGIS"),	# 0x811F
    ("glGet",	X,	1,	"GL_QUAD_LUMINANCE4_SGIS"),	# 0x8120
    ("glGet",	X,	1,	"GL_QUAD_LUMINANCE8_SGIS"),	# 0x8121
    ("glGet",	X,	1,	"GL_QUAD_INTENSITY4_SGIS"),	# 0x8122
    ("glGet",	X,	1,	"GL_QUAD_INTENSITY8_SGIS"),	# 0x8123
    ("glGet",	X,	1,	"GL_DUAL_TEXTURE_SELECT_SGIS"),	# 0x8124
    ("glGet",	X,	1,	"GL_QUAD_TEXTURE_SELECT_SGIS"),	# 0x8125
    ("glGet",	F,	1,	"GL_POINT_SIZE_MIN"),	# 0x8126
    ("glGet",	F,	1,	"GL_POINT_SIZE_MAX"),	# 0x8127
    ("glGet",	F,	1,	"GL_POINT_FADE_THRESHOLD_SIZE"),	# 0x8128
    ("glGet",	F,	3,	"GL_POINT_DISTANCE_ATTENUATION"),	# 0x8129
    ("glGet",	X,	1,	"GL_FOG_FUNC_SGIS"),	# 0x812A
    ("glGet",	I,	1,	"GL_FOG_FUNC_POINTS_SGIS"),	# 0x812B
    ("glGet",	I,	1,	"GL_MAX_FOG_FUNC_POINTS_SGIS"),	# 0x812C
    ("glGet",	X,	1,	"GL_CLAMP_TO_BORDER"),	# 0x812D
    ("glGet",	X,	1,	"GL_TEXTURE_MULTI_BUFFER_HINT_SGIX"),	# 0x812E
    ("glGet",	X,	1,	"GL_CLAMP_TO_EDGE"),	# 0x812F
    ("glGet",	I,	1,	"GL_PACK_SKIP_VOLUMES_SGIS"),	# 0x8130
    ("glGet",	I,	1,	"GL_PACK_IMAGE_DEPTH_SGIS"),	# 0x8131
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_VOLUMES_SGIS"),	# 0x8132
    ("glGet",	I,	1,	"GL_UNPACK_IMAGE_DEPTH_SGIS"),	# 0x8133
    ("glGet",	I,	1,	"GL_TEXTURE_4D_SGIS"),	# 0x8134
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_4D_SGIS"),	# 0x8135
    ("glGet",	X,	1,	"GL_TEXTURE_4DSIZE_SGIS"),	# 0x8136
    ("glGet",	X,	1,	"GL_TEXTURE_WRAP_Q_SGIS"),	# 0x8137
    ("glGet",	I,	1,	"GL_MAX_4D_TEXTURE_SIZE_SGIS"),	# 0x8138
    ("glGet",	I,	1,	"GL_PIXEL_TEX_GEN_SGIX"),	# 0x8139
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MIN_LOD"),	# 0x813A
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MAX_LOD"),	# 0x813B
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_BASE_LEVEL"),	# 0x813C
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MAX_LEVEL"),	# 0x813D
    ("glGet",	I,	1,	"GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX"),	# 0x813E
    ("glGet",	I,	1,	"GL_PIXEL_TILE_CACHE_INCREMENT_SGIX"),	# 0x813F
    ("glGet",	I,	1,	"GL_PIXEL_TILE_WIDTH_SGIX"),	# 0x8140
    ("glGet",	I,	1,	"GL_PIXEL_TILE_HEIGHT_SGIX"),	# 0x8141
    ("glGet",	I,	1,	"GL_PIXEL_TILE_GRID_WIDTH_SGIX"),	# 0x8142
    ("glGet",	I,	1,	"GL_PIXEL_TILE_GRID_HEIGHT_SGIX"),	# 0x8143
    ("glGet",	I,	1,	"GL_PIXEL_TILE_GRID_DEPTH_SGIX"),	# 0x8144
    ("glGet",	I,	1,	"GL_PIXEL_TILE_CACHE_SIZE_SGIX"),	# 0x8145
    ("glGet",	X,	1,	"GL_FILTER4_SGIS"),	# 0x8146
    ("glGet",	X,	1,	"GL_TEXTURE_FILTER4_SIZE_SGIS"),	# 0x8147
    ("glGet",	I,	1,	"GL_SPRITE_SGIX"),	# 0x8148
    ("glGet",	I,	1,	"GL_SPRITE_MODE_SGIX"),	# 0x8149
    ("glGet",	F,	3,	"GL_SPRITE_AXIS_SGIX"),	# 0x814A
    ("glGet",	F,	3,	"GL_SPRITE_TRANSLATION_SGIX"),	# 0x814B
    ("glGet",	X,	1,	"GL_SPRITE_AXIAL_SGIX"),	# 0x814C
    ("glGet",	X,	1,	"GL_SPRITE_OBJECT_ALIGNED_SGIX"),	# 0x814D
    ("glGet",	X,	1,	"GL_SPRITE_EYE_ALIGNED_SGIX"),	# 0x814E
    ("glGet",	I,	1,	"GL_TEXTURE_4D_BINDING_SGIS"),	# 0x814F
    ("glGet",	X,	1,	"GL_IGNORE_BORDER_HP"),	# 0x8150
    ("glGet",	X,	1,	"GL_CONSTANT_BORDER"),	# 0x8151
    ("glGet",	X,	1,	"GL_REPLICATE_BORDER"),	# 0x8153
    ("glGet",	X,	1,	"GL_CONVOLUTION_BORDER_COLOR"),	# 0x8154
    ("glGet",	X,	1,	"GL_IMAGE_SCALE_X_HP"),	# 0x8155
    ("glGet",	X,	1,	"GL_IMAGE_SCALE_Y_HP"),	# 0x8156
    ("glGet",	X,	1,	"GL_IMAGE_TRANSLATE_X_HP"),	# 0x8157
    ("glGet",	X,	1,	"GL_IMAGE_TRANSLATE_Y_HP"),	# 0x8158
    ("glGet",	X,	1,	"GL_IMAGE_ROTATE_ANGLE_HP"),	# 0x8159
    ("glGet",	X,	1,	"GL_IMAGE_ROTATE_ORIGIN_X_HP"),	# 0x815A
    ("glGet",	X,	1,	"GL_IMAGE_ROTATE_ORIGIN_Y_HP"),	# 0x815B
    ("glGet",	X,	1,	"GL_IMAGE_MAG_FILTER_HP"),	# 0x815C
    ("glGet",	X,	1,	"GL_IMAGE_MIN_FILTER_HP"),	# 0x815D
    ("glGet",	X,	1,	"GL_IMAGE_CUBIC_WEIGHT_HP"),	# 0x815E
    ("glGet",	X,	1,	"GL_CUBIC_HP"),	# 0x815F
    ("glGet",	X,	1,	"GL_AVERAGE_HP"),	# 0x8160
    ("glGet",	X,	1,	"GL_IMAGE_TRANSFORM_2D_HP"),	# 0x8161
    ("glGet",	X,	1,	"GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP"),	# 0x8162
    ("glGet",	X,	1,	"GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP"),	# 0x8163
    ("glGet",	X,	1,	"GL_OCCLUSION_TEST_HP"),	# 0x8165
    ("glGet",	X,	1,	"GL_OCCLUSION_TEST_RESULT_HP"),	# 0x8166
    ("glGet",	X,	1,	"GL_TEXTURE_LIGHTING_MODE_HP"),	# 0x8167
    ("glGet",	X,	1,	"GL_TEXTURE_POST_SPECULAR_HP"),	# 0x8168
    ("glGet",	X,	1,	"GL_TEXTURE_PRE_SPECULAR_HP"),	# 0x8169
    ("glGet",	X,	1,	"GL_LINEAR_CLIPMAP_LINEAR_SGIX"),	# 0x8170
    ("glGet",	X,	1,	"GL_TEXTURE_CLIPMAP_CENTER_SGIX"),	# 0x8171
    ("glGet",	X,	1,	"GL_TEXTURE_CLIPMAP_FRAME_SGIX"),	# 0x8172
    ("glGet",	X,	1,	"GL_TEXTURE_CLIPMAP_OFFSET_SGIX"),	# 0x8173
    ("glGet",	X,	1,	"GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX"),	# 0x8174
    ("glGet",	X,	1,	"GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX"),	# 0x8175
    ("glGet",	X,	1,	"GL_TEXTURE_CLIPMAP_DEPTH_SGIX"),	# 0x8176
    ("glGet",	I,	1,	"GL_MAX_CLIPMAP_DEPTH_SGIX"),	# 0x8177
    ("glGet",	I,	1,	"GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX"),	# 0x8178
    ("glGet",	X,	1,	"GL_POST_TEXTURE_FILTER_BIAS_SGIX"),	# 0x8179
    ("glGet",	X,	1,	"GL_POST_TEXTURE_FILTER_SCALE_SGIX"),	# 0x817A
    ("glGet",	F,	2,	"GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX"),	# 0x817B
    ("glGet",	F,	2,	"GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX"),	# 0x817C
    ("glGet",	I,	1,	"GL_REFERENCE_PLANE_SGIX"),	# 0x817D
    ("glGet",	F,	4,	"GL_REFERENCE_PLANE_EQUATION_SGIX"),	# 0x817E
    ("glGet",	I,	1,	"GL_IR_INSTRUMENT1_SGIX"),	# 0x817F
    ("glGet",	X,	1,	"GL_INSTRUMENT_BUFFER_POINTER_SGIX"),	# 0x8180
    ("glGet",	I,	1,	"GL_INSTRUMENT_MEASUREMENTS_SGIX"),	# 0x8181
    ("glGet",	X,	1,	"GL_LIST_PRIORITY_SGIX"),	# 0x8182
    ("glGet",	I,	1,	"GL_CALLIGRAPHIC_FRAGMENT_SGIX"),	# 0x8183
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_Q_CEILING_SGIX"),	# 0x8184
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_Q_ROUND_SGIX"),	# 0x8185
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX"),	# 0x8186
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_REPLACE_SGIX"),	# 0x8187
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_NO_REPLACE_SGIX"),	# 0x8188
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX"),	# 0x8189
    ("glGet",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX"),	# 0x818A
    ("glGet",	I,	1,	"GL_FRAMEZOOM_SGIX"),	# 0x818B
    ("glGet",	I,	1,	"GL_FRAMEZOOM_FACTOR_SGIX"),	# 0x818C
    ("glGet",	I,	1,	"GL_MAX_FRAMEZOOM_FACTOR_SGIX"),	# 0x818D
    ("glGet",	X,	1,	"GL_TEXTURE_LOD_BIAS_S_SGIX"),	# 0x818E
    ("glGet",	X,	1,	"GL_TEXTURE_LOD_BIAS_T_SGIX"),	# 0x818F
    ("glGet",	X,	1,	"GL_TEXTURE_LOD_BIAS_R_SGIX"),	# 0x8190
    ("glGetTexParameter",	B,	1,	"GL_GENERATE_MIPMAP"),	# 0x8191
    ("glGet",	E,	1,	"GL_GENERATE_MIPMAP_HINT"),	# 0x8192
    ("glGet",	X,	1,	"GL_GEOMETRY_DEFORMATION_SGIX"),	# 0x8194
    ("glGet",	X,	1,	"GL_TEXTURE_DEFORMATION_SGIX"),	# 0x8195
    ("glGet",	I,	1,	"GL_DEFORMATIONS_MASK_SGIX"),	# 0x8196
    ("glGet",	X,	1,	"GL_MAX_DEFORMATION_ORDER_SGIX"),	# 0x8197
    ("glGet",	I,	1,	"GL_FOG_OFFSET_SGIX"),	# 0x8198
    ("glGet",	F,	4,	"GL_FOG_OFFSET_VALUE_SGIX"),	# 0x8199
    ("glGet",	X,	1,	"GL_TEXTURE_COMPARE_SGIX"),	# 0x819A
    ("glGet",	X,	1,	"GL_TEXTURE_COMPARE_OPERATOR_SGIX"),	# 0x819B
    ("glGet",	X,	1,	"GL_TEXTURE_LEQUAL_R_SGIX"),	# 0x819C
    ("glGet",	X,	1,	"GL_TEXTURE_GEQUAL_R_SGIX"),	# 0x819D
    ("glGet",	X,	1,	"GL_DEPTH_COMPONENT16"),	# 0x81A5
    ("glGet",	X,	1,	"GL_DEPTH_COMPONENT24"),	# 0x81A6
    ("glGet",	X,	1,	"GL_DEPTH_COMPONENT32"),	# 0x81A7
    ("glGet",	X,	1,	"GL_ARRAY_ELEMENT_LOCK_FIRST_EXT"),	# 0x81A8
    ("glGet",	X,	1,	"GL_ARRAY_ELEMENT_LOCK_COUNT_EXT"),	# 0x81A9
    ("glGet",	X,	1,	"GL_CULL_VERTEX_EXT"),	# 0x81AA
    ("glGet",	X,	1,	"GL_CULL_VERTEX_EYE_POSITION_EXT"),	# 0x81AB
    ("glGet",	X,	1,	"GL_CULL_VERTEX_OBJECT_POSITION_EXT"),	# 0x81AC
    ("glGet",	X,	1,	"GL_IUI_V2F_EXT"),	# 0x81AD
    ("glGet",	X,	1,	"GL_IUI_V3F_EXT"),	# 0x81AE
    ("glGet",	X,	1,	"GL_IUI_N3F_V2F_EXT"),	# 0x81AF
    ("glGet",	X,	1,	"GL_IUI_N3F_V3F_EXT"),	# 0x81B0
    ("glGet",	X,	1,	"GL_T2F_IUI_V2F_EXT"),	# 0x81B1
    ("glGet",	X,	1,	"GL_T2F_IUI_V3F_EXT"),	# 0x81B2
    ("glGet",	X,	1,	"GL_T2F_IUI_N3F_V2F_EXT"),	# 0x81B3
    ("glGet",	X,	1,	"GL_T2F_IUI_N3F_V3F_EXT"),	# 0x81B4
    ("glGet",	X,	1,	"GL_INDEX_TEST_EXT"),	# 0x81B5
    ("glGet",	X,	1,	"GL_INDEX_TEST_FUNC_EXT"),	# 0x81B6
    ("glGet",	X,	1,	"GL_INDEX_TEST_REF_EXT"),	# 0x81B7
    ("glGet",	X,	1,	"GL_INDEX_MATERIAL_EXT"),	# 0x81B8
    ("glGet",	X,	1,	"GL_INDEX_MATERIAL_PARAMETER_EXT"),	# 0x81B9
    ("glGet",	X,	1,	"GL_INDEX_MATERIAL_FACE_EXT"),	# 0x81BA
    ("glGet",	X,	1,	"GL_YCRCB_422_SGIX"),	# 0x81BB
    ("glGet",	X,	1,	"GL_YCRCB_444_SGIX"),	# 0x81BC
    ("glGet",	X,	1,	"GL_WRAP_BORDER_SUN"),	# 0x81D4
    ("glGet",	X,	1,	"GL_UNPACK_CONSTANT_DATA_SUNX"),	# 0x81D5
    ("glGet",	X,	1,	"GL_TEXTURE_CONSTANT_DATA_SUNX"),	# 0x81D6
    ("glGet",	X,	1,	"GL_TRIANGLE_LIST_SUN"),	# 0x81D7
    ("glGet",	X,	1,	"GL_REPLACEMENT_CODE_SUN"),	# 0x81D8
    ("glGet",	X,	1,	"GL_GLOBAL_ALPHA_SUN"),	# 0x81D9
    ("glGet",	X,	1,	"GL_GLOBAL_ALPHA_FACTOR_SUN"),	# 0x81DA
    ("glGet",	X,	1,	"GL_TEXTURE_COLOR_WRITEMASK_SGIS"),	# 0x81EF
    ("glGet",	X,	1,	"GL_EYE_DISTANCE_TO_POINT_SGIS"),	# 0x81F0
    ("glGet",	X,	1,	"GL_OBJECT_DISTANCE_TO_POINT_SGIS"),	# 0x81F1
    ("glGet",	X,	1,	"GL_EYE_DISTANCE_TO_LINE_SGIS"),	# 0x81F2
    ("glGet",	X,	1,	"GL_OBJECT_DISTANCE_TO_LINE_SGIS"),	# 0x81F3
    ("glGet",	X,	1,	"GL_EYE_POINT_SGIS"),	# 0x81F4
    ("glGet",	X,	1,	"GL_OBJECT_POINT_SGIS"),	# 0x81F5
    ("glGet",	X,	1,	"GL_EYE_LINE_SGIS"),	# 0x81F6
    ("glGet",	X,	1,	"GL_OBJECT_LINE_SGIS"),	# 0x81F7
    ("glGet",	E,	1,	"GL_LIGHT_MODEL_COLOR_CONTROL"),	# 0x81F8
    ("glGet",	X,	1,	"GL_SINGLE_COLOR"),	# 0x81F9
    ("glGet",	X,	1,	"GL_SEPARATE_SPECULAR_COLOR"),	# 0x81FA
    ("glGet",	X,	1,	"GL_SHARED_TEXTURE_PALETTE_EXT"),	# 0x81FB
    ("glGet",	X,	1,	"GL_FOG_SCALE_SGIX"),	# 0x81FC
    ("glGet",	X,	1,	"GL_FOG_SCALE_VALUE_SGIX"),	# 0x81FD
    ("glGet",	X,	1,	"GL_TEXT_FRAGMENT_SHADER_ATI"),	# 0x8200
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING"),	# 0x8210
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE"),	# 0x8211
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE"),	# 0x8212
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE"),	# 0x8213
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE"),	# 0x8214
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE"),	# 0x8215
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE"),	# 0x8216
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE"),	# 0x8217
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_DEFAULT"),	# 0x8218
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_UNDEFINED"),	# 0x8219
    ("glGet",	X,	1,	"GL_DEPTH_STENCIL_ATTACHMENT"),	# 0x821A
    ("glGet",	X,	1,	"GL_MAJOR_VERSION"),	# 0x821B
    ("glGet",	X,	1,	"GL_MINOR_VERSION"),	# 0x821C
    ("glGet",	X,	1,	"GL_NUM_EXTENSIONS"),	# 0x821D
    ("glGet",	X,	1,	"GL_CONTEXT_FLAGS"),	# 0x821E
    ("glGet",	X,	1,	"GL_INDEX"),	# 0x8222
    ("glGet",	X,	1,	"GL_DEPTH_BUFFER"),	# 0x8223
    ("glGet",	X,	1,	"GL_STENCIL_BUFFER"),	# 0x8224
    ("glGet",	X,	1,	"GL_COMPRESSED_RED"),	# 0x8225
    ("glGet",	X,	1,	"GL_COMPRESSED_RG"),	# 0x8226
    ("glGet",	X,	1,	"GL_RG"),	# 0x8227
    ("glGet",	X,	1,	"GL_RG_INTEGER"),	# 0x8228
    ("glGet",	X,	1,	"GL_R8"),	# 0x8229
    ("glGet",	X,	1,	"GL_R16"),	# 0x822A
    ("glGet",	X,	1,	"GL_RG8"),	# 0x822B
    ("glGet",	X,	1,	"GL_RG16"),	# 0x822C
    ("glGet",	X,	1,	"GL_R16F"),	# 0x822D
    ("glGet",	X,	1,	"GL_R32F"),	# 0x822E
    ("glGet",	X,	1,	"GL_RG16F"),	# 0x822F
    ("glGet",	X,	1,	"GL_RG32F"),	# 0x8230
    ("glGet",	X,	1,	"GL_R8I"),	# 0x8231
    ("glGet",	X,	1,	"GL_R8UI"),	# 0x8232
    ("glGet",	X,	1,	"GL_R16I"),	# 0x8233
    ("glGet",	X,	1,	"GL_R16UI"),	# 0x8234
    ("glGet",	X,	1,	"GL_R32I"),	# 0x8235
    ("glGet",	X,	1,	"GL_R32UI"),	# 0x8236
    ("glGet",	X,	1,	"GL_RG8I"),	# 0x8237
    ("glGet",	X,	1,	"GL_RG8UI"),	# 0x8238
    ("glGet",	X,	1,	"GL_RG16I"),	# 0x8239
    ("glGet",	X,	1,	"GL_RG16UI"),	# 0x823A
    ("glGet",	X,	1,	"GL_RG32I"),	# 0x823B
    ("glGet",	X,	1,	"GL_RG32UI"),	# 0x823C
    ("glGet",	X,	1,	"GL_SYNC_CL_EVENT_ARB"),	# 0x8240
    ("glGet",	X,	1,	"GL_SYNC_CL_EVENT_COMPLETE_ARB"),	# 0x8241
    ("glGet",	X,	1,	"GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB"),	# 0x8242
    ("glGet",	X,	1,	"GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB"),	# 0x8243
    ("glGet",	X,	1,	"GL_DEBUG_CALLBACK_FUNCTION_ARB"),	# 0x8244
    ("glGet",	X,	1,	"GL_DEBUG_CALLBACK_USER_PARAM_ARB"),	# 0x8245
    ("glGet",	X,	1,	"GL_DEBUG_SOURCE_API_ARB"),	# 0x8246
    ("glGet",	X,	1,	"GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB"),	# 0x8247
    ("glGet",	X,	1,	"GL_DEBUG_SOURCE_SHADER_COMPILER_ARB"),	# 0x8248
    ("glGet",	X,	1,	"GL_DEBUG_SOURCE_THIRD_PARTY_ARB"),	# 0x8249
    ("glGet",	X,	1,	"GL_DEBUG_SOURCE_APPLICATION_ARB"),	# 0x824A
    ("glGet",	X,	1,	"GL_DEBUG_SOURCE_OTHER_ARB"),	# 0x824B
    ("glGet",	X,	1,	"GL_DEBUG_TYPE_ERROR_ARB"),	# 0x824C
    ("glGet",	X,	1,	"GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB"),	# 0x824D
    ("glGet",	X,	1,	"GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB"),	# 0x824E
    ("glGet",	X,	1,	"GL_DEBUG_TYPE_PORTABILITY_ARB"),	# 0x824F
    ("glGet",	X,	1,	"GL_DEBUG_TYPE_PERFORMANCE_ARB"),	# 0x8250
    ("glGet",	X,	1,	"GL_DEBUG_TYPE_OTHER_ARB"),	# 0x8251
    ("glGet",	X,	1,	"GL_LOSE_CONTEXT_ON_RESET_ARB"),	# 0x8252
    ("glGet",	X,	1,	"GL_GUILTY_CONTEXT_RESET_ARB"),	# 0x8253
    ("glGet",	X,	1,	"GL_INNOCENT_CONTEXT_RESET_ARB"),	# 0x8254
    ("glGet",	X,	1,	"GL_UNKNOWN_CONTEXT_RESET_ARB"),	# 0x8255
    ("glGet",	X,	1,	"GL_RESET_NOTIFICATION_STRATEGY_ARB"),	# 0x8256
    ("glGet",	X,	1,	"GL_PROGRAM_BINARY_RETRIEVABLE_HINT"),	# 0x8257
    ("glGet",	X,	1,	"GL_PROGRAM_SEPARABLE"),	# 0x8258
    ("glGet",	I,	1,	"GL_ACTIVE_PROGRAM"),	# 0x8259
    ("glGet",	I,	1,	"GL_PROGRAM_PIPELINE_BINDING"),	# 0x825A
    ("glGet",	X,	1,	"GL_MAX_VIEWPORTS"),	# 0x825B
    ("glGet",	X,	1,	"GL_VIEWPORT_SUBPIXEL_BITS"),	# 0x825C
    ("glGet",	X,	1,	"GL_VIEWPORT_BOUNDS_RANGE"),	# 0x825D
    ("glGet",	X,	1,	"GL_LAYER_PROVOKING_VERTEX"),	# 0x825E
    ("glGet",	X,	1,	"GL_VIEWPORT_INDEX_PROVOKING_VERTEX"),	# 0x825F
    ("glGet",	X,	1,	"GL_UNDEFINED_VERTEX"),	# 0x8260
    ("glGet",	X,	1,	"GL_NO_RESET_NOTIFICATION_ARB"),	# 0x8261
    ("glGet",	X,	1,	"GL_DEPTH_PASS_INSTRUMENT_SGIX"),	# 0x8310
    ("glGet",	X,	1,	"GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX"),	# 0x8311
    ("glGet",	X,	1,	"GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX"),	# 0x8312
    ("glGet",	I,	1,	"GL_CONVOLUTION_HINT_SGIX"),	# 0x8316
    ("glGet",	X,	1,	"GL_YCRCB_SGIX"),	# 0x8318
    ("glGet",	X,	1,	"GL_YCRCBA_SGIX"),	# 0x8319
    ("glGet",	X,	1,	"GL_ALPHA_MIN_SGIX"),	# 0x8320
    ("glGet",	X,	1,	"GL_ALPHA_MAX_SGIX"),	# 0x8321
    ("glGet",	X,	1,	"GL_SCALEBIAS_HINT_SGIX"),	# 0x8322
    ("glGet",	X,	1,	"GL_ASYNC_MARKER_SGIX"),	# 0x8329
    ("glGet",	I,	1,	"GL_PIXEL_TEX_GEN_MODE_SGIX"),	# 0x832B
    ("glGet",	X,	1,	"GL_ASYNC_HISTOGRAM_SGIX"),	# 0x832C
    ("glGet",	X,	1,	"GL_MAX_ASYNC_HISTOGRAM_SGIX"),	# 0x832D
    ("glGet",	X,	1,	"GL_PIXEL_TRANSFORM_2D_EXT"),	# 0x8330
    ("glGet",	X,	1,	"GL_PIXEL_MAG_FILTER_EXT"),	# 0x8331
    ("glGet",	X,	1,	"GL_PIXEL_MIN_FILTER_EXT"),	# 0x8332
    ("glGet",	X,	1,	"GL_PIXEL_CUBIC_WEIGHT_EXT"),	# 0x8333
    ("glGet",	X,	1,	"GL_CUBIC_EXT"),	# 0x8334
    ("glGet",	X,	1,	"GL_AVERAGE_EXT"),	# 0x8335
    ("glGet",	X,	1,	"GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT"),	# 0x8336
    ("glGet",	X,	1,	"GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT"),	# 0x8337
    ("glGet",	X,	1,	"GL_PIXEL_TRANSFORM_2D_MATRIX_EXT"),	# 0x8338
    ("glGet",	X,	1,	"GL_FRAGMENT_MATERIAL_EXT"),	# 0x8349
    ("glGet",	X,	1,	"GL_FRAGMENT_NORMAL_EXT"),	# 0x834A
    ("glGet",	X,	1,	"GL_FRAGMENT_COLOR_EXT"),	# 0x834C
    ("glGet",	X,	1,	"GL_ATTENUATION_EXT"),	# 0x834D
    ("glGet",	X,	1,	"GL_SHADOW_ATTENUATION_EXT"),	# 0x834E
    ("glGet",	I,	1,	"GL_TEXTURE_APPLICATION_MODE_EXT"),	# 0x834F
    ("glGet",	I,	1,	"GL_TEXTURE_LIGHT_EXT"),	# 0x8350
    ("glGet",	I,	1,	"GL_TEXTURE_MATERIAL_FACE_EXT"),	# 0x8351
    ("glGet",	I,	1,	"GL_TEXTURE_MATERIAL_PARAMETER_EXT"),	# 0x8352
    ("glGet",	I,	1,	"GL_PIXEL_TEXTURE_SGIS"),	# 0x8353
    ("glGet",	I,	1,	"GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS"),	# 0x8354
    ("glGet",	I,	1,	"GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS"),	# 0x8355
    ("glGet",	I,	1,	"GL_PIXEL_GROUP_COLOR_SGIS"),	# 0x8356
    ("glGet",	X,	1,	"GL_ASYNC_TEX_IMAGE_SGIX"),	# 0x835C
    ("glGet",	X,	1,	"GL_ASYNC_DRAW_PIXELS_SGIX"),	# 0x835D
    ("glGet",	X,	1,	"GL_ASYNC_READ_PIXELS_SGIX"),	# 0x835E
    ("glGet",	X,	1,	"GL_MAX_ASYNC_TEX_IMAGE_SGIX"),	# 0x835F
    ("glGet",	X,	1,	"GL_MAX_ASYNC_DRAW_PIXELS_SGIX"),	# 0x8360
    ("glGet",	X,	1,	"GL_MAX_ASYNC_READ_PIXELS_SGIX"),	# 0x8361
    ("glGet",	X,	1,	"GL_UNSIGNED_BYTE_2_3_3_REV"),	# 0x8362
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_5_6_5"),	# 0x8363
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_5_6_5_REV"),	# 0x8364
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_4_4_4_4_REV"),	# 0x8365
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_1_5_5_5_REV"),	# 0x8366
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_8_8_8_8_REV"),	# 0x8367
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_2_10_10_10_REV"),	# 0x8368
    ("glGet",	X,	1,	"GL_TEXTURE_MAX_CLAMP_S_SGIX"),	# 0x8369
    ("glGet",	X,	1,	"GL_TEXTURE_MAX_CLAMP_T_SGIX"),	# 0x836A
    ("glGet",	X,	1,	"GL_TEXTURE_MAX_CLAMP_R_SGIX"),	# 0x836B
    ("glGet",	X,	1,	"GL_MIRRORED_REPEAT"),	# 0x8370
    ("glGet",	X,	1,	"GL_RGB_S3TC"),	# 0x83A0
    ("glGet",	X,	1,	"GL_RGB4_S3TC"),	# 0x83A1
    ("glGet",	X,	1,	"GL_RGBA_S3TC"),	# 0x83A2
    ("glGet",	X,	1,	"GL_RGBA4_S3TC"),	# 0x83A3
    ("glGet",	X,	1,	"GL_VERTEX_PRECLIP_SGIX"),	# 0x83EE
    ("glGet",	X,	1,	"GL_VERTEX_PRECLIP_HINT_SGIX"),	# 0x83EF
    ("glGet",	X,	1,	"GL_COMPRESSED_RGB_S3TC_DXT1_EXT"),	# 0x83F0
    ("glGet",	X,	1,	"GL_COMPRESSED_RGBA_S3TC_DXT1_EXT"),	# 0x83F1
    ("glGet",	X,	1,	"GL_COMPRESSED_RGBA_S3TC_DXT3_EXT"),	# 0x83F2
    ("glGet",	X,	1,	"GL_COMPRESSED_RGBA_S3TC_DXT5_EXT"),	# 0x83F3
    ("glGet",	X,	1,	"GL_PARALLEL_ARRAYS_INTEL"),	# 0x83F4
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F5
    ("glGet",	X,	1,	"GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F6
    ("glGet",	X,	1,	"GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F7
    ("glGet",	X,	1,	"GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F8
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHTING_SGIX"),	# 0x8400
    ("glGet",	I,	1,	"GL_FRAGMENT_COLOR_MATERIAL_SGIX"),	# 0x8401
    ("glGet",	I,	1,	"GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX"),	# 0x8402
    ("glGet",	I,	1,	"GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX"),	# 0x8403
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_LIGHTS_SGIX"),	# 0x8404
    ("glGet",	I,	1,	"GL_MAX_ACTIVE_LIGHTS_SGIX"),	# 0x8405
    ("glGet",	I,	1,	"GL_CURRENT_RASTER_NORMAL_SGIX"),	# 0x8406
    ("glGet",	I,	1,	"GL_LIGHT_ENV_MODE_SGIX"),	# 0x8407
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX"),	# 0x8408
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX"),	# 0x8409
    ("glGet",	F,	4,	"GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX"),	# 0x840A
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX"),	# 0x840B
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT0_SGIX"),	# 0x840C
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT1_SGIX"),	# 0x840D
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT2_SGIX"),	# 0x840E
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT3_SGIX"),	# 0x840F
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT4_SGIX"),	# 0x8410
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT5_SGIX"),	# 0x8411
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT6_SGIX"),	# 0x8412
    ("glGet",	X,	1,	"GL_FRAGMENT_LIGHT7_SGIX"),	# 0x8413
    ("glGet",	X,	1,	"GL_PACK_RESAMPLE_SGIX"),	# 0x842C
    ("glGet",	X,	1,	"GL_UNPACK_RESAMPLE_SGIX"),	# 0x842D
    ("glGet",	X,	1,	"GL_RESAMPLE_REPLICATE_SGIX"),	# 0x842E
    ("glGet",	X,	1,	"GL_RESAMPLE_ZERO_FILL_SGIX"),	# 0x842F
    ("glGet",	X,	1,	"GL_RESAMPLE_DECIMATE_SGIX"),	# 0x8430
    ("glGet",	X,	1,	"GL_TANGENT_ARRAY_EXT"),	# 0x8439
    ("glGet",	X,	1,	"GL_BINORMAL_ARRAY_EXT"),	# 0x843A
    ("glGet",	X,	1,	"GL_CURRENT_TANGENT_EXT"),	# 0x843B
    ("glGet",	X,	1,	"GL_CURRENT_BINORMAL_EXT"),	# 0x843C
    ("glGet",	E,	1,	"GL_TANGENT_ARRAY_TYPE_EXT"),	# 0x843E
    ("glGet",	X,	1,	"GL_TANGENT_ARRAY_STRIDE_EXT"),	# 0x843F
    ("glGet",	E,	1,	"GL_BINORMAL_ARRAY_TYPE_EXT"),	# 0x8440
    ("glGet",	X,	1,	"GL_BINORMAL_ARRAY_STRIDE_EXT"),	# 0x8441
    ("glGet",	P,	1,	"GL_TANGENT_ARRAY_POINTER_EXT"),	# 0x8442
    ("glGet",	P,	1,	"GL_BINORMAL_ARRAY_POINTER_EXT"),	# 0x8443
    ("glGet",	X,	1,	"GL_MAP1_TANGENT_EXT"),	# 0x8444
    ("glGet",	X,	1,	"GL_MAP2_TANGENT_EXT"),	# 0x8445
    ("glGet",	X,	1,	"GL_MAP1_BINORMAL_EXT"),	# 0x8446
    ("glGet",	X,	1,	"GL_MAP2_BINORMAL_EXT"),	# 0x8447
    ("glGet",	X,	1,	"GL_NEAREST_CLIPMAP_NEAREST_SGIX"),	# 0x844D
    ("glGet",	X,	1,	"GL_NEAREST_CLIPMAP_LINEAR_SGIX"),	# 0x844E
    ("glGet",	X,	1,	"GL_LINEAR_CLIPMAP_NEAREST_SGIX"),	# 0x844F
    ("glGet",	E,	1,	"GL_FOG_COORD_SRC"),	# 0x8450
    ("glGet",	X,	1,	"GL_FOG_COORD"),	# 0x8451
    ("glGet",	X,	1,	"GL_FRAGMENT_DEPTH"),	# 0x8452
    ("glGet",	F,	1,	"GL_CURRENT_FOG_COORD"),	# 0x8453
    ("glGet",	E,	1,	"GL_FOG_COORD_ARRAY_TYPE"),	# 0x8454
    ("glGet",	I,	1,	"GL_FOG_COORD_ARRAY_STRIDE"),	# 0x8455
    ("glGet",	X,	1,	"GL_FOG_COORD_ARRAY_POINTER"),	# 0x8456
    ("glGet",	B,	1,	"GL_FOG_COORD_ARRAY"),	# 0x8457
    ("glGet",	I,	1,	"GL_COLOR_SUM"),	# 0x8458
    ("glGet",	F,	3,	"GL_CURRENT_SECONDARY_COLOR"),	# 0x8459
    ("glGet",	I,	1,	"GL_SECONDARY_COLOR_ARRAY_SIZE"),	# 0x845A
    ("glGet",	E,	1,	"GL_SECONDARY_COLOR_ARRAY_TYPE"),	# 0x845B
    ("glGet",	I,	1,	"GL_SECONDARY_COLOR_ARRAY_STRIDE"),	# 0x845C
    ("glGet",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_POINTER"),	# 0x845D
    ("glGet",	B,	1,	"GL_SECONDARY_COLOR_ARRAY"),	# 0x845E
    ("glGet",	X,	1,	"GL_CURRENT_RASTER_SECONDARY_COLOR"),	# 0x845F
    ("glGet",	F,	2,	"GL_ALIASED_POINT_SIZE_RANGE"),	# 0x846D
    ("glGet",	F,	2,	"GL_ALIASED_LINE_WIDTH_RANGE"),	# 0x846E
    ("glGet",	X,	1,	"GL_SCREEN_COORDINATES_REND"),	# 0x8490
    ("glGet",	X,	1,	"GL_INVERTED_SCREEN_W_REND"),	# 0x8491
    ("glGet",	X,	1,	"GL_TEXTURE0"),	# 0x84C0
    ("glGet",	X,	1,	"GL_TEXTURE1"),	# 0x84C1
    ("glGet",	X,	1,	"GL_TEXTURE2"),	# 0x84C2
    ("glGet",	X,	1,	"GL_TEXTURE3"),	# 0x84C3
    ("glGet",	X,	1,	"GL_TEXTURE4"),	# 0x84C4
    ("glGet",	X,	1,	"GL_TEXTURE5"),	# 0x84C5
    ("glGet",	X,	1,	"GL_TEXTURE6"),	# 0x84C6
    ("glGet",	X,	1,	"GL_TEXTURE7"),	# 0x84C7
    ("glGet",	X,	1,	"GL_TEXTURE8"),	# 0x84C8
    ("glGet",	X,	1,	"GL_TEXTURE9"),	# 0x84C9
    ("glGet",	X,	1,	"GL_TEXTURE10"),	# 0x84CA
    ("glGet",	X,	1,	"GL_TEXTURE11"),	# 0x84CB
    ("glGet",	X,	1,	"GL_TEXTURE12"),	# 0x84CC
    ("glGet",	X,	1,	"GL_TEXTURE13"),	# 0x84CD
    ("glGet",	X,	1,	"GL_TEXTURE14"),	# 0x84CE
    ("glGet",	X,	1,	"GL_TEXTURE15"),	# 0x84CF
    ("glGet",	X,	1,	"GL_TEXTURE16"),	# 0x84D0
    ("glGet",	X,	1,	"GL_TEXTURE17"),	# 0x84D1
    ("glGet",	X,	1,	"GL_TEXTURE18"),	# 0x84D2
    ("glGet",	X,	1,	"GL_TEXTURE19"),	# 0x84D3
    ("glGet",	X,	1,	"GL_TEXTURE20"),	# 0x84D4
    ("glGet",	X,	1,	"GL_TEXTURE21"),	# 0x84D5
    ("glGet",	X,	1,	"GL_TEXTURE22"),	# 0x84D6
    ("glGet",	X,	1,	"GL_TEXTURE23"),	# 0x84D7
    ("glGet",	X,	1,	"GL_TEXTURE24"),	# 0x84D8
    ("glGet",	X,	1,	"GL_TEXTURE25"),	# 0x84D9
    ("glGet",	X,	1,	"GL_TEXTURE26"),	# 0x84DA
    ("glGet",	X,	1,	"GL_TEXTURE27"),	# 0x84DB
    ("glGet",	X,	1,	"GL_TEXTURE28"),	# 0x84DC
    ("glGet",	X,	1,	"GL_TEXTURE29"),	# 0x84DD
    ("glGet",	X,	1,	"GL_TEXTURE30"),	# 0x84DE
    ("glGet",	X,	1,	"GL_TEXTURE31"),	# 0x84DF
    ("glGet",	E,	1,	"GL_ACTIVE_TEXTURE"),	# 0x84E0
    ("glGet",	E,	1,	"GL_CLIENT_ACTIVE_TEXTURE"),	# 0x84E1
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_UNITS"),	# 0x84E2
    ("glGet",	F,	16,	"GL_TRANSPOSE_MODELVIEW_MATRIX"),	# 0x84E3
    ("glGet",	F,	16,	"GL_TRANSPOSE_PROJECTION_MATRIX"),	# 0x84E4
    ("glGet",	F,	16,	"GL_TRANSPOSE_TEXTURE_MATRIX"),	# 0x84E5
    ("glGet",	F,	16,	"GL_TRANSPOSE_COLOR_MATRIX"),	# 0x84E6
    ("glGet",	X,	1,	"GL_SUBTRACT"),	# 0x84E7
    ("glGet",	X,	1,	"GL_MAX_RENDERBUFFER_SIZE"),	# 0x84E8
    ("glGet",	X,	1,	"GL_COMPRESSED_ALPHA"),	# 0x84E9
    ("glGet",	X,	1,	"GL_COMPRESSED_LUMINANCE"),	# 0x84EA
    ("glGet",	X,	1,	"GL_COMPRESSED_LUMINANCE_ALPHA"),	# 0x84EB
    ("glGet",	X,	1,	"GL_COMPRESSED_INTENSITY"),	# 0x84EC
    ("glGet",	X,	1,	"GL_COMPRESSED_RGB"),	# 0x84ED
    ("glGet",	X,	1,	"GL_COMPRESSED_RGBA"),	# 0x84EE
    ("glGet",	X,	1,	"GL_TEXTURE_COMPRESSION_HINT"),	# 0x84EF
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER"),	# 0x84F0
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER"),	# 0x84F1
    ("glGet",	X,	1,	"GL_ALL_COMPLETED_NV"),	# 0x84F2
    ("glGet",	X,	1,	"GL_FENCE_STATUS_NV"),	# 0x84F3
    ("glGet",	X,	1,	"GL_FENCE_CONDITION_NV"),	# 0x84F4
    ("glGet",	X,	1,	"GL_TEXTURE_RECTANGLE"),	# 0x84F5
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_RECTANGLE"),	# 0x84F6
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_RECTANGLE"),	# 0x84F7
    ("glGet",	X,	1,	"GL_MAX_RECTANGLE_TEXTURE_SIZE"),	# 0x84F8
    ("glGet",	X,	1,	"GL_DEPTH_STENCIL"),	# 0x84F9
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_24_8"),	# 0x84FA
    ("glGet",	X,	1,	"GL_MAX_TEXTURE_LOD_BIAS"),	# 0x84FD
    ("glGet",	X,	1,	"GL_TEXTURE_MAX_ANISOTROPY_EXT"),	# 0x84FE
    ("glGet",	X,	1,	"GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT"),	# 0x84FF
    ("glGet",	X,	1,	"GL_TEXTURE_FILTER_CONTROL"),	# 0x8500
    ("glGet",	X,	1,	"GL_TEXTURE_LOD_BIAS"),	# 0x8501
    ("glGet",	X,	1,	"GL_MODELVIEW1_STACK_DEPTH_EXT"),	# 0x8502
    ("glGet",	X,	1,	"GL_COMBINE4_NV"),	# 0x8503
    ("glGet",	X,	1,	"GL_MAX_SHININESS_NV"),	# 0x8504
    ("glGet",	X,	1,	"GL_MAX_SPOT_EXPONENT_NV"),	# 0x8505
    ("glGet",	X,	1,	"GL_MODELVIEW1_MATRIX_EXT"),	# 0x8506
    ("glGet",	X,	1,	"GL_INCR_WRAP"),	# 0x8507
    ("glGet",	X,	1,	"GL_DECR_WRAP"),	# 0x8508
    ("glGet",	X,	1,	"GL_VERTEX_WEIGHTING_EXT"),	# 0x8509
    ("glGet",	X,	1,	"GL_MODELVIEW1_ARB"),	# 0x850A
    ("glGet",	X,	1,	"GL_CURRENT_VERTEX_WEIGHT_EXT"),	# 0x850B
    ("glGet",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_EXT"),	# 0x850C
    ("glGet",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT"),	# 0x850D
    ("glGet",	E,	1,	"GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT"),	# 0x850E
    ("glGet",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT"),	# 0x850F
    ("glGet",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT"),	# 0x8510
    ("glGet",	X,	1,	"GL_NORMAL_MAP"),	# 0x8511
    ("glGet",	X,	1,	"GL_REFLECTION_MAP"),	# 0x8512
    ("glGet",	B,	1,	"GL_TEXTURE_CUBE_MAP"),	# 0x8513
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_CUBE_MAP"),	# 0x8514
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_POSITIVE_X"),	# 0x8515
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_NEGATIVE_X"),	# 0x8516
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_POSITIVE_Y"),	# 0x8517
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_NEGATIVE_Y"),	# 0x8518
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_POSITIVE_Z"),	# 0x8519
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_NEGATIVE_Z"),	# 0x851A
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_CUBE_MAP"),	# 0x851B
    ("glGet",	X,	1,	"GL_MAX_CUBE_MAP_TEXTURE_SIZE"),	# 0x851C
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_RANGE_NV"),	# 0x851D
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_RANGE_LENGTH_NV"),	# 0x851E
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_RANGE_VALID_NV"),	# 0x851F
    ("glGet",	X,	1,	"GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV"),	# 0x8520
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_RANGE_POINTER_NV"),	# 0x8521
    ("glGet",	X,	1,	"GL_REGISTER_COMBINERS_NV"),	# 0x8522
    ("glGet",	X,	1,	"GL_VARIABLE_A_NV"),	# 0x8523
    ("glGet",	X,	1,	"GL_VARIABLE_B_NV"),	# 0x8524
    ("glGet",	X,	1,	"GL_VARIABLE_C_NV"),	# 0x8525
    ("glGet",	X,	1,	"GL_VARIABLE_D_NV"),	# 0x8526
    ("glGet",	X,	1,	"GL_VARIABLE_E_NV"),	# 0x8527
    ("glGet",	X,	1,	"GL_VARIABLE_F_NV"),	# 0x8528
    ("glGet",	X,	1,	"GL_VARIABLE_G_NV"),	# 0x8529
    ("glGet",	X,	1,	"GL_CONSTANT_COLOR0_NV"),	# 0x852A
    ("glGet",	X,	1,	"GL_CONSTANT_COLOR1_NV"),	# 0x852B
    ("glGet",	X,	1,	"GL_PRIMARY_COLOR_NV"),	# 0x852C
    ("glGet",	X,	1,	"GL_SECONDARY_COLOR_NV"),	# 0x852D
    ("glGet",	X,	1,	"GL_SPARE0_NV"),	# 0x852E
    ("glGet",	X,	1,	"GL_SPARE1_NV"),	# 0x852F
    ("glGet",	X,	1,	"GL_DISCARD_NV"),	# 0x8530
    ("glGet",	X,	1,	"GL_E_TIMES_F_NV"),	# 0x8531
    ("glGet",	X,	1,	"GL_SPARE0_PLUS_SECONDARY_COLOR_NV"),	# 0x8532
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV"),	# 0x8533
    ("glGet",	X,	1,	"GL_MULTISAMPLE_FILTER_HINT_NV"),	# 0x8534
    ("glGet",	X,	1,	"GL_PER_STAGE_CONSTANTS_NV"),	# 0x8535
    ("glGet",	X,	1,	"GL_UNSIGNED_IDENTITY_NV"),	# 0x8536
    ("glGet",	X,	1,	"GL_UNSIGNED_INVERT_NV"),	# 0x8537
    ("glGet",	X,	1,	"GL_EXPAND_NORMAL_NV"),	# 0x8538
    ("glGet",	X,	1,	"GL_EXPAND_NEGATE_NV"),	# 0x8539
    ("glGet",	X,	1,	"GL_HALF_BIAS_NORMAL_NV"),	# 0x853A
    ("glGet",	X,	1,	"GL_HALF_BIAS_NEGATE_NV"),	# 0x853B
    ("glGet",	X,	1,	"GL_SIGNED_IDENTITY_NV"),	# 0x853C
    ("glGet",	X,	1,	"GL_SIGNED_NEGATE_NV"),	# 0x853D
    ("glGet",	X,	1,	"GL_SCALE_BY_TWO_NV"),	# 0x853E
    ("glGet",	X,	1,	"GL_SCALE_BY_FOUR_NV"),	# 0x853F
    ("glGet",	X,	1,	"GL_SCALE_BY_ONE_HALF_NV"),	# 0x8540
    ("glGet",	X,	1,	"GL_BIAS_BY_NEGATIVE_ONE_HALF_NV"),	# 0x8541
    ("glGet",	X,	1,	"GL_COMBINER_INPUT_NV"),	# 0x8542
    ("glGet",	X,	1,	"GL_COMBINER_MAPPING_NV"),	# 0x8543
    ("glGet",	X,	1,	"GL_COMBINER_COMPONENT_USAGE_NV"),	# 0x8544
    ("glGet",	X,	1,	"GL_COMBINER_AB_DOT_PRODUCT_NV"),	# 0x8545
    ("glGet",	X,	1,	"GL_COMBINER_CD_DOT_PRODUCT_NV"),	# 0x8546
    ("glGet",	X,	1,	"GL_COMBINER_MUX_SUM_NV"),	# 0x8547
    ("glGet",	X,	1,	"GL_COMBINER_SCALE_NV"),	# 0x8548
    ("glGet",	X,	1,	"GL_COMBINER_BIAS_NV"),	# 0x8549
    ("glGet",	X,	1,	"GL_COMBINER_AB_OUTPUT_NV"),	# 0x854A
    ("glGet",	X,	1,	"GL_COMBINER_CD_OUTPUT_NV"),	# 0x854B
    ("glGet",	X,	1,	"GL_COMBINER_SUM_OUTPUT_NV"),	# 0x854C
    ("glGet",	X,	1,	"GL_MAX_GENERAL_COMBINERS_NV"),	# 0x854D
    ("glGet",	X,	1,	"GL_NUM_GENERAL_COMBINERS_NV"),	# 0x854E
    ("glGet",	X,	1,	"GL_COLOR_SUM_CLAMP_NV"),	# 0x854F
    ("glGet",	X,	1,	"GL_COMBINER0_NV"),	# 0x8550
    ("glGet",	X,	1,	"GL_COMBINER1_NV"),	# 0x8551
    ("glGet",	X,	1,	"GL_COMBINER2_NV"),	# 0x8552
    ("glGet",	X,	1,	"GL_COMBINER3_NV"),	# 0x8553
    ("glGet",	X,	1,	"GL_COMBINER4_NV"),	# 0x8554
    ("glGet",	X,	1,	"GL_COMBINER5_NV"),	# 0x8555
    ("glGet",	X,	1,	"GL_COMBINER6_NV"),	# 0x8556
    ("glGet",	X,	1,	"GL_COMBINER7_NV"),	# 0x8557
    ("glGet",	X,	1,	"GL_PRIMITIVE_RESTART_NV"),	# 0x8558
    ("glGet",	X,	1,	"GL_PRIMITIVE_RESTART_INDEX_NV"),	# 0x8559
    ("glGet",	X,	1,	"GL_FOG_DISTANCE_MODE_NV"),	# 0x855A
    ("glGet",	X,	1,	"GL_EYE_RADIAL_NV"),	# 0x855B
    ("glGet",	X,	1,	"GL_EYE_PLANE_ABSOLUTE_NV"),	# 0x855C
    ("glGet",	X,	1,	"GL_EMBOSS_LIGHT_NV"),	# 0x855D
    ("glGet",	X,	1,	"GL_EMBOSS_CONSTANT_NV"),	# 0x855E
    ("glGet",	X,	1,	"GL_EMBOSS_MAP_NV"),	# 0x855F
    ("glGet",	X,	1,	"GL_RED_MIN_CLAMP_INGR"),	# 0x8560
    ("glGet",	X,	1,	"GL_GREEN_MIN_CLAMP_INGR"),	# 0x8561
    ("glGet",	X,	1,	"GL_BLUE_MIN_CLAMP_INGR"),	# 0x8562
    ("glGet",	X,	1,	"GL_ALPHA_MIN_CLAMP_INGR"),	# 0x8563
    ("glGet",	X,	1,	"GL_RED_MAX_CLAMP_INGR"),	# 0x8564
    ("glGet",	X,	1,	"GL_GREEN_MAX_CLAMP_INGR"),	# 0x8565
    ("glGet",	X,	1,	"GL_BLUE_MAX_CLAMP_INGR"),	# 0x8566
    ("glGet",	X,	1,	"GL_ALPHA_MAX_CLAMP_INGR"),	# 0x8567
    ("glGet",	X,	1,	"GL_INTERLACE_READ_INGR"),	# 0x8568
    ("glGet",	X,	1,	"GL_COMBINE"),	# 0x8570
    ("glGet",	X,	1,	"GL_COMBINE_RGB"),	# 0x8571
    ("glGet",	X,	1,	"GL_COMBINE_ALPHA"),	# 0x8572
    ("glGet",	X,	1,	"GL_RGB_SCALE"),	# 0x8573
    ("glGet",	X,	1,	"GL_ADD_SIGNED"),	# 0x8574
    ("glGet",	X,	1,	"GL_INTERPOLATE"),	# 0x8575
    ("glGet",	X,	1,	"GL_CONSTANT"),	# 0x8576
    ("glGet",	X,	1,	"GL_PRIMARY_COLOR"),	# 0x8577
    ("glGet",	X,	1,	"GL_PREVIOUS"),	# 0x8578
    ("glGet",	X,	1,	"GL_SRC0_RGB"),	# 0x8580
    ("glGet",	X,	1,	"GL_SRC1_RGB"),	# 0x8581
    ("glGet",	X,	1,	"GL_SRC2_RGB"),	# 0x8582
    ("glGet",	X,	1,	"GL_SOURCE3_RGB_NV"),	# 0x8583
    ("glGet",	X,	1,	"GL_SRC0_ALPHA"),	# 0x8588
    ("glGet",	X,	1,	"GL_SRC1_ALPHA"),	# 0x8589
    ("glGet",	X,	1,	"GL_SRC2_ALPHA"),	# 0x858A
    ("glGet",	X,	1,	"GL_SOURCE3_ALPHA_NV"),	# 0x858B
    ("glGet",	X,	1,	"GL_OPERAND0_RGB"),	# 0x8590
    ("glGet",	X,	1,	"GL_OPERAND1_RGB"),	# 0x8591
    ("glGet",	X,	1,	"GL_OPERAND2_RGB"),	# 0x8592
    ("glGet",	X,	1,	"GL_OPERAND0_ALPHA"),	# 0x8598
    ("glGet",	X,	1,	"GL_OPERAND1_ALPHA"),	# 0x8599
    ("glGet",	X,	1,	"GL_OPERAND2_ALPHA"),	# 0x859A
    ("glGet",	X,	1,	"GL_OPERAND3_ALPHA_NV"),	# 0x859B
    ("glGet",	X,	1,	"GL_PACK_SUBSAMPLE_RATE_SGIX"),	# 0x85A0
    ("glGet",	X,	1,	"GL_UNPACK_SUBSAMPLE_RATE_SGIX"),	# 0x85A1
    ("glGet",	X,	1,	"GL_PIXEL_SUBSAMPLE_4444_SGIX"),	# 0x85A2
    ("glGet",	X,	1,	"GL_PIXEL_SUBSAMPLE_2424_SGIX"),	# 0x85A3
    ("glGet",	X,	1,	"GL_PIXEL_SUBSAMPLE_4242_SGIX"),	# 0x85A4
    ("glGet",	X,	1,	"GL_PERTURB_EXT"),	# 0x85AE
    ("glGet",	X,	1,	"GL_TEXTURE_NORMAL_EXT"),	# 0x85AF
    ("glGet",	X,	1,	"GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE"),	# 0x85B0
    ("glGet",	X,	1,	"GL_TRANSFORM_HINT_APPLE"),	# 0x85B1
    ("glGet",	X,	1,	"GL_UNPACK_CLIENT_STORAGE_APPLE"),	# 0x85B2
    ("glGet",	X,	1,	"GL_BUFFER_OBJECT_APPLE"),	# 0x85B3
    ("glGet",	X,	1,	"GL_STORAGE_CLIENT_APPLE"),	# 0x85B4
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_BINDING"),	# 0x85B5
    ("glGet",	X,	1,	"GL_TEXTURE_RANGE_LENGTH_APPLE"),	# 0x85B7
    ("glGet",	X,	1,	"GL_TEXTURE_RANGE_POINTER_APPLE"),	# 0x85B8
    ("glGet",	X,	1,	"GL_YCBCR_422_APPLE"),	# 0x85B9
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_8_8_MESA"),	# 0x85BA
    ("glGet",	X,	1,	"GL_UNSIGNED_SHORT_8_8_REV_MESA"),	# 0x85BB
    ("glGet",	X,	1,	"GL_TEXTURE_STORAGE_HINT_APPLE"),	# 0x85BC
    ("glGet",	X,	1,	"GL_STORAGE_PRIVATE_APPLE"),	# 0x85BD
    ("glGet",	X,	1,	"GL_STORAGE_CACHED_APPLE"),	# 0x85BE
    ("glGet",	X,	1,	"GL_STORAGE_SHARED_APPLE"),	# 0x85BF
    ("glGet",	X,	1,	"GL_REPLACEMENT_CODE_ARRAY_SUN"),	# 0x85C0
    ("glGet",	E,	1,	"GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN"),	# 0x85C1
    ("glGet",	X,	1,	"GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN"),	# 0x85C2
    ("glGet",	X,	1,	"GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN"),	# 0x85C3
    ("glGet",	X,	1,	"GL_R1UI_V3F_SUN"),	# 0x85C4
    ("glGet",	X,	1,	"GL_R1UI_C4UB_V3F_SUN"),	# 0x85C5
    ("glGet",	X,	1,	"GL_R1UI_C3F_V3F_SUN"),	# 0x85C6
    ("glGet",	X,	1,	"GL_R1UI_N3F_V3F_SUN"),	# 0x85C7
    ("glGet",	X,	1,	"GL_R1UI_C4F_N3F_V3F_SUN"),	# 0x85C8
    ("glGet",	X,	1,	"GL_R1UI_T2F_V3F_SUN"),	# 0x85C9
    ("glGet",	X,	1,	"GL_R1UI_T2F_N3F_V3F_SUN"),	# 0x85CA
    ("glGet",	X,	1,	"GL_R1UI_T2F_C4F_N3F_V3F_SUN"),	# 0x85CB
    ("glGet",	X,	1,	"GL_SLICE_ACCUM_SUN"),	# 0x85CC
    ("glGet",	X,	1,	"GL_QUAD_MESH_SUN"),	# 0x8614
    ("glGet",	X,	1,	"GL_TRIANGLE_MESH_SUN"),	# 0x8615
    ("glGet",	B,	1,	"GL_VERTEX_PROGRAM_ARB"),	# 0x8620
    ("glGet",	X,	1,	"GL_VERTEX_STATE_PROGRAM_NV"),	# 0x8621
    ("glGetVertexAttrib",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY_ENABLED"),	# 0x8622
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_SIZE"),	# 0x8623
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_STRIDE"),	# 0x8624
    ("glGetVertexAttrib",	E,	1,	"GL_VERTEX_ATTRIB_ARRAY_TYPE"),	# 0x8625
    ("glGetVertexAttrib",	D,	4,	"GL_CURRENT_VERTEX_ATTRIB"),	# 0x8626
    ("glGet",	X,	1,	"GL_PROGRAM_LENGTH_ARB"),	# 0x8627
    ("glGet",	X,	1,	"GL_PROGRAM_STRING_ARB"),	# 0x8628
    ("glGet",	X,	1,	"GL_MODELVIEW_PROJECTION_NV"),	# 0x8629
    ("glGet",	X,	1,	"GL_IDENTITY_NV"),	# 0x862A
    ("glGet",	X,	1,	"GL_INVERSE_NV"),	# 0x862B
    ("glGet",	X,	1,	"GL_TRANSPOSE_NV"),	# 0x862C
    ("glGet",	X,	1,	"GL_INVERSE_TRANSPOSE_NV"),	# 0x862D
    ("glGet",	I,	1,	"GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB"),	# 0x862E
    ("glGet",	I,	1,	"GL_MAX_PROGRAM_MATRICES_ARB"),	# 0x862F
    ("glGet",	X,	1,	"GL_MATRIX0_NV"),	# 0x8630
    ("glGet",	X,	1,	"GL_MATRIX1_NV"),	# 0x8631
    ("glGet",	X,	1,	"GL_MATRIX2_NV"),	# 0x8632
    ("glGet",	X,	1,	"GL_MATRIX3_NV"),	# 0x8633
    ("glGet",	X,	1,	"GL_MATRIX4_NV"),	# 0x8634
    ("glGet",	X,	1,	"GL_MATRIX5_NV"),	# 0x8635
    ("glGet",	X,	1,	"GL_MATRIX6_NV"),	# 0x8636
    ("glGet",	X,	1,	"GL_MATRIX7_NV"),	# 0x8637
    ("glGet",	X,	1,	"GL_CURRENT_MATRIX_STACK_DEPTH_ARB"),	# 0x8640
    ("glGet",	X,	1,	"GL_CURRENT_MATRIX_ARB"),	# 0x8641
    ("glGet",	B,	1,	"GL_VERTEX_PROGRAM_POINT_SIZE"),	# 0x8642
    ("glGet",	B,	1,	"GL_VERTEX_PROGRAM_TWO_SIDE"),	# 0x8643
    ("glGet",	X,	1,	"GL_PROGRAM_PARAMETER_NV"),	# 0x8644
    ("glGetVertexAttrib",	P,	1,	"GL_VERTEX_ATTRIB_ARRAY_POINTER"),	# 0x8645
    ("glGet",	X,	1,	"GL_PROGRAM_TARGET_NV"),	# 0x8646
    ("glGet",	X,	1,	"GL_PROGRAM_RESIDENT_NV"),	# 0x8647
    ("glGet",	X,	1,	"GL_TRACK_MATRIX_NV"),	# 0x8648
    ("glGet",	X,	1,	"GL_TRACK_MATRIX_TRANSFORM_NV"),	# 0x8649
    ("glGet",	I,	1,	"GL_VERTEX_PROGRAM_BINDING_NV"),	# 0x864A
    ("glGet",	X,	1,	"GL_PROGRAM_ERROR_POSITION_ARB"),	# 0x864B
    ("glGet",	X,	1,	"GL_OFFSET_TEXTURE_RECTANGLE_NV"),	# 0x864C
    ("glGet",	X,	1,	"GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV"),	# 0x864D
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV"),	# 0x864E
    ("glGet",	X,	1,	"GL_DEPTH_CLAMP"),	# 0x864F
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY0_NV"),	# 0x8650
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY1_NV"),	# 0x8651
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY2_NV"),	# 0x8652
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY3_NV"),	# 0x8653
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY4_NV"),	# 0x8654
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY5_NV"),	# 0x8655
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY6_NV"),	# 0x8656
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY7_NV"),	# 0x8657
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY8_NV"),	# 0x8658
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY9_NV"),	# 0x8659
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY10_NV"),	# 0x865A
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY11_NV"),	# 0x865B
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY12_NV"),	# 0x865C
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY13_NV"),	# 0x865D
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY14_NV"),	# 0x865E
    ("glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY15_NV"),	# 0x865F
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB0_4_NV"),	# 0x8660
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB1_4_NV"),	# 0x8661
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB2_4_NV"),	# 0x8662
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB3_4_NV"),	# 0x8663
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB4_4_NV"),	# 0x8664
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB5_4_NV"),	# 0x8665
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB6_4_NV"),	# 0x8666
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB7_4_NV"),	# 0x8667
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB8_4_NV"),	# 0x8668
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB9_4_NV"),	# 0x8669
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB10_4_NV"),	# 0x866A
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB11_4_NV"),	# 0x866B
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB12_4_NV"),	# 0x866C
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB13_4_NV"),	# 0x866D
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB14_4_NV"),	# 0x866E
    ("glGet",	X,	1,	"GL_MAP1_VERTEX_ATTRIB15_4_NV"),	# 0x866F
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB0_4_NV"),	# 0x8670
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB1_4_NV"),	# 0x8671
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB2_4_NV"),	# 0x8672
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB3_4_NV"),	# 0x8673
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB4_4_NV"),	# 0x8674
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB5_4_NV"),	# 0x8675
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB6_4_NV"),	# 0x8676
    ("glGet",	I,	1,	"GL_PROGRAM_BINDING_ARB"),	# 0x8677
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB8_4_NV"),	# 0x8678
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB9_4_NV"),	# 0x8679
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB10_4_NV"),	# 0x867A
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB11_4_NV"),	# 0x867B
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB12_4_NV"),	# 0x867C
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB13_4_NV"),	# 0x867D
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB14_4_NV"),	# 0x867E
    ("glGet",	X,	1,	"GL_MAP2_VERTEX_ATTRIB15_4_NV"),	# 0x867F
    ("glGet",	X,	1,	"GL_TEXTURE_COMPRESSED_IMAGE_SIZE"),	# 0x86A0
    ("glGetTexLevelParameter",	B,	1,	"GL_TEXTURE_COMPRESSED"),	# 0x86A1
    ("glGet",	X,	1,	"GL_NUM_COMPRESSED_TEXTURE_FORMATS"),	# 0x86A2
    #XXX: the list is GL_NUM_COMPRESSED_TEXTURES
    #("glGet",	E,	1,	"GL_COMPRESSED_TEXTURE_FORMATS"),	# 0x86A3
    ("glGet",	X,	1,	"GL_MAX_VERTEX_UNITS_ARB"),	# 0x86A4
    ("glGet",	X,	1,	"GL_ACTIVE_VERTEX_UNITS_ARB"),	# 0x86A5
    ("glGet",	X,	1,	"GL_WEIGHT_SUM_UNITY_ARB"),	# 0x86A6
    ("glGet",	X,	1,	"GL_VERTEX_BLEND_ARB"),	# 0x86A7
    ("glGet",	X,	1,	"GL_CURRENT_WEIGHT_ARB"),	# 0x86A8
    ("glGet",	E,	1,	"GL_WEIGHT_ARRAY_TYPE_ARB"),	# 0x86A9
    ("glGet",	X,	1,	"GL_WEIGHT_ARRAY_STRIDE_ARB"),	# 0x86AA
    ("glGet",	X,	1,	"GL_WEIGHT_ARRAY_SIZE_ARB"),	# 0x86AB
    ("glGet",	X,	1,	"GL_WEIGHT_ARRAY_POINTER_ARB"),	# 0x86AC
    ("glGet",	X,	1,	"GL_WEIGHT_ARRAY_ARB"),	# 0x86AD
    ("glGet",	X,	1,	"GL_DOT3_RGB"),	# 0x86AE
    ("glGet",	X,	1,	"GL_DOT3_RGBA"),	# 0x86AF
    ("glGet",	X,	1,	"GL_COMPRESSED_RGB_FXT1_3DFX"),	# 0x86B0
    ("glGet",	X,	1,	"GL_COMPRESSED_RGBA_FXT1_3DFX"),	# 0x86B1
    ("glGet",	X,	1,	"GL_MULTISAMPLE_3DFX"),	# 0x86B2
    ("glGet",	X,	1,	"GL_SAMPLE_BUFFERS_3DFX"),	# 0x86B3
    ("glGet",	X,	1,	"GL_SAMPLES_3DFX"),	# 0x86B4
    ("glGet",	X,	1,	"GL_EVAL_2D_NV"),	# 0x86C0
    ("glGet",	X,	1,	"GL_EVAL_TRIANGULAR_2D_NV"),	# 0x86C1
    ("glGet",	X,	1,	"GL_MAP_TESSELLATION_NV"),	# 0x86C2
    ("glGet",	X,	1,	"GL_MAP_ATTRIB_U_ORDER_NV"),	# 0x86C3
    ("glGet",	X,	1,	"GL_MAP_ATTRIB_V_ORDER_NV"),	# 0x86C4
    ("glGet",	X,	1,	"GL_EVAL_FRACTIONAL_TESSELLATION_NV"),	# 0x86C5
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB0_NV"),	# 0x86C6
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB1_NV"),	# 0x86C7
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB2_NV"),	# 0x86C8
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB3_NV"),	# 0x86C9
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB4_NV"),	# 0x86CA
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB5_NV"),	# 0x86CB
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB6_NV"),	# 0x86CC
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB7_NV"),	# 0x86CD
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB8_NV"),	# 0x86CE
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB9_NV"),	# 0x86CF
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB10_NV"),	# 0x86D0
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB11_NV"),	# 0x86D1
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB12_NV"),	# 0x86D2
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB13_NV"),	# 0x86D3
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB14_NV"),	# 0x86D4
    ("glGet",	X,	1,	"GL_EVAL_VERTEX_ATTRIB15_NV"),	# 0x86D5
    ("glGet",	X,	1,	"GL_MAX_MAP_TESSELLATION_NV"),	# 0x86D6
    ("glGet",	X,	1,	"GL_MAX_RATIONAL_EVAL_ORDER_NV"),	# 0x86D7
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_PATCH_ATTRIBS_NV"),	# 0x86D8
    ("glGet",	X,	1,	"GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV"),	# 0x86D9
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_S8_S8_8_8_NV"),	# 0x86DA
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_8_8_S8_S8_REV_NV"),	# 0x86DB
    ("glGet",	X,	1,	"GL_DSDT_MAG_INTENSITY_NV"),	# 0x86DC
    ("glGet",	X,	1,	"GL_SHADER_CONSISTENT_NV"),	# 0x86DD
    ("glGet",	X,	1,	"GL_TEXTURE_SHADER_NV"),	# 0x86DE
    ("glGet",	X,	1,	"GL_SHADER_OPERATION_NV"),	# 0x86DF
    ("glGet",	X,	1,	"GL_CULL_MODES_NV"),	# 0x86E0
    ("glGet",	X,	1,	"GL_OFFSET_TEXTURE_MATRIX_NV"),	# 0x86E1
    ("glGet",	X,	1,	"GL_OFFSET_TEXTURE_SCALE_NV"),	# 0x86E2
    ("glGet",	X,	1,	"GL_OFFSET_TEXTURE_BIAS_NV"),	# 0x86E3
    ("glGet",	X,	1,	"GL_PREVIOUS_TEXTURE_INPUT_NV"),	# 0x86E4
    ("glGet",	X,	1,	"GL_CONST_EYE_NV"),	# 0x86E5
    ("glGet",	X,	1,	"GL_PASS_THROUGH_NV"),	# 0x86E6
    ("glGet",	X,	1,	"GL_CULL_FRAGMENT_NV"),	# 0x86E7
    ("glGet",	X,	1,	"GL_OFFSET_TEXTURE_2D_NV"),	# 0x86E8
    ("glGet",	X,	1,	"GL_DEPENDENT_AR_TEXTURE_2D_NV"),	# 0x86E9
    ("glGet",	X,	1,	"GL_DEPENDENT_GB_TEXTURE_2D_NV"),	# 0x86EA
    ("glGet",	X,	1,	"GL_SURFACE_STATE_NV"),	# 0x86EB
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_NV"),	# 0x86EC
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_DEPTH_REPLACE_NV"),	# 0x86ED
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_2D_NV"),	# 0x86EE
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_3D_NV"),	# 0x86EF
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV"),	# 0x86F0
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV"),	# 0x86F1
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV"),	# 0x86F2
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV"),	# 0x86F3
    ("glGet",	X,	1,	"GL_HILO_NV"),	# 0x86F4
    ("glGet",	X,	1,	"GL_DSDT_NV"),	# 0x86F5
    ("glGet",	X,	1,	"GL_DSDT_MAG_NV"),	# 0x86F6
    ("glGet",	X,	1,	"GL_DSDT_MAG_VIB_NV"),	# 0x86F7
    ("glGet",	X,	1,	"GL_HILO16_NV"),	# 0x86F8
    ("glGet",	X,	1,	"GL_SIGNED_HILO_NV"),	# 0x86F9
    ("glGet",	X,	1,	"GL_SIGNED_HILO16_NV"),	# 0x86FA
    ("glGet",	X,	1,	"GL_SIGNED_RGBA_NV"),	# 0x86FB
    ("glGet",	X,	1,	"GL_SIGNED_RGBA8_NV"),	# 0x86FC
    ("glGet",	X,	1,	"GL_SURFACE_REGISTERED_NV"),	# 0x86FD
    ("glGet",	X,	1,	"GL_SIGNED_RGB_NV"),	# 0x86FE
    ("glGet",	X,	1,	"GL_SIGNED_RGB8_NV"),	# 0x86FF
    ("glGet",	X,	1,	"GL_SURFACE_MAPPED_NV"),	# 0x8700
    ("glGet",	X,	1,	"GL_SIGNED_LUMINANCE_NV"),	# 0x8701
    ("glGet",	X,	1,	"GL_SIGNED_LUMINANCE8_NV"),	# 0x8702
    ("glGet",	X,	1,	"GL_SIGNED_LUMINANCE_ALPHA_NV"),	# 0x8703
    ("glGet",	X,	1,	"GL_SIGNED_LUMINANCE8_ALPHA8_NV"),	# 0x8704
    ("glGet",	X,	1,	"GL_SIGNED_ALPHA_NV"),	# 0x8705
    ("glGet",	X,	1,	"GL_SIGNED_ALPHA8_NV"),	# 0x8706
    ("glGet",	X,	1,	"GL_SIGNED_INTENSITY_NV"),	# 0x8707
    ("glGet",	X,	1,	"GL_SIGNED_INTENSITY8_NV"),	# 0x8708
    ("glGet",	X,	1,	"GL_DSDT8_NV"),	# 0x8709
    ("glGet",	X,	1,	"GL_DSDT8_MAG8_NV"),	# 0x870A
    ("glGet",	X,	1,	"GL_DSDT8_MAG8_INTENSITY8_NV"),	# 0x870B
    ("glGet",	X,	1,	"GL_SIGNED_RGB_UNSIGNED_ALPHA_NV"),	# 0x870C
    ("glGet",	X,	1,	"GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV"),	# 0x870D
    ("glGet",	X,	1,	"GL_HI_SCALE_NV"),	# 0x870E
    ("glGet",	X,	1,	"GL_LO_SCALE_NV"),	# 0x870F
    ("glGet",	X,	1,	"GL_DS_SCALE_NV"),	# 0x8710
    ("glGet",	X,	1,	"GL_DT_SCALE_NV"),	# 0x8711
    ("glGet",	X,	1,	"GL_MAGNITUDE_SCALE_NV"),	# 0x8712
    ("glGet",	X,	1,	"GL_VIBRANCE_SCALE_NV"),	# 0x8713
    ("glGet",	X,	1,	"GL_HI_BIAS_NV"),	# 0x8714
    ("glGet",	X,	1,	"GL_LO_BIAS_NV"),	# 0x8715
    ("glGet",	X,	1,	"GL_DS_BIAS_NV"),	# 0x8716
    ("glGet",	X,	1,	"GL_DT_BIAS_NV"),	# 0x8717
    ("glGet",	X,	1,	"GL_MAGNITUDE_BIAS_NV"),	# 0x8718
    ("glGet",	X,	1,	"GL_VIBRANCE_BIAS_NV"),	# 0x8719
    ("glGet",	X,	1,	"GL_TEXTURE_BORDER_VALUES_NV"),	# 0x871A
    ("glGet",	X,	1,	"GL_TEXTURE_HI_SIZE_NV"),	# 0x871B
    ("glGet",	X,	1,	"GL_TEXTURE_LO_SIZE_NV"),	# 0x871C
    ("glGet",	X,	1,	"GL_TEXTURE_DS_SIZE_NV"),	# 0x871D
    ("glGet",	X,	1,	"GL_TEXTURE_DT_SIZE_NV"),	# 0x871E
    ("glGet",	X,	1,	"GL_TEXTURE_MAG_SIZE_NV"),	# 0x871F
    ("glGet",	X,	1,	"GL_MODELVIEW2_ARB"),	# 0x8722
    ("glGet",	X,	1,	"GL_MODELVIEW3_ARB"),	# 0x8723
    ("glGet",	X,	1,	"GL_MODELVIEW4_ARB"),	# 0x8724
    ("glGet",	X,	1,	"GL_MODELVIEW5_ARB"),	# 0x8725
    ("glGet",	X,	1,	"GL_MODELVIEW6_ARB"),	# 0x8726
    ("glGet",	X,	1,	"GL_MODELVIEW7_ARB"),	# 0x8727
    ("glGet",	X,	1,	"GL_MODELVIEW8_ARB"),	# 0x8728
    ("glGet",	X,	1,	"GL_MODELVIEW9_ARB"),	# 0x8729
    ("glGet",	X,	1,	"GL_MODELVIEW10_ARB"),	# 0x872A
    ("glGet",	X,	1,	"GL_MODELVIEW11_ARB"),	# 0x872B
    ("glGet",	X,	1,	"GL_MODELVIEW12_ARB"),	# 0x872C
    ("glGet",	X,	1,	"GL_MODELVIEW13_ARB"),	# 0x872D
    ("glGet",	X,	1,	"GL_MODELVIEW14_ARB"),	# 0x872E
    ("glGet",	X,	1,	"GL_MODELVIEW15_ARB"),	# 0x872F
    ("glGet",	X,	1,	"GL_MODELVIEW16_ARB"),	# 0x8730
    ("glGet",	X,	1,	"GL_MODELVIEW17_ARB"),	# 0x8731
    ("glGet",	X,	1,	"GL_MODELVIEW18_ARB"),	# 0x8732
    ("glGet",	X,	1,	"GL_MODELVIEW19_ARB"),	# 0x8733
    ("glGet",	X,	1,	"GL_MODELVIEW20_ARB"),	# 0x8734
    ("glGet",	X,	1,	"GL_MODELVIEW21_ARB"),	# 0x8735
    ("glGet",	X,	1,	"GL_MODELVIEW22_ARB"),	# 0x8736
    ("glGet",	X,	1,	"GL_MODELVIEW23_ARB"),	# 0x8737
    ("glGet",	X,	1,	"GL_MODELVIEW24_ARB"),	# 0x8738
    ("glGet",	X,	1,	"GL_MODELVIEW25_ARB"),	# 0x8739
    ("glGet",	X,	1,	"GL_MODELVIEW26_ARB"),	# 0x873A
    ("glGet",	X,	1,	"GL_MODELVIEW27_ARB"),	# 0x873B
    ("glGet",	X,	1,	"GL_MODELVIEW28_ARB"),	# 0x873C
    ("glGet",	X,	1,	"GL_MODELVIEW29_ARB"),	# 0x873D
    ("glGet",	X,	1,	"GL_MODELVIEW30_ARB"),	# 0x873E
    ("glGet",	X,	1,	"GL_MODELVIEW31_ARB"),	# 0x873F
    ("glGet",	X,	1,	"GL_DOT3_RGB_EXT"),	# 0x8740
    ("glGet",	X,	1,	"GL_DOT3_RGBA_EXT"),	# 0x8741
    ("glGet",	X,	1,	"GL_MIRROR_CLAMP_ATI"),	# 0x8742
    ("glGet",	X,	1,	"GL_MIRROR_CLAMP_TO_EDGE_ATI"),	# 0x8743
    ("glGet",	X,	1,	"GL_MODULATE_ADD_ATI"),	# 0x8744
    ("glGet",	X,	1,	"GL_MODULATE_SIGNED_ADD_ATI"),	# 0x8745
    ("glGet",	X,	1,	"GL_MODULATE_SUBTRACT_ATI"),	# 0x8746
    ("glGet",	X,	1,	"GL_YCBCR_MESA"),	# 0x8757
    ("glGet",	X,	1,	"GL_PACK_INVERT_MESA"),	# 0x8758
    ("glGet",	X,	1,	"GL_TEXTURE_1D_STACK_MESAX"),	# 0x8759
    ("glGet",	X,	1,	"GL_TEXTURE_2D_STACK_MESAX"),	# 0x875A
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_1D_STACK_MESAX"),	# 0x875B
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_2D_STACK_MESAX"),	# 0x875C
    ("glGet",	X,	1,	"GL_TEXTURE_1D_STACK_BINDING_MESAX"),	# 0x875D
    ("glGet",	X,	1,	"GL_TEXTURE_2D_STACK_BINDING_MESAX"),	# 0x875E
    ("glGet",	X,	1,	"GL_STATIC_ATI"),	# 0x8760
    ("glGet",	X,	1,	"GL_DYNAMIC_ATI"),	# 0x8761
    ("glGet",	X,	1,	"GL_PRESERVE_ATI"),	# 0x8762
    ("glGet",	X,	1,	"GL_DISCARD_ATI"),	# 0x8763
    ("glGet",	X,	1,	"GL_BUFFER_SIZE"),	# 0x8764
    ("glGet",	X,	1,	"GL_BUFFER_USAGE"),	# 0x8765
    ("glGet",	X,	1,	"GL_ARRAY_OBJECT_BUFFER_ATI"),	# 0x8766
    ("glGet",	X,	1,	"GL_ARRAY_OBJECT_OFFSET_ATI"),	# 0x8767
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_ATI"),	# 0x8768
    ("glGet",	E,	1,	"GL_ELEMENT_ARRAY_TYPE_ATI"),	# 0x8769
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_POINTER_ATI"),	# 0x876A
    ("glGet",	X,	1,	"GL_MAX_VERTEX_STREAMS_ATI"),	# 0x876B
    ("glGet",	X,	1,	"GL_VERTEX_STREAM0_ATI"),	# 0x876C
    ("glGet",	X,	1,	"GL_VERTEX_STREAM1_ATI"),	# 0x876D
    ("glGet",	X,	1,	"GL_VERTEX_STREAM2_ATI"),	# 0x876E
    ("glGet",	X,	1,	"GL_VERTEX_STREAM3_ATI"),	# 0x876F
    ("glGet",	X,	1,	"GL_VERTEX_STREAM4_ATI"),	# 0x8770
    ("glGet",	X,	1,	"GL_VERTEX_STREAM5_ATI"),	# 0x8771
    ("glGet",	X,	1,	"GL_VERTEX_STREAM6_ATI"),	# 0x8772
    ("glGet",	X,	1,	"GL_VERTEX_STREAM7_ATI"),	# 0x8773
    ("glGet",	X,	1,	"GL_VERTEX_SOURCE_ATI"),	# 0x8774
    ("glGet",	X,	1,	"GL_BUMP_ROT_MATRIX_ATI"),	# 0x8775
    ("glGet",	X,	1,	"GL_BUMP_ROT_MATRIX_SIZE_ATI"),	# 0x8776
    ("glGet",	X,	1,	"GL_BUMP_NUM_TEX_UNITS_ATI"),	# 0x8777
    ("glGet",	X,	1,	"GL_BUMP_TEX_UNITS_ATI"),	# 0x8778
    ("glGet",	X,	1,	"GL_DUDV_ATI"),	# 0x8779
    ("glGet",	X,	1,	"GL_DU8DV8_ATI"),	# 0x877A
    ("glGet",	X,	1,	"GL_BUMP_ENVMAP_ATI"),	# 0x877B
    ("glGet",	X,	1,	"GL_BUMP_TARGET_ATI"),	# 0x877C
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_EXT"),	# 0x8780
    ("glGet",	I,	1,	"GL_VERTEX_SHADER_BINDING_EXT"),	# 0x8781
    ("glGet",	X,	1,	"GL_OP_INDEX_EXT"),	# 0x8782
    ("glGet",	X,	1,	"GL_OP_NEGATE_EXT"),	# 0x8783
    ("glGet",	X,	1,	"GL_OP_DOT3_EXT"),	# 0x8784
    ("glGet",	X,	1,	"GL_OP_DOT4_EXT"),	# 0x8785
    ("glGet",	X,	1,	"GL_OP_MUL_EXT"),	# 0x8786
    ("glGet",	X,	1,	"GL_OP_ADD_EXT"),	# 0x8787
    ("glGet",	X,	1,	"GL_OP_MADD_EXT"),	# 0x8788
    ("glGet",	X,	1,	"GL_OP_FRAC_EXT"),	# 0x8789
    ("glGet",	X,	1,	"GL_OP_MAX_EXT"),	# 0x878A
    ("glGet",	X,	1,	"GL_OP_MIN_EXT"),	# 0x878B
    ("glGet",	X,	1,	"GL_OP_SET_GE_EXT"),	# 0x878C
    ("glGet",	X,	1,	"GL_OP_SET_LT_EXT"),	# 0x878D
    ("glGet",	X,	1,	"GL_OP_CLAMP_EXT"),	# 0x878E
    ("glGet",	X,	1,	"GL_OP_FLOOR_EXT"),	# 0x878F
    ("glGet",	X,	1,	"GL_OP_ROUND_EXT"),	# 0x8790
    ("glGet",	X,	1,	"GL_OP_EXP_BASE_2_EXT"),	# 0x8791
    ("glGet",	X,	1,	"GL_OP_LOG_BASE_2_EXT"),	# 0x8792
    ("glGet",	X,	1,	"GL_OP_POWER_EXT"),	# 0x8793
    ("glGet",	X,	1,	"GL_OP_RECIP_EXT"),	# 0x8794
    ("glGet",	X,	1,	"GL_OP_RECIP_SQRT_EXT"),	# 0x8795
    ("glGet",	X,	1,	"GL_OP_SUB_EXT"),	# 0x8796
    ("glGet",	X,	1,	"GL_OP_CROSS_PRODUCT_EXT"),	# 0x8797
    ("glGet",	X,	1,	"GL_OP_MULTIPLY_MATRIX_EXT"),	# 0x8798
    ("glGet",	X,	1,	"GL_OP_MOV_EXT"),	# 0x8799
    ("glGet",	X,	1,	"GL_OUTPUT_VERTEX_EXT"),	# 0x879A
    ("glGet",	X,	1,	"GL_OUTPUT_COLOR0_EXT"),	# 0x879B
    ("glGet",	X,	1,	"GL_OUTPUT_COLOR1_EXT"),	# 0x879C
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD0_EXT"),	# 0x879D
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD1_EXT"),	# 0x879E
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD2_EXT"),	# 0x879F
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD3_EXT"),	# 0x87A0
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD4_EXT"),	# 0x87A1
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD5_EXT"),	# 0x87A2
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD6_EXT"),	# 0x87A3
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD7_EXT"),	# 0x87A4
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD8_EXT"),	# 0x87A5
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD9_EXT"),	# 0x87A6
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD10_EXT"),	# 0x87A7
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD11_EXT"),	# 0x87A8
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD12_EXT"),	# 0x87A9
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD13_EXT"),	# 0x87AA
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD14_EXT"),	# 0x87AB
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD15_EXT"),	# 0x87AC
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD16_EXT"),	# 0x87AD
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD17_EXT"),	# 0x87AE
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD18_EXT"),	# 0x87AF
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD19_EXT"),	# 0x87B0
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD20_EXT"),	# 0x87B1
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD21_EXT"),	# 0x87B2
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD22_EXT"),	# 0x87B3
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD23_EXT"),	# 0x87B4
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD24_EXT"),	# 0x87B5
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD25_EXT"),	# 0x87B6
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD26_EXT"),	# 0x87B7
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD27_EXT"),	# 0x87B8
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD28_EXT"),	# 0x87B9
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD29_EXT"),	# 0x87BA
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD30_EXT"),	# 0x87BB
    ("glGet",	X,	1,	"GL_OUTPUT_TEXTURE_COORD31_EXT"),	# 0x87BC
    ("glGet",	X,	1,	"GL_OUTPUT_FOG_EXT"),	# 0x87BD
    ("glGet",	X,	1,	"GL_SCALAR_EXT"),	# 0x87BE
    ("glGet",	X,	1,	"GL_VECTOR_EXT"),	# 0x87BF
    ("glGet",	X,	1,	"GL_MATRIX_EXT"),	# 0x87C0
    ("glGet",	X,	1,	"GL_VARIANT_EXT"),	# 0x87C1
    ("glGet",	X,	1,	"GL_INVARIANT_EXT"),	# 0x87C2
    ("glGet",	X,	1,	"GL_LOCAL_CONSTANT_EXT"),	# 0x87C3
    ("glGet",	X,	1,	"GL_LOCAL_EXT"),	# 0x87C4
    ("glGet",	X,	1,	"GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT"),	# 0x87C5
    ("glGet",	X,	1,	"GL_MAX_VERTEX_SHADER_VARIANTS_EXT"),	# 0x87C6
    ("glGet",	X,	1,	"GL_MAX_VERTEX_SHADER_INVARIANTS_EXT"),	# 0x87C7
    ("glGet",	X,	1,	"GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT"),	# 0x87C8
    ("glGet",	X,	1,	"GL_MAX_VERTEX_SHADER_LOCALS_EXT"),	# 0x87C9
    ("glGet",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT"),	# 0x87CA
    ("glGet",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT"),	# 0x87CB
    ("glGet",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT"),	# 0x87CC
    ("glGet",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT"),	# 0x87CD
    ("glGet",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT"),	# 0x87CE
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_INSTRUCTIONS_EXT"),	# 0x87CF
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_VARIANTS_EXT"),	# 0x87D0
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_INVARIANTS_EXT"),	# 0x87D1
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT"),	# 0x87D2
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_LOCALS_EXT"),	# 0x87D3
    ("glGet",	X,	1,	"GL_VERTEX_SHADER_OPTIMIZED_EXT"),	# 0x87D4
    ("glGet",	X,	1,	"GL_X_EXT"),	# 0x87D5
    ("glGet",	X,	1,	"GL_Y_EXT"),	# 0x87D6
    ("glGet",	X,	1,	"GL_Z_EXT"),	# 0x87D7
    ("glGet",	X,	1,	"GL_W_EXT"),	# 0x87D8
    ("glGet",	X,	1,	"GL_NEGATIVE_X_EXT"),	# 0x87D9
    ("glGet",	X,	1,	"GL_NEGATIVE_Y_EXT"),	# 0x87DA
    ("glGet",	X,	1,	"GL_NEGATIVE_Z_EXT"),	# 0x87DB
    ("glGet",	X,	1,	"GL_NEGATIVE_W_EXT"),	# 0x87DC
    ("glGet",	X,	1,	"GL_ZERO_EXT"),	# 0x87DD
    ("glGet",	X,	1,	"GL_ONE_EXT"),	# 0x87DE
    ("glGet",	X,	1,	"GL_NEGATIVE_ONE_EXT"),	# 0x87DF
    ("glGet",	X,	1,	"GL_NORMALIZED_RANGE_EXT"),	# 0x87E0
    ("glGet",	X,	1,	"GL_FULL_RANGE_EXT"),	# 0x87E1
    ("glGet",	X,	1,	"GL_CURRENT_VERTEX_EXT"),	# 0x87E2
    ("glGet",	X,	1,	"GL_MVP_MATRIX_EXT"),	# 0x87E3
    ("glGet",	X,	1,	"GL_VARIANT_VALUE_EXT"),	# 0x87E4
    ("glGet",	X,	1,	"GL_VARIANT_DATATYPE_EXT"),	# 0x87E5
    ("glGet",	X,	1,	"GL_VARIANT_ARRAY_STRIDE_EXT"),	# 0x87E6
    ("glGet",	E,	1,	"GL_VARIANT_ARRAY_TYPE_EXT"),	# 0x87E7
    ("glGet",	X,	1,	"GL_VARIANT_ARRAY_EXT"),	# 0x87E8
    ("glGet",	X,	1,	"GL_VARIANT_ARRAY_POINTER_EXT"),	# 0x87E9
    ("glGet",	X,	1,	"GL_INVARIANT_VALUE_EXT"),	# 0x87EA
    ("glGet",	X,	1,	"GL_INVARIANT_DATATYPE_EXT"),	# 0x87EB
    ("glGet",	X,	1,	"GL_LOCAL_CONSTANT_VALUE_EXT"),	# 0x87EC
    ("glGet",	X,	1,	"GL_LOCAL_CONSTANT_DATATYPE_EXT"),	# 0x87ED
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_ATI"),	# 0x87F0
    ("glGet",	X,	1,	"GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI"),	# 0x87F1
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_POINT_MODE_ATI"),	# 0x87F2
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_NORMAL_MODE_ATI"),	# 0x87F3
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI"),	# 0x87F4
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI"),	# 0x87F5
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI"),	# 0x87F6
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI"),	# 0x87F7
    ("glGet",	X,	1,	"GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI"),	# 0x87F8
    ("glGet",	X,	1,	"GL_VBO_FREE_MEMORY_ATI"),	# 0x87FB
    ("glGet",	X,	1,	"GL_TEXTURE_FREE_MEMORY_ATI"),	# 0x87FC
    ("glGet",	X,	1,	"GL_RENDERBUFFER_FREE_MEMORY_ATI"),	# 0x87FD
    ("glGet",	X,	1,	"GL_NUM_PROGRAM_BINARY_FORMATS"),	# 0x87FE
    ("glGet",	X,	1,	"GL_PROGRAM_BINARY_FORMATS"),	# 0x87FF
    ("glGet",	E,	1,	"GL_STENCIL_BACK_FUNC"),	# 0x8800
    ("glGet",	E,	1,	"GL_STENCIL_BACK_FAIL"),	# 0x8801
    ("glGet",	E,	1,	"GL_STENCIL_BACK_PASS_DEPTH_FAIL"),	# 0x8802
    ("glGet",	E,	1,	"GL_STENCIL_BACK_PASS_DEPTH_PASS"),	# 0x8803
    ("glGet",	B,	1,	"GL_FRAGMENT_PROGRAM_ARB"),	# 0x8804
    ("glGet",	X,	1,	"GL_PROGRAM_ALU_INSTRUCTIONS_ARB"),	# 0x8805
    ("glGet",	X,	1,	"GL_PROGRAM_TEX_INSTRUCTIONS_ARB"),	# 0x8806
    ("glGet",	X,	1,	"GL_PROGRAM_TEX_INDIRECTIONS_ARB"),	# 0x8807
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB"),	# 0x8808
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB"),	# 0x8809
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB"),	# 0x880A
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB"),	# 0x880B
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB"),	# 0x880C
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB"),	# 0x880D
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB"),	# 0x880E
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB"),	# 0x880F
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB"),	# 0x8810
    ("glGet",	X,	1,	"GL_RGBA32F"),	# 0x8814
    ("glGet",	X,	1,	"GL_RGB32F"),	# 0x8815
    ("glGet",	X,	1,	"GL_ALPHA32F_ARB"),	# 0x8816
    ("glGet",	X,	1,	"GL_INTENSITY32F_ARB"),	# 0x8817
    ("glGet",	X,	1,	"GL_LUMINANCE32F_ARB"),	# 0x8818
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA32F_ARB"),	# 0x8819
    ("glGet",	X,	1,	"GL_RGBA16F"),	# 0x881A
    ("glGet",	X,	1,	"GL_RGB16F"),	# 0x881B
    ("glGet",	X,	1,	"GL_ALPHA16F_ARB"),	# 0x881C
    ("glGet",	X,	1,	"GL_INTENSITY16F_ARB"),	# 0x881D
    ("glGet",	X,	1,	"GL_LUMINANCE16F_ARB"),	# 0x881E
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA16F_ARB"),	# 0x881F
    ("glGet",	X,	1,	"GL_RGBA_FLOAT_MODE_ARB"),	# 0x8820
    ("glGet",	I,	1,	"GL_MAX_DRAW_BUFFERS"),	# 0x8824
    ("glGet",	E,	1,	"GL_DRAW_BUFFER0"),	# 0x8825
    ("glGet",	E,	1,	"GL_DRAW_BUFFER1"),	# 0x8826
    ("glGet",	E,	1,	"GL_DRAW_BUFFER2"),	# 0x8827
    ("glGet",	E,	1,	"GL_DRAW_BUFFER3"),	# 0x8828
    ("glGet",	E,	1,	"GL_DRAW_BUFFER4"),	# 0x8829
    ("glGet",	E,	1,	"GL_DRAW_BUFFER5"),	# 0x882A
    ("glGet",	E,	1,	"GL_DRAW_BUFFER6"),	# 0x882B
    ("glGet",	E,	1,	"GL_DRAW_BUFFER7"),	# 0x882C
    ("glGet",	E,	1,	"GL_DRAW_BUFFER8"),	# 0x882D
    ("glGet",	E,	1,	"GL_DRAW_BUFFER9"),	# 0x882E
    ("glGet",	E,	1,	"GL_DRAW_BUFFER10"),	# 0x882F
    ("glGet",	E,	1,	"GL_DRAW_BUFFER11"),	# 0x8830
    ("glGet",	E,	1,	"GL_DRAW_BUFFER12"),	# 0x8831
    ("glGet",	E,	1,	"GL_DRAW_BUFFER13"),	# 0x8832
    ("glGet",	E,	1,	"GL_DRAW_BUFFER14"),	# 0x8833
    ("glGet",	E,	1,	"GL_DRAW_BUFFER15"),	# 0x8834
    ("glGet",	X,	1,	"GL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI"),	# 0x8835
    ("glGet",	X,	1,	"GL_BLEND_EQUATION_ALPHA"),	# 0x883D
    ("glGet",	X,	1,	"GL_MATRIX_PALETTE_ARB"),	# 0x8840
    ("glGet",	X,	1,	"GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB"),	# 0x8841
    ("glGet",	X,	1,	"GL_MAX_PALETTE_MATRICES_ARB"),	# 0x8842
    ("glGet",	X,	1,	"GL_CURRENT_PALETTE_MATRIX_ARB"),	# 0x8843
    ("glGet",	X,	1,	"GL_MATRIX_INDEX_ARRAY_ARB"),	# 0x8844
    ("glGet",	X,	1,	"GL_CURRENT_MATRIX_INDEX_ARB"),	# 0x8845
    ("glGet",	X,	1,	"GL_MATRIX_INDEX_ARRAY_SIZE_ARB"),	# 0x8846
    ("glGet",	E,	1,	"GL_MATRIX_INDEX_ARRAY_TYPE_ARB"),	# 0x8847
    ("glGet",	X,	1,	"GL_MATRIX_INDEX_ARRAY_STRIDE_ARB"),	# 0x8848
    ("glGet",	X,	1,	"GL_MATRIX_INDEX_ARRAY_POINTER_ARB"),	# 0x8849
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_DEPTH_SIZE"),	# 0x884A
    ("glGetTexParameter",	E,	1,	"GL_DEPTH_TEXTURE_MODE"),	# 0x884B
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_COMPARE_MODE"),	# 0x884C
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_COMPARE_FUNC"),	# 0x884D
    (None,	X,	1,	"GL_COMPARE_R_TO_TEXTURE"),	# 0x884E
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_SEAMLESS"),	# 0x884F
    ("glGet",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV"),	# 0x8850
    ("glGet",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV"),	# 0x8851
    ("glGet",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV"),	# 0x8852
    ("glGet",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV"),	# 0x8853
    ("glGet",	X,	1,	"GL_OFFSET_HILO_TEXTURE_2D_NV"),	# 0x8854
    ("glGet",	X,	1,	"GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV"),	# 0x8855
    ("glGet",	X,	1,	"GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV"),	# 0x8856
    ("glGet",	X,	1,	"GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV"),	# 0x8857
    ("glGet",	X,	1,	"GL_DEPENDENT_HILO_TEXTURE_2D_NV"),	# 0x8858
    ("glGet",	X,	1,	"GL_DEPENDENT_RGB_TEXTURE_3D_NV"),	# 0x8859
    ("glGet",	X,	1,	"GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV"),	# 0x885A
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_PASS_THROUGH_NV"),	# 0x885B
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_1D_NV"),	# 0x885C
    ("glGet",	X,	1,	"GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV"),	# 0x885D
    ("glGet",	X,	1,	"GL_HILO8_NV"),	# 0x885E
    ("glGet",	X,	1,	"GL_SIGNED_HILO8_NV"),	# 0x885F
    ("glGet",	X,	1,	"GL_FORCE_BLUE_TO_ONE_NV"),	# 0x8860
    ("glGet",	X,	1,	"GL_POINT_SPRITE"),	# 0x8861
    ("glGet",	X,	1,	"GL_COORD_REPLACE"),	# 0x8862
    ("glGet",	X,	1,	"GL_POINT_SPRITE_R_MODE_NV"),	# 0x8863
    ("glGet",	X,	1,	"GL_QUERY_COUNTER_BITS"),	# 0x8864
    ("glGet",	X,	1,	"GL_CURRENT_QUERY"),	# 0x8865
    ("glGet",	X,	1,	"GL_QUERY_RESULT"),	# 0x8866
    ("glGet",	X,	1,	"GL_QUERY_RESULT_AVAILABLE"),	# 0x8867
    ("glGet",	X,	1,	"GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV"),	# 0x8868
    ("glGet",	X,	1,	"GL_MAX_VERTEX_ATTRIBS"),	# 0x8869
    ("glGetVertexAttrib",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY_NORMALIZED"),	# 0x886A
    ("glGet",	X,	1,	"GL_MAX_TESS_CONTROL_INPUT_COMPONENTS"),	# 0x886C
    ("glGet",	X,	1,	"GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS"),	# 0x886D
    ("glGet",	X,	1,	"GL_DEPTH_STENCIL_TO_RGBA_NV"),	# 0x886E
    ("glGet",	X,	1,	"GL_DEPTH_STENCIL_TO_BGRA_NV"),	# 0x886F
    ("glGet",	X,	1,	"GL_FRAGMENT_PROGRAM_NV"),	# 0x8870
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_COORDS"),	# 0x8871
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_IMAGE_UNITS"),	# 0x8872
    ("glGet",	I,	1,	"GL_FRAGMENT_PROGRAM_BINDING_NV"),	# 0x8873
    ("glGet",	S,	1,	"GL_PROGRAM_ERROR_STRING_ARB"),	# 0x8874
    ("glGet",	X,	1,	"GL_PROGRAM_FORMAT_ASCII_ARB"),	# 0x8875
    ("glGet",	X,	1,	"GL_PROGRAM_FORMAT_ARB"),	# 0x8876
    ("glGet",	X,	1,	"GL_WRITE_PIXEL_DATA_RANGE_NV"),	# 0x8878
    ("glGet",	X,	1,	"GL_READ_PIXEL_DATA_RANGE_NV"),	# 0x8879
    ("glGet",	X,	1,	"GL_WRITE_PIXEL_DATA_RANGE_LENGTH_NV"),	# 0x887A
    ("glGet",	X,	1,	"GL_READ_PIXEL_DATA_RANGE_LENGTH_NV"),	# 0x887B
    ("glGet",	X,	1,	"GL_WRITE_PIXEL_DATA_RANGE_POINTER_NV"),	# 0x887C
    ("glGet",	X,	1,	"GL_READ_PIXEL_DATA_RANGE_POINTER_NV"),	# 0x887D
    ("glGet",	X,	1,	"GL_GEOMETRY_SHADER_INVOCATIONS"),	# 0x887F
    ("glGet",	X,	1,	"GL_FLOAT_R_NV"),	# 0x8880
    ("glGet",	X,	1,	"GL_FLOAT_RG_NV"),	# 0x8881
    ("glGet",	X,	1,	"GL_FLOAT_RGB_NV"),	# 0x8882
    ("glGet",	X,	1,	"GL_FLOAT_RGBA_NV"),	# 0x8883
    ("glGet",	X,	1,	"GL_FLOAT_R16_NV"),	# 0x8884
    ("glGet",	X,	1,	"GL_FLOAT_R32_NV"),	# 0x8885
    ("glGet",	X,	1,	"GL_FLOAT_RG16_NV"),	# 0x8886
    ("glGet",	X,	1,	"GL_FLOAT_RG32_NV"),	# 0x8887
    ("glGet",	X,	1,	"GL_FLOAT_RGB16_NV"),	# 0x8888
    ("glGet",	X,	1,	"GL_FLOAT_RGB32_NV"),	# 0x8889
    ("glGet",	X,	1,	"GL_FLOAT_RGBA16_NV"),	# 0x888A
    ("glGet",	X,	1,	"GL_FLOAT_RGBA32_NV"),	# 0x888B
    ("glGet",	X,	1,	"GL_TEXTURE_FLOAT_COMPONENTS_NV"),	# 0x888C
    ("glGet",	X,	1,	"GL_FLOAT_CLEAR_COLOR_VALUE_NV"),	# 0x888D
    ("glGet",	X,	1,	"GL_FLOAT_RGBA_MODE_NV"),	# 0x888E
    ("glGet",	X,	1,	"GL_TEXTURE_UNSIGNED_REMAP_MODE_NV"),	# 0x888F
    ("glGet",	X,	1,	"GL_DEPTH_BOUNDS_TEST_EXT"),	# 0x8890
    ("glGet",	X,	1,	"GL_DEPTH_BOUNDS_EXT"),	# 0x8891
    ("glGet",	X,	1,	"GL_ARRAY_BUFFER"),	# 0x8892
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_BUFFER"),	# 0x8893
    ("glGet",	I,	1,	"GL_ARRAY_BUFFER_BINDING"),	# 0x8894
    ("glGet",	I,	1,	"GL_ELEMENT_ARRAY_BUFFER_BINDING"),	# 0x8895
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_BUFFER_BINDING"),	# 0x8896
    ("glGet",	I,	1,	"GL_NORMAL_ARRAY_BUFFER_BINDING"),	# 0x8897
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_BUFFER_BINDING"),	# 0x8898
    ("glGet",	I,	1,	"GL_INDEX_ARRAY_BUFFER_BINDING"),	# 0x8899
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING"),	# 0x889A
    ("glGet",	I,	1,	"GL_EDGE_FLAG_ARRAY_BUFFER_BINDING"),	# 0x889B
    ("glGet",	I,	1,	"GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING"),	# 0x889C
    ("glGet",	I,	1,	"GL_FOG_COORD_ARRAY_BUFFER_BINDING"),	# 0x889D
    ("glGet",	I,	1,	"GL_WEIGHT_ARRAY_BUFFER_BINDING"),	# 0x889E
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"),	# 0x889F
    ("glGet",	X,	1,	"GL_PROGRAM_INSTRUCTIONS_ARB"),	# 0x88A0
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_INSTRUCTIONS_ARB"),	# 0x88A1
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB"),	# 0x88A2
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB"),	# 0x88A3
    ("glGet",	X,	1,	"GL_PROGRAM_TEMPORARIES_ARB"),	# 0x88A4
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_TEMPORARIES_ARB"),	# 0x88A5
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_TEMPORARIES_ARB"),	# 0x88A6
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB"),	# 0x88A7
    ("glGet",	X,	1,	"GL_PROGRAM_PARAMETERS_ARB"),	# 0x88A8
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_PARAMETERS_ARB"),	# 0x88A9
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_PARAMETERS_ARB"),	# 0x88AA
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB"),	# 0x88AB
    ("glGet",	X,	1,	"GL_PROGRAM_ATTRIBS_ARB"),	# 0x88AC
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_ATTRIBS_ARB"),	# 0x88AD
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_ATTRIBS_ARB"),	# 0x88AE
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB"),	# 0x88AF
    ("glGet",	X,	1,	"GL_PROGRAM_ADDRESS_REGISTERS_ARB"),	# 0x88B0
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB"),	# 0x88B1
    ("glGet",	X,	1,	"GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB"),	# 0x88B2
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB"),	# 0x88B3
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB"),	# 0x88B4
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_ENV_PARAMETERS_ARB"),	# 0x88B5
    ("glGet",	X,	1,	"GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB"),	# 0x88B6
    ("glGet",	X,	1,	"GL_TRANSPOSE_CURRENT_MATRIX_ARB"),	# 0x88B7
    ("glGet",	X,	1,	"GL_READ_ONLY"),	# 0x88B8
    ("glGet",	X,	1,	"GL_WRITE_ONLY"),	# 0x88B9
    ("glGet",	X,	1,	"GL_READ_WRITE"),	# 0x88BA
    ("glGet",	X,	1,	"GL_BUFFER_ACCESS"),	# 0x88BB
    ("glGet",	X,	1,	"GL_BUFFER_MAPPED"),	# 0x88BC
    ("glGet",	X,	1,	"GL_BUFFER_MAP_POINTER"),	# 0x88BD
    ("glGet",	X,	1,	"GL_WRITE_DISCARD_NV"),	# 0x88BE
    ("glGet",	X,	1,	"GL_TIME_ELAPSED"),	# 0x88BF
    ("glGet",	X,	1,	"GL_MATRIX0_ARB"),	# 0x88C0
    ("glGet",	X,	1,	"GL_MATRIX1_ARB"),	# 0x88C1
    ("glGet",	X,	1,	"GL_MATRIX2_ARB"),	# 0x88C2
    ("glGet",	X,	1,	"GL_MATRIX3_ARB"),	# 0x88C3
    ("glGet",	X,	1,	"GL_MATRIX4_ARB"),	# 0x88C4
    ("glGet",	X,	1,	"GL_MATRIX5_ARB"),	# 0x88C5
    ("glGet",	X,	1,	"GL_MATRIX6_ARB"),	# 0x88C6
    ("glGet",	X,	1,	"GL_MATRIX7_ARB"),	# 0x88C7
    ("glGet",	X,	1,	"GL_MATRIX8_ARB"),	# 0x88C8
    ("glGet",	X,	1,	"GL_MATRIX9_ARB"),	# 0x88C9
    ("glGet",	X,	1,	"GL_MATRIX10_ARB"),	# 0x88CA
    ("glGet",	X,	1,	"GL_MATRIX11_ARB"),	# 0x88CB
    ("glGet",	X,	1,	"GL_MATRIX12_ARB"),	# 0x88CC
    ("glGet",	X,	1,	"GL_MATRIX13_ARB"),	# 0x88CD
    ("glGet",	X,	1,	"GL_MATRIX14_ARB"),	# 0x88CE
    ("glGet",	X,	1,	"GL_MATRIX15_ARB"),	# 0x88CF
    ("glGet",	X,	1,	"GL_MATRIX16_ARB"),	# 0x88D0
    ("glGet",	X,	1,	"GL_MATRIX17_ARB"),	# 0x88D1
    ("glGet",	X,	1,	"GL_MATRIX18_ARB"),	# 0x88D2
    ("glGet",	X,	1,	"GL_MATRIX19_ARB"),	# 0x88D3
    ("glGet",	X,	1,	"GL_MATRIX20_ARB"),	# 0x88D4
    ("glGet",	X,	1,	"GL_MATRIX21_ARB"),	# 0x88D5
    ("glGet",	X,	1,	"GL_MATRIX22_ARB"),	# 0x88D6
    ("glGet",	X,	1,	"GL_MATRIX23_ARB"),	# 0x88D7
    ("glGet",	X,	1,	"GL_MATRIX24_ARB"),	# 0x88D8
    ("glGet",	X,	1,	"GL_MATRIX25_ARB"),	# 0x88D9
    ("glGet",	X,	1,	"GL_MATRIX26_ARB"),	# 0x88DA
    ("glGet",	X,	1,	"GL_MATRIX27_ARB"),	# 0x88DB
    ("glGet",	X,	1,	"GL_MATRIX28_ARB"),	# 0x88DC
    ("glGet",	X,	1,	"GL_MATRIX29_ARB"),	# 0x88DD
    ("glGet",	X,	1,	"GL_MATRIX30_ARB"),	# 0x88DE
    ("glGet",	X,	1,	"GL_MATRIX31_ARB"),	# 0x88DF
    ("glGet",	X,	1,	"GL_STREAM_DRAW"),	# 0x88E0
    ("glGet",	X,	1,	"GL_STREAM_READ"),	# 0x88E1
    ("glGet",	X,	1,	"GL_STREAM_COPY"),	# 0x88E2
    ("glGet",	X,	1,	"GL_STATIC_DRAW"),	# 0x88E4
    ("glGet",	X,	1,	"GL_STATIC_READ"),	# 0x88E5
    ("glGet",	X,	1,	"GL_STATIC_COPY"),	# 0x88E6
    ("glGet",	X,	1,	"GL_DYNAMIC_DRAW"),	# 0x88E8
    ("glGet",	X,	1,	"GL_DYNAMIC_READ"),	# 0x88E9
    ("glGet",	X,	1,	"GL_DYNAMIC_COPY"),	# 0x88EA
    ("glGet",	X,	1,	"GL_PIXEL_PACK_BUFFER"),	# 0x88EB
    ("glGet",	X,	1,	"GL_PIXEL_UNPACK_BUFFER"),	# 0x88EC
    ("glGet",	I,	1,	"GL_PIXEL_PACK_BUFFER_BINDING"),	# 0x88ED
    ("glGet",	I,	1,	"GL_PIXEL_UNPACK_BUFFER_BINDING"),	# 0x88EF
    ("glGet",	X,	1,	"GL_DEPTH24_STENCIL8"),	# 0x88F0
    ("glGet",	X,	1,	"GL_TEXTURE_STENCIL_SIZE"),	# 0x88F1
    ("glGet",	X,	1,	"GL_STENCIL_TAG_BITS_EXT"),	# 0x88F2
    ("glGet",	X,	1,	"GL_STENCIL_CLEAR_TAG_VALUE_EXT"),	# 0x88F3
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV"),	# 0x88F4
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_CALL_DEPTH_NV"),	# 0x88F5
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_IF_DEPTH_NV"),	# 0x88F6
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_LOOP_DEPTH_NV"),	# 0x88F7
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_LOOP_COUNT_NV"),	# 0x88F8
    ("glGet",	X,	1,	"GL_SRC1_COLOR"),	# 0x88F9
    ("glGet",	X,	1,	"GL_ONE_MINUS_SRC1_COLOR"),	# 0x88FA
    ("glGet",	X,	1,	"GL_ONE_MINUS_SRC1_ALPHA"),	# 0x88FB
    ("glGet",	X,	1,	"GL_MAX_DUAL_SOURCE_DRAW_BUFFERS"),	# 0x88FC
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_INTEGER"),	# 0x88FD
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_DIVISOR"),	# 0x88FE
    ("glGet",	X,	1,	"GL_MAX_ARRAY_TEXTURE_LAYERS"),	# 0x88FF
    ("glGet",	X,	1,	"GL_MIN_PROGRAM_TEXEL_OFFSET"),	# 0x8904
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_TEXEL_OFFSET"),	# 0x8905
    ("glGet",	X,	1,	"GL_PROGRAM_ATTRIB_COMPONENTS_NV"),	# 0x8906
    ("glGet",	X,	1,	"GL_PROGRAM_RESULT_COMPONENTS_NV"),	# 0x8907
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_ATTRIB_COMPONENTS_NV"),	# 0x8908
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_RESULT_COMPONENTS_NV"),	# 0x8909
    ("glGet",	X,	1,	"GL_STENCIL_TEST_TWO_SIDE_EXT"),	# 0x8910
    ("glGet",	X,	1,	"GL_ACTIVE_STENCIL_FACE_EXT"),	# 0x8911
    ("glGet",	X,	1,	"GL_MIRROR_CLAMP_TO_BORDER_EXT"),	# 0x8912
    ("glGet",	X,	1,	"GL_SAMPLES_PASSED"),	# 0x8914
    ("glGet",	X,	1,	"GL_GEOMETRY_VERTICES_OUT"),	# 0x8916
    ("glGet",	X,	1,	"GL_GEOMETRY_INPUT_TYPE"),	# 0x8917
    ("glGet",	X,	1,	"GL_GEOMETRY_OUTPUT_TYPE"),	# 0x8918
    ("glGet",	I,	1,	"GL_SAMPLER_BINDING"),	# 0x8919
    ("glGet",	X,	1,	"GL_CLAMP_VERTEX_COLOR"),	# 0x891A
    ("glGet",	X,	1,	"GL_CLAMP_FRAGMENT_COLOR"),	# 0x891B
    ("glGet",	X,	1,	"GL_CLAMP_READ_COLOR"),	# 0x891C
    ("glGet",	X,	1,	"GL_FIXED_ONLY"),	# 0x891D
    ("glGet",	X,	1,	"GL_TESS_CONTROL_PROGRAM_NV"),	# 0x891E
    ("glGet",	X,	1,	"GL_TESS_EVALUATION_PROGRAM_NV"),	# 0x891F
    ("glGet",	X,	1,	"GL_FRAGMENT_SHADER_ATI"),	# 0x8920
    ("glGet",	X,	1,	"GL_REG_0_ATI"),	# 0x8921
    ("glGet",	X,	1,	"GL_REG_1_ATI"),	# 0x8922
    ("glGet",	X,	1,	"GL_REG_2_ATI"),	# 0x8923
    ("glGet",	X,	1,	"GL_REG_3_ATI"),	# 0x8924
    ("glGet",	X,	1,	"GL_REG_4_ATI"),	# 0x8925
    ("glGet",	X,	1,	"GL_REG_5_ATI"),	# 0x8926
    ("glGet",	X,	1,	"GL_REG_6_ATI"),	# 0x8927
    ("glGet",	X,	1,	"GL_REG_7_ATI"),	# 0x8928
    ("glGet",	X,	1,	"GL_REG_8_ATI"),	# 0x8929
    ("glGet",	X,	1,	"GL_REG_9_ATI"),	# 0x892A
    ("glGet",	X,	1,	"GL_REG_10_ATI"),	# 0x892B
    ("glGet",	X,	1,	"GL_REG_11_ATI"),	# 0x892C
    ("glGet",	X,	1,	"GL_REG_12_ATI"),	# 0x892D
    ("glGet",	X,	1,	"GL_REG_13_ATI"),	# 0x892E
    ("glGet",	X,	1,	"GL_REG_14_ATI"),	# 0x892F
    ("glGet",	X,	1,	"GL_REG_15_ATI"),	# 0x8930
    ("glGet",	X,	1,	"GL_REG_16_ATI"),	# 0x8931
    ("glGet",	X,	1,	"GL_REG_17_ATI"),	# 0x8932
    ("glGet",	X,	1,	"GL_REG_18_ATI"),	# 0x8933
    ("glGet",	X,	1,	"GL_REG_19_ATI"),	# 0x8934
    ("glGet",	X,	1,	"GL_REG_20_ATI"),	# 0x8935
    ("glGet",	X,	1,	"GL_REG_21_ATI"),	# 0x8936
    ("glGet",	X,	1,	"GL_REG_22_ATI"),	# 0x8937
    ("glGet",	X,	1,	"GL_REG_23_ATI"),	# 0x8938
    ("glGet",	X,	1,	"GL_REG_24_ATI"),	# 0x8939
    ("glGet",	X,	1,	"GL_REG_25_ATI"),	# 0x893A
    ("glGet",	X,	1,	"GL_REG_26_ATI"),	# 0x893B
    ("glGet",	X,	1,	"GL_REG_27_ATI"),	# 0x893C
    ("glGet",	X,	1,	"GL_REG_28_ATI"),	# 0x893D
    ("glGet",	X,	1,	"GL_REG_29_ATI"),	# 0x893E
    ("glGet",	X,	1,	"GL_REG_30_ATI"),	# 0x893F
    ("glGet",	X,	1,	"GL_REG_31_ATI"),	# 0x8940
    ("glGet",	X,	1,	"GL_CON_0_ATI"),	# 0x8941
    ("glGet",	X,	1,	"GL_CON_1_ATI"),	# 0x8942
    ("glGet",	X,	1,	"GL_CON_2_ATI"),	# 0x8943
    ("glGet",	X,	1,	"GL_CON_3_ATI"),	# 0x8944
    ("glGet",	X,	1,	"GL_CON_4_ATI"),	# 0x8945
    ("glGet",	X,	1,	"GL_CON_5_ATI"),	# 0x8946
    ("glGet",	X,	1,	"GL_CON_6_ATI"),	# 0x8947
    ("glGet",	X,	1,	"GL_CON_7_ATI"),	# 0x8948
    ("glGet",	X,	1,	"GL_CON_8_ATI"),	# 0x8949
    ("glGet",	X,	1,	"GL_CON_9_ATI"),	# 0x894A
    ("glGet",	X,	1,	"GL_CON_10_ATI"),	# 0x894B
    ("glGet",	X,	1,	"GL_CON_11_ATI"),	# 0x894C
    ("glGet",	X,	1,	"GL_CON_12_ATI"),	# 0x894D
    ("glGet",	X,	1,	"GL_CON_13_ATI"),	# 0x894E
    ("glGet",	X,	1,	"GL_CON_14_ATI"),	# 0x894F
    ("glGet",	X,	1,	"GL_CON_15_ATI"),	# 0x8950
    ("glGet",	X,	1,	"GL_CON_16_ATI"),	# 0x8951
    ("glGet",	X,	1,	"GL_CON_17_ATI"),	# 0x8952
    ("glGet",	X,	1,	"GL_CON_18_ATI"),	# 0x8953
    ("glGet",	X,	1,	"GL_CON_19_ATI"),	# 0x8954
    ("glGet",	X,	1,	"GL_CON_20_ATI"),	# 0x8955
    ("glGet",	X,	1,	"GL_CON_21_ATI"),	# 0x8956
    ("glGet",	X,	1,	"GL_CON_22_ATI"),	# 0x8957
    ("glGet",	X,	1,	"GL_CON_23_ATI"),	# 0x8958
    ("glGet",	X,	1,	"GL_CON_24_ATI"),	# 0x8959
    ("glGet",	X,	1,	"GL_CON_25_ATI"),	# 0x895A
    ("glGet",	X,	1,	"GL_CON_26_ATI"),	# 0x895B
    ("glGet",	X,	1,	"GL_CON_27_ATI"),	# 0x895C
    ("glGet",	X,	1,	"GL_CON_28_ATI"),	# 0x895D
    ("glGet",	X,	1,	"GL_CON_29_ATI"),	# 0x895E
    ("glGet",	X,	1,	"GL_CON_30_ATI"),	# 0x895F
    ("glGet",	X,	1,	"GL_CON_31_ATI"),	# 0x8960
    ("glGet",	X,	1,	"GL_MOV_ATI"),	# 0x8961
    ("glGet",	X,	1,	"GL_ADD_ATI"),	# 0x8963
    ("glGet",	X,	1,	"GL_MUL_ATI"),	# 0x8964
    ("glGet",	X,	1,	"GL_SUB_ATI"),	# 0x8965
    ("glGet",	X,	1,	"GL_DOT3_ATI"),	# 0x8966
    ("glGet",	X,	1,	"GL_DOT4_ATI"),	# 0x8967
    ("glGet",	X,	1,	"GL_MAD_ATI"),	# 0x8968
    ("glGet",	X,	1,	"GL_LERP_ATI"),	# 0x8969
    ("glGet",	X,	1,	"GL_CND_ATI"),	# 0x896A
    ("glGet",	X,	1,	"GL_CND0_ATI"),	# 0x896B
    ("glGet",	X,	1,	"GL_DOT2_ADD_ATI"),	# 0x896C
    ("glGet",	X,	1,	"GL_SECONDARY_INTERPOLATOR_ATI"),	# 0x896D
    ("glGet",	X,	1,	"GL_NUM_FRAGMENT_REGISTERS_ATI"),	# 0x896E
    ("glGet",	X,	1,	"GL_NUM_FRAGMENT_CONSTANTS_ATI"),	# 0x896F
    ("glGet",	X,	1,	"GL_NUM_PASSES_ATI"),	# 0x8970
    ("glGet",	X,	1,	"GL_NUM_INSTRUCTIONS_PER_PASS_ATI"),	# 0x8971
    ("glGet",	X,	1,	"GL_NUM_INSTRUCTIONS_TOTAL_ATI"),	# 0x8972
    ("glGet",	X,	1,	"GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI"),	# 0x8973
    ("glGet",	X,	1,	"GL_NUM_LOOPBACK_COMPONENTS_ATI"),	# 0x8974
    ("glGet",	X,	1,	"GL_COLOR_ALPHA_PAIRING_ATI"),	# 0x8975
    ("glGet",	X,	1,	"GL_SWIZZLE_STR_ATI"),	# 0x8976
    ("glGet",	X,	1,	"GL_SWIZZLE_STQ_ATI"),	# 0x8977
    ("glGet",	X,	1,	"GL_SWIZZLE_STR_DR_ATI"),	# 0x8978
    ("glGet",	X,	1,	"GL_SWIZZLE_STQ_DQ_ATI"),	# 0x8979
    ("glGet",	X,	1,	"GL_SWIZZLE_STRQ_ATI"),	# 0x897A
    ("glGet",	X,	1,	"GL_SWIZZLE_STRQ_DQ_ATI"),	# 0x897B
    ("glGet",	X,	1,	"GL_INTERLACE_OML"),	# 0x8980
    ("glGet",	X,	1,	"GL_INTERLACE_READ_OML"),	# 0x8981
    ("glGet",	X,	1,	"GL_FORMAT_SUBSAMPLE_24_24_OML"),	# 0x8982
    ("glGet",	X,	1,	"GL_FORMAT_SUBSAMPLE_244_244_OML"),	# 0x8983
    ("glGet",	X,	1,	"GL_PACK_RESAMPLE_OML"),	# 0x8984
    ("glGet",	X,	1,	"GL_UNPACK_RESAMPLE_OML"),	# 0x8985
    ("glGet",	X,	1,	"GL_RESAMPLE_REPLICATE_OML"),	# 0x8986
    ("glGet",	X,	1,	"GL_RESAMPLE_ZERO_FILL_OML"),	# 0x8987
    ("glGet",	X,	1,	"GL_RESAMPLE_AVERAGE_OML"),	# 0x8988
    ("glGet",	X,	1,	"GL_RESAMPLE_DECIMATE_OML"),	# 0x8989
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_APPLE"),	# 0x8A00
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_APPLE"),	# 0x8A01
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_SIZE_APPLE"),	# 0x8A02
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_COEFF_APPLE"),	# 0x8A03
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_ORDER_APPLE"),	# 0x8A04
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_DOMAIN_APPLE"),	# 0x8A05
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_SIZE_APPLE"),	# 0x8A06
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_COEFF_APPLE"),	# 0x8A07
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_ORDER_APPLE"),	# 0x8A08
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_DOMAIN_APPLE"),	# 0x8A09
    ("glGet",	X,	1,	"GL_DRAW_PIXELS_APPLE"),	# 0x8A0A
    ("glGet",	X,	1,	"GL_FENCE_APPLE"),	# 0x8A0B
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_APPLE"),	# 0x8A0C
    ("glGet",	E,	1,	"GL_ELEMENT_ARRAY_TYPE_APPLE"),	# 0x8A0D
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_POINTER_APPLE"),	# 0x8A0E
    ("glGet",	X,	1,	"GL_COLOR_FLOAT_APPLE"),	# 0x8A0F
    ("glGet",	X,	1,	"GL_UNIFORM_BUFFER"),	# 0x8A11
    ("glGet",	X,	1,	"GL_BUFFER_SERIALIZED_MODIFY_APPLE"),	# 0x8A12
    ("glGet",	X,	1,	"GL_BUFFER_FLUSHING_UNMAP_APPLE"),	# 0x8A13
    ("glGet",	X,	1,	"GL_AUX_DEPTH_STENCIL_APPLE"),	# 0x8A14
    ("glGet",	X,	1,	"GL_PACK_ROW_BYTES_APPLE"),	# 0x8A15
    ("glGet",	X,	1,	"GL_UNPACK_ROW_BYTES_APPLE"),	# 0x8A16
    ("glGet",	X,	1,	"GL_RELEASED_APPLE"),	# 0x8A19
    ("glGet",	X,	1,	"GL_VOLATILE_APPLE"),	# 0x8A1A
    ("glGet",	X,	1,	"GL_RETAINED_APPLE"),	# 0x8A1B
    ("glGet",	X,	1,	"GL_UNDEFINED_APPLE"),	# 0x8A1C
    ("glGet",	X,	1,	"GL_PURGEABLE_APPLE"),	# 0x8A1D
    ("glGet",	X,	1,	"GL_RGB_422_APPLE"),	# 0x8A1F
    ("glGet",	I,	1,	"GL_UNIFORM_BUFFER_BINDING"),	# 0x8A28
    ("glGet",	X,	1,	"GL_UNIFORM_BUFFER_START"),	# 0x8A29
    ("glGet",	X,	1,	"GL_UNIFORM_BUFFER_SIZE"),	# 0x8A2A
    ("glGet",	X,	1,	"GL_MAX_VERTEX_UNIFORM_BLOCKS"),	# 0x8A2B
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_UNIFORM_BLOCKS"),	# 0x8A2C
    ("glGet",	X,	1,	"GL_MAX_FRAGMENT_UNIFORM_BLOCKS"),	# 0x8A2D
    ("glGet",	X,	1,	"GL_MAX_COMBINED_UNIFORM_BLOCKS"),	# 0x8A2E
    ("glGet",	X,	1,	"GL_MAX_UNIFORM_BUFFER_BINDINGS"),	# 0x8A2F
    ("glGet",	X,	1,	"GL_MAX_UNIFORM_BLOCK_SIZE"),	# 0x8A30
    ("glGet",	X,	1,	"GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS"),	# 0x8A31
    ("glGet",	X,	1,	"GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS"),	# 0x8A32
    ("glGet",	X,	1,	"GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS"),	# 0x8A33
    ("glGet",	X,	1,	"GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT"),	# 0x8A34
    ("glGet",	X,	1,	"GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH"),	# 0x8A35
    ("glGet",	X,	1,	"GL_ACTIVE_UNIFORM_BLOCKS"),	# 0x8A36
    ("glGet",	E,	1,	"GL_UNIFORM_TYPE"),	# 0x8A37
    ("glGet",	X,	1,	"GL_UNIFORM_SIZE"),	# 0x8A38
    ("glGet",	X,	1,	"GL_UNIFORM_NAME_LENGTH"),	# 0x8A39
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_INDEX"),	# 0x8A3A
    ("glGet",	X,	1,	"GL_UNIFORM_OFFSET"),	# 0x8A3B
    ("glGet",	X,	1,	"GL_UNIFORM_ARRAY_STRIDE"),	# 0x8A3C
    ("glGet",	X,	1,	"GL_UNIFORM_MATRIX_STRIDE"),	# 0x8A3D
    ("glGet",	X,	1,	"GL_UNIFORM_IS_ROW_MAJOR"),	# 0x8A3E
    ("glGet",	I,	1,	"GL_UNIFORM_BLOCK_BINDING"),	# 0x8A3F
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_DATA_SIZE"),	# 0x8A40
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_NAME_LENGTH"),	# 0x8A41
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS"),	# 0x8A42
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES"),	# 0x8A43
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER"),	# 0x8A44
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER"),	# 0x8A45
    ("glGet",	X,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER"),	# 0x8A46
    #"glGet",	(X,	1,	"GL_TEXTURE_SRGB_DECODE_EXT"),	# 0x8A48
    #"glGet",	(X,	1,	"GL_DECODE_EXT"),	# 0x8A49
    #"glGet",	(X,	1,	"GL_SKIP_DECODE_EXT"),	# 0x8A4A
    ("glGet",	X,	1,	"GL_FRAGMENT_SHADER"),	# 0x8B30
    ("glGet",	X,	1,	"GL_VERTEX_SHADER"),	# 0x8B31
    ("glGet",	X,	1,	"GL_PROGRAM_OBJECT_ARB"),	# 0x8B40
    ("glGet",	X,	1,	"GL_SHADER_OBJECT_ARB"),	# 0x8B48
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS"),	# 0x8B49
    ("glGet",	I,	1,	"GL_MAX_VERTEX_UNIFORM_COMPONENTS"),	# 0x8B4A
    ("glGet",	I,	1,	"GL_MAX_VARYING_COMPONENTS"),	# 0x8B4B
    ("glGet",	I,	1,	"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS"),	# 0x8B4C
    ("glGet",	I,	1,	"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS"),	# 0x8B4D
    ("glGet",	E,	1,	"GL_OBJECT_TYPE_ARB"),	# 0x8B4E
    ("glGetShader",	E,	1,	"GL_SHADER_TYPE"),	# 0x8B4F
    ("glGet",	X,	1,	"GL_FLOAT_VEC2"),	# 0x8B50
    ("glGet",	X,	1,	"GL_FLOAT_VEC3"),	# 0x8B51
    ("glGet",	X,	1,	"GL_FLOAT_VEC4"),	# 0x8B52
    ("glGet",	X,	1,	"GL_INT_VEC2"),	# 0x8B53
    ("glGet",	X,	1,	"GL_INT_VEC3"),	# 0x8B54
    ("glGet",	X,	1,	"GL_INT_VEC4"),	# 0x8B55
    ("glGet",	X,	1,	"GL_BOOL"),	# 0x8B56
    ("glGet",	X,	1,	"GL_BOOL_VEC2"),	# 0x8B57
    ("glGet",	X,	1,	"GL_BOOL_VEC3"),	# 0x8B58
    ("glGet",	X,	1,	"GL_BOOL_VEC4"),	# 0x8B59
    ("glGet",	X,	1,	"GL_FLOAT_MAT2"),	# 0x8B5A
    ("glGet",	X,	1,	"GL_FLOAT_MAT3"),	# 0x8B5B
    ("glGet",	X,	1,	"GL_FLOAT_MAT4"),	# 0x8B5C
    ("glGet",	X,	1,	"GL_SAMPLER_1D"),	# 0x8B5D
    ("glGet",	X,	1,	"GL_SAMPLER_2D"),	# 0x8B5E
    ("glGet",	X,	1,	"GL_SAMPLER_3D"),	# 0x8B5F
    ("glGet",	X,	1,	"GL_SAMPLER_CUBE"),	# 0x8B60
    ("glGet",	X,	1,	"GL_SAMPLER_1D_SHADOW"),	# 0x8B61
    ("glGet",	X,	1,	"GL_SAMPLER_2D_SHADOW"),	# 0x8B62
    ("glGet",	X,	1,	"GL_SAMPLER_2D_RECT"),	# 0x8B63
    ("glGet",	X,	1,	"GL_SAMPLER_2D_RECT_SHADOW"),	# 0x8B64
    ("glGet",	X,	1,	"GL_FLOAT_MAT2x3"),	# 0x8B65
    ("glGet",	X,	1,	"GL_FLOAT_MAT2x4"),	# 0x8B66
    ("glGet",	X,	1,	"GL_FLOAT_MAT3x2"),	# 0x8B67
    ("glGet",	X,	1,	"GL_FLOAT_MAT3x4"),	# 0x8B68
    ("glGet",	X,	1,	"GL_FLOAT_MAT4x2"),	# 0x8B69
    ("glGet",	X,	1,	"GL_FLOAT_MAT4x3"),	# 0x8B6A
    ("glGet",	X,	1,	"GL_DELETE_STATUS"),	# 0x8B80
    ("glGet",	X,	1,	"GL_COMPILE_STATUS"),	# 0x8B81
    ("glGet",	X,	1,	"GL_LINK_STATUS"),	# 0x8B82
    ("glGet",	X,	1,	"GL_VALIDATE_STATUS"),	# 0x8B83
    ("glGet",	X,	1,	"GL_INFO_LOG_LENGTH"),	# 0x8B84
    ("glGet",	X,	1,	"GL_ATTACHED_SHADERS"),	# 0x8B85
    ("glGet",	X,	1,	"GL_ACTIVE_UNIFORMS"),	# 0x8B86
    ("glGet",	X,	1,	"GL_ACTIVE_UNIFORM_MAX_LENGTH"),	# 0x8B87
    ("glGetShader",	I,	1,	"GL_SHADER_SOURCE_LENGTH"),	# 0x8B88
    ("glGet",	X,	1,	"GL_ACTIVE_ATTRIBUTES"),	# 0x8B89
    ("glGet",	X,	1,	"GL_ACTIVE_ATTRIBUTE_MAX_LENGTH"),	# 0x8B8A
    ("glGet",	X,	1,	"GL_FRAGMENT_SHADER_DERIVATIVE_HINT"),	# 0x8B8B
    ("glGet",	X,	1,	"GL_SHADING_LANGUAGE_VERSION"),	# 0x8B8C
    ("glGet",	I,	1,	"GL_CURRENT_PROGRAM"),	# 0x8B8D
    ("glGet",	X,	1,	"GL_IMPLEMENTATION_COLOR_READ_TYPE"),	# 0x8B9A
    ("glGet",	X,	1,	"GL_IMPLEMENTATION_COLOR_READ_FORMAT"),	# 0x8B9B
    ("glGet",	X,	1,	"GL_COUNTER_TYPE_AMD"),	# 0x8BC0
    ("glGet",	X,	1,	"GL_COUNTER_RANGE_AMD"),	# 0x8BC1
    ("glGet",	X,	1,	"GL_UNSIGNED_INT64_AMD"),	# 0x8BC2
    ("glGet",	X,	1,	"GL_PERCENTAGE_AMD"),	# 0x8BC3
    ("glGet",	X,	1,	"GL_PERFMON_RESULT_AVAILABLE_AMD"),	# 0x8BC4
    ("glGet",	X,	1,	"GL_PERFMON_RESULT_SIZE_AMD"),	# 0x8BC5
    ("glGet",	X,	1,	"GL_PERFMON_RESULT_AMD"),	# 0x8BC6
    ("glGet",	X,	1,	"GL_TEXTURE_RED_TYPE"),	# 0x8C10
    ("glGet",	X,	1,	"GL_TEXTURE_GREEN_TYPE"),	# 0x8C11
    ("glGet",	X,	1,	"GL_TEXTURE_BLUE_TYPE"),	# 0x8C12
    ("glGet",	X,	1,	"GL_TEXTURE_ALPHA_TYPE"),	# 0x8C13
    ("glGet",	X,	1,	"GL_TEXTURE_LUMINANCE_TYPE"),	# 0x8C14
    ("glGet",	X,	1,	"GL_TEXTURE_INTENSITY_TYPE"),	# 0x8C15
    ("glGet",	X,	1,	"GL_TEXTURE_DEPTH_TYPE"),	# 0x8C16
    ("glGet",	X,	1,	"GL_UNSIGNED_NORMALIZED"),	# 0x8C17
    ("glGet",	X,	1,	"GL_TEXTURE_1D_ARRAY"),	# 0x8C18
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_1D_ARRAY"),	# 0x8C19
    ("glGet",	X,	1,	"GL_TEXTURE_2D_ARRAY"),	# 0x8C1A
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_2D_ARRAY"),	# 0x8C1B
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_1D_ARRAY"),	# 0x8C1C
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_2D_ARRAY"),	# 0x8C1D
    ("glGet",	X,	1,	"GL_GEOMETRY_PROGRAM_NV"),	# 0x8C26
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_OUTPUT_VERTICES_NV"),	# 0x8C27
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV"),	# 0x8C28
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS"),	# 0x8C29
    ("glGet",	X,	1,	"GL_TEXTURE_BUFFER"),	# 0x8C2A
    ("glGet",	X,	1,	"GL_MAX_TEXTURE_BUFFER_SIZE"),	# 0x8C2B
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_BUFFER"),	# 0x8C2C
    ("glGet",	I,	1,	"GL_TEXTURE_BUFFER_DATA_STORE_BINDING"),	# 0x8C2D
    ("glGet",	X,	1,	"GL_TEXTURE_BUFFER_FORMAT"),	# 0x8C2E
    ("glGet",	X,	1,	"GL_ANY_SAMPLES_PASSED"),	# 0x8C2F
    ("glGet",	X,	1,	"GL_SAMPLE_SHADING"),	# 0x8C36
    ("glGet",	X,	1,	"GL_MIN_SAMPLE_SHADING_VALUE"),	# 0x8C37
    ("glGet",	X,	1,	"GL_R11F_G11F_B10F"),	# 0x8C3A
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_10F_11F_11F_REV"),	# 0x8C3B
    ("glGet",	X,	1,	"GL_RGBA_SIGNED_COMPONENTS_EXT"),	# 0x8C3C
    ("glGet",	X,	1,	"GL_RGB9_E5"),	# 0x8C3D
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_5_9_9_9_REV"),	# 0x8C3E
    ("glGet",	X,	1,	"GL_TEXTURE_SHARED_SIZE"),	# 0x8C3F
    ("glGet",	X,	1,	"GL_SRGB"),	# 0x8C40
    ("glGet",	X,	1,	"GL_SRGB8"),	# 0x8C41
    ("glGet",	X,	1,	"GL_SRGB_ALPHA"),	# 0x8C42
    ("glGet",	X,	1,	"GL_SRGB8_ALPHA8"),	# 0x8C43
    ("glGet",	X,	1,	"GL_SLUMINANCE_ALPHA"),	# 0x8C44
    ("glGet",	X,	1,	"GL_SLUMINANCE8_ALPHA8"),	# 0x8C45
    ("glGet",	X,	1,	"GL_SLUMINANCE"),	# 0x8C46
    ("glGet",	X,	1,	"GL_SLUMINANCE8"),	# 0x8C47
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB"),	# 0x8C48
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA"),	# 0x8C49
    ("glGet",	X,	1,	"GL_COMPRESSED_SLUMINANCE"),	# 0x8C4A
    ("glGet",	X,	1,	"GL_COMPRESSED_SLUMINANCE_ALPHA"),	# 0x8C4B
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB_S3TC_DXT1_EXT"),	# 0x8C4C
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT"),	# 0x8C4D
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT"),	# 0x8C4E
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT"),	# 0x8C4F
    ("glGet",	X,	1,	"GL_COMPRESSED_LUMINANCE_LATC1_EXT"),	# 0x8C70
    ("glGet",	X,	1,	"GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT"),	# 0x8C71
    ("glGet",	X,	1,	"GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT"),	# 0x8C72
    ("glGet",	X,	1,	"GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT"),	# 0x8C73
    ("glGet",	X,	1,	"GL_TESS_CONTROL_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8C74
    ("glGet",	X,	1,	"GL_TESS_EVALUATION_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8C75
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH"),	# 0x8C76
    ("glGet",	X,	1,	"GL_BACK_PRIMARY_COLOR_NV"),	# 0x8C77
    ("glGet",	X,	1,	"GL_BACK_SECONDARY_COLOR_NV"),	# 0x8C78
    ("glGet",	X,	1,	"GL_TEXTURE_COORD_NV"),	# 0x8C79
    ("glGet",	X,	1,	"GL_CLIP_DISTANCE_NV"),	# 0x8C7A
    ("glGet",	X,	1,	"GL_VERTEX_ID_NV"),	# 0x8C7B
    ("glGet",	X,	1,	"GL_PRIMITIVE_ID_NV"),	# 0x8C7C
    ("glGet",	X,	1,	"GL_GENERIC_ATTRIB_NV"),	# 0x8C7D
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_MODE"),	# 0x8C7F
    ("glGet",	X,	1,	"GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS"),	# 0x8C80
    ("glGet",	X,	1,	"GL_ACTIVE_VARYINGS_NV"),	# 0x8C81
    ("glGet",	X,	1,	"GL_ACTIVE_VARYING_MAX_LENGTH_NV"),	# 0x8C82
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_VARYINGS"),	# 0x8C83
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_START"),	# 0x8C84
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_SIZE"),	# 0x8C85
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_RECORD_NV"),	# 0x8C86
    ("glGet",	X,	1,	"GL_PRIMITIVES_GENERATED"),	# 0x8C87
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN"),	# 0x8C88
    ("glGet",	X,	1,	"GL_RASTERIZER_DISCARD"),	# 0x8C89
    ("glGet",	X,	1,	"GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS"),	# 0x8C8A
    ("glGet",	X,	1,	"GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS"),	# 0x8C8B
    ("glGet",	X,	1,	"GL_INTERLEAVED_ATTRIBS"),	# 0x8C8C
    ("glGet",	X,	1,	"GL_SEPARATE_ATTRIBS"),	# 0x8C8D
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER"),	# 0x8C8E
    ("glGet",	I,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_BINDING"),	# 0x8C8F
    ("glGet",	X,	1,	"GL_POINT_SPRITE_COORD_ORIGIN"),	# 0x8CA0
    ("glGet",	X,	1,	"GL_LOWER_LEFT"),	# 0x8CA1
    ("glGet",	X,	1,	"GL_UPPER_LEFT"),	# 0x8CA2
    ("glGet",	X,	1,	"GL_STENCIL_BACK_REF"),	# 0x8CA3
    ("glGet",	X,	1,	"GL_STENCIL_BACK_VALUE_MASK"),	# 0x8CA4
    ("glGet",	X,	1,	"GL_STENCIL_BACK_WRITEMASK"),	# 0x8CA5
    ("glGet",	I,	1,	"GL_FRAMEBUFFER_BINDING"),	# 0x8CA6
    ("glGet",	I,	1,	"GL_RENDERBUFFER_BINDING"),	# 0x8CA7
    ("glGet",	X,	1,	"GL_READ_FRAMEBUFFER"),	# 0x8CA8
    ("glGet",	X,	1,	"GL_DRAW_FRAMEBUFFER"),	# 0x8CA9
    ("glGet",	I,	1,	"GL_READ_FRAMEBUFFER_BINDING"),	# 0x8CAA
    ("glGet",	X,	1,	"GL_RENDERBUFFER_SAMPLES"),	# 0x8CAB
    ("glGet",	X,	1,	"GL_DEPTH_COMPONENT32F"),	# 0x8CAC
    ("glGet",	X,	1,	"GL_DEPTH32F_STENCIL8"),	# 0x8CAD
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"),	# 0x8CD0
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"),	# 0x8CD1
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"),	# 0x8CD2
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"),	# 0x8CD3
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER"),	# 0x8CD4
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_COMPLETE"),	# 0x8CD5
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"),	# 0x8CD6
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"),	# 0x8CD7
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT"),	# 0x8CD9
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT"),	# 0x8CDA
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"),	# 0x8CDB
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"),	# 0x8CDC
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_UNSUPPORTED"),	# 0x8CDD
    ("glGet",	X,	1,	"GL_MAX_COLOR_ATTACHMENTS"),	# 0x8CDF
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT0"),	# 0x8CE0
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT1"),	# 0x8CE1
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT2"),	# 0x8CE2
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT3"),	# 0x8CE3
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT4"),	# 0x8CE4
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT5"),	# 0x8CE5
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT6"),	# 0x8CE6
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT7"),	# 0x8CE7
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT8"),	# 0x8CE8
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT9"),	# 0x8CE9
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT10"),	# 0x8CEA
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT11"),	# 0x8CEB
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT12"),	# 0x8CEC
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT13"),	# 0x8CED
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT14"),	# 0x8CEE
    ("glGet",	X,	1,	"GL_COLOR_ATTACHMENT15"),	# 0x8CEF
    ("glGet",	X,	1,	"GL_DEPTH_ATTACHMENT"),	# 0x8D00
    ("glGet",	X,	1,	"GL_STENCIL_ATTACHMENT"),	# 0x8D20
    ("glGet",	X,	1,	"GL_FRAMEBUFFER"),	# 0x8D40
    ("glGet",	X,	1,	"GL_RENDERBUFFER"),	# 0x8D41
    ("glGet",	X,	1,	"GL_RENDERBUFFER_WIDTH"),	# 0x8D42
    ("glGet",	X,	1,	"GL_RENDERBUFFER_HEIGHT"),	# 0x8D43
    ("glGet",	X,	1,	"GL_RENDERBUFFER_INTERNAL_FORMAT"),	# 0x8D44
    ("glGet",	X,	1,	"GL_STENCIL_INDEX1"),	# 0x8D46
    ("glGet",	X,	1,	"GL_STENCIL_INDEX4"),	# 0x8D47
    ("glGet",	X,	1,	"GL_STENCIL_INDEX8"),	# 0x8D48
    ("glGet",	X,	1,	"GL_STENCIL_INDEX16"),	# 0x8D49
    ("glGet",	X,	1,	"GL_RENDERBUFFER_RED_SIZE"),	# 0x8D50
    ("glGet",	X,	1,	"GL_RENDERBUFFER_GREEN_SIZE"),	# 0x8D51
    ("glGet",	X,	1,	"GL_RENDERBUFFER_BLUE_SIZE"),	# 0x8D52
    ("glGet",	X,	1,	"GL_RENDERBUFFER_ALPHA_SIZE"),	# 0x8D53
    ("glGet",	X,	1,	"GL_RENDERBUFFER_DEPTH_SIZE"),	# 0x8D54
    ("glGet",	X,	1,	"GL_RENDERBUFFER_STENCIL_SIZE"),	# 0x8D55
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"),	# 0x8D56
    ("glGet",	X,	1,	"GL_MAX_SAMPLES"),	# 0x8D57
    ("glGet",	X,	1,	"GL_RGBA32UI"),	# 0x8D70
    ("glGet",	X,	1,	"GL_RGB32UI"),	# 0x8D71
    ("glGet",	X,	1,	"GL_ALPHA32UI_EXT"),	# 0x8D72
    ("glGet",	X,	1,	"GL_INTENSITY32UI_EXT"),	# 0x8D73
    ("glGet",	X,	1,	"GL_LUMINANCE32UI_EXT"),	# 0x8D74
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA32UI_EXT"),	# 0x8D75
    ("glGet",	X,	1,	"GL_RGBA16UI"),	# 0x8D76
    ("glGet",	X,	1,	"GL_RGB16UI"),	# 0x8D77
    ("glGet",	X,	1,	"GL_ALPHA16UI_EXT"),	# 0x8D78
    ("glGet",	X,	1,	"GL_INTENSITY16UI_EXT"),	# 0x8D79
    ("glGet",	X,	1,	"GL_LUMINANCE16UI_EXT"),	# 0x8D7A
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA16UI_EXT"),	# 0x8D7B
    ("glGet",	X,	1,	"GL_RGBA8UI"),	# 0x8D7C
    ("glGet",	X,	1,	"GL_RGB8UI"),	# 0x8D7D
    ("glGet",	X,	1,	"GL_ALPHA8UI_EXT"),	# 0x8D7E
    ("glGet",	X,	1,	"GL_INTENSITY8UI_EXT"),	# 0x8D7F
    ("glGet",	X,	1,	"GL_LUMINANCE8UI_EXT"),	# 0x8D80
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA8UI_EXT"),	# 0x8D81
    ("glGet",	X,	1,	"GL_RGBA32I"),	# 0x8D82
    ("glGet",	X,	1,	"GL_RGB32I"),	# 0x8D83
    ("glGet",	X,	1,	"GL_ALPHA32I_EXT"),	# 0x8D84
    ("glGet",	X,	1,	"GL_INTENSITY32I_EXT"),	# 0x8D85
    ("glGet",	X,	1,	"GL_LUMINANCE32I_EXT"),	# 0x8D86
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA32I_EXT"),	# 0x8D87
    ("glGet",	X,	1,	"GL_RGBA16I"),	# 0x8D88
    ("glGet",	X,	1,	"GL_RGB16I"),	# 0x8D89
    ("glGet",	X,	1,	"GL_ALPHA16I_EXT"),	# 0x8D8A
    ("glGet",	X,	1,	"GL_INTENSITY16I_EXT"),	# 0x8D8B
    ("glGet",	X,	1,	"GL_LUMINANCE16I_EXT"),	# 0x8D8C
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA16I_EXT"),	# 0x8D8D
    ("glGet",	X,	1,	"GL_RGBA8I"),	# 0x8D8E
    ("glGet",	X,	1,	"GL_RGB8I"),	# 0x8D8F
    ("glGet",	X,	1,	"GL_ALPHA8I_EXT"),	# 0x8D90
    ("glGet",	X,	1,	"GL_INTENSITY8I_EXT"),	# 0x8D91
    ("glGet",	X,	1,	"GL_LUMINANCE8I_EXT"),	# 0x8D92
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA8I_EXT"),	# 0x8D93
    ("glGet",	X,	1,	"GL_RED_INTEGER"),	# 0x8D94
    ("glGet",	X,	1,	"GL_GREEN_INTEGER"),	# 0x8D95
    ("glGet",	X,	1,	"GL_BLUE_INTEGER"),	# 0x8D96
    ("glGet",	X,	1,	"GL_ALPHA_INTEGER"),	# 0x8D97
    ("glGet",	X,	1,	"GL_RGB_INTEGER"),	# 0x8D98
    ("glGet",	X,	1,	"GL_RGBA_INTEGER"),	# 0x8D99
    ("glGet",	X,	1,	"GL_BGR_INTEGER"),	# 0x8D9A
    ("glGet",	X,	1,	"GL_BGRA_INTEGER"),	# 0x8D9B
    ("glGet",	X,	1,	"GL_LUMINANCE_INTEGER_EXT"),	# 0x8D9C
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA_INTEGER_EXT"),	# 0x8D9D
    ("glGet",	X,	1,	"GL_RGBA_INTEGER_MODE_EXT"),	# 0x8D9E
    ("glGet",	X,	1,	"GL_INT_2_10_10_10_REV"),	# 0x8D9F
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_PARAMETER_BUFFER_BINDINGS_NV"),	# 0x8DA0
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_PARAMETER_BUFFER_SIZE_NV"),	# 0x8DA1
    ("glGet",	X,	1,	"GL_VERTEX_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8DA2
    ("glGet",	X,	1,	"GL_GEOMETRY_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8DA3
    ("glGet",	X,	1,	"GL_FRAGMENT_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8DA4
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_GENERIC_ATTRIBS_NV"),	# 0x8DA5
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_GENERIC_RESULTS_NV"),	# 0x8DA6
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_LAYERED"),	# 0x8DA7
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"),	# 0x8DA8
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB"),	# 0x8DA9
    ("glGet",	X,	1,	"GL_LAYER_NV"),	# 0x8DAA
    ("glGet",	X,	1,	"GL_DEPTH_COMPONENT32F_NV"),	# 0x8DAB
    ("glGet",	X,	1,	"GL_DEPTH32F_STENCIL8_NV"),	# 0x8DAC
    ("glGet",	X,	1,	"GL_FLOAT_32_UNSIGNED_INT_24_8_REV"),	# 0x8DAD
    ("glGet",	X,	1,	"GL_SHADER_INCLUDE_ARB"),	# 0x8DAE
    ("glGet",	X,	1,	"GL_DEPTH_BUFFER_FLOAT_MODE_NV"),	# 0x8DAF
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_SRGB"),	# 0x8DB9
    ("glGet",	X,	1,	"GL_FRAMEBUFFER_SRGB_CAPABLE_EXT"),	# 0x8DBA
    ("glGet",	X,	1,	"GL_COMPRESSED_RED_RGTC1"),	# 0x8DBB
    ("glGet",	X,	1,	"GL_COMPRESSED_SIGNED_RED_RGTC1"),	# 0x8DBC
    ("glGet",	X,	1,	"GL_COMPRESSED_RG_RGTC2"),	# 0x8DBD
    ("glGet",	X,	1,	"GL_COMPRESSED_SIGNED_RG_RGTC2"),	# 0x8DBE
    ("glGet",	X,	1,	"GL_SAMPLER_1D_ARRAY"),	# 0x8DC0
    ("glGet",	X,	1,	"GL_SAMPLER_2D_ARRAY"),	# 0x8DC1
    ("glGet",	X,	1,	"GL_SAMPLER_BUFFER"),	# 0x8DC2
    ("glGet",	X,	1,	"GL_SAMPLER_1D_ARRAY_SHADOW"),	# 0x8DC3
    ("glGet",	X,	1,	"GL_SAMPLER_2D_ARRAY_SHADOW"),	# 0x8DC4
    ("glGet",	X,	1,	"GL_SAMPLER_CUBE_SHADOW"),	# 0x8DC5
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_VEC2"),	# 0x8DC6
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_VEC3"),	# 0x8DC7
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_VEC4"),	# 0x8DC8
    ("glGet",	X,	1,	"GL_INT_SAMPLER_1D"),	# 0x8DC9
    ("glGet",	X,	1,	"GL_INT_SAMPLER_2D"),	# 0x8DCA
    ("glGet",	X,	1,	"GL_INT_SAMPLER_3D"),	# 0x8DCB
    ("glGet",	X,	1,	"GL_INT_SAMPLER_CUBE"),	# 0x8DCC
    ("glGet",	X,	1,	"GL_INT_SAMPLER_2D_RECT"),	# 0x8DCD
    ("glGet",	X,	1,	"GL_INT_SAMPLER_1D_ARRAY"),	# 0x8DCE
    ("glGet",	X,	1,	"GL_INT_SAMPLER_2D_ARRAY"),	# 0x8DCF
    ("glGet",	X,	1,	"GL_INT_SAMPLER_BUFFER"),	# 0x8DD0
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_1D"),	# 0x8DD1
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D"),	# 0x8DD2
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_3D"),	# 0x8DD3
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_CUBE"),	# 0x8DD4
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_RECT"),	# 0x8DD5
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_1D_ARRAY"),	# 0x8DD6
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_ARRAY"),	# 0x8DD7
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_BUFFER"),	# 0x8DD8
    ("glGet",	X,	1,	"GL_GEOMETRY_SHADER"),	# 0x8DD9
    ("glGet",	X,	1,	"GL_GEOMETRY_VERTICES_OUT_ARB"),	# 0x8DDA
    ("glGet",	X,	1,	"GL_GEOMETRY_INPUT_TYPE_ARB"),	# 0x8DDB
    ("glGet",	X,	1,	"GL_GEOMETRY_OUTPUT_TYPE_ARB"),	# 0x8DDC
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB"),	# 0x8DDD
    ("glGet",	X,	1,	"GL_MAX_VERTEX_VARYING_COMPONENTS_ARB"),	# 0x8DDE
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_UNIFORM_COMPONENTS"),	# 0x8DDF
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_OUTPUT_VERTICES"),	# 0x8DE0
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS"),	# 0x8DE1
    ("glGet",	X,	1,	"GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT"),	# 0x8DE2
    ("glGet",	X,	1,	"GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT"),	# 0x8DE3
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT"),	# 0x8DE4
    ("glGet",	X,	1,	"GL_ACTIVE_SUBROUTINES"),	# 0x8DE5
    ("glGet",	X,	1,	"GL_ACTIVE_SUBROUTINE_UNIFORMS"),	# 0x8DE6
    ("glGet",	X,	1,	"GL_MAX_SUBROUTINES"),	# 0x8DE7
    ("glGet",	X,	1,	"GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS"),	# 0x8DE8
    ("glGet",	X,	1,	"GL_NAMED_STRING_LENGTH_ARB"),	# 0x8DE9
    ("glGet",	X,	1,	"GL_NAMED_STRING_TYPE_ARB"),	# 0x8DEA
    ("glGet",	X,	1,	"GL_MAX_BINDABLE_UNIFORM_SIZE_EXT"),	# 0x8DED
    ("glGet",	X,	1,	"GL_UNIFORM_BUFFER_EXT"),	# 0x8DEE
    ("glGet",	I,	1,	"GL_UNIFORM_BUFFER_BINDING_EXT"),	# 0x8DEF
    ("glGet",	X,	1,	"GL_LOW_FLOAT"),	# 0x8DF0
    ("glGet",	X,	1,	"GL_MEDIUM_FLOAT"),	# 0x8DF1
    ("glGet",	X,	1,	"GL_HIGH_FLOAT"),	# 0x8DF2
    ("glGet",	X,	1,	"GL_LOW_INT"),	# 0x8DF3
    ("glGet",	X,	1,	"GL_MEDIUM_INT"),	# 0x8DF4
    ("glGet",	X,	1,	"GL_HIGH_INT"),	# 0x8DF5
    ("glGet",	X,	1,	"GL_NUM_SHADER_BINARY_FORMATS"),	# 0x8DF9
    ("glGet",	X,	1,	"GL_SHADER_COMPILER"),	# 0x8DFA
    ("glGet",	X,	1,	"GL_MAX_VERTEX_UNIFORM_VECTORS"),	# 0x8DFB
    ("glGet",	X,	1,	"GL_MAX_VARYING_VECTORS"),	# 0x8DFC
    ("glGet",	X,	1,	"GL_MAX_FRAGMENT_UNIFORM_VECTORS"),	# 0x8DFD
    ("glGet",	X,	1,	"GL_RENDERBUFFER_COLOR_SAMPLES_NV"),	# 0x8E10
    ("glGet",	X,	1,	"GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV"),	# 0x8E11
    ("glGet",	X,	1,	"GL_MULTISAMPLE_COVERAGE_MODES_NV"),	# 0x8E12
    ("glGet",	X,	1,	"GL_QUERY_WAIT"),	# 0x8E13
    ("glGet",	X,	1,	"GL_QUERY_NO_WAIT"),	# 0x8E14
    ("glGet",	X,	1,	"GL_QUERY_BY_REGION_WAIT"),	# 0x8E15
    ("glGet",	X,	1,	"GL_QUERY_BY_REGION_NO_WAIT"),	# 0x8E16
    ("glGet",	X,	1,	"GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS"),	# 0x8E1E
    ("glGet",	X,	1,	"GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS"),	# 0x8E1F
    ("glGet",	X,	1,	"GL_COLOR_SAMPLES_NV"),	# 0x8E20
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK"),	# 0x8E22
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED"),	# 0x8E23
    ("glGet",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE"),	# 0x8E24
    ("glGet",	I,	1,	"GL_TRANSFORM_FEEDBACK_BINDING"),	# 0x8E25
    ("glGet",	X,	1,	"GL_FRAME_NV"),	# 0x8E26
    ("glGet",	X,	1,	"GL_FIELDS_NV"),	# 0x8E27
    ("glGet",	X,	1,	"GL_TIMESTAMP"),	# 0x8E28
    ("glGet",	X,	1,	"GL_NUM_FILL_STREAMS_NV"),	# 0x8E29
    ("glGet",	X,	1,	"GL_PRESENT_TIME_NV"),	# 0x8E2A
    ("glGet",	X,	1,	"GL_PRESENT_DURATION_NV"),	# 0x8E2B
    ("glGet",	X,	1,	"GL_PROGRAM_MATRIX_EXT"),	# 0x8E2D
    ("glGet",	X,	1,	"GL_TRANSPOSE_PROGRAM_MATRIX_EXT"),	# 0x8E2E
    ("glGet",	X,	1,	"GL_PROGRAM_MATRIX_STACK_DEPTH_EXT"),	# 0x8E2F
    ("glGet",	X,	1,	"GL_TEXTURE_SWIZZLE_R"),	# 0x8E42
    ("glGet",	X,	1,	"GL_TEXTURE_SWIZZLE_G"),	# 0x8E43
    ("glGet",	X,	1,	"GL_TEXTURE_SWIZZLE_B"),	# 0x8E44
    ("glGet",	X,	1,	"GL_TEXTURE_SWIZZLE_A"),	# 0x8E45
    ("glGet",	X,	1,	"GL_TEXTURE_SWIZZLE_RGBA"),	# 0x8E46
    ("glGet",	X,	1,	"GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS"),	# 0x8E47
    ("glGet",	X,	1,	"GL_ACTIVE_SUBROUTINE_MAX_LENGTH"),	# 0x8E48
    ("glGet",	X,	1,	"GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH"),	# 0x8E49
    ("glGet",	X,	1,	"GL_NUM_COMPATIBLE_SUBROUTINES"),	# 0x8E4A
    ("glGet",	X,	1,	"GL_COMPATIBLE_SUBROUTINES"),	# 0x8E4B
    ("glGet",	X,	1,	"GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION"),	# 0x8E4C
    ("glGet",	X,	1,	"GL_FIRST_VERTEX_CONVENTION"),	# 0x8E4D
    ("glGet",	X,	1,	"GL_LAST_VERTEX_CONVENTION"),	# 0x8E4E
    ("glGet",	X,	1,	"GL_PROVOKING_VERTEX"),	# 0x8E4F
    ("glGet",	X,	1,	"GL_SAMPLE_POSITION"),	# 0x8E50
    ("glGet",	X,	1,	"GL_SAMPLE_MASK"),	# 0x8E51
    ("glGet",	X,	1,	"GL_SAMPLE_MASK_VALUE"),	# 0x8E52
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_RENDERBUFFER_NV"),	# 0x8E53
    ("glGet",	I,	1,	"GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV"),	# 0x8E54
    ("glGet",	X,	1,	"GL_TEXTURE_RENDERBUFFER_NV"),	# 0x8E55
    ("glGet",	X,	1,	"GL_SAMPLER_RENDERBUFFER_NV"),	# 0x8E56
    ("glGet",	X,	1,	"GL_INT_SAMPLER_RENDERBUFFER_NV"),	# 0x8E57
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV"),	# 0x8E58
    ("glGet",	X,	1,	"GL_MAX_SAMPLE_MASK_WORDS"),	# 0x8E59
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_SHADER_INVOCATIONS"),	# 0x8E5A
    ("glGet",	X,	1,	"GL_MIN_FRAGMENT_INTERPOLATION_OFFSET"),	# 0x8E5B
    ("glGet",	X,	1,	"GL_MAX_FRAGMENT_INTERPOLATION_OFFSET"),	# 0x8E5C
    ("glGet",	X,	1,	"GL_FRAGMENT_INTERPOLATION_OFFSET_BITS"),	# 0x8E5D
    ("glGet",	X,	1,	"GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET"),	# 0x8E5E
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET"),	# 0x8E5F
    ("glGet",	X,	1,	"GL_MAX_TRANSFORM_FEEDBACK_BUFFERS"),	# 0x8E70
    ("glGet",	X,	1,	"GL_MAX_VERTEX_STREAMS"),	# 0x8E71
    ("glGet",	X,	1,	"GL_PATCH_VERTICES"),	# 0x8E72
    ("glGet",	X,	1,	"GL_PATCH_DEFAULT_INNER_LEVEL"),	# 0x8E73
    ("glGet",	X,	1,	"GL_PATCH_DEFAULT_OUTER_LEVEL"),	# 0x8E74
    ("glGet",	X,	1,	"GL_TESS_CONTROL_OUTPUT_VERTICES"),	# 0x8E75
    ("glGet",	X,	1,	"GL_TESS_GEN_MODE"),	# 0x8E76
    ("glGet",	X,	1,	"GL_TESS_GEN_SPACING"),	# 0x8E77
    ("glGet",	X,	1,	"GL_TESS_GEN_VERTEX_ORDER"),	# 0x8E78
    ("glGet",	X,	1,	"GL_TESS_GEN_POINT_MODE"),	# 0x8E79
    ("glGet",	X,	1,	"GL_ISOLINES"),	# 0x8E7A
    ("glGet",	X,	1,	"GL_FRACTIONAL_ODD"),	# 0x8E7B
    ("glGet",	X,	1,	"GL_FRACTIONAL_EVEN"),	# 0x8E7C
    ("glGet",	X,	1,	"GL_MAX_PATCH_VERTICES"),	# 0x8E7D
    ("glGet",	X,	1,	"GL_MAX_TESS_GEN_LEVEL"),	# 0x8E7E
    ("glGet",	X,	1,	"GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS"),	# 0x8E7F
    ("glGet",	X,	1,	"GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS"),	# 0x8E80
    ("glGet",	X,	1,	"GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS"),	# 0x8E81
    ("glGet",	X,	1,	"GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS"),	# 0x8E82
    ("glGet",	X,	1,	"GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS"),	# 0x8E83
    ("glGet",	X,	1,	"GL_MAX_TESS_PATCH_COMPONENTS"),	# 0x8E84
    ("glGet",	X,	1,	"GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS"),	# 0x8E85
    ("glGet",	X,	1,	"GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS"),	# 0x8E86
    ("glGet",	X,	1,	"GL_TESS_EVALUATION_SHADER"),	# 0x8E87
    ("glGet",	X,	1,	"GL_TESS_CONTROL_SHADER"),	# 0x8E88
    ("glGet",	X,	1,	"GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS"),	# 0x8E89
    ("glGet",	X,	1,	"GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS"),	# 0x8E8A
    ("glGet",	X,	1,	"GL_COMPRESSED_RGBA_BPTC_UNORM_ARB"),	# 0x8E8C
    ("glGet",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB"),	# 0x8E8D
    ("glGet",	X,	1,	"GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB"),	# 0x8E8E
    ("glGet",	X,	1,	"GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB"),	# 0x8E8F
    ("glGet",	X,	1,	"GL_BUFFER_GPU_ADDRESS_NV"),	# 0x8F1D
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV"),	# 0x8F1E
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_UNIFIED_NV"),	# 0x8F1F
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV"),	# 0x8F20
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_ADDRESS_NV"),	# 0x8F21
    ("glGet",	X,	1,	"GL_NORMAL_ARRAY_ADDRESS_NV"),	# 0x8F22
    ("glGet",	X,	1,	"GL_COLOR_ARRAY_ADDRESS_NV"),	# 0x8F23
    ("glGet",	X,	1,	"GL_INDEX_ARRAY_ADDRESS_NV"),	# 0x8F24
    ("glGet",	X,	1,	"GL_TEXTURE_COORD_ARRAY_ADDRESS_NV"),	# 0x8F25
    ("glGet",	X,	1,	"GL_EDGE_FLAG_ARRAY_ADDRESS_NV"),	# 0x8F26
    ("glGet",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_ADDRESS_NV"),	# 0x8F27
    ("glGet",	X,	1,	"GL_FOG_COORD_ARRAY_ADDRESS_NV"),	# 0x8F28
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_ADDRESS_NV"),	# 0x8F29
    ("glGet",	X,	1,	"GL_VERTEX_ATTRIB_ARRAY_LENGTH_NV"),	# 0x8F2A
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_LENGTH_NV"),	# 0x8F2B
    ("glGet",	X,	1,	"GL_NORMAL_ARRAY_LENGTH_NV"),	# 0x8F2C
    ("glGet",	X,	1,	"GL_COLOR_ARRAY_LENGTH_NV"),	# 0x8F2D
    ("glGet",	X,	1,	"GL_INDEX_ARRAY_LENGTH_NV"),	# 0x8F2E
    ("glGet",	X,	1,	"GL_TEXTURE_COORD_ARRAY_LENGTH_NV"),	# 0x8F2F
    ("glGet",	X,	1,	"GL_EDGE_FLAG_ARRAY_LENGTH_NV"),	# 0x8F30
    ("glGet",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_LENGTH_NV"),	# 0x8F31
    ("glGet",	X,	1,	"GL_FOG_COORD_ARRAY_LENGTH_NV"),	# 0x8F32
    ("glGet",	X,	1,	"GL_ELEMENT_ARRAY_LENGTH_NV"),	# 0x8F33
    ("glGet",	X,	1,	"GL_GPU_ADDRESS_NV"),	# 0x8F34
    ("glGet",	X,	1,	"GL_MAX_SHADER_BUFFER_ADDRESS_NV"),	# 0x8F35
    ("glGet",	X,	1,	"GL_COPY_READ_BUFFER"),	# 0x8F36
    ("glGet",	X,	1,	"GL_COPY_WRITE_BUFFER"),	# 0x8F37
    ("glGet",	X,	1,	"GL_MAX_IMAGE_UNITS_EXT"),	# 0x8F38
    ("glGet",	X,	1,	"GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS_EXT"),	# 0x8F39
    ("glGet",	X,	1,	"GL_IMAGE_BINDING_NAME_EXT"),	# 0x8F3A
    ("glGet",	X,	1,	"GL_IMAGE_BINDING_LEVEL_EXT"),	# 0x8F3B
    ("glGet",	X,	1,	"GL_IMAGE_BINDING_LAYERED_EXT"),	# 0x8F3C
    ("glGet",	X,	1,	"GL_IMAGE_BINDING_LAYER_EXT"),	# 0x8F3D
    ("glGet",	X,	1,	"GL_IMAGE_BINDING_ACCESS_EXT"),	# 0x8F3E
    ("glGet",	X,	1,	"GL_DRAW_INDIRECT_BUFFER"),	# 0x8F3F
    ("glGet",	X,	1,	"GL_DRAW_INDIRECT_UNIFIED_NV"),	# 0x8F40
    ("glGet",	X,	1,	"GL_DRAW_INDIRECT_ADDRESS_NV"),	# 0x8F41
    ("glGet",	X,	1,	"GL_DRAW_INDIRECT_LENGTH_NV"),	# 0x8F42
    ("glGet",	I,	1,	"GL_DRAW_INDIRECT_BUFFER_BINDING"),	# 0x8F43
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_SUBROUTINE_PARAMETERS_NV"),	# 0x8F44
    ("glGet",	X,	1,	"GL_MAX_PROGRAM_SUBROUTINE_NUM_NV"),	# 0x8F45
    ("glGet",	X,	1,	"GL_DOUBLE_MAT2"),	# 0x8F46
    ("glGet",	X,	1,	"GL_DOUBLE_MAT3"),	# 0x8F47
    ("glGet",	X,	1,	"GL_DOUBLE_MAT4"),	# 0x8F48
    ("glGet",	X,	1,	"GL_DOUBLE_MAT2x3"),	# 0x8F49
    ("glGet",	X,	1,	"GL_DOUBLE_MAT2x4"),	# 0x8F4A
    ("glGet",	X,	1,	"GL_DOUBLE_MAT3x2"),	# 0x8F4B
    ("glGet",	X,	1,	"GL_DOUBLE_MAT3x4"),	# 0x8F4C
    ("glGet",	X,	1,	"GL_DOUBLE_MAT4x2"),	# 0x8F4D
    ("glGet",	X,	1,	"GL_DOUBLE_MAT4x3"),	# 0x8F4E
    ("glGet",	X,	1,	"GL_RED_SNORM"),	# 0x8F90
    ("glGet",	X,	1,	"GL_RG_SNORM"),	# 0x8F91
    ("glGet",	X,	1,	"GL_RGB_SNORM"),	# 0x8F92
    ("glGet",	X,	1,	"GL_RGBA_SNORM"),	# 0x8F93
    ("glGet",	X,	1,	"GL_R8_SNORM"),	# 0x8F94
    ("glGet",	X,	1,	"GL_RG8_SNORM"),	# 0x8F95
    ("glGet",	X,	1,	"GL_RGB8_SNORM"),	# 0x8F96
    ("glGet",	X,	1,	"GL_RGBA8_SNORM"),	# 0x8F97
    ("glGet",	X,	1,	"GL_R16_SNORM"),	# 0x8F98
    ("glGet",	X,	1,	"GL_RG16_SNORM"),	# 0x8F99
    ("glGet",	X,	1,	"GL_RGB16_SNORM"),	# 0x8F9A
    ("glGet",	X,	1,	"GL_RGBA16_SNORM"),	# 0x8F9B
    ("glGet",	X,	1,	"GL_SIGNED_NORMALIZED"),	# 0x8F9C
    ("glGet",	X,	1,	"GL_PRIMITIVE_RESTART"),	# 0x8F9D
    ("glGet",	X,	1,	"GL_PRIMITIVE_RESTART_INDEX"),	# 0x8F9E
    ("glGet",	X,	1,	"GL_INT8_NV"),	# 0x8FE0
    ("glGet",	X,	1,	"GL_INT8_VEC2_NV"),	# 0x8FE1
    ("glGet",	X,	1,	"GL_INT8_VEC3_NV"),	# 0x8FE2
    ("glGet",	X,	1,	"GL_INT8_VEC4_NV"),	# 0x8FE3
    ("glGet",	X,	1,	"GL_INT16_NV"),	# 0x8FE4
    ("glGet",	X,	1,	"GL_INT16_VEC2_NV"),	# 0x8FE5
    ("glGet",	X,	1,	"GL_INT16_VEC3_NV"),	# 0x8FE6
    ("glGet",	X,	1,	"GL_INT16_VEC4_NV"),	# 0x8FE7
    ("glGet",	X,	1,	"GL_INT64_VEC2_NV"),	# 0x8FE9
    ("glGet",	X,	1,	"GL_INT64_VEC3_NV"),	# 0x8FEA
    ("glGet",	X,	1,	"GL_INT64_VEC4_NV"),	# 0x8FEB
    ("glGet",	X,	1,	"GL_UNSIGNED_INT8_NV"),	# 0x8FEC
    ("glGet",	X,	1,	"GL_UNSIGNED_INT8_VEC2_NV"),	# 0x8FED
    ("glGet",	X,	1,	"GL_UNSIGNED_INT8_VEC3_NV"),	# 0x8FEE
    ("glGet",	X,	1,	"GL_UNSIGNED_INT8_VEC4_NV"),	# 0x8FEF
    ("glGet",	X,	1,	"GL_UNSIGNED_INT16_NV"),	# 0x8FF0
    ("glGet",	X,	1,	"GL_UNSIGNED_INT16_VEC2_NV"),	# 0x8FF1
    ("glGet",	X,	1,	"GL_UNSIGNED_INT16_VEC3_NV"),	# 0x8FF2
    ("glGet",	X,	1,	"GL_UNSIGNED_INT16_VEC4_NV"),	# 0x8FF3
    ("glGet",	X,	1,	"GL_UNSIGNED_INT64_VEC2_NV"),	# 0x8FF5
    ("glGet",	X,	1,	"GL_UNSIGNED_INT64_VEC3_NV"),	# 0x8FF6
    ("glGet",	X,	1,	"GL_UNSIGNED_INT64_VEC4_NV"),	# 0x8FF7
    ("glGet",	X,	1,	"GL_FLOAT16_NV"),	# 0x8FF8
    ("glGet",	X,	1,	"GL_FLOAT16_VEC2_NV"),	# 0x8FF9
    ("glGet",	X,	1,	"GL_FLOAT16_VEC3_NV"),	# 0x8FFA
    ("glGet",	X,	1,	"GL_FLOAT16_VEC4_NV"),	# 0x8FFB
    ("glGet",	X,	1,	"GL_DOUBLE_VEC2"),	# 0x8FFC
    ("glGet",	X,	1,	"GL_DOUBLE_VEC3"),	# 0x8FFD
    ("glGet",	X,	1,	"GL_DOUBLE_VEC4"),	# 0x8FFE
    ("glGet",	X,	1,	"GL_SAMPLER_BUFFER_AMD"),	# 0x9001
    ("glGet",	X,	1,	"GL_INT_SAMPLER_BUFFER_AMD"),	# 0x9002
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_BUFFER_AMD"),	# 0x9003
    ("glGet",	X,	1,	"GL_TESSELLATION_MODE_AMD"),	# 0x9004
    ("glGet",	X,	1,	"GL_TESSELLATION_FACTOR_AMD"),	# 0x9005
    ("glGet",	X,	1,	"GL_DISCRETE_AMD"),	# 0x9006
    ("glGet",	X,	1,	"GL_CONTINUOUS_AMD"),	# 0x9007
    ("glGet",	X,	1,	"GL_TEXTURE_CUBE_MAP_ARRAY"),	# 0x9009
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_CUBE_MAP_ARRAY"),	# 0x900A
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_CUBE_MAP_ARRAY"),	# 0x900B
    ("glGet",	X,	1,	"GL_SAMPLER_CUBE_MAP_ARRAY"),	# 0x900C
    ("glGet",	X,	1,	"GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW"),	# 0x900D
    ("glGet",	X,	1,	"GL_INT_SAMPLER_CUBE_MAP_ARRAY"),	# 0x900E
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY"),	# 0x900F
    ("glGet",	X,	1,	"GL_ALPHA_SNORM"),	# 0x9010
    ("glGet",	X,	1,	"GL_LUMINANCE_SNORM"),	# 0x9011
    ("glGet",	X,	1,	"GL_LUMINANCE_ALPHA_SNORM"),	# 0x9012
    ("glGet",	X,	1,	"GL_INTENSITY_SNORM"),	# 0x9013
    ("glGet",	X,	1,	"GL_ALPHA8_SNORM"),	# 0x9014
    ("glGet",	X,	1,	"GL_LUMINANCE8_SNORM"),	# 0x9015
    ("glGet",	X,	1,	"GL_LUMINANCE8_ALPHA8_SNORM"),	# 0x9016
    ("glGet",	X,	1,	"GL_INTENSITY8_SNORM"),	# 0x9017
    ("glGet",	X,	1,	"GL_ALPHA16_SNORM"),	# 0x9018
    ("glGet",	X,	1,	"GL_LUMINANCE16_SNORM"),	# 0x9019
    ("glGet",	X,	1,	"GL_LUMINANCE16_ALPHA16_SNORM"),	# 0x901A
    ("glGet",	X,	1,	"GL_INTENSITY16_SNORM"),	# 0x901B
    ("glGet",	X,	1,	"GL_DEPTH_CLAMP_NEAR_AMD"),	# 0x901E
    ("glGet",	X,	1,	"GL_DEPTH_CLAMP_FAR_AMD"),	# 0x901F
    ("glGet",	X,	1,	"GL_VIDEO_BUFFER_NV"),	# 0x9020
    ("glGet",	I,	1,	"GL_VIDEO_BUFFER_BINDING_NV"),	# 0x9021
    ("glGet",	X,	1,	"GL_FIELD_UPPER_NV"),	# 0x9022
    ("glGet",	X,	1,	"GL_FIELD_LOWER_NV"),	# 0x9023
    ("glGet",	X,	1,	"GL_NUM_VIDEO_CAPTURE_STREAMS_NV"),	# 0x9024
    ("glGet",	X,	1,	"GL_NEXT_VIDEO_CAPTURE_BUFFER_STATUS_NV"),	# 0x9025
    ("glGet",	X,	1,	"GL_VIDEO_CAPTURE_TO_422_SUPPORTED_NV"),	# 0x9026
    ("glGet",	X,	1,	"GL_LAST_VIDEO_CAPTURE_STATUS_NV"),	# 0x9027
    ("glGet",	X,	1,	"GL_VIDEO_BUFFER_PITCH_NV"),	# 0x9028
    ("glGet",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_MATRIX_NV"),	# 0x9029
    ("glGet",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_MAX_NV"),	# 0x902A
    ("glGet",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_MIN_NV"),	# 0x902B
    ("glGet",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_OFFSET_NV"),	# 0x902C
    ("glGet",	X,	1,	"GL_VIDEO_BUFFER_INTERNAL_FORMAT_NV"),	# 0x902D
    ("glGet",	X,	1,	"GL_PARTIAL_SUCCESS_NV"),	# 0x902E
    ("glGet",	X,	1,	"GL_SUCCESS_NV"),	# 0x902F
    ("glGet",	X,	1,	"GL_FAILURE_NV"),	# 0x9030
    ("glGet",	X,	1,	"GL_YCBYCR8_422_NV"),	# 0x9031
    ("glGet",	X,	1,	"GL_YCBAYCR8A_4224_NV"),	# 0x9032
    ("glGet",	X,	1,	"GL_Z6Y10Z6CB10Z6Y10Z6CR10_422_NV"),	# 0x9033
    ("glGet",	X,	1,	"GL_Z6Y10Z6CB10Z6A10Z6Y10Z6CR10Z6A10_4224_NV"),	# 0x9034
    ("glGet",	X,	1,	"GL_Z4Y12Z4CB12Z4Y12Z4CR12_422_NV"),	# 0x9035
    ("glGet",	X,	1,	"GL_Z4Y12Z4CB12Z4A12Z4Y12Z4CR12Z4A12_4224_NV"),	# 0x9036
    ("glGet",	X,	1,	"GL_Z4Y12Z4CB12Z4CR12_444_NV"),	# 0x9037
    ("glGet",	X,	1,	"GL_VIDEO_CAPTURE_FRAME_WIDTH_NV"),	# 0x9038
    ("glGet",	X,	1,	"GL_VIDEO_CAPTURE_FRAME_HEIGHT_NV"),	# 0x9039
    ("glGet",	X,	1,	"GL_VIDEO_CAPTURE_FIELD_UPPER_HEIGHT_NV"),	# 0x903A
    ("glGet",	X,	1,	"GL_VIDEO_CAPTURE_FIELD_LOWER_HEIGHT_NV"),	# 0x903B
    ("glGet",	X,	1,	"GL_VIDEO_CAPTURE_SURFACE_ORIGIN_NV"),	# 0x903C
    ("glGet",	X,	1,	"GL_IMAGE_1D_EXT"),	# 0x904C
    ("glGet",	X,	1,	"GL_IMAGE_2D_EXT"),	# 0x904D
    ("glGet",	X,	1,	"GL_IMAGE_3D_EXT"),	# 0x904E
    ("glGet",	X,	1,	"GL_IMAGE_2D_RECT_EXT"),	# 0x904F
    ("glGet",	X,	1,	"GL_IMAGE_CUBE_EXT"),	# 0x9050
    ("glGet",	X,	1,	"GL_IMAGE_BUFFER_EXT"),	# 0x9051
    ("glGet",	X,	1,	"GL_IMAGE_1D_ARRAY_EXT"),	# 0x9052
    ("glGet",	X,	1,	"GL_IMAGE_2D_ARRAY_EXT"),	# 0x9053
    ("glGet",	X,	1,	"GL_IMAGE_CUBE_MAP_ARRAY_EXT"),	# 0x9054
    ("glGet",	X,	1,	"GL_IMAGE_2D_MULTISAMPLE_EXT"),	# 0x9055
    ("glGet",	X,	1,	"GL_IMAGE_2D_MULTISAMPLE_ARRAY_EXT"),	# 0x9056
    ("glGet",	X,	1,	"GL_INT_IMAGE_1D_EXT"),	# 0x9057
    ("glGet",	X,	1,	"GL_INT_IMAGE_2D_EXT"),	# 0x9058
    ("glGet",	X,	1,	"GL_INT_IMAGE_3D_EXT"),	# 0x9059
    ("glGet",	X,	1,	"GL_INT_IMAGE_2D_RECT_EXT"),	# 0x905A
    ("glGet",	X,	1,	"GL_INT_IMAGE_CUBE_EXT"),	# 0x905B
    ("glGet",	X,	1,	"GL_INT_IMAGE_BUFFER_EXT"),	# 0x905C
    ("glGet",	X,	1,	"GL_INT_IMAGE_1D_ARRAY_EXT"),	# 0x905D
    ("glGet",	X,	1,	"GL_INT_IMAGE_2D_ARRAY_EXT"),	# 0x905E
    ("glGet",	X,	1,	"GL_INT_IMAGE_CUBE_MAP_ARRAY_EXT"),	# 0x905F
    ("glGet",	X,	1,	"GL_INT_IMAGE_2D_MULTISAMPLE_EXT"),	# 0x9060
    ("glGet",	X,	1,	"GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY_EXT"),	# 0x9061
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_1D_EXT"),	# 0x9062
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_EXT"),	# 0x9063
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_3D_EXT"),	# 0x9064
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_RECT_EXT"),	# 0x9065
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_CUBE_EXT"),	# 0x9066
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_BUFFER_EXT"),	# 0x9067
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_1D_ARRAY_EXT"),	# 0x9068
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_ARRAY_EXT"),	# 0x9069
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_EXT"),	# 0x906A
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_EXT"),	# 0x906B
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY_EXT"),	# 0x906C
    ("glGet",	X,	1,	"GL_MAX_IMAGE_SAMPLES_EXT"),	# 0x906D
    ("glGet",	X,	1,	"GL_IMAGE_BINDING_FORMAT_EXT"),	# 0x906E
    ("glGet",	X,	1,	"GL_RGB10_A2UI"),	# 0x906F
    ("glGet",	X,	1,	"GL_TEXTURE_2D_MULTISAMPLE"),	# 0x9100
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_2D_MULTISAMPLE"),	# 0x9101
    ("glGet",	X,	1,	"GL_TEXTURE_2D_MULTISAMPLE_ARRAY"),	# 0x9102
    ("glGet",	X,	1,	"GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY"),	# 0x9103
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_2D_MULTISAMPLE"),	# 0x9104
    ("glGet",	X,	1,	"GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY"),	# 0x9105
    ("glGet",	X,	1,	"GL_TEXTURE_SAMPLES"),	# 0x9106
    ("glGet",	X,	1,	"GL_TEXTURE_FIXED_SAMPLE_LOCATIONS"),	# 0x9107
    ("glGet",	X,	1,	"GL_SAMPLER_2D_MULTISAMPLE"),	# 0x9108
    ("glGet",	X,	1,	"GL_INT_SAMPLER_2D_MULTISAMPLE"),	# 0x9109
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE"),	# 0x910A
    ("glGet",	X,	1,	"GL_SAMPLER_2D_MULTISAMPLE_ARRAY"),	# 0x910B
    ("glGet",	X,	1,	"GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY"),	# 0x910C
    ("glGet",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY"),	# 0x910D
    ("glGet",	X,	1,	"GL_MAX_COLOR_TEXTURE_SAMPLES"),	# 0x910E
    ("glGet",	X,	1,	"GL_MAX_DEPTH_TEXTURE_SAMPLES"),	# 0x910F
    ("glGet",	X,	1,	"GL_MAX_INTEGER_SAMPLES"),	# 0x9110
    ("glGet",	X,	1,	"GL_MAX_SERVER_WAIT_TIMEOUT"),	# 0x9111
    ("glGet",	X,	1,	"GL_OBJECT_TYPE"),	# 0x9112
    ("glGet",	X,	1,	"GL_SYNC_CONDITION"),	# 0x9113
    ("glGet",	X,	1,	"GL_SYNC_STATUS"),	# 0x9114
    ("glGet",	X,	1,	"GL_SYNC_FLAGS"),	# 0x9115
    ("glGet",	X,	1,	"GL_SYNC_FENCE"),	# 0x9116
    ("glGet",	X,	1,	"GL_SYNC_GPU_COMMANDS_COMPLETE"),	# 0x9117
    ("glGet",	X,	1,	"GL_UNSIGNALED"),	# 0x9118
    ("glGet",	X,	1,	"GL_SIGNALED"),	# 0x9119
    ("glGet",	X,	1,	"GL_ALREADY_SIGNALED"),	# 0x911A
    ("glGet",	X,	1,	"GL_TIMEOUT_EXPIRED"),	# 0x911B
    ("glGet",	X,	1,	"GL_CONDITION_SATISFIED"),	# 0x911C
    ("glGet",	X,	1,	"GL_WAIT_FAILED"),	# 0x911D
    ("glGet",	X,	1,	"GL_BUFFER_ACCESS_FLAGS"),	# 0x911F
    ("glGet",	X,	1,	"GL_BUFFER_MAP_LENGTH"),	# 0x9120
    ("glGet",	X,	1,	"GL_BUFFER_MAP_OFFSET"),	# 0x9121
    ("glGet",	X,	1,	"GL_MAX_VERTEX_OUTPUT_COMPONENTS"),	# 0x9122
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_INPUT_COMPONENTS"),	# 0x9123
    ("glGet",	X,	1,	"GL_MAX_GEOMETRY_OUTPUT_COMPONENTS"),	# 0x9124
    ("glGet",	X,	1,	"GL_MAX_FRAGMENT_INPUT_COMPONENTS"),	# 0x9125
    ("glGet",	X,	1,	"GL_CONTEXT_PROFILE_MASK"),	# 0x9126
    ("glGet",	X,	1,	"GL_MAX_DEBUG_MESSAGE_LENGTH_ARB"),	# 0x9143
    ("glGet",	X,	1,	"GL_MAX_DEBUG_LOGGED_MESSAGES_ARB"),	# 0x9144
    ("glGet",	X,	1,	"GL_DEBUG_LOGGED_MESSAGES_ARB"),	# 0x9145
    ("glGet",	X,	1,	"GL_DEBUG_SEVERITY_HIGH_ARB"),	# 0x9146
    ("glGet",	X,	1,	"GL_DEBUG_SEVERITY_MEDIUM_ARB"),	# 0x9147
    ("glGet",	X,	1,	"GL_DEBUG_SEVERITY_LOW_ARB"),	# 0x9148
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_API_ERROR_AMD"),	# 0x9149
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD"),	# 0x914A
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_DEPRECATION_AMD"),	# 0x914B
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD"),	# 0x914C
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_PERFORMANCE_AMD"),	# 0x914D
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD"),	# 0x914E
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_APPLICATION_AMD"),	# 0x914F
    ("glGet",	X,	1,	"GL_DEBUG_CATEGORY_OTHER_AMD"),	# 0x9150
    ("glGet",	X,	1,	"GL_DATA_BUFFER_AMD"),	# 0x9151
    ("glGet",	X,	1,	"GL_PERFORMANCE_MONITOR_AMD"),	# 0x9152
    ("glGet",	X,	1,	"GL_QUERY_OBJECT_AMD"),	# 0x9153
    ("glGet",	X,	1,	"GL_VERTEX_ARRAY_OBJECT_AMD"),	# 0x9154
    ("glGet",	X,	1,	"GL_SAMPLER_OBJECT_AMD"),	# 0x9155
    (None,	X,	1,	"GL_INVALID_INDEX"),	# 0xFFFFFFFFu
]

texture_targets = [
    ('GL_TEXTURE_1D', 'GL_TEXTURE_BINDING_1D'),
    ('GL_TEXTURE_2D', 'GL_TEXTURE_BINDING_2D'),
    ('GL_TEXTURE_3D', 'GL_TEXTURE_BINDING_3D'),
    #(GL_TEXTURE_CUBE_MAP, 'GL_TEXTURE_BINDING_CUBE_MAP')
]


class GetInflector:
    '''Objects that describes how to inflect.'''

    reduced_types = {
        B: I,
        E: I,
        I: F,
    }

    def __init__(self, radical, suffixes):
        self.radical = radical
        self.suffixes = suffixes

    def reduced_type(self, type):
        if type in self.suffixes:
            return type
        if type in self.reduced_types:
            return self.reduced_type(self.reduced_types[type])
        raise NotImplementedError

    def inflect(self, type):
        return self.radical + self.suffix(type)

    def suffix(self, type):
        type = self.reduced_type(type)
        assert type in self.suffixes
        return self.suffixes[type]


class StateGetter(Visitor):
    '''Type visitor that is able to extract the state via one of the glGet*
    functions.

    It will declare any temporary variable
    '''

    def __init__(self, radical, suffixes):
        self.inflector = GetInflector(radical, suffixes)

    def __call__(self, *args):
        pname = args[-1]

        for function, type, count, name in parameters:
            if type is X:
                continue
            if name == pname:
                if count != 1:
                    type = Array(type, str(count))

                return type, self.visit(type, args)

        raise NotImplementedError

    def temp_name(self, args):
        '''Return the name of a temporary variable to hold the state.'''
        pname = args[-1]

        return pname[3:].lower()

    def visit_const(self, const, args):
        return self.visit(const.type, args)

    def visit_scalar(self, type, args):
        temp_name = self.temp_name(args)
        elem_type = self.inflector.reduced_type(type)
        inflection = self.inflector.inflect(type)
        if inflection.endswith('v'):
            print '    %s %s = 0;' % (elem_type, temp_name)
            print '    %s(%s, &%s);' % (inflection, ', '.join(args), temp_name)
        else:
            print '    %s %s = %s(%s);' % (elem_type, temp_name, inflection, ', '.join(args))
        return temp_name

    def visit_string(self, string, args):
        temp_name = self.temp_name(args)
        inflection = self.inflector.inflect(string)
        assert not inflection.endswith('v')
        print '    %s %s = (%s)%s(%s);' % (string, temp_name, string, inflection, ', '.join(args))
        return temp_name

    def visit_alias(self, alias, args):
        return self.visit_scalar(alias, args)

    def visit_enum(self, enum, args):
        return self.visit(GLint, args)

    def visit_bitmask(self, bitmask, args):
        return self.visit(GLint, args)

    def visit_array(self, array, args):
        temp_name = self.temp_name(args)
        if array.length == '1':
            return self.visit(array.type)
        elem_type = self.inflector.reduced_type(array.type)
        inflection = self.inflector.inflect(array.type)
        assert inflection.endswith('v')
        print '    %s %s[%s];' % (elem_type, temp_name, array.length)
        print '    memset(%s, 0, %s * sizeof *%s);' % (temp_name, array.length, temp_name)
        print '    %s(%s, %s);' % (inflection, ', '.join(args), temp_name)
        return temp_name

    def visit_opaque(self, pointer, args):
        temp_name = self.temp_name(args)
        inflection = self.inflector.inflect(pointer)
        assert inflection.endswith('v')
        print '    GLvoid *%s;' % temp_name
        print '    %s(%s, &%s);' % (inflection, ', '.join(args), temp_name)
        return temp_name


glGet = StateGetter('glGet', {
    B: 'Booleanv',
    I: 'Integerv',
    F: 'Floatv',
    D: 'Doublev',
    S: 'String',
    P: 'Pointerv',
})

glGetVertexAttrib = StateGetter('glGetVertexAttrib', {I: 'iv', F: 'fv', D: 'dv', P: 'Pointerv'})
glGetTexParameter = StateGetter('glGetTexParameter', {I: 'iv', F: 'fv'})
glGetTexLevelParameter = StateGetter('glGetTexLevelParameter', {I: 'iv', F: 'fv'})


class JsonWriter(Visitor):
    '''Type visitor that will dump a value of the specified type through the
    JSON writer.
    
    It expects a previously declared JSONWriter instance named "json".'''

    def visit_literal(self, literal, instance):
        if literal.format == 'Bool':
            print '    json.writeBool(%s);' % instance
        elif literal.format in ('SInt', 'Uint', 'Float', 'Double'):
            print '    json.writeNumber(%s);' % instance
        else:
            raise NotImplementedError

    def visit_string(self, string, instance):
        assert string.length is None
        print '    json.writeString((const char *)%s);' % instance

    def visit_enum(self, enum, instance):
        if enum.expr == 'GLenum':
            print '    writeEnum(json, %s);' % instance
        else:
            print '    json.writeNumber(%s);' % instance

    def visit_bitmask(self, bitmask, instance):
        raise NotImplementedError

    def visit_alias(self, alias, instance):
        self.visit(alias.type, instance)

    def visit_opaque(self, opaque, instance):
        print '    json.writeNumber((size_t)%s);' % instance

    __index = 0

    def visit_array(self, array, instance):
        index = '__i%u' % JsonWriter.__index
        JsonWriter.__index += 1
        print '    json.beginArray();'
        print '    for (unsigned %s = 0; %s < %s; ++%s) {' % (index, index, array.length, index)
        self.visit(array.type, '%s[%s]' % (instance, index))
        print '    }'
        print '    json.endArray();'



class StateDumper:
    '''Class to generate code to dump all GL state in JSON format via
    stdout.'''

    def __init__(self):
        self.level = 0

    def dump(self):
        print '#include <string.h>'
        print '#include <iostream>'
        print
        print '#include "json.hpp"'
        print '#include "glimports.hpp"'
        print '#include "glproc.hpp"'
        print '#include "glsize.hpp"'
        print '#include "glretrace.hpp"'
        print

        print 'static const char *'
        print '_enum_string(GLenum pname)'
        print '{'
        print '    switch(pname) {'
        for name in GLenum.values:
            print '    case %s:' % name
            print '        return "%s";' % name
        print '    default:'
        print '        return NULL;'
        print '    }'
        print '}'
        print

        print 'static const char *'
        print 'enum_string(GLenum pname)'
        print '{'
        print '    const char *s = _enum_string(pname);'
        print '    if (s) {'
        print '        return s;'
        print '    } else {'
        print '        static char buf[16];'
        print '        snprintf(buf, sizeof buf, "0x%04x", pname);'
        print '        return buf;'
        print '    }'
        print '}'
        print

        print 'static inline void'
        print 'writeEnum(JSONWriter &json, GLenum pname)'
        print '{'
        print '    const char *s = _enum_string(pname);'
        print '    if (s) {'
        print '        json.writeString(s);'
        print '    } else {'
        print '        json.writeNumber(pname);'
        print '    }'
        print '}'
        print

        # shaders
        print '''
static void
writeShader(JSONWriter &json, GLuint shader)
{
    if (!shader) {
        return;
    }

    GLint shader_type = 0;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);
    if (!shader_type) {
        return;
    }

    GLint source_length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);
    if (!source_length) {
        return;
    }

    GLchar *source = new GLchar[source_length];
    GLsizei length = 0;
    source[0] = 0;
    glGetShaderSource(shader, source_length, &length, source);

    json.beginMember(enum_string(shader_type));
    json.writeString(source);
    json.endMember();

    delete [] source;
}

static inline void
writeCurrentProgram(JSONWriter &json)
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    if (!program) {
        return;
    }

    GLint attached_shaders = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    GLuint *shaders = new GLuint[attached_shaders];
    GLsizei count = 0;
    glGetAttachedShaders(program, attached_shaders, &count, shaders);
    for (GLsizei i = 0; i < count; ++ i) {
       writeShader(json, shaders[i]);
    }
    delete [] shaders;
}

static inline void
writeArbProgram(JSONWriter &json, GLenum target)
{
    if (!glIsEnabled(target)) {
        return;
    }

    GLint program_length = 0;
    glGetProgramivARB(target, GL_PROGRAM_LENGTH_ARB, &program_length);
    if (!program_length) {
        return;
    }

    GLchar *source = new GLchar[program_length + 1];
    source[0] = 0;
    glGetProgramStringARB(target, GL_PROGRAM_STRING_ARB, source);
    source[program_length] = 0;

    json.beginMember(enum_string(target));
    json.writeString(source);
    json.endMember();

    delete [] source;
}
'''

        # texture image
        print '''
static inline void
writeTextureImage(JSONWriter &json, GLenum target, GLint level)
{
    GLint width, height = 1, depth = 1;

    width = 0;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);

    if (target != GL_TEXTURE_1D) {
        height = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);
        if (target == GL_TEXTURE_3D) {
            depth = 0;
            glGetTexLevelParameteriv(target, level, GL_TEXTURE_DEPTH, &depth);
        }
    }

    if (width <= 0 || height <= 0 || depth <= 0) {
        json.writeNull();
    } else {
        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", depth);

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", 4);
        
        GLubyte *pixels = new GLubyte[depth*width*height*4];
        
        glGetTexImage(target, level, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        json.beginMember("__data__");
        json.writeBase64(pixels, depth * width * height * 4 * sizeof *pixels);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
}

static inline void
writeDrawBufferImage(JSONWriter &json, GLenum format)
{
    GLint width  = glretrace::window_width;
    GLint height = glretrace::window_height;

    GLint channels = __gl_format_channels(format);

    if (!width || !height) {
        json.writeNull();
    } else {
        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", 1);

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", channels);

        GLubyte *pixels = new GLubyte[width*height*channels];
        
        GLint drawbuffer = glretrace::double_buffer ? GL_BACK : GL_FRONT;
        GLint readbuffer = glretrace::double_buffer ? GL_BACK : GL_FRONT;
        glGetIntegerv(GL_DRAW_BUFFER, &drawbuffer);
        glGetIntegerv(GL_READ_BUFFER, &readbuffer);
        glReadBuffer(drawbuffer);

        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);

        glPopClientAttrib();
        glReadBuffer(readbuffer);

        json.beginMember("__data__");
        json.writeBase64(pixels, width * height * channels * sizeof *pixels);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
}

'''

        # textures
        print 'static inline void'
        print 'writeTexture(JSONWriter &json, GLenum target, GLenum binding)'
        print '{'
        print '    GLint texture = 0;'
        print '    glGetIntegerv(binding, &texture);'
        print '    if (!glIsEnabled(target) && !texture) {'
        print '        json.writeNull();'
        print '        return;'
        print '    }'
        print
        print '    json.beginObject();'
        print
        print '    GLfloat param;'
        for function, type, count, name in parameters:
            if function != 'glGetTexParameter' or count != 1:
                continue
            print '        glGetTexParameterfv(target, %s, &param);' % name
            print '        json.beginMember("%s");'  % name
            JsonWriter().visit(type, 'param')
            print '        json.endMember();'
            print
        print
        print '    json.beginMember("levels");'
        print '    json.beginArray();'
        print '    GLint level = 0;'
        print '    do {'
        print '        GLint width = 0, height = 0;'
        print '        glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);'
        print '        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);'
        print '        if (!width || !height) {'
        print '            break;'
        print '        }'
        print '        json.beginObject();'
        print
        # FIXME: This is not the original texture name in the trace -- we need
        # to do a reverse lookup of the texture mappings to find the original one
        print '        json.beginMember("binding");'
        print '        json.writeNumber(texture);'
        print '        json.endMember();'
        print
        # TODO: Generalize this
        for function, type, count, name in parameters:
            if function != 'glGetTexLevelParameter' or count != 1:
                continue
            print '        glGetTexLevelParameterfv(target, level, %s, &param);' % name
            print '        json.beginMember("%s");'  % name
            JsonWriter().visit(type, 'param')
            print '        json.endMember();'
            print
        print '        json.beginMember("image");'
        print '        writeTextureImage(json, target, level);'
        print '        json.endMember();'
        print
        print '        json.endObject();'
        print '        ++level;'
        print '    } while(true);'
        print '    json.endArray();'
        print '    json.endMember(); // levels'
        print
        print '    json.endObject();'
        print '}'
        print

        print 'void glretrace::state_dump(std::ostream &os)'
        print '{'
        print '    JSONWriter json(os);'
        self.dump_parameters()
        self.dump_current_program()
        self.dump_textures()
        self.dump_framebuffer()
        print '}'
        print

    def dump_parameters(self):
        print '    json.beginMember("parameters");'
        print '    json.beginObject();'
        
        self.dump_atoms(glGet)
        
        self.dump_vertex_attribs()

        print '    json.endObject();'
        print '    json.endMember(); // parameters'
        print

    def dump_vertex_attribs(self):
        print '    GLint max_vertex_attribs = 0;'
        print '    __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);'
        print '    for (GLint index = 0; index < max_vertex_attribs; ++index) {'
        print '        char name[32];'
        print '        snprintf(name, sizeof name, "GL_VERTEX_ATTRIB_ARRAY%i", index);'
        print '        json.beginMember(name);'
        print '        json.beginObject();'
        self.dump_atoms(glGetVertexAttrib, 'index')
        print '        json.endObject();'
        print '        json.endMember(); // GL_VERTEX_ATTRIB_ARRAYi'
        print '    }'
        print
        print

    def dump_current_program(self):
        print '    json.beginMember("shaders");'
        print '    json.beginObject();'
        print '    writeCurrentProgram(json);'
        print '    writeArbProgram(json, GL_FRAGMENT_PROGRAM_ARB);'
        print '    writeArbProgram(json, GL_VERTEX_PROGRAM_ARB);'
        print '    json.endObject();'
        print '    json.endMember(); //shaders'
        print

    def dump_textures(self):
        print '    json.beginMember("textures");'
        print '    json.beginArray();'
        print '    GLint active_texture = GL_TEXTURE0;'
        print '    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);'
        print '    GLint max_texture_coords = 0;'
        print '    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);'
        print '    for (GLint unit = 0; unit < max_texture_coords; ++unit) {'
        print '        glActiveTexture(GL_TEXTURE0 + unit);'
        print '        json.beginObject();'
        for target, binding in texture_targets:
            print '        json.beginMember("%s");' % target
            print '        writeTexture(json, %s, %s);' % (target, binding)
            print '        json.endMember();'
        print '        json.endObject();'
        print '    }'
        print '    glActiveTexture(active_texture);'
        print '    json.endArray();'
        print '    json.endMember(); // texture'
        print

    def dump_framebuffer(self):
        print '    json.beginMember("framebuffer");'
        print '    json.beginObject();'
        # TODO: Handle real FBOs
        print
        print '    json.beginMember("GL_RGBA");'
        print '    writeDrawBufferImage(json, GL_RGBA);'
        print '    json.endMember();'
        print
        print '    GLint depth_bits = 0;'
        print '    glGetIntegerv(GL_DEPTH_BITS, &depth_bits);'
        print '    if (depth_bits) {'
        print '        json.beginMember("GL_DEPTH_COMPONENT");'
        print '        writeDrawBufferImage(json, GL_DEPTH_COMPONENT);'
        print '        json.endMember();'
        print '    }'
        print
        print '    GLint stencil_bits = 0;'
        print '    glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);'
        print '    if (stencil_bits) {'
        print '        json.beginMember("GL_STENCIL_INDEX");'
        print '        writeDrawBufferImage(json, GL_STENCIL_INDEX);'
        print '        json.endMember();'
        print '    }'
        print
        print '    json.endObject();'
        print '    json.endMember(); // framebuffer'
        pass

    def dump_atoms(self, getter, *args):
        for function, type, count, name in parameters:
            if function != getter.inflector.radical:
                continue
            if type is X:
                continue
            print '        // %s' % name
            print '        {'
            type, value = getter(*(args + (name,)))
            print '            if (glGetError() != GL_NO_ERROR) {'
            #print '                std::cerr << "warning: %s(%s) failed\\n";' % (glGet.radical, name)
            print '            } else {'
            print '                json.beginMember("%s");' % name
            JsonWriter().visit(type, value)
            print '                json.endMember();'
            print '            }'
            print '        }'
            print

    def write_line(s):
        self.write('  '*self.level + s + '\n')

    def write(self, s):
        sys.stdout.write(s)

    def indent(self):
        self.level += 1

    def dedent(self):
        self.level -= 1



if __name__ == '__main__':
    StateDumper().dump()

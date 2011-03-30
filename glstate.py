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

from glenum import GLenum



I = Int
F = Float

parameters = [
    (F,	1,	"GL_POINT_SIZE"),	# 0x0B11
    (F,	2,	"GL_POINT_SIZE_RANGE"),	# 0x0B12
    (F,	1,	"GL_POINT_SIZE_GRANULARITY"),	# 0x0B13
    (I,	1,	"GL_LINE_SMOOTH"),	# 0x0B20
    (F,	1,	"GL_LINE_WIDTH"),	# 0x0B21
    (F,	2,	"GL_LINE_WIDTH_RANGE"),	# 0x0B22
    (F,	1,	"GL_LINE_WIDTH_GRANULARITY"),	# 0x0B23
    (I,	1,	"GL_POLYGON_SMOOTH"),	# 0x0B41
    (I,	1,	"GL_CULL_FACE"),	# 0x0B44
    (I,	1,	"GL_CULL_FACE_MODE"),	# 0x0B45
    (I,	1,	"GL_FRONT_FACE"),	# 0x0B46
    (F,	2,	"GL_DEPTH_RANGE"),	# 0x0B70
    (I,	1,	"GL_DEPTH_TEST"),	# 0x0B71
    (I,	1,	"GL_DEPTH_WRITEMASK"),	# 0x0B72
    (F,	1,	"GL_DEPTH_CLEAR_VALUE"),	# 0x0B73
    (I,	1,	"GL_DEPTH_FUNC"),	# 0x0B74
    (I,	1,	"GL_STENCIL_TEST"),	# 0x0B90
    (I,	1,	"GL_STENCIL_CLEAR_VALUE"),	# 0x0B91
    (I,	1,	"GL_STENCIL_FUNC"),	# 0x0B92
    (I,	1,	"GL_STENCIL_VALUE_MASK"),	# 0x0B93
    (I,	1,	"GL_STENCIL_FAIL"),	# 0x0B94
    (I,	1,	"GL_STENCIL_PASS_DEPTH_FAIL"),	# 0x0B95
    (I,	1,	"GL_STENCIL_PASS_DEPTH_PASS"),	# 0x0B96
    (I,	1,	"GL_STENCIL_REF"),	# 0x0B97
    (I,	1,	"GL_STENCIL_WRITEMASK"),	# 0x0B98
    (I,	4,	"GL_VIEWPORT"),	# 0x0BA2
    (I,	1,	"GL_DITHER"),	# 0x0BD0
    (I,	1,	"GL_BLEND_DST"),	# 0x0BE0
    (I,	1,	"GL_BLEND_SRC"),	# 0x0BE1
    (I,	1,	"GL_BLEND"),	# 0x0BE2
    (I,	1,	"GL_LOGIC_OP_MODE"),	# 0x0BF0
    (I,	1,	"GL_COLOR_LOGIC_OP"),	# 0x0BF2
    (I,	1,	"GL_DRAW_BUFFER"),	# 0x0C01
    (I,	1,	"GL_READ_BUFFER"),	# 0x0C02
    (I,	4,	"GL_SCISSOR_BOX"),	# 0x0C10
    (I,	1,	"GL_SCISSOR_TEST"),	# 0x0C11
    (F,	4,	"GL_COLOR_CLEAR_VALUE"),	# 0x0C22
    (I,	4,	"GL_COLOR_WRITEMASK"),	# 0x0C23
    (I,	1,	"GL_DOUBLEBUFFER"),	# 0x0C32
    (I,	1,	"GL_STEREO"),	# 0x0C33
    (I,	1,	"GL_LINE_SMOOTH_HINT"),	# 0x0C52
    (I,	1,	"GL_POLYGON_SMOOTH_HINT"),	# 0x0C53
    (I,	1,	"GL_UNPACK_SWAP_BYTES"),	# 0x0CF0
    (I,	1,	"GL_UNPACK_LSB_FIRST"),	# 0x0CF1
    (I,	1,	"GL_UNPACK_ROW_LENGTH"),	# 0x0CF2
    (I,	1,	"GL_UNPACK_SKIP_ROWS"),	# 0x0CF3
    (I,	1,	"GL_UNPACK_SKIP_PIXELS"),	# 0x0CF4
    (I,	1,	"GL_UNPACK_ALIGNMENT"),	# 0x0CF5
    (I,	1,	"GL_PACK_SWAP_BYTES"),	# 0x0D00
    (I,	1,	"GL_PACK_LSB_FIRST"),	# 0x0D01
    (I,	1,	"GL_PACK_ROW_LENGTH"),	# 0x0D02
    (I,	1,	"GL_PACK_SKIP_ROWS"),	# 0x0D03
    (I,	1,	"GL_PACK_SKIP_PIXELS"),	# 0x0D04
    (I,	1,	"GL_PACK_ALIGNMENT"),	# 0x0D05
    (I,	1,	"GL_MAX_TEXTURE_SIZE"),	# 0x0D33
    (F,	2,	"GL_MAX_VIEWPORT_DIMS"),	# 0x0D3A
    (I,	1,	"GL_SUBPIXEL_BITS"),	# 0x0D50
    (I,	1,	"GL_TEXTURE_1D"),	# 0x0DE0
    (I,	1,	"GL_TEXTURE_2D"),	# 0x0DE1
    (F,	1,	"GL_POLYGON_OFFSET_UNITS"),	# 0x2A00
    (I,	1,	"GL_POLYGON_OFFSET_POINT"),	# 0x2A01
    (I,	1,	"GL_POLYGON_OFFSET_LINE"),	# 0x2A02
    (I,	1,	"GL_POLYGON_OFFSET_FILL"),	# 0x8037
    (F,	1,	"GL_POLYGON_OFFSET_FACTOR"),	# 0x8038
    (I,	1,	"GL_TEXTURE_BINDING_1D"),	# 0x8068
    (I,	1,	"GL_TEXTURE_BINDING_2D"),	# 0x8069
    (F,	4,	"GL_CURRENT_COLOR"),	# 0x0B00
    (F,	1,	"GL_CURRENT_INDEX"),	# 0x0B01
    (F,	3,	"GL_CURRENT_NORMAL"),	# 0x0B02
    (F,	4,	"GL_CURRENT_TEXTURE_COORDS"),	# 0x0B03
    (F,	4,	"GL_CURRENT_RASTER_COLOR"),	# 0x0B04
    (F,	1,	"GL_CURRENT_RASTER_INDEX"),	# 0x0B05
    (F,	4,	"GL_CURRENT_RASTER_TEXTURE_COORDS"),	# 0x0B06
    (F,	4,	"GL_CURRENT_RASTER_POSITION"),	# 0x0B07
    (I,	1,	"GL_CURRENT_RASTER_POSITION_VALID"),	# 0x0B08
    (F,	1,	"GL_CURRENT_RASTER_DISTANCE"),	# 0x0B09
    (I,	1,	"GL_POINT_SMOOTH"),	# 0x0B10
    (I,	1,	"GL_LINE_STIPPLE"),	# 0x0B24
    (I,	1,	"GL_LINE_STIPPLE_PATTERN"),	# 0x0B25
    (I,	1,	"GL_LINE_STIPPLE_REPEAT"),	# 0x0B26
    (I,	1,	"GL_LIST_MODE"),	# 0x0B30
    (I,	1,	"GL_MAX_LIST_NESTING"),	# 0x0B31
    (I,	1,	"GL_LIST_BASE"),	# 0x0B32
    (I,	1,	"GL_LIST_INDEX"),	# 0x0B33
    (I,	2,	"GL_POLYGON_MODE"),	# 0x0B40
    (I,	1,	"GL_POLYGON_STIPPLE"),	# 0x0B42
    (I,	1,	"GL_EDGE_FLAG"),	# 0x0B43
    (I,	1,	"GL_LIGHTING"),	# 0x0B50
    (I,	1,	"GL_LIGHT_MODEL_LOCAL_VIEWER"),	# 0x0B51
    (I,	1,	"GL_LIGHT_MODEL_TWO_SIDE"),	# 0x0B52
    (F,	4,	"GL_LIGHT_MODEL_AMBIENT"),	# 0x0B53
    (I,	1,	"GL_SHADE_MODEL"),	# 0x0B54
    (I,	1,	"GL_COLOR_MATERIAL_FACE"),	# 0x0B55
    (I,	1,	"GL_COLOR_MATERIAL_PARAMETER"),	# 0x0B56
    (I,	1,	"GL_COLOR_MATERIAL"),	# 0x0B57
    (I,	1,	"GL_FOG"),	# 0x0B60
    (I,	1,	"GL_FOG_INDEX"),	# 0x0B61
    (F,	1,	"GL_FOG_DENSITY"),	# 0x0B62
    (F,	1,	"GL_FOG_START"),	# 0x0B63
    (F,	1,	"GL_FOG_END"),	# 0x0B64
    (I,	1,	"GL_FOG_MODE"),	# 0x0B65
    (F,	4,	"GL_FOG_COLOR"),	# 0x0B66
    (F,	4,	"GL_ACCUM_CLEAR_VALUE"),	# 0x0B80
    (I,	1,	"GL_MATRIX_MODE"),	# 0x0BA0
    (I,	1,	"GL_NORMALIZE"),	# 0x0BA1
    (I,	1,	"GL_MODELVIEW_STACK_DEPTH"),	# 0x0BA3
    (I,	1,	"GL_PROJECTION_STACK_DEPTH"),	# 0x0BA4
    (I,	1,	"GL_TEXTURE_STACK_DEPTH"),	# 0x0BA5
    (F,	16,	"GL_MODELVIEW_MATRIX"),	# 0x0BA6
    (F,	16,	"GL_PROJECTION_MATRIX"),	# 0x0BA7
    (F,	16,	"GL_TEXTURE_MATRIX"),	# 0x0BA8
    (I,	1,	"GL_ATTRIB_STACK_DEPTH"),	# 0x0BB0
    (I,	1,	"GL_CLIENT_ATTRIB_STACK_DEPTH"),	# 0x0BB1
    (I,	1,	"GL_ALPHA_TEST"),	# 0x0BC0
    (I,	1,	"GL_ALPHA_TEST_FUNC"),	# 0x0BC1
    (F,	1,	"GL_ALPHA_TEST_REF"),	# 0x0BC2
    (I,	1,	"GL_INDEX_LOGIC_OP"),	# 0x0BF1
    (I,	1,	"GL_LOGIC_OP"),	# 0x0BF1
    (I,	1,	"GL_AUX_BUFFERS"),	# 0x0C00
    (I,	1,	"GL_INDEX_CLEAR_VALUE"),	# 0x0C20
    (I,	1,	"GL_INDEX_WRITEMASK"),	# 0x0C21
    (I,	1,	"GL_INDEX_MODE"),	# 0x0C30
    (I,	1,	"GL_RGBA_MODE"),	# 0x0C31
    (I,	1,	"GL_RENDER_MODE"),	# 0x0C40
    (I,	1,	"GL_PERSPECTIVE_CORRECTION_HINT"),	# 0x0C50
    (I,	1,	"GL_POINT_SMOOTH_HINT"),	# 0x0C51
    (I,	1,	"GL_FOG_HINT"),	# 0x0C54
    (I,	1,	"GL_TEXTURE_GEN_S"),	# 0x0C60
    (I,	1,	"GL_TEXTURE_GEN_T"),	# 0x0C61
    (I,	1,	"GL_TEXTURE_GEN_R"),	# 0x0C62
    (I,	1,	"GL_TEXTURE_GEN_Q"),	# 0x0C63
    (I,	1,	"GL_PIXEL_MAP_I_TO_I_SIZE"),	# 0x0CB0
    (I,	1,	"GL_PIXEL_MAP_S_TO_S_SIZE"),	# 0x0CB1
    (I,	1,	"GL_PIXEL_MAP_I_TO_R_SIZE"),	# 0x0CB2
    (I,	1,	"GL_PIXEL_MAP_I_TO_G_SIZE"),	# 0x0CB3
    (I,	1,	"GL_PIXEL_MAP_I_TO_B_SIZE"),	# 0x0CB4
    (I,	1,	"GL_PIXEL_MAP_I_TO_A_SIZE"),	# 0x0CB5
    (I,	1,	"GL_PIXEL_MAP_R_TO_R_SIZE"),	# 0x0CB6
    (I,	1,	"GL_PIXEL_MAP_G_TO_G_SIZE"),	# 0x0CB7
    (I,	1,	"GL_PIXEL_MAP_B_TO_B_SIZE"),	# 0x0CB8
    (I,	1,	"GL_PIXEL_MAP_A_TO_A_SIZE"),	# 0x0CB9
    (I,	1,	"GL_MAP_COLOR"),	# 0x0D10
    (I,	1,	"GL_MAP_STENCIL"),	# 0x0D11
    (I,	1,	"GL_INDEX_SHIFT"),	# 0x0D12
    (I,	1,	"GL_INDEX_OFFSET"),	# 0x0D13
    (F,	1,	"GL_RED_SCALE"),	# 0x0D14
    (F,	1,	"GL_RED_BIAS"),	# 0x0D15
    (F,	1,	"GL_ZOOM_X"),	# 0x0D16
    (F,	1,	"GL_ZOOM_Y"),	# 0x0D17
    (F,	1,	"GL_GREEN_SCALE"),	# 0x0D18
    (F,	1,	"GL_GREEN_BIAS"),	# 0x0D19
    (F,	1,	"GL_BLUE_SCALE"),	# 0x0D1A
    (F,	1,	"GL_BLUE_BIAS"),	# 0x0D1B
    (F,	1,	"GL_ALPHA_SCALE"),	# 0x0D1C
    (F,	1,	"GL_ALPHA_BIAS"),	# 0x0D1D
    (F,	1,	"GL_DEPTH_SCALE"),	# 0x0D1E
    (F,	1,	"GL_DEPTH_BIAS"),	# 0x0D1F
    (I,	1,	"GL_MAX_EVAL_ORDER"),	# 0x0D30
    (I,	1,	"GL_MAX_LIGHTS"),	# 0x0D31
    (I,	1,	"GL_MAX_CLIP_PLANES"),	# 0x0D32
    (I,	1,	"GL_MAX_PIXEL_MAP_TABLE"),	# 0x0D34
    (I,	1,	"GL_MAX_ATTRIB_STACK_DEPTH"),	# 0x0D35
    (I,	1,	"GL_MAX_MODELVIEW_STACK_DEPTH"),	# 0x0D36
    (I,	1,	"GL_MAX_NAME_STACK_DEPTH"),	# 0x0D37
    (I,	1,	"GL_MAX_PROJECTION_STACK_DEPTH"),	# 0x0D38
    (I,	1,	"GL_MAX_TEXTURE_STACK_DEPTH"),	# 0x0D39
    (I,	1,	"GL_MAX_CLIENT_ATTRIB_STACK_DEPTH"),	# 0x0D3B
    (I,	1,	"GL_INDEX_BITS"),	# 0x0D51
    (I,	1,	"GL_RED_BITS"),	# 0x0D52
    (I,	1,	"GL_GREEN_BITS"),	# 0x0D53
    (I,	1,	"GL_BLUE_BITS"),	# 0x0D54
    (I,	1,	"GL_ALPHA_BITS"),	# 0x0D55
    (I,	1,	"GL_DEPTH_BITS"),	# 0x0D56
    (I,	1,	"GL_STENCIL_BITS"),	# 0x0D57
    (I,	1,	"GL_ACCUM_RED_BITS"),	# 0x0D58
    (I,	1,	"GL_ACCUM_GREEN_BITS"),	# 0x0D59
    (I,	1,	"GL_ACCUM_BLUE_BITS"),	# 0x0D5A
    (I,	1,	"GL_ACCUM_ALPHA_BITS"),	# 0x0D5B
    (I,	1,	"GL_NAME_STACK_DEPTH"),	# 0x0D70
    (I,	1,	"GL_AUTO_NORMAL"),	# 0x0D80
    (I,	1,	"GL_MAP1_COLOR_4"),	# 0x0D90
    (I,	1,	"GL_MAP1_INDEX"),	# 0x0D91
    (I,	1,	"GL_MAP1_NORMAL"),	# 0x0D92
    (I,	1,	"GL_MAP1_TEXTURE_COORD_1"),	# 0x0D93
    (I,	1,	"GL_MAP1_TEXTURE_COORD_2"),	# 0x0D94
    (I,	1,	"GL_MAP1_TEXTURE_COORD_3"),	# 0x0D95
    (I,	1,	"GL_MAP1_TEXTURE_COORD_4"),	# 0x0D96
    (I,	1,	"GL_MAP1_VERTEX_3"),	# 0x0D97
    (I,	1,	"GL_MAP1_VERTEX_4"),	# 0x0D98
    (I,	1,	"GL_MAP2_COLOR_4"),	# 0x0DB0
    (I,	1,	"GL_MAP2_INDEX"),	# 0x0DB1
    (I,	1,	"GL_MAP2_NORMAL"),	# 0x0DB2
    (I,	1,	"GL_MAP2_TEXTURE_COORD_1"),	# 0x0DB3
    (I,	1,	"GL_MAP2_TEXTURE_COORD_2"),	# 0x0DB4
    (I,	1,	"GL_MAP2_TEXTURE_COORD_3"),	# 0x0DB5
    (I,	1,	"GL_MAP2_TEXTURE_COORD_4"),	# 0x0DB6
    (I,	1,	"GL_MAP2_VERTEX_3"),	# 0x0DB7
    (I,	1,	"GL_MAP2_VERTEX_4"),	# 0x0DB8
    (F,	2,	"GL_MAP1_GRID_DOMAIN"),	# 0x0DD0
    (I,	1,	"GL_MAP1_GRID_SEGMENTS"),	# 0x0DD1
    (F,	4,	"GL_MAP2_GRID_DOMAIN"),	# 0x0DD2
    (I,	2,	"GL_MAP2_GRID_SEGMENTS"),	# 0x0DD3
    (I,	1,	"GL_FEEDBACK_BUFFER_SIZE"),	# 0x0DF1
    (I,	1,	"GL_FEEDBACK_BUFFER_TYPE"),	# 0x0DF2
    (I,	1,	"GL_SELECTION_BUFFER_SIZE"),	# 0x0DF4
    (I,	1,	"GL_VERTEX_ARRAY"),	# 0x8074
    (I,	1,	"GL_NORMAL_ARRAY"),	# 0x8075
    (I,	1,	"GL_COLOR_ARRAY"),	# 0x8076
    (I,	1,	"GL_INDEX_ARRAY"),	# 0x8077
    (I,	1,	"GL_TEXTURE_COORD_ARRAY"),	# 0x8078
    (I,	1,	"GL_EDGE_FLAG_ARRAY"),	# 0x8079
    (I,	1,	"GL_VERTEX_ARRAY_SIZE"),	# 0x807A
    (I,	1,	"GL_VERTEX_ARRAY_TYPE"),	# 0x807B
    (I,	1,	"GL_VERTEX_ARRAY_STRIDE"),	# 0x807C
    (I,	1,	"GL_NORMAL_ARRAY_TYPE"),	# 0x807E
    (I,	1,	"GL_NORMAL_ARRAY_STRIDE"),	# 0x807F
    (I,	1,	"GL_COLOR_ARRAY_SIZE"),	# 0x8081
    (I,	1,	"GL_COLOR_ARRAY_TYPE"),	# 0x8082
    (I,	1,	"GL_COLOR_ARRAY_STRIDE"),	# 0x8083
    (I,	1,	"GL_INDEX_ARRAY_TYPE"),	# 0x8085
    (I,	1,	"GL_INDEX_ARRAY_STRIDE"),	# 0x8086
    (I,	1,	"GL_TEXTURE_COORD_ARRAY_SIZE"),	# 0x8088
    (I,	1,	"GL_TEXTURE_COORD_ARRAY_TYPE"),	# 0x8089
    (I,	1,	"GL_TEXTURE_COORD_ARRAY_STRIDE"),	# 0x808A
    (I,	1,	"GL_EDGE_FLAG_ARRAY_STRIDE"),	# 0x808C
]


class StateDumper:

    def __init__(self):
        self.level = 0

    def dump(self):
        print '#include <string.h>'
        print
        print '#include "json.hpp"'
        print '#include "glimports.hpp"'
        print '#include "glstate.hpp"'
        print
        print 'void state_dump(std::ostream &os)'
        print '{'
        print '    JSONWriter json(os);'
        self.dump_parameters()

        print '}'
        print

    def dump_parameters(self):
        print '    GLint iparams[16];'
        print '    GLfloat fparams[16];'
        print '    json.beginMember("parameters");'
        print '    json.beginObject();'
        for type, count, name in parameters:
            print '    json.beginMember("%s");' % name
            if type is I:
                print '    memset(iparams, 0, %u * sizeof *iparams);' % count
                print '    glGetIntegerv(%s, iparams);' % name
                if count == 1:
                    print '    json.writeNumber(iparams[0]);'
                else:
                    print '    json.beginArray();'
                    for i in range(count):
                        print '    json.writeNumber(iparams[%u]);' % i
                    print '    json.endArray();'
            else:
                print '    memset(fparams, 0, %u * sizeof *fparams);' % count
                print '    glGetFloatv(%s, fparams);' % name
                if count == 1:
                    print '    json.writeNumber(fparams[0]);'
                else:
                    print '    json.beginArray();'
                    for i in range(count):
                        print '    json.writeNumber(fparams[%u]);' % i
                    print '    json.endArray();'
            print '    json.endMember();'
        print '    json.endObject();'
        print '    json.endMember(); // parameters'
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

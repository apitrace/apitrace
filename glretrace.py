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


"""GL retracer generator."""


import stdapi
import glapi
from retrace import Retracer


class GlRetracer(Retracer):

    def retrace_function(self, function):
        Retracer.retrace_function(self, function)

    array_pointer_function_names = set((
        "glVertexPointer",
        "glNormalPointer",
        "glColorPointer",
        "glIndexPointer",
        "glTexCoordPointer",
        "glEdgeFlagPointer",
        "glFogCoordPointer",
        "glSecondaryColorPointer",

        "glInterleavedArrays",

        "glVertexPointerEXT",
        "glNormalPointerEXT",
        "glColorPointerEXT",
        "glIndexPointerEXT",
        "glTexCoordPointerEXT",
        "glEdgeFlagPointerEXT",
        "glFogCoordPointerEXT",
        "glSecondaryColorPointerEXT",

        "glVertexAttribPointer",
        "glVertexAttribPointerARB",
        "glVertexAttribPointerNV",
        "glVertexAttribIPointer",
        "glVertexAttribIPointerEXT",
        "glVertexAttribLPointer",
        "glVertexAttribLPointerEXT",
        
        #"glMatrixIndexPointerARB",
    ))

    draw_array_function_names = set([
        "glDrawArrays",
        "glDrawArraysEXT",
        "glDrawArraysIndirect",
        "glDrawArraysInstanced",
        "glDrawArraysInstancedARB",
        "glDrawArraysInstancedEXT",
        "glDrawMeshArraysSUN",
        "glMultiDrawArrays",
        "glMultiDrawArraysEXT",
        "glMultiModeDrawArraysIBM",
    ])

    draw_elements_function_names = set([
        "glDrawElements",
        "glDrawElementsBaseVertex",
        "glDrawElementsIndirect",
        "glDrawElementsInstanced",
        "glDrawElementsInstancedARB",
        "glDrawElementsInstancedBaseVertex",
        "glDrawElementsInstancedEXT",
        "glDrawRangeElements",
        "glDrawRangeElementsBaseVertex",
        "glDrawRangeElementsEXT",
        "glMultiDrawElements",
        "glMultiDrawElementsBaseVertex",
        "glMultiDrawElementsEXT",
        "glMultiModeDrawElementsIBM",
    ])

    misc_draw_function_names = set([
        "glClear",
        "glEnd",
        "glDrawPixels",
        "glBlitFramebuffer",
        "glBlitFramebufferEXT",
    ])

    bind_framebuffer_function_names = set([
        "glBindFramebuffer",
        "glBindFramebufferARB",
        "glBindFramebufferEXT",
    ])

    # Names of the functions that can pack into the current pixel buffer
    # object.  See also the ARB_pixel_buffer_object specification.
    pack_function_names = set([
        'glGetCompressedTexImage',
        'glGetConvolutionFilter',
        'glGetHistogram',
        'glGetMinmax',
        'glGetPixelMapfv',
        'glGetPixelMapuiv',
        'glGetPixelMapusv',
        'glGetPolygonStipple',
        'glGetSeparableFilter,',
        'glGetTexImage',
        'glReadPixels',
        'glGetnCompressedTexImageARB',
        'glGetnConvolutionFilterARB',
        'glGetnHistogramARB',
        'glGetnMinmaxARB',
        'glGetnPixelMapfvARB',
        'glGetnPixelMapuivARB',
        'glGetnPixelMapusvARB',
        'glGetnPolygonStippleARB',
        'glGetnSeparableFilterARB',
        'glGetnTexImageARB',
        'glReadnPixelsARB',
    ])

    def retrace_function_body(self, function):
        is_array_pointer = function.name in self.array_pointer_function_names
        is_draw_array = function.name in self.draw_array_function_names
        is_draw_elements = function.name in self.draw_elements_function_names
        is_misc_draw = function.name in self.misc_draw_function_names

        if is_array_pointer or is_draw_array or is_draw_elements:
            print '    if (glretrace::parser.version < 1) {'

            if is_array_pointer or is_draw_array:
                print '        GLint __array_buffer = 0;'
                print '        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__array_buffer);'
                print '        if (!__array_buffer) {'
                self.fail_function(function)
                print '        }'

            if is_draw_elements:
                print '        GLint __element_array_buffer = 0;'
                print '        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__element_array_buffer);'
                print '        if (!__element_array_buffer) {'
                self.fail_function(function)
                print '        }'
            
            print '    }'

        # When no pack buffer object is bound, the pack functions are no-ops.
        if function.name in self.pack_function_names:
            print '    GLint __pack_buffer = 0;'
            print '    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &__pack_buffer);'
            print '    if (!__pack_buffer) {'
            print '        return;'
            print '    }'

        # Pre-snapshots
        if function.name in self.bind_framebuffer_function_names:
            print '    if (glretrace::snapshot_frequency == glretrace::FREQUENCY_FRAMEBUFFER) {'
            print '        glretrace::snapshot(call.no - 1);'
            print '    }'

        Retracer.retrace_function_body(self, function)

        # Post-snapshots
        if function.name in ('glFlush', 'glFinish'):
            print '    if (!glretrace::double_buffer) {'
            print '        glretrace::frame_complete(call.no);'
            print '    }'
        if function.name == 'glReadPixels':
            print '    glFinish();'
            print '    if (glretrace::snapshot_frequency == glretrace::FREQUENCY_FRAME ||'
            print '        glretrace::snapshot_frequency == glretrace::FREQUENCY_FRAMEBUFFER) {'
            print '        glretrace::snapshot(call.no);'
            print '    }'
        if is_draw_array or is_draw_elements or is_misc_draw:
            print '    if (glretrace::snapshot_frequency == glretrace::FREQUENCY_DRAW) {'
            print '        glretrace::snapshot(call.no);'
            print '    }'


    def call_function(self, function):
        if function.name == "glViewport":
            print '    GLint draw_framebuffer = 0;'
            print '    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_framebuffer);'
            print '    if (draw_framebuffer == 0) {'
            print '        if (glretrace::drawable) {'
            print '            int drawable_width  = x + width;'
            print '            int drawable_height = y + height;'
            print '            if (drawable_width  > (int)glretrace::drawable->width ||'
            print '                drawable_height > (int)glretrace::drawable->height) {'
            print '                glretrace::drawable->resize(drawable_width, drawable_height);'
            print '                if (!glretrace::drawable->visible) {'
            print '                    glretrace::drawable->show();'
            print '                }'
            print '                glScissor(0, 0, drawable_width, drawable_height);'
            print '            }'
            print '        }'
            print '    }'

        if function.name == "glEnd":
            print '    glretrace::insideGlBeginEnd = false;'

        if function.name == 'memcpy':
            print '    if (!dest || !src || !n) return;'
        
        Retracer.call_function(self, function)

        # Error checking
        if function.name == "glBegin":
            print '    glretrace::insideGlBeginEnd = true;'
        elif function.name.startswith('gl'):
            # glGetError is not allowed inside glBegin/glEnd
            print '    if (!glretrace::benchmark && !glretrace::insideGlBeginEnd) {'
            print '        glretrace::checkGlError(call);'
            if function.name in ('glProgramStringARB', 'glProgramStringNV'):
                print r'        GLint error_position = -1;'
                print r'        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_position);'
                print r'        if (error_position != -1) {'
                print r'            const char *error_string = (const char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);'
                print r'            std::cerr << call.no << ": warning: " << error_string << "\n";'
                print r'        }'
            if function.name == 'glCompileShader':
                print r'        GLint compile_status = 0;'
                print r'        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);'
                print r'        if (!compile_status) {'
                print r'             GLint info_log_length = 0;'
                print r'             glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);'
                print r'             GLchar *infoLog = new GLchar[info_log_length];'
                print r'             glGetShaderInfoLog(shader, info_log_length, NULL, infoLog);'
                print r'             std::cerr << call.no << ": warning: " << infoLog << "\n";'
                print r'             delete [] infoLog;'
                print r'        }'
            if function.name == 'glLinkProgram':
                print r'        GLint link_status = 0;'
                print r'        glGetProgramiv(program, GL_LINK_STATUS, &link_status);'
                print r'        if (!link_status) {'
                print r'             GLint info_log_length = 0;'
                print r'             glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);'
                print r'             GLchar *infoLog = new GLchar[info_log_length];'
                print r'             glGetProgramInfoLog(program, info_log_length, NULL, infoLog);'
                print r'             std::cerr << call.no << ": warning: " << infoLog << "\n";'
                print r'             delete [] infoLog;'
                print r'        }'
            if function.name == 'glCompileShaderARB':
                print r'        GLint compile_status = 0;'
                print r'        glGetObjectParameterivARB(shaderObj, GL_OBJECT_COMPILE_STATUS_ARB, &compile_status);'
                print r'        if (!compile_status) {'
                print r'             GLint info_log_length = 0;'
                print r'             glGetObjectParameterivARB(shaderObj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_log_length);'
                print r'             GLchar *infoLog = new GLchar[info_log_length];'
                print r'             glGetInfoLogARB(shaderObj, info_log_length, NULL, infoLog);'
                print r'             std::cerr << call.no << ": warning: " << infoLog << "\n";'
                print r'             delete [] infoLog;'
                print r'        }'
            if function.name == 'glLinkProgramARB':
                print r'        GLint link_status = 0;'
                print r'        glGetObjectParameterivARB(programObj, GL_OBJECT_LINK_STATUS_ARB, &link_status);'
                print r'        if (!link_status) {'
                print r'             GLint info_log_length = 0;'
                print r'             glGetObjectParameterivARB(programObj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_log_length);'
                print r'             GLchar *infoLog = new GLchar[info_log_length];'
                print r'             glGetInfoLogARB(programObj, info_log_length, NULL, infoLog);'
                print r'             std::cerr << call.no << ": warning: " << infoLog << "\n";'
                print r'             delete [] infoLog;'
                print r'        }'
            if function.name in ('glMapBuffer', 'glMapBufferARB', 'glMapBufferRange', 'glMapNamedBufferEXT', 'glMapNamedBufferRangeEXT'):
                print r'        if (!__result) {'
                print r'             std::cerr << call.no << ": warning: failed to map buffer\n";'
                print r'        }'
            if function.name in ('glGetAttribLocation', 'glGetAttribLocationARB'):
                print r'    GLint __orig_result = call.ret->toSInt();'
                print r'    if (__result != __orig_result) {'
                print r'        std::cerr << call.no << ": warning vertex attrib location mismatch " << __orig_result << " -> " << __result << "\n";'
                print r'    }'
            if function.name in ('glCheckFramebufferStatus', 'glCheckFramebufferStatusEXT', 'glCheckNamedFramebufferStatusEXT'):
                print r'    GLint __orig_result = call.ret->toSInt();'
                print r'    if (__orig_result == GL_FRAMEBUFFER_COMPLETE &&'
                print r'        __result != GL_FRAMEBUFFER_COMPLETE) {'
                print r'        std::cerr << call.no << ": incomplete framebuffer (" << __result << ")\n";'
                print r'    }'
            print '    }'

    def extract_arg(self, function, arg, arg_type, lvalue, rvalue):
        if function.name in self.array_pointer_function_names and arg.name == 'pointer':
            print '    %s = static_cast<%s>(%s.toPointer());' % (lvalue, arg_type, rvalue)
            return

        if function.name in self.draw_elements_function_names and arg.name == 'indices':
            self.extract_opaque_arg(function, arg, arg_type, lvalue, rvalue)
            return

        # Handle pointer with offsets into the current pack pixel buffer
        # object.
        if function.name in self.pack_function_names and arg.output:
            self.extract_opaque_arg(function, arg, arg_type, lvalue, rvalue)
            return

        if arg.type is glapi.GLlocation \
           and 'program' not in [arg.name for arg in function.args]:
            print '    GLint program = -1;'
            print '    glGetIntegerv(GL_CURRENT_PROGRAM, &program);'
        
        if arg.type is glapi.GLlocationARB \
           and 'programObj' not in [arg.name for arg in function.args]:
            print '    GLhandleARB programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);'

        Retracer.extract_arg(self, function, arg, arg_type, lvalue, rvalue)

        # Don't try to use more samples than the implementation supports
        if arg.name == 'samples':
            assert arg.type is glapi.GLsizei
            print '    GLint max_samples = 0;'
            print '    glGetIntegerv(GL_MAX_SAMPLES, &max_samples);'
            print '    if (samples > max_samples) {'
            print '        samples = max_samples;'
            print '    }'


if __name__ == '__main__':
    print r'''
#include <string.h>

#include "glproc.hpp"
#include "glretrace.hpp"


'''
    api = glapi.glapi
    api.add_function(glapi.memcpy)
    retracer = GlRetracer()
    retracer.retrace_api(api)

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


import specs.stdapi as stdapi
import specs.glapi as glapi
import specs.glesapi as glesapi
from retrace import Retracer


class GlRetracer(Retracer):

    table_name = 'glretrace::gl_callbacks'

    def retraceFunction(self, function):
        Retracer.retraceFunction(self, function)

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
        "glDrawArraysInstancedBaseInstance",
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
        "glDrawElementsInstancedEXT",
        "glDrawElementsInstancedBaseVertex",
        "glDrawElementsInstancedBaseInstance",
        "glDrawElementsInstancedBaseVertexBaseInstance",
        "glDrawRangeElements",
        "glDrawRangeElementsEXT",
        "glDrawRangeElementsBaseVertex",
        "glMultiDrawElements",
        "glMultiDrawElementsBaseVertex",
        "glMultiDrawElementsEXT",
        "glMultiModeDrawElementsIBM",
    ])

    draw_indirect_function_names = set([
        "glDrawArraysIndirect",
        "glDrawElementsIndirect",
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
        "glBindFramebufferEXT",
        "glBindFramebufferOES",
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
        'glGetSeparableFilter',
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

    map_function_names = set([
        'glMapBuffer',
        'glMapBufferARB',
        'glMapBufferOES',
        'glMapBufferRange',
        'glMapNamedBufferEXT',
        'glMapNamedBufferRangeEXT'
    ])

    unmap_function_names = set([
        'glUnmapBuffer',
        'glUnmapBufferARB',
        'glUnmapBufferOES',
        'glUnmapNamedBufferEXT',
    ])

    def retraceFunctionBody(self, function):
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
                self.failFunction(function)
                print '        }'

            if is_draw_elements:
                print '        GLint __element_array_buffer = 0;'
                print '        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__element_array_buffer);'
                print '        if (!__element_array_buffer) {'
                self.failFunction(function)
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
            print '    assert(call.flags & trace::CALL_FLAG_SWAP_RENDERTARGET);'
        if function.name == 'glFrameTerminatorGREMEDY':
            print '    glretrace::frame_complete(call);'
            return

        Retracer.retraceFunctionBody(self, function)

        # Post-snapshots
        if function.name in ('glFlush', 'glFinish'):
            print '    if (!glretrace::double_buffer) {'
            print '        glretrace::frame_complete(call);'
            print '    }'
        if is_draw_array or is_draw_elements or is_misc_draw:
            print '    assert(call.flags & trace::CALL_FLAG_RENDER);'


    def invokeFunction(self, function):
        # Infer the drawable size from GL calls
        if function.name == "glViewport":
            print '    glretrace::updateDrawable(x + width, y + height);'
        if function.name in ('glBlitFramebuffer', 'glBlitFramebufferEXT'):
            # Some applications do all their rendering in a framebuffer, and
            # then just blit to the drawable without ever calling glViewport.
            print '    glretrace::updateDrawable(std::max(dstX0, dstX1), std::max(dstY0, dstY1));'

        if function.name == "glEnd":
            print '    glretrace::insideGlBeginEnd = false;'

        if function.name.startswith('gl') and not function.name.startswith('glX'):
            print r'    if (!glretrace::context && !glretrace::benchmark && !retrace::profiling) {'
            print r'        retrace::warning(call) << "no current context\n";'
            print r'    }'

        if function.name == 'memcpy':
            print '    if (!dest || !src || !n) return;'
        
        Retracer.invokeFunction(self, function)

        # Error checking
        if function.name == "glBegin":
            print '    glretrace::insideGlBeginEnd = true;'
        elif function.name.startswith('gl'):
            # glGetError is not allowed inside glBegin/glEnd
            print '    if (!glretrace::benchmark && !retrace::profiling && !glretrace::insideGlBeginEnd) {'
            print '        glretrace::checkGlError(call);'
            if function.name in ('glProgramStringARB', 'glProgramStringNV'):
                print r'        GLint error_position = -1;'
                print r'        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_position);'
                print r'        if (error_position != -1) {'
                print r'            const char *error_string = (const char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);'
                print r'            retrace::warning(call) << error_string << "\n";'
                print r'        }'
            if function.name == 'glCompileShader':
                print r'        GLint compile_status = 0;'
                print r'        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);'
                print r'        if (!compile_status) {'
                print r'             GLint info_log_length = 0;'
                print r'             glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);'
                print r'             GLchar *infoLog = new GLchar[info_log_length];'
                print r'             glGetShaderInfoLog(shader, info_log_length, NULL, infoLog);'
                print r'             retrace::warning(call) << infoLog << "\n";'
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
                print r'             retrace::warning(call) << infoLog << "\n";'
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
                print r'             retrace::warning(call) << infoLog << "\n";'
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
                print r'             retrace::warning(call) << infoLog << "\n";'
                print r'             delete [] infoLog;'
                print r'        }'
            if function.name in self.map_function_names:
                print r'        if (!__result) {'
                print r'             retrace::warning(call) << "failed to map buffer\n";'
                print r'        }'
            if function.name in ('glGetAttribLocation', 'glGetAttribLocationARB'):
                print r'    GLint __orig_result = call.ret->toSInt();'
                print r'    if (__result != __orig_result) {'
                print r'        retrace::warning(call) << "vertex attrib location mismatch " << __orig_result << " -> " << __result << "\n";'
                print r'    }'
            if function.name in ('glCheckFramebufferStatus', 'glCheckFramebufferStatusEXT', 'glCheckNamedFramebufferStatusEXT'):
                print r'    GLint __orig_result = call.ret->toSInt();'
                print r'    if (__orig_result == GL_FRAMEBUFFER_COMPLETE &&'
                print r'        __result != GL_FRAMEBUFFER_COMPLETE) {'
                print r'        retrace::warning(call) << "incomplete framebuffer (" << glstate::enumToString(__result) << ")\n";'
                print r'    }'
            print '    }'

            # Query the buffer length for whole buffer mappings
            if function.name in self.map_function_names:
                if 'length' in function.argNames():
                    assert 'BufferRange' in function.name
                else:
                    assert 'BufferRange' not in function.name
                    print r'    GLint length = 0;'
                    if function.name in ('glMapBuffer', 'glMapBufferOES'):
                        print r'    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &length);'
                    elif function.name == 'glMapBufferARB':
                        print r'    glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &length);'
                    elif function.name == 'glMapNamedBufferEXT':
                        print r'    glGetNamedBufferParameterivEXT(buffer, GL_BUFFER_SIZE, &length);'
                    else:
                        assert False
            # Destroy the buffer mapping
            if function.name in self.unmap_function_names:
                print r'        GLvoid *ptr = NULL;'
                if function.name == 'glUnmapBuffer':
                    print r'            glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &ptr);'
                elif function.name == 'glUnmapBufferARB':
                    print r'            glGetBufferPointervARB(target, GL_BUFFER_MAP_POINTER_ARB, &ptr);'
                elif function.name == 'glUnmapBufferOES':
                    print r'            glGetBufferPointervOES(target, GL_BUFFER_MAP_POINTER_OES, &ptr);'
                elif function.name == 'glUnmapNamedBufferEXT':
                    print r'            glGetNamedBufferPointervEXT(buffer, GL_BUFFER_MAP_POINTER, &ptr);'
                else:
                    assert False
                print r'        if (ptr) {'
                print r'            retrace::delRegionByPointer(ptr);'
                print r'        }'

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        if function.name in self.array_pointer_function_names and arg.name == 'pointer':
            print '    %s = static_cast<%s>(retrace::toPointer(%s, true));' % (lvalue, arg_type, rvalue)
            return

        if function.name in self.draw_elements_function_names and arg.name == 'indices' or\
           function.name in self.draw_indirect_function_names and arg.name == 'indirect':
            self.extractOpaqueArg(function, arg, arg_type, lvalue, rvalue)
            return

        # Handle pointer with offsets into the current pack pixel buffer
        # object.
        if function.name in self.pack_function_names and arg.output:
            self.extractOpaqueArg(function, arg, arg_type, lvalue, rvalue)
            return

        if arg.type is glapi.GLlocation \
           and 'program' not in function.argNames():
            print '    GLint program = -1;'
            print '    glGetIntegerv(GL_CURRENT_PROGRAM, &program);'
        
        if arg.type is glapi.GLlocationARB \
           and 'programObj' not in function.argNames():
            print '    GLhandleARB programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);'

        Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)

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
#include "glstate.hpp"


'''
    api = glapi.glapi
    api.addApi(glesapi.glesapi)
    retracer = GlRetracer()
    retracer.retraceApi(api)

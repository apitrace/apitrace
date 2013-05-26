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


from retrace import Retracer
import specs.stdapi as stdapi
import specs.glapi as glapi
import specs.glesapi as glesapi


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
        'glMultiDrawArraysIndirect',
        'glMultiDrawArraysIndirectAMD',
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
        'glMultiDrawElementsIndirect',
        'glMultiDrawElementsIndirectAMD',
    ])

    draw_indirect_function_names = set([
        "glDrawArraysIndirect",
        "glDrawElementsIndirect",
        'glMultiDrawArraysIndirect',
        'glMultiDrawArraysIndirectAMD',
        'glMultiDrawElementsIndirect',
        'glMultiDrawElementsIndirectAMD',
    ])

    misc_draw_function_names = set([
        "glCallList",
        "glCallLists",
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
        'glGetCompressedTexImageARB',
        'glGetCompressedTextureImageEXT',
        'glGetCompressedMultiTexImageEXT',
        'glGetConvolutionFilter',
        'glGetHistogram',
        'glGetMinmax',
        'glGetPixelMapfv',
        'glGetPixelMapuiv',
        'glGetPixelMapusv',
        'glGetPolygonStipple',
        'glGetSeparableFilter',
        'glGetTexImage',
        'glGetTextureImageEXT',
        'glGetMultiTexImageEXT',
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
        'glMapNamedBufferRangeEXT',
        'glMapObjectBufferATI',
    ])

    unmap_function_names = set([
        'glUnmapBuffer',
        'glUnmapBufferARB',
        'glUnmapBufferOES',
        'glUnmapNamedBufferEXT',
        'glUnmapObjectBufferATI',
    ])

    def retraceFunctionBody(self, function):
        is_array_pointer = function.name in self.array_pointer_function_names
        is_draw_array = function.name in self.draw_array_function_names
        is_draw_elements = function.name in self.draw_elements_function_names
        is_misc_draw = function.name in self.misc_draw_function_names

        if is_array_pointer or is_draw_array or is_draw_elements:
            print '    if (retrace::parser.version < 1) {'

            if is_array_pointer or is_draw_array:
                print '        GLint _array_buffer = 0;'
                print '        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &_array_buffer);'
                print '        if (!_array_buffer) {'
                self.failFunction(function)
                print '        }'

            if is_draw_elements:
                print '        GLint _element_array_buffer = 0;'
                print '        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &_element_array_buffer);'
                print '        if (!_element_array_buffer) {'
                self.failFunction(function)
                print '        }'
            
            print '    }'

        # When no pack buffer object is bound, the pack functions are no-ops.
        if function.name in self.pack_function_names:
            print '    GLint _pack_buffer = 0;'
            print '    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &_pack_buffer);'
            print '    if (!_pack_buffer) {'
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
            print '    if (!retrace::doubleBuffer) {'
            print '        glretrace::frame_complete(call);'
            print '    }'
        if is_draw_array or is_draw_elements or is_misc_draw:
            print '    assert(call.flags & trace::CALL_FLAG_RENDER);'


    def invokeFunction(self, function):
        # Infer the drawable size from GL calls
        if function.name == "glViewport":
            print '    glretrace::updateDrawable(x + width, y + height);'
        if function.name == "glViewportArray":
            # We are concerned about drawables so only care for the first viewport
            print '    if (first == 0 && count > 0) {'
            print '        GLfloat x = v[0], y = v[1], w = v[2], h = v[3];'
            print '        glretrace::updateDrawable(x + w, y + h);'
            print '    }'
        if function.name == "glViewportIndexedf":
            print '    if (index == 0) {'
            print '        glretrace::updateDrawable(x + w, y + h);'
            print '    }'
        if function.name == "glViewportIndexedfv":
            print '    if (index == 0) {'
            print '        GLfloat x = v[0], y = v[1], w = v[2], h = v[3];'
            print '        glretrace::updateDrawable(x + w, y + h);'
            print '    }'
        if function.name in ('glBlitFramebuffer', 'glBlitFramebufferEXT'):
            # Some applications do all their rendering in a framebuffer, and
            # then just blit to the drawable without ever calling glViewport.
            print '    glretrace::updateDrawable(std::max(dstX0, dstX1), std::max(dstY0, dstY1));'

        if function.name == "glEnd":
            print '    glretrace::insideGlBeginEnd = false;'

        if function.name.startswith('gl') and not function.name.startswith('glX'):
            print r'    if (retrace::debug && !glretrace::getCurrentContext()) {'
            print r'        retrace::warning(call) << "no current context\n";'
            print r'    }'

        if function.name == 'memcpy':
            print '    if (!dest || !src || !n) return;'

        # Skip glEnable/Disable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB) as we don't
        # faithfully set the CONTEXT_DEBUG_BIT_ARB flags on context creation.
        if function.name in ('glEnable', 'glDisable'):
            print '    if (cap == GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB) return;'

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
            elif function.name == 'glUnmapObjectBufferATI':
                # TODO
                pass
            else:
                assert False
            print r'        if (ptr) {'
            print r'            retrace::delRegionByPointer(ptr);'
            print r'        } else {'
            print r'            retrace::warning(call) << "no current context\n";'
            print r'        }'

        if function.name in ('glBindProgramPipeline', 'glBindProgramPipelineEXT'):
            # Note if glBindProgramPipeline has ever been called
            print r'    if (pipeline) {'
            print r'        _pipelineHasBeenBound = true;'
            print r'    }'

        profileDraw = (
            function.name in self.draw_array_function_names or
            function.name in self.draw_elements_function_names or
            function.name in self.draw_indirect_function_names or
            function.name in self.misc_draw_function_names or
            function.name == 'glBegin'
        )

        if function.name in ('glUseProgram', 'glUseProgramObjectARB'):
            print r'    glretrace::Context *currentContext = glretrace::getCurrentContext();'
            print r'    if (currentContext) {'
            print r'        currentContext->activeProgram = call.arg(0).toUInt();'
            print r'    }'

        # Only profile if not inside a list as the queries get inserted into list
        if function.name == 'glNewList':
            print r'    glretrace::insideList = true;'

        if function.name == 'glEndList':
            print r'    glretrace::insideList = false;'

        if function.name != 'glEnd':
            print r'    if (!glretrace::insideList && !glretrace::insideGlBeginEnd && retrace::profiling) {'
            if profileDraw:
                print r'        glretrace::beginProfile(call, true);'
            else:
                print r'        glretrace::beginProfile(call, false);'
            print r'    }'

        if function.name == 'glCreateShaderProgramv':
            # When dumping state, break down glCreateShaderProgramv so that the
            # shader source can be recovered.
            print r'    if (retrace::dumpingState) {'
            print r'        GLuint _shader = glCreateShader(type);'
            print r'        if (_shader) {'
            print r'            glShaderSource(_shader, count, strings, NULL);'
            print r'            glCompileShader(_shader);'
            print r'            const GLuint _program = glCreateProgram();'
            print r'            if (_program) {'
            print r'                GLint compiled = GL_FALSE;'
            print r'                glGetShaderiv(_shader, GL_COMPILE_STATUS, &compiled);'
            print r'                glProgramParameteri(_program, GL_PROGRAM_SEPARABLE, GL_TRUE);'
            print r'                if (compiled) {'
            print r'                    glAttachShader(_program, _shader);'
            print r'                    glLinkProgram(_program);'
            print r'                    //glDetachShader(_program, _shader);'
            print r'                }'
            print r'                //append-shader-info-log-to-program-info-log'
            print r'            }'
            print r'            //glDeleteShader(_shader);'
            print r'            _result = _program;'
            print r'        } else {'
            print r'            _result = 0;'
            print r'        }'
            print r'    } else {'
            Retracer.invokeFunction(self, function)
            print r'    }'
        else:
            Retracer.invokeFunction(self, function)

        if function.name == "glBegin":
            print '    glretrace::insideGlBeginEnd = true;'

        print r'    if (!glretrace::insideList && !glretrace::insideGlBeginEnd && retrace::profiling) {'
        if profileDraw:
            print r'        glretrace::endProfile(call, true);'
        else:
            print r'        glretrace::endProfile(call, false);'
        print r'    }'

        # Error checking
        if function.name.startswith('gl'):
            # glGetError is not allowed inside glBegin/glEnd
            print '    if (retrace::debug && !glretrace::insideGlBeginEnd && glretrace::getCurrentContext()) {'
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
            if function.name in ('glLinkProgram', 'glCreateShaderProgramv', 'glCreateShaderProgramEXT'):
                if function.name != 'glLinkProgram':
                    print r'        GLuint program = _result;'
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
                print r'        if (!_result) {'
                print r'             retrace::warning(call) << "failed to map buffer\n";'
                print r'        }'
            if function.name in self.unmap_function_names and function.type is not stdapi.Void:
                print r'        if (!_result) {'
                print r'             retrace::warning(call) << "failed to unmap buffer\n";'
                print r'        }'
            if function.name in ('glGetAttribLocation', 'glGetAttribLocationARB'):
                print r'    GLint _origResult = call.ret->toSInt();'
                print r'    if (_result != _origResult) {'
                print r'        retrace::warning(call) << "vertex attrib location mismatch " << _origResult << " -> " << _result << "\n";'
                print r'    }'
            if function.name in ('glCheckFramebufferStatus', 'glCheckFramebufferStatusEXT', 'glCheckNamedFramebufferStatusEXT'):
                print r'    GLint _origResult = call.ret->toSInt();'
                print r'    if (_origResult == GL_FRAMEBUFFER_COMPLETE &&'
                print r'        _result != GL_FRAMEBUFFER_COMPLETE) {'
                print r'        retrace::warning(call) << "incomplete framebuffer (" << glstate::enumToString(_result) << ")\n";'
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
                elif function.name == 'glMapObjectBufferATI':
                    print r'    glGetObjectBufferivATI(buffer, GL_OBJECT_BUFFER_SIZE_ATI, &length);'
                else:
                    assert False

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
            assert isinstance(arg_type, (stdapi.Pointer, stdapi.Array, stdapi.Blob, stdapi.Opaque))
            print '    %s = static_cast<%s>((%s).toPointer());' % (lvalue, arg_type, rvalue)
            return

        if arg.type is glapi.GLlocation \
           and 'program' not in function.argNames():
            # Determine the active program for uniforms swizzling
            print '    GLint program = -1;'
            print '    if (glretrace::insideList) {'
            print '        // glUseProgram & glUseProgramObjectARB are display-list-able'
            print r'    glretrace::Context *currentContext = glretrace::getCurrentContext();'
            print '        program = _program_map[currentContext->activeProgram];'
            print '    } else {'
            print '        GLint pipeline = 0;'
            print '        if (_pipelineHasBeenBound) {'
            print '            glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &pipeline);'
            print '        }'
            print '        if (pipeline) {'
            print '            glGetProgramPipelineiv(pipeline, GL_ACTIVE_PROGRAM, &program);'
            print '        } else {'
            print '            glGetIntegerv(GL_CURRENT_PROGRAM, &program);'
            print '        }'
            print '    }'
            print

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

        # These parameters are referred beyond the call life-time
        # TODO: Replace ad-hoc solution for bindable parameters with general one
        if function.name in ('glFeedbackBuffer', 'glSelectBuffer') and arg.output:
            print '    _allocator.bind(%s);' % arg.name



if __name__ == '__main__':
    print r'''
#include <string.h>

#include "glproc.hpp"
#include "glretrace.hpp"
#include "glstate.hpp"


static bool _pipelineHasBeenBound = false;
'''
    api = stdapi.API()
    api.addModule(glapi.glapi)
    api.addModule(glesapi.glesapi)
    retracer = GlRetracer()
    retracer.retraceApi(api)

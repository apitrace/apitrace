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


import re
import sys

from retrace import Retracer
import specs.stdapi as stdapi
import specs.glapi as glapi


class GlRetracer(Retracer):

    table_name = 'glretrace::gl_callbacks'

    def retraceApi(self, api):
        # Ensure pack function have side effects
        abort = False
        for function in api.getAllFunctions():
            if not function.sideeffects:
                if self.pack_function_regex.match(function.name) or \
                   function.name.startswith('glGetQueryObject'):
                    sys.stderr.write('error: function %s must have sideeffects\n' % function.name)
                    abort = True
        if abort:
            sys.exit(1)

        Retracer.retraceApi(self, api)

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

        "glMultiTexCoordPointerEXT",

        "glVertexAttribPointer",
        "glVertexAttribPointerARB",
        "glVertexAttribPointerNV",
        "glVertexAttribIPointer",
        "glVertexAttribIPointerEXT",
        "glVertexAttribLPointer",
        "glVertexAttribLPointerEXT",
        
        #"glMatrixIndexPointerARB",
    ))

    draw_arrays_function_regex = re.compile(r'^gl([A-Z][a-z]+)*Draw(Range)?Arrays([A-Z][a-zA-Z]*)?$' )
    draw_elements_function_regex = re.compile(r'^gl([A-Z][a-z]+)*Draw(Range)?Elements([A-Z][a-zA-Z]*)?$' )
    draw_indirect_function_regex = re.compile(r'^gl([A-Z][a-z]+)*Draw(Range)?(Arrays|Elements)Indirect([A-Z][a-zA-Z]*)?$' )

    misc_draw_function_regex = re.compile(r'^gl(' + r'|'.join([
        r'CallList',
        r'CallLists',
        r'Clear',
        r'End',
        r'DrawPixels',
        r'DrawTransformFeedback([A-Z][a-zA-Z]*)?',
        r'BlitFramebuffer',
        r'Rect[dfis]v?',
        r'EvalMesh[0-9]+',
    ]) + r')[0-9A-Z]*$')


    bind_framebuffer_function_regex = re.compile(r'^glBindFramebuffer[0-9A-Z]*$')

    # Names of the functions that can pack into the current pixel buffer
    # object.  See also the ARB_pixel_buffer_object specification.
    pack_function_regex = re.compile(r'^gl(' + r'|'.join([
        r'Getn?Histogram',
        r'Getn?PolygonStipple',
        r'Getn?PixelMap[a-z]+v',
        r'Getn?Minmax',
        r'Getn?(Convolution|Separable)Filter',
        r'Getn?(Compressed)?(Multi)?Tex(ture)?(Sub)?Image',
        r'Readn?Pixels',
    ]) + r')[0-9A-Z]*$')

    map_function_regex = re.compile(r'^glMap(|Named|Object)Buffer(Range)?[0-9A-Z]*$')

    unmap_function_regex = re.compile(r'^glUnmap(|Named|Object)Buffer[0-9A-Z]*$')

    def retraceFunctionBody(self, function):
        is_array_pointer = function.name in self.array_pointer_function_names
        is_draw_arrays = self.draw_arrays_function_regex.match(function.name) is not None
        is_draw_elements = self.draw_elements_function_regex.match(function.name) is not None
        is_draw_indirect = self.draw_indirect_function_regex.match(function.name) is not None
        is_misc_draw = self.misc_draw_function_regex.match(function.name)

        if function.name.startswith('gl') and not function.name.startswith('glX'):
            # The Windows OpenGL runtime will skip calls when there's no
            # context bound to the current context, but this might cause
            # crashes on other systems, particularly with NVIDIA Linux drivers.
            print(r'    glretrace::Context *currentContext = glretrace::getCurrentContext();')
            print(r'    if (!currentContext) {')
            print(r'        if (retrace::debug > 0) {')
            print(r'            retrace::warning(call) << "no current context\n";')
            print(r'        }')
            print(r'#ifndef _WIN32')
            print(r'        return;')
            print(r'#endif')
            print(r'    }')

            print(r'    if (retrace::markers) {')
            print(r'        glretrace::insertCallMarker(call, currentContext);')
            print(r'    }')

        # For backwards compatibility with old traces where non VBO drawing was supported
        if (is_array_pointer or is_draw_arrays or is_draw_elements) and not is_draw_indirect:
            print('    if (retrace::parser->getVersion() < 1) {')

            if is_array_pointer or is_draw_arrays:
                print('        GLint _array_buffer = 0;')
                print('        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &_array_buffer);')
                print('        if (!_array_buffer) {')
                self.failFunction(function)
                print('        }')

            if is_draw_elements:
                print('        GLint _element_array_buffer = 0;')
                print('        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &_element_array_buffer);')
                print('        if (!_element_array_buffer) {')
                self.failFunction(function)
                print('        }')
            
            print('    }')

        # When no query buffer object is bound, and we don't request that glGetQueryObject
        # is run than glGetQueryObject is a no-op.
        if function.name.startswith('glGetQueryObject'):
            print(r'    GLint _query_buffer = 0;')
            print(r'    if (currentContext && currentContext->features().query_buffer_object) {')
            print(r'        glGetIntegerv(GL_QUERY_BUFFER_BINDING, &_query_buffer);')
            print(r'    }')
            print(r'    if (!_query_buffer && retrace::queryHandling == retrace::QUERY_SKIP) {')
            print(r'        return;')
            print(r'    }')
            print(r'wait_for_query_result:')

        # Pre-snapshots
        if self.bind_framebuffer_function_regex.match(function.name):
            print('    assert(call.flags & trace::CALL_FLAG_SWAP_RENDERTARGET);')
        if function.name == 'glStringMarkerGREMEDY':
            return
        if function.name == 'glFrameTerminatorGREMEDY':
            print('    glretrace::frame_complete(call);')
            return

        Retracer.retraceFunctionBody(self, function)

        # If we have a query buffer or if we execute the query we have to loop until the
        # query result is actually available to get the proper result - for the following
        # execution if the query buffer is used or for the check to make sense, and if we
        # just want to execute the query for timing purpouses we also should wait
        # for the result.
        if function.name.startswith('glGetQueryObject'):
           print(r'    if (!_query_buffer && retrace::queryHandling != retrace::QUERY_SKIP) {')
           print(r'        auto query_result = call.arg(2).toArray();')
           print(r'        assert(query_result && query_result->values.size() == 1);')
           print(r'        auto expect = static_cast<decltype(retval)>(query_result->values[0]->toUInt());')
           print(r'        if (call.arg(1).toUInt() == GL_QUERY_RESULT_AVAILABLE) {')
           print(r'            if (expect == 1 && retval == 0)')
           print(r'                goto wait_for_query_result;')
           print(r'        } else if (retrace::queryHandling == retrace::QUERY_RUN_AND_CHECK_RESULT &&')
           print(r'                   abs(static_cast<int64_t>(expect) - static_cast<int64_t>(retval)) > retrace::queryTolerance) {')
           print(r'           retrace::warning(call) << "Warning: query returned " << retval << " but trace contained " << expect')
           print(r'                                  << " (tol = " << retrace::queryTolerance << ")\n";')
           print(r'        }')
           print(r'    }')


        # Post-snapshots
        if function.name in ('glFlush', 'glFinish'):
            print('    if (!retrace::doubleBuffer) {')
            print('        glretrace::frame_complete(call);')
            print('    }')
        if is_draw_arrays or is_draw_elements or is_misc_draw:
            print('    assert(call.flags & trace::CALL_FLAG_RENDER);')

    def overrideArgs(self, function):
        if not self.pack_function_regex.match(function.name):
            return

        print(r'    GLint _pack_buffer = 0;')
        print(r'    if (currentContext && currentContext->features().pixel_buffer_object) {')
        print(r'        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &_pack_buffer);')
        print(r'    }')
        print(r'     std::vector<char> buffer;')
        print(r'    if (!_pack_buffer) {')
        # if no pack buffer is bound we have to read back
        data_param_name = "pixels"
        if function.name == "glGetTexImage":
            # TODO: https://github.com/apitrace/apitrace/commit/2a83ddd4f67014e2aacf99c6b203fd3f6b13c4f3#r130319306
            print(r'        return;')
        elif function.name == "glGetTexnImage":
            print(r'     buffer.resize(call.arg(4).toUInt());');
        elif function.name == "glGetTextureImage":
            print(r'     buffer.resize(call.arg(4).toUInt());');
        elif function.name == "glReadPixels":
            print(r'     GLsizei _w = call.arg(2).toSInt();')
            print(r'     GLsizei _h = call.arg(3).toSInt();')
            print(r'     buffer.resize(_w * _h * 64);')
        elif function.name == "glReadnPixels":
            print(r'     buffer.resize(call.arg(6).toSInt());')
            data_param_name = "data"
        else:
            print(r'    return;')
            print(r'    }')
            return

        print(r'    }')
        print('    {} = buffer.data();'.format(data_param_name))


    def invokeFunction(self, function):
        if function.name == "glGetActiveUniformBlockName":
            print('    std::vector<GLchar> name_buf(bufSize);')
            print('    uniformBlockName = name_buf.data();')
            print('    const auto traced_name = (const GLchar *)((call.arg(4)).toString());')
            print('    glretrace::mapUniformBlockName(program, (call.arg(1)).toSInt(), traced_name, _uniformBlock_map);')
        if function.name == "glGetProgramResourceName":
            print('    std::vector<GLchar> name_buf(bufSize);')
            print('    name = name_buf.data();')
            print('    const auto traced_name = (const GLchar *)((call.arg(5)).toString());')
            print('    glretrace::trackResourceName(program, programInterface, index, traced_name);')
        if function.name == "glGetProgramResourceiv":
            print('    glretrace::mapResourceLocation(program, programInterface, index, call.arg(4).toArray(), call.arg(7).toArray(), _location_map);')
        # Infer the drawable size from GL calls
        if function.name == "glViewport":
            print('    glretrace::updateDrawable(x + width, y + height);')
        if function.name == "glViewportArrayv":
            # We are concerned about drawables so only care for the first viewport
            print('    if (first == 0 && count > 0) {')
            print('        GLfloat x = v[0], y = v[1], w = v[2], h = v[3];')
            print('        glretrace::updateDrawable(x + w, y + h);')
            print('    }')
        if function.name == "glViewportIndexedf":
            print('    if (index == 0) {')
            print('        glretrace::updateDrawable(x + w, y + h);')
            print('    }')
        if function.name == "glViewportIndexedfv":
            print('    if (index == 0) {')
            print('        GLfloat x = v[0], y = v[1], w = v[2], h = v[3];')
            print('        glretrace::updateDrawable(x + w, y + h);')
            print('    }')
        if function.name in ('glBlitFramebuffer', 'glBlitFramebufferEXT'):
            # Some applications do all their rendering in a framebuffer, and
            # then just blit to the drawable without ever calling glViewport.
            print('    glretrace::updateDrawable(std::max(dstX0, dstX1), std::max(dstY0, dstY1));')

        if function.name == "glEnd":
            print(r'    if (currentContext) {')
            print(r'        currentContext->insideBeginEnd = false;')
            print(r'    }')

        if function.name == 'memcpy':
            print('    if (!dest || !src || !n) return;')

        # Skip glEnable/Disable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB) as we don't
        # faithfully set the CONTEXT_DEBUG_BIT_ARB flags on context creation.
        if function.name in ('glEnable', 'glDisable'):
            print('    if (cap == GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB) return;')

        # Destroy the buffer mapping
        if self.unmap_function_regex.match(function.name):
            print(r'        GLvoid *ptr = NULL;')
            if function.name == 'glUnmapBuffer':
                print(r'            glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &ptr);')
            elif function.name == 'glUnmapBufferARB':
                print(r'            glGetBufferPointervARB(target, GL_BUFFER_MAP_POINTER_ARB, &ptr);')
            elif function.name == 'glUnmapBufferOES':
                print(r'            glGetBufferPointervOES(target, GL_BUFFER_MAP_POINTER_OES, &ptr);')
            elif function.name == 'glUnmapNamedBuffer':
                print(r'            glGetNamedBufferPointerv(buffer, GL_BUFFER_MAP_POINTER, &ptr);')
            elif function.name == 'glUnmapNamedBufferEXT':
                print(r'            glGetNamedBufferPointervEXT(buffer, GL_BUFFER_MAP_POINTER, &ptr);')
            elif function.name == 'glUnmapObjectBufferATI':
                # TODO
                pass
            else:
                assert False
            print(r'        if (ptr) {')
            print(r'            retrace::delRegionByPointer(ptr);')
            print(r'        } else {')
            print(r'            retrace::warning(call) << "failed to get mapped pointer\n";')
            print(r'        }')

        # Implicit destruction of buffer mappings
        # TODO: handle BufferData variants
        # TODO: don't rely on GL_ARB_direct_state_access
        if function.name in ('glDeleteBuffers', 'glDeleteBuffersARB'):
            print(r'    if (currentContext && currentContext->features().ARB_direct_state_access) {')
            print(r'        for (GLsizei i = 0; i < n; ++i) {')
            print(r'            GLuint buffer = buffers[i];')
            print(r'            if (buffer != 0 && glIsBuffer(buffer)) {')
            print(r'                GLvoid *ptr = nullptr;')
            print(r'                glGetNamedBufferPointerv(buffers[i], GL_BUFFER_MAP_POINTER, &ptr);')
            print(r'                if (ptr) {')
            print(r'                    retrace::delRegionByPointer(ptr);')
            print(r'                }')
            print(r'            }')
            print(r'        }')
            print(r'    }')

        if function.name.startswith('glCopyImageSubData'):
            print(r'    if (srcTarget == GL_RENDERBUFFER || dstTarget == GL_RENDERBUFFER) {')
            print(r'        retrace::warning(call) << " renderbuffer targets unsupported (https://git.io/JOMRC)\n";')
            print(r'    }')

        is_draw_arrays = self.draw_arrays_function_regex.match(function.name) is not None
        is_draw_elements = self.draw_elements_function_regex.match(function.name) is not None
        is_misc_draw = self.misc_draw_function_regex.match(function.name) is not None

        profileDraw = (
            is_draw_arrays or
            is_draw_elements or
            is_misc_draw or
            function.name == 'glBegin' or
            function.name.startswith('glDispatchCompute')
        )

        # Only profile if not inside a list as the queries get inserted into list
        if function.name == 'glNewList':
            print(r'    if (currentContext) {')
            print(r'        currentContext->insideList = true;')
            print(r'    }')

        if function.name == 'glEndList':
            print(r'    if (currentContext) {')
            print(r'        currentContext->insideList = false;')
            print(r'    }')

        if function.name == 'glBegin' or \
           is_draw_arrays or \
           is_draw_elements or \
           function.name.startswith('glBeginTransformFeedback'):
            print(r'    if (retrace::debug > 0) {')
            print(r'        _validateActiveProgram(call);')
            print(r'    }')

        if function.name != 'glEnd':
            print(r'    if (currentContext && !currentContext->insideList && !currentContext->insideBeginEnd && retrace::profiling) {')
            if profileDraw:
                print(r'        glretrace::beginProfile(call, true);')
            else:
                print(r'        glretrace::beginProfile(call, false);')
            print(r'    }')

        if function.name in ('glCreateShaderProgramv', 'glCreateShaderProgramEXT', 'glCreateShaderProgramvEXT'):
            # When dumping state, break down glCreateShaderProgram* so that the
            # shader source can be recovered.
            print(r'    if (retrace::dumpingState) {')
            print(r'        GLuint _shader = glCreateShader(type);')
            print(r'        if (_shader) {')
            if not function.name.startswith('glCreateShaderProgramv'):
                print(r'            GLsizei count = 1;')
                print(r'            const GLchar **strings = &string;')
            print(r'            glShaderSource(_shader, count, strings, NULL);')
            print(r'            glCompileShader(_shader);')
            print(r'            const GLuint _program = glCreateProgram();')
            print(r'            if (_program) {')
            print(r'                GLint compiled = GL_FALSE;')
            print(r'                glGetShaderiv(_shader, GL_COMPILE_STATUS, &compiled);')
            if function.name == 'glCreateShaderProgramvEXT':
                print(r'                glProgramParameteriEXT(_program, GL_PROGRAM_SEPARABLE, GL_TRUE);')
            else:
                print(r'                glProgramParameteri(_program, GL_PROGRAM_SEPARABLE, GL_TRUE);')
            print(r'                if (compiled) {')
            print(r'                    glAttachShader(_program, _shader);')
            print(r'                    glLinkProgram(_program);')
            print(r'                    if (false) glDetachShader(_program, _shader);')
            print(r'                }')
            print(r'                // TODO: append shader info log to program info log')
            print(r'            }')
            print(r'            glDeleteShader(_shader);')
            print(r'            _result = _program;')
            print(r'        } else {')
            print(r'            _result = 0;')
            print(r'        }')
            print(r'    } else {')
            Retracer.invokeFunction(self, function)
            print(r'    }')
        elif function.name in ('glDetachShader', 'glDetachObjectARB'):
            print(r'    if (!retrace::dumpingState) {')
            Retracer.invokeFunction(self, function)
            print(r'    }')
        elif function.name == 'glClientWaitSync':
            print(r'    _result = glretrace::clientWaitSync(call, sync, flags, timeout);')
            print(r'    (void)_result;')
        elif function.name == 'glGetSynciv':
            print(r'    if (pname == GL_SYNC_STATUS &&')
            print(r'        bufSize >= 1 &&')
            print(r'        values != NULL &&')
            print(r'        call.arg(4)[0].toSInt() == GL_SIGNALED) {')
            print(r'        // Fence was signalled, so ensure it happened here')
            print(r'        glretrace::blockOnFence(call, sync, GL_SYNC_FLUSH_COMMANDS_BIT);')
            print(r'        (void)length;')
            print(r'    }')
        else:
            Retracer.invokeFunction(self, function)

        # Keep track of current program/pipeline.  Using glGet as opposed to
        # the call parameter ensures the cached value stays consistent despite
        # GL errors.  See also https://github.com/apitrace/apitrace/issues/679
        if function.name in ('glUseProgram'):
            print(r'    if (currentContext) {')
            print(r'        currentContext->currentUserProgram = call.arg(0).toUInt();')
            print(r'        currentContext->currentProgram = _glGetInteger(GL_CURRENT_PROGRAM);')
            print(r'    }')
        if function.name in ('glUseProgramObjectARB',):
            print(r'    if (currentContext) {')
            print(r'        currentContext->currentUserProgram = call.arg(0).toUInt();')
            print(r'        currentContext->currentProgram = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);')
            print(r'    }')
        if function.name in ('glBindProgramPipeline', 'glBindProgramPipelineEXT'):
            print(r'    if (currentContext) {')
            print(r'        currentContext->currentPipeline = pipeline;')
            print(r'    }')

        # Ensure this context flushes before switching to another thread to
        # prevent deadlock.
        # TODO: Defer flushing until another thread actually invokes
        # ClientWaitSync.
        if function.name.startswith("glFenceSync"):
            print('    if (currentContext) {')
            print('        currentContext->needsFlush = true;')
            print('    }')
        if function.name in ("glFlush", "glFinish"):
            print('    if (currentContext) {')
            print('        currentContext->needsFlush = false;')
            print('    }')

        if function.name == "glBegin":
            print('    if (currentContext) {')
            print('        currentContext->insideBeginEnd = true;')
            print('    }')

        print(r'    if (currentContext && !currentContext->insideList && !currentContext->insideBeginEnd && retrace::profiling) {')
        if profileDraw:
            print(r'        glretrace::endProfile(call, true);')
        else:
            print(r'        glretrace::endProfile(call, false);')
        print(r'    }')

        # Error checking
        if function.name.startswith('gl'):
            # glGetError is not allowed inside glBegin/glEnd
            print('    if (retrace::debug > 0 && currentContext && !currentContext->insideBeginEnd) {')
            print('        glretrace::checkGlError(call);')
            if function.name in ('glProgramStringARB', 'glLoadProgramNV'):
                print(r'        GLint error_position = -1;')
                print(r'        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_position);')
                print(r'        if (error_position != -1) {')
                print(r'            const char *error_string = (const char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);')
                print(r'            retrace::warning(call) << "error in position " << error_position << ": " << error_string << "\n";')
                print(r'        }')
            if function.name == 'glCompileShader':
                print(r'        GLint compile_status = 0;')
                print(r'        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);')
                print(r'        if (!compile_status) {')
                print(r'             retrace::warning(call) << "compilation failed\n";')
                print(r'        }')
                print(r'        GLint info_log_length = 0;')
                print(r'        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);')
                print(r'        if (info_log_length > 1) {')
                print(r'             GLchar *infoLog = new GLchar[info_log_length];')
                print(r'             glGetShaderInfoLog(shader, info_log_length, NULL, infoLog);')
                print(r'             retrace::warning(call) << infoLog << "\n";')
                print(r'             delete [] infoLog;')
                print(r'        }')
            if function.name in ('glLinkProgram', 'glCreateShaderProgramv', 'glCreateShaderProgramEXT', 'glCreateShaderProgramvEXT', 'glProgramBinary', 'glProgramBinaryOES'):
                if function.name.startswith('glCreateShaderProgram'):
                    print(r'        GLuint program = _result;')
                print(r'        GLint link_status = 0;')
                print(r'        glGetProgramiv(program, GL_LINK_STATUS, &link_status);')
                print(r'        if (!link_status) {')
                print(r'             retrace::warning(call) << "link failed\n";')
                print(r'        }')
                print(r'        GLint info_log_length = 0;')
                print(r'        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);')
                print(r'        if (info_log_length > 1) {')
                print(r'             GLchar *infoLog = new GLchar[info_log_length];')
                print(r'             glGetProgramInfoLog(program, info_log_length, NULL, infoLog);')
                print(r'             retrace::warning(call) << infoLog << "\n";')
                print(r'             delete [] infoLog;')
                print(r'        }')
            if function.name == 'glCompileShaderARB':
                print(r'        GLint compile_status = 0;')
                print(r'        glGetObjectParameterivARB(shaderObj, GL_OBJECT_COMPILE_STATUS_ARB, &compile_status);')
                print(r'        if (!compile_status) {')
                print(r'             retrace::warning(call) << "compilation failed\n";')
                print(r'        }')
                print(r'        GLint info_log_length = 0;')
                print(r'        glGetObjectParameterivARB(shaderObj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_log_length);')
                print(r'        if (info_log_length > 1) {')
                print(r'             GLchar *infoLog = new GLchar[info_log_length];')
                print(r'             glGetInfoLogARB(shaderObj, info_log_length, NULL, infoLog);')
                print(r'             retrace::warning(call) << infoLog << "\n";')
                print(r'             delete [] infoLog;')
                print(r'        }')
            if function.name == 'glLinkProgramARB':
                print(r'        GLint link_status = 0;')
                print(r'        glGetObjectParameterivARB(programObj, GL_OBJECT_LINK_STATUS_ARB, &link_status);')
                print(r'        if (!link_status) {')
                print(r'             retrace::warning(call) << "link failed\n";')
                print(r'        }')
                print(r'        GLint info_log_length = 0;')
                print(r'        glGetObjectParameterivARB(programObj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_log_length);')
                print(r'        if (info_log_length > 1) {')
                print(r'             GLchar *infoLog = new GLchar[info_log_length];')
                print(r'             glGetInfoLogARB(programObj, info_log_length, NULL, infoLog);')
                print(r'             retrace::warning(call) << infoLog << "\n";')
                print(r'             delete [] infoLog;')
                print(r'        }')
            if self.map_function_regex.match(function.name):
                print(r'        if (!_result) {')
                print(r'             retrace::warning(call) << "failed to map buffer\n";')
                print(r'        }')
            if self.unmap_function_regex.match(function.name) and function.type is not stdapi.Void:
                print(r'        if (!_result) {')
                print(r'             retrace::warning(call) << "failed to unmap buffer\n";')
                print(r'        }')
            if function.name in ('glGetAttribLocation', 'glGetAttribLocationARB'):
                print(r'    GLint _origResult = call.ret->toSInt();')
                print(r'    if (_result != _origResult) {')
                print(r'        retrace::warning(call) << "vertex attrib location mismatch " << _origResult << " -> " << _result << "\n";')
                print(r'    }')
            if function.name in ('glCheckFramebufferStatus', 'glCheckFramebufferStatusEXT', 'glCheckNamedFramebufferStatus', 'glCheckNamedFramebufferStatusEXT'):
                print(r'    GLint _origResult = call.ret->toSInt();')
                print(r'    if (_origResult == GL_FRAMEBUFFER_COMPLETE &&')
                print(r'        _result != GL_FRAMEBUFFER_COMPLETE) {')
                print(r'        retrace::warning(call) << "incomplete framebuffer (" << glstate::enumToString(_result) << ")\n";')
                print(r'    }')
            print('    }')

        # Query the buffer length for whole buffer mappings
        if self.map_function_regex.match(function.name):
            if 'length' in function.argNames():
                assert 'BufferRange' in function.name
            else:
                assert 'BufferRange' not in function.name
                print(r'    GLint length = 0;')
                if function.name in ('glMapBuffer', 'glMapBufferOES'):
                    print(r'    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &length);')
                elif function.name == 'glMapBufferARB':
                    print(r'    glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &length);')
                elif function.name == 'glMapNamedBuffer':
                    print(r'    glGetNamedBufferParameteriv(buffer, GL_BUFFER_SIZE, &length);')
                elif function.name == 'glMapNamedBufferEXT':
                    print(r'    glGetNamedBufferParameterivEXT(buffer, GL_BUFFER_SIZE, &length);')
                elif function.name == 'glMapObjectBufferATI':
                    print(r'    glGetObjectBufferivATI(buffer, GL_OBJECT_BUFFER_SIZE_ATI, &length);')
                else:
                    assert False

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        if function.name in self.array_pointer_function_names and arg.name == 'pointer':
            print('    %s = static_cast<%s>(retrace::toPointer(%s, true));' % (lvalue, arg_type, rvalue))
            return

        if self.draw_elements_function_regex.match(function.name) and arg.name == 'indices' or\
           self.draw_indirect_function_regex.match(function.name) and arg.name == 'indirect':
            self.extractOpaqueArg(function, arg, arg_type, lvalue, rvalue)
            return

        # Handle pointer with offsets into the current pack pixel buffer
        # object.
        if self.pack_function_regex.match(function.name) and arg.output:
            assert isinstance(arg_type, (stdapi.Pointer, stdapi.Array, stdapi.Blob, stdapi.Opaque))
            print('    %s = static_cast<%s>((%s).toPointer());' % (lvalue, arg_type, rvalue))
            return
        if function.name.startswith('glGetQueryObject') and arg.output:
            pointer_type = "%s" % (arg_type)
            basetype = pointer_type.split(" ")[0]
            print('    %s retval;' % (basetype))
            print('    if (_query_buffer)')
            print('        %s = static_cast<%s>((%s).toPointer());' % (lvalue, arg_type, rvalue))
            print('    else')
            print('        %s = &retval;' % (lvalue))
            return

        if (arg.type.depends(glapi.GLlocation) or \
            arg.type.depends(glapi.GLsubroutine)) \
           and 'program' not in function.argNames():
            # Determine the active program for uniforms swizzling
            print('    GLint program = _getActiveProgram();')

        if arg.type is glapi.GLlocationARB \
           and 'programObj' not in function.argNames():
            print('    GLhandleARB programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);')

        Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)

        # Don't try to use more samples than the implementation supports
        if arg.name == 'samples':
            if function.name == 'glRasterSamplesEXT':
                assert arg.type is glapi.GLuint
                print('    GLint max_samples = 0;')
                print('    glGetIntegerv(GL_MAX_RASTER_SAMPLES_EXT, &max_samples);')
                print('    if (samples > static_cast<GLuint>(max_samples)) {')
                print('        samples = static_cast<GLuint>(max_samples);')
                print('    }')
            else:
                assert arg.type is glapi.GLsizei
                print('    GLint max_samples = 0;')
                print('    glGetIntegerv(GL_MAX_SAMPLES, &max_samples);')
                print('    if (samples > max_samples) {')
                print('        samples = max_samples;')
                print('    }')

        # These parameters are referred beyond the call life-time
        # TODO: Replace ad-hoc solution for bindable parameters with general one
        if function.name in ('glFeedbackBuffer', 'glSelectBuffer') and arg.output:
            print('    _allocator.bind(%s);' % arg.name)



if __name__ == '__main__':
    print(r'''
#include <string.h>

#include "glproc.hpp"
#include "glretrace.hpp"
#include "glstate.hpp"
#include "glsize.hpp"


static GLint
_getActiveProgram(void);

static void
_validateActiveProgram(trace::Call &call);

''')
    api = stdapi.API()
    api.addModule(glapi.glapi)
    retracer = GlRetracer()
    retracer.retraceApi(api)

    print(r'''
static GLint
_getActiveProgram(void)
{
    GLint program = -1;
    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (currentContext) {
        GLint pipeline = currentContext->currentPipeline;
        if (pipeline) {
            glGetProgramPipelineiv(pipeline, GL_ACTIVE_PROGRAM, &program);
        } else {
            program = currentContext->currentProgram;
            assert(program == _glGetInteger(GL_CURRENT_PROGRAM));
        }
    }
    return program;
}

static void
_validateActiveProgram(trace::Call &call)
{
    assert(retrace::debug > 0);

    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (!currentContext ||
        currentContext->insideList ||
        currentContext->insideBeginEnd ||
        currentContext->wsContext->profile.major < 2) {
        return;
    }

    GLint pipeline = currentContext->currentPipeline;
    if (pipeline) {
        // TODO
    } else {
        GLint program = currentContext->currentProgram;
        assert(program == _glGetInteger(GL_CURRENT_PROGRAM));
        if (!program) {
            return;
        }

        GLint validate_status = GL_FALSE;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);
        if (validate_status) {
            // Validate only once
            return;
        }

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);
        if (!validate_status) {
            retrace::warning(call) << "program validation failed\n";
        }

        GLint info_log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 1) {
             GLchar *infoLog = new GLchar[info_log_length];
             glGetProgramInfoLog(program, info_log_length, NULL, infoLog);
             retrace::warning(call) << infoLog << "\n";
             delete [] infoLog;
        }
    }
}

''')

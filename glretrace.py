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
        #"glMultiDrawElements",
        #"glMultiDrawElementsBaseVertex",
        #"glMultiDrawElementsEXT",
        #"glMultiModeDrawElementsIBM",
    ])

    def retrace_function_body(self, function):
        is_array_pointer = function.name in self.array_pointer_function_names
        is_draw_array = function.name in self.draw_array_function_names
        is_draw_elements = function.name in self.draw_elements_function_names

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

        Retracer.retrace_function_body(self, function)

    def call_function(self, function):
        if function.name == "glViewport":
            print '    bool reshape_window = false;'
            print '    if (x + width > glretrace::window_width) {'
            print '        glretrace::window_width = x + width;'
            print '        reshape_window = true;'
            print '    }'
            print '    if (y + height > glretrace::window_height) {'
            print '        glretrace::window_height = y + height;'
            print '        reshape_window = true;'
            print '    }'
            print '    if (reshape_window) {'
            print '        // XXX: does not always work'
            print '        glretrace::drawable->resize(glretrace::window_width, glretrace::window_height);'
            print '        reshape_window = false;'
            print '    }'

        if function.name == "glEnd":
            print '    glretrace::insideGlBeginEnd = false;'
        
        Retracer.call_function(self, function)

        if function.name == "glBegin":
            print '    glretrace::insideGlBeginEnd = true;'
        else:
            # glGetError is not allowed inside glBegin/glEnd
            print '    glretrace::checkGlError(call.no);'

    def extract_arg(self, function, arg, arg_type, lvalue, rvalue):
        if function.name in self.array_pointer_function_names and arg.name == 'pointer':
            print '    %s = static_cast<%s>(%s.blob());' % (lvalue, arg_type, rvalue)
            return

        if function.name in self.draw_elements_function_names and arg.name == 'indices':
            print '    %s = %s.blob();' % (lvalue, rvalue)
            return

        if function.name.startswith('glUniform') and function.args[0].name == arg.name == 'location':
            print '    GLint program = -1;'
            print '    glGetIntegerv(GL_CURRENT_PROGRAM, &program);'

        Retracer.extract_arg(self, function, arg, arg_type, lvalue, rvalue)


if __name__ == '__main__':
    print r'''
#include <string.h>

#include "glproc.hpp"
#include "glretrace.hpp"


'''
    api = glapi.glapi
    api.add_function(glapi.memcpy)
    retracer = GlRetracer()
    retracer.retrace_api(glapi.glapi)

##########################################################################
#
# Copyright 2008-2010 VMware, Inc.
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


"""GL tracing generator."""


import stdapi
import glapi
from glxapi import glxapi
from trace import Tracer, dump_instance


class TypeGetter(stdapi.Visitor):
    '''Determine which glGet*v function that matches the specified type.'''

    def visit_const(self, const):
        return self.visit(const.type)

    def visit_alias(self, alias):
        if alias.expr == 'GLboolean':
            return 'glGetBooleanv', alias.expr
        elif alias.expr == 'GLdouble':
            return 'glGetDoublev', alias.expr
        elif alias.expr == 'GLfloat':
            return 'glGetFloatv', alias.expr
        elif alias.expr in ('GLint', 'GLsizei'):
            return 'glGetIntegerv', 'GLint'
        else:
            print alias.expr
            assert False
    
    def visit_enum(self, enum):
        return 'glGetIntegerv', 'GLint'

    def visit_bitmask(self, bitmask):
        return 'glGetIntegerv', 'GLint'

    def visit_opaque(self, pointer):
        return 'glGetPointerv', 'GLvoid *'


class GlTracer(Tracer):

    def header(self, api):
        Tracer.header(self, api)
        self.state_tracker_decl(api)

    def footer(self, api):
        Tracer.footer(self, api)
        self.state_tracker_impl(api)

    arrays = [
        ("Vertex", "VERTEX"),
        ("Normal", "NORMAL"),
        ("Color", "COLOR"),
        ("Index", "INDEX"),
        ("TexCoord", "TEXTURE_COORD"),
        ("EdgeFlag", "EDGE_FLAG"),
        ("FogCoord", "FOG_COORD"),
        ("SecondaryColor", "SECONDARY_COLOR"),
    ]

    def state_tracker_decl(self, api):
        # Whether we need user arrays
        print 'static inline bool __need_user_arrays(void)'
        print '{'
        for camelcase_name, uppercase_name in self.arrays:
            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            print '    // %s' % function_name
            print '    {'
            print '        GLboolean __enabled = GL_FALSE;'
            print '        __glGetBooleanv(%s, &__enabled);' % enable_name
            print '        if (__enabled) {'
            print '            GLint __binding = 0;'
            print '            __glGetIntegerv(%s, &__binding);' % binding_name
            print '            if (!__binding) {'
            print '                return true;'
            print '            }'
            print '        }'
            print '    }'
            print
        print '    return false;'
        print '}'
        print

        print 'static void __trace_user_arrays(GLuint maxindex);'
        print
    
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

        #"glVertexPointerEXT",
        #"glNormalPointerEXT",
        #"glColorPointerEXT",
        #"glIndexPointerEXT",
        #"glTexCoordPointerEXT",
        #"glEdgeFlagPointerEXT",
        #"glFogCoordPointerEXT",
        #"glSecondaryColorPointerEXT",

        #"glVertexAttribPointer",
        #"glVertexAttribPointerARB",
        #"glVertexAttribPointerNV",
        #"glVertexAttribLPointer",
        
        #"glMatrixIndexPointerARB",
    ))

    draw_function_names = set((
        'glDrawArrays',
        'glDrawElements',
        'glDrawRangeElements',
    ))

    def trace_function_impl_body(self, function):
        # Defer tracing of user array pointers...
        if function.name in self.array_pointer_function_names:
            print '    GLint __array_buffer = 0;'
            print '    __glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__array_buffer);'
            print '    if (!__array_buffer) {'
            self.dispatch_function(function)
            print '        return;'
            print '    }'

        # ... to the draw calls
        if function.name in self.draw_function_names:
            print '    if (__need_user_arrays()) {'
            arg_names = ', '.join([arg.name for arg in function.args[1:]])
            print '        GLuint maxindex = __%s_maxindex(%s);' % (function.name, arg_names)
            print '        __trace_user_arrays(maxindex);'
            print '    }'
        
        Tracer.trace_function_impl_body(self, function)

    def dump_arg_instance(self, function, arg):
        if function.name in self.draw_function_names and arg.name == 'indices':
            print '    GLint __element_array_buffer = 0;'
            print '    __glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__element_array_buffer);'
            print '    if (!__element_array_buffer) {'
            print '        Trace::LiteralBlob((const void *)%s, count*__gl_type_size(type));' % (arg.name)
            print '    } else {'
            print '        Trace::LiteralOpaque((const void *)%s);' % (arg.name)
            print '    }'
            return

        Tracer.dump_arg_instance(self, function, arg)

    def state_tracker_impl(self, api):
        # A simple state tracker to track the pointer values

        # update the state
        print 'static void __trace_user_arrays(GLuint maxindex)'
        print '{'
        for camelcase_name, uppercase_name in self.arrays:
            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            function = api.get_function_by_name(function_name)

            print '    // %s' % function.name
            print '    {'
            print '        GLboolean __enabled = GL_FALSE;'
            print '        __glGetBooleanv(%s, &__enabled);' % enable_name
            print '        if (__enabled) {'
            print '            GLint __binding = 0;'
            print '            __glGetIntegerv(%s, &__binding);' % binding_name
            print '            if (!__binding) {'

            # Get the arguments via glGet*
            for arg in function.args:
                arg_get_enum = 'GL_%s_ARRAY_%s' % (uppercase_name, arg.name.upper())
                arg_get_function, arg_type = TypeGetter().visit(arg.type)
                print '                %s %s = 0;' % (arg_type, arg.name)
                print '                __%s(%s, &%s);' % (arg_get_function, arg_get_enum, arg.name)
            
            arg_names = ', '.join([arg.name for arg in function.args[:-1]])
            print '                size_t __size = __%s_size(%s, maxindex);' % (function.name, arg_names)

            # Emit a fake function
            print '                unsigned __call = Trace::BeginEnter(__%s_sig);' % (function.name,)
            for arg in function.args:
                assert not arg.output
                print '                Trace::BeginArg(%u);' % (arg.index,)
                if arg.name != 'pointer':
                    dump_instance(arg.type, arg.name)
                else:
                    print '                Trace::LiteralBlob((const void *)%s, __size);' % (arg.name)
                print '                Trace::EndArg();'
            
            print '                Trace::EndEnter();'
            print '                Trace::BeginLeave(__call);'
            print '                Trace::EndLeave();'
            print '            }'
            print '        }'
            print '    }'
            print
        print '}'
        print








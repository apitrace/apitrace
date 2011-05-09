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
import glparams
from glxapi import glxapi
from trace import Tracer, dump_instance


class TypeGetter(stdapi.Visitor):
    '''Determine which glGet*v function that matches the specified type.'''

    def __init__(self, prefix = 'glGet', long_suffix = True):
        self.prefix = prefix
        self.long_suffix = long_suffix

    def visit_const(self, const):
        return self.visit(const.type)

    def visit_alias(self, alias):
        if alias.expr == 'GLboolean':
            if self.long_suffix:
                return self.prefix + 'Booleanv', alias.expr
            else:
                return self.prefix + 'iv', 'GLint'
        elif alias.expr == 'GLdouble':
            if self.long_suffix:
                return self.prefix + 'Doublev', alias.expr
            else:
                return self.prefix + 'dv', alias.expr
        elif alias.expr == 'GLfloat':
            if self.long_suffix:
                return self.prefix + 'Floatv', alias.expr
            else:
                return self.prefix + 'fv', alias.expr
        elif alias.expr in ('GLint', 'GLuint', 'GLsizei'):
            if self.long_suffix:
                return self.prefix + 'Integerv', 'GLint'
            else:
                return self.prefix + 'iv', 'GLint'
        else:
            print alias.expr
            assert False
    
    def visit_enum(self, enum):
        return self.visit(glapi.GLint)

    def visit_bitmask(self, bitmask):
        return self.visit(glapi.GLint)

    def visit_opaque(self, pointer):
        return self.prefix + 'Pointerv', 'GLvoid *'


class GlTracer(Tracer):

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
    arrays.reverse()

    def header(self, api):
        Tracer.header(self, api)

        print '// Whether user arrays were used'
        print 'static bool __user_arrays = false;'
        print
        # Whether we need user arrays
        print 'static inline bool __need_user_arrays(void)'
        print '{'
        print '    if (!__user_arrays) {'
        print '        return false;'
        print '    }'
        print

        for camelcase_name, uppercase_name in self.arrays:
            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            print '    // %s' % function_name
            self.array_prolog(api, uppercase_name)
            print '    if (__glIsEnabled(%s)) {' % enable_name
            print '        GLint __binding = 0;'
            print '        __glGetIntegerv(%s, &__binding);' % binding_name
            print '        if (!__binding) {'
            self.array_cleanup(api, uppercase_name)
            print '            return true;'
            print '        }'
            print '    }'
            self.array_epilog(api, uppercase_name)
            print

        print '    // glVertexAttribPointer'
        print '    GLint __max_vertex_attribs = 0;'
        print '    __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &__max_vertex_attribs);'
        print '    for (GLint index = 0; index < __max_vertex_attribs; ++index) {'
        print '        GLint __enabled = 0;'
        print '        __glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &__enabled);'
        print '        if (__enabled) {'
        print '            GLint __binding = 0;'
        print '            __glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &__binding);'
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

        print 'struct buffer_mapping {'
        print '    void *map;'
        print '    GLint length;'
        print '    bool write;'
        print '};'
        print
        for target in self.buffer_targets:
            print 'struct buffer_mapping __%s_mapping;' % target.lower();
        print
        print 'static inline struct buffer_mapping *'
        print 'get_buffer_mapping(GLenum target) {'
        print '    switch(target) {'
        for target in self.buffer_targets:
            print '    case GL_%s:' % target
            print '        return & __%s_mapping;' % target.lower()
        print '    default:'
        print '        OS::DebugMessage("warning: unknown buffer target 0x%04X\\n", target);'
        print '        return NULL;'
        print '    }'
        print '}'
        print

        # Generate memcpy's signature
        self.trace_function_decl(glapi.memcpy)

        # Generate a helper function to determine whether a parameter name
        # refers to a symbolic value or not
        print 'static bool'
        print 'is_symbolic_pname(GLenum pname) {'
        print '    switch(pname) {'
        for function, type, count, name in glparams.parameters:
            if type is glapi.GLenum:
                print '    case %s:' % name
        print '        return true;'
        print '    default:'
        print '        return false;'
        print '    }'
        print '}'
        print
        
        # Generate a helper function to determine whether a parameter value is
        # potentially symbolic or not; i.e., if the value can be represented in
        # an enum or not
        print 'template<class T>'
        print 'static inline bool'
        print 'is_symbolic_param(T param) {'
        print '    return static_cast<T>(static_cast<GLenum>(param)) == param;'
        print '}'
        print

        # Generate a helper function to know how many elements a parameter has
        print 'static size_t'
        print '__gl_param_size(GLenum pname) {'
        print '    switch(pname) {'
        for function, type, count, name in glparams.parameters:
            if type is not None:
                print '    case %s: return %u;' % (name, count)
        print '    case GL_COMPRESSED_TEXTURE_FORMATS: {'
        print '            GLint num_compressed_texture_formats = 0;'
        print '            __glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num_compressed_texture_formats);'
        print '            return num_compressed_texture_formats;'
        print '        }'
        print '    default:'
        print r'        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);'
        print '        return 1;'
        print '    }'
        print '}'
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

    draw_function_names = set((
        'glDrawArrays',
        'glDrawElements',
        'glDrawRangeElements',
        'glMultiDrawArrays',
        'glMultiDrawElements',
        'glDrawArraysInstanced',
        'glDrawElementsInstanced',
        'glDrawArraysInstancedARB',
        'glDrawElementsInstancedARB',
        'glDrawElementsBaseVertex',
        'glDrawRangeElementsBaseVertex',
        'glDrawElementsInstancedBaseVertex',
        'glMultiDrawElementsBaseVertex',
        'glDrawArraysIndirect',
        'glDrawElementsIndirect',
        'glDrawArraysEXT',
        'glDrawRangeElementsEXT',
        'glDrawRangeElementsEXT_size',
        'glMultiDrawArraysEXT',
        'glMultiDrawElementsEXT',
        'glMultiModeDrawArraysIBM',
        'glMultiModeDrawElementsIBM',
        'glDrawArraysInstancedEXT',
        'glDrawElementsInstancedEXT',
    ))

    interleaved_formats = [
         'GL_V2F',
         'GL_V3F',
         'GL_C4UB_V2F',
         'GL_C4UB_V3F',
         'GL_C3F_V3F',
         'GL_N3F_V3F',
         'GL_C4F_N3F_V3F',
         'GL_T2F_V3F',
         'GL_T4F_V4F',
         'GL_T2F_C4UB_V3F',
         'GL_T2F_C3F_V3F',
         'GL_T2F_N3F_V3F',
         'GL_T2F_C4F_N3F_V3F',
         'GL_T4F_C4F_N3F_V4F',
    ]

    def trace_function_impl_body(self, function):
        # Defer tracing of user array pointers...
        if function.name in self.array_pointer_function_names:
            print '    GLint __array_buffer = 0;'
            print '    __glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__array_buffer);'
            print '    if (!__array_buffer) {'
            print '        __user_arrays = true;'
            self.dispatch_function(function)

            # And also break down glInterleavedArrays into the individual calls
            if function.name == 'glInterleavedArrays':
                print

                # Initialize the enable flags
                for camelcase_name, uppercase_name in self.arrays:
                    flag_name = '__' + uppercase_name.lower()
                    print '        GLboolean %s = GL_FALSE;' % flag_name
                print

                # Switch for the interleaved formats
                print '        switch (format) {'
                for format in self.interleaved_formats:
                    print '            case %s:' % format
                    for camelcase_name, uppercase_name in self.arrays:
                        flag_name = '__' + uppercase_name.lower()
                        if format.find('_' + uppercase_name[0]) >= 0:
                            print '                %s = GL_TRUE;' % flag_name
                    print '                break;'
                print '            default:'
                print '               return;'
                print '        }'
                print

                # Emit fake glEnableClientState/glDisableClientState flags
                for camelcase_name, uppercase_name in self.arrays:
                    flag_name = '__' + uppercase_name.lower()
                    enable_name = 'GL_%s_ARRAY' % uppercase_name

                    # Emit a fake function
                    print '        {'
                    print '            static const Trace::FunctionSig &__sig = %s ? __glEnableClientState_sig : __glDisableClientState_sig;' % flag_name
                    print '            unsigned __call = Trace::BeginEnter(__sig);'
                    print '            Trace::BeginArg(0);'
                    dump_instance(glapi.GLenum, enable_name)
                    print '            Trace::EndArg();'
                    print '            Trace::EndEnter();'
                    print '            Trace::BeginLeave(__call);'
                    print '            Trace::EndLeave();'
                    print '        }'

            print '        return;'
            print '    }'

        # ... to the draw calls
        if function.name in self.draw_function_names:
            print '    if (__need_user_arrays()) {'
            arg_names = ', '.join([arg.name for arg in function.args[1:]])
            print '        GLuint maxindex = __%s_maxindex(%s);' % (function.name, arg_names)
            print '        __trace_user_arrays(maxindex);'
            print '    }'
        
        # Emit a fake memcpy on
        if function.name in ('glUnmapBuffer', 'glUnmapBufferARB'):
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping && mapping->write) {'
            print '        unsigned __call = Trace::BeginEnter(__memcpy_sig);'
            print '        Trace::BeginArg(0);'
            print '        Trace::LiteralOpaque(mapping->map);'
            print '        Trace::EndArg();'
            print '        Trace::BeginArg(1);'
            print '        Trace::LiteralBlob(mapping->map, mapping->length);'
            print '        Trace::EndArg();'
            print '        Trace::BeginArg(2);'
            print '        Trace::LiteralUInt(mapping->length);'
            print '        Trace::EndArg();'
            print '        Trace::EndEnter();'
            print '        Trace::BeginLeave(__call);'
            print '        Trace::EndLeave();'
            print '    }'

        Tracer.trace_function_impl_body(self, function)
       
    buffer_targets = [
        'ARRAY_BUFFER',
        'ELEMENT_ARRAY_BUFFER',
        'PIXEL_PACK_BUFFER',
        'PIXEL_UNPACK_BUFFER',
    ]

    def wrap_ret(self, function, instance):
        Tracer.wrap_ret(self, function, instance)
            
        if function.name in ('glMapBuffer', 'glMapBufferARB'):
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping) {'
            print '        mapping->map = %s;' % (instance)
            print '        mapping->length = 0;'
            print '        __glGetBufferParameteriv(target, GL_BUFFER_SIZE, &mapping->length);'
            print '        mapping->write = (access != GL_READ_ONLY);'
            print '    }'

        if function.name == 'glMapBufferRange':
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping) {'
            print '        mapping->map = %s;' % (instance)
            print '        mapping->length = length;'
            print '        mapping->write = access & GL_MAP_WRITE_BIT;'
            print '    }'

    boolean_names = [
        'GL_FALSE',
        'GL_TRUE',
    ]

    def gl_boolean(self, value):
        return self.boolean_names[int(bool(value))]

    def dump_arg_instance(self, function, arg):
        if function.name in self.draw_function_names and arg.name == 'indices':
            print '    GLint __element_array_buffer = 0;'
            print '    __glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__element_array_buffer);'
            print '    if (!__element_array_buffer) {'
            if isinstance(arg.type, stdapi.Array):
                Tracer.dump_arg_instance(self, function, arg)
                print '        Trace::BeginArray(%s);' % arg.type.length
                print '        for(GLsizei i = 0; i < %s; ++i) {' % arg.type.length
                print '            Trace::BeginElement();'
                print '            Trace::LiteralBlob((const void *)%s, count[i]*__gl_type_size(type));' % (arg.name)
                print '            Trace::EndElement();'
                print '        }'
                print '        Trace::EndArray();'
            else:
                print '        Trace::LiteralBlob((const void *)%s, count*__gl_type_size(type));' % (arg.name)
            print '    } else {'
            Tracer.dump_arg_instance(self, function, arg)
            print '    }'
            return

        # Several GL state functions take GLenum symbolic names as
        # integer/floats; so dump the symbolic name whenever possible
        if arg.type in (glapi.GLint, glapi.GLfloat) and arg.name == 'param':
            assert arg.index > 0
            assert function.args[arg.index - 1].name == 'pname'
            assert function.args[arg.index - 1].type == glapi.GLenum
            print '    if (is_symbolic_pname(pname) && is_symbolic_param(%s)) {' % arg.name
            dump_instance(glapi.GLenum, arg.name)
            print '    } else {'
            Tracer.dump_arg_instance(self, function, arg)
            print '    }'
            return

        Tracer.dump_arg_instance(self, function, arg)

    def footer(self, api):
        Tracer.footer(self, api)

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
            self.array_trace_prolog(api, uppercase_name)
            self.array_prolog(api, uppercase_name)
            print '    if (__glIsEnabled(%s)) {' % enable_name
            print '        GLint __binding = 0;'
            print '        __glGetIntegerv(%s, &__binding);' % binding_name
            print '        if (!__binding) {'

            # Get the arguments via glGet*
            for arg in function.args:
                arg_get_enum = 'GL_%s_ARRAY_%s' % (uppercase_name, arg.name.upper())
                arg_get_function, arg_type = TypeGetter().visit(arg.type)
                print '            %s %s = 0;' % (arg_type, arg.name)
                print '            __%s(%s, &%s);' % (arg_get_function, arg_get_enum, arg.name)
            
            arg_names = ', '.join([arg.name for arg in function.args[:-1]])
            print '            size_t __size = __%s_size(%s, maxindex);' % (function.name, arg_names)

            # Emit a fake function
            self.array_trace_intermezzo(api, uppercase_name)
            print '            unsigned __call = Trace::BeginEnter(__%s_sig);' % (function.name,)
            for arg in function.args:
                assert not arg.output
                print '            Trace::BeginArg(%u);' % (arg.index,)
                if arg.name != 'pointer':
                    dump_instance(arg.type, arg.name)
                else:
                    print '            Trace::LiteralBlob((const void *)%s, __size);' % (arg.name)
                print '            Trace::EndArg();'
            
            print '            Trace::EndEnter();'
            print '            Trace::BeginLeave(__call);'
            print '            Trace::EndLeave();'
            print '        }'
            print '    }'
            self.array_epilog(api, uppercase_name)
            self.array_trace_epilog(api, uppercase_name)
            print

        # Samething, but for glVertexAttribPointer
        print '    // glVertexAttribPointer'
        print '    GLint __max_vertex_attribs = 0;'
        print '    __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &__max_vertex_attribs);'
        print '    for (GLint index = 0; index < __max_vertex_attribs; ++index) {'
        print '        GLint __enabled = 0;'
        print '        __glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &__enabled);'
        print '        if (__enabled) {'
        print '            GLint __binding = 0;'
        print '            __glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &__binding);'
        print '            if (!__binding) {'

        function = api.get_function_by_name('glVertexAttribPointer')

        # Get the arguments via glGet*
        for arg in function.args[1:]:
            arg_get_enum = 'GL_VERTEX_ATTRIB_ARRAY_%s' % (arg.name.upper(),)
            arg_get_function, arg_type = TypeGetter('glGetVertexAttrib', False).visit(arg.type)
            print '                %s %s = 0;' % (arg_type, arg.name)
            print '                __%s(index, %s, &%s);' % (arg_get_function, arg_get_enum, arg.name)
        
        arg_names = ', '.join([arg.name for arg in function.args[1:-1]])
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

    #
    # Hooks for glTexCoordPointer, which is identical to the other array
    # pointers except the fact that it is indexed by glClientActiveTexture.
    #

    def array_prolog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print '    GLint client_active_texture = 0;'
            print '    __glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &client_active_texture);'
            print '    GLint max_texture_coords = 0;'
            print '    __glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);'
            print '    for (GLint unit = 0; unit < max_texture_coords; ++unit) {'
            print '        GLenum texture = GL_TEXTURE0 + unit;'
            print '        __glClientActiveTexture(texture);'

    def array_trace_prolog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print '    bool client_active_texture_dirty = false;'

    def array_epilog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print '    }'
        self.array_cleanup(api, uppercase_name)

    def array_cleanup(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print '    __glClientActiveTexture(client_active_texture);'
        
    def array_trace_intermezzo(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print '    if (texture != client_active_texture || client_active_texture_dirty) {'
            print '        client_active_texture_dirty = true;'
            self.fake_glClientActiveTexture_call(api, "texture");
            print '    }'

    def array_trace_epilog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print '    if (client_active_texture_dirty) {'
            self.fake_glClientActiveTexture_call(api, "client_active_texture");
            print '    }'

    def fake_glClientActiveTexture_call(self, api, texture):
        function = api.get_function_by_name('glClientActiveTexture')
        self.fake_call(function, [texture])

    def fake_call(self, function, args):
        print '            unsigned __fake_call = Trace::BeginEnter(__%s_sig);' % (function.name,)
        for arg, instance in zip(function.args, args):
            assert not arg.output
            print '            Trace::BeginArg(%u);' % (arg.index,)
            dump_instance(arg.type, instance)
            print '            Trace::EndArg();'
        print '            Trace::EndEnter();'
        print '            Trace::BeginLeave(__fake_call);'
        print '            Trace::EndLeave();'












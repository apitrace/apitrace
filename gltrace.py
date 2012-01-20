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


import specs.stdapi as stdapi
import specs.glapi as glapi
import specs.glparams as glparams
from specs.glxapi import glxapi
from trace import Tracer


class TypeGetter(stdapi.Visitor):
    '''Determine which glGet*v function that matches the specified type.'''

    def __init__(self, prefix = 'glGet', long_suffix = True, ext_suffix = ''):
        self.prefix = prefix
        self.long_suffix = long_suffix
        self.ext_suffix = ext_suffix

    def visitConst(self, const):
        return self.visit(const.type)

    def visitAlias(self, alias):
        if alias.expr == 'GLboolean':
            if self.long_suffix:
                suffix = 'Booleanv'
                arg_type = alias.expr
            else:
                suffix = 'iv'
                arg_type = 'GLint'
        elif alias.expr == 'GLdouble':
            if self.long_suffix:
                suffix = 'Doublev'
                arg_type = alias.expr
            else:
                suffix = 'dv'
                arg_type = alias.expr
        elif alias.expr == 'GLfloat':
            if self.long_suffix:
                suffix = 'Floatv'
                arg_type = alias.expr
            else:
                suffix = 'fv'
                arg_type = alias.expr
        elif alias.expr in ('GLint', 'GLuint', 'GLsizei'):
            if self.long_suffix:
                suffix = 'Integerv'
                arg_type = 'GLint'
            else:
                suffix = 'iv'
                arg_type = 'GLint'
        else:
            print alias.expr
            assert False
        function_name = self.prefix + suffix + self.ext_suffix
        return function_name, arg_type
    
    def visitEnum(self, enum):
        return self.visit(glapi.GLint)

    def visitBitmask(self, bitmask):
        return self.visit(glapi.GLint)

    def visitOpaque(self, pointer):
        return self.prefix + 'Pointerv' + self.ext_suffix, 'GLvoid *'


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

    # arrays available in PROFILE_ES1
    arrays_es1 = ("Vertex", "Normal", "Color", "TexCoord")

    def header(self, api):
        Tracer.header(self, api)

        print '#include "gltrace.hpp"'
        print
        print 'enum tracer_context_profile {'
        print '    PROFILE_COMPAT,'
        print '    PROFILE_ES1,'
        print '    PROFILE_ES2,'
        print '};'
        print
        print 'struct tracer_context {'
        print '    enum tracer_context_profile profile;'
        print '    bool user_arrays;'
        print '    bool user_arrays_arb;'
        print '    bool user_arrays_nv;'
        print '};'
        print
        
        # Which glVertexAttrib* variant to use
        print 'enum vertex_attrib {'
        print '    VERTEX_ATTRIB,'
        print '    VERTEX_ATTRIB_ARB,'
        print '    VERTEX_ATTRIB_NV,'
        print '};'
        print
        print 'static tracer_context *__get_context(void)'
        print '{'
        print '    // TODO return the context set by other APIs (GLX, EGL, and etc.)'
        print '    static tracer_context __ctx = { PROFILE_COMPAT, false, false, false };'
        print '    return &__ctx;'
        print '}'
        print
        print 'static vertex_attrib __get_vertex_attrib(void) {'
        print '    tracer_context *ctx = __get_context();'
        print '    if (ctx->user_arrays_arb || ctx->user_arrays_nv) {'
        print '        GLboolean __vertex_program = GL_FALSE;'
        print '        __glGetBooleanv(GL_VERTEX_PROGRAM_ARB, &__vertex_program);'
        print '        if (__vertex_program) {'
        print '            if (ctx->user_arrays_nv) {'
        print '                GLint __vertex_program_binding_nv = 0;'
        print '                __glGetIntegerv(GL_VERTEX_PROGRAM_BINDING_NV, &__vertex_program_binding_nv);'
        print '                if (__vertex_program_binding_nv) {'
        print '                    return VERTEX_ATTRIB_NV;'
        print '                }'
        print '            }'
        print '            return VERTEX_ATTRIB_ARB;'
        print '        }'
        print '    }'
        print '    return VERTEX_ATTRIB;'
        print '}'
        print

        # Whether we need user arrays
        print 'static inline bool __need_user_arrays(void)'
        print '{'
        print '    tracer_context *ctx = __get_context();'
        print '    if (!ctx->user_arrays) {'
        print '        return false;'
        print '    }'
        print

        for camelcase_name, uppercase_name in self.arrays:
            # in which profile is the array available?
            profile_check = 'ctx->profile == PROFILE_COMPAT'
            if camelcase_name in self.arrays_es1:
                profile_check = '(' + profile_check + ' || ctx->profile == PROFILE_ES1)';

            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            print '    // %s' % function_name
            print '  if (%s) {' % profile_check
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
            print '  }'
            print

        print '    // ES1 does not support generic vertex attributes'
        print '    if (ctx->profile == PROFILE_ES1)'
        print '        return false;'
        print
        print '    vertex_attrib __vertex_attrib = __get_vertex_attrib();'
        print
        print '    // glVertexAttribPointer'
        print '    if (__vertex_attrib == VERTEX_ATTRIB) {'
        print '        GLint __max_vertex_attribs = 0;'
        print '        __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &__max_vertex_attribs);'
        print '        for (GLint index = 0; index < __max_vertex_attribs; ++index) {'
        print '            GLint __enabled = 0;'
        print '            __glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &__enabled);'
        print '            if (__enabled) {'
        print '                GLint __binding = 0;'
        print '                __glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &__binding);'
        print '                if (!__binding) {'
        print '                    return true;'
        print '                }'
        print '            }'
        print '        }'
        print '    }'
        print
        print '    // glVertexAttribPointerARB'
        print '    if (__vertex_attrib == VERTEX_ATTRIB_ARB) {'
        print '        GLint __max_vertex_attribs = 0;'
        print '        __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &__max_vertex_attribs);'
        print '        for (GLint index = 0; index < __max_vertex_attribs; ++index) {'
        print '            GLint __enabled = 0;'
        print '            __glGetVertexAttribivARB(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB, &__enabled);'
        print '            if (__enabled) {'
        print '                GLint __binding = 0;'
        print '                __glGetVertexAttribivARB(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB, &__binding);'
        print '                if (!__binding) {'
        print '                    return true;'
        print '                }'
        print '            }'
        print '        }'
        print '    }'
        print
        print '    // glVertexAttribPointerNV'
        print '    if (__vertex_attrib == VERTEX_ATTRIB_NV) {'
        print '        for (GLint index = 0; index < 16; ++index) {'
        print '            GLint __enabled = 0;'
        print '            __glGetIntegerv(GL_VERTEX_ATTRIB_ARRAY0_NV + index, &__enabled);'
        print '            if (__enabled) {'
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
        print '    bool explicit_flush;'
        print '};'
        print
        for target in self.buffer_targets:
            print 'struct buffer_mapping __%s_mapping;' % target.lower();
        print
        print 'static inline struct buffer_mapping *'
        print 'get_buffer_mapping(GLenum target) {'
        print '    switch (target) {'
        for target in self.buffer_targets:
            print '    case GL_%s:' % target
            print '        return & __%s_mapping;' % target.lower()
        print '    default:'
        print '        os::log("apitrace: warning: unknown buffer target 0x%04X\\n", target);'
        print '        return NULL;'
        print '    }'
        print '}'
        print

        # Generate a helper function to determine whether a parameter name
        # refers to a symbolic value or not
        print 'static bool'
        print 'is_symbolic_pname(GLenum pname) {'
        print '    switch (pname) {'
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
        print '    switch (pname) {'
        for function, type, count, name in glparams.parameters:
            if type is not None:
                print '    case %s: return %u;' % (name, count)
        print '    case GL_COMPRESSED_TEXTURE_FORMATS: {'
        print '            GLint num_compressed_texture_formats = 0;'
        print '            __glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num_compressed_texture_formats);'
        print '            return num_compressed_texture_formats;'
        print '        }'
        print '    default:'
        print r'        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);'
        print '        return 1;'
        print '    }'
        print '}'
        print

        # states such as GL_UNPACK_ROW_LENGTH are not available in GLES
        print 'static inline bool'
        print 'can_unpack_subimage(void) {'
        print '    tracer_context *ctx = __get_context();'
        print '    return (ctx->profile == PROFILE_COMPAT);'
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
        "glDrawArraysInstancedBaseInstance",
        'glDrawElementsInstanced',
        'glDrawArraysInstancedARB',
        'glDrawElementsInstancedARB',
        'glDrawElementsBaseVertex',
        'glDrawRangeElementsBaseVertex',
        'glDrawElementsInstancedBaseVertex',
        "glDrawElementsInstancedBaseInstance",
        "glDrawElementsInstancedBaseVertexBaseInstance",
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

    def traceFunctionImplBody(self, function):
        # Defer tracing of user array pointers...
        if function.name in self.array_pointer_function_names:
            print '    GLint __array_buffer = 0;'
            print '    __glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__array_buffer);'
            print '    if (!__array_buffer) {'
            print '        tracer_context *ctx = __get_context();'
            print '        ctx->user_arrays = true;'
            if function.name == "glVertexAttribPointerARB":
                print '        ctx->user_arrays_arb = true;'
            if function.name == "glVertexAttribPointerNV":
                print '        ctx->user_arrays_nv = true;'
            self.invokeFunction(function)

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
                    print '            static const trace::FunctionSig &__sig = %s ? __glEnableClientState_sig : __glDisableClientState_sig;' % flag_name
                    print '            unsigned __call = trace::localWriter.beginEnter(&__sig);'
                    print '            trace::localWriter.beginArg(0);'
                    self.serializeValue(glapi.GLenum, enable_name)
                    print '            trace::localWriter.endArg();'
                    print '            trace::localWriter.endEnter();'
                    print '            trace::localWriter.beginLeave(__call);'
                    print '            trace::localWriter.endLeave();'
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
        
        # Emit a fake memcpy on buffer uploads
        if function.name in ('glUnmapBuffer', 'glUnmapBufferARB', 'glUnmapBufferOES'):
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping && mapping->write && !mapping->explicit_flush) {'
            self.emit_memcpy('mapping->map', 'mapping->map', 'mapping->length')
            print '    }'
        if function.name == 'glUnmapNamedBufferEXT':
            print '    GLint access = 0;'
            print '    glGetNamedBufferParameterivEXT(buffer, GL_BUFFER_ACCESS, &access);'
            print '    if ((access & GL_MAP_WRITE_BIT) & !(access & GL_MAP_FLUSH_EXPLICIT_BIT)) {'
            print '        GLvoid *map = NULL;'
            print '        glGetNamedBufferPointervEXT(buffer, GL_BUFFER_MAP_POINTER, &map);'
            print '        GLint length = 0;'
            print '        glGetNamedBufferParameterivEXT(buffer, GL_BUFFER_MAP_LENGTH, &length);'
            self.emit_memcpy('map', 'map', 'length')
            print '    }'
        if function.name in ('glFlushMappedBufferRange', 'glFlushMappedBufferRangeAPPLE'):
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping) {'
            if function.name.endswith('APPLE'):
                 print '        GLsizeiptr length = size;'
                 print '        mapping->explicit_flush = true;'
            print '        //assert(offset + length <= mapping->length);'
            self.emit_memcpy('(char *)mapping->map + offset', '(const char *)mapping->map + offset', 'length')
            print '    }'
        if function.name == 'glFlushMappedNamedBufferRangeEXT':
            print '    GLvoid *map = NULL;'
            print '    glGetNamedBufferPointervEXT(buffer, GL_BUFFER_MAP_POINTER, &map);'
            print '    if (map) {'
            self.emit_memcpy('(char *)map + offset', '(const char *)map + offset', 'length')
            print '    }'

        # Don't leave vertex attrib locations to chance.  Instead emit fake
        # glBindAttribLocation calls to ensure that the same locations will be
        # used when retracing.  Trying to remap locations after the fact would
        # be an herculian task given that vertex attrib locations appear in
        # many entry-points, including non-shader related ones.
        if function.name == 'glLinkProgram':
            Tracer.invokeFunction(self, function)
            print '    GLint active_attributes = 0;'
            print '    __glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &active_attributes);'
            print '    for (GLint attrib = 0; attrib < active_attributes; ++attrib) {'
            print '        GLint size = 0;'
            print '        GLenum type = 0;'
            print '        GLchar name[256];'
            # TODO: Use ACTIVE_ATTRIBUTE_MAX_LENGTH instead of 256
            print '        __glGetActiveAttrib(program, attrib, sizeof name, NULL, &size, &type, name);'
            print "        if (name[0] != 'g' || name[1] != 'l' || name[2] != '_') {"
            print '            GLint location = __glGetAttribLocation(program, name);'
            print '            if (location >= 0) {'
            bind_function = glapi.glapi.get_function_by_name('glBindAttribLocation')
            self.fake_call(bind_function, ['program', 'location', 'name'])
            print '            }'
            print '        }'
            print '    }'
        if function.name == 'glLinkProgramARB':
            Tracer.invokeFunction(self, function)
            print '    GLint active_attributes = 0;'
            print '    __glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &active_attributes);'
            print '    for (GLint attrib = 0; attrib < active_attributes; ++attrib) {'
            print '        GLint size = 0;'
            print '        GLenum type = 0;'
            print '        GLcharARB name[256];'
            # TODO: Use ACTIVE_ATTRIBUTE_MAX_LENGTH instead of 256
            print '        __glGetActiveAttribARB(programObj, attrib, sizeof name, NULL, &size, &type, name);'
            print "        if (name[0] != 'g' || name[1] != 'l' || name[2] != '_') {"
            print '            GLint location = __glGetAttribLocationARB(programObj, name);'
            print '            if (location >= 0) {'
            bind_function = glapi.glapi.get_function_by_name('glBindAttribLocationARB')
            self.fake_call(bind_function, ['programObj', 'location', 'name'])
            print '            }'
            print '        }'
            print '    }'

        Tracer.traceFunctionImplBody(self, function)

    gremedy_functions = [
        'glStringMarkerGREMEDY',
        'glFrameTerminatorGREMEDY',
    ]

    def invokeFunction(self, function):
        if function.name in ('glLinkProgram', 'glLinkProgramARB'):
            # These functions have been dispatched already
            return

        # We implement the GREMEDY extensions, not the driver
        if function.name in self.gremedy_functions:
            return

        if function.name in ('glXGetProcAddress', 'glXGetProcAddressARB', 'wglGetProcAddress'):
            if_ = 'if'
            for gremedy_function in self.gremedy_functions:
                print '    %s (strcmp("%s", (const char *)%s) == 0) {' % (if_, gremedy_function, function.args[0].name)
                print '        __result = (%s)&%s;' % (function.type, gremedy_function)
                print '    }'
                if_ = 'else if'
            print '    else {'
            Tracer.invokeFunction(self, function)
            print '    }'
            return

        # Override GL extensions
        if function.name in ('glGetString', 'glGetIntegerv', 'glGetStringi'):
            Tracer.invokeFunction(self, function, prefix = 'gltrace::__', suffix = '_override')
            return

        Tracer.invokeFunction(self, function)

    buffer_targets = [
        'ARRAY_BUFFER',
        'ELEMENT_ARRAY_BUFFER',
        'PIXEL_PACK_BUFFER',
        'PIXEL_UNPACK_BUFFER',
        'UNIFORM_BUFFER',
        'TEXTURE_BUFFER',
        'TRANSFORM_FEEDBACK_BUFFER',
        'COPY_READ_BUFFER',
        'COPY_WRITE_BUFFER',
        'DRAW_INDIRECT_BUFFER',
        'ATOMIC_COUNTER_BUFFER',
    ]

    def wrapRet(self, function, instance):
        Tracer.wrapRet(self, function, instance)

            
        if function.name in ('glMapBuffer', 'glMapBufferARB', 'glMapBufferOES'):
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping) {'
            print '        mapping->map = %s;' % (instance)
            print '        mapping->length = 0;'
            print '        __glGetBufferParameteriv(target, GL_BUFFER_SIZE, &mapping->length);'
            print '        mapping->write = (access != GL_READ_ONLY);'
            print '        mapping->explicit_flush = false;'
            print '    }'

        if function.name == 'glMapBufferRange':
            print '    struct buffer_mapping *mapping = get_buffer_mapping(target);'
            print '    if (mapping) {'
            print '        mapping->map = %s;' % (instance)
            print '        mapping->length = length;'
            print '        mapping->write = access & GL_MAP_WRITE_BIT;'
            print '        mapping->explicit_flush = access & GL_MAP_FLUSH_EXPLICIT_BIT;'
            print '    }'

    boolean_names = [
        'GL_FALSE',
        'GL_TRUE',
    ]

    def gl_boolean(self, value):
        return self.boolean_names[int(bool(value))]

    # Names of the functions that unpack from a pixel buffer object.  See the
    # ARB_pixel_buffer_object specification.
    unpack_function_names = set([
        'glBitmap',
        'glColorSubTable',
        'glColorTable',
        'glCompressedTexImage1D',
        'glCompressedTexImage2D',
        'glCompressedTexImage3D',
        'glCompressedTexSubImage1D',
        'glCompressedTexSubImage2D',
        'glCompressedTexSubImage3D',
        'glConvolutionFilter1D',
        'glConvolutionFilter2D',
        'glDrawPixels',
        'glMultiTexImage1DEXT',
        'glMultiTexImage2DEXT',
        'glMultiTexImage3DEXT',
        'glMultiTexSubImage1DEXT',
        'glMultiTexSubImage2DEXT',
        'glMultiTexSubImage3DEXT',
        'glPixelMapfv',
        'glPixelMapuiv',
        'glPixelMapusv',
        'glPolygonStipple',
        'glSeparableFilter2D',
        'glTexImage1D',
        'glTexImage1DEXT',
        'glTexImage2D',
        'glTexImage2DEXT',
        'glTexImage3D',
        'glTexImage3DEXT',
        'glTexSubImage1D',
        'glTexSubImage1DEXT',
        'glTexSubImage2D',
        'glTexSubImage2DEXT',
        'glTexSubImage3D',
        'glTexSubImage3DEXT',
        'glTextureImage1DEXT',
        'glTextureImage2DEXT',
        'glTextureImage3DEXT',
        'glTextureSubImage1DEXT',
        'glTextureSubImage2DEXT',
        'glTextureSubImage3DEXT',
    ])

    def serializeArgValue(self, function, arg):
        if function.name in self.draw_function_names and arg.name == 'indices':
            print '    GLint __element_array_buffer = 0;'
            print '    __glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__element_array_buffer);'
            print '    if (!__element_array_buffer) {'
            if isinstance(arg.type, stdapi.Array):
                print '        trace::localWriter.beginArray(%s);' % arg.type.length
                print '        for(GLsizei i = 0; i < %s; ++i) {' % arg.type.length
                print '            trace::localWriter.beginElement();'
                print '            trace::localWriter.writeBlob(%s[i], count[i]*__gl_type_size(type));' % (arg.name)
                print '            trace::localWriter.endElement();'
                print '        }'
                print '        trace::localWriter.endArray();'
            else:
                print '        trace::localWriter.writeBlob(%s, count*__gl_type_size(type));' % (arg.name)
            print '    } else {'
            Tracer.serializeArgValue(self, function, arg)
            print '    }'
            return

        # Recognize offsets instead of blobs when a PBO is bound
        if function.name in self.unpack_function_names \
           and (isinstance(arg.type, stdapi.Blob) \
                or (isinstance(arg.type, stdapi.Const) \
                    and isinstance(arg.type.type, stdapi.Blob))):
            print '    {'
            print '        tracer_context *ctx = __get_context();'
            print '        GLint __unpack_buffer = 0;'
            print '        if (ctx->profile == PROFILE_COMPAT)'
            print '            __glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &__unpack_buffer);'
            print '        if (__unpack_buffer) {'
            print '            trace::localWriter.writeOpaque(%s);' % arg.name
            print '        } else {'
            Tracer.serializeArgValue(self, function, arg)
            print '        }'
            print '    }'
            return

        # Several GL state functions take GLenum symbolic names as
        # integer/floats; so dump the symbolic name whenever possible
        if function.name.startswith('gl') \
           and arg.type in (glapi.GLint, glapi.GLfloat, glapi.GLdouble) \
           and arg.name == 'param':
            assert arg.index > 0
            assert function.args[arg.index - 1].name == 'pname'
            assert function.args[arg.index - 1].type == glapi.GLenum
            print '    if (is_symbolic_pname(pname) && is_symbolic_param(%s)) {' % arg.name
            self.serializeValue(glapi.GLenum, arg.name)
            print '    } else {'
            Tracer.serializeArgValue(self, function, arg)
            print '    }'
            return

        Tracer.serializeArgValue(self, function, arg)

    def footer(self, api):
        Tracer.footer(self, api)

        # A simple state tracker to track the pointer values
        # update the state
        print 'static void __trace_user_arrays(GLuint maxindex)'
        print '{'
        print '    tracer_context *ctx = __get_context();'

        for camelcase_name, uppercase_name in self.arrays:
            # in which profile is the array available?
            profile_check = 'ctx->profile == PROFILE_COMPAT'
            if camelcase_name in self.arrays_es1:
                profile_check = '(' + profile_check + ' || ctx->profile == PROFILE_ES1)';

            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            function = api.get_function_by_name(function_name)

            print '    // %s' % function.prototype()
            print '  if (%s) {' % profile_check
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
            print '            unsigned __call = trace::localWriter.beginEnter(&__%s_sig);' % (function.name,)
            for arg in function.args:
                assert not arg.output
                print '            trace::localWriter.beginArg(%u);' % (arg.index,)
                if arg.name != 'pointer':
                    self.serializeValue(arg.type, arg.name)
                else:
                    print '            trace::localWriter.writeBlob((const void *)%s, __size);' % (arg.name)
                print '            trace::localWriter.endArg();'
            
            print '            trace::localWriter.endEnter();'
            print '            trace::localWriter.beginLeave(__call);'
            print '            trace::localWriter.endLeave();'
            print '        }'
            print '    }'
            self.array_epilog(api, uppercase_name)
            self.array_trace_epilog(api, uppercase_name)
            print '  }'
            print

        # Samething, but for glVertexAttribPointer*
        #
        # Some variants of glVertexAttribPointer alias conventional and generic attributes:
        # - glVertexAttribPointer: no
        # - glVertexAttribPointerARB: implementation dependent
        # - glVertexAttribPointerNV: yes
        #
        # This means that the implementations of these functions do not always
        # alias, and they need to be considered independently.
        #
        print '    // ES1 does not support generic vertex attributes'
        print '    if (ctx->profile == PROFILE_ES1)'
        print '        return;'
        print
        print '    vertex_attrib __vertex_attrib = __get_vertex_attrib();'
        print
        for suffix in ['', 'ARB', 'NV']:
            if suffix:
                SUFFIX = '_' + suffix
            else:
                SUFFIX = suffix
            function_name = 'glVertexAttribPointer' + suffix
            function = api.get_function_by_name(function_name)

            print '    // %s' % function.prototype()
            print '    if (__vertex_attrib == VERTEX_ATTRIB%s) {' % SUFFIX
            if suffix == 'NV':
                print '        GLint __max_vertex_attribs = 16;'
            else:
                print '        GLint __max_vertex_attribs = 0;'
                print '        __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &__max_vertex_attribs);'
            print '        for (GLint index = 0; index < __max_vertex_attribs; ++index) {'
            print '            GLint __enabled = 0;'
            if suffix == 'NV':
                print '            __glGetIntegerv(GL_VERTEX_ATTRIB_ARRAY0_NV + index, &__enabled);'
            else:
                print '            __glGetVertexAttribiv%s(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED%s, &__enabled);' % (suffix, SUFFIX)
            print '            if (__enabled) {'
            print '                GLint __binding = 0;'
            if suffix != 'NV':
                # It doesn't seem possible to use VBOs with NV_vertex_program.
                print '                __glGetVertexAttribiv%s(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING%s, &__binding);' % (suffix, SUFFIX)
            print '                if (!__binding) {'

            # Get the arguments via glGet*
            for arg in function.args[1:]:
                if suffix == 'NV':
                    arg_get_enum = 'GL_ATTRIB_ARRAY_%s%s' % (arg.name.upper(), SUFFIX)
                else:
                    arg_get_enum = 'GL_VERTEX_ATTRIB_ARRAY_%s%s' % (arg.name.upper(), SUFFIX)
                arg_get_function, arg_type = TypeGetter('glGetVertexAttrib', False, suffix).visit(arg.type)
                print '                    %s %s = 0;' % (arg_type, arg.name)
                print '                    __%s(index, %s, &%s);' % (arg_get_function, arg_get_enum, arg.name)
            
            arg_names = ', '.join([arg.name for arg in function.args[1:-1]])
            print '                    size_t __size = __%s_size(%s, maxindex);' % (function.name, arg_names)

            # Emit a fake function
            print '                    unsigned __call = trace::localWriter.beginEnter(&__%s_sig);' % (function.name,)
            for arg in function.args:
                assert not arg.output
                print '                    trace::localWriter.beginArg(%u);' % (arg.index,)
                if arg.name != 'pointer':
                    self.serializeValue(arg.type, arg.name)
                else:
                    print '                    trace::localWriter.writeBlob((const void *)%s, __size);' % (arg.name)
                print '                    trace::localWriter.endArg();'
            
            print '                    trace::localWriter.endEnter();'
            print '                    trace::localWriter.beginLeave(__call);'
            print '                    trace::localWriter.endLeave();'
            print '                }'
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
            print '    if (ctx->profile == PROFILE_COMPAT)'
            print '        __glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);'
            print '    else'
            print '        __glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_coords);'
            print '    for (GLint unit = 0; unit < max_texture_coords; ++unit) {'
            print '        GLint texture = GL_TEXTURE0 + unit;'
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
        print '            unsigned __fake_call = trace::localWriter.beginEnter(&__%s_sig);' % (function.name,)
        for arg, instance in zip(function.args, args):
            assert not arg.output
            print '            trace::localWriter.beginArg(%u);' % (arg.index,)
            self.serializeValue(arg.type, instance)
            print '            trace::localWriter.endArg();'
        print '            trace::localWriter.endEnter();'
        print '            trace::localWriter.beginLeave(__fake_call);'
        print '            trace::localWriter.endLeave();'












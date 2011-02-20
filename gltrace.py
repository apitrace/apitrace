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


from glxapi import glxapi
from trace import Tracer, dump_instance


class GlTracer(Tracer):

    def header(self, api):
        Tracer.header(self, api)
        self.state_tracker_decl(api)

    def footer(self, api):
        Tracer.footer(self, api)
        self.state_tracker_impl(api)

    array_names = {
        "VERTEX": "glVertexPointer",
        "NORMAL": "glNormalPointer",
        "COLOR": "glColorPointer",
        "INDEX": "glIndexPointer",
        "TEX_COORD": "glTexCoordPointer",
        "EDGE_FLAG": "glEdgeFlagPointer",
        "FOG_COORD": "glFogCoordPointer",
        "SECONDARY_COLOR": "glSecondaryColorPointer",
    }

    pointer_function_names = {
        "glVertexPointer": "VERTEX",
        "glNormalPointer": "NORMAL",
        "glColorPointer": "COLOR",
        "glIndexPointer": "INDEX",
        "glTexCoordPointer": "TEX_COORD",
        "glEdgeFlagPointer": "EDGE_FLAG",
        "glFogCoordPointer": "FOG_COORD",
        "glSecondaryColorPointer": "SECONDARY_COLOR",
        #"glInterleavedArrays": ("InterleavedArrays", None)
        #"glVertexPointerEXT": "VERTEX",
        #"glNormalPointerEXT": "NORMAL",
        #"glColorPointerEXT": "COLOR",
        #"glIndexPointerEXT": "INDEX",
        #"glTexCoordPointerEXT": "TEX_COORD",
        #"glEdgeFlagPointerEXT": "EDGE_FLAG",
        #"glFogCoordPointerEXT": "FOG_COORD",
        #"glSecondaryColorPointerEXT": "SECONDARY_COLOR",

        #"glVertexAttribPointer": "VertexAttribPointer",
        #"glVertexAttribPointerARB": "VertexAttribPointer",
        #"glVertexAttribPointerNV": "VertexAttribPointer",
        #"glVertexAttribLPointer": "VertexAttribLPointer",
        
        #"glMatrixIndexPointerARB": "MatrixIndexPointer",
    }

    bind_buffer_enums = [
        'ARRAY_BUFFER',
        'ELEMENT_ARRAY_BUFFER',
        'PIXEL_PACK_BUFFER',
        'PIXEL_UNPACK_BUFFER',
    ]

    client_state_enums = [
         'COLOR_ARRAY',
         'EDGE_FLAG_ARRAY', 
         'FOG_COORD_ARRAY', 
         'INDEX_ARRAY', 
         'NORMAL_ARRAY', 
         'SECONDARY_COLOR_ARRAY', 
         'TEXTURE_COORD_ARRAY', 
         'VERTEX_ARRAY',
    ]

    def state_tracker_decl(self, api):
        # A simple state tracker to track the pointer values

        # define the NEW_XXXX dirty flags
        value = 1
        for array_name in self.array_names.iterkeys():
            dirtyflag = "NEW_%s" % array_name.upper()
            print '#define %s 0x%x' % (dirtyflag, value)
            value <<= 1
        print

        # declare the state structure
        print 'struct {'
        for enum in self.bind_buffer_enums:
            print '    GLuint %s;' % (enum.lower(),)
        for enum in self.client_state_enums:
            print '    GLboolean %s;' % (enum.lower(),)
        for array_name, function_name in self.array_names.iteritems():
            function = api.get_function_by_name(function_name)
            print '    struct {'
            for arg in function.args:
                print '        %s %s;' % (arg.type, arg.name)
            print '    } %s;' % array_name.lower()
        print '    unsigned dirty;'
        print '} __state;'
        print
        print 'static void __state_update(GLsizei maxIndex);'
        print
    
    def trace_function_impl_body(self, function):
        # Track bound VBOs
        if function.name in ('glBindBuffer', 'glBindBufferARB'):
            print '    switch(%s) {' % function.args[0].name
            for enum in self.bind_buffer_enums:
                print '    case GL_%s:' % enum
                print '        __state.%s = %s;' % (enum.lower(), function.args[1].name)
                print '        break;'
            print '    }'

        # Track enabled arrays
        if function.name == 'glEnableClientState':
            print '    switch(%s) {' % function.args[0].name
            for enum in self.client_state_enums:
                print '    case GL_%s:' % enum
                print '        __state.%s = GL_TRUE;' % (enum.lower(),)
                print '        break;'
            print '    }'
        if function.name == 'glDisableClientState':
            print '    switch(%s) {' % function.args[0].name
            for enum in self.client_state_enums:
                print '    case GL_%s:' % enum
                print '        __state.%s = GL_FALSE;' % (enum.lower(),)
                print '        break;'
            print '    }'

        # Track array pointers
        if function.name in self.pointer_function_names:
            array_name = self.pointer_function_names[function.name]
            dirtyflag = "NEW_%s" % array_name.upper()
            for arg in function.args:
                assert not arg.output
                print '    __state.%s.%s = %s;' % (array_name.lower(), arg.name, arg.name)
            print '    __state.dirty |= %s; ' % dirtyflag

            # Defer tracing
            self.dispatch_function(function)
            return

        if function.name == 'glDrawArrays':
            print '   __state_update(first + count - 1);'
        
        Tracer.trace_function_impl_body(self, function)

    def state_tracker_impl(self, api):
        # A simple state tracker to track the pointer values

        # update the state
        print 'static void __state_update(GLsizei maxIndex)'
        print '{'
        print '    GLint __array_buffer = 0;'
        print '    __glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__array_buffer);'
        for array_name, function_name in self.array_names.iteritems():
            function = api.get_function_by_name(function_name)
            dirtyflag = "NEW_%s" % array_name.upper()
            if array_name == 'TEX_COORD':
                enableflag = 'TEXTURE_COORD_ARRAY'.lower()
            else:
                enableflag = '%s_array' % array_name.lower()
            print '    if (__state.%s && (__state.dirty & %s)) {' % (enableflag, dirtyflag)
            print '        unsigned __call = Trace::BeginEnter(__%s_sig);' % (function.name,)
            for arg in function.args:
                assert not arg.output
                value = '__state.%s.%s' % (array_name.lower(), arg.name)
                print '        Trace::BeginArg(%u);' % (arg.index,)
                if arg.name != 'pointer':
                    dump_instance(arg.type, value)
                else:
                    print '        if (__state.array_buffer) {'
                    print '            Trace::LiteralOpaque((const void *)%s);' % value
                    print '            __state.dirty &= ~%s;' % dirtyflag
                    print '        } else {'
                    if array_name in ('INDEX', 'EDGE_FLAG', 'FOG_COORD'):
                        size = '1'
                    elif array_name == 'NORMAL':
                        size = '3'
                    else:
                        size = '__state.%s.size' % array_name.lower()
                    if array_name == 'EDGE_FLAG':
                        type = 'GL_BOOL'
                    else:
                        type = '__state.%s.type' % array_name.lower()
                    stride = '__state.%s.stride' % array_name.lower()
                    print '            Trace::LiteralBlob((const void *)%s, __glArrayPointer_size(%s, %s, %s, maxIndex));' % (value, size, type, stride)
                    print '        }'
                    print '        Trace::EndArg();'
            print '        Trace::EndEnter();'
            print '        Trace::BeginLeave(__call);'
            print '        Trace::EndLeave();'
            print '    }'
        print '}'
        print








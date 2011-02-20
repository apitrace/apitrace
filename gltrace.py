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

    pointer_function_names = {
        "glVertexPointer": "VertexPointer",
        "glVertexPointerEXT": "VertexPointer",
        "glNormalPointer": "NormalPointer",
        "glNormalPointerEXT": "NormalPointer",
        "glColorPointer": "ColorPointer",
        "glColorPointerEXT": "ColorPointer",
        "glIndexPointer": "IndexPointer",
        "glIndexPointerEXT": "IndexPointer",
        "glTexCoordPointer": "TexCoordPointer",
        "glTexCoordPointerEXT": "TexCoordPointer",
        "glEdgeFlagPointer": "EdgeFlagPointer",
        "glEdgeFlagPointerEXT": "EdgeFlagPointer",
        "glFogCoordPointer": "FogCoordPointer",
        "glFogCoordPointerEXT": "FogCoordPointer",
        "glSecondaryColorPointer": "SecondaryColorPointer",
        "glSecondaryColorPointerEXT": "SecondaryColorPointer",
        "glInterleavedArrays": "InterleavedArrays",

        #"glVertexAttribPointer": "VertexAttribPointer",
        #"glVertexAttribPointerARB": "VertexAttribPointer",
        #"glVertexAttribPointerNV": "VertexAttribPointer",
        #"glVertexAttribLPointer": "VertexAttribLPointer",
        
        #"glMatrixIndexPointerARB": "MatrixIndexPointer",
    }

    def header(self, api):
        Tracer.header(self, api)
        self.state_tracker_decl(api)

    def footer(self, api):
        Tracer.footer(self, api)
        self.state_tracker_impl(api)

    def state_tracker_decl(self, api):
        # A simple state tracker to track the pointer values

        atoms = list(set(self.pointer_function_names.itervalues()))
        atoms.sort()

        # define the NEW_XXXX dirty flags
        for i in range(len(atoms)):
            atom = atoms[i]
            dirtyflag = "NEW_%s" % atom.upper()
            print '#define %s 0x%x' % (dirtyflag, 1 << i)
        print

        # declare the state structure
        print 'struct {'
        for atom in atoms:
            function = api.get_function_by_name('gl%s' % atom)
            print '    struct {'
            for arg in function.args:
                print '        %s %s;' % (arg.type, arg.name)
            print '    } %s;' % atom
        print '    unsigned dirty;'
        print '} __state;'
        print

    def state_tracker_impl(self, api):
        # A simple state tracker to track the pointer values

        atoms = list(set(self.pointer_function_names.itervalues()))
        atoms.sort()

        # update the state
        print 'void __state_update(GLsizei )'
        print '{'
        print '    GLint __array_buffer = 0;'
        print '    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__array_buffer);'
        for atom in atoms:
            function = api.get_function_by_name('gl%s' % atom)
            dirtyflag = "NEW_%s" % atom.upper()
            print '    if (__state.dirty & %s) {' % dirtyflag
            print '        unsigned __call = Trace::BeginEnter(__%s_sig);' % (function.name,)
            for arg in function.args:
                assert not arg.output
                print '        Trace::BeginArg(%u);' % (arg.index,)
                dump_instance(arg.type, '__state.%s.%s' % (atom, arg.name))
                print '        Trace::EndArg();'
            print '        Trace::EndEnter();'
            print '        Trace::BeginLeave(__call);'
            print '        Trace::EndLeave();'
            print '    }'
        print '}'
        print








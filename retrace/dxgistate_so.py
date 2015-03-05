##########################################################################
#
# Copyright 2008-2015 VMware, Inc.
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

"""Common  code generation."""


# Adjust path
import os.path
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import itertools

from specs import stdapi
from specs import dxgi
from specs import d3d10
from specs import d3d10_1
from specs import d3d11
from specs import d3d11


class ValueDumper(stdapi.Visitor, stdapi.ExpanderMixin):
    '''Visitor which generates code to dump any type.
    '''

    # TODO: Don't dump all types inline -- do like trace.py's ComplexValueDumper


    def visitLiteral(self, literal, instance):
        assert literal.kind in ('SInt', 'UInt')
        print '    json.writeInt(%s);' % (instance)

    def visitString(self, string, instance):
        if not string.wide:
            cast = 'const char *'
            suffix = 'String'
        else:
            assert False
            cast = 'const wchar_t *'
            suffix = 'WString'
        if cast != string.expr:
            # reinterpret_cast is necessary for GLubyte * <=> char *
            instance = 'reinterpret_cast<%s>(%s)' % (cast, instance)
        assert string.length is None
        print '    json.writeString(%s%s);' % (suffix, instance)

    def visitConst(self, const, instance):
        self.visit(const.type, instance)

    def visitStruct(self, struct, instance):
        print '    json.beginObject();'
        for member in struct.members:
            memberType, memberName = member
            print '    json.beginMember("%s");' % memberName
            self.visitMember(member, instance)
            print '    json.endMember(); // %s' % memberName
        print '    json.endObject();'

    def visitArray(self, array, instance):
        length = '_c' + array.type.tag
        index = '_i' + array.type.tag
        array_length = self.expand(array.length)
        print '    if (%s) {' % instance
        print '        size_t %s = %s > 0 ? %s : 0;' % (length, array_length, array_length)
        print '        json.beginArray();'
        print '        for (size_t %s = 0; %s < %s; ++%s) {' % (index, index, length, index)
        self.visitElement(index, array.type, '(%s)[%s]' % (instance, index))
        print '        }'
        print '        json.endArray();'
        print '    } else {'
        print '        json.writeNull();'
        print '    }'

    def visitAttribArray(self, array, instance):
        raise NotImplementedError

    def visitBlob(self, blob, instance):
        raise NotImplementedError

    def visitEnum(self, enum, instance):
        print '    switch (%s) {' % instance
        for value in enum.values:
            print '    case %s:' % value
            print '        json.writeString("%s");' % value
            print '        break;'
        print '    default:'
        print '        json.writeInt(%s);' % instance
        print '        break;'
        print '    }'

    def visitBitmask(self, bitmask, instance):
        # TODO
        self.visit(bitmask.type, instance)

    def visitPointer(self, pointer, instance):
        print '    if (%s) {' % instance
        print '        json.beginArray();'
        self.visit(pointer.type, "*" + instance)
        print '        json.endArray();'
        print '    } else {'
        print '        json.writeNull();'
        print '    }'

    def visitIntPointer(self, pointer, instance):
        print '    json.writeInt((uintptr_t)%s);' % instance

    def visitObjPointer(self, pointer, instance):
        print '    json.writeInt((uintptr_t)%s);' % instance

    def visitLinearPointer(self, pointer, instance):
        print '    json.writeInt((uintptr_t)%s);' % instance

    def visitReference(self, reference, instance):
        self.visit(reference.type, instance)

    def visitHandle(self, handle, instance):
        self.visit(handle.type, instance)

    def visitAlias(self, alias, instance):
        self.visit(alias.type, instance)

    def visitOpaque(self, opaque, instance):
        print '    json.writeInt((uintptr_t)%s);' % instance

    def visitInterface(self, interface, instance):
        assert False

    def visitPolymorphic(self, polymorphic, instance):
        raise NotImplementedError


class Dumper:
    '''Base class to orchestrate the code generation of state object dumpers.'''

    def __init__(self, api, types):
        self.api = api
        self.types = types

    def dumperFactory(self):
        '''Create a dumper.

        Can be overriden by derived classes to inject their own dumper visitor.
        '''

        return ValueDumper()

    def dump(self):

        self.header()

        # Includes
        for module in api.modules:
            for header in module.headers:
                print header
        print

        # Generate the dumper functions
        types = api.getAllTypes()
        for type in self.types:
            self.dumpType(type)

    def dumpType(self, type):
        print r'void'
        print r'dumpStateObject(JSONWriter &json, const %s & so)' % type
        print r'{'
        visitor = self.dumperFactory()
        visitor.visit(type, 'so')
        print r'}'
        print

    def header(self):
        pass

    def footer(self):
        pass


if __name__ == '__main__':
    print r'#include "dxgistate_so.hpp"'
    print
    print r'#include "json.hpp"'
    print

    api = stdapi.API()
    api.addModule(dxgi.dxgi)
    api.addModule(d3d10.d3d10)
    api.addModule(d3d10_1.d3d10_1)
    api.addModule(d3d11.d3d11)

    types = [
        d3d10.D3D10_BLEND_DESC,
        d3d11.D3D11_BLEND_DESC,
    ]

    dumper = Dumper(api, types)
    dumper.dump()

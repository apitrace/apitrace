#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 Jose Fonseca
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


import gzip
import string
import struct
import sys

try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

import format


class GzipFile(gzip.GzipFile):

    def _read_eof(self):
        # Ignore incomplete files
        try:
            gzip.GzipFile._read_eof(self)
        except IOError:
            pass


TRACE_VERSION = 1

EVENT_ENTER, EVENT_LEAVE = range(2)

CALL_END, CALL_ARG, CALL_RET, CALL_THREAD = range(4)

TYPE_NULL, TYPE_FALSE, TYPE_TRUE, TYPE_SINT, TYPE_UINT, TYPE_FLOAT, TYPE_DOUBLE, TYPE_STRING, TYPE_BLOB, TYPE_ENUM, TYPE_BITMASK, TYPE_ARRAY, TYPE_STRUCT, TYPE_OPAQUE = range(14)


class Signature:
    '''Dummy class for signatures.'''

    pass


class Node:

    def visit(self, visitor):
        raise NotImplementedError

    def pretty_print(self, formatter):
        pretty_printer = PrettyPrinter(formatter)
        self.visit(pretty_printer)

    def __str__(self):
        stream = StringIO()
        formatter = format.Formatter(stream)
        self.pretty_print(formatter)
        return stream.getvalue()

    def __eq__(self, other):
        raise NotImplementedError

    def __hash__(self):
        raise NotImplementedError


class Literal(Node):

    def __init__(self, value):
        self.value = value

    def visit(self, visitor):
        visitor.visit_literal(self)

    def __eq__(self, other):
        return \
            self.__class__ == other.__class__ and \
            self.value == other.value

    def __hash__(self):
        return hash(self.value)


class Enum(Node):

    def __init__(self, name, value):
        self.name = name
        self.value = value

    def visit(self, visitor):
        visitor.visit_enum(self)

    def __eq__(self, other):
        return \
            self.__class__ == other.__class__ and \
            self.name == other.name and \
            self.value == other.value

    def __hash__(self):
        return hash(self.value)


class Array(Node):

    def __init__(self, elements):
        self.elements = tuple(elements)

    def visit(self, visitor):
        visitor.visit_array(self)

    def __eq__(self, other):
        return \
            self.__class__ == other.__class__ and \
            self.elements == other.elements

    def __hash__(self):
        return hash(self.elements)


class Pointer(Node):

    def __init__(self, value):
        self.value = value

    def visit(self, visitor):
        visitor.visit_pointer(self)

    def __eq__(self, other):
        return \
            self.__class__ == other.__class__ and \
            self.value == other.value

    def __hash__(self):
        return hash(self.value)


def Null():
    return Enum("NULL", 0)

def Bitmask(sig, value):
    return Literal(value)

def Blob(buf):
    return Literal('blob(%u)' % len(buf))


class Struct(Node):

    def __init__(self, sig, members):
        self.sig = sig
        self.members = tuple(members)

    def visit(self, visitor):
        visitor.visit_struct(self)

    def __eq__(self, other):
        return \
            self.__class__ == other.__class__ and \
            self.sig.member_names == other.sig.member_names and \
            self.members == other.members

    def __hash__(self):
        return hash(self.sig.member_names) ^ hash(self.members)


class Call(Node):

    def __init__(self, sig):
        self.sig = sig
        self.args = [None] * len(sig.arg_names)
        self.ret = None

    def get_name(self):
        return self.sig.name

    name = property(get_name)

    def visit(self, visitor):
        visitor.visit_call(self)

    def __eq__(self, other):
        return \
            self.__class__ == other.__class__ and \
            self.sig.name == other.sig.name and \
            self.args == other.args and \
            self.ret == other.ret

    def __hash__(self):
        return hash(self.sig.name) ^ hash(tuple(self.args)) ^ hash(self.ret)


class Trace(Node):

    def __init__(self, calls):
        self.calls = calls

    def visit(self, visitor):
        visitor.visit_trace(self)


class Visitor:

    def visit_literal(self, node):
        raise NotImplementedError

    def visit_enum(self, node):
        raise NotImplementedError

    def visit_array(self, node):
        raise NotImplementedError

    def visit_struct(self, node):
        raise NotImplementedError

    def visit_pointer(self, node):
        raise NotImplementedError

    def visit_call(self, node):
        raise NotImplementedError

    def visit_trace(self, node):
        raise NotImplementedError


class PrettyPrinter:

    def __init__(self, formatter):
        self.formatter = formatter

    def visit_literal(self, node):
        if isinstance(node.value, basestring):
            if len(node.value) >= 4096 or node.value.strip(string.printable):
                self.formatter.text('...')
                return

            self.formatter.literal('"' + node.value + '"')
            return

        self.formatter.literal(repr(node.value))

    def visit_enum(self, node):
        self.formatter.literal(node.name)

    def visit_array(self, node):
        self.formatter.text('{')
        sep = ''
        for value in node.elements:
            self.formatter.text(sep)
            value.visit(self)
            sep = ', '
        self.formatter.text('}')

    def visit_struct(self, node):
        self.formatter.text('{')
        sep = ''
        for name, value in zip(node.sig.member_names, node.members):
            self.formatter.text(sep)
            self.formatter.variable(name)
            self.formatter.text(' = ')
            value.visit(self)
            sep = ', '
        self.formatter.text('}')

    def visit_pointer(self, node):
        self.formatter.address(node.value)

    def visit_call(self, node):
        #self.formatter.text('%s ' % node.no)
        self.formatter.function(node.name)
        self.formatter.text('(')
        sep = ''
        for name, value in zip(node.sig.arg_names, node.args):
            self.formatter.text(sep)
            self.formatter.variable(name)
            self.formatter.text(' = ')
            value.visit(self)
            sep = ', '
        self.formatter.text(')')
        if node.ret is not None:
            self.formatter.text(' = ')
            node.ret.visit(self)

    def visit_trace(self, node):
        for call in node.calls:
            call.visit(self)
            self.formatter.newline()


class Parser:

    debug = False

    def __init__(self):
        self.file = None
        self.next_call_no = 0
        self.version = 0

        self.functions = {}
        self.enums = {}
        self.bitmasks = {}
        self.structs = {}

        self.calls = []

    def open(self, filename):
        self.file = GzipFile(filename, "rb")
        if not self.file:
            return False

        version = self.read_uint()
        if version > TRACE_VERSION:
            sys.stderr.write("error: unsupported trace format version %u\n" % version)
            return False

        return True

    def parse_call(self):
        if self.debug:
            sys.stdout.flush()
        while True:
            c = self.read_byte()
            if c == EVENT_ENTER:
                self.parse_enter()
            elif c == EVENT_LEAVE:
                return self.parse_leave()
            elif c == -1:
                return None
            else:
                sys.stderr.write("error: unknown event %i\n" % c)
                sys.exit(1)

    def parse_enter(self):
        id = self.read_uint()

        try:
            sig = self.functions[id]
        except KeyError:
            sig = Signature()
            sig.name = self.read_string()
            num_args = self.read_uint()
            sig.arg_names = tuple([self.read_string() for i in range(num_args)])
            self.functions[id] = sig

        call = Call(sig)
        call.no = self.next_call_no
        self.next_call_no += 1

        if self.parse_call_details(call):
            self.calls.append(call)
        else:
            del call

    def parse_leave(self):
        call_no = self.read_uint()
        call = None
        for i in range(len(self.calls)):
            if self.calls[i].no == call_no:
                call = self.calls.pop(i)
                break
        if call is None:
            return None

        if self.parse_call_details(call):
            return call
        else:
            del call
            return None

    def parse_call_details(self, call):
        while True:
            c = self.read_byte()
            if c == CALL_END:
                return True
            elif c == CALL_ARG:
                self.parse_arg(call)
            elif c == CALL_RET:
                call.ret = self.parse_value()
            else:
                sys.stderr.write("error: unknown call detail %i\n" % c)
                sys.exit(1)

    def parse_arg(self, call):
        index = self.read_uint()
        value = self.parse_value()
        if index >= len(call.args):
            call.args.resize(index + 1)
        call.args[index] = value
        if self.debug:
            sys.stderr.write("\tARG %i %s\n" % (index, value))

    def parse_value(self):
        c = self.read_byte()
        if c == TYPE_NULL:
            value = Null()
        elif c == TYPE_FALSE:
            value = Literal(False)
        elif c == TYPE_TRUE:
            value = Literal(True)
        elif c == TYPE_SINT:
            value = self.parse_sint()
        elif c == TYPE_UINT:
            value = self.parse_uint()
        elif c == TYPE_FLOAT:
            value = self.parse_float()
        elif c == TYPE_DOUBLE:
            value = self.parse_double()
        elif c == TYPE_STRING:
            value = self.parse_string()
        elif c == TYPE_ENUM:
            value = self.parse_enum()
        elif c == TYPE_BITMASK:
            value = self.parse_bitmask()
        elif c == TYPE_ARRAY:
            value = self.parse_array()
        elif c == TYPE_STRUCT:
            value = self.parse_struct()
        elif c == TYPE_BLOB:
            value = self.parse_blob()
        elif c == TYPE_OPAQUE:
            value = self.parse_opaque()
        else:
            sys.stderr.write("error: unknown type %i\n" % c)
            sys.exit(1)
        if self.debug:
            sys.stderr.write("\tVALUE %s\n" % value)
        return value

    def parse_sint(self):
        return Literal(-self.read_uint())

    def parse_uint(self):
        return Literal(self.read_uint())

    def parse_float(self):
        value = self.file.read(4)
        value, = struct.unpack('f', value)
        return Literal(value)

    def parse_double(self):
        value = self.file.read(8)
        value, = struct.unpack('d', value)
        return Literal(value)

    def parse_string(self):
        return Literal(self.read_string())

    def parse_enum(self):
        id = self.read_uint()
        try:
            enum = self.enums[id]
        except KeyError:
            name = self.read_string()
            value = self.parse_value()
            enum = Enum(name, value)
            self.enums[id] = enum
        return enum

    def parse_bitmask(self):
        id = self.read_uint()
        try:
            sig = self.bitmasks[id]
        except KeyError:
            sig = Signature()
            num_flags = self.read_uint()
            sig.flags = []
            for i in range(num_flags):
                name = self.read_string()
                value = self.read_uint()
                if value == 0 and i:
                    sys.stderr.write("warning: bitmask %s is zero but is not first flag\n" % name)
                flag = name, value
                sig.flags.append(flag)
            self.bitmasks[id] = sig
        assert sig

        value = self.read_uint()

        return Bitmask(sig, value)

    def parse_array(self):
        size = self.read_uint()
        elements = [self.parse_value() for i in range(size)]
        return Array(elements)

    def parse_blob(self):
        size = self.read_uint()
        if size:
            buf = self.file.read(size)
        else:
            buf = ""
        return Blob(buf)

    def parse_struct(self):
        id = self.read_uint()

        try:
            sig = self.structs[id]
        except KeyError:
            sig = Signature()
            sig.name = self.read_string()
            num_members = self.read_uint()
            sig.member_names = tuple([self.read_string() for i in range(num_members)])
            self.structs[id] = sig

        members = [self.parse_value() for i in range(len(sig.member_names))]
        value = Struct(sig, members)

        return value

    def parse_opaque(self):
        addr = self.read_uint()
        return Pointer(addr)

    def read_string(self):
        size = self.read_uint()
        if size:
            value = self.file.read(size)
        else:
            value = ''
        if self.debug:
            sys.stderr.write("\tSTRING \"%s\"\n" % value)
        return value

    def read_uint(self):
        value = 0
        shift = 0
        while True:
            c = self.file.read(1)
            if c == "":
                return 0
            c = ord(c)
            value |= (c & 0x7f) << shift
            shift += 7
            if c & 0x80 == 0:
                break
        if self.debug:
            sys.stderr.write("\tUINT %u\n" % value)
        return value

    def read_byte(self):
        c = self.file.read(1)
        if c == "":
            if self.debug:
                sys.stderr.write("\tEOF\n")
            return -1
        else:
            c = ord(c)
            if self.debug:
                sys.stderr.write("\tBYTE 0x%x\n" % c)
        return c


def main():
    formatter = format.DefaultFormatter(sys.stdout)
    for arg in sys.argv[1:]:
        parser = Parser()
        parser.open(arg)
        call = parser.parse_call()
        while call:
            formatter.text('%u ' % call.no)
            call.pretty_print(formatter)
            formatter.text('\n')
            call = parser.parse_call()


if __name__ == '__main__':
    main()

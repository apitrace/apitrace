#!/usr/bin/env python
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


import sys
import re
import optparse


class Parser:

    def __init__(self, stream):
        pass


class LineParser:
    """Base class for parsers that read line-based formats."""

    def __init__(self, stream):
        self._stream = stream
        self._line = None
        self._eof = False
        # read lookahead
        self.readline()
    
    def parse(self):
        raise NotImplementedError

    def readline(self):
        line = self._stream.readline()
        if not line:
            self._line = ''
            self._eof = True
        self._line = line.rstrip('\r\n')

    def lookahead(self):
        assert self._line is not None
        return self._line

    def consume(self):
        assert self._line is not None
        line = self._line
        self.readline()
        return line

    def eof(self):
        assert self._line is not None
        return self._eof
    
    def skip_whitespace(self):
        while not self.eof() and self.match_whitespace() or self.match_comment():
            self.consume()

    def match_whitespace(self):
        line = self.lookahead()
        return not line.strip()

    def match_comment(self):
        return False


class TypemapParser(LineParser):

    def parse(self):
        typemap = {}
        self.skip_whitespace()
        while not self.eof():
            line = self.consume()
            fields = [field.strip() for field in line.split(',')]
            src = fields[0]
            dst = fields[3]
            self.skip_whitespace()
            typemap[src] = dst
        return typemap
    
    def match_comment(self):
        line = self.lookahead()
        return line.startswith('#')


class SpecParser(LineParser):

    property_re = re.compile(r'^\w+:')
    prototype_re = re.compile(r'^(\w+)\((.*)\)$')

    def __init__(self, stream, prefix='', typemap = None):
        LineParser.__init__(self, stream)
        if typemap is None:
            self.typemap = {}
        else:
            self.typemap = typemap
        self.prefix = prefix
        self.category = None

    def parse(self):
        self.skip_whitespace()
        while not self.eof():
            line = self.lookahead()
            if self.property_re.match(line):
                self.parse_property()
            elif self.prototype_re.match(line):
                self.parse_prototype()
            else:
                self.consume()
            self.skip_whitespace()

    def parse_property(self):
        line = self.consume()
        name, value = line.split(':', 1)
        if name == 'category':
            values = value.split()
            #self.prefix = values[0]

    get_function_re = re.compile(r'^Get[A-Z]\w+')

    def parse_prototype(self):
        line = self.consume()
        mo = self.prototype_re.match(line)
        function_name, arg_names = mo.groups()
        arg_names = [arg_name.strip() for arg_name in arg_names.split(',') if arg_name.strip()]
        
        ret_type = 'Void'
        arg_types = {}
        category = None
        line = self.lookahead()
        while line.startswith('\t'):
            fields = line.split(None, 2)
            if fields[0] == 'return':
                ret_type = self.parse_type(fields[1])
            elif fields[0] == 'param':
                arg_name, arg_type = fields[1:3]
                arg_types[fields[1]] = self.parse_arg(arg_name, arg_type)
            elif fields[0] == 'category':
                category = fields[1]
            else:
                pass
            self.consume()
            line = self.lookahead()
        self.consume()
        args = [arg_types[arg_name] for arg_name in arg_names]

        if category is not None:
            if category == self.prefix:
                category = self.prefix.upper()
            else:
                category = self.prefix.upper() + '_' + category
            if category != self.category:
                if self.category is not None:
                    print
                print '    # %s' % category
                self.category = category

        if self.prefix == 'wgl':
            constructor = 'StdFunction'
        else:
            constructor = 'glFunction'
        extra = ''
        if self.get_function_re.match(function_name):
            extra += ', sideeffects=False'
        print '    %s(%s, "%s%s", [%s]%s),' % (constructor, ret_type, self.prefix, function_name, ', '.join(args), extra)

    array_re = re.compile(r'^array\s+\[(.*)\]$')

    def parse_arg(self, arg_name, arg_type):
        base_type, inout, kind = arg_type.split(' ', 2)
        base_type = self.parse_type(base_type)

        if kind == 'value':
            arg_type = base_type
        elif kind == 'reference':
            arg_type = 'Pointer(%s)' % base_type
            if inout == 'in':
                arg_type = 'Const(%s)' % arg_type
        elif kind.startswith("array"):
            mo = self.array_re.match(kind)
            if mo:
                length = mo.group(1)
                arg_type = 'Array(%s, "%s")' % (base_type, length)
            else:
                arg_type = 'OpaquePointer(%s)' % base_type
            if inout == 'in':
                arg_type = 'Const(%s)' % arg_type
        else:
            assert False
        
        arg = '(%s, "%s")' % (arg_type, arg_name)
        if inout == 'out':
            arg = 'Out' + arg
        return arg

    _typemap = {
        'void': 'Void',
        'int': 'Int',
        'float': 'Float',
    }

    def parse_type(self, type):
        type = self.typemap.get(type, type)
        type = self._typemap.get(type, type)
        return type

    def match_comment(self):
        line = self.lookahead()
        return line.startswith('#')


def main():
    prefix = sys.argv[1]

    parser = TypemapParser(open(sys.argv[2], 'rt'))
    typemap = parser.parse()

    for arg in sys.argv[3:]:
        parser = SpecParser(open(arg, 'rt'), prefix=prefix, typemap=typemap)
        parser.parse()
    

if __name__ == '__main__':
    main()

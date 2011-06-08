#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 Jose Fonseca
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


'''Script to parse C declarations and spew API definitions.
'''


import sys
import re
import optparse


class Parser:

    token_re = re.compile(r'(\w+|\s+|.)')

    multi_comment_re = re.compile(r'/\*.*?\*/', flags = re.DOTALL)
    single_comment_re = re.compile(r'//.*',)

    def __init__(self):
        self.tokens = []

    def has_side_effects(self, name):
        return True

    def parse(self, s):
        s = self.multi_comment_re.sub('', s)
        s = self.single_comment_re.sub('', s)
        self.tokens = self.token_re.split(s)
        self.tokens = [token for token in self.tokens if self.filter_token(token)]

        while self.tokens:
            #print self.tokens[0:10]
            self.parse_declaration()

    def filter_token(self, token):
        if not token or token.isspace():
            return False
        if token.startswith('AVAILABLE_') or token.startswith('DEPRECATED_'):
            return False
        if token in ['FAR']:
            return False
        return True

    def parse_declaration(self):
        if self.tokens[0] == 'mask':
            self.parse_value('mask', 'Flags')
        elif self.tokens[0] == 'value':
            self.parse_value('value', 'FakeEnum')
        elif self.tokens[0] == 'interface':
            self.parse_interface()
        else:
            self.parse_prototype()

    def parse_value(self, ref_token, constructor):
        self.match(ref_token)
        type = self.tokens.pop(0)
        name = self.tokens.pop(0)
        self.match('{')

        print '%s = %s(%s, [' % (name, constructor, type)

        while self.tokens[0] != '}':
            self.match('#')
            self.match('define')
            name = self.tokens.pop(0)
            value = self.tokens.pop(0)
            tag = self.parse_tag()
            #print '    "%s",\t# %s' % (name, value) 
            print '    "%s",' % (name,) 
        self.match('}')
        self.match(';')

        print '])'
        print

    def parse_interface(self):
        self.match('interface')
        name = self.tokens.pop(0)
        self.match(':')
        base = self.tokens.pop(0)
        self.match('{')

        print '%s = Interface("%s", %s)' % (name, name, base)
        print '%s.methods += [' % (name,)

        while self.tokens[0] != '}':
            self.parse_prototype('Method')
        self.match('}')
        self.match(';')

        print ']'
        print

    def parse_prototype(self, creator = 'Function'):
        if self.tokens[0] == 'extern':
            self.tokens.pop(0)

        ret = self.parse_type()

        name = self.tokens.pop(0)
        extra = ''
        if not self.has_side_effects(name):
            extra += ', sideeffects=False'
        name = name

        self.match('(')
        args = []
        if self.tokens[0] == 'void' and self.tokens[1] == ')':
            self.tokens.pop(0)
        while self.tokens[0] != ')':
            arg = self.parse_arg()
            args.append(arg)
            if self.tokens[0] == ',':
                self.tokens.pop(0)
        self.tokens.pop(0) == ')'
        
        if self.tokens and self.tokens[0] == ';':
            self.tokens.pop(0)

        print '    %s(%s, "%s", [%s]%s),' % (creator, ret, name, ', '.join(args), extra)

    def parse_arg(self):
        tag = self.parse_tag()

        type = self.parse_type()
        name = self.tokens.pop(0)

        arg = '(%s, "%s")' % (type, name)
        if tag == 'out':
            arg = 'Out' + arg
        return arg

    def parse_tag(self):
        tag = None
        if self.tokens[0] == '[':
            self.tokens.pop(0)
            tag = self.tokens.pop(0)
            self.match(']')
        return tag

    int_tokens = ('unsigned', 'signed', 'int', 'long', 'short', 'char')

    type_table = {
        'float':    'Float',
        'double':   'Double',
        'int8_t':   'Int8',
        'uint8_t':  'UInt8',
        'int16_t':  'Int16',
        'uint16_t': 'UInt16',
        'int32_t':  'Int32',
        'uint32_t': 'UInt32',
        'int64_t' : 'Int64',
        'uint64_t': 'UInt64',
    }

    def parse_type(self):
        token = self.tokens.pop(0)
        if token == 'const':
            return 'Const(%s)' % self.parse_type()
        if token == 'void':
            type = 'Void'
        elif token in self.int_tokens:
            unsigned = False
            signed = False
            long = 0
            short = 0
            char = False
            while token in self.int_tokens:
                if token == 'unsigned':
                    unsigned = True
                if token == 'signed':
                    signed = True
                if token == 'long':
                    long += 1
                if token == 'short':
                    short += 1
                if token == 'char':
                    char = False
                if self.tokens[0] in self.int_tokens:
                    token = self.tokens.pop(0)
                else:
                    token = None
            if char:
                type = 'Char'
                if signed:
                    type = 'S' + type
            elif short:
                type = 'Short'
            elif long:
                type = 'Long' * long
            else:
                type = 'Int'
            if unsigned:
                type = 'U' + type
        else:
            type = self.type_table.get(token, token)
        while self.tokens[0] == '*':
            type = 'OpaquePointer(%s)' % type
            self.tokens.pop(0)
        return type

    def match(self, ref_token):
        if not self.tokens:
            raise Exception('unexpected EOF')
        token = self.tokens.pop(0)
        if token != ref_token:
            raise Exception('token mismatch', token, ref_token)


        


def main():
    parser = Parser()
    for arg in sys.argv[1:]:
        parser.parse(open(arg, 'rt').read())
    

if __name__ == '__main__':
    main()

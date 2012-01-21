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


class DeclParser:

    token_re = re.compile(r'(\d[x0-9a-fA-F.UL]*|\w+|\s+|.)')

    multi_comment_re = re.compile(r'/\*.*?\*/', flags = re.DOTALL)
    single_comment_re = re.compile(r'//.*',)

    def __init__(self):
        self.tokens = []

    def has_side_effects(self, name):
        return True


    def tokenize(self, s):
        s = self.multi_comment_re.sub('', s)
        s = self.single_comment_re.sub('', s)
        self.tokens = self.token_re.split(s)
        self.tokens = [token for token in self.tokens if self.filter_token(token)]

    def filter_token(self, token):
        if not token or token.isspace():
            return False
        if token.startswith('AVAILABLE_') or token.startswith('DEPRECATED_'):
            return False
        if token in ['FAR']:
            return False
        return True

    def lookahead(self, index = 0):
        try:
            return self.tokens[index]
        except KeyError:
            return None

    def match(self, *ref_tokens):
        return self.lookahead() in ref_tokens

    def consume(self, *ref_tokens):
        if not self.tokens:
            raise Exception('unexpected EOF')
        token = self.tokens.pop(0)
        if ref_tokens and token not in ref_tokens:
            raise Exception('token mismatch', token, ref_tokens)
        return token

    def eof(self):
        return not self.tokens


    def parse(self, s):
        self.tokenize(s)

        while not self.eof():
            #print self.tokens[0:10]
            self.parse_declaration()

    def parse_declaration(self):
        self.parse_tags()
        if self.match('#'):
            self.parse_define()
        elif self.match('enum'):
            self.parse_enum()
        elif self.match('interface'):
            self.parse_interface()
        elif self.match('mask'):
            self.parse_value('mask', 'Flags')
        elif self.match('struct'):
            self.parse_struct()
        elif self.match('value'):
            self.parse_value('value', 'FakeEnum')
        elif self.match('typedef'):
            self.parse_typedef()
        else:
            self.parse_prototype()
        if not self.eof() and self.match(';'):
            self.consume(';')

    def parse_typedef(self):
        self.consume('typedef')
        if self.lookahead(2) in (';', ','):
            base_type = self.consume()
            while True:
                type = base_type
                if self.match('*'):
                    self.consume()
                    type = 'Pointer(%s)' % type
                name = self.consume()
                print '%s = Alias("%s", %s)' % (name, name, type)
                if self.match(','):
                    self.consume()
                else:
                    break
        else:
            self.parse_declaration()
            self.consume()

    def parse_enum(self):
        self.consume('enum')
        name = self.consume()
        self.consume('{')

        print '%s = Enum("%s", [' % (name, name)

        #value = 0
        while self.lookahead() != '}':
            name = self.consume()
            if self.match('='):
                self.consume('=')
                value = self.consume()
            if self.match(','):
                self.consume(',')
            tags = self.parse_tags()
            #print '    "%s",\t# %s' % (name, value) 
            print '    "%s",' % (name,) 
            #value += 1
        self.consume('}')

        print '])'
        print

    def parse_value(self, ref_token, constructor):
        self.consume(ref_token)
        type = self.consume()
        name = self.consume()
        self.consume('{')

        print '%s = %s(%s, [' % (name, constructor, type)

        while self.lookahead() != '}':
            name, value = self.parse_define()
        self.consume('}')

        print '])'
        print

    def parse_define(self):
        self.consume('#')
        self.consume('define')
        name = self.consume()
        value = self.consume()
        #print '    "%s",\t# %s' % (name, value) 
        print '    "%s",' % (name,) 
        return name, value

    def parse_struct(self):
        self.consume('struct')
        name = self.consume()
        self.consume('{')

        print '%s = Struct("%s", [' % (name, name)

        value = 0
        while self.lookahead() != '}':
            type, name = self.parse_named_type()

            if self.match(':'):
                self.consume()
                self.consume()

            if self.match(','):
                self.consume(',')
            self.consume(';')
            print '    (%s, "%s"),' % (type, name) 
            value += 1
        self.consume('}')

        print '])'
        print

    def parse_interface(self):
        self.consume('interface')
        name = self.consume()
        if self.match(';'):
            return
        self.consume(':')
        if self.lookahead() in ('public', 'protected'):
            self.consume()
        base = self.consume()
        self.consume('{')

        print '%s = Interface("%s", %s)' % (name, name, base)
        print '%s.methods += [' % (name,)

        while self.lookahead() != '}':
            self.parse_prototype('Method')
            self.consume(';')
        self.consume('}')

        print ']'
        print

    def parse_prototype(self, creator = 'Function'):
        if self.match('extern'):
            self.consume()

        ret = self.parse_type()

        if self.match('__stdcall', 'WINAPI'):
            self.consume()
            creator = 'StdFunction'

        name = self.consume()
        extra = ''
        if not self.has_side_effects(name):
            extra += ', sideeffects=False'
        name = name

        self.consume('(')
        args = []
        if self.match('void') and self.tokens[1] == ')':
            self.consume()
        while self.lookahead() != ')':
            arg = self.parse_arg()
            args.append(arg)
            if self.match(','):
                self.consume()
        self.consume(')')
        if self.lookahead() == 'const':
            self.consume()
            extra = ', const=True' + extra
        
        print '    %s(%s, "%s", [%s]%s),' % (creator, ret, name, ', '.join(args), extra)

    def parse_arg(self):
        tags = self.parse_tags()

        type, name = self.parse_named_type()

        arg = '(%s, "%s")' % (type, name)
        if 'out' in tags:
            arg = 'Out' + arg

        if self.match('='):
            self.consume()
            while not self.match(',', ')'):
                self.consume()

        return arg

    def parse_tags(self):
        tags = []
        if self.match('['):
            self.consume()
            while not self.match(']'):
                tag = self.consume()
                tags.append(tag)
            self.consume(']')
        return tags

    def parse_named_type(self):
        type = self.parse_type()
        name = self.consume()
        if self.match('['):
            self.consume()
            length = self.consume()
            self.consume(']')
            try:
                int(length)
            except ValueError:
                length = "%s" % length
            type = 'Array(%s, %s)' % (type, length)
        return type, name

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
        const = False
        token = self.consume()
        if token == 'const':
            token = self.consume()
            const = True
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
                if self.lookahead() in self.int_tokens:
                    token = self.consume()
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
        if const:
            type = 'Const(%s)' % type
        while True:
            if self.match('*'):
                self.consume('*')
                type = 'OpaquePointer(%s)' % type
            elif self.match('const'):
                self.consume('const')
                type = 'Const(%s)' % type
            else:
                break
        return type


def main():
    args = sys.argv[1:]

    parser = DeclParser()
    if args:
        for arg in args:
            parser.parse(open(arg, 'rt').read())
    else:
        parser.parse(sys.stdin.read())
    

if __name__ == '__main__':
    main()

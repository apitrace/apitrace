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

    comment_re = re.compile(r'/\*.*?\*/', flags = re.DOTALL)

    def __init__(self):
        self.tokens = []

    def has_side_effects(self, name):
        return True

    def parse(self, s):
        s = self.comment_re.sub('', s)
        self.tokens = self.token_re.split(s)
        self.tokens = [token for token in self.tokens if self.filter_token(token)]

        while self.tokens:
            #print self.tokens[0:10]
            self.parse_prototype()

    def filter_token(self, token):
        if not token or token.isspace():
            return False
        if token.startswith('AVAILABLE_') or token.startswith('DEPRECATED_'):
            return False
        if token in ['FAR']:
            return False
        return True

    def parse_prototype(self):
        if self.tokens[0] == 'extern':
            self.tokens.pop(0)

        ret = self.parse_type()

        name = self.tokens.pop(0)
        extra = ''
        if not self.has_side_effects(name):
            extra += ', sideeffects=False'
        name = name

        assert self.tokens.pop(0) == '('
        args = []
        while self.tokens[0] != ')':
            arg = self.parse_arg()
            args.append(arg)
            if self.tokens[0] == ',':
                self.tokens.pop(0)
        self.tokens.pop(0) == ')'
        
        if self.tokens and self.tokens[0] == ';':
            self.tokens.pop(0)

        print '    Function(%s, "%s", [%s]%s),' % (ret, name, ', '.join(args), extra)

    def parse_arg(self):
        type = self.parse_type()
        name = self.tokens.pop(0)
        return '(%s, "%s")' % (type, name)

    def parse_type(self):
        token = self.tokens.pop(0)
        if token == 'const':
            return 'Const(%s)' % self.parse_type()
        if token == 'void':
            type = 'Void'
        else:
            type = token
        while self.tokens[0] == '*':
            type = 'OpaquePointer(%s)' % type
            self.tokens.pop(0)
        return type


def main():
    parser = Parser()
    for arg in sys.argv[1:]:
        parser.parse(open(arg, 'rt').read())
    

if __name__ == '__main__':
    main()

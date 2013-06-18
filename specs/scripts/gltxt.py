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


"""Parser for OpenGL .txt extensions specification."""


import sys
import re
import optparse
from urllib2 import urlopen


def stderr(x):
    sys.stderr.write(str(x) + '\n')


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


class TxtParser(LineParser):

    section_re = re.compile(r'^([A-Z]\w+)( \w+)*$')

    property_re = re.compile(r'^\w+:')
    prototype_re = re.compile(r'^(\w+)\((.*)\)$')

    comment_start_re = re.compile(r'^/\*')
    comment_end_re = re.compile(r'.*\*/$')

    def __init__(self, stream, prefix=''):
        LineParser.__init__(self, stream)
        self.prefix = prefix

    def parse(self):
        while  not self.eof():
            while not self.eof():
                line = self.lookahead()
                if self.eof():
                    return
                mo = self.section_re.match(line)
                if mo:
                    break
                self.consume()
            line = self.consume()
            self.parse_section(line)
        print

    def parse_section(self, name):
        if name == 'Name Strings':
            self.parse_strings()
        if name == 'New Procedures and Functions':
            self.parse_procs()

    def parse_strings(self):
        while not self.eof():
            line = self.lookahead()
            if not line.strip():
                self.consume()
                continue
            if not line.startswith(' '):
                break
            self.consume()
            name = line.strip()
            if name.startswith('EGL_'):
                self.prefix = ''
            print '    # %s' % name

    def skip_c_comments(self):
        while not self.eof():
            line = self.lookahead().strip()
            mo = self.comment_start_re.match(line)
            if not mo:
                return
            while not self.eof():
                self.consume()
                mo = self.comment_end_re.match(line)
                if mo:
                    return
                line = self.lookahead().strip()

    def parse_procs(self):
        lines = []
        while not self.eof():
            self.skip_c_comments()
            line = self.lookahead()
            if not line.strip():
                self.consume()
                continue
            if not line[0].isspace():
                break
            self.consume()
            lines.append(line.strip())
            if line[-1] in (';', ')'):
                prototype = ' '.join(lines)
                self.parse_proc(prototype)
                lines = []

    token_re = re.compile(r'(\w+|\s+|.)')
    get_function_re = re.compile(r'^Get[A-Z]\w+')

    def parse_proc(self, prototype):
        #print prototype
        tokens = self.token_re.split(prototype)
        self.tokens = [token for token in tokens if token.strip()]
        #print self.tokens

        ret = self.parse_type()

        name = self.tokens.pop(0)
        extra = ''
        if self.get_function_re.match(name):
            extra += ', sideeffects=False'
        name = self.prefix + name

        assert self.tokens.pop(0) == '('
        args = []
        while self.tokens[0] != ')':
            arg = self.parse_arg()
            args.append(arg)
            if self.tokens[0] == ',':
                self.tokens.pop(0)
        print '    GlFunction(%s, "%s", [%s]%s),' % (ret, name, ', '.join(args), extra)

    def parse_arg(self):
        type = self.parse_type()
        if self.tokens[0] == ')':
            assert type == 'Void'
            return ''
        name = self.tokens.pop(0)
        if self.tokens[0] == '[':
            self.tokens.pop(0)
            n = int(self.tokens.pop(0))
            assert self.tokens.pop(0) == ']'
            type = 'Array(%s, %d)' % (type, n)
        return '(%s, "%s")' % (type, name)

    def parse_type(self):
        token = self.tokens.pop(0)
        if token == 'const':
            return 'Const(%s)' % self.parse_type()
        if token == 'void':
            type = 'Void'
        else:
            type = self.prefix.upper() + token
        while self.tokens[0] == '*':
            type = 'OpaquePointer(%s)' % type
            self.tokens.pop(0)
        return type


def main():
    optparser = optparse.OptionParser(
        usage="\n\t%prog [options] [URL|TXT] ...")
    optparser.add_option(
        '-p', '--prefix', metavar='STRING',
        type="string", dest="prefix", default='gl',
        help="function prefix [default: %default]")

    (options, args) = optparser.parse_args(sys.argv[1:])

    for arg in args:
        if arg.startswith('http://'):
            stream = urlopen(arg, 'rt')
        else:
            stream = open(arg, 'rt')
        parser = TxtParser(stream, prefix = options.prefix)
        parser.parse()
    

if __name__ == '__main__':
    main()

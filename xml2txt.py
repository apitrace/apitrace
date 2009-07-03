#!/usr/bin/env python
#############################################################################
#
# Copyright 2008 Tungsten Graphics, Inc.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################


import sys
import optparse
import xml.parsers.expat
import gzip


ELEMENT_START, ELEMENT_END, CHARACTER_DATA, EOF = range(4)


class XmlToken:

    def __init__(self, type, name_or_data, attrs = None, line = None, column = None):
        assert type in (ELEMENT_START, ELEMENT_END, CHARACTER_DATA, EOF)
        self.type = type
        self.name_or_data = name_or_data
        self.attrs = attrs
        self.line = line
        self.column = column

    def __str__(self):
        if self.type == ELEMENT_START:
            return '<' + self.name_or_data + ' ...>'
        if self.type == ELEMENT_END:
            return '</' + self.name_or_data + '>'
        if self.type == CHARACTER_DATA:
            return self.name_or_data
        if self.type == EOF:
            return 'end of file'
        assert 0


class XmlTokenizer:
    """Expat based XML tokenizer."""

    def __init__(self, fp, skip_ws = True):
        self.fp = fp
        self.tokens = []
        self.index = 0
        self.final = False
        self.skip_ws = skip_ws
        
        self.character_pos = 0, 0
        self.character_data = ''
        
        self.parser = xml.parsers.expat.ParserCreate()
        self.parser.StartElementHandler  = self.handle_element_start
        self.parser.EndElementHandler    = self.handle_element_end
        self.parser.CharacterDataHandler = self.handle_character_data
    
    def handle_element_start(self, name, attributes):
        self.finish_character_data()
        line, column = self.pos()
        token = XmlToken(ELEMENT_START, name, attributes, line, column)
        self.tokens.append(token)
    
    def handle_element_end(self, name):
        self.finish_character_data()
        line, column = self.pos()
        token = XmlToken(ELEMENT_END, name, None, line, column)
        self.tokens.append(token)

    def handle_character_data(self, data):
        if not self.character_data:
            self.character_pos = self.pos()
        self.character_data += data
    
    def finish_character_data(self):
        if self.character_data:
            if not self.skip_ws or not self.character_data.isspace(): 
                line, column = self.character_pos
                token = XmlToken(CHARACTER_DATA, self.character_data, None, line, column)
                self.tokens.append(token)
            self.character_data = ''
    
    def next(self):
        size = 16*1024
        while self.index >= len(self.tokens) and not self.final:
            self.tokens = []
            self.index = 0
            data = self.fp.read(size)
            self.final = len(data) < size
            data = data.rstrip('\0')
            try:
                self.parser.Parse(data, self.final)
            except xml.parsers.expat.ExpatError, e:
                #if e.code == xml.parsers.expat.errors.XML_ERROR_NO_ELEMENTS:
                if e.code == 3:
                    pass
                else:
                    raise e
        if self.index >= len(self.tokens):
            line, column = self.pos()
            token = XmlToken(EOF, None, None, line, column)
        else:
            token = self.tokens[self.index]
            self.index += 1
        return token

    def pos(self):
        return self.parser.CurrentLineNumber, self.parser.CurrentColumnNumber


class TokenMismatch(Exception):

    def __init__(self, expected, found):
        self.expected = expected
        self.found = found

    def __str__(self):
        return '%u:%u: %s expected, %s found' % (self.found.line, self.found.column, str(self.expected), str(self.found))



class XmlParser:
    """Base XML document parser."""

    def __init__(self, fp):
        self.tokenizer = XmlTokenizer(fp)
        self.consume()
    
    def consume(self):
        self.token = self.tokenizer.next()

    def match_element_start(self, name):
        return self.token.type == ELEMENT_START and self.token.name_or_data == name
    
    def match_element_end(self, name):
        return self.token.type == ELEMENT_END and self.token.name_or_data == name

    def element_start(self, name):
        while self.token.type == CHARACTER_DATA:
            self.consume()
        if self.token.type != ELEMENT_START:
            raise TokenMismatch(XmlToken(ELEMENT_START, name), self.token)
        if self.token.name_or_data != name:
            raise TokenMismatch(XmlToken(ELEMENT_START, name), self.token)
        attrs = self.token.attrs
        self.consume()
        return attrs
    
    def element_end(self, name):
        while self.token.type == CHARACTER_DATA:
            self.consume()
        if self.token.type != ELEMENT_END:
            raise TokenMismatch(XmlToken(ELEMENT_END, name), self.token)
        if self.token.name_or_data != name:
            raise TokenMismatch(XmlToken(ELEMENT_END, name), self.token)
        self.consume()

    def character_data(self, strip = True):
        data = ''
        while self.token.type == CHARACTER_DATA:
            data += self.token.name_or_data
            self.consume()
        if strip:
            data = data.strip()
        return data


class GzipFile(gzip.GzipFile):

    def _read_eof(self):
        # Ignore incomplete files
        try:
            gzip.GzipFile._read_eof(self)
        except IOError:
            pass


class Formatter:
    
    def function(self, name):
        return name
        
    def variable(self, name):
        return name

    def literal(self, value):
        return str(value)
    
    def address(self, addr):
        return addr


class AnsiFormatter(Formatter):
    '''Formatter for plain-text files which outputs ANSI escape codes. See
    http://en.wikipedia.org/wiki/ANSI_escape_code for more information
    concerning ANSI escape codes.
    '''

    _csi = '\33['

    _normal = '0m'
    _bold = '1m'
    _italic = '3m'
    _red = '31m'
    _green = '32m'
    _blue = '34m'

    def _escape(self, code, text):
        return self._csi + code + text + self._csi + self._normal

    def function(self, name):
        text = Formatter.function(self, name)
        return self._escape(self._bold, text)
        
    def variable(self, name):
        text = Formatter.variable(self, name)
        return self._escape(self._italic, text)

    def literal(self, value):
        text = Formatter.literal(self, value)
        return self._escape(self._blue, text)
    
    def address(self, value):
        text = Formatter.address(self, value)
        return self._escape(self._green, text)


class TraceParser(XmlParser):

    def __init__(self, stream, formatter):
        XmlParser.__init__(self, stream)
        self.formatter = formatter

    def parse(self):
        self.element_start('trace')
        while self.token.type not in (ELEMENT_END, EOF):
            self.parse_call()
        if self.token.type != EOF:
            self.element_end('trace')

    def parse_call(self):
        attrs = self.element_start('call')
        name = attrs['name']
        args = []
        ret = None
        while self.token.type == ELEMENT_START:
            if self.token.name_or_data == 'arg':
                arg = self.parse_arg()
                args.append(arg)
            elif self.token.name_or_data == 'ret':
                ret = self.parse_ret()
            elif self.token.name_or_data == 'call':
                # ignore nested function calls
                self.parse_call()
            else:
                raise TokenMismatch("<arg ...> or <ret ...>", self.token)
        self.element_end('call')
        
        call = self.formatter.function(name)
        call += '(' + ', '.join([self.formatter.variable(name) + ' = ' + value for name, value in args]) + ')'
        if ret is not None:
            call += ' = ' + ret
        call += '\n'
        
        try:
            sys.stdout.write(call)
        except IOError: 
            # catch broken pipe
            sys.exit(0)

    def parse_arg(self):
        attrs = self.element_start('arg')
        name = attrs['name']
        value = self.parse_value()
        self.element_end('arg')

        return name, value

    def parse_ret(self):
        attrs = self.element_start('ret')
        value = self.parse_value()
        self.element_end('ret')

        return value

    def parse_value(self):
        if self.token.type == CHARACTER_DATA:
            return self.formatter.literal(self.character_data())
        if self.token.type == ELEMENT_START:
            if self.token.name_or_data == 'elem':
                return self.parse_elems()
            if self.token.name_or_data == 'ref':
                return self.parse_ref()
        raise TokenMismatch("<elem ...>, <ref ...>, or text", self.token)

    def parse_elems(self):
        elems = [self.parse_elem()]
        while self.token.type != ELEMENT_END:
            elems.append(self.parse_elem())
        return '{' + ', '.join([name + ' = ' + value for name, value in elems]) + '}'

    def parse_elem(self):
        attrs = self.element_start('elem')
        name = attrs['name']
        value = self.parse_value()
        self.element_end('elem')

        return name, value

    def parse_ref(self):
        attrs = self.element_start('ref')
        if self.token.type != ELEMENT_END:
            value = '&' + self.parse_value()
        else:
            value = self.formatter.address(attrs['addr'])
        self.element_end('ref')

        return value


def main():
    parser = optparse.OptionParser(
        usage="\n\t%prog [options] [file] ...")
    parser.add_option(
        '--color', '--colour',
        type="choice", choices=('never', 'always', 'auto'), metavar='WHEN',
        dest="color", default="always",
        help="coloring: never, always, or auto [default: %default]")
    (options, args) = parser.parse_args(sys.argv[1:])

    if options.color == 'always' or options.color == 'auto' and sys.stdout.isatty():
        formatter = AnsiFormatter()
    else:
        formatter = Formatter()
    
    if args:
        for arg in args:
            if arg.endswith('.gz'):
                stream = GzipFile(arg, 'rt')
            elif arg.endswith('.bz2'):
                from bz2 import BZ2File
                stream = BZ2File(arg, 'rt')
            else:
                stream = open(arg, 'rt')
            parser = TraceParser(stream, formatter)
            parser.parse()
    else:
            parser = TraceParser(sys.stdin, formatter)
            parser.parse()


if __name__ == '__main__':
    main()

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


import difflib
import optparse
import os.path
import sys

from trace import Parser

try:
    import debug
except ImportError:
    pass


ignored_function_names = set([
    'glGetString',
    'glXGetClientString',
    'glXGetCurrentDisplay',
    'glXGetProcAddress',
    'glXGetProcAddressARB',
    'wglGetProcAddress',
])


def readtrace(trace, limit=sys.maxint):
    calls = []
    parser = Parser()
    parser.open(trace)
    call = parser.parse_call()
    while call and len(calls) < limit:
        hash(call)
        if call.sig.name not in ignored_function_names:
            calls.append(call)
        call = parser.parse_call()
    return calls


class SDiffer:

    def __init__(self, a, b):
        self.a = a
        self.b = b

    def diff(self):
        matcher = difflib.SequenceMatcher(None, self.a, self.b)
        for tag, alo, ahi, blo, bhi in matcher.get_opcodes():
            if tag == 'replace':
                self.replace(alo, ahi, blo, bhi)
            elif tag == 'delete':
                self.delete(alo, ahi)
            elif tag == 'insert':
                self.insert(blo, bhi)
            elif tag == 'equal':
                self.equal(alo, ahi)
            else:
                raise ValueError, 'unknown tag %s' % (tag,)

    def replace(self, alo, ahi, blo, bhi):
        assert alo < ahi and blo < bhi
        
        a_names = [call.name for call in self.a[alo:ahi]]
        b_names = [call.name for call in self.b[blo:bhi]]

        matcher = difflib.SequenceMatcher(None, a_names, b_names)
        for tag, _alo, _ahi, _blo, _bhi in matcher.get_opcodes():
            _alo += alo
            _ahi += alo
            _blo += blo
            _bhi += blo
            if tag == 'replace':
                self.replace_dissimilar(_alo, _ahi, _blo, _bhi)
            elif tag == 'delete':
                self.delete(_alo, _ahi)
            elif tag == 'insert':
                self.insert(_blo, _bhi)
            elif tag == 'equal':
                self.replace_similar(_alo, _ahi, _blo, _bhi)
            else:
                raise ValueError, 'unknown tag %s' % (tag,)

    def replace_similar(self, alo, ahi, blo, bhi):
        assert alo < ahi and blo < bhi
        assert ahi - alo == bhi - blo
        for i in xrange(0, bhi - blo):
            a_call = self.a[alo + i]
            b_call = self.b[blo + i]
            assert a_call.name == b_call.name
            assert len(a_call.args) == len(b_call.args)
            sys.stdout.write(b_call.name + '(')
            sep = ''
            for j in xrange(len(b_call.args)):
                sys.stdout.write(sep)
                self.replace_value(a_call.args[j], b_call.args[j])
                sep = ', '
            sys.stdout.write(')')
            if a_call.ret is not None or b_call.ret is not None:
                sys.stdout.write(' = ')
                self.replace_value(a_call.ret, b_call.ret)
            sys.stdout.write('\n')

    def replace_dissimilar(self, alo, ahi, blo, bhi):
        assert alo < ahi and blo < bhi
        if bhi - blo < ahi - alo:
            first  = self.insert(blo, bhi)
            second = self.delete(alo, ahi)
        else:
            first  = self.delete(alo, ahi)
            second = self.insert(blo, bhi)

        for g in first, second:
            for line in g:
                yield line

    def replace_value(self, a, b):
        if b == a:
            sys.stdout.write(str(b))
        else:
            sys.stdout.write('%s -> %s' % (a, b))

    escape = "\33["

    def delete(self, alo, ahi):
        self.dump('- ' + self.escape + '9m', self.a, alo, ahi, self.escape + '0m')

    def insert(self, blo, bhi):
        self.dump('+ ', self.b, blo, bhi)

    def equal(self, alo, ahi):
        self.dump('  ' + self.escape + '2m', self.a, alo, ahi, self.escape + '0m')

    def dump(self, prefix, x, lo, hi, suffix=""):
        for i in xrange(lo, hi):
            sys.stdout.write(prefix + str(x[i]) + suffix + '\n')


def main():
    global options

    optparser = optparse.OptionParser(
        usage='\n\t%prog <trace> <trace>',
        version='%%prog')
    optparser.add_option(
        '-d', '--tracedump', metavar='PROGRAM',
        type='string', dest='tracedump', default='tracedump',
        help='tracedump command [default: %default]')
    optparser.add_option(
        '-l', '--limit', metavar='CALLS',
        type='int', dest='limit', default=1000,
        help='limit the number of calls [default: %default]')

    (options, args) = optparser.parse_args(sys.argv[1:])
    if len(args) != 2:
        optparser.error("incorrect number of arguments")

    ref_calls = readtrace(args[0], options.limit)
    src_calls = readtrace(args[1], options.limit)

    differ = SDiffer(ref_calls, src_calls)
    differ.diff()


if __name__ == '__main__':
    main()

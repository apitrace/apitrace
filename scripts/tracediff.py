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


import difflib
import itertools
import optparse
import os.path
import platform
import shutil
import subprocess
import sys
import tempfile


##########################################################################/
#
# Abstract interface
#


class Differ:

    def __init__(self, apitrace):
        self.apitrace = apitrace
        self.isatty = sys.stdout.isatty()

    def setRefTrace(self, refTrace, ref_calls):
        raise NotImplementedError

    def setSrcTrace(self, srcTrace, src_calls):
        raise NotImplementedError

    def diff(self):
        raise NotImplementedError


##########################################################################/
#
# External diff tool
#


class AsciiDumper:

    def __init__(self, apitrace, trace, calls, callNos):
        self.output = tempfile.NamedTemporaryFile()

        dump_args = [
            apitrace,
            'dump',
            '--color=never',
            '--call-nos=' + ('yes' if callNos else 'no'),
            '--arg-names=no',
            '--calls=' + calls,
            trace
        ]

        self.dump = subprocess.Popen(
            args = dump_args,
            stdout = self.output,
            universal_newlines = True,
        )


class ExternalDiffer(Differ):

    if platform.system() == 'Windows':
        start_delete = ''
        end_delete   = ''
        start_insert = ''
        end_insert   = ''
    else:
        start_delete = '\33[9m\33[31m'
        end_delete   = '\33[0m'
        start_insert = '\33[32m'
        end_insert   = '\33[0m'

    def __init__(self, apitrace, options):
        Differ.__init__(self, apitrace)
        tool = options.tool
        callNos = options.callNos

        self.diff_args = [tool]
        if tool == 'diff':
            self.diff_args += [
                '--speed-large-files',
            ]
            if self.isatty:
                if options.suppressCommonLines:
                    self.diff_args += ['--unchanged-line-format=']
                else:
                    self.diff_args += ['--unchanged-line-format=%l\n']
                self.diff_args += [
                    '--old-line-format=' + self.start_delete + '%l' + self.end_delete + '\n',
                    '--new-line-format=' + self.start_insert + '%l' + self.end_insert + '\n',
                ]
            else:
                if options.suppressCommonLines:
                    self.diff_args += ['--unchanged-line-format=']
                else:
                    self.diff_args += ['--unchanged-line-format=  %l\n']
                self.diff_args += [
                    '--old-line-format=- %l\n',
                    '--new-line-format=+ %l\n',
                ]
        elif tool == 'sdiff':
            if options.width is None:
                import curses
                curses.setupterm()
                options.width = curses.tigetnum('cols')
            self.diff_args += [
                '--width=%u' % options.width,
                '--speed-large-files',
            ]
        elif tool == 'wdiff':
            self.diff_args += [
                #'--terminal',
                '--avoid-wraps',
            ]
            if self.isatty:
                self.diff_args += [
                    '--start-delete=' + self.start_delete,
                    '--end-delete=' + self.end_delete,
                    '--start-insert=' + self.start_insert,
                    '--end-insert=' + self.end_insert,
                ]
        else:
            assert False
        self.callNos = callNos

    def setRefTrace(self, refTrace, ref_calls):
        self.ref_dumper = AsciiDumper(self.apitrace, refTrace, ref_calls, self.callNos)

    def setSrcTrace(self, srcTrace, src_calls):
        self.src_dumper = AsciiDumper(self.apitrace, srcTrace, src_calls, self.callNos)

    def diff(self):
        diff_args = self.diff_args + [
            self.ref_dumper.output.name,
            self.src_dumper.output.name,
        ]

        self.ref_dumper.dump.wait()
        self.src_dumper.dump.wait()

        less = None
        diff_stdout = None
        if self.isatty:
            try:
                less = subprocess.Popen(
                    args = ['less', '-FRXn'],
                    stdin = subprocess.PIPE
                )
            except OSError:
                pass
            else:
                diff_stdout = less.stdin

        diff = subprocess.Popen(
            args = diff_args,
            stdout = diff_stdout,
            universal_newlines = True,
        )

        diff.wait()

        if less is not None:
            less.stdin.close()
            less.wait()


##########################################################################/
#
# Python diff
#

from unpickle import Unpickler, Dumper, Rebuilder
from highlight import PlainHighlighter, LessHighlighter


ignoredFunctionNames = set([
    'glGetString',
    'glXGetClientString',
    'glXGetCurrentDisplay',
    'glXGetCurrentContext',
    'glXGetProcAddress',
    'glXGetProcAddressARB',
    'wglGetProcAddress',
])


class Blob:
    '''Data-less proxy for bytearrays, to save memory.'''

    def __init__(self, size, hash):
        self.size = size
        self.hash = hash

    def __repr__(self):
        return 'blob(%u)' % self.size

    def __eq__(self, other):
        return isinstance(other, Blob) and self.size == other.size and self.hash == other.hash

    def __hash__(self):
        return self.hash


class BlobReplacer(Rebuilder):
    '''Replace blobs with proxys.'''

    def visitByteArray(self, obj):
        return Blob(len(obj), hash(str(obj)))

    def visitCall(self, call):
        call.args = map(self.visit, call.args)
        call.ret = self.visit(call.ret)


class Loader(Unpickler):

    def __init__(self, stream):
        Unpickler.__init__(self, stream)
        self.calls = []
        self.rebuilder = BlobReplacer()

    def handleCall(self, call):
        if call.functionName not in ignoredFunctionNames:
            self.rebuilder.visitCall(call)
            self.calls.append(call)


class PythonDiffer(Differ):

    def __init__(self, apitrace, options):
        Differ.__init__(self, apitrace)
        self.a = None
        self.b = None
        if self.isatty:
            self.highlighter = LessHighlighter()
        else:
            self.highlighter = PlainHighlighter()
        self.delete_color = self.highlighter.red
        self.insert_color = self.highlighter.green
        self.callNos = options.callNos
        self.suppressCommonLines = options.suppressCommonLines
        self.aSpace = 0
        self.bSpace = 0
        self.dumper = Dumper()

    def setRefTrace(self, refTrace, ref_calls):
        self.a = self.readTrace(refTrace, ref_calls)

    def setSrcTrace(self, srcTrace, src_calls):
        self.b = self.readTrace(srcTrace, src_calls)

    def readTrace(self, trace, calls):
        p = subprocess.Popen(
            args = [
                self.apitrace,
                'pickle',
                '--symbolic',
                '--calls=' + calls,
                trace
            ],
            stdout = subprocess.PIPE,
        )

        parser = Loader(p.stdout)
        parser.parse()
        return parser.calls

    def diff(self):
        try:
            self._diff()
        except IOError:
            pass

    def _diff(self):
        matcher = difflib.SequenceMatcher(self.isjunk, self.a, self.b)
        for tag, alo, ahi, blo, bhi in matcher.get_opcodes():
            if tag == 'replace':
                self.replace(alo, ahi, blo, bhi)
            elif tag == 'delete':
                self.delete(alo, ahi, blo, bhi)
            elif tag == 'insert':
                self.insert(alo, ahi, blo, bhi)
            elif tag == 'equal':
                self.equal(alo, ahi, blo, bhi)
            else:
                raise ValueError, 'unknown tag %s' % (tag,)

    def isjunk(self, call):
        return call.functionName == 'glGetError' and call.ret in ('GL_NO_ERROR', 0)

    def replace(self, alo, ahi, blo, bhi):
        assert alo < ahi and blo < bhi

        a_names = [call.functionName for call in self.a[alo:ahi]]
        b_names = [call.functionName for call in self.b[blo:bhi]]

        matcher = difflib.SequenceMatcher(None, a_names, b_names)
        for tag, _alo, _ahi, _blo, _bhi in matcher.get_opcodes():
            _alo += alo
            _ahi += alo
            _blo += blo
            _bhi += blo
            if tag == 'replace':
                self.replace_dissimilar(_alo, _ahi, _blo, _bhi)
            elif tag == 'delete':
                self.delete(_alo, _ahi, _blo, _bhi)
            elif tag == 'insert':
                self.insert(_alo, _ahi, _blo, _bhi)
            elif tag == 'equal':
                self.replace_similar(_alo, _ahi, _blo, _bhi)
            else:
                raise ValueError, 'unknown tag %s' % (tag,)

    def replace_similar(self, alo, ahi, blo, bhi):
        assert alo < ahi and blo < bhi
        assert ahi - alo == bhi - blo
        for i in xrange(0, bhi - blo):
            self.highlighter.write('| ')
            a_call = self.a[alo + i]
            b_call = self.b[blo + i]
            assert a_call.functionName == b_call.functionName
            self.dumpCallNos(a_call.no, b_call.no)
            self.highlighter.bold(True)
            self.highlighter.write(b_call.functionName)
            self.highlighter.bold(False)
            self.highlighter.write('(')
            sep = ''
            numArgs = max(len(a_call.args), len(b_call.args))
            for j in xrange(numArgs):
                self.highlighter.write(sep)
                try:
                    a_argName, a_argVal = a_call.args[j]
                except IndexError:
                    pass
                try:
                    b_argName, b_argVal = b_call.args[j]
                except IndexError:
                    pass
                self.replace_value(a_argName, b_argName)
                self.highlighter.write(' = ')
                self.replace_value(a_argVal, b_argVal)
                sep = ', '
            self.highlighter.write(')')
            if a_call.ret is not None or b_call.ret is not None:
                self.highlighter.write(' = ')
                self.replace_value(a_call.ret, b_call.ret)
            self.highlighter.write('\n')

    def replace_dissimilar(self, alo, ahi, blo, bhi):
        assert alo < ahi and blo < bhi
        if bhi - blo < ahi - alo:
            self.insert(alo, alo, blo, bhi)
            self.delete(alo, ahi, bhi, bhi)
        else:
            self.delete(alo, ahi, blo, blo)
            self.insert(ahi, ahi, blo, bhi)

    def replace_value(self, a, b):
        if b == a:
            self.highlighter.write(self.dumper.visit(b))
        else:
            self.highlighter.strike()
            self.highlighter.color(self.delete_color)
            self.highlighter.write(self.dumper.visit(a))
            self.highlighter.normal()
            self.highlighter.write(" -> ")
            self.highlighter.color(self.insert_color)
            self.highlighter.write(self.dumper.visit(b))
            self.highlighter.normal()

    escape = "\33["

    def delete(self, alo, ahi, blo, bhi):
        assert alo < ahi
        assert blo == bhi
        for i in xrange(alo, ahi):
            call = self.a[i]
            self.highlighter.write('- ')
            self.dumpCallNos(call.no, None)
            self.highlighter.strike()
            self.highlighter.color(self.delete_color)
            self.dumpCall(call)

    def insert(self, alo, ahi, blo, bhi):
        assert alo == ahi
        assert blo < bhi
        for i in xrange(blo, bhi):
            call = self.b[i]
            self.highlighter.write('+ ')
            self.dumpCallNos(None, call.no)
            self.highlighter.color(self.insert_color)
            self.dumpCall(call)

    def equal(self, alo, ahi, blo, bhi):
        if self.suppressCommonLines:
            return
        assert alo < ahi and blo < bhi
        assert ahi - alo == bhi - blo
        for i in xrange(0, bhi - blo):
            self.highlighter.write('  ')
            a_call = self.a[alo + i]
            b_call = self.b[blo + i]
            assert a_call.functionName == b_call.functionName
            assert len(a_call.args) == len(b_call.args)
            self.dumpCallNos(a_call.no, b_call.no)
            self.dumpCall(b_call)

    def dumpCallNos(self, aNo, bNo):
        if not self.callNos:
            return

        if aNo is not None and bNo is not None and aNo == bNo:
            aNoStr = str(aNo)
            self.highlighter.write(aNoStr)
            self.aSpace = len(aNoStr)
            self.bSpace = self.aSpace
            self.highlighter.write(' ')
            return

        if aNo is None:
            self.highlighter.write(' '*self.aSpace)
        else:
            aNoStr = str(aNo)
            self.highlighter.strike()
            self.highlighter.color(self.delete_color)
            self.highlighter.write(aNoStr)
            self.highlighter.normal()
            self.aSpace = len(aNoStr)
        self.highlighter.write(' ')
        if bNo is None:
            self.highlighter.write(' '*self.bSpace)
        else:
            bNoStr = str(bNo)
            self.highlighter.color(self.insert_color)
            self.highlighter.write(bNoStr)
            self.highlighter.normal()
            self.bSpace = len(bNoStr)
        self.highlighter.write(' ')

    def dumpCall(self, call):
        self.highlighter.bold(True)
        self.highlighter.write(call.functionName)
        self.highlighter.bold(False)
        self.highlighter.write('(' + self.dumper.visitItems(call.args) + ')')
        if call.ret is not None:
            self.highlighter.write(' = ' + self.dumper.visit(call.ret))
        self.highlighter.normal()
        self.highlighter.write('\n')



##########################################################################/
#
# Main program
#


def which(executable):
    '''Search for the executable on the PATH.'''

    if platform.system() == 'Windows':
        exts = ['.exe']
    else:
        exts = ['']
    dirs = os.environ['PATH'].split(os.path.pathsep)
    for dir in dirs:
        path = os.path.join(dir, executable)
        for ext in exts:
            if os.path.exists(path + ext):
                return True
    return False


def main():
    '''Main program.
    '''

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] TRACE TRACE',
        version='%%prog')
    optparser.add_option(
        '-a', '--apitrace', metavar='PROGRAM',
        type='string', dest='apitrace', default='apitrace',
        help='apitrace command [default: %default]')
    optparser.add_option(
        '-t', '--tool', metavar='TOOL',
        type="choice", choices=('diff', 'sdiff', 'wdiff', 'python'),
        dest="tool", default=None,
        help="diff tool: diff, sdiff, wdiff, or python [default: auto]")
    optparser.add_option(
        '-c', '--calls', metavar='CALLSET',
        type="string", dest="calls", default='0-10000',
        help="calls to compare [default: %default]")
    optparser.add_option(
        '--ref-calls', metavar='CALLSET',
        type="string", dest="refCalls", default=None,
        help="calls to compare from reference trace")
    optparser.add_option(
        '--src-calls', metavar='CALLSET',
        type="string", dest="srcCalls", default=None,
        help="calls to compare from source trace")
    optparser.add_option(
        '--call-nos',
        action="store_true",
        dest="callNos", default=False,
        help="dump call numbers")
    optparser.add_option(
        '--suppress-common-lines',
        action="store_true",
        dest="suppressCommonLines", default=False,
        help="do not output common lines")
    optparser.add_option(
        '-w', '--width', metavar='NUM',
        type="int", dest="width",
        help="columns [default: auto]")

    (options, args) = optparser.parse_args(sys.argv[1:])
    if len(args) != 2:
        optparser.error("incorrect number of arguments")

    if options.tool is None:
        if platform.system() == 'Windows':
            options.tool = 'python'
        else:
            if which('wdiff'):
                options.tool = 'wdiff'
            else:
                sys.stderr.write('warning: wdiff not found\n')
                if which('sdiff'):
                    options.tool = 'sdiff'
                else:
                    sys.stderr.write('warning: sdiff not found\n')
                    options.tool = 'diff'

    if options.refCalls is None:
        options.refCalls = options.calls
    if options.srcCalls is None:
        options.srcCalls = options.calls

    refTrace, srcTrace = args

    if options.tool == 'python':
        factory = PythonDiffer
    else:
        factory = ExternalDiffer
    differ = factory(options.apitrace, options)
    differ.setRefTrace(refTrace, options.refCalls)
    differ.setSrcTrace(srcTrace, options.srcCalls)
    differ.diff()


if __name__ == '__main__':
    main()

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


import platform
import optparse
import os.path
import shutil
import subprocess
import sys
import tempfile


class Dumper:

    def __init__(self, trace, calls):
        self.output = tempfile.NamedTemporaryFile()

        dump_args = [
            options.apitrace,
            'dump',
            '--color=never',
            '--call-nos=no',
            '--arg-names=no',
            '--calls=' + calls,
            trace
        ]

        self.dump = subprocess.Popen(
            args = dump_args,
            stdout = self.output,
            universal_newlines = True,
        )


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


def diff(ref_trace, src_trace):

    ref_dumper = Dumper(ref_trace, options.ref_calls)
    src_dumper = Dumper(src_trace, options.src_calls)

    # TODO use difflib instead
    if options.diff == 'diff':
        diff_args = [
                'diff',
                '--speed-large-files',
                '--old-line-format=' + start_delete + '%l' + end_delete + '\n',
                '--new-line-format=' + start_insert + '%l' + end_insert + '\n',
            ]
    elif options.diff == 'sdiff':
        diff_args = [
                'sdiff',
                '--width=%u' % options.width,
                '--speed-large-files',
            ]
    elif options.diff == 'wdiff':
        diff_args = [
                'wdiff',
                #'--terminal',
                '--avoid-wraps',
                '--start-delete=' + start_delete,
                '--end-delete=' + end_delete,
                '--start-insert=' + start_insert,
                '--end-insert=' + end_insert,
            ]
    else:
        assert False
    diff_args += [ref_dumper.output.name, src_dumper.output.name]

    ref_dumper.dump.wait()
    src_dumper.dump.wait()

    less = None
    if sys.stdout.isatty():
        less = subprocess.Popen(
            args = ['less', '-FRXn'],
            stdin = subprocess.PIPE
        )

        diff_stdout = less.stdin
    else:
        diff_stdout = None

    diff = subprocess.Popen(
        args = diff_args,
        stdout = diff_stdout,
        universal_newlines = True,
    )

    diff.wait()

    if less is not None:
        less.wait()


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


def columns():
    import curses
    curses.setupterm()
    return curses.tigetnum('cols')


def main():
    '''Main program.
    '''

    # Determine default options
    default_width = columns()

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] -- TRACE_FILE TRACE_FILE',
        version='%%prog')
    optparser.add_option(
        '-a', '--apitrace', metavar='PROGRAM',
        type='string', dest='apitrace', default='apitrace',
        help='apitrace command [default: %default]')
    optparser.add_option(
        '-d', '--diff',
        type="choice", choices=('diff', 'sdiff', 'wdiff'),
        dest="diff", default=None,
        help="diff program: wdiff, sdiff, or diff [default: auto]")
    optparser.add_option(
        '-c', '--calls', metavar='CALLSET',
        type="string", dest="calls", default='1-10000',
        help="calls to compare [default: %default]")
    optparser.add_option(
        '--ref-calls', metavar='CALLSET',
        type="string", dest="ref_calls", default=None,
        help="calls to compare from reference trace")
    optparser.add_option(
        '--src-calls', metavar='CALLSET',
        type="string", dest="src_calls", default=None,
        help="calls to compare from source trace")
    optparser.add_option(
        '-w', '--width', metavar='NUM',
        type="string", dest="width", default=default_width,
        help="columns [default: %default]")

    global options
    (options, args) = optparser.parse_args(sys.argv[1:])
    if len(args) != 2:
        optparser.error("incorrect number of arguments")

    if options.diff is None:
        if which('wdiff'):
            options.diff = 'wdiff'
        else:
            sys.stderr.write('warning: wdiff not found\n')
            if which('sdiff'):
                options.diff = 'sdiff'
            else:
                sys.stderr.write('warning: sdiff not found\n')
                options.diff = 'diff'

    if options.ref_calls is None:
        options.ref_calls = options.calls
    if options.src_calls is None:
        options.src_calls = options.calls

    diff(*args)


if __name__ == '__main__':
    main()

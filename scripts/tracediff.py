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
import os
import shutil
import subprocess
import sys
import tempfile
import time


def stripdump(trace, fifo):
    dump = subprocess.Popen(
        args = [
            options.apitrace,
            'dump',
            '--color=never',
            '--arg-names=no',
            '--calls=' + options.calls,
            trace
        ],
        stdout = subprocess.PIPE,
        universal_newlines = True,
    )

    sed = subprocess.Popen(
        args = [
            'sed',
            '-e', r's/\r$//g',
            '-e', r's/^[0-9]\+ //',
            '-e', r's/hdc = \w\+/hdc/g',
        ],
        stdin = dump.stdout,
        stdout = open(fifo, 'wt'),
        universal_newlines = True,
    )

    # XXX: Avoid a weird race condition
    time.sleep(0.01)


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


def diff(traces):
    fifodir = tempfile.mkdtemp()
    try:
        fifos = []
        for i in range(len(traces)):
            trace = traces[i]
            fifo = os.path.join(fifodir, str(i))
            stripdump(trace, fifo)
            fifos.append(fifo)

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
        diff_args += fifos
            
        diff = subprocess.Popen(
            args = diff_args,
            stdout = subprocess.PIPE,
            universal_newlines = True,
        )

        less = subprocess.Popen(
            args = ['less', '-FRXn'],
            stdin = diff.stdout
        )

        less.wait()

    finally:
        shutil.rmtree(fifodir)


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

    if which('wdiff'):
        default_diff = 'wdiff'
    elif which('sdiff'):
        default_diff = 'sdiff'
    else:
        default_diff = 'diff'

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
        dest="diff", default=default_diff,
        help="diff program: diff, sdiff, or wdiff [default: %default]")
    optparser.add_option(
        '-c', '--calls', metavar='CALLSET',
        type="string", dest="calls", default='1-10000',
        help="calls to compare [default: %default]")
    optparser.add_option(
        '-w', '--width', metavar='NUM',
        type="string", dest="width", default=default_width,
        help="columns [default: %default]")

    global options
    (options, args) = optparser.parse_args(sys.argv[1:])
    if len(args) != 2:
        optparser.error("incorrect number of arguments")

    diff(args)


if __name__ == '__main__':
    main()

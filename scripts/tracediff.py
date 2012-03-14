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
        sdiff = subprocess.Popen(
            args = [
                'sdiff',
                '--width=%u' % options.width,
                '--speed-large-files',
            ] + fifos,
            stdout = subprocess.PIPE,
            universal_newlines = True,
        )

        less = subprocess.Popen(
            args = ['less', '-FRXn'],
            stdin = sdiff.stdout
        )

        less.wait()

    finally:
        shutil.rmtree(fifodir)


def columns():
    import curses
    curses.setupterm()
    return curses.tigetnum('cols')


def main():
    '''Main program.
    '''

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

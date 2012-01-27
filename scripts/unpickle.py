#!/usr/bin/env python
##########################################################################
#
# Copyright 2012 Jose Fonseca
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

'''Sample program for apitrace pickle command.

Run as:

   apitrace pickle foo.trace | python unpickle.py

'''


import optparse
import cPickle as pickle
import sys
import time


def main():
    optparser = optparse.OptionParser(
        usage="\n\tapitrace pickle trace. %prog [options]")
    optparser.add_option(
        '--quiet',
        action="store_true", dest="quiet", default=False,
        help="don't dump calls to stdout")

    (options, args) = optparser.parse_args(sys.argv[1:])

    if args:
        optparser.error('unexpected arguments')

    # Change stdin to binary mode
    try:
        import msvcrt
    except ImportError:
        pass
    else:
        import os
        msvcrt.setmode(sys.stdin.fileno(), os.O_BINARY)

    calls = 0
    startTime = time.time()
    while True:
        try:
            call = pickle.load(sys.stdin)
        except EOFError:
            break
        else:
            callNo, functionName, args, ret = call
            if not options.quiet:
                sys.stdout.write('%u ' % callNo)
                sys.stdout.write(functionName)
                sys.stdout.write('(' + ', '.join(map(repr, args)) + ')')
                if ret is not None:
                    sys.stdout.write(' = ')
                    sys.stdout.write(repr(ret))
                sys.stdout.write('\n')
            calls += 1
    stopTime = time.time()
    duration = stopTime - startTime
    sys.stderr.write('%u calls, %.03f secs, %u calls/sec\n' % (calls, duration, calls/duration))


if __name__ == '__main__':
    main()

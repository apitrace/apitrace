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


import cPickle as pickle
import sys
import time


def main():
    calls = 0
    startTime = time.time()
    while True:
        try:
            call = pickle.load(sys.stdin)
        except EOFError:
            break
        else:
            callNo, functionName, args, ret = call
            if False:
                sys.stdout.write('%u %s%r = %r\n' % (callNo, functionName, tuple(args), ret))
            calls += 1
    stopTime = time.time()
    duration = stopTime - startTime
    sys.stderr.write('%u calls, %.03f secs, %u class/sec\n' % (calls, duration, calls/duration))


if __name__ == '__main__':
    main()

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


class Call:

    def __init__(self, callTuple):
        self.no, self.functionName, self.args, self.ret = callTuple

    def __str__(self):
        s = self.functionName
        if self.no is not None:
            s = str(self.no) + ' ' + s
        s += '(' + ', '.join(map(repr, self.args)) + ')'
        if self.ret is not None:
            s += ' = '
            s += repr(self.ret)
        return s

    def __eq__(self, other):
        return \
            self.functionName == other.functionName and \
            self.args == other.args and \
            self.ret == other.ret

    def __hash__(self):
        # XXX: hack due to unhashable types
        #return hash(self.functionName) ^ hash(tuple(self.args)) ^ hash(self.ret)
        return hash(self.functionName) ^ hash(repr(self.args)) ^ hash(repr(self.ret))



class Unpickler:

    callFactory = Call

    def __init__(self, stream):
        self.stream = stream

    def parse(self):
        while self.parseCall():
            pass

    def parseCall(self):
        try:
            callTuple = pickle.load(self.stream)
        except EOFError:
            return False
        else:
            call = self.callFactory(callTuple)
            self.handleCall(call)
            return True

    def handleCall(self, call):
        pass


class Counter(Unpickler):

    def __init__(self, stream, quiet):
        Unpickler.__init__(self, stream)
        self.quiet = quiet
        self.calls = 0

    def handleCall(self, call):
        if not self.quiet:
            sys.stdout.write(str(call))
            sys.stdout.write('\n')
        self.calls += 1


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

    startTime = time.time()
    parser = Counter(sys.stdin, options.quiet)
    parser.parse()
    stopTime = time.time()
    duration = stopTime - startTime
    sys.stderr.write('%u calls, %.03f secs, %u calls/sec\n' % (parser.calls, duration, parser.calls/duration))


if __name__ == '__main__':
    main()

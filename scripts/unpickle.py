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


import itertools
import optparse
import sys
import time
import re
import cPickle as pickle


class Visitor:

    def __init__(self):
        self.dispatch = {}
        self.dispatch[type(None)] = self.visitNone
        self.dispatch[bool] = self.visitBool
        self.dispatch[int] = self.visitInt
        self.dispatch[long] = self.visitInt
        self.dispatch[float] = self.visitFloat
        self.dispatch[str] = self.visitStr
        self.dispatch[tuple] = self.visitTuple
        self.dispatch[list] = self.visitList
        self.dispatch[dict] = self.visitDict
        self.dispatch[bytearray] = self.visitByteArray

    def visit(self, obj):
        method = self.dispatch.get(type(obj), self.visitObj)
        return method(obj)

    def visitObj(self, obj):
        raise NotImplementedError

    def visitAtom(self, obj):
        return self.visitObj(obj)

    def visitNone(self, obj):
        return self.visitAtom(obj)

    def visitBool(self, obj):
        return self.visitAtom(obj)

    def visitInt(self, obj):
        return self.visitAtom(obj)

    def visitFloat(self, obj):
        return self.visitAtom(obj)

    def visitStr(self, obj):
        return self.visitAtom(obj)

    def visitIterable(self, obj):
        return self.visitObj(obj)

    def visitTuple(self, obj):
        return self.visitIterable(obj)

    def visitList(self, obj):
        return self.visitIterable(obj)

    def visitDict(self, obj):
        raise NotImplementedError

    def visitByteArray(self, obj):
        raise NotImplementedError


class Dumper(Visitor):

    id_re = re.compile('^[_A-Za-z][_A-Za-z0-9]*$')

    def visitObj(self, obj):
        return repr(obj)

    def visitStr(self, obj):
        if self.id_re.match(obj):
            return obj
        else:
            return repr(obj)

    def visitTuple(self, obj):
        return '[' + ', '.join(itertools.imap(self.visit, obj)) + ']'

    def visitList(self, obj):
        return '(' + ', '.join(itertools.imap(self.visit, obj)) + ')'

    def visitByteArray(self, obj):
        return 'blob(%u)' % len(obj)


class Hasher(Visitor):
    '''Returns a hashable version of the objtree.'''

    def visitObj(self, obj):
        return obj

    def visitAtom(self, obj):
        return obj

    def visitIterable(self, obj):
        return tuple(itertools.imap(self.visit, obj))

    def visitByteArray(self, obj):
        return str(obj)


class Rebuilder(Visitor):
    '''Returns a hashable version of the objtree.'''

    def visitAtom(self, obj):
        return obj

    def visitIterable(self, obj):
        changed = False
        newItems = []
        for oldItem in obj:
            newItem = self.visit(oldItem)
            if newItem is not oldItem:
                changed = True
            newItems.append(newItem)
        if changed:
            klass = type(obj)
            return klass(newItems)
        else:
            return obj

    def visitByteArray(self, obj):
        return obj


class Call:

    def __init__(self, callTuple):
        self.no, self.functionName, self.args, self.ret = callTuple
        self._hash = None

    def __str__(self):
        s = self.functionName
        if self.no is not None:
            s = str(self.no) + ' ' + s
        dumper = Dumper()
        s += '(' + ', '.join(itertools.imap(dumper.visit, self.args)) + ')'
        if self.ret is not None:
            s += ' = '
            s += dumper.visit(self.ret)
        return s

    def __eq__(self, other):
        return \
            self.functionName == other.functionName and \
            self.args == other.args and \
            self.ret == other.ret

    def __hash__(self):
        if self._hash is None:
            hasher = Hasher()
            hashable = hasher.visit(self.functionName), hasher.visit(self.args), hasher.visit(self.ret)
            self._hash = hash(hashable)
        return self._hash


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

    def __init__(self, stream, verbose = False):
        Unpickler.__init__(self, stream)
        self.verbose = verbose
        self.numCalls = 0
        self.functionFrequencies = {}

    def parse(self):
        Unpickler.parse(self)

        functionFrequencies = self.functionFrequencies.items()
        functionFrequencies.sort(lambda (name1, freq1), (name2, freq2): cmp(freq1, freq2))
        for name, frequency in functionFrequencies:
            sys.stdout.write('%8u %s\n' % (frequency, name))

    def handleCall(self, call):
        if self.verbose:
            sys.stdout.write(str(call))
            sys.stdout.write('\n')
        self.numCalls += 1
        try:
            self.functionFrequencies[call.functionName] += 1
        except KeyError:
            self.functionFrequencies[call.functionName] = 1


def main():
    optparser = optparse.OptionParser(
        usage="\n\tapitrace pickle <trace> | %prog [options]")
    optparser.add_option(
        '-p', '--profile',
        action="store_true", dest="profile", default=False,
        help="profile call parsing")
    optparser.add_option(
        '-v', '--verbose',
        action="store_true", dest="verbose", default=False,
        help="dump calls to stdout")

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
    parser = Counter(sys.stdin, options.verbose)
    parser.parse()
    stopTime = time.time()
    duration = stopTime - startTime

    if options.profile:
        sys.stderr.write('Processed %u calls in %.03f secs, at %u calls/sec\n' % (parser.numCalls, duration, parser.numCalls/duration))


if __name__ == '__main__':
    main()

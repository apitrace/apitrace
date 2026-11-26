#!/usr/bin/env python3
##########################################################################
#
# Copyright 2026 Broadcom
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

'''Generates the fixup-*.trace test traces.

These mimic traces recorded by old versions of the tracer, whose client memory
blobs are smaller than what drivers may read, which a current tracer can't
produce.  The traces are written in the (gzip) legacy container, using the trace
format described in docs/FORMAT.markdown.

Usage: mkfixup.py [OUTPUT_DIR]
'''


import gzip
import os
import struct
import sys


TRACE_VERSION = 6

EVENT_ENTER = 0
EVENT_LEAVE = 1

CALL_END = 0
CALL_ARG = 1
CALL_RET = 2
CALL_FLAGS = 5

TYPE_NULL = 0
TYPE_FALSE = 1
TYPE_TRUE = 2
TYPE_SINT = 3
TYPE_UINT = 4
TYPE_BLOB = 8
TYPE_ENUM = 9
TYPE_ARRAY = 11

FLAG_FAKE = 1


GL = {
    'GL_TRIANGLES': 0x0004,
    'GL_FLOAT': 0x1406,
    'GL_UNSIGNED_INT': 0x1405,
    'GL_UNSIGNED_BYTE': 0x1401,
    'GL_TEXTURE_2D': 0x0DE1,
    'GL_RGB': 0x1907,
    'GL_RGB8': 0x8051,
    'GL_UNPACK_ALIGNMENT': 0x0CF5,
    'GL_MAX_TEXTURE_SIZE': 0x0D33,
}


class Blob:
    def __init__(self, size, fill=0):
        self.data = bytes((fill + i) & 0xff for i in range(size))


class Enum:
    def __init__(self, name):
        self.name = name
        self.value = GL[name]


class Writer:

    def __init__(self):
        self.out = bytearray()
        self.functions = {}
        self.enums = {}
        self.callNo = 0

    def uint(self, value):
        while True:
            byte = value & 0x7f
            value >>= 7
            if value:
                self.out.append(byte | 0x80)
            else:
                self.out.append(byte)
                break

    def string(self, s):
        data = s.encode('utf-8')
        self.uint(len(data))
        self.out += data

    def sint(self, value):
        if value < 0:
            self.out.append(TYPE_SINT)
            self.uint(-value)
        else:
            self.out.append(TYPE_UINT)
            self.uint(value)

    def header(self):
        self.uint(TRACE_VERSION)
        self.uint(TRACE_VERSION)
        self.string('')  # no properties

    def value(self, value):
        if value is None:
            self.out.append(TYPE_NULL)
        elif isinstance(value, bool):
            self.out.append(TYPE_TRUE if value else TYPE_FALSE)
        elif isinstance(value, int):
            self.sint(value)
        elif isinstance(value, Enum):
            self.out.append(TYPE_ENUM)
            sigId = self.enums.get(value.name)
            if sigId is None:
                sigId = len(self.enums)
                self.enums[value.name] = sigId
                self.uint(sigId)
                self.uint(1)
                self.string(value.name)
                self.sint(value.value)
            else:
                self.uint(sigId)
            self.sint(value.value)
        elif isinstance(value, Blob):
            self.out.append(TYPE_BLOB)
            self.uint(len(value.data))
            self.out += value.data
        elif isinstance(value, list):
            self.out.append(TYPE_ARRAY)
            self.uint(len(value))
            for element in value:
                self.value(element)
        else:
            raise TypeError(value)

    def enter(self, name, argNames, args, fake=False):
        '''Writes the enter event, returning the call number.  args are the
        input arguments, given as an {index: value} dictionary.'''
        self.out.append(EVENT_ENTER)
        self.uint(0)  # thread
        sigId = self.functions.get(name)
        if sigId is None:
            sigId = len(self.functions)
            self.functions[name] = sigId
            self.uint(sigId)
            self.string(name)
            self.uint(len(argNames))
            for argName in argNames:
                self.string(argName)
        else:
            self.uint(sigId)
        if fake:
            self.out.append(CALL_FLAGS)
            self.uint(FLAG_FAKE)
        for index in sorted(args):
            self.out.append(CALL_ARG)
            self.uint(index)
            self.value(args[index])
        self.out.append(CALL_END)
        callNo = self.callNo
        self.callNo += 1
        return callNo

    def leave(self, callNo, outArgs={}, ret=None, hasRet=False):
        self.out.append(EVENT_LEAVE)
        self.uint(callNo)
        for index in sorted(outArgs):
            self.out.append(CALL_ARG)
            self.uint(index)
            self.value(outArgs[index])
        if hasRet:
            self.out.append(CALL_RET)
            self.value(ret)
        self.out.append(CALL_END)

    def call(self, name, argNames, args, fake=False):
        callNo = self.enter(name, argNames, dict(enumerate(args)), fake)
        self.leave(callNo)
        return callNo

    def save(self, path):
        # mtime=0 for reproducible output
        with open(path, 'wb') as f:
            f.write(gzip.compress(bytes(self.out), mtime=0))


def vertexAttribPointer(w, size):
    w.call('glVertexAttribPointer',
           ['index', 'size', 'type', 'normalized', 'stride', 'pointer'],
           [0, 3, Enum('GL_FLOAT'), False, 0, Blob(size)],
           fake=True)


def drawRangeElements(w, end):
    w.call('glDrawRangeElements',
           ['mode', 'start', 'end', 'count', 'type', 'indices'],
           [Enum('GL_TRIANGLES'), 0, end, 3, Enum('GL_UNSIGNED_INT'), Blob(12)])


def texImage2D(w, width, height, size):
    w.call('glTexImage2D',
           ['target', 'level', 'internalformat', 'width', 'height', 'border',
            'format', 'type', 'pixels'],
           [Enum('GL_TEXTURE_2D'), 0, Enum('GL_RGB8'), width, height, 0,
            Enum('GL_RGB'), Enum('GL_UNSIGNED_BYTE'), Blob(size, 1)])


def pixelStorei(w, param):
    w.call('glPixelStorei', ['pname', 'param'], [Enum('GL_UNPACK_ALIGNMENT'), param])


def drawRange():
    w = Writer()
    w.header()

    # A call that encloses another one (e.g., a wrapper calling the real one),
    # and a call with an output argument, to check that the order of the events
    # and where the arguments are written are preserved.
    outer = w.enter('glFlush', [], {})
    w.call('glFinish', [], [])
    w.leave(outer)

    query = w.enter('glGetIntegerv', ['pname', 'params'], {0: Enum('GL_MAX_TEXTURE_SIZE')})
    w.leave(query, {1: [4096]})

    # Array one vertex short of the range: end=3 needs 4 vertices.
    vertexAttribPointer(w, 36)
    drawRangeElements(w, 3)

    # Large enough already
    vertexAttribPointer(w, 48)
    drawRangeElements(w, 3)

    # Not a range
    vertexAttribPointer(w, 36)
    drawRangeElements(w, 0xffffffff)

    # The pointer calls of a draw call that doesn't have a range are left alone
    vertexAttribPointer(w, 36)
    w.call('glDrawElements', ['mode', 'count', 'type', 'indices'],
           [Enum('GL_TRIANGLES'), 3, Enum('GL_UNSIGNED_INT'), Blob(12)])

    # Range only seen two calls after the pointer
    vertexAttribPointer(w, 36)
    w.call('glFlush', [], [])
    drawRangeElements(w, 3)

    return w


def unpackAlignment():
    w = Writer()
    w.header()

    # Default alignment (4): 2x2 RGB rows are 6 bytes, padded to 8, so 14 bytes
    texImage2D(w, 2, 2, 12)

    # Already large enough
    texImage2D(w, 2, 2, 14)

    # Tightly packed is right with alignment 1
    pixelStorei(w, 1)
    texImage2D(w, 2, 2, 12)

    # And is not with alignment 8
    pixelStorei(w, 8)
    texImage2D(w, 2, 2, 12)

    # Not tightly packed, so unknown how to fix
    texImage2D(w, 2, 2, 13)

    return w


def main():
    outDir = sys.argv[1] if len(sys.argv) > 1 else '.'
    drawRange().save(os.path.join(outDir, 'fixup-draw-range.trace'))
    unpackAlignment().save(os.path.join(outDir, 'fixup-unpack-alignment.trace'))


if __name__ == '__main__':
    main()

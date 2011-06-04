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


import json
import sys


def object_hook(obj):
    if '__class__' in obj:
        return None
    for name in obj.keys():
        if name.startswith('__') and name.endswith('__'):
            del obj[name]
    return obj


class Visitor:

    def visit(self, node, *args, **kwargs):
        if isinstance(node, dict):
            return self.visit_object(node, *args, **kwargs)
        elif isinstance(node, list):
            return self.visit_array(node, *args, **kwargs)
        else:
            return self.visit_value(node, *args, **kwargs)

    def visit_object(self, node, *args, **kwargs):
        pass

    def visit_array(self, node, *args, **kwargs):
        pass

    def visit_value(self, node, *args, **kwargs):
        pass


class Dumper(Visitor):

    def __init__(self, stream = sys.stdout):
        self.stream = stream
        self.level = 0;

    def _write(self, s):
        self.stream.write(s)

    def _indent(self):
        self._write('  '*self.level)

    def _newline(self):
        self._write('\n')

    def visit_object(self, node):
        self.enter_object()

        members = node.keys()
        members.sort()
        for i in range(len(members)):
            name = members[i]
            value = node[name]
            self.enter_member(name)
            self.visit(value)
            if i:
                self._write(',')
            self.leave_member()
        self.leave_object()

    def enter_object(self):
        self._write('{')
        self._newline()
        self.level += 1

    def enter_member(self, name):
        self._indent()
        self._write('%s: ' % name)

    def leave_member(self):
        self._newline()

    def leave_object(self):
        self.level -= 1
        self._indent()
        self._write('}')

    def visit_array(self, node):
        self.enter_array()
        for i in range(len(node)):
            value = node[i]
            self._indent()
            self.visit(value)
            if i:
                self._write(',')
            self._newline()
        self.leave_array()

    def enter_array(self):
        self._write('[')
        self._newline()
        self.level += 1

    def leave_array(self):
        self.level -= 1
        self._indent()
        self._write(']')

    def visit_value(self, node):
        self._write(repr(node))



class Comparer(Visitor):

    def visit_object(self, a, b):
        if not isinstance(b, dict):
            return False
        if len(a) != len(b):
            return False
        ak = a.keys()
        bk = b.keys()
        ak.sort()
        bk.sort()
        if ak != bk:
            return False
        for k in ak:
            if not self.visit(a[k], b[k]):
                return False
        return True

    def visit_array(self, a, b):
        if not isinstance(b, list):
            return False
        if len(a) != len(b):
            return False
        for ae, be in zip(a, b):
            if not self.visit(ae, be):
                return False
        return True

    def visit_value(self, a, b):
        return a == b

comparer = Comparer()


class Differ(Visitor):

    def __init__(self, stream = sys.stdout):
        self.dumper = Dumper(stream)

    def visit(self, a, b):
        if comparer.visit(a, b):
            return
        Visitor.visit(self, a, b)

    def visit_object(self, a, b):
        if not isinstance(b, dict):
            self.replace(a, b)
        else:
            self.dumper.enter_object()
            names = set(a.keys())
            names.update(b.keys())
            names = list(names)
            names.sort()

            for name in names:
                ae = a.get(name, None)
                be = b.get(name, None)
                if not comparer.visit(ae, be):
                    self.dumper.enter_member(name)
                    self.visit(ae, be)
                    self.dumper.leave_member()

            self.dumper.leave_object()

    def visit_array(self, a, b):
        if not isinstance(b, list):
            self.replace(a, b)
        else:
            self.dumper.enter_array()
            for i in range(max(len(a), len(b))):
                try:
                    ae = a[i]
                except IndexError:
                    ae = None
                try:
                    be = b[i]
                except IndexError:
                    be = None
                self.dumper._indent()
                if comparer.visit(ae, be):
                    self.dumper.visit(ae)
                else:
                    self.visit(ae, be)
                self.dumper._newline()

            self.dumper.leave_array()

    def visit_value(self, a, b):
        if a != b:
            self.replace(a, b)

    def replace(self, a, b):
        self.dumper.visit(a)
        self.dumper._write(' -> ')
        self.dumper.visit(b)


def load(stream):
    return json.load(stream, strict=False, object_hook = object_hook)


def main():
    a = load(open(sys.argv[1], 'rt'))
    b = load(open(sys.argv[2], 'rt'))

    #dumper = Dumper()
    #dumper.visit(a)

    differ = Differ()
    differ.visit(a, b)


if __name__ == '__main__':
    main()

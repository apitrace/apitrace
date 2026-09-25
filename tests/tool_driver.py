#!/usr/bin/env python3
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

'''Tool test driver.'''


import os.path
import subprocess
import sys


from base_driver import *


class AsciiComparer:

    def __init__(self, srcStream, refStream, verbose=False):
        self.srcStream = srcStream
        self.refStream = refStream
    
    def readLines(self, stream):
        lines = []
        for line in stream:
            line = line[:-1]
            lines.append(line)
        return lines

    def compare(self):
        refLines = self.readLines(self.refStream)
        srcLines = self.readLines(self.srcStream)

        numLines = max(len(refLines), len(srcLines))

        for lineNo in range(numLines):
            try:
                refLine = refLines[lineNo]
            except IndexError:
                fail('unexpected junk: %r' % srcLines[lineNo])

            try:
                srcLine = srcLines[lineNo]
            except IndexError:
                fail('unexpected EOF: %r expected' % refLine)

            if refLine != srcLine:
                fail('mismatch: expected %r but got %r' % (refLine ,srcLine))



class ToolDriver(Driver):

    def runScript(self, refScript):
        '''Run the application standalone, skipping this test if it fails by
        some reason.'''

        refStream = open(refScript, 'rt')
        cwd = os.path.dirname(os.path.abspath(refScript))

        while True:
            args = refStream.readline().split()
            cmd = [self.options.apitrace] + args

            if args[0] in ('dump', 'diff'):
                break

            p = popen(cmd, cwd=cwd, universal_newlines=True)
            p.wait()
            if p.returncode != 0:
                fail('`apitrace %s` returned code %i' % (args[0], p.returncode))
            
        p = popen(cmd, cwd=cwd, stdout=subprocess.PIPE, universal_newlines=True)

        comparer = AsciiComparer(p.stdout, refStream, self.options.verbose)
        comparer.compare()

        p.wait()
        if p.returncode != 0:
            fail('`apitrace %s` returned code %i' % (args[0], p.returncode))
    
    def run(self):
        self.parseOptions()

        for arg in self.args:
            self.runScript(arg)

        pass_()


if __name__ == '__main__':
    ToolDriver().run()

#!/usr/bin/env python
##########################################################################
#
# Copyright 2014-2016 VMware, Inc.
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


import subprocess
import sys
import os.path
import optparse

import unpickle


class ContextState:

    def __init__(self):
        self.textures = {}


class LeakDetector(unpickle.Unpickler):

    def __init__(self, apitrace, trace):

        cmd = [apitrace, 'pickle', '--symbolic', trace]
        p = subprocess.Popen(args = cmd, stdout = subprocess.PIPE)

        unpickle.Unpickler.__init__(self, p.stdout)

        self.context = ContextState()

    def parse(self):
        unpickle.Unpickler.parse(self)

        # Reached the end of the trace -- dump any live objects
        self.dumpLeaks("<EOF>", self.context)

    def handleCall(self, call):
        # Ignore calls without side effects
        if call.flags & unpickle.CALL_FLAG_NO_SIDE_EFFECTS:
            return

        # Dump call for debugging:
        if False:
            sys.stderr.write(str(call))

        # FIXME: keep track of current context on each thread (*MakeCurrent)
        context = self.context

        if call.functionName == 'glGenTextures':
            n, textures = call.argValues()
            for i in range(n):
                texture = textures[i]
                context.textures[texture] = call.no

        if call.functionName == 'glDeleteTextures':
            n, textures = call.argValues()
            for i in range(n):
                texture = textures[i]
                try:
                    del context.textures[texture]
                except KeyError:
                    # Ignore if texture name was never generated
                    pass

        if call.functionName in [
            'glXDestroyContext',
            'eglDestroyContext',
            'wglDeleteContext',
        ]:
            self.dumpLeaks(call.no, context)

    def dumpLeaks(self, callNo, context):
        for textureName, textureCallNo in context.textures.iteritems():
            sys.stderr.write('%u: error: texture %u was not destroyed until %s\n' % (textureCallNo, textureName, callNo))
        context.textures.clear()


def main():
    '''Main program.
    '''

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] TRACE',
        version='%%prog')
    optparser.add_option(
        '-a', '--apitrace', metavar='PROGRAM',
        type='string', dest='apitrace', default='apitrace',
        help='apitrace command [default: %default]')

    options, args = optparser.parse_args(sys.argv[1:])
    if len(args) != 1:
        optparser.error("incorrect number of arguments")

    inTrace = args[0]
    if not os.path.isfile(inTrace):
        sys.stderr.write("error: `%s` does not exist\n" % inTrace)
        sys.exit(1)

    detector = LeakDetector(options.apitrace, inTrace)
    detector.parse()


if __name__ == '__main__':
    main()

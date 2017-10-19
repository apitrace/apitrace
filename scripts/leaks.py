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
import re

import unpickle


class LeakDetector(unpickle.Unpickler):

    def __init__(self, apitrace, trace):

        cmd = [apitrace, 'pickle', '--symbolic', trace]
        p = subprocess.Popen(args = cmd, stdout = subprocess.PIPE)

        unpickle.Unpickler.__init__(self, p.stdout)

        self.numContexts = 0

        # a map of maps
        self.objectDicts = {}

    def parse(self):
        unpickle.Unpickler.parse(self)

        # Reached the end of the trace -- dump any live objects
        self.dumpLeaks("<EOF>")

    genDelRegExp = re.compile('^gl(Gen|Delete)(Buffers|Textures|FrameBuffers|RenderBuffers)[A-Z]*$')

    def handleCall(self, call):
        # Ignore calls without side effects
        if call.flags & unpickle.CALL_FLAG_NO_SIDE_EFFECTS:
            return

        # Dump call for debugging:
        if 0:
            sys.stderr.write('%s\n' % call)

        mo = self.genDelRegExp.match(call.functionName)
        if mo:
            verb = mo.group(1)
            subject = mo.group(2)

            subject = subject.lower().rstrip('s')
            objectDict = self.objectDicts.setdefault(subject, {})

            if verb == 'Gen':
                self.handleGenerate(call, objectDict)
            elif verb == 'Delete':
                self.handleDelete(call, objectDict)
            else:
                assert 0

        # TODO: Track labels via glObjectLabel* calls

        if call.functionName in [
            'CGLCreateContext',
            'eglCreateContext',
            'glXCreateContext',
            'glXCreateNewContext',
            'glXCreateContextAttribsARB',
            'glXCreateContextWithConfigSGIX',
            'wglCreateContext',
            'wglCreateContextAttribsARB',
        ]:
            # FIXME: Ignore failing context creation calls
            self.numContexts += 1

        if call.functionName in [
            'CGLDestroyContext',
            'glXDestroyContext',
            'eglDestroyContext',
            'wglDeleteContext',
        ]:
            assert self.numContexts > 0
            self.numContexts -= 1
            if self.numContexts == 0:
                self.dumpLeaks(call.no)

    def handleGenerate(self, call, objectDict):
        n, names = call.argValues()
        for i in range(n):
            name = names[i]
            objectDict[name] = call.no
            # TODO: Keep track of call stack backtrace too

    def handleDelete(self, call, objectDict):
        n, names = call.argValues()
        for i in range(n):
            name = names[i]
            try:
                del objectDict[name]
            except KeyError:
                # Ignore if texture name was never generated
                pass

    def dumpLeaks(self, currentCallNo):
        for kind, objectDict in self.objectDicts.iteritems():
            self.dumpNamespaceLeaks(currentCallNo, objectDict, kind)

    def dumpNamespaceLeaks(self, currentCallNo, objectDict, kind):
        for name, creationCallNo in (sorted(objectDict.iteritems(),key=lambda t: t[1])):
            sys.stderr.write('%u: error: %s %u was not destroyed until %s\n' % (creationCallNo, kind, name, currentCallNo))
        objectDict.clear()


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

#!/usr/bin/env python
##########################################################################
#
# Copyright 2014 VMware, Inc.
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

import unpickle



class Tracker(unpickle.Unpickler):

    def __init__(self, stream, inCallNo, inFrameNo):
        unpickle.Unpickler.__init__(self, stream)
        self.inCallNo = inCallNo
        self.inFrameNo = inFrameNo
        self.callNos = []
        self.callNo = None
        self.frameNo = 0

    def handleCall(self, call):
        if False:
            sys.stdout.write("%s\n" % call)
        else:
            if call.no % 10 == 0:
                sys.stdout.write("%s\r" % call.no)

        self.analyzeCall(call)

        if self.inCallNo is not None:
            if call.no >= self.inCallNo:
                self.callNo = call.no
                raise StopIteration

        if self.inFrameNo is not None:
            if call.flags & unpickle.CALL_FLAG_END_FRAME:
                if self.frameNo >= self.inFrameNo:
                    self.callNo = call.no
                    raise StopIteration
                self.frameNo += 1

    def analyzeCall(self, call):
        # Ignore calls without side effects
        if call.flags & unpickle.CALL_FLAG_NO_SIDE_EFFECTS:
            return

        self.callNos.append(call.no)


def track(apitrace, trace, callNo=None, frameNo=None):
    assert callNo is not None or frameNo is not None
    cmd = [
        apitrace,
        'pickle',
        '--symbolic',
    ]
    if callNo is not None:
        cmd.append('--calls=0-%u' % callNo)
    cmd.append(trace)

    p = subprocess.Popen(args = cmd, stdout = subprocess.PIPE)

    parser = Tracker(p.stdout, callNo, frameNo)
    parser.parse()

    return parser.callNos, parser.callNo



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


import optparse
import os.path
import platform
import subprocess
import sys
import tempfile
import json

import jsondiff
import unpickle



##########################################################################/
#
# State tracking
#


class Tracker(unpickle.Unpickler):

    def __init__(self, stream, inCallNo, inFrameNo):
        unpickle.Unpickler.__init__(self, stream)
        self.inCallNo = inCallNo
        self.inFrameNo = inFrameNo
        self.callNos = []
        self.callNo = None
        self.frameNo = 0

    def handleCall(self, call):
        if True:
            sys.stdout.write("%s\n" % call)
        self.callNos.append(call.no)

        if self.inCallNo is not None:
            if call.no >= self.inCallNo:
                self.callNo = call.no
                raise StopIteration

        if self.frameNo is not None:
            if call.flags & unpickle.CALL_FLAG_END_FRAME:
                if self.frameNo >= self.inFrameNo:
                    self.callNo = call.no
                    raise StopIteration
                self.frameNo += 1


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


##########################################################################/
#
# Verification
#


# Null file, to use when we're not interested in subprocesses output
if platform.system() == 'Windows':
    NULL = open('NUL:', 'wb')
else:
    NULL = open('/dev/null', 'wb')


def getState(apitrace, trace, callNo):
    sys.stdout.write('Obtaining state from %s, call %u\n' % (trace, callNo))
    sys.stdout.flush()
            
    cmd = [apitrace, 'retrace', '-D', str(callNo), trace]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=NULL)
    state = json.load(p.stdout, strict=False)
    p.wait()

    # Only care for framebuffer for now
    state = state['framebuffer']

    # Ensure its not empty
    assert len(state)

    return state


def verify(apitrace, inTrace, inCallNo, outTrace, outCallNo):
    inState = getState(apitrace, inTrace, inCallNo)
    outState = getState(apitrace, outTrace, outCallNo)

    comparer = jsondiff.Comparer(ignore_added = True)
    match = comparer.visit(inState, outState)
    if match:
        sys.stdout.write("States match.\n")
    else:
        sys.stdout.write("States do not match.\n")
        differ = Differ(sys.stdout)
        differ.visit(inState, outState)


##########################################################################/
#
# Main program
#


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
    optparser.add_option(
        '-c', '--call', metavar='CALLNO',
        type="string", dest="call", default=None,
        help="call to trim")
    optparser.add_option(
        '-f', '--frame', metavar='FRAMENO',
        type="string", dest="frame", default=None,
        help="frame to trim")
    optparser.add_option(
        '-o', '--output', metavar='TRACE',
        type="string", dest="output", default=None,
        help="output trace")
    optparser.add_option(
        '--no-verify',
        action="store_false", dest="verify", default=True,
        help="don't verify correctness")
    optparser.add_option(
        '--diff',
        action="store_true", dest="diff", default=False,
        help="diff traces at the end")

    (options, args) = optparser.parse_args(sys.argv[1:])
    if len(args) != 1:
        optparser.error("incorrect number of arguments")

    inTrace = args[0]
    if not os.path.isfile(inTrace):
        sys.stderr.write("error: `%s` does not exist\n" % inTrace)
        sys.exit(1)

    inCallNo = None
    inFrameNo = None
    if options.call is not None:
        inCallNo = int(options.call)
    elif options.frame is not None:
        inFrameNo = int(options.frame)
    else:
        optparser.error("--call or --frame must be given")

    if options.output is None:
        name, ext = os.path.splitext(os.path.basename(inTrace))
        outTrace = name + '-trim' + ext
    else:
        outTrace = options.output

    # Obtain the dependencies
    inCallNos, inCallNo = track(options.apitrace, inTrace, inCallNo, inFrameNo)

    # Do the actual trimming via `apitrace trim`
    # FIXME: Use a tempfile
    inCallSet = ','.join(map(str, inCallNos))
    subprocess.call([
        options.apitrace,
        'trim',
        '--exact',
        '--calls', inCallSet,
        '--output', outTrace,
        inTrace,
    ])

    # Verify that the state at the given call matches
    if options.verify:
        inCallNos.sort()
        outCallNo = inCallNos.index(inCallNo)

        verify(options.apitrace, inTrace, inCallNo, outTrace, outCallNo)

    # Compare the original and trimmed traces
    if options.diff:
        subprocess.call([
            options.apitrace,
            'diff',
            '--tool=python',
            '--call-nos',
            #'--ref-calls=0-%u' % inCallNo,
            #'--src-calls=0-%u' % outCallNo,
            inTrace,
            outTrace,
        ])


if __name__ == '__main__':
    main()

#!/usr/bin/env python
##########################################################################
#
# Copyright 2012 VMware Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/

'''Convert traces to/from PIX.
'''


import optparse
import os.path
import subprocess
import platform
import sys


def convert(inTrace, outPixrun):
    try:
        programFiles = os.environ['ProgramFiles(x86)']
    except KeyError:
        programFiles = os.environ['ProgramFiles']
    try:
        dxsdkDir = os.environ['DXSDK_DIR']
    except KeyError:
        dxsdkDir = os.path.join(programFiles, "Microsoft DirectX SDL (June 2010)")
    pix = os.path.join(dxsdkDir, "Utilities", "bin", 'x86', 'PIXwin.exe')

    pixExp = os.path.join(os.path.dirname(__file__), 'apitrace.PIXExp')

    # http://social.msdn.microsoft.com/Forums/sv/devdocs/thread/15addc0c-036d-413a-854a-35637ccbb834
    # http://src.chromium.org/svn/trunk/o3d/tests/test_driver.py
    cmd = [
        pix,
        pixExp,
        '-start',
        '-runfile', os.path.abspath(outPixrun),
        '-targetpath', os.path.abspath(options.retrace),
        #'-targetstartfolder', ...,
        '-targetargs', os.path.abspath(inTrace),
    ]

    if options.verbose:
        sys.stderr.write(' '.join(cmd) + '\n')

    ret = subprocess.call(cmd)
    if ret:
        sys.stderr.write('error: pix failued with exit code %u\n' % ret)
        sys.exit(ret)
    if os.path.exists(outPixrun):
        sys.stderr.write('info: %s written\n' % outPixrun)
        if False:
            subprocess.call([pix, os.path.abspath(outPixrun)])
    else:
        sys.stderr.write('error: %s not written\n' % outPixrun)
        sys.exit(1)


def main():
    global options

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] <trace> ...',
        version='%%prog')
    optparser.add_option(
        '-r', '--retrace', metavar='PROGRAM',
        type='string', dest='retrace', default='d3dretrace',
        help='retrace command [default: %default]')
    optparser.add_option(
        '-v', '--verbose',
        action='store_true', dest='verbose', default=False,
        help='verbose output')
    optparser.add_option(
        '-o', '--output', metavar='FILE',
        type="string", dest="output",
        help="output file [default: stdout]")

    (options, args) = optparser.parse_args(sys.argv[1:])
    if not args:
        optparser.error("incorrect number of arguments")
    
    for arg in args:
        if options.output:
            output = options.output
        else:
            name, ext = os.path.splitext(os.path.basename(arg))
            output = name + '.PIXRun'
        convert(arg, output)


if __name__ == '__main__':
    main()

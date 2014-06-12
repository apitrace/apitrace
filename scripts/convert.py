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


def getPixExe():
    try:
        programFiles = os.environ['ProgramFiles(x86)']
    except KeyError:
        programFiles = os.environ['ProgramFiles']
    try:
        dxsdkDir = os.environ['DXSDK_DIR']
    except KeyError:
        dxsdkDir = os.path.join(programFiles, "Microsoft DirectX SDL (June 2010)")
    pixExe = os.path.join(dxsdkDir, "Utilities", "bin", 'x86', 'PIXwin.exe')
    return pixExe


def callProcess(cmd):
    if options.verbose:
        sys.stderr.write(' '.join(cmd) + '\n')
    ret = subprocess.call(cmd)
    if ret:
        exeName = os.path.basename(cmd[0])
        sys.stderr.write('error: %s failed with exit code %u\n' % (exeName, ret))
        sys.exit(ret)
    return ret


def convertToPix(inTrace, outPixrun):
    pix = getPixExe()

    pixExp = os.path.join(os.path.dirname(__file__), 'apitrace.PIXExp')

    # http://social.msdn.microsoft.com/Forums/sv/devdocs/thread/15addc0c-036d-413a-854a-35637ccbb834
    # http://src.chromium.org/svn/trunk/o3d/tests/test_driver.py
    cmd = [
        getPixExe(),
        pixExp,
        '-start',
        '-runfile', os.path.abspath(outPixrun),
        '-targetpath', os.path.abspath(options.retrace),
        #'-targetstartfolder', ...,
        '-targetargs', os.path.abspath(inTrace),
    ]

    callProcess(cmd)
    if os.path.exists(outPixrun):
        sys.stderr.write('info: %s written\n' % outPixrun)
        if options.verify:
            subprocess.call([pix, os.path.abspath(outPixrun)])
    else:
        sys.stderr.write('error: %s not written\n' % outPixrun)
        sys.exit(1)


def convertFromPix(inPix, outTrace):
    pixExe = getPixExe()

    if False:
        # TODO: Use -exporttocsv option to detect which API to use
        cmd = [
            pixExe,
            inPix,
            '-exporttocsv', # XXX: output filename is ignored
        ]
        callProcess(cmd)

    cmd = [
        options.apitrace,
        'trace',
        '-a', options.api,
        '-o', outTrace,
        pixExe,
        inPix,
        '-playstandalone',
    ]

    callProcess(cmd)
    if os.path.exists(outTrace):
        sys.stderr.write('info: %s written\n' % outTrace)
        if options.verify:
            subprocess.call([options.retrace, os.path.abspath(outTrace)])
    else:
        sys.stderr.write('error: %s not written\n' % outTrace)
        sys.exit(1)


def main():
    global options

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] <trace> ...',
        version='%%prog')
    optparser.add_option(
        '--apitrace', metavar='PROGRAM',
        type='string', dest='apitrace', default='apitrace.exe',
        help='path to apitrace command [default: %default]')
    optparser.add_option(
        '-a', '--api', metavar='API',
        type='string', dest='api', default='d3d9',
        help='api [default: %default]')
    optparser.add_option(
        '-r', '--retrace', metavar='PROGRAM',
        type='string', dest='retrace', default='d3dretrace.exe',
        help='path to retrace command [default: %default]')
    optparser.add_option(
        '-v', '--verbose',
        action='store_true', dest='verbose', default=False,
        help='verbose output')
    optparser.add_option(
        '-o', '--output', metavar='FILE',
        type="string", dest="output",
        help="output file [default: stdout]")
    optparser.add_option(
        '--verify',
        action='store_true', dest='verify', default=False,
        help='verify output by replaying it')

    (options, args) = optparser.parse_args(sys.argv[1:])
    if not args:
        optparser.error("incorrect number of arguments")
    
    for inFile in args:
        name, inExt = os.path.splitext(os.path.basename(inFile))
        inExt = inExt
        if inExt.lower() == '.trace':
            convert = convertToPix
            outExt = '.PIXRun'
        elif inExt.lower() == '.pixrun':
            convert = convertFromPix
            outExt = '.trace'
        else:
            optparser.error("unexpected file extensions `%s`" % inExt)
        if options.output:
            outFile = options.output
        else:
            outFile = name + outExt
        convert(inFile, outFile)


if __name__ == '__main__':
    main()

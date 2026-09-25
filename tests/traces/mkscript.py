#!/usr/bin/env python3
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

'''Script generator'''


import optparse
import subprocess
import sys


def main():

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [OPTIONS] -- [ARGS] ...',
        version='%%prog')
    optparser.add_option(
        '--apitrace', metavar='PROGRAM',
        type='string', dest='apitrace', default='apitrace',
        help='path to apitrace executable')
    (options, args) = optparser.parse_args(sys.argv[1:])
    if not args:
        optparser.error('an argument must be specified')

    sys.stdout.write(' '.join(args) + '\n')
    sys.stdout.flush()
    cmd = [options.apitrace] + args
    p = subprocess.Popen(cmd)
    p.wait()
    sys.exit(p.returncode)


if __name__ == '__main__':
    main()

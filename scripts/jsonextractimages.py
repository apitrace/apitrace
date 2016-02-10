#!/usr/bin/env python
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


'''Simple script to extract PNG files from the JSON state dumps.'''


import json
import optparse
import base64
import sys


pngSignature = "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"


def dumpSurfaces(state, memberName):
    for name, imageObj in state[memberName].iteritems():
        data = imageObj['__data__']
        data = base64.b64decode(data)

        if data.startswith(pngSignature):
            extName = 'png'
        else:
            magic = data[:2]
            if magic in ('P1', 'P4'):
                extName = 'pbm'
            elif magic in ('P2', 'P5'):
                extName = 'pgm'
            elif magic in ('P3', 'P6'):
                extName = 'ppm'
            elif magic in ('Pf', 'PF'):
                extName = 'pfm'
            else:
                sys.stderr.write('warning: unsupport Netpbm format %s\n' % magic)
                continue

        imageName = '%s.%s' % (name, extName)
        open(imageName, 'wb').write(data)
        sys.stderr.write('Wrote %s\n' % imageName)


def main():
    optparser = optparse.OptionParser(
        usage="\n\t%prog [options] <json>")

    (options, args) = optparser.parse_args(sys.argv[1:])

    for arg in args:
        state = json.load(open(arg, 'rt'), strict=False)

        dumpSurfaces(state, 'textures')
        dumpSurfaces(state, 'framebuffer')



if __name__ == '__main__':
    main()

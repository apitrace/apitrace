#!/usr/bin/env python
##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
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


import sys
import os.path
import subprocess
import optparse

import Image


thumb_size = 320, 320


def compare(im, ref):
    import ImageMath
    # See http://www.pythonware.com/library/pil/handbook/imagemath.htm
    mask = ImageMath.eval("min(abs(a - b), 1)", a=im, b=ref)
    gray = ref.convert('L')
    # TODO


def surface(html, image):
    if False:
        html.write('        <td><a href="%s"><img src="%s"/></a></td>\n' % (image, image))
    else:
        name, ext = os.path.splitext(image)
        thumb = name + '_thumb' + ext
        if not os.path.exists(thumb) and os.path.exists(image):
            im = Image.open(image)
            im.thumbnail(thumb_size)
            im.save(thumb)
        html.write('        <td><a href="%s"><img src="%s"/></a></td>\n' % (image, thumb))


def main():
    optparser = optparse.OptionParser(
        usage="\n\t%prog [options] [file] ...",
        version="%%prog")
    optparser.add_option(
        '-o', '--output', metavar='FILE',
        type="string", dest="output",
        help="output filename [stdout]")
    optparser.add_option(
        '--start', metavar='FRAME',
        type="int", dest="start", default=1,
        help="start frame [default: %default]")
    optparser.add_option(
        '--stop', metavar='FRAME',
        type="int", dest="stop", default=9999,
        help="stop frame [default: %default]")
    optparser.add_option(
        '-f', '--fuzz',
        type="string", dest="fuzz", default='5%',
        help="fuzz [default: %default]")

    (options, args) = optparser.parse_args(sys.argv[1:])

    if len(args) != 2:
        optparser.error('incorrect number of arguments')

    ref_prefix = sys.argv[1]
    src_prefix = sys.argv[2]

    if options.output:
        html = open(options.output, 'wt')
    else:
        html = sys.stdout
    html.write('<html>\n')
    html.write('  <body>\n')
    html.write('    <table border="1">\n')
    html.write('      <tr><th><th>ref</th><th>src</th><th>&Delta;</th></tr>\n')
    for frame_no in range(options.start, options.stop + 1):
        ref_image = "%s%04u.png" % (ref_prefix, frame_no)
        src_image = "%s%04u.png" % (src_prefix, frame_no)
        delta_image = "%s%04u_diff.png" % (src_prefix, frame_no)
        if os.path.exists(ref_image) and os.path.exists(src_image):
            html.write('      <tr>\n')
            subprocess.call(["compare", '-metric', 'AE', '-fuzz', options.fuzz, ref_image, src_image, delta_image])
            surface(html, ref_image)
            surface(html, src_image)
            surface(html, delta_image)
            html.write('      </tr>\n')
            html.flush()
    html.write('    </table>\n')
    html.write('  </body>\n')
    html.write('</html>\n')


if __name__ == '__main__':
    main()

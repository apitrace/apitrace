#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 Jose Fonseca
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


'''Snapshot (image) comparison script.
'''


import sys
import os.path
import optparse
import math
import operator

import Image
import ImageChops
import ImageEnhance


thumb_size = 320, 320


def _compare(ref_image, src_image, delta_image):
    import subprocess
    p = subprocess.Popen([
        'compare', 
        '-metric', 'AE', 
        '-fuzz', '%u%%' % (100*options.fuzz,),
        '-dissimilarity-threshold', '1',
        ref_image, src_image, delta_image
    ], stderr=subprocess.PIPE)
    _, stderr = p.communicate()
    try:
        return int(stderr)
    except ValueError:
        return 0xffffffff


def compare(ref_image, src_image, delta_image):
    ref_im = Image.open(ref_image)
    src_im = Image.open(src_image)

    ref_im = ref_im.convert('RGB')
    src_im = src_im.convert('RGB')

    diff = ImageChops.difference(src_im, ref_im)

    mask = ImageEnhance.Brightness(diff).enhance(1.0/options.fuzz)
    mask = mask.convert('L')

    lowlight = Image.new('RGB', src_im.size, (0xff, 0xff, 0xff))
    highlight = Image.new('RGB', src_im.size, (0xf1, 0x00, 0x1e))
    delta_im = Image.composite(highlight, lowlight, mask)

    delta_im = Image.blend(src_im, delta_im, 0xcc/255.0)
    delta_im.save(delta_image)

    # See also http://effbot.org/zone/pil-comparing-images.htm
    # TODO: this is approximate due to the grayscale conversion
    h = diff.convert('L').histogram()
    ae = sum(h[int(255 * options.fuzz) + 1 : 256])
    return ae


def surface(html, image):
    if True:
        name, ext = os.path.splitext(image)
        thumb = name + '.thumb' + ext
        if os.path.exists(image) \
           and (not os.path.exists(thumb) \
                or os.path.getmtime(thumb) < os.path.getmtime(image)):
            im = Image.open(image)
            im.thumbnail(thumb_size)
            im.save(thumb)
    else:
        thumb = image
    html.write('        <td><a href="%s"><img src="%s"/></a></td>\n' % (image, thumb))


def is_image(path):
    return \
        path.endswith('.png') \
        and not path.endswith('.diff.png') \
        and not path.endswith('.thumb.png')


def find_images(prefix):
    prefix = os.path.abspath(prefix)
    if os.path.isdir(prefix):
        prefix_dir = prefix
    else:
        prefix_dir = os.path.dirname(prefix)

    images = []
    for dirname, dirnames, filenames in os.walk(prefix_dir, followlinks=True):
        for filename in filenames:
            filepath = os.path.join(dirname, filename)
            if filepath.startswith(prefix) and is_image(filepath):
                images.append(filepath[len(prefix):])

    return images


def main():
    global options

    optparser = optparse.OptionParser(
        usage="\n\t%prog [options] <ref_prefix> <src_prefix>",
        version="%%prog")
    optparser.add_option(
        '-o', '--output', metavar='FILE',
        type="string", dest="output", default='index.html',
        help="output filename [default: %default]")
    optparser.add_option(
        '-f', '--fuzz',
        type="float", dest="fuzz", default=.05,
        help="fuzz ratio [default: %default]")
    optparser.add_option(
        '--overwrite',
        action="store_true", dest="overwrite", default=False,
        help="overwrite")

    (options, args) = optparser.parse_args(sys.argv[1:])

    if len(args) != 2:
        optparser.error('incorrect number of arguments')

    ref_prefix = args[0]
    src_prefix = args[1]

    ref_images = find_images(ref_prefix)
    src_images = find_images(src_prefix)
    images = list(set(ref_images).intersection(set(src_images)))
    images.sort()

    if options.output:
        html = open(options.output, 'wt')
    else:
        html = sys.stdout
    html.write('<html>\n')
    html.write('  <body>\n')
    html.write('    <table border="1">\n')
    html.write('      <tr><th>%s</th><th>%s</th><th>&Delta;</th></tr>\n' % (ref_prefix, src_prefix))
    for image in images:
        ref_image = ref_prefix + image
        src_image = src_prefix + image
        root, ext = os.path.splitext(src_image)
        delta_image = "%s.diff.png" % (root, )
        if os.path.exists(ref_image) and os.path.exists(src_image):
            if options.overwrite \
               or not os.path.exists(delta_image) \
               or (os.path.getmtime(delta_image) < os.path.getmtime(ref_image) \
                   and os.path.getmtime(delta_image) < os.path.getmtime(src_image)):
                compare(ref_image, src_image, delta_image)

            html.write('      <tr>\n')
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

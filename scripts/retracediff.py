#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 Jose Fonseca
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

'''Run two retrace instances in parallel, comparing generated snapshots.
'''


import math
import optparse
import os.path
import subprocess
import platform
import sys

from PIL import Image

from snapdiff import Comparer
from highlight import AutoHighlighter
import jsondiff


# Null file, to use when we're not interested in subprocesses output
NULL = open(os.path.devnull, 'wb')


class RetraceRun:

    def __init__(self, process):
        self.process = process

    def nextSnapshot(self):
        image, comment = read_pnm(self.process.stdout)
        if image is None:
            return None, None

        callNo = int(comment.strip())

        return image, callNo

    def terminate(self):
        try:
            self.process.terminate()
        except OSError:
            # Avoid http://bugs.python.org/issue14252
            pass


class Retracer:

    def __init__(self, retraceExe, args, env=None):
        self.retraceExe = retraceExe
        self.args = args
        self.env = env

    def _retrace(self, args, stdout=subprocess.PIPE):
        cmd = [
            self.retraceExe,
        ] + args + self.args
        if self.env:
            for name, value in self.env.iteritems():
                sys.stderr.write('%s=%s ' % (name, value))
        sys.stderr.write(' '.join(cmd) + '\n')
        try:
            return subprocess.Popen(cmd, env=self.env, stdout=stdout, stderr=NULL)
        except OSError, ex:
            sys.stderr.write('error: failed to execute %s: %s\n' % (cmd[0], ex.strerror))
            sys.exit(1)

    def retrace(self, args):
        p = self._retrace([])
        p.wait()
        return p.returncode

    def snapshot(self, call_nos):
        process = self._retrace([
            '-s', '-',
            '-S', call_nos,
        ])
        return RetraceRun(process)

    def dump_state(self, call_no):
        '''Get the state dump at the specified call no.'''

        p = self._retrace([
            '-D', str(call_no),
        ])
        state = jsondiff.load(p.stdout)
        p.wait()
        return state.get('parameters', {})

    def diff_state(self, ref_call_no, src_call_no, stream):
        '''Compare the state between two calls.'''

        ref_state = self.dump_state(ref_call_no)
        src_state = self.dump_state(src_call_no)

        stream.flush()
        differ = jsondiff.Differ(stream)
        differ.visit(ref_state, src_state)
        stream.write('\n')


def read_pnm(stream):
    '''Read a PNM from the stream, and return the image object, and the comment.'''

    magic = stream.readline()
    if not magic:
        return None, None
    magic = magic.rstrip()
    if magic == 'P5':
        channels = 1
        bytesPerChannel = 1
        mode = 'L'
    elif magic == 'P6':
        channels = 3
        bytesPerChannel = 1
        mode = 'RGB'
    elif magic == 'Pf':
        channels = 1
        bytesPerChannel = 4
        mode = 'R'
    elif magic == 'PF':
        channels = 3
        bytesPerChannel = 4
        mode = 'RGB'
    elif magic == 'PX':
        channels = 4
        bytesPerChannel = 4
        mode = 'RGB'
    else:
        raise Exception('Unsupported magic `%s`' % magic)
    comment = ''
    line = stream.readline()
    while line.startswith('#'):
        comment += line[1:]
        line = stream.readline()
    width, height = map(int, line.strip().split())
    maximum = int(stream.readline().strip())
    if bytesPerChannel == 1:
        assert maximum == 255
    else:
        assert maximum == 1
    data = stream.read(height * width * channels * bytesPerChannel)
    if bytesPerChannel == 4:
        # Image magic only supports single channel floating point images, so
        # represent the image as numpy arrays

        import numpy
        pixels = numpy.fromstring(data, dtype=numpy.float32)
        pixels.resize((height, width, channels))
        return pixels, comment

    image = Image.frombuffer(mode, (width, height), data, 'raw', mode, 0, 1)
    return image, comment


def dumpNumpyImage(output, pixels, filename):
    height, width, channels = pixels.shape

    import numpy

    pixels = (pixels*255).clip(0, 255).astype('uint8')

    if 0:
        # XXX: Doesn't work somehow
        im = Image.fromarray(pixels)
    else:
        # http://code.activestate.com/recipes/577591-conversion-of-pil-image-and-numpy-array/
        pixels = pixels.reshape(height*width, channels)
        if channels == 4:
            mode = 'RGBA'
        else:
            if channels < 3:
                pixels = numpy.c_[arr, 255*numpy.ones((heigth * width, 3 - channels), numpy.uint8)]
            assert channels == 3
            mode = 'RGB'
        im = Image.frombuffer(mode, (width, height), pixels.tostring(), 'raw', mode, 0, 1)
    im.save(filename)

    if 0:
        # Dump to stdout
        for y in range(height):
            output.write('  ')
            for x in range(width):
                for c in range(channels):
                    output.write('%0.9g,' % pixels[y, x, c])
                output.write('  ')
            output.write('\n')


def parse_env(optparser, entries):
    '''Translate a list of NAME=VALUE entries into an environment dictionary.'''

    if not entries:
        return None

    env = os.environ.copy()
    for entry in entries:
        try:
            name, var = entry.split('=', 1)
        except Exception:
            optparser.error('invalid environment entry %r' % entry)
        env[name] = var
    return env


def main():
    '''Main program.
    '''

    global options

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] -- [glretrace options] <trace>',
        version='%%prog')
    optparser.add_option(
        '-r', '--retrace', metavar='PROGRAM',
        type='string', dest='retrace', default='glretrace',
        help='retrace command [default: %default]')
    optparser.add_option(
        '--ref-driver', metavar='DRIVER',
        type='string', dest='ref_driver', default=None,
        help='force reference driver')
    optparser.add_option(
        '--src-driver', metavar='DRIVER',
        type='string', dest='src_driver', default=None,
        help='force source driver')
    optparser.add_option(
        '--ref-arg', metavar='OPTION',
        type='string', action='append', dest='ref_args', default=[],
        help='pass argument to reference retrace')
    optparser.add_option(
        '--src-arg', metavar='OPTION',
        type='string', action='append', dest='src_args', default=[],
        help='pass argument to source retrace')
    optparser.add_option(
        '--ref-env', metavar='NAME=VALUE',
        type='string', action='append', dest='ref_env', default=[],
        help='add variable to reference environment')
    optparser.add_option(
        '--src-env', metavar='NAME=VALUE',
        type='string', action='append', dest='src_env', default=[],
        help='add variable to source environment')
    optparser.add_option(
        '--diff-prefix', metavar='PATH',
        type='string', dest='diff_prefix', default='.',
        help='prefix for the difference images')
    optparser.add_option(
        '-t', '--threshold', metavar='BITS',
        type="float", dest="threshold", default=12.0,
        help="threshold precision  [default: %default]")
    optparser.add_option(
        '-S', '--snapshot-frequency', metavar='CALLSET',
        type="string", dest="snapshot_frequency", default='draw',
        help="calls to compare [default: %default]")
    optparser.add_option(
        '--diff-state',
        action='store_true', dest='diff_state', default=False,
        help='diff state between failing calls')
    optparser.add_option(
        '-o', '--output', metavar='FILE',
        type="string", dest="output",
        help="output file [default: stdout]")

    (options, args) = optparser.parse_args(sys.argv[1:])
    ref_env = parse_env(optparser, options.ref_env)
    src_env = parse_env(optparser, options.src_env)
    if not args:
        optparser.error("incorrect number of arguments")
    
    if options.ref_driver:
        options.ref_args.insert(0, '--driver=' + options.ref_driver)
    if options.src_driver:
        options.src_args.insert(0, '--driver=' + options.src_driver)

    refRetracer = Retracer(options.retrace, options.ref_args + args, ref_env)
    srcRetracer = Retracer(options.retrace, options.src_args + args, src_env)

    if options.output:
        output = open(options.output, 'wt')
    else:
        output = sys.stdout

    highligher = AutoHighlighter(output)

    highligher.write('call\tprecision\n')

    last_bad = -1
    last_good = 0
    refRun = refRetracer.snapshot(options.snapshot_frequency)
    try:
        srcRun = srcRetracer.snapshot(options.snapshot_frequency)
        try:
            while True:
                # Get the reference image
                refImage, refCallNo = refRun.nextSnapshot()
                if refImage is None:
                    break

                # Get the source image
                srcImage, srcCallNo = srcRun.nextSnapshot()
                if srcImage is None:
                    break

                assert refCallNo == srcCallNo
                callNo = refCallNo

                # Compare the two images
                if isinstance(refImage, Image.Image) and isinstance(srcImage, Image.Image):
                    # Using PIL
                    numpyImages = False
                    comparer = Comparer(refImage, srcImage)
                    precision = comparer.precision()
                else:
                    # Using numpy (for floating point images)
                    # TODO: drop PIL when numpy path becomes general enough
                    import numpy
                    assert not isinstance(refImage, Image.Image)
                    assert not isinstance(srcImage, Image.Image)
                    numpyImages = True
                    assert refImage.shape == srcImage.shape
                    diffImage = numpy.square(srcImage - refImage)

                    height, width, channels = diffImage.shape
                    square_error = numpy.sum(diffImage)
                    square_error += numpy.finfo(numpy.float32).eps
                    rel_error = square_error / float(height*width*channels)
                    bits = -math.log(rel_error)/math.log(2.0)
                    precision = bits

                mismatch = precision < options.threshold

                if mismatch:
                    highligher.color(highligher.red)
                    highligher.bold()
                highligher.write('%u\t%f\n' % (callNo, precision))
                if mismatch:
                    highligher.normal()

                if mismatch:
                    if options.diff_prefix:
                        prefix = os.path.join(options.diff_prefix, '%010u' % callNo)
                        prefix_dir = os.path.dirname(prefix)
                        if not os.path.isdir(prefix_dir):
                            os.makedirs(prefix_dir)
                        if numpyImages:
                            dumpNumpyImage(output, refImage, prefix + '.ref.png')
                            dumpNumpyImage(output, srcImage, prefix + '.src.png')
                        else:
                            refImage.save(prefix + '.ref.png')
                            srcImage.save(prefix + '.src.png')
                            comparer.write_diff(prefix + '.diff.png')
                    if last_bad < last_good and options.diff_state:
                        srcRetracer.diff_state(last_good, callNo, output)
                    last_bad = callNo
                else:
                    last_good = callNo

                highligher.flush()
        finally:
            srcRun.terminate()
    finally:
        refRun.terminate()


if __name__ == '__main__':
    main()

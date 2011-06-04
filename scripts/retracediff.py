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


import optparse
import os.path
import re
import shutil
import subprocess
import platform
import sys
import tempfile

from snapdiff import Comparer
import jsondiff


# Null file, to use when we're not interested in subprocesses output
if platform.system() == 'Windows':
    NULL = open('NUL:', 'wt')
else:
    NULL = open('/dev/null', 'wt')


class Setup:

    def __init__(self, args, env=None):
        self.args = args
        self.env = env

    def retrace(self, snapshot_dir):
        cmd = [
            options.retrace,
            '-s', snapshot_dir + os.path.sep,
            '-S', options.snapshot_frequency,
        ] + self.args
        p = subprocess.Popen(cmd, env=self.env, stdout=subprocess.PIPE, stderr=NULL)
        return p

    def dump_state(self, call_no):
        '''Get the state dump at the specified call no.'''

        cmd = [
            options.retrace,
            '-D', str(call_no),
        ] + self.args
        p = subprocess.Popen(cmd, env=self.env, stdout=subprocess.PIPE, stderr=NULL)
        state = jsondiff.load(p.stdout)
        p.wait()
        return state


def diff_state(setup, ref_call_no, src_call_no):
    ref_state = setup.dump_state(ref_call_no)
    src_state = setup.dump_state(src_call_no)
    sys.stdout.flush()
    differ = jsondiff.Differ(sys.stdout)
    differ.visit(ref_state, src_state)
    sys.stdout.write('\n')


def parse_env(optparser, entries):
    '''Translate a list of NAME=VALUE entries into an environment dictionary.'''

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
        '--ref-env', metavar='NAME=VALUE',
        type='string', action='append', dest='ref_env', default=[],
        help='reference environment variable')
    optparser.add_option(
        '--src-env', metavar='NAME=VALUE',
        type='string', action='append', dest='src_env', default=[],
        help='reference environment variable')
    optparser.add_option(
        '--diff-prefix', metavar='PATH',
        type='string', dest='diff_prefix', default='.',
        help='reference environment variable')
    optparser.add_option(
        '-t', '--threshold', metavar='BITS',
        type="float", dest="threshold", default=12.0,
        help="threshold precision  [default: %default]")
    optparser.add_option(
        '-S', '--snapshot-frequency', metavar='FREQUENCY',
        type="string", dest="snapshot_frequency", default='draw',
        help="snapshot frequency  [default: %default]")

    (options, args) = optparser.parse_args(sys.argv[1:])
    ref_env = parse_env(optparser, options.ref_env)
    src_env = parse_env(optparser, options.src_env)
    if not args:
        optparser.error("incorrect number of arguments")

    ref_setup = Setup(args, ref_env)
    src_setup = Setup(args, src_env)

    image_re = re.compile('^Wrote (.*\.png)$')

    last_good = -1
    last_bad = -1
    ref_snapshot_dir = tempfile.mkdtemp()
    try:
        src_snapshot_dir = tempfile.mkdtemp()
        try:
            ref_proc = ref_setup.retrace(ref_snapshot_dir)
            try:
                src_proc = src_setup.retrace(src_snapshot_dir)
                try:
                    for ref_line in ref_proc.stdout:
                        # Get the reference image
                        ref_line = ref_line.rstrip()
                        mo = image_re.match(ref_line)
                        if mo:
                            ref_image = mo.group(1)
                            for src_line in src_proc.stdout:
                                # Get the source image
                                src_line = src_line.rstrip()
                                mo = image_re.match(src_line)
                                if mo:
                                    src_image = mo.group(1)
                                    
                                    root, ext = os.path.splitext(os.path.basename(src_image))
                                    call_no = int(root)

                                    # Compare the two images
                                    comparer = Comparer(ref_image, src_image)
                                    precision = comparer.precision()

                                    sys.stdout.write('%u %f\n' % (call_no, precision))
                                    
                                    if precision < options.threshold:
                                        if options.diff_prefix:
                                            comparer.write_diff(os.path.join(options.diff_prefix, root + '.diff.png'))
                                        if last_bad < last_good:
                                            diff_state(src_setup, last_good, call_no)
                                        last_bad = call_no
                                    else:
                                        last_good = call_no

                                    sys.stdout.flush()
                                   
                                    os.unlink(src_image)
                                    break
                            os.unlink(ref_image)
                finally:
                    src_proc.terminate()
            finally:
                ref_proc.terminate()
        finally:
            shutil.rmtree(ref_snapshot_dir)
    finally:
        shutil.rmtree(src_snapshot_dir)


if __name__ == '__main__':
    main()

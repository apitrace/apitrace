#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 VMware, Inc.
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

'''Check a trace replays successfully or not.

It is meant to be used with git bisect.  See git bisect manpage for more
details.
'''


import optparse
import os.path
import platform
import re
import subprocess
import sys
import traceback

import snapdiff
import retracediff


def good():
    '''Tell git-bisect that this commit is good.'''

    sys.stdout.write('GOOD\n')
    sys.exit(0)


def bad():
    '''Tell git-bisect that this commit is bad.'''

    sys.stdout.write('BAD\n')
    sys.exit(1)


def skip():
    '''Tell git-bisect to skip this commit.'''

    sys.stdout.write('SKIP\n')
    sys.exit(125)


def abort():
    '''Tell git-bisect to abort.'''

    sys.stdout.write('ABORT\n')
    sys.exit(-1)


def which(executable):
    '''Search for the executable on the PATH.'''

    if platform.system() == 'Windows':
        exts = ['.exe']
    else:
        exts = ['']
    dirs = os.environ['PATH'].split(os.path.pathsep)
    for dir in dirs:
        path = os.path.join(dir, executable)
        for ext in exts:
            if os.path.exists(path + ext):
                return True
    return False


def main():
    '''Main program.

    It will always invoke sys.exit(), and never return normally.
    '''

    # Try to guess the build command.
    if os.path.exists('SConstruct'):
        default_build = 'scons'
    elif os.path.exists('Makefile'):
        default_build = 'make'
    else:
        default_build = None

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\tgit bisect run %prog [options] -- [glretrace options] <trace>',
        version='%%prog')
    optparser.add_option(
        '-b', '--build', metavar='COMMAND',
        type='string', dest='build', default=default_build,
        help='build command [default: %default]')
    optparser.add_option(
        '-r', '--retrace', metavar='PROGRAM',
        type='string', dest='retrace', default='glretrace',
        help='retrace command [default: %default]')
    optparser.add_option(
        '-c', '--compare', metavar='PREFIX',
        type='string', dest='compare_prefix', default=None,
        help='snapshot comparison prefix')
    optparser.add_option(
        '--precision-threshold', metavar='BITS',
        type='float', dest='precision_threshold', default=8.0,
        help='precision threshold in bits [default: %default]')
    optparser.add_option(
        '--gl-renderer', metavar='REGEXP',
        type='string', dest='gl_renderer_re', default='^.*$',
        help='require a matching GL_RENDERER string [default: %default]')

    (options, args) = optparser.parse_args(sys.argv[1:])
    if not args:
        optparser.error("incorrect number of arguments")

    # Build the source
    if options.build:
        sys.stdout.write(options.build + '\n')
        sys.stdout.flush()
        returncode = subprocess.call(options.build, shell=True)
        if returncode:
            skip()

    # TODO: For this to be useful on Windows we'll also need an installation
    # procedure here.

    # Do some sanity checks.  In particular we want to make sure that the GL
    # implementation is usable, and is the right one (i.e., we didn't fallback
    # to a different OpenGL implementation due to missing symbols).
    if platform.system() != 'Windows' and which('glxinfo'):
        glxinfo = subprocess.Popen(['glxinfo'], stdout=subprocess.PIPE)
        stdout, stderr = glxinfo.communicate()
        if glxinfo.returncode:
            skip()

        # Search for the GL_RENDERER string
        gl_renderer_header = 'OpenGL renderer string: '
        gl_renderer = ''
        for line in stdout.split('\n'):
            if line.startswith(gl_renderer_header):
                gl_renderer = line[len(gl_renderer_header):]
            if line.startswith('direct rendering: No'):
                sys.stderr.write('Indirect rendering.\n')
                skip()

        # and match it against the regular expression specified in the command
        # line.
        if not re.search(options.gl_renderer_re, gl_renderer):
            sys.stderr.write("GL_RENDERER mismatch: %r !~ /%s/\n"  % (gl_renderer, options.gl_renderer_re))
            skip()

    # Run glretrace
    
    retracer = retracediff.Retracer(options.retrace, args)

    if options.compare_prefix:
        refImages = {}
        callNos = []
            
        images = snapdiff.find_images(options.compare_prefix)
        images.sort()
        for image in images:
            imageName, ext = os.path.splitext(image)
            try:
                callNo = int(imageName)
            except ValueError:
                continue
            refImages[callNo] = options.compare_prefix + image
            callNos.append(callNo)

        run = retracer.snapshot(','.join(map(str, callNos)))
        while True:
            srcImage, callNo = run.nextSnapshot()
            if srcImage is None:
                break

            refImage = refImages[callNo]

            # Compare the two images
            comparer = snapdiff.Comparer(refImage, srcImage)
            precision = comparer.precision()

            mismatch = precision < options.precision_threshold
            if mismatch:
                bad()
        run.process.wait()
        if run.process.returncode:
            skip()
    else:
        returncode = retracer.retrace('-b')
        if returncode:
            bad()

    # TODO: allow more criterias here, such as, performance threshold

    # Success
    good()


# Invoke main program, aborting the bisection on Ctrl+C or any uncaught Python
# exception.
if __name__ == '__main__':
    try:
        main()
    except SystemExit:
        raise
    except KeyboardInterrupt:
        abort()
    except:
        traceback.print_exc()
        abort()

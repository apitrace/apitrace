#!/usr/bin/env python3
##########################################################################
#
# Copyright 2011-2012 Jose Fonseca
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

'''Common test driver code.'''


import optparse
import os.path
import platform
import subprocess
import sys


# Disable Windows error dialog boxes
if sys.platform == 'win32':
    import ctypes
    SEM_FAILCRITICALERRORS = 1
    SEM_NOGPFAULTERRORBOX = 2
    dwMode = ctypes.windll.kernel32.GetErrorMode()
    dwMode |= SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX
    ctypes.windll.kernel32.SetErrorMode(dwMode)
    del dwMode


def _exit(status, code, reason=None):
    if reason is None:
        reason = ''
    else:
        reason = ' (%s)' % reason
    sys.stdout.write('%s%s\n' % (status, reason))
    sys.exit(code)

def fail(reason=None):
    _exit('FAIL', 1, reason)

def skip(reason=None):
    _exit('SKIP', 125, reason)

def pass_(reason=None):
    _exit('PASS', 0, reason)


def popen(command, *args, **kwargs):
    if kwargs.get('cwd', None) is not None:
        sys.stdout.write('cd %s && ' % kwargs['cwd'])

    try:
        env = kwargs.pop('env')
    except KeyError:
        env = None
    else:
        names = list(env.keys())
        names.sort()
        for name in names:
            value = env[name]
            if value != os.environ.get(name, None):
                sys.stdout.write('%s=%s ' % (name, value))
            env[name] = str(value)

    sys.stdout.write(' '.join(command) + '\n')
    sys.stdout.flush()

    return subprocess.Popen(command, *args, env=env, **kwargs)


def which(executable):
    dirs = os.environ['PATH'].split(os.path.pathsep)
    for dir in dirs:
        path = os.path.join(dir, executable)
        if os.path.exists(path):
            return path
    return None


def get_bin_path():
    if os.path.exists(options.apitrace):
        apitrace_abspath = os.path.abspath(options.apitrace)
    else:
        apitrace_abspath = which(options.apitrace)
        if apitrace_abspath is None:
            sys.stderr.write('error: could not determine the absolute path of\n' % options.apitrace)
            sys.exit(1)
    return os.path.dirname(apitrace_abspath)


def get_build_program(program):
    bin_path = get_bin_path()
    if platform.system() == 'Windows':
        program += '.exe'
    path = os.path.join(bin_path, program)
    if not os.path.exists(path):
        sys.stderr.write('error: %s does not exist\n' % path)
        sys.exit(1)
    return path


def get_scripts_path():
    if options.apitrace_source:
        return os.path.join(options.apitrace_source, 'scripts')

    bin_path = get_bin_path()

    try_paths = [
        'scripts',
        '../lib/scripts',
        '../lib/apitrace/scripts',
    ]

    for try_path in try_paths:
        path = os.path.join(bin_path, try_path)
        if os.path.exists(path):
            return os.path.abspath(path)

    sys.stderr.write('error: could not find scripts directory\n')
    sys.exit(1)


class Driver:

    def __init__(self):
        pass

    def createOptParser(self):
        default_apitrace = 'apitrace'
        if platform.system() == 'Windows':
            default_apitrace += '.exe'

        # Parse command line options
        optparser = optparse.OptionParser(
            usage='\n\t%prog [OPTIONS] -- [ARGS] ...',
            version='%%prog')
        optparser.add_option(
            '-v', '--verbose',
            action="store_true",
            dest="verbose", default=False,
            help="verbose output")
        optparser.add_option(
            '--apitrace', metavar='PROGRAM',
            type='string', dest='apitrace', default=default_apitrace,
            help='path to apitrace executable')
        optparser.add_option(
            '--apitrace-source', metavar='PATH',
            type='string', dest='apitrace_source',
            help='path to apitrace source tree')
        optparser.add_option(
            '-C', '--directory', metavar='PATH',
            type='string', dest='cwd', default=None,
            help='change to directory')

        return optparser

    def parseOptions(self):
        global options

        optparser = self.createOptParser()
        (options, args) = optparser.parse_args(sys.argv[1:])
        if not args:
            optparser.error('an argument must be specified')
        
        sys.path.insert(0, get_scripts_path())

        self.options = options
        self.args = args

        return options, args

    def run(self):
        raise NotImplementedError


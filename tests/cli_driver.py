#!/usr/bin/env python3

# Copyright 2012 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

'''Test driver for scripts in the cli directory.'''

import json
import errno
import shutil
import subprocess
import difflib

from base_driver import *

class CliDriver(Driver):

    def do_apitrace(self, args):
        cmd = [self.options.apitrace] + args.split()
 
        print(" ".join(cmd))
        proc = subprocess.Popen(cmd, stdout = subprocess.PIPE, universal_newlines=True)
        self.output = proc.communicate()[0]

        proc.wait()

        if (self.expect_failure):
            if (proc.returncode == 0):
                fail("Command unexpectedly passed when expecting failure:\n    " + " ".join(cmd))
        else:
            if (proc.returncode != 0):
                fail("Command failed (returned non-zero):\n    " + " ".join(cmd))

    def do_expect(self, args):
        expected = eval(args)
        if (self.output != expected):
            differ = difflib.Differ()
            diff = differ.compare(expected.splitlines(1), self.output.splitlines(1))
            diff = ''.join(diff)
            fail("Unexpected output:\n%s\n" % diff)

    def do_rm_and_mkdir(self, args):

        args = args.split()

        # Operate only on local directories
        dir = './' + args[0]

        # Failing to delete a directory that doesn't exist is no failure
        def rmtree_onerror(function, path, excinfo):
            if excinfo[0] == OSError and excinfo[1].errno != errno.ENOENT:
                raise

        shutil.rmtree(dir, onerror = rmtree_onerror)
        os.makedirs(dir)

    def unknown_command(self, args):
        fail('Broken test script: Unknown command: %s' % (args))

    def run_script(self, cli_script):
        "Execute the commands in the given cli script."

        script_dir, clip_script = os.path.split(cli_script)
        os.chdir(script_dir)

        commands = {
            'apitrace': self.do_apitrace,
            'expect': self.do_expect,
            'rm_and_mkdir': self.do_rm_and_mkdir
        }

        script = open(cli_script, 'rt')

        while True:

            self.expect_failure = False

            line = script.readline()

            # Exit loop on EOF
            if (line == ''):
                break

            line = line.rstrip()

            if " " in line:
                (cmd, args) = line.split(None,1)
                if args.startswith('r"""'):
                    while not line.endswith('"""'):
                        line = script.readline()
                        line = line.rstrip()
                        args += '\n' + line
            else:
                cmd = line
                args = ''

            # Ignore blank lines and comments
            if (len(cmd) == 0 or cmd == '\n' or cmd[0] == '#'):
                continue

            if (cmd == 'EXPECT_FAILURE:'):
                self.expect_failure = True
                if " " in args:
                    (cmd, args) = args.split(None, 1)
                else:
                    cmd = args
                    args = ''

            commands.get(cmd, self.unknown_command)(args)

    def run(self):
        self.parseOptions()

        self.run_script(self.args[0])

        pass_()

if __name__ == '__main__':
    CliDriver().run()

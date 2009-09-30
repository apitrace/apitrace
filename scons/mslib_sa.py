"""mslib_sa

Tool-specific initialization for lib (MicroSoft library archiver).

Based on SCons.Tool.mslib, without the MSVC detection.

"""

#
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

import os
import tempfile
import string

import SCons.Defaults
import SCons.Tool
import SCons.Util
import SCons.Errors

class TempFileMunge:
    """Same as SCons.Platform.TempFileMunge, but preserves LINK /LIB
    together.""" 

    def __init__(self, cmd):
        self.cmd = cmd

    def __call__(self, target, source, env, for_signature):
        if for_signature:
            return self.cmd
        cmd = env.subst_list(self.cmd, 0, target, source)[0]
        try:
            maxline = int(env.subst('$MAXLINELENGTH'))
        except ValueError:
            maxline = 2048

        if (reduce(lambda x, y: x + len(y), cmd, 0) + len(cmd)) <= maxline:
            return self.cmd

        # We do a normpath because mktemp() has what appears to be
        # a bug in Windows that will use a forward slash as a path
        # delimiter.  Windows's link mistakes that for a command line
        # switch and barfs.
        #
        # We use the .lnk suffix for the benefit of the Phar Lap
        # linkloc linker, which likes to append an .lnk suffix if
        # none is given.
        tmp = os.path.normpath(tempfile.mktemp('.lnk'))
        native_tmp = SCons.Util.get_native_path(tmp)

        if env['SHELL'] and env['SHELL'] == 'sh':
            # The sh shell will try to escape the backslashes in the
            # path, so unescape them.
            native_tmp = string.replace(native_tmp, '\\', r'\\\\')
            # In Cygwin, we want to use rm to delete the temporary
            # file, because del does not exist in the sh shell.
            rm = env.Detect('rm') or 'del'
        else:
            # Don't use 'rm' if the shell is not sh, because rm won't
            # work with the Windows shells (cmd.exe or command.com) or
            # Windows path names.
            rm = 'del'

        prefix = env.subst('$TEMPFILEPREFIX')
        if not prefix:
            prefix = '@'

        if cmd[0:2] == ['link', '/lib']:
            split = 2
        else:
            split = 1

        args = map(SCons.Subst.quote_spaces, cmd[split:])
        open(tmp, 'w').write(string.join(args, " ") + "\n")
        # XXX Using the SCons.Action.print_actions value directly
        # like this is bogus, but expedient.  This class should
        # really be rewritten as an Action that defines the
        # __call__() and strfunction() methods and lets the
        # normal action-execution logic handle whether or not to
        # print/execute the action.  The problem, though, is all
        # of that is decided before we execute this method as
        # part of expanding the $TEMPFILE construction variable.
        # Consequently, refactoring this will have to wait until
        # we get more flexible with allowing Actions to exist
        # independently and get strung together arbitrarily like
        # Ant tasks.  In the meantime, it's going to be more
        # user-friendly to not let obsession with architectural
        # purity get in the way of just being helpful, so we'll
        # reach into SCons.Action directly.
        if SCons.Action.print_actions:
            print("Using tempfile "+native_tmp+" for command line:\n"+
                  " ".join(map(str,cmd)))
        return cmd[:split] + [ prefix + native_tmp + '\n' + rm, native_tmp ]

def generate(env):
    """Add Builders and construction variables for lib to an Environment."""
    SCons.Tool.createStaticLibBuilder(env)

    if env.Detect('lib'):
        env['AR']          = 'lib'
    else:
        # Recent WINDDK versions do not ship with lib.
        env['AR']          = 'link /lib'
        env['TEMPFILE']    = TempFileMunge
    env['ARFLAGS']     = SCons.Util.CLVar('/nologo')
    env['ARCOM']       = "${TEMPFILE('$AR $ARFLAGS /OUT:$TARGET $SOURCES')}"
    env['LIBPREFIX']   = ''
    env['LIBSUFFIX']   = '.lib'

def exists(env):
    return env.Detect('lib') or env.Detect('link')

# vim:set ts=4 sw=4 et:

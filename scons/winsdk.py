"""winsdk

Tool-specific initialization for Microsoft Windows SDK.

"""

#
# Copyright (c) 2001-2007 The SCons Foundation
# Copyright (c) 2008 Tungsten Graphics, Inc.
# Copyright (c) 2009 VMware, Inc.
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

import os.path
import platform

import SCons.Errors
import SCons.Util

import msvc_sa
import mslib_sa
import mslink_sa


def get_vs_root(env):
    # TODO: Check HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VS7 
    path = os.path.join(os.getenv('ProgramFiles', r'C:\Program Files'), 'Microsoft Visual Studio 9.0')
    return path 

def get_vs_paths(env):
    vs_root = get_vs_root(env)
    if vs_root is None:
        raise SCons.Errors.InternalError, "WINSDK compiler not found"

    tool_path = os.path.join(vs_root, 'Common7', 'IDE')

    env.PrependENVPath('PATH', tool_path)

def get_vc_root(env):
    # TODO: Check HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VC7 
    path = os.path.join(os.getenv('ProgramFiles', r'C:\Program Files'), 'Microsoft Visual Studio 9.0', 'VC')
    return path 

def get_vc_paths(env):
    vc_root = get_vc_root(env)
    if vc_root is None:
        raise SCons.Errors.InternalError, "WINSDK compiler not found"

    target_cpu = env['machine']

    if target_cpu in ('generic', 'x86'):
        bin_dir = 'bin'
        lib_dir = 'lib'
    elif target_cpu == 'x86_64':
        # TODO: take in consideration the host cpu
        bin_dir = r'bin\x86_amd64'
        lib_dir = r'lib\amd64'
    else:
        raise SCons.Errors.InternalError, "Unsupported target machine"
    include_dir = 'include'

    env.PrependENVPath('PATH',    os.path.join(vc_root, bin_dir))
    env.PrependENVPath('INCLUDE', os.path.join(vc_root, include_dir))
    env.PrependENVPath('LIB',     os.path.join(vc_root, lib_dir))

def get_sdk_root(env):
    if SCons.Util.can_read_reg:
        key = r'SOFTWARE\Microsoft\Microsoft SDKs\Windows\CurrentInstallFolder'
        try:
            path, t = SCons.Util.RegGetValue(SCons.Util.HKEY_LOCAL_MACHINE, key)
        except SCons.Util.RegError:
            pass
        else:
            return path

    return None 

def get_sdk_paths(env):
    sdk_root = get_sdk_root(env)
    if sdk_root is None:
        raise SCons.Errors.InternalError, "WINSDK not found"

    target_cpu = env['machine']

    bin_dir = 'Bin'
    if target_cpu in ('generic', 'x86'):
        lib_dir = 'Lib'
    elif target_cpu == 'x86_64':
        lib_dir = r'Lib\x64'
    else:
        raise SCons.Errors.InternalError, "Unsupported target machine"
    include_dir = 'Include'

    env.PrependENVPath('PATH',    os.path.join(sdk_root, bin_dir))
    env.PrependENVPath('INCLUDE', os.path.join(sdk_root, include_dir))
    env.PrependENVPath('LIB',     os.path.join(sdk_root, lib_dir))

def generate(env):
    if not env.has_key('ENV'):
        env['ENV'] = {}
    
    get_vs_paths(env)
    get_vc_paths(env)
    get_sdk_paths(env)

    msvc_sa.generate(env)
    mslib_sa.generate(env)
    mslink_sa.generate(env)

def exists(env):
    return get_vc_root(env) is not None and get_sdk_root(env) is not None

# vim:set ts=4 sw=4 et:

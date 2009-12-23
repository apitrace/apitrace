#############################################################################
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
#############################################################################

import os
import os.path
import platform
import sys

_platform_map = {
    'freebsd': 'freebsd',
    'linux2': 'linux',
    'win32': 'windows',
}

default_platform = _platform_map.get(sys.platform, 'unix')

_machine_map = {
    'x86': 'x86',
    'i386': 'x86',
    'i486': 'x86',
    'i586': 'x86',
    'i686': 'x86',
    'ppc' : 'ppc',
    'x86_64': 'x86_64',
}
if 'PROCESSOR_ARCHITECTURE' in os.environ:
    default_machine = os.environ['PROCESSOR_ARCHITECTURE']
else:
    default_machine = platform.machine()
default_machine = _machine_map.get(default_machine, 'generic')

vars = Variables()
vars.Add(BoolVariable('debug', 'debug build', 'no'))
vars.Add(EnumVariable('platform', 'target platform', default_platform,
                      allowed_values=('linux', 'freebsd', 'unix', 'other', 'windows')))
vars.Add(EnumVariable('machine', 'use machine-specific assembly code', default_machine,
                      allowed_values=('generic', 'ppc', 'x86', 'x86_64')))
vars.Add(EnumVariable('toolchain', 'compiler toolchain', 'default',
                      allowed_values=('default', 'crossmingw', 'winsdk')))
vars.Add(EnumVariable('MSVS_VERSION', 'Microsoft Visual Studio version', None, allowed_values=('7.1', '8.0', '9.0')))

env = Environment(
    variables = vars, 
    ENV = os.environ)
Help(vars.GenerateHelpText(env))

Export(['env'])

env.Tool(env['toolchain'], toolpath = ['scons'])

env['gcc'] = 'gcc' in os.path.basename(env['CC']).split('-')
env['msvc'] = env['CC'] == 'cl'

# C preprocessor options
cppdefines = []
if not env['debug']:
    cppdefines += ['NDEBUG']
if env['platform'] == 'windows':
    cppdefines += [
        'WIN32',
        '_WINDOWS', 
        '_UNICODE',
        'UNICODE',
        '_CRT_SECURE_NO_DEPRECATE',
        '_CRT_NON_CONFORMING_SWPRINTFS',
        'WIN32_LEAN_AND_MEAN',
        '_USRDLL',
        ('_WIN32_WINNT', '0x0501'), # minimum required OS version
    ]
    if env['debug']:
        cppdefines += ['_DEBUG']
env.Append(CPPDEFINES = cppdefines)

# C compiler options
cflags = [] # C
cxxflags = [] # C++
ccflags = [] # C & C++
if env['gcc']:
    if env['debug']:
        ccflags += ['-O0', '-g3']
    else:
        ccflags += ['-O3', '-g0']
    if env['machine'] == 'x86':
        ccflags += ['-m32']
    if env['machine'] == 'x86_64':
        ccflags += ['-m64']
    # See also:
    # - http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
    ccflags += [
        '-Werror=declaration-after-statement',
        '-Wall',
        '-Wmissing-field-initializers',
        '-Wpointer-arith',
        '-fmessage-length=0', # be nice to Eclipse
    ]
    cflags += [
        '-Wmissing-prototypes',
    ]
if env['msvc']:
    if env['debug']:
        ccflags += [
          '/Od', # disable optimizations
          '/Oi', # enable intrinsic functions
          '/Oy-', # disable frame pointer omission
          '/GL-', # disable whole program optimization
        ]
    else:
        ccflags += [
          '/Ox', # maximum optimizations
          '/Oi', # enable intrinsic functions
          '/Ot', # favor code speed
        ]
    ccflags += [
        '/EHsc', # set exception handling model
        '/W4', # warning level
        #'/Wp64', # enable 64 bit porting warnings
    ]
    # Automatic pdb generation
    # See http://scons.tigris.org/issues/show_bug.cgi?id=1656
    env.EnsureSConsVersion(0, 98, 0)
    env['PDB'] = '${TARGET.base}.pdb'
env.Append(CCFLAGS = ccflags)
env.Append(CFLAGS = cflags)
env.Append(CXXFLAGS = cxxflags)

if env['platform'] == 'windows' and env['msvc']:
    # Choose the appropriate MSVC CRT
    # http://msdn.microsoft.com/en-us/library/2kzt1wy3.aspx
    if env['debug']:
        env.Append(CCFLAGS = ['/MTd'])
        env.Append(SHCCFLAGS = ['/LDd'])
    else:
        env.Append(CCFLAGS = ['/MT'])
        env.Append(SHCCFLAGS = ['/LD'])
    
# Assembler options
if env['gcc']:
    if env['machine'] == 'x86':
        env.Append(ASFLAGS = ['-m32'])
    if env['machine'] == 'x86_64':
        env.Append(ASFLAGS = ['-m64'])

# Linker options
linkflags = []
if env['gcc']:
    if env['machine'] == 'x86':
        linkflags += ['-m32']
    if env['machine'] == 'x86_64':
        linkflags += ['-m64']
if env['platform'] == 'windows' and env['msvc']:
    # See also:
    # - http://msdn2.microsoft.com/en-us/library/y0zzbyt4.aspx
    linkflags += [
        '/fixed:no',
        '/incremental:no',
    ]
env.Append(LINKFLAGS = linkflags)

env.Prepend(LIBS = [
    'kernel32',
    'user32',
    'gdi32',
])

SConscript('zlib/SConscript')

env.Tool('dxsdk', toolpath = ['scons'])

conf = Configure(env)
has_d3d7 = conf.CheckCXXHeader('ddraw.h')
has_d3d8 = conf.CheckCXXHeader('d3d8.h')
has_d3d9 = conf.CheckCXXHeader('d3d9.h')
if env['toolchain'] != 'crossmingw':
    has_d3d10 = conf.CheckCXXHeader('d3d10.h')
    has_d3d10_1 = conf.CheckCXXHeader('d3d10_1.h')
else:
    # The above checks do not give reliable results for MinGW
    has_d3d10 = True
    has_d3d10_1 = True
env = conf.Finish()

if has_d3d7 and False:
    env.Command(
        target = 'ddraw.cpp', 
        source = ['ddraw.py', 'd3d.py', 'd3dtypes.py', 'd3dcaps.py', 'windows.py', 'base.py'],
        action = 'python $SOURCE > $TARGET',
    )
        
    ddraw = env.SharedLibrary(
        target = 'ddraw',
        source = [
            'ddraw.def',
            'ddraw.cpp',
            'log.cpp',
        ]
    )

    env.Default(ddraw)

if has_d3d8:
    env.Command(
        target = 'd3d8.cpp', 
        source = ['d3d8.py', 'd3d8types.py', 'd3d8caps.py', 'windows.py', 'base.py'],
        action = 'python $SOURCE > $TARGET',
    )
        
    d3d8 = env.SharedLibrary(
        target = 'd3d8',
        source = [
            'd3d8.def',
            'd3d8.cpp',
            'log.cpp',
        ]
    )

    env.Default(d3d8)

if has_d3d9:
    env.Command(
        target = 'd3d9.cpp', 
        source = ['d3d9.py', 'd3d9types.py', 'd3d9caps.py', 'd3dshader.py', 'windows.py', 'base.py'],
        action = 'python $SOURCE > $TARGET',
    )
        
    d3d9 = env.SharedLibrary(
        target = 'd3d9',
        source = [
            'd3d9.def',
            'd3d9.cpp',
            'log.cpp',
        ]
    )

    env.Default(d3d9)

if has_d3d10:
    env.Command(
        target = 'd3d10.cpp', 
        source = ['d3d10misc.py', 'windows.py', 'base.py'],
        action = 'python $SOURCE > $TARGET',
    )
        
    d3d10 = env.SharedLibrary(
        target = 'd3d10',
        source = [
            'd3d10.def',
            'd3d10.cpp',
            'log.cpp',
        ]
    )

    env.Default(d3d10)

if has_d3d10_1:
    env.Command(
        target = 'd3d10_1.cpp', 
        source = ['d3d10_1.py', 'windows.py', 'base.py'],
        action = 'python $SOURCE > $TARGET',
    )
        
    d3d10_1 = env.SharedLibrary(
        target = 'd3d10_1',
        source = [
            'd3d10_1.def',
            'd3d10_1.cpp',
            'log.cpp',
        ]
    )

    env.Default(d3d10_1)

env.Command(
    target = 'opengl32.cpp', 
    source = ['opengl32.py', 'gl.py', 'windows.py', 'base.py'],
    action = 'python $SOURCE > $TARGET',
)
    
opengl32 = env.SharedLibrary(
    target = 'opengl32',
    source = [
        'opengl32.def',
        'opengl32.cpp',
        'log.cpp',
    ]
)

env.Default(opengl32)

env.Tool('packaging')

zip = env.Package(
    NAME           = 'apitrace',
    VERSION        = '0.3',
    PACKAGEVERSION = 0,
    PACKAGETYPE    = 'zip',
    LICENSE        = 'lgpl',
    SUMMARY        = 'Tool to trace Direct3D & OpenGL API calls from applications.',
    SOURCE_URL     = 'http://code.google.com/p/jrfonseca/source/browse?repo=apitrace',
    source = [
        'README',
        'd3d8.dll',
        'd3d9.dll',
        'opengl32.dll',
        'apitrace.xsl',
        'xml2txt.py',
    ],
)

env.Alias('zip', zip)

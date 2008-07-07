#######################################################################
# Top-level SConstruct
#
# For example, invoke scons as 
#
#   scons debug=1
#
# to set configuration variables. Or you can write those options to a file
# named config.py:
#
#   # config.py
#   debug=1
#   dxsdk='C:\\DXSDK'
# 
# Invoke
#
#   scons -h
#
# to get the full list of options. See scons manpage for more info.
#  

import os
import os.path
import sys

opts = Options('config.py')
opts.Add(BoolOption('debug', 'build debug version', 'no'))
opts.Add(PathOption('dxsdk', 'DirectX SDK installation dir', os.environ.get('DXSDK_DIR', 'C:\\DXSDK')))

env = Environment(
    options = opts, 
    ENV = os.environ)
Help(opts.GenerateHelpText(env))

env.Append(CPPDEFINES = [
    'WIN32', 
    '_WINDOWS', 
    '_UNICODE',
    'UNICODE',
    '_CRT_SECURE_NO_DEPRECATE',
    '_CRT_NON_CONFORMING_SWPRINTFS',
    'WIN32_LEAN_AND_MEAN',
    '_USRDLL',
])

if env['debug']:
    env.Append(CPPDEFINES = ['_DEBUG'])
else:
    env.Append(CPPDEFINES = ['NDEBUG'])
env['PDB'] = '${TARGET.base}.pdb'

cflags = [
    '/W4', # warning level
]
if env['debug']:
    cflags += [
      '/Od', # disable optimizations
      '/Oi', # enable intrinsic functions
      '/Oy-', # disable frame pointer omission
    ]
else:
    cflags += [
      '/Ox', # maximum optimizations
      '/Oi', # enable intrinsic functions
      '/Os', # favor code space
    ]
cflags += ['/MT']
env.Append(CFLAGS = cflags)
env.Append(CXXFLAGS = cflags)

env.Prepend(LIBS = [
    'kernel32',
    'user32',
    'gdi32',
    'comdlg32',
    'advapi32',
    'shell32',
])

env.Append(CPPPATH = [
    os.path.join(env['dxsdk'], 'Include'),
    '#common',
])

Export('env')



env.Command(
    target = 'd3d8.cpp', 
    source = ['d3d8.py', 'd3d8types.py', 'd3d8caps.py', 'windows.py', 'base.py'],
    action = 'python $SOURCE > $TARGET',
)
    

env.SharedLibrary(
    target = 'd3d8.dll',
    source = [
        'd3d8.def',
        'd3d8.cpp',
    ]
)

SConscript([
    'd3d9/SConscript',
])

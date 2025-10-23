##########################################################################
#
# Copyright 2011 Jose Fonseca
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


"""D3D retracer generator."""


import sys
from dllretrace import DllRetracer as Retracer
from specs.stdapi import API
from specs.d3d import ddraw, HWND
from specs.ddraw import DDCREATE_LPGUID


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print('// Swizzling mapping for lock addresses')
        print('static std::map<void *, void *> _maps;')
        print()
        # TODO: Keep a table of windows
        print('static HWND g_hWnd;')
        print()

        Retracer.retraceApi(self, api)

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice7',):
            if method.name == 'Release':
                print(r'    if (call.ret->toUInt() == 0) {')
                print(r'        d3d7Dumper.unbindDevice(_this);')
                print(r'    }')
            else:
                print(r'    d3d7Dumper.bindDevice(_this);')

        # create windows as neccessary
        hWndArg = method.getArgByType(HWND)
        if hWndArg is not None:
            # FIXME: Try to guess the window size (e.g., from IDirectDrawSurface7::Blt)
            print(r'    if (!g_hWnd) {')
            print(r'        g_hWnd = d3dretrace::createWindow(512, 512);')
            print(r'    }')
            print(r'    %s = g_hWnd;' % hWndArg.name)


        if method.name == 'Lock':
            # Reset _DONOTWAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('dwFlags')
            if mapFlagsArg is not None:
                print(r'    dwFlags &= ~DDLOCK_DONOTWAIT;')
                print(r'    dwFlags |= DDLOCK_WAIT;')

        Retracer.invokeInterfaceMethod(self, interface, method)

        if method.name == 'CreateDevice':
            print(r'    if (FAILED(_result)) {')
            print(r'        exit(1);')
            print(r'    }')

        # notify frame has been completed
        # process events after presents
        if interface.name.startswith('IDirectDrawSurface') and method.name == 'Blt':
            if interface.name in ('IDirectDrawSurface4', 'IDirectDrawSurface7'):
                print(r'    DDSCAPS2 ddsCaps;')
            else:
                print(r'    DDSCAPS ddsCaps;')
            print(r'    if (SUCCEEDED(_this->GetCaps(&ddsCaps)) &&')
            print(r'        (ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)) {')
            print(r'        retrace::frameComplete(call);')
            print(r'        d3dretrace::processEvents();')
            print(r'    }')

        if method.name == 'Lock':
            print('    VOID *_pbData = NULL;')
            print('    size_t _MappedSize = 0;')
            # FIXME: determine the mapping size
            #print '    _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames()[:-1])
            print('    if (_MappedSize) {')
            print('        _maps[_this] = _pbData;')
            # TODO: check pitches match
            print('    } else {')
            print('        return;')
            print('    }')
        
        if method.name == 'Unlock':
            print('    VOID *_pbData = 0;')
            print('    _pbData = _maps[_this];')
            print('    if (_pbData) {')
            print('        retrace::delRegionByPointer(_pbData);')
            print('        _maps[_this] = 0;')
            print('    }')

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        # Handle DDCREATE_* flags
        if arg.type is DDCREATE_LPGUID:
            print('    if (%s.toArray()) {' % rvalue)
            Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)
            print('    } else {')
            print('        %s = static_cast<%s>(%s.toPointer());' % (lvalue, arg_type, rvalue))
            print('    }')
            return

        Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)


def main():
    print(r'#include <string.h>')
    print()
    print(r'#include <iostream>')
    print()
    print(r'#include "d3dretrace.hpp"')
    print()

    api = API()
    
    print(r'#include "d3dimports.hpp"')
    api.addModule(ddraw)
    print()
    print('''static d3dretrace::D3DDumper<IDirect3DDevice7> d3d7Dumper;''')
    print()

    retracer = D3DRetracer()
    retracer.table_name = 'd3dretrace::ddraw_callbacks'
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

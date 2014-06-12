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


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print '// Swizzling mapping for lock addresses'
        print 'static std::map<void *, void *> _maps;'
        print

        Retracer.retraceApi(self, api)

    def invokeFunction(self, function):
        if function.name in ('Direct3DCreate9', 'Direct3DCreate9Ex'):
            print 'if (retrace::debug >= 2 && !g_szD3D9DllName && LoadLibraryA("d3d9d.dll")) {'
            print '    /*'
            print '     * D3D9D only works for simple applications, it will often report bogus errors'
            print '     * on complex traces, or traces which use unofficial D3D9 features.'
            print '     */'
            print '    g_szD3D9DllName = "d3d9d.dll";'
            print '    SDKVersion |= 0x80000000;'
            print '} else {'
            print '    SDKVersion &= ~0x80000000;'
            print '}'

        Retracer.invokeFunction(self, function)

    createDeviceMethodNames = [
        'CreateDevice',
        'CreateDeviceEx',
    ]

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d9Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    d3d9Dumper.bindDevice(_this);'
        if interface.name in ('IDirect3DDevice8', 'IDirect3DDevice8Ex'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d8Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    d3d8Dumper.bindDevice(_this);'

        # create windows as neccessary
        if method.name in ('CreateDevice', 'CreateDeviceEx', 'CreateAdditionalSwapChain'):
            print r'    HWND hWnd = d3dretrace::createWindow(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    pPresentationParameters->hDeviceWindow = hWnd;'
            if 'hFocusWindow' in method.argNames():
                print r'    hFocusWindow = hWnd;'

            # force windowed mode
            print r'    if (retrace::forceWindowed) {'
            print r'        pPresentationParameters->Windowed = TRUE;'
            print r'        pPresentationParameters->FullScreen_RefreshRateInHz = 0;'
            print r'    }'
        
        if method.name in self.createDeviceMethodNames:
            # override the device type
            print r'    switch (retrace::driver) {'
            print r'    case retrace::DRIVER_HARDWARE:'
            print r'        DeviceType = D3DDEVTYPE_HAL;'
            print r'        break;'
            print r'    case retrace::DRIVER_SOFTWARE:'
            print r'    case retrace::DRIVER_REFERENCE:'
            print r'        DeviceType = D3DDEVTYPE_REF;'
            print r'        break;'
            print r'    case retrace::DRIVER_NULL:'
            if interface.name.startswith('IDirect3D9'):
                print r'        DeviceType = D3DDEVTYPE_NULLREF;'
            else:
                print r'        retrace::warning(call) << "null driver not supported\n";'
            print r'        break;'
            print r'    case retrace::DRIVER_MODULE:'
            print r'        retrace::warning(call) << "driver module not supported\n";'
            print r'        break;'
            print r'    default:'
            print r'        assert(0);'
            print r'        /* fall-through */'
            print r'    case retrace::DRIVER_DEFAULT:'
            print r'        break;'
            print r'    }'

        if method.name in ('Reset', 'ResetEx'):
            # force windowed mode
            print r'    if (retrace::forceWindowed) {'
            print r'        pPresentationParameters->Windowed = TRUE;'
            print r'        pPresentationParameters->FullScreen_RefreshRateInHz = 0;'
            print r'    }'
            # resize window
            print r'    if (pPresentationParameters->Windowed) {'
            print r'        d3dretrace::resizeWindow(pPresentationParameters->hDeviceWindow, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    }'

        # notify frame has been completed
        if method.name in ('Present', 'PresentEx'):
            print r'    retrace::frameComplete(call);'
            print r'    hDestWindowOverride = NULL;'

        if 'pSharedHandle' in method.argNames():
            print r'    if (pSharedHandle) {'
            print r'        retrace::warning(call) << "shared surfaces unsupported\n";'
            print r'        pSharedHandle = NULL;'
            print r'    }'

        Retracer.invokeInterfaceMethod(self, interface, method)

        if method.name in self.createDeviceMethodNames:
            print r'    if (FAILED(_result)) {'
            print r'        exit(1);'
            print r'    }'

        # process events after presents
        if method.name == 'Present':
            print r'    d3dretrace::processEvents();'

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print '    VOID *_pbData = NULL;'
            print '    size_t _MappedSize = 0;'
            print '    _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames()[:-1])
            print '    if (_MappedSize) {'
            print '        _maps[_this] = _pbData;'
            print '    } else {'
            print '        return;'
            print '    }'
        
        if method.name in ('Unlock', 'UnlockRect', 'UnlockBox'):
            print '    VOID *_pbData = 0;'
            print '    _pbData = _maps[_this];'
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '        _maps[_this] = 0;'
            print '    }'


def main():
    print r'#include <string.h>'
    print
    print r'#include <iostream>'
    print
    print r'#include "d3dretrace.hpp"'
    print

    moduleName = sys.argv[1]
    support = int(sys.argv[2])

    api = API()
    
    if support:
        if moduleName == 'd3d9':
            from specs.d3d9 import d3d9
            print r'#include "d3d9imports.hpp"'
            print r'#include "d3d9size.hpp"'
            api.addModule(d3d9)
            print
            print '''static d3dretrace::D3DDumper<IDirect3DDevice9> d3d9Dumper;'''
            print
        elif moduleName == 'd3d8':
            from specs.d3d8 import d3d8
            print r'#include "d3d8imports.hpp"'
            print r'#include "d3d8size.hpp"'
            api.addModule(d3d8)
            print
            print '''static d3dretrace::D3DDumper<IDirect3DDevice8> d3d8Dumper;'''
            print
        else:
            assert False

    retracer = D3DRetracer()
    retracer.table_name = 'd3dretrace::%s_callbacks' % moduleName
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

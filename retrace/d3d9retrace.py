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


from dllretrace import DllRetracer as Retracer
from specs.stdapi import API
from specs.d3d9 import *


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print '''

class D3D9Dumper : public retrace::Dumper {
public:
    IDirect3DDevice9 *pLastDirect3DDevice9;

    D3D9Dumper() :
        pLastDirect3DDevice9(NULL)
    {}

    image::Image *
    getSnapshot(void) {
        if (!pLastDirect3DDevice9) {
            return NULL;
        }
        return d3dstate::getRenderTargetImage(pLastDirect3DDevice9);
    }

    bool
    dumpState(std::ostream &os) {
        if (!pLastDirect3DDevice9) {
            return false;
        }
        d3dstate::dumpDevice(os, pLastDirect3DDevice9);
        return true;
    }

    inline void
    bindDevice(IDirect3DDevice9 *pDevice) {
        pLastDirect3DDevice9 = pDevice;
        retrace::dumper = this;
    }
    
    inline void
    unbindDevice(IDirect3DDevice9 *pDevice) {
        if (pLastDirect3DDevice9 == pDevice) {
            pLastDirect3DDevice9 = NULL;
        }
    }
};

static D3D9Dumper d3d9Dumper;
'''

        print '// Swizzling mapping for lock addresses'
        print 'static std::map<void *, void *> _maps;'
        print

        self.table_name = 'd3dretrace::d3d9_callbacks'

        Retracer.retraceApi(self, api)

    def invokeFunction(self, function):
        if function.name in ('Direct3DCreate9', 'Direct3DCreate9Ex'):
            print 'if (retrace::debug && !g_szD3D9DllName) {'
            print '    /* '
            print '     * XXX: D3D9D only works for simple things, it often introduces errors'
            print '     * on complex traces, or traces which use unofficial D3D9 features.'
            print '     */'
            print '    if (0) {'
            print '        g_szD3D9DllName = "d3d9d.dll";'
            print '    }'
            print '}'

        Retracer.invokeFunction(self, function)

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
            if method.name == 'Release':
                print r'    d3d9Dumper.unbindDevice(_this);'
            else:
                print r'    d3d9Dumper.bindDevice(_this);'

        # create windows as neccessary
        if method.name in ('CreateDevice', 'CreateDeviceEx', 'CreateAdditionalSwapChain'):
            print r'    HWND hWnd = d3dretrace::createWindow(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    pPresentationParameters->hDeviceWindow = hWnd;'
            if 'hFocusWindow' in method.argNames():
                print r'    hFocusWindow = hWnd;'

        if method.name in ('Reset', 'ResetEx'):
            print r'    if (pPresentationParameters->Windowed) {'
            print r'        d3dretrace::resizeWindow(pPresentationParameters->hDeviceWindow, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    }'

        # notify frame has been completed
        if method.name == 'Present':
            print r'    retrace::frameComplete(call);'
            print r'    hDestWindowOverride = NULL;'

        if 'pSharedHandle' in method.argNames():
            print r'    if (pSharedHandle) {'
            print r'        retrace::warning(call) << "shared surfaces unsupported\n";'
            print r'        pSharedHandle = NULL;'
            print r'    }'

        Retracer.invokeInterfaceMethod(self, interface, method)

        # process events after presents
        if method.name == 'Present':
            print r'    d3dretrace::processEvents();'

        # check errors
        if str(method.type) == 'HRESULT':
            print r'    if (FAILED(_result)) {'
            print r'        retrace::warning(call) << "failed\n";'
            print r'    }'

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print '    VOID *_pbData = NULL;'
            print '    size_t _MappedSize = 0;'
            print '    _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames()[:-1])
            print '    _maps[_this] = _pbData;'
        
        if method.name in ('Unlock', 'UnlockRect', 'UnlockBox'):
            print '    VOID *_pbData = 0;'
            print '    _pbData = _maps[_this];'
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '    }'


if __name__ == '__main__':
    print r'''
#include <string.h>

#include <iostream>

#include "d3d9imports.hpp"
#include "d3d9size.hpp"
#include "d3dretrace.hpp"
#include "d3d9state.hpp"

'''

    api = API()
    api.addModule(d3d9)
    retracer = D3DRetracer()
    retracer.retraceApi(api)

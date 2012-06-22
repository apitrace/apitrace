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
import specs.stdapi as stdapi
from specs.d3d9 import *


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print 'static const GUID GUID_D3DRETRACE = {0x7D71CAC9,0x7F58,0x432C,{0xA9,0x75,0xA1,0x9F,0xCF,0xCE,0xFD,0x14}};'
        print

        self.table_name = 'd3dretrace::%s_callbacks' % api.name.lower()

        Retracer.retraceApi(self, api)

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
            print r'    d3dretrace::pLastDirect3DDevice9 = _this;'

        # create windows as neccessary
        if method.name in ('CreateDevice', 'CreateDeviceEx', 'CreateAdditionalSwapChain'):
            print r'    HWND hWnd = d3dretrace::createWindow(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    pPresentationParameters->hDeviceWindow = hWnd;'
            if 'hFocusWindow' in method.argNames():
                print r'    hFocusWindow = hWnd;'

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

        # check errors
        if str(method.type) == 'HRESULT':
            print r'    if (FAILED(_result)) {'
            print r'        retrace::warning(call) << "failed\n";'
            print r'    }'

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print '    VOID *_pbData = NULL;'
            print '    size_t _LockedSize = 0;'
            print '    _getLockInfo(_this, %s, _pbData, _LockedSize);' % ', '.join(method.argNames()[:-1])
            print '    _this->SetPrivateData(GUID_D3DRETRACE, &_pbData, sizeof _pbData, 0);'
        
        if method.name in ('Unlock', 'UnlockRect', 'UnlockBox'):
            print '    VOID *_pbData = 0;'
            print '    DWORD dwSizeOfData = sizeof _pbData;'
            print '    _this->GetPrivateData(GUID_D3DRETRACE, &_pbData, &dwSizeOfData);'
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '    }'


if __name__ == '__main__':
    print r'''
#include <string.h>

#include <iostream>

#include "d3d9imports.hpp"
#include "d3dsize.hpp"
#include "d3dretrace.hpp"

'''

    retracer = D3DRetracer()
    retracer.retraceApi(d3d9)

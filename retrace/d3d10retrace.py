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
from specs.d3d10 import *


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print '// Swizzling mapping for lock addresses'
        print 'static std::map<void *, void *> _maps;'
        print

        self.table_name = 'd3dretrace::%s_callbacks' % api.name.lower()

        Retracer.retraceApi(self, api)

    def invokeFunction(self, function):
        # create windows as neccessary
        if function.name == 'D3D10CreateDeviceAndSwapChain':
            print r'    pSwapChainDesc->OutputWindow = d3dretrace::createWindow(512, 512);'

        Retracer.invokeFunction(self, function)

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        #if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
        #    print r'    d3dretrace::pLastDirect3DDevice9 = _this;'

        # create windows as neccessary
        if method.name == 'CreateSwapChain':
            print r'    pDesc->OutputWindow = d3dretrace::createWindow(512, 512);'

        # notify frame has been completed
        if method.name == 'Present':
            print r'    retrace::frameComplete(call);'

        if 'pSharedResource' in method.argNames():
            print r'    if (pSharedResource) {'
            print r'        retrace::warning(call) << "shared surfaces unsupported\n";'
            print r'        pSharedResource = NULL;'
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

        if method.name == 'Map':
            print '    VOID *_pbData = NULL;'
            print '    size_t _MappedSize = 0;'
            print '    _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames())
            print '    _maps[_this] = _pbData;'
        
        if method.name == 'Unmap':
            print '    VOID *_pbData = 0;'
            print '    _pbData = _maps[_this];'
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '    }'


if __name__ == '__main__':
    print r'''
#include <string.h>

#include <iostream>

#include "d3d10imports.hpp"
#include "d3d10size.hpp"
#include "d3dretrace.hpp"

'''

    retracer = D3DRetracer()
    retracer.retraceApi(d3d10)

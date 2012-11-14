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
from specs.dxgi import dxgi
from specs.d3d10 import d3d10
from specs.d3d10_1 import d3d10_1
from specs.d3d11 import d3d11


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print '// Swizzling mapping for lock addresses'
        print 'static std::map<void *, void *> _maps;'
        print

        self.table_name = 'd3dretrace::d3d10_callbacks'

        Retracer.retraceApi(self, api)

    def invokeFunction(self, function):
        # create windows as neccessary
        if function.name in ('D3D10CreateDeviceAndSwapChain', 'D3D10CreateDeviceAndSwapChain1', 'D3D11CreateDeviceAndSwapChain'):
            print r'    pSwapChainDesc->OutputWindow = d3dretrace::createWindow(512, 512);'

        if 'Software' in function.argNames():
            print r'    if (Software) {'
            print r'        retrace::warning(call) << "software device\n";'
            print r'        Software = LoadLibraryA("d3d10warp");'
            print r'    }'

        Retracer.invokeFunction(self, function)


    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('ID3D11DeviceContext',):
            if method.name == 'Release':
                print r'    d3d11Dumper.unbindDevice(_this);'
            else:
                print r'    d3d11Dumper.bindDevice(_this);'

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


def main():
    print r'''#include <string.h>'''
    print
    print r'#include <iostream>'
    print
    print r'#include "d3dretrace.hpp"'
    print

    moduleNames = sys.argv[1:]

    api = API()
    if moduleNames:
        api.addModule(dxgi)
    if 'd3d10' in moduleNames:
        if 'd3d10_1' in moduleNames:
            print r'#include "d3d10_1imports.hpp"'
            # D3D10CreateBlob is duplicated in d3d10 and d3d10_1
            d3d10_1.functions = [function for function in d3d10_1.functions if function.name != 'D3D10CreateBlob']
            api.addModule(d3d10_1)
        else:
            print r'#include "d3d10imports.hpp"'
        print r'#include "d3d10size.hpp"'
        api.addModule(d3d10)
    if 'd3d11' in moduleNames:
        print r'#include "d3d11imports.hpp"'
        if 'd3d11_1' in moduleNames:
            print '#include <d3d11_1.h>'
            import specs.d3d11_1
        print r'#include "d3d11size.hpp"'
        print r'#include "d3dstate.hpp"'
        api.addModule(d3d11)
        
        print
        print '''static d3dretrace::D3DDumper<ID3D11DeviceContext> d3d11Dumper;'''
        print

    retracer = D3DRetracer()
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

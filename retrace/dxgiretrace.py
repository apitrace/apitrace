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
        print r'''
static void 
createWindow(DXGI_SWAP_CHAIN_DESC *pSwapChainDesc) {
    if (pSwapChainDesc->Windowed) {
        UINT Width  = pSwapChainDesc->BufferDesc.Width;
        UINT Height = pSwapChainDesc->BufferDesc.Height;
        if (!Width)  Width = 1024;
        if (!Height) Height = 768;
        pSwapChainDesc->OutputWindow = d3dretrace::createWindow(Width, Height);
    }
}
'''

        self.table_name = 'd3dretrace::dxgi_callbacks'

        Retracer.retraceApi(self, api)

    createDeviceFunctionNames = [
        "D3D10CreateDevice",
        "D3D10CreateDeviceAndSwapChain",
        "D3D10CreateDevice1",
        "D3D10CreateDeviceAndSwapChain1",
        "D3D11CreateDevice",
        "D3D11CreateDeviceAndSwapChain",
    ]

    def invokeFunction(self, function):
        if function.name in self.createDeviceFunctionNames:
            # create windows as neccessary
            if 'pSwapChainDesc' in function.argNames():
                print r'    createWindow(pSwapChainDesc);'

            # Compensate for the fact we don't trace DXGI object creation
            if function.name.startswith('D3D11CreateDevice'):
                print r'    if (DriverType == D3D_DRIVER_TYPE_UNKNOWN && !pAdapter) {'
                print r'        DriverType = D3D_DRIVER_TYPE_HARDWARE;'
                print r'    }'

            if function.name.startswith('D3D10CreateDevice'):
                self.forceDriver('D3D10_DRIVER_TYPE')
            if function.name.startswith('D3D11CreateDevice'):
                self.forceDriver('D3D_DRIVER_TYPE')

        Retracer.invokeFunction(self, function)

    def forceDriver(self, enum):
        print r'    switch (retrace::driver) {'
        print r'    case retrace::DRIVER_HARDWARE:'
        print r'        DriverType = %s_HARDWARE;' % enum
        print r'        Software = NULL;'
        print r'        break;'
        print r'    case retrace::DRIVER_SOFTWARE:'
        print r'        pAdapter = NULL;'
        print r'        DriverType = %s_WARP;' % enum
        print r'        Software = NULL;'
        print r'        break;'
        print r'    case retrace::DRIVER_REFERENCE:'
        print r'        pAdapter = NULL;'
        print r'        DriverType = %s_REFERENCE;' % enum
        print r'        Software = NULL;'
        print r'        break;'
        print r'    case retrace::DRIVER_NULL:'
        print r'        pAdapter = NULL;'
        print r'        DriverType = %s_NULL;' % enum
        print r'        Software = NULL;'
        print r'        break;'
        print r'    case retrace::DRIVER_MODULE:'
        print r'        pAdapter = NULL;'
        print r'        DriverType = %s_SOFTWARE;' % enum
        print r'        Software = LoadLibraryA(retrace::driverModule);'
        print r'        if (!Software) {'
        print r'            retrace::warning(call) << "failed to load " << retrace::driverModule << "\n";'
        print r'        }'
        print r'        break;'
        print r'    default:'
        print r'        assert(0);'
        print r'        /* fall-through */'
        print r'    case retrace::DRIVER_DEFAULT:'
        print r'        if (DriverType == %s_SOFTWARE) {' % enum
        print r'            Software = LoadLibraryA("d3d10warp");'
        print r'            if (!Software) {'
        print r'                retrace::warning(call) << "failed to load d3d10warp.dll\n";'
        print r'            }'
        print r'        }'
        print r'        break;'
        print r'    }'

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('ID3D10Device', 'ID3D10Device1'):
            if method.name == 'Release':
                print r'    d3d10Dumper.unbindDevice(_this);'
            else:
                print r'    d3d10Dumper.bindDevice(_this);'
        if interface.name in ('ID3D11DeviceContext',):
            if method.name == 'Release':
                print r'    d3d11Dumper.unbindDevice(_this);'
            else:
                print r'    d3d11Dumper.bindDevice(_this);'

        # create windows as neccessary
        if method.name == 'CreateSwapChain':
            print r'    createWindow(pDesc);'

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

        if method.name == 'Map':
            print '    VOID *_pbData = NULL;'
            print '    size_t _MappedSize = 0;'
            print '    _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames())
            print '    if (_MappedSize) {'
            print '        _maps[_this] = _pbData;'
            print '    } else {'
            print '        return;'
            print '    }'
        
        if method.name == 'Unmap':
            print '    VOID *_pbData = 0;'
            print '    _pbData = _maps[_this];'
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '        _maps[_this] = 0;'
            print '    }'

        # Attach shader byte code for lookup
        if 'pShaderBytecode' in method.argNames():
            ppShader = method.args[-1]
            assert ppShader.output
            print r'    if (retrace::dumpingState && SUCCEEDED(_result)) {'
            print r'        (*%s)->SetPrivateData(d3dstate::GUID_D3DSTATE, BytecodeLength, pShaderBytecode);' % ppShader.name
            print r'    }'


def main():
    print r'#include <string.h>'
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
            api.addModule(d3d10_1)
        else:
            print r'#include "d3d10imports.hpp"'
        print r'#include "d3d10size.hpp"'
        api.addModule(d3d10)
        print
        print '''static d3dretrace::D3DDumper<ID3D10Device> d3d10Dumper;'''
        print

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

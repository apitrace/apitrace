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
import specs.dxgi
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
    UINT Width  = pSwapChainDesc->BufferDesc.Width;
    UINT Height = pSwapChainDesc->BufferDesc.Height;
    if (!Width)  Width = 1024;
    if (!Height) Height = 768;
    pSwapChainDesc->OutputWindow = d3dretrace::createWindow(Width, Height);
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
                # Toggle debugging
                print r'    Flags &= ~D3D10_CREATE_DEVICE_DEBUG;'
                print r'    if (retrace::debug) {'
                print r'        if (LoadLibraryA("d3d10sdklayers")) {'
                print r'            Flags |= D3D10_CREATE_DEVICE_DEBUG;'
                print r'        }'
                print r'    }'

                # Force driver
                self.forceDriver('D3D10_DRIVER_TYPE')

            if function.name.startswith('D3D11CreateDevice'):
                # Toggle debugging
                print r'    Flags &= ~D3D11_CREATE_DEVICE_DEBUG;'
                print r'    if (retrace::debug) {'
                print r'        OSVERSIONINFO osvi;'
                print r'        BOOL bIsWindows8orLater;'
                print r'        ZeroMemory(&osvi, sizeof osvi);'
                print r'        osvi.dwOSVersionInfoSize = sizeof osvi;'
                print r'        GetVersionEx(&osvi);'
                print r'        bIsWindows8orLater = '
                print r'            (osvi.dwMajorVersion > 6) ||'
                print r'            (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2);'
                print r'        const char *szD3d11SdkLayers = bIsWindows8orLater ? "d3d11_1sdklayers" : "d3d11sdklayers";'
                print r'        if (LoadLibraryA(szD3d11SdkLayers)) {'
                print r'            Flags |= D3D11_CREATE_DEVICE_DEBUG;'
                print r'        }'
                print r'    }'

                # Force driver
                self.forceDriver('D3D_DRIVER_TYPE')

        Retracer.invokeFunction(self, function)

        # Debug layers with Windows 8 or Windows 7 Platform update are a mess.
        # It's not possible to know before hand whether they are or not
        # available, so always retry with debug flag off..
        if function.name in self.createDeviceFunctionNames:
            print r'    if (FAILED(_result)) {'

            if function.name.startswith('D3D10CreateDevice'):
                print r'        if (_result == E_FAIL && (Flags & D3D10_CREATE_DEVICE_DEBUG)) {'
                print r'            retrace::warning(call) << "debug layer (d3d10sdklayers.dll) not installed\n";'
                print r'            Flags &= ~D3D10_CREATE_DEVICE_DEBUG;'
                Retracer.invokeFunction(self, function)
                print r'        }'
            elif function.name.startswith('D3D11CreateDevice'):
                print r'        if (_result == E_FAIL && (Flags & D3D11_CREATE_DEVICE_DEBUG)) {'
                print r'            retrace::warning(call) << "debug layer (d3d11sdklayers.dll for Windows 7, d3d11_1sdklayers.dll for Windows 8 or Windows 7 with KB 2670838) not properly installed\n";'
                print r'            Flags &= ~D3D11_CREATE_DEVICE_DEBUG;'
                Retracer.invokeFunction(self, function)
                print r'        }'
            else:
                assert False

            print r'        if (FAILED(_result)) {'
            print r'            exit(1);'
            print r'        }'

            print r'    }'

    def forceDriver(self, enum):
        # This can only work when pAdapter is NULL. For non-NULL pAdapter we
        # need to override inside the EnumAdapters call below
        print r'    if (pAdapter == NULL) {'
        print r'        switch (retrace::driver) {'
        print r'        case retrace::DRIVER_HARDWARE:'
        print r'            DriverType = %s_HARDWARE;' % enum
        print r'            Software = NULL;'
        print r'            break;'
        print r'        case retrace::DRIVER_SOFTWARE:'
        print r'            DriverType = %s_WARP;' % enum
        print r'            Software = NULL;'
        print r'            break;'
        print r'        case retrace::DRIVER_REFERENCE:'
        print r'            DriverType = %s_REFERENCE;' % enum
        print r'            Software = NULL;'
        print r'            break;'
        print r'        case retrace::DRIVER_NULL:'
        print r'            DriverType = %s_NULL;' % enum
        print r'            Software = NULL;'
        print r'            break;'
        print r'        case retrace::DRIVER_MODULE:'
        print r'            DriverType = %s_SOFTWARE;' % enum
        print r'            Software = LoadLibraryA(retrace::driverModule);'
        print r'            if (!Software) {'
        print r'                retrace::warning(call) << "failed to load " << retrace::driverModule << "\n";'
        print r'            }'
        print r'            break;'
        print r'        default:'
        print r'            assert(0);'
        print r'            /* fall-through */'
        print r'        case retrace::DRIVER_DEFAULT:'
        print r'            if (DriverType == %s_SOFTWARE) {' % enum
        print r'                Software = LoadLibraryA("d3d10warp");'
        print r'                if (!Software) {'
        print r'                    retrace::warning(call) << "failed to load d3d10warp.dll\n";'
        print r'                }'
        print r'            }'
        print r'            break;'
        print r'        }'
        print r'    } else {'
        print r'        Software = NULL;'
        print r'    }'

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('ID3D10Device', 'ID3D10Device1'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d10Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    d3d10Dumper.bindDevice(_this);'
        if interface.name in ('ID3D11DeviceContext', 'ID3D11DeviceContext1'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d11Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    if (_this->GetType() == D3D11_DEVICE_CONTEXT_IMMEDIATE) {'
                print r'        d3d11Dumper.bindDevice(_this);'
                print r'    }'

        if interface.name == 'IDXGIFactory' and method.name == 'QueryInterface':
            print r'    if (riid == IID_IDXGIFactoryDWM) {'
            print r'        _this->AddRef();'
            print r'        *ppvObj = new d3dretrace::CDXGIFactoryDWM(_this);'
            print r'        _result = S_OK;'
            print r'    } else {'
            Retracer.invokeInterfaceMethod(self, interface, method)
            print r'    }'
            return

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

        # Force driver
        if interface.name.startswith('IDXGIFactory') and method.name.startswith('EnumAdapters'):
            print r'    const char *szSoftware = NULL;'
            print r'    switch (retrace::driver) {'
            print r'    case retrace::DRIVER_REFERENCE:'
            print r'    case retrace::DRIVER_SOFTWARE:'
            print r'        szSoftware = "d3d10warp.dll";'
            print r'        break;'
            print r'    case retrace::DRIVER_MODULE:'
            print r'        szSoftware = retrace::driverModule;'
            print r'        break;'
            print r'    default:'
            print r'        break;'
            print r'    }'
            print r'    HMODULE hSoftware = NULL;'
            print r'    if (szSoftware) {'
            print r'        hSoftware = LoadLibraryA(szSoftware);'
            print r'        if (!hSoftware) {'
            print r'            retrace::warning(call) << "failed to load " << szSoftware << "\n";'
            print r'        }'
            print r'    }'
            print r'    if (hSoftware) {'
            print r'        _result = _this->CreateSoftwareAdapter(hSoftware, reinterpret_cast<IDXGIAdapter **>(ppAdapter));'
            print r'    } else {'
            Retracer.invokeInterfaceMethod(self, interface, method)
            print r'    }'
            return

        if interface.name.startswith('ID3D10Device') and method.name == 'OpenSharedResource':
            print r'    retrace::warning(call) << "replacing shared resource with checker pattern\n";'
            print r'    D3D10_TEXTURE2D_DESC Desc;'
            print r'    memset(&Desc, 0, sizeof Desc);'
            print r'    Desc.Width = 8;'
            print r'    Desc.Height = 8;'
            print r'    Desc.MipLevels = 1;'
            print r'    Desc.ArraySize = 1;'
            print r'    Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;'
            print r'    Desc.SampleDesc.Count = 1;'
            print r'    Desc.SampleDesc.Quality = 0;'
            print r'    Desc.Usage = D3D10_USAGE_DEFAULT;'
            print r'    Desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;'
            print r'    Desc.CPUAccessFlags = 0x0;'
            print r'    Desc.MiscFlags = 0 /* D3D10_RESOURCE_MISC_SHARED */;'
            print r'''
            static const DWORD Checker[8][8] = {
               { 0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
               {~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U },
               { 0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
               {~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U },
               { 0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
               {~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U },
               { 0U, ~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U },
               {~0U,  0U, ~0U,  0U, ~0U,  0U, ~0U,  0U }
            };
            static const D3D10_SUBRESOURCE_DATA InitialData = {Checker, sizeof Checker[0], sizeof Checker};
            '''
            print r'    _result = _this->CreateTexture2D(&Desc, &InitialData, (ID3D10Texture2D**)ppResource);'
            self.checkResult(method.type)
            return

        if method.name == 'Map':
            # Reset _DO_NOT_WAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('MapFlags')
            for flag in mapFlagsArg.type.values:
                if flag.endswith('_MAP_FLAG_DO_NOT_WAIT'):
                    print r'    MapFlags &= ~%s;' % flag

        Retracer.invokeInterfaceMethod(self, interface, method)

        # process events after presents
        if method.name == 'Present':
            print r'    d3dretrace::processEvents();'

        if method.name == 'Map':
            print '    _MAP_DESC _MapDesc;'
            print '    _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames())
            print '    size_t _MappedSize = _MapDesc.Size;'
            print '    if (_MapDesc.Size) {'
            if interface.name.startswith('ID3D11DeviceContext'):
                print '        _maps[pResource] = _MapDesc.pData;'
            else:
                print '        _maps[_this] = _MapDesc.pData;'
            print '    } else {'
            print '        return;'
            print '    }'
        
        if method.name == 'Unmap':
            if interface.name.startswith('ID3D11DeviceContext'):
                print '    VOID *_pbData = 0;'
                print '    _pbData = _maps[pResource];'
                print '    if (_pbData) {'
                print '        retrace::delRegionByPointer(_pbData);'
                print '        _maps[pResource] = 0;'
                print '    }'
            else:
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
    print r'#define INITGUID'
    print
    print r'#include <string.h>'
    print
    print r'#include <iostream>'
    print
    print r'#include "d3dretrace.hpp"'
    print

    moduleNames = sys.argv[1:]

    api = API()
    
    if moduleNames:
        print r'#include "d3dretrace_dxgi.hpp"'
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

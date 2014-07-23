##########################################################################
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
##########################################################################/


import sys
from dlltrace import DllTracer
from trace import getWrapperInterfaceName
from specs import stdapi
from specs.stdapi import API
from specs import dxgi
from specs import d3d10
from specs import d3d10_1
from specs import d3d11


class D3DCommonTracer(DllTracer):

    def serializeArgValue(self, function, arg):
        # Dump shaders as strings
        if isinstance(arg.type, stdapi.Blob) and arg.name.startswith('pShaderBytecode'):
            print '    DumpShader(trace::localWriter, %s, %s);' % (arg.name, arg.type.size)
            return

        # Serialize the swapchain dimensions
        if function.name == 'CreateSwapChain' and arg.name == 'pDesc' \
           or arg.name == 'pSwapChainDesc':
            print r'    DXGI_SWAP_CHAIN_DESC *_pSwapChainDesc = NULL;'
            print r'    DXGI_SWAP_CHAIN_DESC _SwapChainDesc;'
            print r'    if (%s) {' % arg.name
            print r'        _SwapChainDesc = *%s;' % arg.name
            if function.name != 'CreateSwapChain' or not self.interface.name.endswith('DWM'):
                # Obtain size from the window
                print r'        RECT _rect;'
                print r'        if (GetClientRect(%s->OutputWindow, &_rect)) {' % arg.name
                print r'            if (%s->BufferDesc.Width == 0) {' % arg.name
                print r'                _SwapChainDesc.BufferDesc.Width = _rect.right  - _rect.left;'
                print r'            }'
                print r'            if (%s->BufferDesc.Height == 0) {' % arg.name
                print r'                _SwapChainDesc.BufferDesc.Height = _rect.bottom - _rect.top;'
                print r'            }'
                print r'        }'
            else:
                # Obtain size from the output
                print r'        DXGI_OUTPUT_DESC _OutputDesc;'
                print r'        if (SUCCEEDED(pOutput->GetDesc(&_OutputDesc))) {'
                print r'            _SwapChainDesc.BufferDesc.Width  = _OutputDesc.DesktopCoordinates.right  - _OutputDesc.DesktopCoordinates.left;'
                print r'            _SwapChainDesc.BufferDesc.Height = _OutputDesc.DesktopCoordinates.bottom - _OutputDesc.DesktopCoordinates.top;'
                print r'        }'
            print r'        _pSwapChainDesc = &_SwapChainDesc;'
            print r'    }'
            self.serializeValue(arg.type, '_pSwapChainDesc')
            return

        # Serialize object names
        # http://blogs.msdn.com/b/chuckw/archive/2010/04/15/object-naming.aspx
        if function.name == 'SetPrivateData' and arg.name == 'pData':
            iid = function.args[0].name
            print r'    if (%s == WKPDID_D3DDebugObjectName) {' % iid
            print r'        trace::localWriter.writeString(static_cast<const char *>(pData), DataSize);'
            print r'    } else {'
            DllTracer.serializeArgValue(self, function, arg)
            print r'    }'
            return

        DllTracer.serializeArgValue(self, function, arg)

    # Interfaces that need book-keeping for maps
    mapInterfaces = (
        dxgi.IDXGISurface,
        d3d10.ID3D10Resource,
        d3d11.ID3D11Resource,
    )
    
    def enumWrapperInterfaceVariables(self, interface):
        variables = DllTracer.enumWrapperInterfaceVariables(self, interface)
        
        # Add additional members to track maps
        if interface.hasBase(*self.mapInterfaces):
            variables += [
                ('_MAP_DESC', '_MapDesc', None),
            ]

        return variables

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        if method.name in ('Map', 'Unmap'):
            # On D3D11 Map/Unmap is not a resource method, but a context method instead.
            resourceArg = method.getArgByName('pResource')
            if resourceArg is None:
                pResource = 'this'
            else:
                wrapperInterfaceName = getWrapperInterfaceName(resourceArg.type.type)
                print '    %s * _pResource = static_cast<%s*>(%s);' % (wrapperInterfaceName, wrapperInterfaceName, resourceArg.name)
                pResource = '_pResource'

        if method.name == 'Unmap':
            print '    _MAP_DESC _MapDesc = %s->_MapDesc;' % pResource
            #print r'    os::log("%%p -> %%p+%%lu\n", %s,_MapDesc.pData, (unsigned long)_MapDesc.Size);' % pResource
            print '    if (_MapDesc.Size && _MapDesc.pData) {'
            self.emit_memcpy('_MapDesc.pData', '_MapDesc.Size')
            print '    }'

        DllTracer.implementWrapperInterfaceMethodBody(self, interface, base, method)

        if method.name == 'Map':
            # NOTE: recursive locks are explicitely forbidden
            print '    _MAP_DESC _MapDesc;'
            print '    if (SUCCEEDED(_result)) {'
            print '        _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames())
            print '    } else {'
            print '        _MapDesc.pData = NULL;'
            print '        _MapDesc.Size = 0;'
            print '    }'
            #print r'    os::log("%%p <- %%p+%%lu\n", %s,_MapDesc.pData, (unsigned long)_MapDesc.Size);' % pResource
            print '    %s->_MapDesc = _MapDesc;' % pResource


if __name__ == '__main__':
    print r'#define INITGUID'
    print
    print r'#include "trace_writer_local.hpp"'
    print r'#include "os.hpp"'
    print
    print r'#include "d3dcommonshader.hpp"'
    print
    print r'#include "d3d10imports.hpp"'
    print r'#include "d3d10size.hpp"'
    print r'#include "d3d11imports.hpp"'
    print r'#include "d3d11size.hpp"'
    print

    api = API()
    api.addModule(dxgi.dxgi)
    api.addModule(d3d10.d3d10)
    api.addModule(d3d10_1.d3d10_1)
    api.addModule(d3d11.d3d11)

    tracer = D3DCommonTracer()
    tracer.traceApi(api)

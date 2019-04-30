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
from specs import d3d11
from specs import dcomp
from specs import d3d9


class D3DCommonTracer(DllTracer):

    def serializeArgValue(self, function, arg):
        # Dump shaders as strings
        if isinstance(arg.type, stdapi.Blob) and arg.name.startswith('pShaderBytecode'):
            print('    DumpShader(trace::localWriter, %s, %s);' % (arg.name, arg.type.size))
            return

        # Serialize the swap-chain dimensions
        if function.name.startswith('CreateSwapChain') and arg.name == 'pDesc' \
           or arg.name == 'pSwapChainDesc':
            assert isinstance(arg.type, stdapi.Pointer)
            descType = arg.type.type.mutable()
            print(r'    %s *_pSwapChainDesc = nullptr;' % descType)
            print(r'    %s _SwapChainDesc;' % descType)
            print(r'    if (%s) {' % arg.name)
            print(r'        _SwapChainDesc = *%s;' % arg.name)
            if self.interface is not None and self.interface.name.endswith('DWM'):
                # Obtain size from the output
                print(r'        assert(pOutput);')
                print(r'        DXGI_OUTPUT_DESC _OutputDesc;')
                print(r'        if (SUCCEEDED(pOutput->GetDesc(&_OutputDesc))) {')
                print(r'            _SwapChainDesc.BufferDesc.Width  = _OutputDesc.DesktopCoordinates.right  - _OutputDesc.DesktopCoordinates.left;')
                print(r'            _SwapChainDesc.BufferDesc.Height = _OutputDesc.DesktopCoordinates.bottom - _OutputDesc.DesktopCoordinates.top;')
                print(r'        }')
            elif function.name == 'CreateSwapChainForHwnd':
                # Obtain size from the window
                print(r'        RECT _rect;')
                print(r'        if (GetClientRect(hWnd, &_rect)) {')
                print(r'            if (%s->Width == 0) {' % arg.name)
                print(r'                _SwapChainDesc.Width = _rect.right  - _rect.left;')
                print(r'            }')
                print(r'            if (%s->Height == 0) {' % arg.name)
                print(r'                _SwapChainDesc.Height = _rect.bottom - _rect.top;')
                print(r'            }')
                print(r'        }')
            elif function.name.startswith('CreateSwapChainFor'):
                # TODO
                pass
            else:
                # Obtain size from the window
                print(r'        RECT _rect;')
                print(r'        if (GetClientRect(%s->OutputWindow, &_rect)) {' % arg.name)
                print(r'            if (%s->BufferDesc.Width == 0) {' % arg.name)
                print(r'                _SwapChainDesc.BufferDesc.Width = _rect.right  - _rect.left;')
                print(r'            }')
                print(r'            if (%s->BufferDesc.Height == 0) {' % arg.name)
                print(r'                _SwapChainDesc.BufferDesc.Height = _rect.bottom - _rect.top;')
                print(r'            }')
                print(r'        }')
            print(r'        _pSwapChainDesc = &_SwapChainDesc;')
            print(r'    }')
            self.serializeValue(arg.type, '_pSwapChainDesc')
            return

        # Serialize object names
        if function.name == 'SetPrivateData' and arg.name == 'pData':
            iid = function.args[0].name
            print(r'    if (%s == WKPDID_D3DDebugObjectName) {' % iid)
            print(r'        trace::localWriter.writeString(static_cast<const char *>(pData), DataSize);')
            print(r'    } else {')
            DllTracer.serializeArgValue(self, function, arg)
            print(r'    }')
            return

        DllTracer.serializeArgValue(self, function, arg)

    # Interfaces that need book-keeping for maps
    mapInterfaces = (
        dxgi.IDXGISurface,
        d3d10.ID3D10Resource,
    )
    
    def enumWrapperInterfaceVariables(self, interface):
        variables = DllTracer.enumWrapperInterfaceVariables(self, interface)
        
        # Add additional members to track maps
        if interface.hasBase(*self.mapInterfaces):
            variables += [
                ('_MAP_DESC', 'm_MapDesc', None),
                ('MemoryShadow', 'm_MapShadow', None),
            ]
        if interface.hasBase(d3d11.ID3D11DeviceContext):
            variables += [
                ('std::map< std::pair<ID3D11Resource *, UINT>, _MAP_DESC >', 'm_MapDescs', None),
                ('std::map< std::pair<ID3D11Resource *, UINT>, MemoryShadow >', 'm_MapShadows', None),
            ]
        if interface.hasBase(d3d11.ID3D11VideoContext):
            variables += [
                ('std::map<UINT, std::pair<void *, UINT> >', 'm_MapDesc', None),
            ]

        return variables

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        if method.getArgByName('pInitialData'):
            pDesc1 = method.getArgByName('pDesc1')
            if pDesc1 is not None:
                print(r'    %s pDesc = pDesc1;' % (pDesc1.type,))

        if method.name in ('Map', 'Unmap'):
            # On D3D11 Map/Unmap is not a resource method, but a context method instead.
            resourceArg = method.getArgByName('pResource')
            if resourceArg is None:
                print('    _MAP_DESC & _MapDesc = m_MapDesc;')
                print('    MemoryShadow & _MapShadow = m_MapShadow;')
                print('    %s *pResourceInstance = m_pInstance;' % interface.name)
            else:
                print(r'    static bool _warned = false;')
                print(r'    if (_this->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED && !_warned) {')
                print(r'        os::log("apitrace: warning: map with deferred context may not be realiably traced\n");')
                print(r'        _warned = true;')
                print(r'    }')
                print('    _MAP_DESC & _MapDesc = m_MapDescs[std::pair<%s, UINT>(pResource, Subresource)];' % resourceArg.type)
                print('    MemoryShadow & _MapShadow = m_MapShadows[std::pair<%s, UINT>(pResource, Subresource)];' % resourceArg.type)
                print('    Wrap%spResourceInstance = static_cast<Wrap%s>(%s);' % (resourceArg.type, resourceArg.type, resourceArg.name))

        if method.name == 'Unmap':
            print('    if (_MapDesc.Size && _MapDesc.pData) {')
            print('        if (_shouldShadowMap(pResourceInstance)) {')
            print('            _MapShadow.update(trace::fakeMemcpy);')
            print('        } else {')
            self.emit_memcpy('_MapDesc.pData', '_MapDesc.Size')
            print('        }')
            print('    }')

        if interface.hasBase(d3d11.ID3D11VideoContext) and \
           method.name == 'ReleaseDecoderBuffer':
            print('    std::map<UINT, std::pair<void *, UINT> >::iterator it = m_MapDesc.find(Type);')
            print('    if (it != m_MapDesc.end()) {')
            self.emit_memcpy('it->second.first', 'it->second.second')
            print('        m_MapDesc.erase(it);')
            print('    }')

        DllTracer.implementWrapperInterfaceMethodBody(self, interface, base, method)

        if method.name == 'Map':
            # NOTE: recursive locks are explicitely forbidden
            print('    if (SUCCEEDED(_result)) {')
            print('        _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames()))
            print('        if (_MapDesc.pData && _shouldShadowMap(pResourceInstance)) {')
            if interface.name.startswith('IDXGI'):
                print('            (void)_MapShadow;')
            else:
                print('            bool _discard = MapType == 4 /* D3D1[01]_MAP_WRITE_DISCARD */;')
                print('            _MapShadow.cover(_MapDesc.pData, _MapDesc.Size, _discard);')
            print('        }')
            print('    } else {')
            print('        _MapDesc.pData = NULL;')
            print('        _MapDesc.Size = 0;')
            print('    }')

        if interface.hasBase(d3d11.ID3D11VideoContext) and \
           method.name == 'GetDecoderBuffer':
            print('    if (SUCCEEDED(_result)) {')
            print('        m_MapDesc[Type] = std::make_pair(*ppBuffer, *pBufferSize);')
            print('    } else {')
            print('        m_MapDesc[Type] = std::make_pair(nullptr, 0);')
            print('    }')

    def invokeMethod(self, interface, base, method):
        DllTracer.invokeMethod(self, interface, base, method)

        # When D2D is used on top of WARP software rasterizer it seems to do
        # most of its rendering via the undocumented and opaque IWarpPrivateAPI
        # interface.  Althought hiding this interface will affect behavior,
        # there's little point in traces that use it, as they lack enough
        # information to replay correctly.
        #
        # Returning E_NOINTERFACE when for IID_IWarpPrivateAPI matches what
        # happens when D2D is used with drivers other than WARP.
        if method.name == 'QueryInterface':
            print(r'    if (_result == S_OK && riid == IID_IWarpPrivateAPI && ppvObj && *ppvObj) {')
            print(r'        static_cast<IUnknown *>(*ppvObj)->Release();')
            print(r'        *ppvObj = nullptr;')
            print(r'        _result = E_NOINTERFACE;')
            print(r'        os::log("apitrace: warning: hiding IWarpPrivateAPI interface\n");')
            print(r'    }')

        # Ensure buffers are initialized, otherwise we can fail to detect
        # changes when unititialized data matches what the app wrote.
        if method.name == 'CreateBuffer':
            print(r'    if (SUCCEEDED(_result) && !pInitialData) {')
            print(r'        _initializeBuffer(_this, pDesc, *ppBuffer);')
            print(r'    }')


if __name__ == '__main__':
    print(r'#include "guids_defs.hpp"')
    print()
    print(r'#include "trace_writer_local.hpp"')
    print(r'#include "os.hpp"')
    print()
    print(r'#include "dxgitrace.hpp"')
    print()

    api = API()
    api.addModule(dxgi.dxgi)
    api.addModule(d3d10.d3d10)
    api.addModule(d3d10.d3d10_1)
    api.addModule(d3d11.d3d11)
    api.addModule(dcomp.dcomp)
    api.addModule(d3d9.d3dperf)

    tracer = D3DCommonTracer()
    tracer.traceApi(api)

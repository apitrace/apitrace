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
        #if method.name == 'QueryInterface':
            #if interface.name.startswith('IDXGIAdapter'):
            #    print r'    if (riid == IID_CDXGIAdapter) {'
            #    self.invokeMethod(interface, base, method)
            #    print r'        return _result;'
            #    print r'    }'
            
            #print r'    if (riid == IID_IWarpPrivateAPI) {'
            #print r'        *ppvObj = NULL;'
            #print r'        return E_NOINTERFACE;'
            #print r'    }'

        # Replace ID3D1?Device::OpenSharedResource* calls with fake CreateTexture calls.
        if method.name.startswith('OpenSharedResource'):
            self.invokeMethod(interface, base, method)
            print r'    _fake_OpenSharedResource(_this, ReturnedInterface, ppResource, _result);'
            print r'    wrapIID("%s::%s", ReturnedInterface, ppResource);' % (interface.name, method.name)
            return

        if method.getArgByName('pInitialData'):
            pDesc1 = method.getArgByName('pDesc1')
            if pDesc1 is not None:
                print r'    %s pDesc = pDesc1;' % (pDesc1.type,)

        if method.name in ('Map', 'Unmap'):
            # On D3D11 Map/Unmap is not a resource method, but a context method instead.
            resourceArg = method.getArgByName('pResource')
            if resourceArg is None:
                print '    _MAP_DESC & _MapDesc = m_MapDesc;'
                print '    MemoryShadow & _MapShadow = m_MapShadow;'
                print '    %s *pResourceInstance = m_pInstance;' % interface.name
            else:
                print '    _MAP_DESC & _MapDesc = m_MapDescs[std::pair<%s, UINT>(pResource, Subresource)];' % resourceArg.type
                print '    MemoryShadow & _MapShadow = m_MapShadows[std::pair<%s, UINT>(pResource, Subresource)];' % resourceArg.type
                print '    Wrap%spResourceInstance = static_cast<Wrap%s>(%s);' % (resourceArg.type, resourceArg.type, resourceArg.name)

        if method.name == 'Unmap':
            print '    if (_MapDesc.Size && _MapDesc.pData) {'
            print '        if (_shouldShadowMap(pResourceInstance)) {'
            print '            _MapShadow.update(trace::fakeMemcpy);'
            print '        } else {'
            self.emit_memcpy('_MapDesc.pData', '_MapDesc.Size')
            print '        }'
            print '    }'

        if interface.hasBase(d3d11.ID3D11VideoContext) and \
           method.name == 'ReleaseDecoderBuffer':
            print '    std::map<UINT, std::pair<void *, UINT> >::iterator it = m_MapDesc.find(Type);'
            print '    if (it != m_MapDesc.end()) {'
            self.emit_memcpy('it->second.first', 'it->second.second')
            print '        m_MapDesc.erase(it);'
            print '    }'

        DllTracer.implementWrapperInterfaceMethodBody(self, interface, base, method)

        if method.name == 'Map':
            # NOTE: recursive locks are explicitely forbidden
            print '    if (SUCCEEDED(_result)) {'
            print '        _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames())
            print '        if (_MapDesc.pData && _shouldShadowMap(pResourceInstance)) {'
            if interface.name.startswith('IDXGI'):
                print '            (void)_MapShadow;'
            else:
                print '            bool _discard = MapType == 4 /* D3D1[01]_MAP_WRITE_DISCARD */;'
                print '            _MapShadow.cover(_MapDesc.pData, _MapDesc.Size, _discard);'
            print '        }'
            print '    } else {'
            print '        _MapDesc.pData = NULL;'
            print '        _MapDesc.Size = 0;'
            print '    }'

        if interface.hasBase(d3d11.ID3D11VideoContext) and \
           method.name == 'GetDecoderBuffer':
            print '    if (SUCCEEDED(_result)) {'
            print '        m_MapDesc[Type] = std::make_pair(*ppBuffer, *pBufferSize);'
            print '    } else {'
            print '        m_MapDesc[Type] = std::make_pair(nullptr, 0);'
            print '    }'

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
            print r'    if (_result == S_OK && riid == IID_IWarpPrivateAPI && ppvObj && *ppvObj) {'
            print r'        static_cast<IUnknown *>(*ppvObj)->Release();'
            print r'        *ppvObj = nullptr;'
            print r'        _result = E_NOINTERFACE;'
            print r'        os::log("apitrace: warning: hiding IWarpPrivateAPI interface\n");'
            print r'    }'

        # Ensure buffers are initialized, otherwise we can fail to detect
        # changes when unititialized data matches what the app wrote.
        if method.name == 'CreateBuffer':
            print r'    if (SUCCEEDED(_result) && !pInitialData) {'
            print r'        _initializeBuffer(_this, pDesc, *ppBuffer);'
            print r'    }'


if __name__ == '__main__':
    print r'#include "guids_defs.hpp"'
    print
    print r'#include "trace_writer_local.hpp"'
    print r'#include "os.hpp"'
    print
    print r'#include "dxgitrace.hpp"'
    print
    print r'''
static void
_fake_OpenSharedResource(ID3D10Device *pDevice, REFIID ReturnedInterface, void **ppResource, HRESULT _result);
static void
_fake_OpenSharedResource(ID3D11Device *pDevice, REFIID ReturnedInterface, void **ppResource, HRESULT _result);
    '''

    api = API()
    api.addModule(dxgi.dxgi)
    api.addModule(d3d10.d3d10)
    api.addModule(d3d10.d3d10_1)
    api.addModule(d3d11.d3d11)
    api.addModule(dcomp.dcomp)
    api.addModule(d3d9.d3dperf)

    tracer = D3DCommonTracer()
    tracer.traceApi(api)

print r'''

static void
_fake_QueryInterface(ID3D11Device *_this, REFIID riid, void * * ppvObj, HRESULT _result = S_OK) {
    static const char * _args[3] = {"this", "riid", "ppvObj"};
    static const trace::FunctionSig _sig = {10003, "ID3D11Device::QueryInterface", 3, _args};
    unsigned _call = trace::localWriter.beginEnter(&_sig, true);
    trace::localWriter.beginArg(0);
    trace::localWriter.writePointer((uintptr_t)_this);
    trace::localWriter.endArg();
    trace::localWriter.beginArg(1);
    trace::localWriter.beginStruct(&_structGUID_sig);
    trace::localWriter.writeUInt(riid.Data1);
    trace::localWriter.writeUInt(riid.Data2);
    trace::localWriter.writeUInt(riid.Data3);
    trace::localWriter.beginArray(8);
    for (size_t i = 0; i < 8; ++i) {
        trace::localWriter.beginElement();
        trace::localWriter.writeUInt(riid.Data4[i]);
        trace::localWriter.endElement();
    }
    trace::localWriter.endArray();
    trace::localWriter.endStruct();
    trace::localWriter.endArg();
    trace::localWriter.endEnter();
    trace::localWriter.beginLeave(_call);
    trace::localWriter.beginArg(2);
    if (ppvObj) {
        trace::localWriter.beginArray(1);
        trace::localWriter.beginElement();
        if (SUCCEEDED(_result)) {
            trace::localWriter.writePointer((uintptr_t)*ppvObj);
        } else {
            trace::localWriter.writeNull();
        }
        trace::localWriter.endElement();
        trace::localWriter.endArray();
    } else {
        trace::localWriter.writeNull();
    }
    trace::localWriter.endArg();
    trace::localWriter.beginReturn();
    trace::localWriter.writeEnum(&_enumHRESULT_sig, _result);
    trace::localWriter.endReturn();
    trace::localWriter.endLeave();
}


static void
_fake_OpenSharedResource(ID3D10Device *pDevice, REFIID ReturnedInterface, void ** ppResource, HRESULT _result) {
    HRESULT hr;

    if (ReturnedInterface == IID_ID3D10Texture2D) {
        ID3D10Resource *pResource = static_cast<ID3D10Resource *>(*ppResource);
        ID3D10Texture2D *pTexture2D = static_cast<ID3D10Texture2D *>(pResource);

        D3D10_TEXTURE2D_DESC Desc;
        pTexture2D->GetDesc(&Desc);
        assert(Desc.MipLevels == 1);
        assert(Desc.ArraySize == 1);
        assert(Desc.SampleDesc.Count == 1);
        assert(Desc.SampleDesc.Quality == 0);
        Desc.MiscFlags &= ~(D3D10_RESOURCE_MISC_SHARED|D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX);

        D3D10_SUBRESOURCE_DATA InitialData;
        D3D10_TEXTURE2D_DESC StagingDesc = Desc;
        StagingDesc.Usage = D3D10_USAGE_STAGING;
        StagingDesc.BindFlags = 0;
        StagingDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
        StagingDesc.MiscFlags &= D3D10_RESOURCE_MISC_TEXTURECUBE;
        com_ptr<ID3D10Texture2D> pStagingResource;
        hr = pDevice->CreateTexture2D(&StagingDesc, NULL, &pStagingResource);
        assert(SUCCEEDED(hr));
        pDevice->CopyResource(pStagingResource, pTexture2D);
        D3D10_MAPPED_TEXTURE2D MappedTex2D;
        hr = pStagingResource->Map(0, D3D10_MAP_READ, 0, &MappedTex2D);
        assert(SUCCEEDED(hr));
        InitialData.pSysMem = MappedTex2D.pData;
        InitialData.SysMemPitch = MappedTex2D.RowPitch;
        InitialData.SysMemSlicePitch = 0;

        const D3D10_TEXTURE2D_DESC * pDesc = &Desc;
        const D3D10_SUBRESOURCE_DATA * pInitialData = &InitialData;
        ID3D10Texture2D **ppTexture2D = &pTexture2D;

        // FIXME: Use the real signature.
        static const char * _args[4] = {"this", "pDesc", "pInitialData", "ppTexture2D"};
        static const trace::FunctionSig _sig = {10000, "ID3D10Device::CreateTexture2D", 4, _args};
        unsigned _call = trace::localWriter.beginEnter(&_sig, true);

        trace::localWriter.beginArg(0);
        trace::localWriter.writePointer((uintptr_t)pDevice);
        trace::localWriter.endArg();

        trace::localWriter.beginArg(1);
        trace::localWriter.beginArray(1);
        trace::localWriter.beginElement();
        trace::localWriter.beginStruct(&_structD3D10_TEXTURE2D_DESC_sig);
        trace::localWriter.writeUInt(pDesc->Width);
        trace::localWriter.writeUInt(pDesc->Height);
        trace::localWriter.writeUInt(pDesc->MipLevels);
        trace::localWriter.writeUInt(pDesc->ArraySize);
        trace::localWriter.writeEnum(&_enumDXGI_FORMAT_sig, pDesc->Format);
        trace::localWriter.beginStruct(&_structDXGI_SAMPLE_DESC_sig);
        trace::localWriter.writeUInt(pDesc->SampleDesc.Count);
        trace::localWriter.writeUInt(pDesc->SampleDesc.Quality);
        trace::localWriter.endStruct();
        trace::localWriter.writeEnum(&_enumD3D10_USAGE_sig, pDesc->Usage);
        trace::localWriter.writeUInt(pDesc->BindFlags);
        trace::localWriter.writeUInt(pDesc->CPUAccessFlags);
        trace::localWriter.writeUInt(pDesc->MiscFlags);
        trace::localWriter.endStruct();
        trace::localWriter.endElement();
        trace::localWriter.endArray();
        trace::localWriter.endArg();

        trace::localWriter.beginArg(2);
        if (pInitialData) {
            UINT NumSubResources = _getNumSubResources(pDesc);
            trace::localWriter.beginArray(NumSubResources);
            for (UINT SubResource = 0; SubResource < NumSubResources; ++SubResource) {
                trace::localWriter.beginElement();
                trace::localWriter.beginStruct(&_structD3D10_SUBRESOURCE_DATA_sig);
                trace::localWriter.writeBlob(pInitialData[SubResource].pSysMem, _calcSubresourceSize(pDesc, SubResource, pInitialData[SubResource].SysMemPitch, pInitialData[SubResource].SysMemSlicePitch));
                trace::localWriter.writeUInt(pInitialData[SubResource].SysMemPitch);
                trace::localWriter.writeUInt(pInitialData[SubResource].SysMemSlicePitch);
                trace::localWriter.endStruct();
                trace::localWriter.endElement();
            }
            trace::localWriter.endArray();
        } else {
            trace::localWriter.writeNull();
        }
        trace::localWriter.endArg();

        trace::localWriter.endEnter();

        trace::localWriter.beginLeave(_call);

        trace::localWriter.beginArg(3);
        trace::localWriter.beginArray(1);
        trace::localWriter.beginElement();
        trace::localWriter.writePointer((uintptr_t)*ppTexture2D);
        trace::localWriter.endElement();
        trace::localWriter.endArray();
        trace::localWriter.endArg();

        trace::localWriter.beginReturn();
        trace::localWriter.writeEnum(&_enumHRESULT_sig, _result);
        trace::localWriter.endReturn();

        trace::localWriter.endLeave();

        pStagingResource->Unmap(0);

        return;
    }

    os::log("apitrace: warning: unsupported IID in OpenSharedResource\n");
}

static void
_fake_OpenSharedResource(ID3D11Device *pDevice, REFIID ReturnedInterface, void ** ppResource, HRESULT _result) {
    HRESULT hr;

    if (ReturnedInterface == IID_ID3D10Texture2D) {
        ID3D10Device *pDevice10 = NULL;
        hr = pDevice->QueryInterface(IID_ID3D10Device, (void **) &pDevice10);
        assert(SUCCEEDED(hr));
        if (SUCCEEDED(hr)) {
            _fake_QueryInterface(pDevice, IID_ID3D10Device, (void **) &pDevice10, hr);
            _fake_OpenSharedResource(pDevice10, ReturnedInterface, ppResource, _result);
            pDevice10->Release();
        }
        return;
    }

    if (ReturnedInterface == IID_ID3D11Texture2D) {
        com_ptr<ID3D11DeviceContext> pDeviceContext;
        pDevice->GetImmediateContext(&pDeviceContext);
        assert(pDeviceContext);
        ID3D11Resource *pResource = static_cast<ID3D11Resource *>(*ppResource);
        ID3D11Texture2D *pTexture2D = static_cast<ID3D11Texture2D *>(pResource);

        D3D11_TEXTURE2D_DESC Desc;
        pTexture2D->GetDesc(&Desc);
        assert(Desc.MipLevels == 1);
        assert(Desc.ArraySize == 1);
        assert(Desc.SampleDesc.Count == 1);
        assert(Desc.SampleDesc.Quality == 0);
        Desc.MiscFlags &= ~(D3D11_RESOURCE_MISC_SHARED|D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX|D3D11_RESOURCE_MISC_SHARED_NTHANDLE);

        D3D11_SUBRESOURCE_DATA InitialData;
        D3D11_TEXTURE2D_DESC StagingDesc = Desc;
        StagingDesc.Usage = D3D11_USAGE_STAGING;
        StagingDesc.BindFlags = 0;
        StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        StagingDesc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
        com_ptr<ID3D11Texture2D> pStagingResource;
        hr = pDevice->CreateTexture2D(&StagingDesc, NULL, &pStagingResource);
        assert(SUCCEEDED(hr));
        pDeviceContext->CopyResource(pStagingResource, pTexture2D);
        D3D11_MAPPED_SUBRESOURCE MappedSubResource;
        hr = pDeviceContext->Map(pStagingResource, 0, D3D11_MAP_READ, 0, &MappedSubResource);
        assert(SUCCEEDED(hr));
        InitialData.pSysMem = MappedSubResource.pData;
        InitialData.SysMemPitch = MappedSubResource.RowPitch;
        InitialData.SysMemSlicePitch = MappedSubResource.DepthPitch;

        const D3D11_TEXTURE2D_DESC * pDesc = &Desc;
        const D3D11_SUBRESOURCE_DATA * pInitialData = &InitialData;
        ID3D11Texture2D **ppTexture2D = &pTexture2D;

        // FIXME: Use the real signature.
        static const char * _args[4] = {"this", "pDesc", "pInitialData", "ppTexture2D"};
        static const trace::FunctionSig _sig = {10001, "ID3D11Device::CreateTexture2D", 4, _args};
        unsigned _call = trace::localWriter.beginEnter(&_sig, true);

        trace::localWriter.beginArg(0);
        trace::localWriter.writePointer((uintptr_t)pDevice);
        trace::localWriter.endArg();

        trace::localWriter.beginArg(1);
        trace::localWriter.beginArray(1);
        trace::localWriter.beginElement();
        trace::localWriter.beginStruct(&_structD3D11_TEXTURE2D_DESC_sig);
        trace::localWriter.writeUInt(pDesc->Width);
        trace::localWriter.writeUInt(pDesc->Height);
        trace::localWriter.writeUInt(pDesc->MipLevels);
        trace::localWriter.writeUInt(pDesc->ArraySize);
        trace::localWriter.writeEnum(&_enumDXGI_FORMAT_sig, pDesc->Format);
        trace::localWriter.beginStruct(&_structDXGI_SAMPLE_DESC_sig);
        trace::localWriter.writeUInt(pDesc->SampleDesc.Count);
        trace::localWriter.writeUInt(pDesc->SampleDesc.Quality);
        trace::localWriter.endStruct();
        trace::localWriter.writeEnum(&_enumD3D11_USAGE_sig, pDesc->Usage);
        trace::localWriter.writeUInt(pDesc->BindFlags);
        trace::localWriter.writeUInt(pDesc->CPUAccessFlags);
        trace::localWriter.writeUInt(pDesc->MiscFlags);
        trace::localWriter.endStruct();
        trace::localWriter.endElement();
        trace::localWriter.endArray();
        trace::localWriter.endArg();

        trace::localWriter.beginArg(2);
        if (pInitialData) {
            UINT NumSubResources = _getNumSubResources(pDesc);
            trace::localWriter.beginArray(NumSubResources);
            for (UINT SubResource = 0; SubResource < NumSubResources; ++SubResource) {
                trace::localWriter.beginElement();
                trace::localWriter.beginStruct(&_structD3D11_SUBRESOURCE_DATA_sig);
                trace::localWriter.writeBlob(pInitialData[SubResource].pSysMem, _calcSubresourceSize(pDesc, SubResource, pInitialData[SubResource].SysMemPitch, pInitialData[SubResource].SysMemSlicePitch));
                trace::localWriter.writeUInt(pInitialData[SubResource].SysMemPitch);
                trace::localWriter.writeUInt(pInitialData[SubResource].SysMemSlicePitch);
                trace::localWriter.endStruct();
                trace::localWriter.endElement();
            }
            trace::localWriter.endArray();
        } else {
            trace::localWriter.writeNull();
        }
        trace::localWriter.endArg();

        trace::localWriter.endEnter();

        trace::localWriter.beginLeave(_call);

        trace::localWriter.beginArg(3);
        trace::localWriter.beginArray(1);
        trace::localWriter.beginElement();
        trace::localWriter.writePointer((uintptr_t)*ppTexture2D);
        trace::localWriter.endElement();
        trace::localWriter.endArray();
        trace::localWriter.endArg();

        trace::localWriter.beginReturn();
        trace::localWriter.writeEnum(&_enumHRESULT_sig, _result);
        trace::localWriter.endReturn();

        trace::localWriter.endLeave();

        pDeviceContext->Unmap(pStagingResource, 0);
        return;
    }

    os::log("apitrace: warning: unsupported IID in OpenSharedResource\n");
}
'''


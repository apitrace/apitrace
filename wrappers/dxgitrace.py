##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
# Copyright 2020 Joshua Ashton for Valve Software
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
from typing import cast
from dlltrace import DllTracer
from trace import getWrapperInterfaceName
from specs import stdapi
from specs.stdapi import API
from specs import dxgi
from specs import d3d10
from specs import d3d11
from specs import d3d12
from specs import dcomp
from specs import d3d9

def typeArrayOrPointer(arg, type_):
    return (isinstance(arg.type, stdapi.Pointer) and arg.type.type is type_) or \
           (isinstance(arg.type, stdapi.Pointer) and isinstance(arg.type.type, stdapi.Const) and arg.type.type.type is type_) or \
           (isinstance(arg.type, stdapi.Array) and arg.type.type is type_) or \
           (isinstance(arg.type, stdapi.Array) and isinstance(arg.type.type, stdapi.Const) and arg.type.type.type is type_)

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
        if interface.hasBase(d3d12.ID3D12DescriptorHeap):
            variables += [
                ('UINT64', 'm_DescriptorSlab', None),
            ]
        if interface.hasBase(d3d12.ID3D12Resource):
            variables += [
                ('D3D12_GPU_VIRTUAL_ADDRESS', 'm_FakeAddress', '0'),
                ('std::mutex', 'm_RefCountMutex', None)
            ]

        return variables

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        result_name = '_result'

        if interface.name.startswith('ID3D12'):
            # Ensure ordering for functions that interact with fences
            # Otherwise we can have races if we have eg.
            # GetCompletedValue and then Signal
            has_fence = False
            if method.sideeffects:
                if interface in (d3d12.ID3D12Fence, d3d12.ID3D12Fence1):
                    has_fence = True
                for arg in method.args:
                    if isinstance(arg.type, stdapi.ObjPointer) and arg.type.type in (d3d12.ID3D12Fence, d3d12.ID3D12Fence1):
                        has_fence = True
            if has_fence:
                print('    std::unique_lock<std::mutex> _ordering_lock = std::unique_lock<std::mutex>(g_D3D12FenceOrderingMutex);')

            if method.name == 'GetCPUDescriptorHandleForHeapStart':
                print('    D3D12_CPU_DESCRIPTOR_HANDLE _fake_result = D3D12_CPU_DESCRIPTOR_HANDLE { m_DescriptorSlab };')

            if method.name == 'GetGPUDescriptorHandleForHeapStart':
                print('    D3D12_GPU_DESCRIPTOR_HANDLE _fake_result = D3D12_GPU_DESCRIPTOR_HANDLE { m_DescriptorSlab };')

            if method.name == 'GetGPUVirtualAddress':
                print('    D3D12_GPU_VIRTUAL_ADDRESS _fake_result = m_FakeAddress;')
                print('    assert(_fake_result != 0);')

            if method.name == 'GetDescriptorHandleIncrementSize':
                print('    UINT _fake_result = _DescriptorIncrementSize;')

            if method.name in ('GetCPUDescriptorHandleForHeapStart', 'GetGPUDescriptorHandleForHeapStart', 'GetGPUVirtualAddress', 'GetDescriptorHandleIncrementSize'):
                result_name = '_fake_result'

            if method.name in ('Map', 'Unmap', 'ExecuteCommandLists'):
                print('    std::unique_lock<std::mutex> _ordering_lock = std::unique_lock<std::mutex>(g_D3D12AddressMappingsMutex);')

            if method.name == 'ExecuteCommandLists':
                print('    _flush_mappings();')

            # Disable raytracing (we don't support that right now.)
            # Needs GPU VAs in buffers and such.
            if method.name == 'CreateStateObject':
                print('    return E_NOTIMPL;')

            # Need to unmap the resource if the last public reference is
            # eliminated.
            if interface in (d3d12.ID3D12Resource, d3d12.ID3D12Resource1):
                if method.name == 'AddRef':
                    # Need to lock here to avoid another thread potentially
                    # releasing while we are flushing.
                    print('    std::unique_lock<std::mutex> _lock = std::unique_lock<std::mutex>(m_RefCountMutex);')
                if method.name == 'Release':
                    # Need to lock here to avoid another thread potentially
                    # releasing while we are flushing.
                    print('    std::unique_lock<std::mutex> _lock = std::unique_lock<std::mutex>(m_RefCountMutex);')
                    # TODO(Josh): Make this less... hacky to get the reference.
                    print('    m_pInstance->AddRef();')
                    print('    ULONG _current_ref = m_pInstance->Release();')
                    # If the current ref is 1, then the next one with be 0,
                    # therefore we need to unmap the resource here to
                    # avoid a dangling ptr.
                    print('    std::unique_lock<std::mutex> _ordering_lock;')
                    print('    if (_current_ref == 1) {')
                    print('        _unmap_resource(m_pInstance);')
                    print('        _ordering_lock = std::unique_lock<std::mutex>(g_D3D12AddressMappingsMutex);')
                    print('    }')

        if method.getArgByName('pInitialData'):
            pDesc1 = method.getArgByName('pDesc1')
            if pDesc1 is not None:
                print(r'    %s pDesc = pDesc1;' % (pDesc1.type,))

        if method.name in ('Map', 'Unmap') and not interface.name.startswith('ID3D12'):
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
            if interface.name.startswith('ID3D12'):
                print('    _unmap_resource(m_pInstance);')
            else:
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

        DllTracer.implementWrapperInterfaceMethodBodyEx(self, interface, base, method, result_name)

        if method.name in ('GetCPUDescriptorHandleForHeapStart', 'GetGPUDescriptorHandleForHeapStart', 'GetGPUVirtualAddress', 'GetDescriptorHandleIncrementSize'):
            print('    return _fake_result;')

        if method.name == 'Map':
            if interface.name.startswith('ID3D12'):
                print('    if (SUCCEEDED(_result) && ppData && *ppData)')
                print('        _map_resource(m_pInstance, *ppData);')
            else:
                # NOTE: recursive locks are explicitely forbidden
                print('    if (SUCCEEDED(_result)) {')
                print('        _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames()))
                print('        if (_MapDesc.pData && _shouldShadowMap(pResourceInstance)) {')
                if interface.name.startswith('IDXGI') or \
                method.getArgByName('MapType') is None:
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

        if interface.name.startswith('ID3D12'):
            if method.name == 'CreateDescriptorHeap':
                print('    if (SUCCEEDED(_result)) {')
                print('        WrapID3D12DescriptorHeap* _descriptor_heap_wrap = (*reinterpret_cast<WrapID3D12DescriptorHeap**>(ppvHeap));')
                print('        _descriptor_heap_wrap->m_DescriptorSlab = g_D3D12DescriptorSlabs.RegisterSlab(_descriptor_heap_wrap->m_pInstance->GetCPUDescriptorHandleForHeapStart(), _descriptor_heap_wrap->m_pInstance->GetGPUDescriptorHandleForHeapStart(), m_pInstance->GetDescriptorHandleIncrementSize(pDescriptorHeapDesc->Type));')
                print('    }')

            if method.name in ('CreateCommittedResource', 'CreateCommittedResource1'):
                # Create a fake GPU VA for buffers.
                print('    if (SUCCEEDED(_result) && pResourceDesc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {')
                print('        WrapID3D12Resource* _resource_wrap = (*reinterpret_cast<WrapID3D12Resource**>(ppvResource));')
                print('        _resource_wrap->m_FakeAddress = g_D3D12AddressSlabs.RegisterSlab(_resource_wrap->m_pInstance->GetGPUVirtualAddress());')
                print('    }')

            if method.name in ('CreatePlacedResource', 'CreatePlacedResource1'):
                # Create a fake GPU VA for buffers.
                print('    if (SUCCEEDED(_result) && pDesc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {')
                print('        WrapID3D12Resource* _resource_wrap = (*reinterpret_cast<WrapID3D12Resource**>(ppvResource));')
                print('        _resource_wrap->m_FakeAddress = g_D3D12AddressSlabs.RegisterSlab(_resource_wrap->m_pInstance->GetGPUVirtualAddress());')
                print('    }')

            if method.name in ('CopyTileMappings', 'UpdateTileMappings'):
                # Extract these out here as we need to potentially update the VA.
                if method.name == 'CopyTileMappings':
                    resource_name = 'pDstResource_original'
                else:
                    resource_name = 'pResource_original'
                print('    D3D12_RESOURCE_DESC _desc = %s->m_pInstance->GetDesc();' % resource_name)
                print('    if (_desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)')
                print('        %s->m_FakeAddress = g_D3D12AddressSlabs.RegisterSlab(%s->m_pInstance->GetGPUVirtualAddress());' % (resource_name, resource_name))

    def invokeFunction(self, function):
        if function.name.startswith('D3D12CreateDevice'):
            # Setup SEH to handle persistent mapping
            print(r'    _setup_seh();')
            print(r'    _setup_event_hooking();')

        DllTracer.invokeFunction(self, function)

    def lookupDescriptor(self, arg, _type, handler):
        if arg.type is _type:
            print(r'    %s = g_D3D12DescriptorSlabs.%s(%s);' % (arg.name, handler, arg.name))
        if typeArrayOrPointer(arg, _type):
            real_name = r'_real_%s' % arg.name
            array_length = 1
            if isinstance(arg.type, stdapi.Array):
                array_length = arg.type.length
            print(r'    %s* %s = (%s *) (%s > 1024 ? nullptr : alloca(sizeof(%s) * %s));' % (_type, real_name, _type, array_length, _type, array_length))
            print(r'    std::vector<%s> _vec_%s;' % (_type, real_name))
            print(r'    if (%s > 1024) {' % array_length)
            print(r'        _vec_%s.resize(%s);' % (real_name, array_length))
            print(r'        %s = _vec_%s.data();' % (real_name, real_name))
            print(r'    }')
            print(r'    if (%s != nullptr) {' % arg.name)
            print(r'        for (UINT _i = 0; _i < %s; _i++)' % array_length)
            print(r'            %s[_i] = g_D3D12DescriptorSlabs.%s(%s[_i]);' % (real_name, handler, arg.name))
            print(r'        %s = %s;' % (arg.name, real_name))
            print(r'    }')

    def invokeMethod(self, interface, base, method):
        if method.name == 'CreateBuffer':
            if interface.name.startswith('ID3D11'):
                print(r'    D3D11_SUBRESOURCE_DATA initialData;')
            else:
                print(r'    D3D10_SUBRESOURCE_DATA initialData;')
            print(r'    if (!pInitialData) {')
            print(r'        pInitialData = &initialData;')
            print(r'        _initialBufferAlloc(pDesc, &initialData);')
            print(r'    }')

        for arg in method.args:
            self.lookupDescriptor(arg, d3d12.D3D12_CPU_DESCRIPTOR_HANDLE, 'LookupCPUDescriptorHandle')
            self.lookupDescriptor(arg, d3d12.D3D12_GPU_DESCRIPTOR_HANDLE, 'LookupGPUDescriptorHandle')

            if arg.type is d3d12.D3D12_GPU_VIRTUAL_ADDRESS:
                print(r'    %s = g_D3D12AddressSlabs.LookupSlab(%s);' % (arg.name, arg.name))

        if interface.name.startswith('ID3D12'):
            if method.name == 'SetEventOnCompletion':
                print('     {')
                print('         auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);')
                print('         auto _fence_event_iter = g_D3D12FenceEventMap.find(hEvent);')
                print('         if (_fence_event_iter == g_D3D12FenceEventMap.end())')
                print('             g_D3D12FenceEventMap.insert(std::make_pair(hEvent, hEvent));')
                print('     }')

            if method.name == 'IASetVertexBuffers':
                # TODO(Josh): MAKE THIS AUTOGEN
                print('    D3D12_VERTEX_BUFFER_VIEW _real_views[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];')
                print('    if (pViews != nullptr) {')
                print('        for (UINT i = 0; i < NumViews; i++) {')
                print('            _real_views[i] = pViews[i];')
                print('            _real_views[i].BufferLocation = g_D3D12AddressSlabs.LookupSlab(_real_views[i].BufferLocation);')
                print('        }')
                print('        pViews = _real_views;')
                print('    }')

            if method.name == 'IASetIndexBuffer':
                # TODO(Josh): MAKE THIS AUTOGEN
                print('    D3D12_INDEX_BUFFER_VIEW _real_view;')
                print('    if (pView) {')
                print('        _real_view = *pView;')
                print('        _real_view.BufferLocation = g_D3D12AddressSlabs.LookupSlab(_real_view.BufferLocation);')
                print('        pView = &_real_view;')
                print('    }')

            if method.name == 'CreateConstantBufferView':
                # TODO(Josh): MAKE THIS AUTOGEN
                print('    D3D12_CONSTANT_BUFFER_VIEW_DESC _real_desc;')
                print('    if (pDesc) {')
                print('        _real_desc = *pDesc;')
                print('        _real_desc.BufferLocation = g_D3D12AddressSlabs.LookupSlab(_real_desc.BufferLocation);')
                print('        pDesc = &_real_desc;')
                print('    }')

            if method.name == 'WriteBufferImmediate':
                # TODO(Josh): MAKE THIS AUTOGEN
                print('    for (UINT i = 0; i < Count; i++) {')
                print('         D3D12_WRITEBUFFERIMMEDIATE_PARAMETER _param = pParams[i];')
                print('         _param.Dest = g_D3D12AddressSlabs.LookupSlab(_param.Dest);')
                print('        _this->WriteBufferImmediate(1, &_param, &pModes[i]);')
                print('    }')

            if method.name in ('CreateCommittedResource', 'CreateCommittedResource1'):
                print('    D3D12_HEAP_PROPERTIES _heap_properties = *pHeapProperties;')
                print('    if (pHeapProperties->Type == D3D12_HEAP_TYPE_UPLOAD)')
                print('        HeapFlags |= D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;')
                print('    if (pHeapProperties->Type == D3D12_HEAP_TYPE_CUSTOM) {')
                # Enable WRITE_WATCH for the resource
                print('        if (pHeapProperties->CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || pHeapProperties->CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK)')
                print('            HeapFlags |= D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;')
                print('    }')
                print('    pHeapProperties = &_heap_properties;')

            if method.name in ('CreateHeap', 'CreateHeap1'):
                print('    D3D12_HEAP_DESC _heap_desc = *pDesc;')
                print('    if (_heap_desc.Properties.Type == D3D12_HEAP_TYPE_UPLOAD)')
                print('        _heap_desc.Flags |= D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;')
                print('    if (_heap_desc.Properties.Type == D3D12_HEAP_TYPE_CUSTOM) {')
                # Enable WRITE_WATCH for the resource
                print('        if (_heap_desc.Properties.CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || _heap_desc.Properties.CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK)')
                print('            _heap_desc.Flags |= D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;')
                print('    }')
                print('    pDesc = &_heap_desc;')

        if method.name != 'WriteBufferImmediate':
            DllTracer.invokeMethod(self, interface, base, method)

        if interface.name.startswith('ID3D12'):
            if method.name == 'GetHeapProperties':
                # Hide write watch from the application
                print('    if (pHeapFlags)')
                print('        *pHeapFlags &= ~D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;')

            # Disable raytracing (we don't support that right now.)
            # Needs GPU VAs in buffers and such.
            if method.name == 'CheckFeatureSupport':
                print('    if (Feature == D3D12_FEATURE_D3D12_OPTIONS5) {')
                #print('        reinterpret_cast<D3D12_FEATURE_DATA_D3D12_OPTIONS5*>(pFeatureSupportData)->RaytracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;')
                print('    }')

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

        if interface.name.startswith('ID3D11Device') and method.name == 'CheckFeatureSupport':
            print(r'    if (FORCE_D3D_FEATURE_LEVEL_11_0 &&')
            print(r'        _result == S_OK &&')
            print(r'        Feature >= D3D11_FEATURE_D3D11_OPTIONS) {')
            print(r'        ZeroMemory(pFeatureSupportData, FeatureSupportDataSize);')
            print(r'    }')

        if method.name == 'CheckMultisampleQualityLevels':
            print(r'    if (FORCE_D3D_FEATURE_LEVEL_11_0 &&')
            print(r'        _result == S_OK &&')
            print(r'        pNumQualityLevels && *pNumQualityLevels > 1) {')
            print(r'        *pNumQualityLevels = 1;')
            print(r'    }')

        # Ensure buffers are initialized, otherwise we can fail to detect
        # changes when unititialized data matches what the app wrote.
        if method.name == 'CreateBuffer':
            print(r'    if (pInitialData == &initialData) {')
            print(r'        _initialBufferFree(&initialData);')
            print(r'    }')


if __name__ == '__main__':
    print(r'#include "guids_defs.hpp"')
    print()
    print(r'#include "trace_writer_local.hpp"')
    print(r'#include "os.hpp"')
    print()
    print(r'#include "dxgitrace.hpp"')
    print()

    # TODO: Expose this via a runtime option
    print('#define FORCE_D3D_FEATURE_LEVEL_11_0 0')

    print('_D3D12_DESCRIPTOR_TRACER g_D3D12DescriptorSlabs;')
    print('_D3D12_ADDRESS_SLAB_TRACER<D3D12_GPU_VIRTUAL_ADDRESS> g_D3D12AddressSlabs;')

    print('std::mutex g_D3D12AddressMappingsMutex;')
    print('std::map<SIZE_T, _D3D12_MAP_DESC> g_D3D12AddressMappings;')

    print('std::map<HANDLE, HANDLE> g_D3D12FenceEventMap;')
    print('std::mutex g_D3D12FenceEventMapMutex;')

    print('std::mutex g_D3D12FenceOrderingMutex;')

    api = API()
    api.addModule(dxgi.dxgi)
    api.addModule(d3d10.d3d10)
    api.addModule(d3d10.d3d10_1)
    api.addModule(d3d11.d3d11)
    api.addModule(d3d12.d3d12)
    api.addModule(dcomp.dcomp)
    api.addModule(d3d9.d3dperf)

    tracer = D3DCommonTracer()
    tracer.traceApi(api)

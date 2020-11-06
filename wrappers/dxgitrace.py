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
                ('D3D12_CPU_DESCRIPTOR_HANDLE', 'm_DescriptorCPUSlab', None),
                ('D3D12_GPU_DESCRIPTOR_HANDLE', 'm_DescriptorGPUSlab', None),
            ]
        if interface.hasBase(d3d12.ID3D12Resource):
            variables += [
                ('D3D12_GPU_VIRTUAL_ADDRESS', 'm_FakeAddress', None),
            ]

        return variables

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
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
                print('    _unmap_resource(this);')
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

        if method.name == 'GetCPUDescriptorHandleForHeapStart':
            print('    D3D12_CPU_DESCRIPTOR_HANDLE _fake_result = m_DescriptorCPUSlab;')

        if method.name == 'GetGPUDescriptorHandleForHeapStart':
            print('    D3D12_GPU_DESCRIPTOR_HANDLE _fake_result = m_DescriptorGPUSlab;')

        if method.name == 'GetGPUVirtualAddress':
            print('    D3D12_GPU_VIRTUAL_ADDRESS _fake_result = m_FakeAddress;')

        if method.name in ('GetCPUDescriptorHandleForHeapStart', 'GetGPUDescriptorHandleForHeapStart', 'GetGPUVirtualAddress'):
            result_name = '_fake_result'
        else:
            result_name = '_result'

        if method.name == 'ExecuteCommandLists':
            print('    _flush_mappings();')

        DllTracer.implementWrapperInterfaceMethodBodyEx(self, interface, base, method, result_name)

        if method.name in ('GetCPUDescriptorHandleForHeapStart', 'GetGPUDescriptorHandleForHeapStart', 'GetGPUVirtualAddress'):
            print('    return _fake_result;')

        if method.name == 'Map':
            if interface.name.startswith('ID3D12'):
                print('    if (SUCCEEDED(_result) && ppData && *ppData) {')
                print('        SIZE_T key = reinterpret_cast<SIZE_T>(this);')
                print('        auto _lock = std::unique_lock<std::mutex>(g_D3D12AddressMappingsMutex);')
                print('        if (g_D3D12AddressMappings.find(key) == g_D3D12AddressMappings.end()) {')
                print('            MEMORY_BASIC_INFORMATION _memory_info;')
                print('            VirtualQuery(*ppData, &_memory_info, sizeof(_memory_info));')
                # TODO(Josh): Is this assertion true for placed resources...?
                # I'm not sure, review me if this ever triggers.
                print('            assert(*ppData == _memory_info.BaseAddress);')
                print('            _D3D12_MAP_DESC _map_desc;')
                print('            _map_desc.pData = _memory_info.BaseAddress;')
                print('            _map_desc.Size = static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(_memory_info.BaseAddress) - reinterpret_cast<uintptr_t>(*ppData)) + _getMapSize(this);')
                print('            if (_guard_mapped_memory(_map_desc, &_map_desc.OldProtect))')
                print('                g_D3D12AddressMappings.insert(std::make_pair(key, _map_desc));')
                print('        }')
                print('    }')
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

        if method.name == 'CreateDescriptorHeap':
            print('    if (SUCCEEDED(_result)) {')
            print('        WrapID3D12DescriptorHeap* _descriptor_heap_wrap = (*reinterpret_cast<WrapID3D12DescriptorHeap**>(ppvHeap));')
            print('        _descriptor_heap_wrap->m_DescriptorCPUSlab = g_D3D12DescriptorCPUSlabs.RegisterSlab(_descriptor_heap_wrap->m_pInstance->GetCPUDescriptorHandleForHeapStart());')
            print('        _descriptor_heap_wrap->m_DescriptorGPUSlab = g_D3D12DescriptorGPUSlabs.RegisterSlab(_descriptor_heap_wrap->m_pInstance->GetGPUDescriptorHandleForHeapStart());')
            print('    }')

        if method.name == 'CreateCommittedResource':
            # Create a fake GPU VA for buffers.
            print('    if (SUCCEEDED(_result) && pResourceDesc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {')
            print('        WrapID3D12Resource* _resource_wrap = (*reinterpret_cast<WrapID3D12Resource**>(ppvResource));')
            print('        _resource_wrap->m_FakeAddress = g_D3D12AddressSlabs.RegisterSlab(_resource_wrap->m_pInstance->GetGPUVirtualAddress());')
            print('    }')

    def invokeFunction(self, function):
        if function.name.startswith('D3D12CreateDevice'):
            # Setup SEH to handle persistent mapping
            print(r'    _setup_seh();')
            print(r'    _setup_event_hooking();')

        DllTracer.invokeFunction(self, function)

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
            # TODO(Josh): Clean me!!!!!!!!
            if arg.type is d3d12.D3D12_CPU_DESCRIPTOR_HANDLE:
                print(r'    %s = g_D3D12DescriptorCPUSlabs.LookupSlab(%s);' % (arg.name, arg.name))
            if (isinstance(arg.type, stdapi.Pointer) and arg.type.type is d3d12.D3D12_CPU_DESCRIPTOR_HANDLE) or \
               (isinstance(arg.type, stdapi.Pointer) and isinstance(arg.type.type, stdapi.Const) and arg.type.type.type is d3d12.D3D12_CPU_DESCRIPTOR_HANDLE):
                real_name = r'_real_%s' % arg.name
                print(r'    D3D12_CPU_DESCRIPTOR_HANDLE %s;' % real_name)
                print(r'    if (%s != nullptr) {' % arg.name)
                print(r'        %s = g_D3D12DescriptorCPUSlabs.LookupSlab(*%s);' % (real_name, arg.name))
                print(r'        %s = &%s;' % (arg.name, real_name))
                print(r'    }')

            if arg.type is d3d12.D3D12_GPU_DESCRIPTOR_HANDLE:
                print(r'    %s = g_D3D12DescriptorGPUSlabs.LookupSlab(%s);' % (arg.name, arg.name))
            if (isinstance(arg.type, stdapi.Pointer) and arg.type.type is d3d12.D3D12_GPU_DESCRIPTOR_HANDLE) or \
               (isinstance(arg.type, stdapi.Pointer) and isinstance(arg.type.type, stdapi.Const) and arg.type.type.type is d3d12.D3D12_GPU_DESCRIPTOR_HANDLE):
                real_name = r'_real_%s' % arg.name
                print(r'    D3D12_GPU_DESCRIPTOR_HANDLE %s;' % real_name)
                print(r'    if (%s != nullptr) {' % arg.name)
                print(r'        %s = g_D3D12DescriptorGPUSlabs.LookupSlab(*%s);' % (real_name, arg.name))
                print(r'        %s = &%s;' % (arg.name, real_name))
                print(r'    }')

            if arg.type is d3d12.D3D12_GPU_VIRTUAL_ADDRESS:
                print(r'    %s = g_D3D12AddressSlabs.LookupSlab(%s);' % (arg.name, arg.name))

        if interface.name.startswith('ID3D12'):
            if method.name == 'OMSetRenderTargets':
                # TODO(Josh): MAKE THIS AUTOGEN
                print('    assert(NumRenderTargetDescriptors <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);')
                print('    D3D12_CPU_DESCRIPTOR_HANDLE _real_render_target_descriptors[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];')
                print('    for (UINT i = 0; i < NumRenderTargetDescriptors; i++) {')
                print('        _real_render_target_descriptors[i] = g_D3D12DescriptorCPUSlabs.LookupSlab(pRenderTargetDescriptors[i]);')
                print('    }')
                print('    pRenderTargetDescriptors = _real_render_target_descriptors;')

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
                print('    for (UINT i = 0; i < NumViews; i++) {')
                print('        _real_views[i] = pViews[i];')
                print('        _real_views[i].BufferLocation = g_D3D12AddressSlabs.LookupSlab(_real_views[i].BufferLocation);')
                print('    }')
                print('    pViews = _real_views;')

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

        if method.name == 'CreateCommittedResource':
            # Disable WRITE_COMBINE for upload heap so we can use PAGE_GUARD
            # to handle persistently mapped resources.
            print('    D3D12_HEAP_PROPERTIES _heap_properties = *pHeapProperties;')
            print('    if (pHeapProperties->Type == D3D12_HEAP_TYPE_UPLOAD) {')
            print('        _heap_properties.Type = D3D12_HEAP_TYPE_CUSTOM;')
            print('        _heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;')
            print('        _heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;')
            print('        _heap_properties.CreationNodeMask = 0;')
            print('        _heap_properties.VisibleNodeMask = 0;')
            print('        pHeapProperties = &_heap_properties;')
            print('    }')

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
    print('_D3D12_ADDRESS_SLAB_TRACER<D3D12_CPU_DESCRIPTOR_HANDLE, SIZE_T> g_D3D12DescriptorCPUSlabs;')
    print('_D3D12_ADDRESS_SLAB_TRACER<D3D12_GPU_DESCRIPTOR_HANDLE, UINT64> g_D3D12DescriptorGPUSlabs;')
    print('_D3D12_ADDRESS_SLAB_TRACER<D3D12_GPU_VIRTUAL_ADDRESS,   UINT64> g_D3D12AddressSlabs;')

    print('std::mutex g_D3D12AddressMappingsMutex;')
    print('std::map<SIZE_T, _D3D12_MAP_DESC> g_D3D12AddressMappings;')

    print('std::map<HANDLE, HANDLE> g_D3D12FenceEventMap;')
    print('std::mutex g_D3D12FenceEventMapMutex;')

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

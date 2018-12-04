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
from specs.winapi import LPCSTR
from specs.dxgi import dxgi
from specs.d3d10 import d3d10, d3d10_1
from specs.d3d11 import d3d11
from specs.dcomp import dcomp


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print '// Swizzling mapping for lock addresses, mapping a (pDeviceContext, pResource, Subresource) -> void *'
        print 'typedef std::pair< IUnknown *, UINT > SubresourceKey;'
        print 'static std::map< IUnknown *, std::map< SubresourceKey, void * > > g_Maps;'
        print
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
                print r'    d3dretrace::createWindowForSwapChain(pSwapChainDesc);'

            # Compensate for the fact we don't trace DXGI object creation
            if function.name.startswith('D3D11CreateDevice'):
                print r'    if (DriverType == D3D_DRIVER_TYPE_UNKNOWN && !pAdapter) {'
                print r'        DriverType = D3D_DRIVER_TYPE_HARDWARE;'
                print r'    }'

            if function.name.startswith('D3D10CreateDevice'):
                # Toggle debugging
                print r'    if (retrace::debug >= 2) {'
                print r'        Flags |= D3D10_CREATE_DEVICE_DEBUG;'
                print r'    } else if (retrace::debug < 0) {'
                print r'        Flags &= ~D3D10_CREATE_DEVICE_DEBUG;'
                print r'    }'

                # Force driver
                self.forceDriver('D3D10_DRIVER_TYPE')

            if function.name.startswith('D3D11CreateDevice'):
                # Toggle debugging
                print r'    if (retrace::debug >= 2) {'
                print r'        Flags |= D3D11_CREATE_DEVICE_DEBUG;'
                print r'    } else if (retrace::debug < 0) {'
                print r'        Flags &= ~D3D11_CREATE_DEVICE_DEBUG;'
                print r'    }'
                print r'    if (IsWindows8OrGreater()) {'
                print r'        Flags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;'
                print r'    }'

                # Force driver
                self.forceDriver('D3D_DRIVER_TYPE')

        Retracer.invokeFunction(self, function)

    def doInvokeFunction(self, function):
        Retracer.doInvokeFunction(self, function)

        # Handle missing debug layer.  While it's possible to detect whether
        # the debug layers are present, by creating a null device, and checking
        # the result.  It's simpler to retry.
        if function.name.startswith('D3D10CreateDevice'):
            print r'        if ((_result == E_FAIL || _result == DXGI_ERROR_SDK_COMPONENT_MISSING) && (Flags & D3D10_CREATE_DEVICE_DEBUG)) {'
            print r'            retrace::warning(call) << "Direct3D 10.x SDK Debug Layer (d3d10sdklayers.dll) not available, continuing without debug output\n";'
            print r'            Flags &= ~D3D10_CREATE_DEVICE_DEBUG;'
            Retracer.doInvokeFunction(self, function)
            print r'        }'
        if function.name.startswith('D3D11CreateDevice'):
            print r'        if ((_result == E_FAIL || _result == DXGI_ERROR_SDK_COMPONENT_MISSING) && (Flags & D3D11_CREATE_DEVICE_DEBUG)) {'
            print r'            retrace::warning(call) << "Direct3D 11.x SDK Debug Layer (d3d11*sdklayers.dll) not available, continuing without debug output\n";'
            print r'            Flags &= ~D3D11_CREATE_DEVICE_DEBUG;'
            Retracer.doInvokeFunction(self, function)
            print r'        }'

    def handleFailure(self, interface, methodOrFunction):
        # Catch when device is removed, and report the reason.
        if interface is not None:
            print r'        if (_result == DXGI_ERROR_DEVICE_REMOVED) {'
            getDeviceRemovedReasonMethod = interface.getMethodByName("GetDeviceRemovedReason")
            if getDeviceRemovedReasonMethod is not None:
                print r'            HRESULT _reason = _this->GetDeviceRemovedReason();'
                print r'            retrace::failed(call, _reason);'
            getDeviceMethod = interface.getMethodByName("GetDevice")
            if getDeviceMethod is not None and len(getDeviceMethod.args) == 1:
                print r'            com_ptr<%s> _pDevice;' % getDeviceMethod.args[0].type.type.type
                print r'            _this->GetDevice(&_pDevice);'
                print r'            HRESULT _reason = _pDevice->GetDeviceRemovedReason();'
                print r'            retrace::failed(call, _reason);'
            print r'            exit(EXIT_FAILURE);'
            print r'        }'

        Retracer.handleFailure(self, interface, methodOrFunction)

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

    def doInvokeInterfaceMethod(self, interface, method):
        Retracer.doInvokeInterfaceMethod(self, interface, method)

        # Force driver
        if interface.name.startswith('IDXGIFactory') and method.name.startswith('EnumAdapters'):
            print r'    const char *szSoftware = NULL;'
            print r'    switch (retrace::driver) {'
            print r'    case retrace::DRIVER_REFERENCE:'
            print r'        szSoftware = "d3d11ref.dll";'
            print r'        break;'
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
            print r'        if (Adapter != 0) {'
            print r'            retrace::warning(call) << "ignoring non-default adapter " << Adapter << "\n";'
            print r'            Adapter = 0;'
            print r'        }'
            Retracer.doInvokeInterfaceMethod(self, interface, method)
            print r'    }'
            return

        if interface.name.startswith('IDXGIFactory') and method.name == 'CreateSoftwareAdapter':
            print r'    const char *szSoftware = NULL;'
            print r'    switch (retrace::driver) {'
            print r'    case retrace::DRIVER_REFERENCE:'
            print r'        szSoftware = "d3d11ref.dll";'
            print r'        break;'
            print r'    case retrace::DRIVER_MODULE:'
            print r'        szSoftware = retrace::driverModule;'
            print r'        break;'
            print r'    case retrace::DRIVER_SOFTWARE:'
            print r'    default:'
            print r'        szSoftware = "d3d10warp.dll";'
            print r'        break;'
            print r'    }'
            print r'    Module = LoadLibraryA("d3d10warp");'
            print r'    if (!Module) {'
            print r'        retrace::warning(call) << "failed to load " << szSoftware << "\n";'
            print r'    }'
            Retracer.doInvokeInterfaceMethod(self, interface, method)

        # Keep retrying ID3D11VideoContext::DecoderBeginFrame when returns E_PENDING
        if interface.name == 'ID3D11VideoContext' and method.name == 'DecoderBeginFrame':
            print r'    while (_result == D3DERR_WASSTILLDRAWING || _result == E_PENDING) {'
            print r'        Sleep(1);'
            Retracer.doInvokeInterfaceMethod(self, interface, method)
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
        if interface.name.startswith('ID3D11DeviceContext'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d11Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    d3d11Dumper.bindDevice(_this);'

        # intercept private interfaces
        if method.name == 'QueryInterface':
            print r'    if (!d3dretrace::overrideQueryInterface(_this, riid, ppvObj, &_result)) {'
            Retracer.invokeInterfaceMethod(self, interface, method)
            print r'    }'
            return

        # create windows as neccessary
        if method.name == 'CreateSwapChain':
            print r'    d3dretrace::createWindowForSwapChain(pDesc);'
        if method.name == 'CreateSwapChainForHwnd':
            print r'    hWnd = d3dretrace::createWindow(pDesc->Width, pDesc->Height);'
            print r'    // DXGI_SCALING_NONE is only supported on Win8 and beyond'
            print r'    if (pDesc->Scaling == DXGI_SCALING_NONE && !IsWindows8OrGreater()) {'
            print r'        pDesc->Scaling = DXGI_SCALING_STRETCH;'
            print r'    }'
        if method.name == 'CreateSwapChainForComposition':
            print r'    HWND hWnd = d3dretrace::createWindow(pDesc->Width, pDesc->Height);'
            print r'    _result = _this->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, NULL, pRestrictToOutput, ppSwapChain);'
            self.checkResult(interface, method)
            return
        if method.name == 'CreateTargetForHwnd':
            print r'    hwnd = d3dretrace::createWindow(1024, 768);'

        if method.name == 'SetFullscreenState':
            print r'    if (retrace::forceWindowed) {'
            print r'         DXGI_SWAP_CHAIN_DESC Desc;'
            print r'         _this->GetDesc(&Desc);'
            print r'         if (Desc.BufferDesc.Format != DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) {'
            print r'             Fullscreen = FALSE;'
            print r'             pTarget = nullptr;'
            print r'        }'
            print r'    }'

        # notify frame has been completed
        if interface.name.startswith('IDXGISwapChain') and method.name.startswith('Present'):
            if interface.name.startswith('IDXGISwapChainDWM'):
                print r'    com_ptr<IDXGISwapChain> pSwapChain;'
                print r'    if (SUCCEEDED(_this->QueryInterface(IID_IDXGISwapChain, (void **) &pSwapChain))) {'
                print r'        dxgiDumper.bindDevice(pSwapChain);'
                print r'    } else {'
                print r'        assert(0);'
                print r'    }'
            else:
                print r'    dxgiDumper.bindDevice(_this);'
            print r'    retrace::frameComplete(call);'

        if 'pSharedResource' in method.argNames():
            print r'    if (pSharedResource) {'
            print r'        retrace::warning(call) << "shared surfaces unsupported\n";'
            print r'        pSharedResource = NULL;'
            print r'    }'

        if interface.name.startswith('ID3D10Device') and method.name.startswith('OpenSharedResource'):
            print r'    retrace::warning(call) << "replacing shared resource with checker pattern\n";'
            print r'    _result = d3dretrace::createSharedResource(_this, ReturnedInterface, ppResource);'
            self.checkResult(interface, method)
            return
        if interface.name.startswith('ID3D11Device') and method.name == 'OpenSharedResource':
            # Some applications (e.g., video playing in IE11) create shared resources within the same process.
            # TODO: Generalize to other OpenSharedResource variants
            print r'    retrace::map<HANDLE>::const_iterator it = _shared_handle_map.find(hResource);'
            print r'    if (it == _shared_handle_map.end()) {'
            print r'        retrace::warning(call) << "replacing shared resource with checker pattern\n";'
            print r'        _result = d3dretrace::createSharedResource(_this, ReturnedInterface, ppResource);'
            self.checkResult(interface, method)
            print r'    } else {'
            print r'        hResource = it->second;'
            Retracer.invokeInterfaceMethod(self, interface, method)
            print r'    }'
            return
        if interface.name.startswith('ID3D11Device') and method.name.startswith('OpenSharedResource'):
            print r'    retrace::warning(call) << "replacing shared resource with checker pattern\n";'
            print r'    _result = d3dretrace::createSharedResource(_this, ReturnedInterface, ppResource);'
            if method.name == 'OpenSharedResourceByName':
                print r'    (void)lpName;'
                print r'    (void)dwDesiredAccess;'
            else:
                print r'    (void)hResource;'
            self.checkResult(interface, method)
            return

        if method.name == 'Map':
            # Reset _DO_NOT_WAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('MapFlags')
            for flag in mapFlagsArg.type.values:
                if flag.endswith('_MAP_FLAG_DO_NOT_WAIT'):
                    print r'    MapFlags &= ~%s;' % flag

        if method.name.startswith('UpdateSubresource'):
            # The D3D10 debug layer is buggy (or at least inconsistent with the
            # runtime), as it seems to estimate and enforce the data size based on the
            # SrcDepthPitch, even for non 3D textures, but in some traces
            # SrcDepthPitch is garbagge for non 3D textures.
            # XXX: It also seems to expect padding bytes at the end of the last
            # row, but we never record (or allocate) those...
            print r'    if (retrace::debug >= 2 && pDstBox && pDstBox->front == 0 && pDstBox->back == 1) {'
            print r'        SrcDepthPitch = 0;'
            print r'    }'

        if method.name == 'SetGammaControl':
            # This method is only supported while in full-screen mode
            print r'    if (retrace::forceWindowed) {'
            print r'        return;'
            print r'    }'

        if method.name == 'GetData':
            print r'    pData = _allocator.alloc(DataSize);'
            print r'    do {'
            self.doInvokeInterfaceMethod(interface, method)
            print r'        GetDataFlags = 0; // Prevent infinite loop'
            print r'    } while (_result == S_FALSE);'
            self.checkResult(interface, method)
            print r'    return;'

        Retracer.invokeInterfaceMethod(self, interface, method)

        if method.name in ('AcquireSync', 'ReleaseSync'):
            print r'    if (SUCCEEDED(_result) && _result != S_OK) {'
            print r'        retrace::warning(call) << " returned " << _result << "\n";'
            print r'    }'

        # process events after presents
        if interface.name.startswith('IDXGISwapChain') and method.name.startswith('Present'):
            print r'    d3dretrace::processEvents();'

        if method.name in ('Map', 'Unmap'):
            if interface.name.startswith('ID3D11DeviceContext'):
                print '    void * & _pbData = g_Maps[_this][SubresourceKey(pResource, Subresource)];'
            else:
                subresourceArg = method.getArgByName('Subresource')
                if subresourceArg is None:
                    print '    UINT Subresource = 0;'
                print '    void * & _pbData = g_Maps[0][SubresourceKey(_this, Subresource)];'

        if method.name == 'Map':
            print '    _MAP_DESC _MapDesc;'
            print '    _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames())
            print '    size_t _MappedSize = _MapDesc.Size;'
            print '    if (_MapDesc.Size) {'
            print '        _pbData = _MapDesc.pData;'
            if interface.name.startswith('ID3D11DeviceContext'):
                # Prevent false warnings on 1D and 2D resources, since the
                # pitches are often junk there...
                print '        _normalizeMap(pResource, pMappedResource);'
            else:
                print '        _pbData = _MapDesc.pData;'
            print '    } else {'
            print '        return;'
            print '    }'

        if method.name == 'Unmap':
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '        _pbData = 0;'
            print '    }'

        if interface.name.startswith('ID3D11VideoContext'):
            if method.name == 'GetDecoderBuffer':
                print '    if (*ppBuffer && *pBufferSize) {'
                print '        g_Maps[nullptr][SubresourceKey(_this, Type)] = *ppBuffer;'
                print '    }'
            if method.name == 'ReleaseDecoderBuffer':
                print '    SubresourceKey _mappingKey(_this, Type);'
                print '    void *_pBuffer = g_Maps[nullptr][_mappingKey];'
                print '    if (_pBuffer) {'
                print '        retrace::delRegionByPointer(_pBuffer);'
                print '        g_Maps[nullptr][_mappingKey] = 0;'
                print '    }'

        # Attach shader byte code for lookup
        if 'pShaderBytecode' in method.argNames():
            ppShader = method.args[-1]
            assert ppShader.output
            print r'    if (retrace::dumpingState && SUCCEEDED(_result)) {'
            print r'        (*%s)->SetPrivateData(d3dstate::GUID_D3DSTATE, BytecodeLength, pShaderBytecode);' % ppShader.name
            print r'    }'

        if method.name == 'CreateBuffer':
            ppBuffer = method.args[-1]
            print r'    if (retrace::dumpingState && SUCCEEDED(_result)) {'
            print r'       char label[32];'
            print r'       _snprintf(label, sizeof label, "0x%%llx", call.arg(%u).toArray()->values[0]->toUIntPtr());' % ppBuffer.index
            print r'        (*%s)->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(label)+1, label);' % ppBuffer.name
            print r'    }'

    def retraceInterfaceMethodBody(self, interface, method):
        Retracer.retraceInterfaceMethodBody(self, interface, method)

        # Add pitch swizzling information to the region
        if method.name == 'Map' and interface.name not in ('ID3D10Buffer', 'ID3D10Texture1D'):
            if interface.name.startswith('ID3D11DeviceContext'):
                outArg = method.getArgByName('pMappedResource')
                memberNames = ('pData', 'RowPitch', 'DepthPitch')
            elif interface.name.startswith('ID3D10'):
                outArg = method.args[-1]
                memberNames = ('pData', 'RowPitch', 'DepthPitch')
            elif interface.name == 'IDXGISurface':
                outArg = method.getArgByName('pLockedRect')
                memberNames = ('pBits', 'Pitch', None)
            else:
                raise NotImplementedError
            struct = outArg.type.type
            dataMemberName, rowPitchMemberName, depthPitchMemberName = memberNames
            dataMemberIndex = struct.getMemberByName(dataMemberName)
            rowPitchMemberIndex = struct.getMemberByName(rowPitchMemberName)
            print r'    if (_pbData && %s->%s != 0) {' % (outArg.name, rowPitchMemberName)
            print r'        const trace::Array *_%s = call.arg(%u).toArray();' % (outArg.name, outArg.index)
            print r'        if (%s) {' % outArg.name
            print r'            const trace::Struct *_struct = _%s->values[0]->toStruct();' % (outArg.name)
            print r'            if (_struct) {'
            print r'                unsigned long long traceAddress = _struct->members[%u]->toUIntPtr();' % dataMemberIndex
            print r'                int traceRowPitch = _struct->members[%u]->toSInt();' % rowPitchMemberIndex
            print r'                int realRowPitch = %s->%s;' % (outArg.name, rowPitchMemberName)
            print r'                if (realRowPitch && traceRowPitch != realRowPitch) {'
            print r'                    retrace::setRegionPitch(traceAddress, 2, traceRowPitch, realRowPitch);'
            print r'                }'
            try:
                depthPitchMemberIndex = struct.getMemberByName(depthPitchMemberName)
            except ValueError:
                assert len(struct.members) < 3
                pass
            else:
                assert depthPitchMemberName == 'DepthPitch'
                print r'                if (%s->DepthPitch) {' % outArg.name
                print r'                    retrace::checkMismatch(call, "DepthPitch", _struct->members[%u], %s->DepthPitch);' % (struct.getMemberByName('DepthPitch'), outArg.name)
                print r'                }'
            print r'            }'
            print r'        }'
            print r'    }'


    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        # Set object names
        if function.name == 'SetPrivateData' and arg.name == 'pData':
            iid = function.args[0].name
            print r'    if (%s != WKPDID_D3DDebugObjectName) {' % iid
            print r'        return;'
            print r'    }'
            # Interpret argument as string
            Retracer.extractArg(self, function, arg, LPCSTR, lvalue, rvalue)
            print r'    if (!pData) {'
            print r'        return;'
            print r'    }'
            print r'    assert(DataSize >= strlen((const char *)pData));'
            print r'    // Some applications include the trailing zero terminator in the data'
            print r'    DataSize = strlen((const char *)pData);'
            return

        Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)


def main():
    print r'#define INITGUID'
    print
    print r'#include <string.h>'
    print
    print r'#include <iostream>'
    print
    print r'#include "d3dretrace.hpp"'
    print r'#include "os_version.hpp"'
    print
    print r'#include "d3dretrace_dxgi.hpp"'
    print r'#include "d3d10imports.hpp"'
    print r'#include "d3d10size.hpp"'
    print r'#include "d3d10state.hpp"'
    print r'#include "d3d11imports.hpp"'
    print r'#include "d3d11size.hpp"'
    print r'#include "dcompimports.hpp"'
    print r'#include "d3dstate.hpp"'
    print r'#include "d3d9imports.hpp" // D3DERR_WASSTILLDRAWING'
    print
    print '''static d3dretrace::D3DDumper<IDXGISwapChain> dxgiDumper;'''
    print '''static d3dretrace::D3DDumper<ID3D10Device> d3d10Dumper;'''
    print '''static d3dretrace::D3DDumper<ID3D11DeviceContext> d3d11Dumper;'''
    print

    api = API()
    api.addModule(dxgi)
    api.addModule(d3d10)
    api.addModule(d3d10_1)
    api.addModule(d3d11)
    api.addModule(dcomp)

    retracer = D3DRetracer()
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

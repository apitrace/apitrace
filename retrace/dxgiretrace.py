##########################################################################
#
# Copyright 2011 Jose Fonseca
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


"""D3D retracer generator."""


import sys
from dllretrace import DllRetracer as Retracer
import specs.dxgi
from specs import stdapi
from specs.stdapi import API, Visitor
from specs.winapi import LPCSTR
from specs.dxgi import dxgi
from specs.d3d10 import d3d10, d3d10_1
from specs.d3d11 import d3d11
from specs import d3d12
from specs.dcomp import dcomp

def typeArrayOrPointer(arg, type_):
    return (isinstance(arg.type, stdapi.Pointer) and arg.type.type is type_) or \
           (isinstance(arg.type, stdapi.Pointer) and isinstance(arg.type.type, stdapi.Const) and arg.type.type.type is type_) or \
           (isinstance(arg.type, stdapi.Array) and arg.type.type is type_) or \
           (isinstance(arg.type, stdapi.Array) and isinstance(arg.type.type, stdapi.Const) and arg.type.type.type is type_)

class D3D12ArgumentResolver(Visitor):
    class TypeCheck(Visitor):
        resolvable = dict()

        def visit(self, type):
            _result = self.resolvable.get(type, None)
            if _result is None:
                _result = Visitor.visit(self, type)
                self.resolvable[type] = _result
            return _result

        def visitAlias(self, alias):
            if alias.expr == 'D3D12_GPU_VIRTUAL_ADDRESS':
                return True
            return self.visit(alias.type)

        def visitLiteral(self, literal):
            return False

        def visitOpaque(self, array):
            return False

        def visitString(self, array):
            return False

        def visitEnum(self, array):
            return False

        def visitBlob(self, array):
            return False

        def visitBitmask(self, array):
            return False

        def visitIntPointer(self, array):
            return False

        def visitObjPointer(self, array):
            return False

        def visitLinearPointer(self, array):
            return False

        def visitHandle(self, array):
            return False

        def visitPointer(self, pointer):
            return self.visit(pointer.type)

        def visitConst(self, const):
            return self.visit(const.type)

        def visitReference(self, reference):
            return self.visit(reference.type)

        def visitArray(self, array):
            return self.visit(array.type)

        def visitStruct(self, struct):
            # NOTE(damcclos): Skipping Ray tracing structures with const internals for now
            if struct.name in ('D3D12_RAYTRACING_GEOMETRY_DESC',):
                return False
            if struct.name in ('D3D12_CPU_DESCRIPTOR_HANDLE', 'D3D12_GPU_DESCRIPTOR_HANDLE'):
                return True

            _result = False
            for type, _ in struct.members:
                _result |= self.visit(type)
            return _result

        def visitPolymorphic(self, polymorphic):
            _result = False
            for _, type in polymorphic.switchTypes:
                _result |= self.visit(type)
            return _result
            
    __type_check = TypeCheck()
    __default_indent = '    '
    __structs = None

    def expand(self, expr):
        # Expand a C expression, replacing certain variables
        if not isinstance(expr, str):
            return expr
        variables = {}

        if self.__structs is not None:
            variables['self'] = '(%s)' % self.__structs[0]

        expandedExpr = expr.format(**variables)
        if expandedExpr != expr and 0:
            sys.stderr.write("  %r -> %r\n" % (expr, expandedExpr))
        return expandedExpr

    def visit(self, type, lvalue, **kwargs):
        if self.__type_check.visit(type):
            return Visitor.visit(self, type, lvalue, **kwargs)

    def visitMethod(self, method):
        for arg in method.args:
            if arg.input:
                self.visit(arg.type, arg.name)

    def visitAlias(self, alias, lvalue, indent = __default_indent, **kwargs):
        if alias.expr == 'D3D12_GPU_VIRTUAL_ADDRESS':
            print(r'%s%s = g_D3D12AddressResolver.Resolve(%s);' % (indent, lvalue, lvalue))
        else:
            self.visit(alias.type, lvalue, indent = indent, **kwargs)

    def visitConst(self, const, lvalue, **kwargs):
        self.visit(const.type, lvalue, **kwargs)

    def visitReference(self, reference, lvalue, **kwargs):
        self.visit(reference.type, lvalue, **kwargs)

    def visitStruct(self, struct, lvalue, indent = __default_indent, **kwargs):
        if struct.name in ('D3D12_CPU_DESCRIPTOR_HANDLE', 'D3D12_GPU_DESCRIPTOR_HANDLE'):
            print(r'%s%s = g_D3D12DescriptorResolver.Resolve(%s);' % (indent, lvalue, lvalue))
        else:
            self.__structs = (lvalue, self.__structs)
            for type, name in struct.members:
                lvalue1 = lvalue
                if name is not None:
                    lvalue1 += '.' + name
                self.visit(type, lvalue1, indent = indent, **kwargs)
            _, self.__structs = self.__structs

    def visitPointer(self, pointer, lvalue, indent = __default_indent, **kwargs):
        print(r'%sif(%s != nullptr) {' % (indent, lvalue))
        self.visit(pointer.type, '(*' + lvalue + ')', indent = indent + self.__default_indent, **kwargs)
        print(r'%s}' % (indent))

    def visitArray(self, array, lvalue, indent = __default_indent, **kwargs):
        array_length = self.expand(array.length)
        print(r'%sif(%s != nullptr) {' % (indent, lvalue))
        indent1 = indent + self.__default_indent
        print(r'%sfor (size_t _i = 0, _s = %s; _i < _s; ++_i) {' % (indent1, array_length))
        self.visit(array.type, lvalue + '[_i]', indent = indent1 + self.__default_indent, **kwargs)
        print(r'%s}' % (indent1))
        print(r'%s}' % (indent))

    def visitPolymorphic(self, polymorphic, lvalue, indent = __default_indent, **kwargs):
        if polymorphic.defaultType is None or polymorphic.stream:
            if polymorphic.stream:
                raise NotImplementedError
            indent1 = indent + self.__default_indent
            switchExpr = self.expand(polymorphic.switchExpr)
            print(r'%sswitch (%s) {' % (indent, switchExpr))
            for cases, type in polymorphic.iterSwitch():
                for case in cases:
                    print(r'%s%s:' % (indent, case))
                self.visit(type, lvalue, indent = indent1, **kwargs)
                print(r'%sbreak;' % indent1)
            if polymorphic.defaultType is None:
                print(r'%sdefault:' % indent)
                print(r'%sretrace::warning(call) << "unexpected polymorphic case " << %s << "\n";' % (indent1, switchExpr))
                print(r'%sbreak;' % (indent1))
            print(r'%s}' % indent)
        else:
            self.visit(polymorphic.defaultType, lvalue, indent = indent, **kwargs)
            
class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print('// Swizzling mapping for lock addresses, mapping a (pDeviceContext, pResource, Subresource) -> void *')
        print('typedef std::pair< IUnknown *, UINT > SubresourceKey;')
        print('static std::map< IUnknown *, std::map< SubresourceKey, void * > > g_Maps;')
        print('struct _D3D12_MAP_REPLAY_DESC { uint32_t RefCount = 0; void* pData = nullptr; };')
        print('static std::map< IUnknown *, _D3D12_MAP_REPLAY_DESC > g_MapsD3D12;')
        print()
        self.table_name = 'd3dretrace::dxgi_callbacks'

        Retracer.retraceApi(self, api)

    createDeviceFunctionNames = [
        "D3D10CreateDevice",
        "D3D10CreateDeviceAndSwapChain",
        "D3D10CreateDevice1",
        "D3D10CreateDeviceAndSwapChain1",
        "D3D11CreateDevice",
        "D3D11CreateDeviceAndSwapChain",
        "D3D12CreateDevice",
    ]

    def invokeFunction(self, function):
        if function.name in self.createDeviceFunctionNames:
            # create windows as neccessary
            if 'pSwapChainDesc' in function.argNames():
                print(r'    if (pSwapChainDesc) {')
                print(r'        d3dretrace::createWindowForSwapChain(pSwapChainDesc);')
                print(r'    }')

            # Compensate for the fact we don't trace DXGI object creation
            if function.name.startswith('D3D11CreateDevice'):
                print(r'    if (DriverType == D3D_DRIVER_TYPE_UNKNOWN && !pAdapter) {')
                print(r'        DriverType = D3D_DRIVER_TYPE_HARDWARE;')
                print(r'    }')

            if function.name.startswith('D3D10CreateDevice'):
                # Toggle debugging
                print(r'    if (retrace::debug >= 2) {')
                print(r'        Flags |= D3D10_CREATE_DEVICE_DEBUG;')
                print(r'    } else if (retrace::debug < 0) {')
                print(r'        Flags &= ~D3D10_CREATE_DEVICE_DEBUG;')
                print(r'    }')

                # D3D10CreateDevice(D3D10_DRIVER_TYPE_REFERENCE) fails with
                # DXGI_ERROR_UNSUPPORTED on 64bits.
                print(r'#ifdef _WIN64')
                print(r'    if (DriverType == D3D10_DRIVER_TYPE_REFERENCE) {')
                print(r'        DriverType = D3D10_DRIVER_TYPE_WARP;')
                print(r'    }')
                print(r'#endif')

                # Force driver
                self.forceDriver('D3D10_DRIVER_TYPE_HARDWARE')

            if function.name.startswith('D3D11CreateDevice'):
                # Toggle debugging
                print(r'    if (retrace::debug >= 2) {')
                print(r'        Flags |= D3D11_CREATE_DEVICE_DEBUG;')
                print(r'    } else if (retrace::debug < 0) {')
                print(r'        Flags &= ~D3D11_CREATE_DEVICE_DEBUG;')
                print(r'    }')
                print(r'    if (IsWindows8OrGreater()) {')
                print(r'        Flags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;')
                print(r'    }')

                # Force driver
                self.forceDriver('D3D_DRIVER_TYPE_UNKNOWN')

            if function.name.startswith('D3D12CreateDevice'):
                # Toggle debugging
                print(r'    if (retrace::debug >= 2) {')
                print(r'        ID3D12Debug* debugController;')
                print(r'        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))')
                print(r'            debugController->EnableDebugLayer();')
                print(r'        else')
                print(r'            retrace::warning(call) << "Direct3D 12.x SDK Debug Layer (d3d12*sdklayers.dll) not available, continuing without debug output\n";')
                print(r'    }')

        Retracer.invokeFunction(self, function)

    def doInvokeFunction(self, function):
        Retracer.doInvokeFunction(self, function)

        # Handle missing debug layer.  While it's possible to detect whether
        # the debug layers are present, by creating a null device, and checking
        # the result.  It's simpler to retry.
        if function.name.startswith('D3D10CreateDevice'):
            print(r'        if ((_result == E_FAIL || _result == DXGI_ERROR_SDK_COMPONENT_MISSING) && (Flags & D3D10_CREATE_DEVICE_DEBUG)) {')
            print(r'            retrace::warning(call) << "Direct3D 10.x SDK Debug Layer (d3d10sdklayers.dll) not available, continuing without debug output\n";')
            print(r'            Flags &= ~D3D10_CREATE_DEVICE_DEBUG;')
            Retracer.doInvokeFunction(self, function)
            print(r'        }')
        if function.name.startswith('D3D11CreateDevice'):
            print(r'        if ((_result == E_FAIL || _result == DXGI_ERROR_SDK_COMPONENT_MISSING) && (Flags & D3D11_CREATE_DEVICE_DEBUG)) {')
            print(r'            retrace::warning(call) << "Direct3D 11.x SDK Debug Layer (d3d11*sdklayers.dll) not available, continuing without debug output\n";')
            print(r'            Flags &= ~D3D11_CREATE_DEVICE_DEBUG;')
            Retracer.doInvokeFunction(self, function)
            print(r'        }')

    def handleFailure(self, interface, methodOrFunction):
        # Catch when device is removed, and report the reason.
        if interface is not None:
            print(r'        if (_result == DXGI_ERROR_DEVICE_REMOVED) {')
            print(r'            d3dretrace::deviceRemoved(call, _this);')
            print(r'        }')

        Retracer.handleFailure(self, interface, methodOrFunction)

    def forceDriver(self, driverType):
        # This can only work when pAdapter is NULL. For non-NULL pAdapter we
        # need to override inside the EnumAdapters call below
        print(r'    ComPtr<IDXGIFactory1> _pFactory;')
        print(r'    ComPtr<IDXGIAdapter> _pAdapter;')
        print(r'    if (pAdapter == nullptr && retrace::driver != retrace::DRIVER_DEFAULT) {')
        print(r'        _result = CreateDXGIFactory1(IID_IDXGIFactory1, &_pFactory);')
        print(r'        assert(SUCCEEDED(_result));')
        print(r'        _result = d3dretrace::createAdapter(_pFactory.Get(), IID_IDXGIAdapter1, &_pAdapter);')
        print(r'        pAdapter = _pAdapter.Get();')
        print(r'        DriverType = %s;' % driverType)
        print(r'        Software = nullptr;')
        print(r'    }')
        print(r'    if (Software) {')
        print(r'        Software = LoadLibraryA("d3d10warp.dll");')
        print(r'        assert(Software != nullptr);')
        print(r'    }')

    def doInvokeInterfaceMethod(self, interface, method):
        if interface.name.startswith('IDXGIAdapter') and method.name == 'EnumOutputs':
            print(r'    if (Output != 0) {')
            print(r'        retrace::warning(call) << "ignoring output " << Output << "\n";')
            print(r'        Output = 0;')
            print(r'    }')

        # GPU counters are vendor specific and likely to fail, so use a
        # timestamp query instead, which is guaranteed to succeed
        if method.name == 'CreateCounter':
            if interface.name.startswith('ID3D10'):
                print(r'    D3D10_QUERY_DESC _queryDesc;')
                print(r'    _queryDesc.Query = D3D10_QUERY_TIMESTAMP;')
                print(r'    _queryDesc.MiscFlags = 0;')
                print(r'    _result = _this->CreateQuery(&_queryDesc, reinterpret_cast<ID3D10Query **>(ppCounter));')
                return
            if interface.name.startswith('ID3D11'):
                print(r'    D3D11_QUERY_DESC _queryDesc;')
                print(r'    _queryDesc.Query = D3D11_QUERY_TIMESTAMP;')
                print(r'    _queryDesc.MiscFlags = 0;')
                print(r'    _result = _this->CreateQuery(&_queryDesc, reinterpret_cast<ID3D11Query **>(ppCounter));')
                return
            # TODO(Josh): We need to do something for d3d12 counters here but this gets a lil more complex

        Retracer.doInvokeInterfaceMethod(self, interface, method)

        # Force driver
        if interface.name.startswith('IDXGIFactory') and method.name.startswith('EnumAdapters'):
            print(r'    if (Adapter != 0) {')
            print(r'        retrace::warning(call) << "ignoring non-default adapter " << Adapter << "\n";')
            print(r'        Adapter = 0;')
            print(r'    }')
            print(r'    if (retrace::driver != retrace::DRIVER_DEFAULT) {')
            print(r'        _result = d3dretrace::createAdapter(_this, IID_IDXGIAdapter1, (void **)ppAdapter);')
            print(r'    } else {')
            Retracer.doInvokeInterfaceMethod(self, interface, method)
            print(r'    }')
            return

        if interface.name.startswith('IDXGIFactory') and method.name == 'CreateSoftwareAdapter':
            print(r'    const char *szSoftware = NULL;')
            print(r'    switch (retrace::driver) {')
            print(r'    case retrace::DRIVER_REFERENCE:')
            print(r'        szSoftware = "d3d11ref.dll";')
            print(r'        break;')
            print(r'    case retrace::DRIVER_MODULE:')
            print(r'        szSoftware = retrace::driverModule;')
            print(r'        break;')
            print(r'    case retrace::DRIVER_SOFTWARE:')
            print(r'    default:')
            print(r'        szSoftware = "d3d10warp.dll";')
            print(r'        break;')
            print(r'    }')
            print(r'    Module = LoadLibraryA("d3d10warp");')
            print(r'    if (!Module) {')
            print(r'        retrace::warning(call) << "failed to load " << szSoftware << "\n";')
            print(r'    }')
            Retracer.doInvokeInterfaceMethod(self, interface, method)

        # Keep retrying ID3D11VideoContext::DecoderBeginFrame when returns E_PENDING
        if interface.name == 'ID3D11VideoContext' and method.name == 'DecoderBeginFrame':
            print(r'    while (_result == D3DERR_WASSTILLDRAWING || _result == E_PENDING) {')
            print(r'        Sleep(1);')
            Retracer.doInvokeInterfaceMethod(self, interface, method)
            print(r'    }')
        # TODO(Josh): D3D12 Video

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('ID3D10Device', 'ID3D10Device1'):
            if method.name == 'Release':
                print(r'    if (call.ret->toUInt() == 0) {')
                print(r'        d3d10Dumper.unbindDevice(_this);')
                print(r'    }')
            else:
                print(r'    d3d10Dumper.bindDevice(_this);')
        if interface.name.startswith('ID3D11DeviceContext'):
            if method.name == 'Release':
                print(r'    if (call.ret->toUInt() == 0) {')
                print(r'        d3d11Dumper.unbindDevice(_this);')
                print(r'    }')
            else:
                print(r'    d3d11Dumper.bindDevice(_this);')
        # TODO(Josh): d3d12 command queue???

        # intercept private interfaces
        if method.name == 'QueryInterface':
            print(r'    if (!d3dretrace::overrideQueryInterface(_this, riid, ppvObj, &_result)) {')
            Retracer.invokeInterfaceMethod(self, interface, method)
            print(r'    }')
            return

        # create windows as neccessary
        if method.name == 'CreateSwapChain':
            print(r'    d3dretrace::createWindowForSwapChain(pDesc);')
        if method.name == 'CreateSwapChainForHwnd':
            print(r'    hWnd = d3dretrace::createWindow(pDesc->Width, pDesc->Height);')
            print(r'    // DXGI_SCALING_NONE is only supported on Win8 and beyond')
            print(r'    if (pDesc->Scaling == DXGI_SCALING_NONE && !IsWindows8OrGreater()) {')
            print(r'        pDesc->Scaling = DXGI_SCALING_STRETCH;')
            print(r'    }')
        if method.name == 'CreateSwapChainForComposition':
            print(r'    HWND hWnd = d3dretrace::createWindow(pDesc->Width, pDesc->Height);')
            print(r'    _result = _this->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, NULL, pRestrictToOutput, ppSwapChain);')
            self.checkResult(interface, method)
            return
        if method.name == 'CreateSwapChainForCompositionSurfaceHandle':
            print(r'    ComPtr<IDXGIFactory2> pFactory;')
            print(r'    _result = _this->QueryInterface(IID_IDXGIFactory2, &pFactory);')
            print(r'    assert(SUCCEEDED(_result));')
            print(r'    HWND hWnd = d3dretrace::createWindow(pDesc->Width, pDesc->Height);')
            print(r'    pDesc->Flags &= ~DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO;')
            print(r'    _result = pFactory->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, NULL, pRestrictToOutput, ppSwapChain);')
            self.checkResult(interface, method)
            return
        if method.name == 'ResizeBuffers':
            print(r'    SwapChainFlags &= ~DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO;')
        if method.name == 'CreateTargetForHwnd':
            print(r'    hwnd = d3dretrace::createWindow(1024, 768);')

        if method.name == 'SetFullscreenState':
            print(r'    if (retrace::forceWindowed) {')
            print(r'         DXGI_SWAP_CHAIN_DESC Desc;')
            print(r'         _this->GetDesc(&Desc);')
            print(r'         if (Desc.BufferDesc.Format != DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) {')
            print(r'             Fullscreen = FALSE;')
            print(r'             pTarget = nullptr;')
            print(r'        }')
            print(r'    }')

        # notify frame has been completed
        if interface.name.startswith('IDXGISwapChain') and method.name.startswith('Present'):
            # Reset _DO_NOT_WAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            print(r'    Flags &= ~DXGI_PRESENT_DO_NOT_WAIT;')
            if interface.name.startswith('IDXGISwapChainDWM'):
                print(r'    ComPtr<IDXGISwapChain> pSwapChain;')
                print(r'    if (SUCCEEDED(_this->QueryInterface(IID_IDXGISwapChain, &pSwapChain))) {')
                print(r'        dxgiDumper.bindDevice(pSwapChain.Get());')
                print(r'    } else {')
                print(r'        assert(0);')
                print(r'    }')
            else:
                print(r'    dxgiDumper.bindDevice(_this);')
            print(r'    if ((Flags & DXGI_PRESENT_TEST) == 0) {')
            print(r'        retrace::frameComplete(call);')
            print(r'    }')

        if 'pSharedResource' in method.argNames():
            print(r'    if (pSharedResource) {')
            print(r'        retrace::warning(call) << "shared surfaces unsupported\n";')
            print(r'        pSharedResource = NULL;')
            print(r'    }')

        if interface.name.startswith('ID3D10Device') and method.name.startswith('OpenSharedResource'):
            print(r'    retrace::warning(call) << "replacing shared resource with checker pattern\n";')
            print(r'    _result = d3dretrace::createSharedResource(_this, ReturnedInterface, ppResource);')
            self.checkResult(interface, method)
            return
        if interface.name.startswith('ID3D11Device') and method.name == 'OpenSharedResource':
            # Some applications (e.g., video playing in IE11) create shared resources within the same process.
            # TODO: Generalize to other OpenSharedResource variants
            print(r'    retrace::map<HANDLE>::const_iterator it = _shared_handle_map.find(hResource);')
            print(r'    if (it == _shared_handle_map.end()) {')
            print(r'        retrace::warning(call) << "replacing shared resource with checker pattern\n";')
            print(r'        _result = d3dretrace::createSharedResource(_this, ReturnedInterface, ppResource);')
            self.checkResult(interface, method)
            print(r'    } else {')
            print(r'        hResource = it->second;')
            Retracer.invokeInterfaceMethod(self, interface, method)
            print(r'    }')
            return
        if interface.name.startswith('ID3D11Device') and method.name.startswith('OpenSharedResource'):
            print(r'    retrace::warning(call) << "replacing shared resource with checker pattern\n";')
            print(r'    _result = d3dretrace::createSharedResource(_this, ReturnedInterface, ppResource);')
            if method.name == 'OpenSharedResourceByName':
                print(r'    (void)lpName;')
                print(r'    (void)dwDesiredAccess;')
            else:
                print(r'    (void)hResource;')
            self.checkResult(interface, method)
            return

        if method.name == 'Map' and not interface.name.startswith('ID3D12Resource'):
            # Reset _DO_NOT_WAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('MapFlags')
            for flag in mapFlagsArg.type.values:
                if flag.endswith('_MAP_FLAG_DO_NOT_WAIT'):
                    print(r'    MapFlags &= ~%s;' % flag)

        if method.name.startswith('UpdateSubresource'):
            # The D3D10 debug layer is buggy (or at least inconsistent with the
            # runtime), as it seems to estimate and enforce the data size based on the
            # SrcDepthPitch, even for non 3D textures, but in some traces
            # SrcDepthPitch is garbagge for non 3D textures.
            # XXX: It also seems to expect padding bytes at the end of the last
            # row, but we never record (or allocate) those...
            print(r'    if (retrace::debug >= 2 && pDstBox && pDstBox->front == 0 && pDstBox->back == 1) {')
            print(r'        SrcDepthPitch = 0;')
            print(r'    }')

        if method.name == 'SetGammaControl':
            # This method is only supported while in full-screen mode
            print(r'    if (retrace::forceWindowed) {')
            print(r'        return;')
            print(r'    }')

        if method.name == 'GetData':
            print(r'    pData = DataSize ? _allocator.alloc(DataSize) : nullptr;')
            print(r'    do {')
            self.doInvokeInterfaceMethod(interface, method)
            print(r'        GetDataFlags = 0; // Prevent infinite loop')
            print(r'    } while (_result == S_FALSE);')
            self.checkResult(interface, method)
            print(r'    return;')

        # We don't capture multiple processes, so don't wait keyed mutexes to
        # avoid deadlocks.  However it's important to try honouring the
        # IDXGIKeyedMutex interfaces so that single processes using multiple
        # contexts work reliably, by ensuring pending commands get flushed.
        if method.name == 'AcquireSync':
            print(r'    dwMilliseconds = 0;')

        if interface.name.startswith('ID3D12'):
            D3D12ArgumentResolver().visitMethod(method)

            if method.name == 'CreateCommandQueue':
                # Disable GPU timeout
                print('    D3D12_COMMAND_QUEUE_DESC _desc = *pDesc;')
                print('    _desc.Flags |= D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;')
                print('    pDesc = &_desc;')

            if method.name == 'SetEventOnCompletion':
                print('     {')
                print('         auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);')
                print('         auto _fence_event_iter = g_D3D12FenceEventMap.find(hEvent);')
                print('         if (_fence_event_iter == g_D3D12FenceEventMap.end()) {')
                print('             HANDLE event = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);')
                print('             g_D3D12FenceEventMap.insert(std::make_pair(hEvent, event));')
                print('             hEvent = event;')
                print('         } else {')
                print('             hEvent = _fence_event_iter->second;')
                print('         }')
                print('     }')

            if method.name == 'OpenExistingHeapFromAddress':
                pAddress = method.args[0]
                print(r'    %s = g_D3D12HeapAllocator.Register(%s, AllocationSize);' % (pAddress.name, pAddress.name))

        if method.name == 'CreatePipelineLibrary':
            # Make a fake pipeline library, so we can still make the state objects.
            print('    *ppPipelineLibrary = reinterpret_cast<void*>(new _D3D12FakePipelineLibrary(_this));')
            print('    _result = S_OK;')
        else:
            Retracer.invokeInterfaceMethod(self, interface, method)

        if method.name == 'GetCompletedValue':
            print('    UINT64 _traced_result = call.ret->toUInt();')
            print('    if (_result < _traced_result)')
            print('    {')
            print('        HANDLE _wait_event = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);')
            print('        _this->SetEventOnCompletion(_traced_result, _wait_event);')
            print('        WaitForSingleObject(_wait_event, INFINITE);')
            print('    }')

        # process events after presents
        if interface.name.startswith('IDXGISwapChain') and method.name.startswith('Present'):
            print(r'    d3dretrace::processEvents();')

        if method.name in ('Map', 'Unmap'):
            if interface.name.startswith('ID3D12'):
                print('    _D3D12_MAP_REPLAY_DESC & _desc = g_MapsD3D12[_this];')
                print('    void * & _pbData = _desc.pData;')
            elif interface.name.startswith('ID3D11DeviceContext'):
                print('    void * & _pbData = g_Maps[_this][SubresourceKey(pResource, Subresource)];')
            else:
                subresourceArg = method.getArgByName('Subresource')
                if subresourceArg is None:
                    print('    UINT Subresource = 0;')
                print('    void * & _pbData = g_Maps[0][SubresourceKey(_this, Subresource)];')

        if method.name == 'Map':
            if interface.name.startswith('ID3D12'):
                print('    size_t _MappedSize = size_t(_getMapSize(_this));')
                print('    if (_desc.RefCount) {')
                print('        ppData = nullptr;')
                print('        _MappedSize = 0;')
                print('    }')
                print('    else')
                print('        _pbData = *ppData;')
                print('    _desc.RefCount++;')
            else:
                print('    _MAP_DESC _MapDesc;')
                print('    _getMapDesc(_this, %s, _MapDesc);' % ', '.join(method.argNames()))
                print('    size_t _MappedSize = _MapDesc.Size;')
                print('    if (_MapDesc.Size) {')
                print('        _pbData = _MapDesc.pData;')
                if interface.name.startswith('ID3D11DeviceContext'):
                    # Prevent false warnings on 1D and 2D resources, since the
                    # pitches are often junk there...
                    print('        _normalizeMap(pResource, pMappedResource);')
                else:
                    print('        _pbData = _MapDesc.pData;')
                print('    } else {')
                print('        return;')
                print('    }')

        if method.name == 'Unmap':
            if interface.name.startswith('ID3D12'):
                print('    if (_pbData && --_desc.RefCount == 0) {')
            else:
                print('    if (_pbData) {')
            print('        retrace::delRegionByPointer(_pbData);')
            print('        _pbData = 0;')
            print('    }')

        if interface.name.startswith('ID3D11VideoContext'):
            if method.name == 'GetDecoderBuffer':
                print('    if (*ppBuffer && *pBufferSize) {')
                print('        g_Maps[nullptr][SubresourceKey(_this, Type)] = *ppBuffer;')
                print('    }')
            if method.name == 'ReleaseDecoderBuffer':
                print('    SubresourceKey _mappingKey(_this, Type);')
                print('    void *_pBuffer = g_Maps[nullptr][_mappingKey];')
                print('    if (_pBuffer) {')
                print('        retrace::delRegionByPointer(_pBuffer);')
                print('        g_Maps[nullptr][_mappingKey] = 0;')
                print('    }')

        # Attach shader byte code for lookup
        if 'pShaderBytecode' in method.argNames():
            ppShader = method.args[-1]
            assert ppShader.output
            print(r'    if (retrace::dumpingState && SUCCEEDED(_result)) {')
            print(r'        (*%s)->SetPrivateData(d3dstate::GUID_D3DSTATE, BytecodeLength, pShaderBytecode);' % ppShader.name)
            print(r'    }')

        if method.name == 'CreateBuffer':
            ppBuffer = method.args[-1]
            print(r'    if (retrace::dumpingState && SUCCEEDED(_result)) {')
            print(r'       char label[32];')
            print(r'       _snprintf(label, sizeof label, "0x%%llx", call.arg(%u).toArray()->values[0]->toUIntPtr());' % ppBuffer.index)
            print(r'        (*%s)->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(label)+1, label);' % ppBuffer.name)
            print(r'    }')

        if method.name == 'CreateDescriptorHeap':
            pDesc = method.args[0]
            ppvHeap = method.args[2]
            print(r'    com_ptr<ID3D12DescriptorHeap> pDescriptorHeap(IID_ID3D12DescriptorHeap, %s);' % (ppvHeap.name))
            print(r'    if (pDescriptorHeap) {')
            print(r'        UINT ToIncrementSize = _this->GetDescriptorHandleIncrementSize(%s->Type);' % (pDesc.name))
            print(r'')
            print(r'        bool GPUHeap = false;')
            print(r'        if (%s->Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || %s->Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {' % (pDesc.name, pDesc.name))
            print(r'            if (%s->Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {' % (pDesc.name))
            print(r'                GPUHeap = true;')
            print(r'            }')
            print(r'        }')
            print(r'')
            print(r'        if (!GPUHeap) {')
            print(r'            D3D12_CPU_DESCRIPTOR_HANDLE CPUTo = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();')
            print(r'            g_D3D12DescriptorResolver.Register(CPUFrom, FromIncrementSize, CPUTo, ToIncrementSize, %s->NumDescriptors);' % (pDesc.name))
            print(r'        }')
            print(r'        else {')
            print(r'            D3D12_GPU_DESCRIPTOR_HANDLE GPUTo = pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();')
            print(r'            g_D3D12DescriptorResolver.Register(GPUFrom, FromIncrementSize, GPUTo, ToIncrementSize, %s->NumDescriptors);' % (pDesc.name))
            print(r'        }')
            print(r'    }')

        if method.name == 'CreatePlacedResource':
            pDesc = method.args[2]
            ppvResource = method.args[6]
            print(r'    if (SUCCEEDED(_result) && %s->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {' % (pDesc.name))
            print(r'        com_ptr<ID3D12Resource> pResource(IID_ID3D12Resource, %s);' % (ppvResource.name))
            print(r'        if (pResource) {')
            print(r'            D3D12_GPU_VIRTUAL_ADDRESS GPUTo = pResource->GetGPUVirtualAddress();')
            print(r'            D3D12_RESOURCE_ALLOCATION_INFO ToAllocationInfo = _this->GetResourceAllocationInfo(0, 1, %s);' % (pDesc.name))
            print(r'            g_D3D12AddressResolver.Register(GPUFrom, FromAllocationInfo, GPUTo, ToAllocationInfo);')
            print(r'        }')
            print(r'    }')

    def retraceInterfaceMethodBody(self, interface, method):
        Retracer.retraceInterfaceMethodBody(self, interface, method)

        # Add pitch swizzling information to the region
        # TODO(Josh): Any way we can fudge this to make pitches device agnostic on D3D12?
        if method.name == 'Map' and interface.name not in ('ID3D10Buffer', 'ID3D10Texture1D', 'ID3D12Resource'):
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
            print(r'    if (_pbData && %s->%s != 0) {' % (outArg.name, rowPitchMemberName))
            print(r'        const trace::Array *_%s = call.arg(%u).toArray();' % (outArg.name, outArg.index))
            print(r'        if (%s) {' % outArg.name)
            print(r'            const trace::Struct *_struct = _%s->values[0]->toStruct();' % (outArg.name))
            print(r'            if (_struct) {')
            print(r'                unsigned long long traceAddress = _struct->members[%u]->toUIntPtr();' % dataMemberIndex)
            print(r'                int traceRowPitch = _struct->members[%u]->toSInt();' % rowPitchMemberIndex)
            print(r'                int realRowPitch = %s->%s;' % (outArg.name, rowPitchMemberName))
            print(r'                if (realRowPitch && traceRowPitch != realRowPitch) {')
            print(r'                    retrace::setRegionPitch(traceAddress, 2, traceRowPitch, realRowPitch);')
            print(r'                }')
            try:
                depthPitchMemberIndex = struct.getMemberByName(depthPitchMemberName)
            except ValueError:
                assert len(struct.members) < 3
                pass
            else:
                assert depthPitchMemberName == 'DepthPitch'
                print(r'                if (%s->DepthPitch) {' % outArg.name)
                print(r'                    retrace::checkMismatch(call, "DepthPitch", _struct->members[%u], %s->DepthPitch);' % (struct.getMemberByName('DepthPitch'), outArg.name))
                print(r'                }')
            print(r'            }')
            print(r'        }')
            print(r'    }')

    def checkResult(self, interface, methodOrFunction):
        if interface is not None and interface.name == 'IDXGIKeyedMutex' and methodOrFunction.name == 'AcquireSync':
            print(r'    if (_result != S_OK) {')
            print(r'        retrace::failed(call, _result);')
            self.handleFailure(interface, methodOrFunction)
            print(r'    }')
            return

        return Retracer.checkResult(self, interface, methodOrFunction)

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        # Set object names
        if function.name == 'SetPrivateData' and arg.name == 'pData':
            iid = function.args[0].name
            print(r'    if (%s != WKPDID_D3DDebugObjectName) {' % iid)
            print(r'        return;')
            print(r'    }')
            # Interpret argument as string
            Retracer.extractArg(self, function, arg, LPCSTR, lvalue, rvalue)
            print(r'    if (!pData) {')
            print(r'        return;')
            print(r'    }')
            print(r'    assert(DataSize >= strlen((const char *)pData));')
            print(r'    // Some applications include the trailing zero terminator in the data')
            print(r'    DataSize = strlen((const char *)pData);')
            return

        Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)


def main():
    print(r'#define INITGUID')
    print()
    print(r'#include <string.h>')
    print()
    print(r'#include <iostream>')
    print(r'#include <mutex>')
    print()
    print(r'#include "d3dretrace.hpp"')
    print(r'#include "os_version.hpp"')
    print()
    print(r'#include "d3dretrace_dxgi.hpp"')
    print(r'#include "d3d10imports.hpp"')
    print(r'#include "d3d10size.hpp"')
    print(r'#include "d3d10state.hpp"')
    print(r'#include "d3d11imports.hpp"')
    print(r'#include "d3d11size.hpp"')
    print(r'#include "d3d12imports.hpp"')
    print(r'#include "d3d12size.hpp"')
    print(r'#include "d3d12slab.hpp"')
    print(r'#include "d3d12va.hpp"')
    print(r'#include "d3d12pipelinelibrary.hpp"')
    print(r'#include "dcompimports.hpp"')
    print(r'#include "d3dstate.hpp"')
    print(r'#include "d3d9imports.hpp" // D3DERR_WASSTILLDRAWING')
    print()
    print('''static d3dretrace::D3DDumper<IDXGISwapChain> dxgiDumper;''')
    print('''static d3dretrace::D3DDumper<ID3D10Device> d3d10Dumper;''')
    print('''static d3dretrace::D3DDumper<ID3D11DeviceContext> d3d11Dumper;''')
    print()
    print('D3D12_HEAP_ALLOCATOR g_D3D12HeapAllocator;')
    print('D3D12_DESCRIPTOR_RESOLVER g_D3D12DescriptorResolver;')
    print('D3D12_ADDRESS_RESOLVER g_D3D12AddressResolver;')
    print('std::map<HANDLE, HANDLE> g_D3D12FenceEventMap;')
    print('std::mutex g_D3D12FenceEventMapMutex;')
    print()

    api = API()
    api.addModule(dxgi)
    api.addModule(d3d10)
    api.addModule(d3d10_1)
    api.addModule(d3d11)
    api.addModule(d3d12.d3d12)
    api.addModule(dcomp)

    retracer = D3DRetracer()
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

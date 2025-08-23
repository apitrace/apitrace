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


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print('// Swizzling mapping for lock addresses')
        print('typedef std::pair<void *, UINT> MappingKey;')
        print('static std::map<MappingKey, void *> _maps;')
        print()

        Retracer.retraceApi(self, api)

    def invokeFunction(self, function):
        if function.name.startswith('Direct3DCreate9'):
            print(r'    if (retrace::debug >= 3 && !g_szD3D9DllName && LoadLibraryA("d3d9d.dll")) {')
            print(r'        /*')
            print(r'         * D3D9D only works for simple applications, it will often report bogus errors')
            print(r'         * on complex traces, or traces which use unofficial D3D9 features.')
            print(r'         */')
            print(r'        g_szD3D9DllName = "d3d9d.dll";')
            print(r'        SDKVersion |= 0x80000000;')
            print(r'    } else {')
            print(r'        SDKVersion &= ~0x80000000;')
            print(r'    }')

        # d3d8d.dll can be found in the Aug 2007 DXSDK.  It works on XP, but
        # not on Windows 7.
        if function.name.startswith('Direct3DCreate8'):
            print(r'    if (retrace::debug >= 3 && !g_szD3D8DllName && LoadLibraryA("d3d8d.dll")) {')
            print(r'        g_szD3D8DllName = "d3d8d.dll";')
            print(r'    }')

        if function.name.startswith('Direct3DCreate9'):
            print(r'    // 0: default')
            print(r'    // 1: force discrete')
            print(r'    // 2: prefer integrated?')
            print(r'    // 3: force integrated')
            print(r'    UINT uHybrid = 0;')
            print(r'    if (retrace::driver == retrace::DRIVER_DISCRETE) {')
            print(r'        uHybrid = 1;')
            print(r'    }')
            print(r'    if (retrace::driver == retrace::DRIVER_INTEGRATED) {')
            print(r'        uHybrid = 3;')
            print(r'    }')
            print(r'    if (uHybrid != 0) {')
            print(r'        HMODULE hD3D9 = LoadLibraryA("D3D9");')
            print(r'        assert(hD3D9);')
            print(r'        typedef void (WINAPI *PFNDIRECT3D9FORCEHYBRIDENUMERATION)(UINT);')
            print(r'        PFNDIRECT3D9FORCEHYBRIDENUMERATION pfnDirect3D9ForceHybridEnumeration =')
            print(r'            (PFNDIRECT3D9FORCEHYBRIDENUMERATION)GetProcAddress(hD3D9, MAKEINTRESOURCEA(16));')
            print(r'        if (pfnDirect3D9ForceHybridEnumeration) {')
            print(r'            pfnDirect3D9ForceHybridEnumeration(uHybrid);')
            print(r'        }')
            print(r'    }')

        Retracer.invokeFunction(self, function)

        if function.name.startswith('Direct3DCreate'):
            print(r'    if (retrace::driver == retrace::DRIVER_DISCRETE ||')
            print(r'        retrace::driver == retrace::DRIVER_INTEGRATED) {')
            if function.name == 'Direct3DCreate9Ex':
                print(r'        auto pD3D = SUCCEEDED(_result) ? *ppD3D : nullptr;')
            else:
                print(r'        auto pD3D = _result;')
            if function.name.startswith('Direct3DCreate9'):
                print(r'        D3DADAPTER_IDENTIFIER9 Identifier;')
            else:
                assert function.name.startswith('Direct3DCreate8')
                print(r'        D3DADAPTER_IDENTIFIER8 Identifier;')
            print(r'        if (pD3D) {')
            print(r'            if (SUCCEEDED(pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &Identifier))) {')
            print(r'                std::cerr << "info: using " << Identifier.Description << std::endl;')
            print(r'            }')
            print(r'        }')
            print(r'    }')

    createDeviceMethodNames = [
        'CreateDevice',
        'CreateDeviceEx',
    ]

    def doInvokeInterfaceMethod(self, interface, method):
        Retracer.doInvokeInterfaceMethod(self, interface, method)

        # Gracefully handle missing REF driver.
        if method.name in self.createDeviceMethodNames:
            print(r'    if (_result == D3DERR_NOTAVAILABLE && DeviceType == D3DDEVTYPE_REF) {')
            print(r'        retrace::warning(call) << "reference driver not available, continuing with software vertex processing\n";')
            print(r'        DeviceType = D3DDEVTYPE_HAL;')
            print(r'        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;')
            print(r'        BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;')
            print(r'        BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;')
            Retracer.doInvokeInterfaceMethod(self, interface, method)
            print(r'    }')

        # Keep retrying IDirectXVideoDecoder::BeginFrame when returns E_PENDING
        if interface.name == 'IDirectXVideoDecoder' and method.name == 'BeginFrame':
            print(r'    while (_result == E_PENDING) {')
            print(r'        Sleep(1);')
            Retracer.doInvokeInterfaceMethod(self, interface, method)
            print(r'    }')

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
            if method.name == 'Release':
                print(r'    if (call.ret->toUInt() == 0) {')
                print(r'        d3d9Dumper.unbindDevice(_this);')
                print(r'    }')
            else:
                print(r'    d3d9Dumper.bindDevice(_this);')
        if interface.name in ('IDirect3DDevice8', 'IDirect3DDevice8Ex'):
            if method.name == 'Release':
                print(r'    if (call.ret->toUInt() == 0) {')
                print(r'        d3d8Dumper.unbindDevice(_this);')
                print(r'    }')
            else:
                print(r'    d3d8Dumper.bindDevice(_this);')

        if method.name in self.createDeviceMethodNames:
            # override the device type
            print(r'    switch (retrace::driver) {')
            print(r'    case retrace::DRIVER_HARDWARE:')
            print(r'    case retrace::DRIVER_DISCRETE:')
            print(r'    case retrace::DRIVER_INTEGRATED:')
            print(r'        DeviceType = D3DDEVTYPE_HAL;')
            print(r'        break;')
            print(r'    case retrace::DRIVER_SOFTWARE:')
            print(r'        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;')
            print(r'        BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;')
            print(r'        BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;')
            print(r'        break;')
            print(r'    case retrace::DRIVER_REFERENCE:')
            print(r'        DeviceType = D3DDEVTYPE_REF;')
            print(r'        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;')
            print(r'        BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;')
            print(r'        BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;')
            print(r'        break;')
            print(r'    case retrace::DRIVER_NULL:')
            if interface.name.startswith('IDirect3D9'):
                print(r'        DeviceType = D3DDEVTYPE_NULLREF;')
            else:
                print(r'        retrace::warning(call) << "null driver not supported\n";')
            print(r'        break;')
            print(r'    case retrace::DRIVER_MODULE:')
            print(r'        retrace::warning(call) << "driver module not supported\n";')
            print(r'        break;')
            print(r'    default:')
            print(r'        assert(0);')
            print(r'        /* fall-through */')
            print(r'    case retrace::DRIVER_DEFAULT:')
            print(r'        break;')
            print(r'    }')

        # create windows as neccessary
        if method.name in ('CreateDevice', 'CreateDeviceEx', 'CreateAdditionalSwapChain'):
            print(r'    HWND hWnd = pPresentationParameters->hDeviceWindow;')
            if 'hFocusWindow' in method.argNames():
                print(r'    if (hWnd == NULL) {')
                print(r'        hWnd = hFocusWindow;')
                print(r'    }')
            print(r'    hWnd = d3dretrace::createWindow(hWnd, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);')
            print(r'    pPresentationParameters->hDeviceWindow = hWnd;')
            if 'hFocusWindow' in method.argNames():
                print(r'    hFocusWindow = hWnd;')

            # force windowed mode
            print(r'    if (retrace::forceWindowed) {')
            print(r'        pPresentationParameters->Windowed = TRUE;')
            print(r'        pPresentationParameters->FullScreen_RefreshRateInHz = 0;')
            if interface.name.startswith('IDirect3D9'):
                print(r'        if (pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_TWO')
                print(r'            || pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_THREE')
                print(r'            || pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_FOUR)')
                print(r'            pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;')
            if method.name == 'CreateDeviceEx':
                print(r'   pFullscreenDisplayMode = nullptr;')
            if interface.name.startswith('IDirect3D8'):
                print(r'        pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;')
            print(r'    }')
            if 'BehaviorFlags' in method.argNames():
                print(r'    if (retrace::dumpingState) {')
                print(r'        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;')
                print(r'    }')

            # On D3D8, ensure we use BackBufferFormat compatible with the
            # current DisplayFormat.
            #
            # TODO: BackBufferFormat doesn't need to be always exectly to
            # DisplayFormat.  For example, if DisplayFormat is D3DFMT_X1R5G5B5,
            # valid values for BackBufferFormat include D3DFMT_X1R5G5B5 and
            # D3DFMT_A1R5G5B5, but exclude D3DFMT_R5G6B5.
            if interface.name.startswith('IDirect3D8'):
                print(r'    if (pPresentationParameters->Windowed) {')
                print(r'        D3DDISPLAYMODE Mode;')
                print(r'        HRESULT hr;')
                print(r'        hr = _this->GetAdapterDisplayMode(Adapter, &Mode);')
                print(r'        assert(SUCCEEDED(hr));')
                print(r'        hr = _this->CheckDeviceType(Adapter, DeviceType, Mode.Format, pPresentationParameters->BackBufferFormat, pPresentationParameters->Windowed);')
                print(r'        if (hr == D3DERR_NOTAVAILABLE) {')
                print(r'            retrace::warning(call) << "forcing back buffer format to match display mode format\n";')
                print(r'            pPresentationParameters->BackBufferFormat = Mode.Format;')
                print(r'        }')
                print(r'    }')

        if method.name in ('Reset', 'ResetEx'):
            # force windowed mode
            print(r'    if (retrace::forceWindowed) {')
            print(r'        pPresentationParameters->Windowed = TRUE;')
            print(r'        pPresentationParameters->FullScreen_RefreshRateInHz = 0;')
            if interface.name.startswith('IDirect3DDevice9'):
                print(r'        if (pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_TWO')
                print(r'            || pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_THREE')
                print(r'            || pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_FOUR)')
                print(r'            pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;')
            if interface.name.startswith('IDirect3DDevice8'):
                print(r'        pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;')
            print(r'    }')

            # resize window
            print(r'    if (pPresentationParameters->Windowed) {')
            print(r'        d3dretrace::resizeWindow(pPresentationParameters->hDeviceWindow, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);')
            print(r'    }')

        # notify frame has been completed
        if method.name in ('Present', 'PresentEx'):
            if interface.name.startswith('IDirect3DSwapChain9'):
                print(r'    d3d9scDumper.bindDevice(_this);')
            print(r'    retrace::frameComplete(call);')
            print(r'    hDestWindowOverride = NULL;')

        # Ensure textures can be locked when dumping
        # TODO: Pre-check with CheckDeviceFormat
        if method.name in ('CreateTexture', 'CreateCubeTexture', 'CreateVolumeTexture'):
            print(r'    if (retrace::dumpingState &&')
            print(r'        Pool == D3DPOOL_DEFAULT &&')
            print(r'        !(Usage & (D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL))) {')
            print(r'        Usage |= D3DUSAGE_DYNAMIC;')
            print(r'    }')

        # Deal with shared surfaces
        # https://msdn.microsoft.com/en-us/library/windows/desktop/bb219800.aspx
        # https://msdn.microsoft.com/en-gb/library/windows/desktop/ee913554.aspx
        pSharedHandleArg = method.getArgByName('pSharedHandle')
        if pSharedHandleArg:
            print(r'    if (pSharedHandle) {')
            if method.name == 'CreateTexture':
                # Some applications (e.g., DOTA2) create shared resources within the same process.
                # https://msdn.microsoft.com/en-us/library/windows/desktop/bb219800.aspx#Textures
                print(r'        if (Pool == D3DPOOL_SYSTEMMEM) {')
                print(r'            // Ensure the memory stays around.')
                print(r'            trace::Blob *blob = call.arg(%u).toArray()->values[0]->toBlob();' % pSharedHandleArg.index)
                print(r'            if (blob) {')
                print(r'                blob->toPointer(true);')
                print(r'            } else {')
                print(r'                retrace::warning(call) << "invalid system memory\n";')
                print(r'                pSharedHandle = NULL;')
                print(r'            }')
                print(r'        } else {')
            print(r'        retrace::warning(call) << "shared surfaces unsupported\n";')
            print(r'        pSharedHandle = NULL;')
            if method.name == 'CreateTexture':
                print(r'        }')
            print(r'    }')

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            # Reset _DONOTWAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('Flags')
            for flag in mapFlagsArg.type.values:
                if flag.endswith('_DONOTWAIT'):
                    print(r'    Flags &= ~%s;' % flag)

        # SetClipPlane methods were broken until cf519352c
        if method.name == 'SetClipPlane':
            print(r'    if (retrace::parser->getVersion() < 6) {')
            print(r'        const trace::Array *_pPlane = call.arg(2).toArray();')
            print(r'        if (_pPlane && _pPlane->values.size() < 4) {')
            print(r'            static bool _warned = false;')
            print(r'            if (!_warned) {')
            print(r'                retrace::warning(call) << "ignoring incomplete SetClipPlane plane\n";')
            print(r'                _warned = true;')
            print(r'            }')
            print(r'            static float _zeroPlane[4] = { 0.0f, 0.0f, 0.0f, 0.0f };')
            print(r'            pPlane = _zeroPlane;')
            print(r'        }')
            print(r'    }')

        Retracer.invokeInterfaceMethod(self, interface, method)

        # process events after presents
        if method.name == 'Present':
            print(r'    d3dretrace::processEvents();')

        def mapping_subkey():
            # A single texture object might have multiple mappings.  This key
            # allows to tell them apart.
            if 'FaceType' in method.argNames():
                return ('static_cast<UINT>(FaceType) + Level*6',)
            elif 'Level' in method.argNames():
                return ('Level',)
            else:
                return ('0',)

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print('    VOID *_pbData = nullptr;')
            print('    size_t _MappedSize = 0;')
            if method.name == 'Lock':
                # Ignore D3DLOCK_READONLY for buffers.
                # https://github.com/apitrace/apitrace/issues/435
                print('    if (true) {')
            else:
                print('    if (!(Flags & D3DLOCK_READONLY)) {')
            print('        _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames()[:-1]))
            print('    }')
            print('    if (_MappedSize) {')
            print('        _maps[MappingKey(_this, %s)] = _pbData;' % mapping_subkey())
            self.checkPitchMismatch(method)
            print('    } else {')
            print('        return;')
            print('    }')
        
        if method.name in ('Unlock', 'UnlockRect', 'UnlockBox'):
            print('    VOID *_pbData = nullptr;')
            print('    MappingKey _mappingKey(_this, %s);' % mapping_subkey())
            print('    _pbData = _maps[_mappingKey];')
            print('    if (_pbData) {')
            print('        retrace::delRegionByPointer(_pbData);')
            print('        _maps[_mappingKey] = nullptr;')
            print('    }')

        if interface.name == 'IDirectXVideoDecoder':
            if method.name == 'GetBuffer':
                print('    if (*ppBuffer && *pBufferSize) {')
                print('        _maps[MappingKey(_this, BufferType)] = *ppBuffer;')
                print('    }')
            if method.name == 'ReleaseBuffer':
                print('    MappingKey _mappingKey(_this, BufferType);')
                print('    void *_pBuffer = _maps[_mappingKey];')
                print('    if (_pBuffer) {')
                print('        retrace::delRegionByPointer(_pBuffer);')
                print('        _maps[_mappingKey] = nullptr;')
                print('    }')

    def handleFailure(self, interface, methodOrFunction):
        if methodOrFunction.name in self.createDeviceMethodNames:
            print(r'        exit(EXIT_FAILURE);')
            return

        # https://msdn.microsoft.com/en-us/library/windows/desktop/bb324479.aspx
        if methodOrFunction.name in ('Present', 'PresentEx'):
            print(r'        if (_result == D3DERR_DEVICELOST) {')
            print(r'            exit(EXIT_FAILURE);')
            print(r'        }')

        Retracer.handleFailure(self, interface, methodOrFunction)


def main():
    print(r'#include <string.h>')
    print()
    print(r'#include <iostream>')
    print()
    print(r'#include "d3dretrace.hpp"')
    print()

    moduleName = sys.argv[1]
    support = int(sys.argv[2])

    api = API()
    
    if support:
        if moduleName == 'd3d9':
            from specs.d3d9 import d3d9, d3dperf
            from specs.dxva2 import dxva2
            print(r'#include "d3d9imports.hpp"')
            print(r'#include "d3d9size.hpp"')
            print(r'#include "dxva2imports.hpp"')
            d3d9.mergeModule(d3dperf)
            api.addModule(d3d9)
            api.addModule(dxva2)
            print()
            print('''static d3dretrace::D3DDumper<IDirect3DDevice9> d3d9Dumper;''')
            print('''static d3dretrace::D3DDumper<IDirect3DSwapChain9> d3d9scDumper;''')
            print()
        elif moduleName == 'd3d8':
            from specs.d3d8 import d3d8
            print(r'#include "d3d8imports.hpp"')
            print(r'#include "d3d8size.hpp"')
            api.addModule(d3d8)
            print()
            print('''static d3dretrace::D3DDumper<IDirect3DDevice8> d3d8Dumper;''')
            print()
        else:
            assert False

    retracer = D3DRetracer()
    retracer.table_name = 'd3dretrace::%s_callbacks' % moduleName
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

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
        print '// Swizzling mapping for lock addresses'
        print 'typedef std::pair<void *, UINT> MappingKey;'
        print 'static std::map<MappingKey, void *> _maps;'
        print

        Retracer.retraceApi(self, api)

    def invokeFunction(self, function):
        if function.name in ('Direct3DCreate9', 'Direct3DCreate9Ex'):
            print 'if (retrace::debug >= 2 && !g_szD3D9DllName && LoadLibraryA("d3d9d.dll")) {'
            print '    /*'
            print '     * D3D9D only works for simple applications, it will often report bogus errors'
            print '     * on complex traces, or traces which use unofficial D3D9 features.'
            print '     */'
            print '    g_szD3D9DllName = "d3d9d.dll";'
            print '    SDKVersion |= 0x80000000;'
            print '} else {'
            print '    SDKVersion &= ~0x80000000;'
            print '}'

        # d3d8d.dll can be found in the Aug 2007 DXSDK.  It works on XP, but
        # not on Windows 7.
        if function.name in ('Direct3DCreate8'):
            print 'if (retrace::debug >= 2 && !g_szD3D8DllName && LoadLibraryA("d3d8d.dll")) {'
            print '    g_szD3D8DllName = "d3d8d.dll";'
            print '}'

        Retracer.invokeFunction(self, function)

    createDeviceMethodNames = [
        'CreateDevice',
        'CreateDeviceEx',
    ]

    def doInvokeInterfaceMethod(self, interface, method):
        Retracer.doInvokeInterfaceMethod(self, interface, method)

        # Keep retrying IDirectXVideoDecoder::BeginFrame when returns E_PENDING
        if interface.name == 'IDirectXVideoDecoder' and method.name == 'BeginFrame':
            print r'    while (_result == E_PENDING) {'
            print r'        Sleep(1);'
            Retracer.doInvokeInterfaceMethod(self, interface, method)
            print r'    }'

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d9Dumper.unbindDevice(_this);'
                print r'        if (retrace::profiling) {'
                print r'            d3dretrace::setProfileDevice(NULL);'
                print r'        }'
                print r'    }'
            else:
                print r'    d3d9Dumper.bindDevice(_this);'
                print r'    if (retrace::profiling) {'
                print r'        d3dretrace::setProfileDevice(_this);'
                print r'    }'
        if interface.name in ('IDirect3DDevice8', 'IDirect3DDevice8Ex'):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d8Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    d3d8Dumper.bindDevice(_this);'

        # create windows as neccessary
        if method.name in ('CreateDevice', 'CreateDeviceEx', 'CreateAdditionalSwapChain'):
            print r'    HWND hWnd = pPresentationParameters->hDeviceWindow;'
            if 'hFocusWindow' in method.argNames():
                print r'    if (hWnd == NULL) {'
                print r'        hWnd = hFocusWindow;'
                print r'    }'
            print r'    hWnd = d3dretrace::createWindow(hWnd, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    pPresentationParameters->hDeviceWindow = hWnd;'
            if 'hFocusWindow' in method.argNames():
                print r'    hFocusWindow = hWnd;'

            # force windowed mode
            print r'    if (retrace::forceWindowed) {'
            print r'        pPresentationParameters->Windowed = TRUE;'
            print r'        pPresentationParameters->FullScreen_RefreshRateInHz = 0;'
            if interface.name.startswith('IDirect3D8'):
                print r'        pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;'
            print r'    }'
            if 'BehaviorFlags' in method.argNames():
                print r'    if (retrace::dumpingState) {'
                print r'        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;'
                print r'    }'

            # On D3D8, ensure we use BackBufferFormat compatible with the
            # current DisplayFormat.
            #
            # TODO: BackBufferFormat doesn't need to be always exectly to
            # DisplayFormat.  For example, if DisplayFormat is D3DFMT_X1R5G5B5,
            # valid values for BackBufferFormat include D3DFMT_X1R5G5B5 and
            # D3DFMT_A1R5G5B5, but exclude D3DFMT_R5G6B5.
            if interface.name.startswith('IDirect3D8'):
                print r'    if (pPresentationParameters->Windowed) {'
                print r'        D3DDISPLAYMODE Mode;'
                print r'        HRESULT hr;'
                print r'        hr = _this->GetAdapterDisplayMode(Adapter, &Mode);'
                print r'        assert(SUCCEEDED(hr));'
                print r'        hr = _this->CheckDeviceType(Adapter, DeviceType, Mode.Format, pPresentationParameters->BackBufferFormat, pPresentationParameters->Windowed);'
                print r'        if (hr == D3DERR_NOTAVAILABLE) {'
                print r'            retrace::warning(call) << "forcing back buffer format to match display mode format\n";'
                print r'            pPresentationParameters->BackBufferFormat = Mode.Format;'
                print r'        }'
                print r'    }'
        
        if method.name in self.createDeviceMethodNames:
            # override the device type
            print r'    switch (retrace::driver) {'
            print r'    case retrace::DRIVER_HARDWARE:'
            print r'        DeviceType = D3DDEVTYPE_HAL;'
            print r'        break;'
            print r'    case retrace::DRIVER_SOFTWARE:'
            print r'    case retrace::DRIVER_REFERENCE:'
            print r'        DeviceType = D3DDEVTYPE_REF;'
            print r'        break;'
            print r'    case retrace::DRIVER_NULL:'
            if interface.name.startswith('IDirect3D9'):
                print r'        DeviceType = D3DDEVTYPE_NULLREF;'
            else:
                print r'        retrace::warning(call) << "null driver not supported\n";'
            print r'        break;'
            print r'    case retrace::DRIVER_MODULE:'
            print r'        retrace::warning(call) << "driver module not supported\n";'
            print r'        break;'
            print r'    default:'
            print r'        assert(0);'
            print r'        /* fall-through */'
            print r'    case retrace::DRIVER_DEFAULT:'
            print r'        break;'
            print r'    }'

        if method.name in ('Reset', 'ResetEx'):
            # force windowed mode
            print r'    if (retrace::forceWindowed) {'
            print r'        pPresentationParameters->Windowed = TRUE;'
            print r'        pPresentationParameters->FullScreen_RefreshRateInHz = 0;'
            print r'    }'
            # resize window
            print r'    if (pPresentationParameters->Windowed) {'
            print r'        d3dretrace::resizeWindow(pPresentationParameters->hDeviceWindow, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    }'

        # notify frame has been completed
        if method.name in ('Present', 'PresentEx'):
            if interface.name.startswith('IDirect3DSwapChain9'):
                print r'    d3d9scDumper.bindDevice(_this);'
            print r'    d3dretrace::frame_complete(call);'
            print r'    hDestWindowOverride = NULL;'

        # Ensure textures can be locked when dumping
        # TODO: Pre-check with CheckDeviceFormat
        if method.name in ('CreateTexture', 'CreateCubeTexture', 'CreateVolumeTexture'):
            print r'    if (retrace::dumpingState &&'
            print r'        Pool == D3DPOOL_DEFAULT &&'
            print r'        !(Usage & (D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL))) {'
            print r'        Usage |= D3DUSAGE_DYNAMIC;'
            print r'    }'

        # Deal with shared surfaces
        # https://msdn.microsoft.com/en-us/library/windows/desktop/bb219800.aspx
        # https://msdn.microsoft.com/en-gb/library/windows/desktop/ee913554.aspx
        pSharedHandleArg = method.getArgByName('pSharedHandle')
        if pSharedHandleArg:
            print r'    if (pSharedHandle) {'
            if method.name == 'CreateTexture':
                # Some applications (e.g., DOTA2) create shared resources within the same process.
                # https://msdn.microsoft.com/en-us/library/windows/desktop/bb219800.aspx#Textures
                print r'        if (Pool == D3DPOOL_SYSTEMMEM) {'
                print r'            // Ensure the memory stays around.'
                print r'            trace::Blob *blob = call.arg(%u).toArray()->values[0]->toBlob();' % pSharedHandleArg.index
                print r'            if (blob) {'
                print r'                blob->toPointer(true);'
                print r'            } else {'
                print r'                retrace::warning(call) << "invalid system memory\n";'
                print r'                pSharedHandle = NULL;'
                print r'            }'
                print r'        } else {'
            print r'        retrace::warning(call) << "shared surfaces unsupported\n";'
            print r'        pSharedHandle = NULL;'
            if method.name == 'CreateTexture':
                print r'        }'
            print r'    }'

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            # Reset _DONOTWAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('Flags')
            for flag in mapFlagsArg.type.values:
                if flag.endswith('_DONOTWAIT'):
                    print r'    Flags &= ~%s;' % flag

        if method.name.startswith("Unlock") or method.name.startswith("Lock"):
            print r'    if (retrace::profiling) {'
            print r'        d3dretrace::beginProfile(call, false);'
            print r'    }'

        if method.name.startswith("Draw"):
            print r'    if (retrace::profiling) {'
            print r'        d3dretrace::beginProfile(call, true);'
            print r'    }'

        Retracer.invokeInterfaceMethod(self, interface, method)

        if method.name.startswith("Draw"):
            print r'    if (retrace::profiling) {'
            print r'        d3dretrace::endProfile(call, true);'
            print r'    }'

        if method.name.startswith("Unlock") or method.name.startswith("Lock"):
            print r'    if (retrace::profiling) {'
            print r'        d3dretrace::endProfile(call, false);'
            print r'    }'

        if method.name in self.createDeviceMethodNames:
            print r'    if (FAILED(_result)) {'
            print r'        exit(1);'
            print r'    }'

        # process events after presents
        if method.name == 'Present':
            print r'    d3dretrace::processEvents();'

        def mapping_subkey():
            if 'Level' in method.argNames():
                return ('Level',)
            else:
                return ('0',)

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print '    VOID *_pbData = NULL;'
            print '    size_t _MappedSize = 0;'
            if method.name == 'Lock':
                # Ignore D3DLOCK_READONLY for buffers.
                # https://github.com/apitrace/apitrace/issues/435
                print '    if (true) {'
            else:
                print '    if (!(Flags & D3DLOCK_READONLY)) {'
            print '        _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames()[:-1])
            print '    }'
            print '    if (_MappedSize) {'
            print '        _maps[MappingKey(_this, %s)] = _pbData;' % mapping_subkey()
            self.checkPitchMismatch(method)
            print '    } else {'
            print '        return;'
            print '    }'
        
        if method.name in ('Unlock', 'UnlockRect', 'UnlockBox'):
            print '    VOID *_pbData = 0;'
            print '    MappingKey _mappingKey(_this, %s);' % mapping_subkey()
            print '    _pbData = _maps[_mappingKey];'
            print '    if (_pbData) {'
            print '        retrace::delRegionByPointer(_pbData);'
            print '        _maps[_mappingKey] = 0;'
            print '    }'

        if interface.name == 'IDirectXVideoDecoder':
            if method.name == 'GetBuffer':
                print '    if (*ppBuffer && *pBufferSize) {'
                print '        _maps[MappingKey(_this, BufferType)] = *ppBuffer;'
                print '    }'
            if method.name == 'ReleaseBuffer':
                print '    MappingKey _mappingKey(_this, BufferType);'
                print '    void *_pBuffer = _maps[_mappingKey];'
                print '    if (_pBuffer) {'
                print '        retrace::delRegionByPointer(_pBuffer);'
                print '        _maps[_mappingKey] = 0;'
                print '    }'


def main():
    print r'#include <string.h>'
    print
    print r'#include <iostream>'
    print
    print r'#include "d3dretrace.hpp"'
    print

    moduleName = sys.argv[1]
    support = int(sys.argv[2])

    api = API()
    
    if support:
        if moduleName == 'd3d9':
            from specs.d3d9 import d3d9, d3dperf
            from specs.dxva2 import dxva2
            print r'#include "d3d9imports.hpp"'
            print r'#include "d3d9size.hpp"'
            print r'#include "dxva2imports.hpp"'
            d3d9.mergeModule(d3dperf)
            api.addModule(d3d9)
            api.addModule(dxva2)
            print
            print '''static d3dretrace::D3DDumper<IDirect3DDevice9> d3d9Dumper;'''
            print '''static d3dretrace::D3DDumper<IDirect3DSwapChain9> d3d9scDumper;'''
            print
        elif moduleName == 'd3d8':
            from specs.d3d8 import d3d8
            print r'#include "d3d8imports.hpp"'
            print r'#include "d3d8size.hpp"'
            api.addModule(d3d8)
            print
            print '''static d3dretrace::D3DDumper<IDirect3DDevice8> d3d8Dumper;'''
            print
        else:
            assert False

    retracer = D3DRetracer()
    retracer.table_name = 'd3dretrace::%s_callbacks' % moduleName
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

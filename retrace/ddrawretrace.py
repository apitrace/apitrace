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
from specs.d3d import ddraw, HWND


class D3DRetracer(Retracer):

    def retraceApi(self, api):
        print '// Swizzling mapping for lock addresses'
        print 'typedef std::pair<void *, UINT> MappingKey;'
        print 'static std::map<MappingKey, void *> _maps;'
        print
        print 'static HWND g_hWnd;'
        print

        Retracer.retraceApi(self, api)

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice7',):
            if method.name == 'Release':
                print r'    if (call.ret->toUInt() == 0) {'
                print r'        d3d7Dumper.unbindDevice(_this);'
                print r'    }'
            else:
                print r'    d3d7Dumper.bindDevice(_this);'

        # create windows as neccessary
        hWndArg = method.getArgByType(HWND)
        if hWndArg is not None:
            print r'    if (!g_hWnd) {'
            print r'        g_hWnd = d3dretrace::createWindow(512, 512);'
            print r'    }'
            print r'    %s = g_hWnd;' % hWndArg.name


        # Ensure textures can be locked when dumping
        # TODO: Pre-check with CheckDeviceFormat
        if method.name in ('CreateTexture', 'CreateCubeTexture', 'CreateVolumeTexture'):
            print r'    if (retrace::dumpingState &&'
            print r'        Pool == D3DPOOL_DEFAULT &&'
            print r'        !(Usage & (D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL))) {'
            print r'        Usage |= D3DUSAGE_DYNAMIC;'
            print r'    }'

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            # Reset _DONOTWAIT flags. Otherwise they may fail, and we have no
            # way to cope with it (other than retry).
            mapFlagsArg = method.getArgByName('dwFlags')
            if mapFlagsArg is not None:
                for flag in mapFlagsArg.type.values:
                    if flag.endswith('_DONOTWAIT'):
                        print r'    dwFlags &= ~%s;' % flag

        Retracer.invokeInterfaceMethod(self, interface, method)

        if method.name == 'CreateDevice':
            print r'    if (FAILED(_result)) {'
            print r'        exit(1);'
            print r'    }'

        # notify frame has been completed
        # process events after presents
        if interface.name == 'IDirectDrawSurface7' and method.name == 'Blt':
            print r'    DDSCAPS2 ddsCaps;'
            print r'    if (SUCCEEDED(_this->GetCaps(&ddsCaps)) &&'
            print r'        (ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)) {'
            print r'        retrace::frameComplete(call);'
            print r'        d3dretrace::processEvents();'
            print r'    }'

        def mapping_subkey():
            if 'Level' in method.argNames():
                return ('Level',)
            else:
                return ('0',)

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print '    VOID *_pbData = NULL;'
            print '    size_t _MappedSize = 0;'
            # FIXME
            #print '    _getMapInfo(_this, %s, _pbData, _MappedSize);' % ', '.join(method.argNames()[:-1])
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


def main():
    print r'#include <string.h>'
    print
    print r'#include <iostream>'
    print
    print r'#include "d3dretrace.hpp"'
    print

    api = API()
    
    print r'#include "d3dimports.hpp"'
    api.addModule(ddraw)
    print
    print '''static d3dretrace::D3DDumper<IDirect3DDevice7> d3d7Dumper;'''
    print

    retracer = D3DRetracer()
    retracer.table_name = 'd3dretrace::ddraw_callbacks'
    retracer.retraceApi(api)


if __name__ == '__main__':
    main()

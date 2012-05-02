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


from dllretrace import DllRetracer as Retracer
import specs.stdapi as stdapi
from specs.d3d9 import *


class D3DRetracer(Retracer):

    table_name = 'd3dretrace::d3d9_callbacks'

    bufferInterfaceNames = [
        'IDirect3DVertexBuffer9',
        'IDirect3DIndexBuffer9',
    ]

    def invokeInterfaceMethod(self, interface, method):
        # keep track of the last used device for state dumping
        if interface.name in ('IDirect3DDevice9', 'IDirect3DDevice9Ex'):
            print r'    d3dretrace::pLastDirect3DDevice9 = _this;'

        # create windows as neccessary
        if method.name in ('CreateDevice', 'CreateDeviceEx'):
            print r'    HWND hWnd = createWindow(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    hFocusWindow = hWnd;'
            print r'    pPresentationParameters->hDeviceWindow = hWnd;'

        Retracer.invokeInterfaceMethod(self, interface, method)

        # check errors
        if str(method.type) == 'HRESULT':
            print r'    if (FAILED(_result)) {'
            print r'        retrace::warning(call) << "failed\n";'
            print r'    }'

        if method.name in ('Lock', 'LockRect', 'LockBox'):
            print '        size_t _LockedSize = _getLockSize(_this, %s);' % ', '.join(method.argNames()[:-1])
            if method.name == 'Lock':
                # FIXME: handle recursive locks
                print '        VOID *_pbData = *ppbData;'
            elif method.name == 'LockRect':
                print '        VOID *_pbData = pLockedRect->pBits;'
            elif method.name == 'LockBox':
                print '        VOID *_pbData = pLockedVolume->pBits;'
            else:
                raise NotImplementedError
            print '        _this->SetPrivateData(GUID_APITRACE, &_pbData, sizeof _pbData, 0);'
        
        if method.name in ('Unlock', 'UnlockRect', 'UnlockBox'):
            print '        VOID *_pbData = 0;'
            print '        DWORD dwSizeOfData = sizeof _pbData;'
            print '        _this->GetPrivateData(GUID_APITRACE, &_pbData, &dwSizeOfData);'
            print '        if (_pbData) {'
            print '            retrace::delRegionByPointer(_pbData);'
            print '        }'


if __name__ == '__main__':
    print r'''
#define INITGUID

#include <string.h>

#include <iostream>

#include "d3d9imports.hpp"
#include "d3dsize.hpp"
#include "d3dretrace.hpp"


// XXX: Don't duplicate this code.

static LRESULT CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MINMAXINFO *pMMI;
    switch (uMsg) {
    case WM_GETMINMAXINFO:
        // Allow to create a window bigger than the desktop
        pMMI = (MINMAXINFO *)lParam;
        pMMI->ptMaxSize.x = 60000;
        pMMI->ptMaxSize.y = 60000;
        pMMI->ptMaxTrackSize.x = 60000;
        pMMI->ptMaxTrackSize.y = 60000;
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


DEFINE_GUID(GUID_APITRACE,0X7D71CAC9,0X7F58,0X432C,0XA9,0X75,0XA1,0X9F,0XCF,0XCE,0XFD,0X14);


static HWND
createWindow(int width, int height) {
    static bool first = TRUE;
    RECT rect;

    if (first) {
        WNDCLASS wc;
        memset(&wc, 0, sizeof wc);
        wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.lpfnWndProc = WndProc;
        wc.lpszClassName = "d3dretrace";
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        RegisterClass(&wc);
        first = FALSE;
    }

    DWORD dwExStyle;
    DWORD dwStyle;
    HWND hWnd;

    dwExStyle = 0;
    dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

    int x = 0, y = 0;

    rect.left = x;
    rect.top = y;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

    hWnd = CreateWindowEx(dwExStyle,
                          "d3dretrace", /* wc.lpszClassName */
                          NULL,
                          dwStyle,
                          0, /* x */
                          0, /* y */
                          rect.right - rect.left, /* width */
                          rect.bottom - rect.top, /* height */
                          NULL,
                          NULL,
                          NULL,
                          NULL);
    ShowWindow(hWnd, SW_SHOW);
    return hWnd;
}



typedef HRESULT
(WINAPI *PD3DXASSEMBLESHADER)(
    LPCSTR pSrcData,
    UINT SrcDataLen,
    const D3DXMACRO *pDefines,
    LPD3DXINCLUDE pInclude,
    DWORD Flags,
    LPD3DXBUFFER *ppShader,
    LPD3DXBUFFER *ppErrorMsgs
);

DWORD *
extractShader(LPCSTR pSrcData)
{
    static BOOL firsttime = TRUE;
    static HMODULE hD3DXModule = NULL;
    static PD3DXASSEMBLESHADER pfnD3DXAssembleShader = NULL;

    if (firsttime) {
        if (!hD3DXModule) {
            unsigned release;
            int version;
            for (release = 0; release <= 1; ++release) {
                /* Version 41 corresponds to Mar 2009 version of DirectX Runtime / SDK */
                for (version = 41; version >= 0; --version) {
                    char filename[256];
                    _snprintf(filename, sizeof(filename),
                              "d3dx9%s%s%u.dll", release ? "" : "d", version ? "_" : "", version);
                    hD3DXModule = LoadLibraryA(filename);
                    if (hD3DXModule)
                        goto found;
                }
            }
found:
            ;
        }

        if (hD3DXModule) {
            if (!pfnD3DXAssembleShader) {
                pfnD3DXAssembleShader = (PD3DXASSEMBLESHADER)GetProcAddress(hD3DXModule, "D3DXAssembleShader");
            }
        }

        firsttime = FALSE;
    }

    if (pfnD3DXAssembleShader) {
        LPD3DXBUFFER pTokens = NULL;
        HRESULT hr;

        hr = pfnD3DXAssembleShader(pSrcData, strlen(pSrcData), NULL, NULL, 0, &pTokens, NULL);
        if (SUCCEEDED(hr)) {
            return (DWORD *)pTokens->GetBufferPointer();
        }

        // FIXME: Don't leak pTokens
    }

    return NULL;
}
'''

    retracer = D3DRetracer()
    retracer.retraceApi(d3d9)

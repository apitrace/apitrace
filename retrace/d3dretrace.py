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

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        if arg.type is D3DSHADER9:
            print r'    %s = extractShader((%s).toString());' % (lvalue, rvalue)
            print r'    if (!%s) {' % lvalue
            print r'        retrace::warning(call) << "failed to assemble shader\n";'
            print r'        return;'
            print r'    }'
            return
            
        Retracer.extractArg(self, function, arg, arg_type, lvalue, rvalue)

    def invokeInterfaceMethod(self, interface, method):
        if method.name in ('CreateDevice', 'CreateDeviceEx'):
            print r'    HWND hWnd = createWindow(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);'
            print r'    hFocusWindow = hWnd;'
            print r'    pPresentationParameters->hDeviceWindow = hWnd;'

        Retracer.invokeInterfaceMethod(self, interface, method)

        if str(method.type) == 'HRESULT':
            print r'    if (FAILED(_result)) {'
            print r'        retrace::warning(call) << DXGetErrorString9(_result) << ": " << DXGetErrorDescription9(_result) << "\n";'
            print r'        return;'
            print r'    }'

        if interface.name in self.bufferInterfaceNames and method.name == 'Lock':
            getDescMethod = interface.getMethodByName('GetDesc')
            descArg = getDescMethod.args[0]
            assert descArg.output
            descType = getDescMethod.args[0].type.type

            print '        if (!SizeToLock) {'
            print '            %s Desc;' % descType
            print '            _this->GetDesc(&Desc);'
            print '            SizeToLock = Desc.Size;'
            print '        }'


if __name__ == '__main__':
    print r'''
#include <string.h>

#include <iostream>

#include "d3d9imports.hpp"
#include <dxerr9.h>
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
        if (hr == D3D_OK) {
            return (DWORD *)pTokens->GetBufferPointer();
        }

        // FIXME: Don't leak pTokens
    }

    return NULL;
}
'''

    d3d9.name = 'd3d9d'
    retracer = D3DRetracer()
    retracer.retraceApi(d3d9)

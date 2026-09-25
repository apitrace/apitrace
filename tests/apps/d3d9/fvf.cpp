/**************************************************************************
 *
 * Copyright 2012-2021 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <stdio.h>
#include <windows.h>

#include <d3d9.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


int
main(int argc, char *argv[])
{
    HRESULT hr;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        DefWindowProc,
        0,
        0,
        hInstance,
        NULL,
        NULL,
        NULL,
        NULL,
        "fvf",
        NULL
    };
    RegisterClassEx(&wc);

    const int WindowWidth = 250;
    const int WindowHeight = 250;

    DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

    RECT rect = {0, 0, WindowWidth, WindowHeight};
    AdjustWindowRect(&rect, dwStyle, FALSE);

    HWND hWnd = CreateWindow(wc.lpszClassName,
                             "Simple example using DirectX9",
                             dwStyle,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
                             NULL,
                             NULL,
                             hInstance,
                             NULL);
    if (!hWnd) {
        return 1;
    }

    ComPtr<IDirect3D9> pD3D;
    pD3D.Attach(Direct3DCreate9(D3D_SDK_VERSION));
    if (!pD3D) {
        return 1;
    }

    D3DCAPS9 caps;
    hr = pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if (FAILED(hr)) {
       return 1;
    }

    DWORD dwBehaviorFlags;
    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION(1, 1)) {
       dwBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    } else {
       dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    D3DPRESENT_PARAMETERS PresentationParameters;
    ZeroMemory(&PresentationParameters, sizeof PresentationParameters);
    PresentationParameters.Windowed = TRUE;
    PresentationParameters.BackBufferCount = 1;
    PresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    PresentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
    PresentationParameters.hDeviceWindow = hWnd;

    PresentationParameters.EnableAutoDepthStencil = FALSE;
    PresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    ComPtr<IDirect3DDevice9> pDevice;
    hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT,
                              D3DDEVTYPE_HAL,
                              hWnd,
                              dwBehaviorFlags,
                              &PresentationParameters,
                              &pDevice);
    if (FAILED(hr)) {
        return 1;
    }

    pDevice->SetFVF(D3DFVF_RESERVED0);
    pDevice->SetFVF(D3DFVF_XYZ);
    pDevice->SetFVF(D3DFVF_XYZRHW);
    pDevice->SetFVF(D3DFVF_XYZB1);
    pDevice->SetFVF(D3DFVF_XYZB2);
    pDevice->SetFVF(D3DFVF_XYZB3);
    pDevice->SetFVF(D3DFVF_XYZB4);
    pDevice->SetFVF(D3DFVF_XYZB5);
    pDevice->SetFVF(D3DFVF_XYZW);
    pDevice->SetFVF(D3DFVF_NORMAL);
    pDevice->SetFVF(D3DFVF_PSIZE);
    pDevice->SetFVF(D3DFVF_DIFFUSE);
    pDevice->SetFVF(D3DFVF_SPECULAR);
    pDevice->SetFVF(D3DFVF_TEX0);
    pDevice->SetFVF(D3DFVF_TEX1);
    pDevice->SetFVF(D3DFVF_TEX2);
    pDevice->SetFVF(D3DFVF_TEX3);
    pDevice->SetFVF(D3DFVF_TEX4);
    pDevice->SetFVF(D3DFVF_TEX5);
    pDevice->SetFVF(D3DFVF_TEX6);
    pDevice->SetFVF(D3DFVF_TEX7);
    pDevice->SetFVF(D3DFVF_TEX8);
    pDevice->SetFVF(D3DFVF_LASTBETA_UBYTE4);
    pDevice->SetFVF(D3DFVF_LASTBETA_D3DCOLOR);
    pDevice->SetFVF(D3DFVF_RESERVED2);

    for (UINT CoordIndex = 0; CoordIndex < 8; ++CoordIndex) {
        pDevice->SetFVF(D3DFVF_TEXCOORDSIZE1(CoordIndex));
        pDevice->SetFVF(D3DFVF_TEXCOORDSIZE2(CoordIndex));
        pDevice->SetFVF(D3DFVF_TEXCOORDSIZE3(CoordIndex));
        pDevice->SetFVF(D3DFVF_TEXCOORDSIZE4(CoordIndex));
    }

    DestroyWindow(hWnd);

    return 0;
}


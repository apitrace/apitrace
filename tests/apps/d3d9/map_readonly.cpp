/**************************************************************************
 *
 * Copyright 2016 VMware, Inc.
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
        "tri",
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

    ShowWindow(hWnd, SW_SHOW);

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

    struct Vertex {
        float x, y, z;
        DWORD color;
    };

    D3DCOLOR clearColor = D3DCOLOR_COLORVALUE(0.3f, 0.1f, 0.3f, 1.0f);
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
    pDevice->BeginScene();

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    static const Vertex vertices[] = {
        { -0.9f, -0.9f, 0.5f, D3DCOLOR_COLORVALUE(0.8f, 0.0f, 0.0f, 0.1f) },
        {  0.9f, -0.9f, 0.5f, D3DCOLOR_COLORVALUE(0.0f, 0.9f, 0.0f, 0.1f) },
        {  0.0f,  0.9f, 0.5f, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.7f, 0.1f) },
    };

    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

    ComPtr<IDirect3DVertexBuffer9> pVertexBuffer;
    hr = pDevice->CreateVertexBuffer( sizeof vertices, 0, FVF, D3DPOOL_DEFAULT, &pVertexBuffer, NULL);
    if (FAILED(hr)) {
        return 1;
    }

    void *pData;
    hr = pVertexBuffer->Lock(0, 0, &pData, D3DLOCK_READONLY);
    if (FAILED(hr)) {
        return 1;
    }

    memcpy(pData, vertices, sizeof vertices);
    pVertexBuffer->Unlock();

    pDevice->SetStreamSource(0, pVertexBuffer.Get(), 0, sizeof vertices[0]);
    pDevice->SetFVF(FVF);

    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    pDevice->EndScene();

    pDevice->Present(NULL, NULL, NULL, NULL);

    Sleep(1000);

    DestroyWindow(hWnd);

    return 0;
}


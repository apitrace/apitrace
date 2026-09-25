/**************************************************************************
 *
 * Copyright 2016 VMware, Inc
 * Copyright 2012 Jose Fonseca
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
        "tex_sysmem",
        NULL
    };
    RegisterClassEx(&wc);

    const int WindowWidth = 256;
    const int WindowHeight = 256;

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

    ComPtr<IDirect3D9Ex> pD3D;
    hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D);
    if (FAILED(hr)) {
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

    ComPtr<IDirect3DDevice9Ex> pDevice;
    hr = pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT,
                              D3DDEVTYPE_HAL,
                              hWnd,
                              dwBehaviorFlags,
                              &PresentationParameters,
                              nullptr,
                              &pDevice);
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<IDirect3DTexture9> pTexture;
    hr = pDevice->CreateTexture(32, 32, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, nullptr);
    if (FAILED(hr)) {
        return 1;
    }

    {
        static DWORD texels[32][32];
        for (unsigned y = 0; y < 32; ++y) {
            for (unsigned x = 0; x < 32; ++x) {
                if (((y ^ x) / 16) & 1) {
                    texels[y][x] = 0xffffffff;
                } else {
                    texels[y][x] = 0xffff0000;
                }
            }
        }

        HANDLE hTexels = reinterpret_cast<HANDLE>(texels);

        ComPtr<IDirect3DTexture9> pStagingTexture;
        hr = pDevice->CreateTexture(32, 32, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pStagingTexture, &hTexels);
        if (FAILED(hr)) {
            return 1;
        }

        hr = pDevice->UpdateTexture(pStagingTexture.Get(), pTexture.Get());
        if (FAILED(hr)) {
            return 1;
        }
    }

    D3DCOLOR clearColor = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 1.0f, 1.0f);
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
    pDevice->BeginScene();

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

    struct Vertex {
        float x, y, z;
        float s, t;
    };

    static const Vertex vertices[] = {
        { -0.8f, -0.8f, 0.5f, 0.0f, 1.0f },
        {  0.8f, -0.8f, 0.5f, 1.0f, 1.0f },
        { -0.8f,  0.8f, 0.5f, 0.0f, 0.0f },
        {  0.8f,  0.8f, 0.5f, 1.0f, 0.0f }
    };

    pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

    pDevice->SetTexture(0, pTexture.Get());

    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex));

    pDevice->EndScene();

    pDevice->Present(NULL, NULL, NULL, NULL);

    DestroyWindow(hWnd);

    return 0;
}


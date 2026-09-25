/**************************************************************************
 *
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


#include <stdlib.h>
#include <stdio.h>

#include <initguid.h>

#include <windows.h>

#include "winsdk_compat.h"

#include <d3d.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


int main(int argc, char *argv[])
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
        "SimpleDX7",
        NULL
    };
    RegisterClassEx(&wc);

    const int WindowWidth = 250;
    const int WindowHeight = 250;

    DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

    RECT rect = {0, 0, WindowWidth, WindowHeight};
    AdjustWindowRect(&rect, dwStyle, FALSE);

    HWND hWnd = CreateWindow(wc.lpszClassName,
                             "Simple example using DirectX7",
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

    ComPtr<IDirectDraw7> pDD;
    hr = DirectDrawCreateEx(NULL, (void **)&pDD, IID_IDirectDraw7, NULL);
    if (FAILED(hr)) {
        return 1;
    }

    hr = pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if (FAILED(hr)) {
        return 1;
    }

    /*
     * Create primary
     */

    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof ddsd);
    ddsd.dwSize         = sizeof ddsd;
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    ComPtr<IDirectDrawSurface7> pddsPrimary;
    hr = pDD->CreateSurface(&ddsd, &pddsPrimary, NULL);
    if (FAILED(hr)) {
        return 1;
    }

    /*
     * Create backbuffer
     */

    ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

    RECT rcScreenRect;
    RECT rcViewportRect;
    GetClientRect(hWnd, &rcScreenRect);
    GetClientRect(hWnd, &rcViewportRect);
    ClientToScreen(hWnd, (POINT*)&rcScreenRect.left);
    ClientToScreen(hWnd, (POINT*)&rcScreenRect.right);
    ddsd.dwWidth  = rcScreenRect.right - rcScreenRect.left;
    ddsd.dwHeight = rcScreenRect.bottom - rcScreenRect.top;

    ComPtr<IDirectDrawSurface7> pddsBackBuffer;
    hr = pDD->CreateSurface(&ddsd, &pddsBackBuffer, NULL);
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<IDirectDrawClipper> pcClipper;
    hr = pDD->CreateClipper(0, &pcClipper, NULL);
    if (FAILED(hr)) {
        return 1;
    }

    pcClipper->SetHWnd(0, hWnd);
    pddsPrimary->SetClipper(pcClipper.Get());

    ComPtr<IDirect3D7> pD3D;
    hr = pDD->QueryInterface(IID_IDirect3D7, (void **)&pD3D);
    if (FAILED(hr)) {
        /* D3D7 is not supported on 64 bits processes. */
        fprintf(stderr, "error: failed to get IDirect3D7 interface.\n");
        return EXIT_SKIP;
    }

    ComPtr<IDirect3DDevice7> pDevice;
    // IID_IDirect3DTnLHalDevice is absent from RDP sessions, whereas
    // IID_IDirect3DRGBDevice seems always available.
    hr = pD3D->CreateDevice(IID_IDirect3DRGBDevice, pddsBackBuffer.Get(), &pDevice);
    if (FAILED(hr)) {
        return 1;
    }

    struct Vertex {
        float x, y, z;
        DWORD color;
    };

    D3DCOLOR clearColor = D3DRGBA(0.3f, 0.1f, 0.3f, 1.0f);
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
    pDevice->BeginScene();

    pDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

    static Vertex vertices[] = {
        { -0.9f, -0.9f, 0.5f, D3DRGBA(0.8f, 0.0f, 0.0f, 0.1f) },
        {  0.9f, -0.9f, 0.5f, D3DRGBA(0.0f, 0.9f, 0.0f, 0.1f) },
        {  0.0f,  0.9f, 0.5f, D3DRGBA(0.0f, 0.0f, 0.7f, 0.1f) },
    };

    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, vertices, 3, 0);

    pDevice->EndScene();

    /*
     * Present
     */
    hr = pddsPrimary->Blt(&rcScreenRect, pddsBackBuffer.Get(), &rcViewportRect, DDBLT_WAIT, NULL);
    if (FAILED(hr)) {
        return 1;
    }

    DestroyWindow(hWnd);

    return 0;
}


/**************************************************************************
 *
 * Copyright 2016 VMware, Inc
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

typedef IDirect3D9 * (WINAPI * PFNDIRECT3DCREATE9)(UINT SDKVersion);


static bool
load(void)
{
    bool result = false;
    HMODULE hD3D9 = LoadLibraryA("d3d9.dll");
    if (hD3D9) {
        PFNDIRECT3DCREATE9 pfnDirect3DCreate9 = (PFNDIRECT3DCREATE9)GetProcAddress(hD3D9, "Direct3DCreate9");
        if (pfnDirect3DCreate9) {
            ComPtr<IDirect3D9> pD3D;
            pD3D.Attach(pfnDirect3DCreate9(D3D_SDK_VERSION));
            if (pD3D) {
                result = true;
            }
        }
        FreeLibrary(hD3D9);
    }

    return result;
}


int
main(int argc, char *argv[])
{
    if (!load()) {
        return EXIT_FAILURE;
    }

    if (!load()) {
        return EXIT_FAILURE;
    }

    return 0;
}


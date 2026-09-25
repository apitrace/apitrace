/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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


/*
 * Test case for usage of D3D10_MAP_WRITE_DISCARD with D3D10_MAP_WRITE_NO_OVERWRITE
 * http://msdn.microsoft.com/en-us/library/windows/desktop/bb205318.aspx#NO_OVERWRITE_DETAILS
 *
 */
#include <stdio.h>
#include <stddef.h>

#include <initguid.h>
#include <windows.h>

#include "winsdk_compat.h"

#include <d3d10.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;


int
main(int argc, char *argv[])
{
    HRESULT hr;

    D3D10_USAGE Usage = D3D10_USAGE_DYNAMIC;
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "-dynamic") == 0) {
            Usage = D3D10_USAGE_DYNAMIC;
        } else if (strcmp(arg, "-staging") == 0) {
            Usage = D3D10_USAGE_STAGING;
        } else {
            fprintf(stderr, "error: unknown arg %s\n", arg);
            exit(1);
        }
    }

    ComPtr<IDXGIFactory> pFactory;
    hr = CreateDXGIFactory(IID_IDXGIFactory, (void**)&pFactory);
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<IDXGIAdapter> pAdapter;
    hr = pFactory->EnumAdapters(0, &pAdapter);
    if (FAILED(hr)) {
        return 1;
    }

    UINT Flags = 0;
    if (0) {
        Flags |= D3D10_CREATE_DEVICE_DEBUG;
    }

    ComPtr<ID3D10Device> pDevice;
    hr = D3D10CreateDevice(pAdapter.Get(),
                           D3D10_DRIVER_TYPE_HARDWARE,
                           nullptr,
                           Flags,
                           D3D10_SDK_VERSION,
                           &pDevice);
    if (FAILED(hr)) {
        return 1;
    }

    UINT NumBuffers = 4;
    UINT NumSegments = 8;
    UINT SegmentSize = 512;

    D3D10_BUFFER_DESC BufferDesc;
    ZeroMemory(&BufferDesc, sizeof BufferDesc);
    BufferDesc.ByteWidth = NumSegments * SegmentSize;
    BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags = 0;
    BufferDesc.Usage = Usage;
    if (Usage == D3D10_USAGE_STAGING) {
        BufferDesc.BindFlags = 0;
    } else {
        BufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    }

    D3D10_MAP MapType = D3D10_MAP_WRITE;
    for (UINT i = 0; i < NumBuffers; ++i) {
        ComPtr<ID3D10Buffer> pVertexBuffer;
        hr = pDevice->CreateBuffer(&BufferDesc, nullptr, &pVertexBuffer);
        if (FAILED(hr)) {
            return 1;
        }

        if (Usage == D3D10_USAGE_DYNAMIC) {
            MapType = D3D10_MAP_WRITE_DISCARD;
        }

        for (UINT j = 0; j < NumSegments; ++j) {
            BYTE *pMap = nullptr;
            hr = pVertexBuffer->Map(MapType, 0, (void **)&pMap);
            if (FAILED(hr)) {
                return 1;
            } 

            int c = (j % 255) + 1;
            memset(pMap + j*SegmentSize, c, SegmentSize);

            pVertexBuffer->Unmap();
            
            if (Usage == D3D10_USAGE_DYNAMIC) {
                MapType = D3D10_MAP_WRITE_NO_OVERWRITE;
            }
        }

        pDevice->Flush();

        D3D10_QUERY_DESC QueryDesc;
        QueryDesc.Query = D3D10_QUERY_EVENT;
        QueryDesc.MiscFlags = 0;
        ComPtr<ID3D10Query> pQuery;
        hr = pDevice->CreateQuery(&QueryDesc, &pQuery);
        if (FAILED(hr)) {
            return 1;
        } 
        
        pQuery->End();

        do {
            hr = pQuery->GetData(nullptr, 0, 0);
        } while (hr == S_FALSE);
        if (FAILED(hr)) {
            return 1;
        } 

    }

    return 0;
}


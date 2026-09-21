/**************************************************************************
 *
 * Copyright 2026 Broadcom
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

#include <iostream>

#include <d3d9.h>

#ifndef __MINGW32__
#include <d3d9on12.h>
#endif

#include "d3dretrace.hpp"
#include "d3dretrace_d3d9on12.hpp"


namespace d3dretrace {
namespace d3d9on12 {


#ifdef __MINGW32__
typedef struct D3D9ON12_ARGS {
    BOOL Enable9On12;
    IUnknown *pD3D12Device;
    IUnknown *ppD3D12Queues[2];
    UINT NumQueues;
    UINT NodeMask;
} D3D9ON12_ARGS;
#endif


typedef IDirect3D9 *
(WINAPI *PFN_Direct3DCreate9On12)(UINT SDKVersion, D3D9ON12_ARGS *pOverrideList, UINT NumOverrideEntries);

typedef HRESULT
(WINAPI *PFN_Direct3DCreate9On12Ex)(UINT SDKVersion, D3D9ON12_ARGS *pOverrideList, UINT NumOverrideEntries, IDirect3D9Ex **ppOutputInterface);


template< typename PFN >
static PFN
loadProcAddress(const char *szModule, const char *szProc)
{
    HMODULE hModule = LoadLibraryA(szModule);
    if (!hModule) {
        std::cerr << "error: failed to load " << szModule << " for --driver=d3d9on12\n";
        return nullptr;
    }
    PFN pfn = reinterpret_cast<PFN>(GetProcAddress(hModule, szProc));
    if (!pfn) {
        std::cerr << "error: failed to resolve " << szProc << " from " << szModule << " for --driver=d3d9on12\n";
    }
    return pfn;
}


IDirect3D9 *
createDirect3D9(UINT SDKVersion)
{
    static PFN_Direct3DCreate9On12 pfn = loadProcAddress<PFN_Direct3DCreate9On12>("d3d9.dll", "Direct3DCreate9On12");
    if (!pfn) {
        return nullptr;
    }

    D3D9ON12_ARGS args = {};
    args.Enable9On12 = TRUE;

    IDirect3D9 *pD3D9 = pfn(SDKVersion, &args, 1);
    if (pD3D9) {
        std::cerr << "info: --driver=d3d9on12: created device via Direct3DCreate9On12\n";
    }
    return pD3D9;
}


HRESULT
createDirect3D9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
    static PFN_Direct3DCreate9On12Ex pfn = loadProcAddress<PFN_Direct3DCreate9On12Ex>("d3d9.dll", "Direct3DCreate9On12Ex");
    if (!pfn) {
        return E_FAIL;
    }

    D3D9ON12_ARGS args = {};
    args.Enable9On12 = TRUE;

    HRESULT hr = pfn(SDKVersion, &args, 1, ppD3D);
    if (SUCCEEDED(hr)) {
        std::cerr << "info: --driver=d3d9on12: created device via Direct3DCreate9On12Ex\n";
    }
    return hr;
}


} /* namespace d3d9on12 */
} /* namespace d3dretrace */

/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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
 * Stubs for internal DXGI functions.
 */


#include <assert.h>

#include "d3d10imports.hpp"

#include "os.hpp"


static HMODULE g_hDXGIModule = NULL;


static PROC
_getDXGIProcAddress(LPCSTR lpProcName) {
    if (!g_hDXGIModule) {
        char szDll[MAX_PATH] = {0};
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        strcat(szDll, "\\\\dxgi.dll");
        g_hDXGIModule = LoadLibraryA(szDll);
        if (!g_hDXGIModule) {
           return NULL;
        }
    }
    return GetProcAddress(g_hDXGIModule, lpProcName);
}


#define STUB(_Ret, _Name, _ArgsDecl, _Args) \
    EXTERN_C _Ret WINAPI \
    _Name _ArgsDecl { \
        typedef _Ret (WINAPI *_PFN) _ArgsDecl; \
        static _PFN _pfn = NULL; \
        if (!_pfn) { \
            static const char *_name = #_Name; \
            _pfn = (_PFN)_getDXGIProcAddress(_name); \
            if (!_pfn) { \
                os::log("error: unavailable function %s\n", _name); \
                os::abort(); \
            } \
        } \
        return _pfn _Args; \
    }


STUB(HRESULT, DXGID3D10CreateDevice,
     (HMODULE hModule, IDXGIFactory *pFactory, IDXGIAdapter *pAdapter, UINT Flags, void *unknown, void *ppDevice),
     (hModule, pFactory, pAdapter, Flags, unknown, ppDevice)
)

struct UNKNOWN {
    BYTE unknown[20];
};

STUB(HRESULT, DXGID3D10CreateLayeredDevice,
    (UNKNOWN Unknown),
    (Unknown)
)

STUB(SIZE_T, DXGID3D10GetLayeredDeviceSize,
    (const void *pLayers, UINT NumLayers),
    (pLayers, NumLayers)
)

STUB(HRESULT, DXGID3D10RegisterLayers,
    (const void *pLayers, UINT NumLayers),
    (pLayers, NumLayers)
)

EXTERN_C HRESULT WINAPI
DXGIDumpJournal()
{
    assert(0);
    return E_NOTIMPL;
}

EXTERN_C HRESULT WINAPI
DXGIReportAdapterConfiguration()
{
    assert(0);
    return E_NOTIMPL;
}



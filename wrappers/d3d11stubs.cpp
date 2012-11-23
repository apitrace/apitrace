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
 *Stubs for internal D3D11 functions.
 */


#include "os.hpp"

#include "d3d11imports.hpp"


static HMODULE g_hD3D11Module = NULL;


static PROC
_getD3D11ProcAddress(LPCSTR lpProcName) {
    if (!g_hD3D11Module) {
        char szDll[MAX_PATH] = {0};
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        strcat(szDll, "\\\\d3d11.dll");
        g_hD3D11Module = LoadLibraryA(szDll);
        if (!g_hD3D11Module) {
            return NULL;
        }
    }
    return GetProcAddress(g_hD3D11Module, lpProcName);
}


typedef HRESULT (WINAPI *PFN_D3D11COREREGISTERLAYERS)(const void *unknown0, DWORD unknown1);
static PFN_D3D11COREREGISTERLAYERS _D3D11CoreRegisterLayers_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D11CoreRegisterLayers(const void *unknown0, DWORD unknown1) {
    const char *_name = "D3D11CoreRegisterLayers";
    if (!_D3D11CoreRegisterLayers_ptr) {
        _D3D11CoreRegisterLayers_ptr = (PFN_D3D11COREREGISTERLAYERS)_getD3D11ProcAddress(_name);
        if (!_D3D11CoreRegisterLayers_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D11CoreRegisterLayers_ptr(unknown0, unknown1);
}

typedef SIZE_T (WINAPI *PFN_D3D11COREGETLAYEREDDEVICESIZE)(const void *unknown0, DWORD unknown1);
static PFN_D3D11COREGETLAYEREDDEVICESIZE _D3D11CoreGetLayeredDeviceSize_ptr = NULL;

EXTERN_C SIZE_T WINAPI
D3D11CoreGetLayeredDeviceSize(const void *unknown0, DWORD unknown1) {
    const char *_name = "D3D11CoreGetLayeredDeviceSize";
    if (!_D3D11CoreGetLayeredDeviceSize_ptr) {
        _D3D11CoreGetLayeredDeviceSize_ptr = (PFN_D3D11COREGETLAYEREDDEVICESIZE)_getD3D11ProcAddress(_name);
        if (!_D3D11CoreGetLayeredDeviceSize_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D11CoreGetLayeredDeviceSize_ptr(unknown0, unknown1);
}

typedef HRESULT (WINAPI *PFN_D3D11CORECREATELAYEREDDEVICE)(const void *unknown0, DWORD unknown1, const void *unknown2, REFIID riid, void **ppvObj);
static PFN_D3D11CORECREATELAYEREDDEVICE _D3D11CoreCreateLayeredDevice_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D11CoreCreateLayeredDevice(const void *unknown0, DWORD unknown1, const void *unknown2, REFIID riid, void **ppvObj) {
    const char *_name = "D3D11CoreCreateLayeredDevice";
    if (!_D3D11CoreCreateLayeredDevice_ptr) {
        _D3D11CoreCreateLayeredDevice_ptr = (PFN_D3D11CORECREATELAYEREDDEVICE)_getD3D11ProcAddress(_name);
        if (!_D3D11CoreCreateLayeredDevice_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D11CoreCreateLayeredDevice_ptr(unknown0, unknown1, unknown2, riid, ppvObj);
}

typedef HRESULT (WINAPI *PFN_D3D11CORECREATEDEVICE)(DWORD unknown0, DWORD unknown1, DWORD unknown2, DWORD unknown3, DWORD unknown4, DWORD unknown5, DWORD unknown6, DWORD unknown7, DWORD unknown8);
static PFN_D3D11CORECREATEDEVICE _D3D11CoreCreateDevice_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D11CoreCreateDevice(DWORD unknown0, DWORD unknown1, DWORD unknown2, DWORD unknown3, DWORD unknown4, DWORD unknown5, DWORD unknown6, DWORD unknown7, DWORD unknown8) {
    const char *_name = "D3D11CoreCreateDevice";
    if (!_D3D11CoreCreateDevice_ptr) {
        _D3D11CoreCreateDevice_ptr = (PFN_D3D11CORECREATEDEVICE)_getD3D11ProcAddress(_name);
        if (!_D3D11CoreCreateDevice_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D11CoreCreateDevice_ptr(unknown0, unknown1, unknown2, unknown3, unknown4, unknown5, unknown6, unknown7, unknown8);
}



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
 * Direct3D error codes.
 *
 * These were defined in d3dx10.h/d3dx11.h on DirectX SDK, and moved to
 * winerror.h on Windows 8 SDK.
 *
 * See also:
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/ff476174.aspx
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/bb205278.aspx
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/bb172554.aspx
 */

#pragma once


#include <windows.h>


#ifndef _FACD3D
#define _FACD3D 0x876
#endif

#ifndef MAKE_D3DHRESULT
#define MAKE_D3DHRESULT(code) MAKE_HRESULT(1, _FACD3D, code)
#endif

#ifndef MAKE_D3DSTATUS
#define MAKE_D3DSTATUS(code) MAKE_HRESULT(0, _FACD3D, code)
#endif

#ifndef D3DERR_INVALIDCALL
#define D3DERR_INVALIDCALL MAKE_D3DHRESULT(2156)
#endif

#ifndef D3DERR_WASSTILLDRAWING
#define D3DERR_WASSTILLDRAWING MAKE_D3DHRESULT(540)
#endif


#ifndef DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED
#define DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED MAKE_DXGI_HRESULT(35)
#endif

#ifndef DXGI_ERROR_REMOTE_OUTOFMEMORY
#define DXGI_ERROR_REMOTE_OUTOFMEMORY MAKE_DXGI_HRESULT(36)
#endif


#ifndef D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS
#define D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS MAKE_D3D10_HRESULT(1)
#endif

#ifndef D3D10_ERROR_FILE_NOT_FOUND
#define D3D10_ERROR_FILE_NOT_FOUND MAKE_D3D10_HRESULT(2)
#endif


#ifndef D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS
#define D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS MAKE_D3D11_HRESULT(1)
#endif

#ifndef D3D11_ERROR_FILE_NOT_FOUND
#define D3D11_ERROR_FILE_NOT_FOUND MAKE_D3D11_HRESULT(2)
#endif

#ifndef D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS
#define D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS MAKE_D3D11_HRESULT(3)
#endif

#ifndef D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD
#define D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD MAKE_D3D11_HRESULT(4)
#endif



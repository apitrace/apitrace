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

#pragma once


#include "retrace.hpp"
#include "d3dretrace.hpp"


/*
 * This module implements d3d9on12 driver type, which replays a D3D9 trace
 * via the D3D9On12 layer by intercepting and replacing Direct3DCreate9 /
 * Direct3DCreate9Ex with Direct3DCreate9On12 / Direct3DCreate9On12Ex.
 *
 * Unlike D3D11On12, D3D9On12 doesn't expose any D3D12 objects (command
 * queues, swap chains, etc.) to the caller: the IDirect3D9/IDirect3D9Ex
 * interfaces it returns behave as regular D3D9 interfaces, so no swap chain
 * wrapping is required beyond substituting the creation entry points.
 */


struct IDirect3D9;
struct IDirect3D9Ex;


namespace d3dretrace {
namespace d3d9on12 {

IDirect3D9 *
createDirect3D9(UINT SDKVersion);

HRESULT
createDirect3D9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D);

} /* namespace d3d9on12 */
} /* namespace d3dretrace */

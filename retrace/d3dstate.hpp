/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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


#include <iostream>

#include <windows.h>

#ifdef HAVE_DXGI
#include <dxgiformat.h>
#endif


struct IDirect3DDevice7;
struct IDirect3DDevice8;
struct IDirect3DDevice9;
struct IDirect3DSwapChain9;
struct IDXGISwapChain;
struct ID3D10Device;
struct ID3D10Resource;
struct ID3D11DeviceContext;
struct ID3D11Resource;


class StateWriter;

namespace image {
    class Image;
}


namespace d3dstate {


extern const GUID GUID_D3DSTATE;


/*
 * D3D7
 */

image::Image *
getRenderTargetImage(IDirect3DDevice7 *pDevice);

void
dumpFramebuffer(StateWriter &writer, IDirect3DDevice7 *pDevice);

void
dumpDevice(StateWriter &writer, IDirect3DDevice7 *pDevice);


/*
 * D3D8
 */

image::Image *
getRenderTargetImage(IDirect3DDevice8 *pDevice);

void
dumpFramebuffer(StateWriter &writer, IDirect3DDevice8 *pDevice);

void
dumpDevice(StateWriter &writer, IDirect3DDevice8 *pDevice);


/*
 * D3D9
 */

image::Image *
getRenderTargetImage(IDirect3DSwapChain9 *pSwapChain);

void
dumpTextures(StateWriter &writer, IDirect3DDevice9 *pDevice);

void
dumpFramebuffer(StateWriter &writer, IDirect3DDevice9 *pDevice);

void
dumpDevice(StateWriter &writer, IDirect3DDevice9 *pDevice);

image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice);

void
dumpDevice(StateWriter &writer, IDirect3DSwapChain9 *pSwapChain);


#ifdef HAVE_DXGI


/*
 * DXGI
 */

image::Image *
getRenderTargetImage(IDXGISwapChain *pSwapChain);

void
dumpDevice(StateWriter &writer, IDXGISwapChain *pSwapChain);


/*
 * D3D10
 */

image::Image *
getRenderTargetImage(ID3D10Device *pDevice,
                     DXGI_FORMAT *dxgiFormat = 0);

void
dumpTextures(StateWriter &writer, ID3D10Device *pDevice);

void
dumpFramebuffer(StateWriter &writer, ID3D10Device *pDevice);

void
dumpDevice(StateWriter &writer, ID3D10Device *pDevice);


/*
 * D3D11
 */

image::Image *
getRenderTargetImage(ID3D11DeviceContext *pDeviceContext,
                     DXGI_FORMAT *dxgiFormat = 0);

void
dumpTextures(StateWriter &writer, ID3D11DeviceContext *pDeviceContext);

void
dumpFramebuffer(StateWriter &writer, ID3D11DeviceContext *pDeviceContext);

void
dumpDevice(StateWriter &writer, ID3D11DeviceContext *pDeviceContext);


#endif /* HAVE_DXGI */


} /* namespace d3dstate */



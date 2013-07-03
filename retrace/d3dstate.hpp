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

#ifndef _D3DSTATE_HPP_
#define _D3DSTATE_HPP_


#include <iostream>

#include <windows.h>


struct IDirect3DDevice8;
struct IDirect3DDevice9;
struct ID3D10Device;
struct ID3D11DeviceContext;


class JSONWriter;

namespace image {
    class Image;
}


namespace d3dstate {


extern const GUID GUID_D3DSTATE;


image::Image *
getRenderTargetImage(IDirect3DDevice8 *pDevice);

void
dumpFramebuffer(JSONWriter &json, IDirect3DDevice8 *pDevice);

void
dumpDevice(std::ostream &os, IDirect3DDevice8 *pDevice);


image::Image *
getRenderTargetImage(IDirect3DDevice9 *pDevice);

void
dumpFramebuffer(JSONWriter &json, IDirect3DDevice9 *pDevice);

void
dumpDevice(std::ostream &os, IDirect3DDevice9 *pDevice);


image::Image *
getRenderTargetImage(ID3D10Device *pDevice);

void
dumpTextures(JSONWriter &json, ID3D10Device *pDevice);

void
dumpFramebuffer(JSONWriter &json, ID3D10Device *pDevice);

void
dumpDevice(std::ostream &os, ID3D10Device *pDevice);


image::Image *
getRenderTargetImage(ID3D11DeviceContext *pDeviceContext);

void
dumpTextures(JSONWriter &json, ID3D11DeviceContext *pDeviceContext);

void
dumpFramebuffer(JSONWriter &json, ID3D11DeviceContext *pDeviceContext);

void
dumpDevice(std::ostream &os, ID3D11DeviceContext *pDeviceContext);


} /* namespace d3dstate */


#endif /* _D3DSTATE_HPP_ */

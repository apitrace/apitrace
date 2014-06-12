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


#include <stdio.h>

#include <iostream>

#include "d3d10imports.hpp"
#include "d3d10state.hpp"


namespace d3dstate {


const GUID
GUID_D3DSTATE = {0x7D71CAC9,0x7F58,0x432C,{0xA9,0x75,0xA1,0x9F,0xCF,0xCE,0xFD,0x14}};


static void
dumpShaders(JSONWriter &json, ID3D10Device *pDevice)
{
    json.beginMember("shaders");
    json.beginObject();

    ID3D10VertexShader *pVertexShader = NULL;
    pDevice->VSGetShader(&pVertexShader);
    if (pVertexShader) {
        dumpShader<ID3D10DeviceChild>(json, "VS", pVertexShader);
        pVertexShader->Release();
    }

    ID3D10GeometryShader *pGeometryShader = NULL;
    pDevice->GSGetShader(&pGeometryShader);
    if (pGeometryShader) {
        dumpShader<ID3D10DeviceChild>(json, "GS", pGeometryShader);
        pGeometryShader->Release();
    }

    ID3D10PixelShader *pPixelShader = NULL;
    pDevice->PSGetShader(&pPixelShader);
    if (pPixelShader) {
        dumpShader<ID3D10DeviceChild>(json, "PS", pPixelShader);
    }

    json.endObject();
    json.endMember(); // shaders
}


void
dumpDevice(std::ostream &os, ID3D10Device *pDevice)
{
    JSONWriter json(os);

    /* TODO */
    json.beginMember("parameters");
    json.beginObject();
    json.endObject();
    json.endMember(); // parameters

    dumpShaders(json, pDevice);

    dumpTextures(json, pDevice);

    dumpFramebuffer(json, pDevice);
}


} /* namespace d3dstate */

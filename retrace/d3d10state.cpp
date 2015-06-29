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

#include "com_ptr.hpp"
#include "d3d10imports.hpp"
#include "d3d10state.hpp"
#include "dxgistate_so.hpp"


namespace d3dstate {


const GUID
GUID_D3DSTATE = {0x7D71CAC9,0x7F58,0x432C,{0xA9,0x75,0xA1,0x9F,0xCF,0xCE,0xFD,0x14}};


static void
dumpRasterizerState(StateWriter &writer, ID3D10Device *pDevice)
{
    com_ptr<ID3D10RasterizerState> pRasterizerState;

    pDevice->RSGetState(&pRasterizerState);
    writer.beginMember("RasterizerState");
    dumpStateObjectDesc(writer, pRasterizerState);
    writer.endMember(); // RasterizerState
}


static void
dumpBlendState(StateWriter &writer, ID3D10Device *pDevice)
{
    com_ptr<ID3D10BlendState> pBlendState;
    FLOAT BlendFactor[4];
    UINT SampleMask;

    pDevice->OMGetBlendState(&pBlendState, BlendFactor, &SampleMask);

    writer.beginMember("BlendState");
    dumpStateObjectDesc(writer, pBlendState);
    writer.endMember(); // BlendState
    
    writer.beginMember("BlendFactor");
    writer.beginArray();
    writer.writeFloat(BlendFactor[0]);
    writer.writeFloat(BlendFactor[1]);
    writer.writeFloat(BlendFactor[2]);
    writer.writeFloat(BlendFactor[3]);
    writer.endArray();
    writer.endMember(); // BlendFactor

    writer.writeIntMember("SampleMask", SampleMask);
}


static void
dumpDepthStencilState(StateWriter &writer, ID3D10Device *pDevice)
{
    com_ptr<ID3D10DepthStencilState> pDepthStencilState;
    UINT stencilRef;

    pDevice->OMGetDepthStencilState(&pDepthStencilState, &stencilRef);
    writer.beginMember("DepthStencilState");
    dumpStateObjectDesc(writer, pDepthStencilState);
    writer.endMember(); // DepthStencilState
    writer.writeIntMember("StencilRef", stencilRef);
}


static void
dumpViewports(StateWriter &writer, ID3D10Device *pDevice)
{
   D3D10_VIEWPORT vps[D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
   UINT numViewports = 0, i;

   pDevice->RSGetViewports(&numViewports, NULL);
   pDevice->RSGetViewports(&numViewports, vps);

   writer.beginMember("Viewports");
   writer.beginArray();
   for (i = 0; i < numViewports; ++i) {
      dumpStateObject(writer, vps[i]);
   }
   writer.endArray();
   writer.endMember();
}


static void
dumpScissors(StateWriter &writer, ID3D10Device *pDevice)
{
   D3D10_RECT rects[D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
   UINT numRects = 0, i;

   pDevice->RSGetScissorRects(&numRects, NULL);
   pDevice->RSGetScissorRects(&numRects, rects);

   writer.beginMember("Scissors");
   writer.beginArray();
   for (i = 0; i < numRects; ++i) {
      dumpStateObject(writer, rects[i]);
   }
   writer.endArray();
   writer.endMember();
}


static void
dumpParameters(StateWriter &writer, ID3D10Device *pDevice)
{
    // TODO: dump description of current bound state
    writer.beginMember("parameters");
    writer.beginObject();

    dumpRasterizerState(writer, pDevice);
    dumpBlendState(writer, pDevice);
    dumpDepthStencilState(writer, pDevice);
    dumpViewports(writer, pDevice);
    dumpScissors(writer, pDevice);

    writer.endObject();
    writer.endMember(); // parameters
}


static void
dumpShaders(StateWriter &writer, ID3D10Device *pDevice)
{
    writer.beginMember("shaders");
    writer.beginObject();

    com_ptr<ID3D10VertexShader> pVertexShader;
    pDevice->VSGetShader(&pVertexShader);
    if (pVertexShader) {
        dumpShader<ID3D10DeviceChild>(writer, "VS", pVertexShader);
    }

    com_ptr<ID3D10GeometryShader> pGeometryShader;
    pDevice->GSGetShader(&pGeometryShader);
    if (pGeometryShader) {
        dumpShader<ID3D10DeviceChild>(writer, "GS", pGeometryShader);
    }

    com_ptr<ID3D10PixelShader> pPixelShader;
    pDevice->PSGetShader(&pPixelShader);
    if (pPixelShader) {
        dumpShader<ID3D10DeviceChild>(writer, "PS", pPixelShader);
    }

    writer.endObject();
    writer.endMember(); // shaders
}


void
dumpDevice(StateWriter &writer, ID3D10Device *pDevice)
{
    dumpParameters(writer, pDevice);

    dumpShaders(writer, pDevice);

    dumpTextures(writer, pDevice);

    dumpFramebuffer(writer, pDevice);
}


} /* namespace d3dstate */

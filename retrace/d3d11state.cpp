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
#include <type_traits>

#include "com_ptr.hpp"
#include "d3d11imports.hpp"
#include "d3d10state.hpp"
#include "dxgistate_so.hpp"
#include "dxgistate.hpp"


namespace d3dstate {


// Ensure ExtractDescType template is sane
static_assert( std::is_same< ExtractDescType< decltype( &ID3D11BlendState::GetDesc ) >::type, D3D11_BLEND_DESC >::value, "type mismatch" );


static void
dumpRasterizerState(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    com_ptr<ID3D11RasterizerState> pRasterizerState;

    pDeviceContext->RSGetState(&pRasterizerState);
    writer.beginMember("RasterizerState");
    dumpStateObjectDesc(writer, pRasterizerState);
    writer.endMember(); // RasterizerState
}


static void
dumpBlendState(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    com_ptr<ID3D11BlendState> pBlendState;
    FLOAT BlendFactor[4];
    UINT SampleMask;

    pDeviceContext->OMGetBlendState(&pBlendState, BlendFactor, &SampleMask);

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
dumpDepthStencilState(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    com_ptr<ID3D11DepthStencilState> pDepthStencilState;
    UINT stencilRef;

    pDeviceContext->OMGetDepthStencilState(&pDepthStencilState, &stencilRef);
    writer.beginMember("DepthStencilState");
    dumpStateObjectDesc(writer, pDepthStencilState);
    writer.endMember(); // DepthStencilState
    writer.writeIntMember("StencilRef", stencilRef);
}


static void
dumpViewports(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
   D3D11_VIEWPORT vps[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
   UINT numViewports = 0, i;

   pDeviceContext->RSGetViewports(&numViewports, NULL);
   pDeviceContext->RSGetViewports(&numViewports, vps);

   writer.beginMember("Viewports");
   writer.beginArray();
   for (i = 0; i < numViewports; ++i) {
      dumpStateObject(writer, vps[i]);
   }
   writer.endArray();
   writer.endMember();
}


static void
dumpScissors(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
   D3D11_RECT rects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
   UINT numRects = 0, i;

   pDeviceContext->RSGetScissorRects(&numRects, NULL);
   pDeviceContext->RSGetScissorRects(&numRects, rects);

   writer.beginMember("Scissors");
   writer.beginArray();
   for (i = 0; i < numRects; ++i) {
      dumpStateObject(writer, rects[i]);
   }
   writer.endArray();
   writer.endMember();
}


static void
dumpParameters(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    // TODO: dump description of current bound state
    writer.beginMember("parameters");
    writer.beginObject();

    D3D11_DEVICE_CONTEXT_TYPE Type = pDeviceContext->GetType();
    writer.beginMember("Type");
    dumpStateObject(writer, Type);
    writer.endMember(); // Type

    dumpRasterizerState(writer, pDeviceContext);
    dumpBlendState(writer, pDeviceContext);
    dumpDepthStencilState(writer, pDeviceContext);
    dumpViewports(writer, pDeviceContext);
    dumpScissors(writer, pDeviceContext);

    writer.endObject();
    writer.endMember(); // parameters
}


static void
dumpShaders(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    writer.beginMember("shaders");
    writer.beginObject();

    com_ptr<ID3D11VertexShader> pVertexShader;
    pDeviceContext->VSGetShader(&pVertexShader, NULL, NULL);
    if (pVertexShader) {
        dumpShader<ID3D11DeviceChild>(writer, "VS", pVertexShader);
    }

    com_ptr<ID3D11HullShader> pHullShader;
    pDeviceContext->HSGetShader(&pHullShader, NULL, NULL);
    if (pHullShader) {
        dumpShader<ID3D11DeviceChild>(writer, "HS", pHullShader);
    }

    com_ptr<ID3D11DomainShader> pDomainShader;
    pDeviceContext->DSGetShader(&pDomainShader, NULL, NULL);
    if (pDomainShader) {
        dumpShader<ID3D11DeviceChild>(writer, "DS", pDomainShader);
    }

    com_ptr<ID3D11GeometryShader> pGeometryShader;
    pDeviceContext->GSGetShader(&pGeometryShader, NULL, NULL);
    if (pGeometryShader) {
        dumpShader<ID3D11DeviceChild>(writer, "GS", pGeometryShader);
    }

    com_ptr<ID3D11PixelShader> pPixelShader;
    pDeviceContext->PSGetShader(&pPixelShader, NULL, NULL);
    if (pPixelShader) {
        dumpShader<ID3D11DeviceChild>(writer, "PS", pPixelShader);
    }

    com_ptr<ID3D11ComputeShader> pComputeShader;
    pDeviceContext->CSGetShader(&pComputeShader, NULL, NULL);
    if (pComputeShader) {
        dumpShader<ID3D11DeviceChild>(writer, "CS", pComputeShader);
    }

    writer.endObject();
    writer.endMember(); // shaders
}


static void
dumpConstantBuffers(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    writer.beginMember("uniforms");
    writer.beginObject();
    if (pDeviceContext->GetType() != D3D11_DEVICE_CONTEXT_DEFERRED) {
        ID3D11Buffer *pConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

        pDeviceContext->VSGetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);
        dumpBuffers(writer, "VS", ARRAYSIZE(pConstantBuffers), pConstantBuffers);

        pDeviceContext->HSGetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);
        dumpBuffers(writer, "HS", ARRAYSIZE(pConstantBuffers), pConstantBuffers);

        pDeviceContext->DSGetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);
        dumpBuffers(writer, "DS", ARRAYSIZE(pConstantBuffers), pConstantBuffers);

        pDeviceContext->GSGetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);
        dumpBuffers(writer, "GS", ARRAYSIZE(pConstantBuffers), pConstantBuffers);

        pDeviceContext->PSGetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);
        dumpBuffers(writer, "PS", ARRAYSIZE(pConstantBuffers), pConstantBuffers);

    }
    writer.endObject();
    writer.endMember();
}


static void
dumpBuffers(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    writer.beginMember("buffers");
    writer.beginObject();

    UINT numBuffers = D3D11_SO_BUFFER_SLOT_COUNT;
    ID3D11Buffer *soBuffers[D3D11_SO_BUFFER_SLOT_COUNT];
    pDeviceContext->SOGetTargets(numBuffers, soBuffers);
    dumpBuffers(writer, "SO", ARRAYSIZE(soBuffers), soBuffers);

    ID3D11Buffer *vertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    pDeviceContext->IAGetVertexBuffers(0, ARRAYSIZE(vertexBuffers),
                                       vertexBuffers, strides, offsets);
    bool hasVBs = false;
    for (UINT i = 0; i < ARRAYSIZE(vertexBuffers); ++i) {
        hasVBs = hasVBs || (vertexBuffers[i] != 0);
    }
    if (hasVBs) {
        writer.beginMember("Vertex Buffers");
        writer.beginArray();
        for (UINT i = 0; i < ARRAYSIZE(vertexBuffers); ++i) {
            char label[64];
            std::string name;

            if (vertexBuffers[i]) {
                name = getObjectName(vertexBuffers[i]);
                _snprintf(label, sizeof label, "%s, stride: %d, offset: %d",
                          name.c_str(), strides[i], offsets[i]);
                vertexBuffers[i]->Release();
            } else {
                _snprintf(label, sizeof label, "null");
            }
            writer.writeString(label);
        }
        writer.endArray();
        writer.endMember();
    }

    ID3D11Buffer *indexBuffer = NULL;
    DXGI_FORMAT iaFormat;
    UINT iaOffset;
    pDeviceContext->IAGetIndexBuffer(&indexBuffer, &iaFormat, &iaOffset);
    if (indexBuffer) {
        char label[64];
        std::string formatName = d3dstate::getDXGIFormatName(iaFormat);
        std::string name = getObjectName(indexBuffer);
        _snprintf(label, sizeof label, "%s, format: %s, offset: %d",
                  name.c_str(), formatName.c_str(), iaOffset);
        writer.writeStringMember("Index Buffer", label);
        indexBuffer->Release();
    }
    writer.endObject();
    writer.endMember();
}

void
dumpDevice(StateWriter &writer, ID3D11DeviceContext *pDeviceContext)
{
    dumpParameters(writer, pDeviceContext);

    dumpShaders(writer, pDeviceContext);

    dumpConstantBuffers(writer, pDeviceContext);

    dumpBuffers(writer, pDeviceContext);

    dumpTextures(writer, pDeviceContext);

    dumpFramebuffer(writer, pDeviceContext);
}


} /* namespace d3dstate */

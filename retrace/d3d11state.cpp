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


namespace d3dstate {


// Ensure ExtractDescType template is sane
static_assert( std::is_same< ExtractDescType< decltype( &ID3D11BlendState::GetDesc ) >::type, D3D11_BLEND_DESC >::value, "type mismatch" );


static void
dumpRasterizerState(JSONWriter &json, ID3D11DeviceContext *pDeviceContext)
{
    com_ptr<ID3D11RasterizerState> pRasterizerState;

    pDeviceContext->RSGetState(&pRasterizerState);
    json.beginMember("RasterizerState");
    dumpStateObjectDesc(json, pRasterizerState);
    json.endMember(); // RasterizerState
}


static void
dumpBlendState(JSONWriter &json, ID3D11DeviceContext *pDeviceContext)
{
    com_ptr<ID3D11BlendState> pBlendState;
    FLOAT BlendFactor[4];
    UINT SampleMask;

    pDeviceContext->OMGetBlendState(&pBlendState, BlendFactor, &SampleMask);

    json.beginMember("BlendState");
    dumpStateObjectDesc(json, pBlendState);
    json.endMember(); // BlendState
    
    json.beginMember("BlendFactor");
    json.beginArray();
    json.writeFloat(BlendFactor[0]);
    json.writeFloat(BlendFactor[1]);
    json.writeFloat(BlendFactor[2]);
    json.writeFloat(BlendFactor[3]);
    json.endArray();
    json.endMember(); // BlendFactor

    json.writeIntMember("SampleMask", SampleMask);
}


static void
dumpDepthStencilState(JSONWriter &json, ID3D11DeviceContext *pDeviceContext)
{
    com_ptr<ID3D11DepthStencilState> pDepthStencilState;
    UINT stencilRef;

    pDeviceContext->OMGetDepthStencilState(&pDepthStencilState, &stencilRef);
    json.beginMember("DepthStencilState");
    dumpStateObjectDesc(json, pDepthStencilState);
    json.endMember(); // DepthStencilState
    json.writeIntMember("StencilRef", stencilRef);
}


static void
dumpParameters(JSONWriter &json, ID3D11DeviceContext *pDeviceContext)
{
    // TODO: dump description of current bound state
    json.beginMember("parameters");
    json.beginObject();

    dumpRasterizerState(json, pDeviceContext);
    dumpBlendState(json, pDeviceContext);
    dumpDepthStencilState(json, pDeviceContext);

    json.endObject();
    json.endMember(); // parameters
}


static void
dumpShaders(JSONWriter &json, ID3D11DeviceContext *pDeviceContext)
{
    json.beginMember("shaders");
    json.beginObject();

    com_ptr<ID3D11VertexShader> pVertexShader;
    pDeviceContext->VSGetShader(&pVertexShader, NULL, NULL);
    if (pVertexShader) {
        dumpShader<ID3D11DeviceChild>(json, "VS", pVertexShader);
    }

    com_ptr<ID3D11HullShader> pHullShader;
    pDeviceContext->HSGetShader(&pHullShader, NULL, NULL);
    if (pHullShader) {
        dumpShader<ID3D11DeviceChild>(json, "HS", pHullShader);
    }

    com_ptr<ID3D11DomainShader> pDomainShader;
    pDeviceContext->DSGetShader(&pDomainShader, NULL, NULL);
    if (pDomainShader) {
        dumpShader<ID3D11DeviceChild>(json, "DS", pDomainShader);
    }

    com_ptr<ID3D11GeometryShader> pGeometryShader;
    pDeviceContext->GSGetShader(&pGeometryShader, NULL, NULL);
    if (pGeometryShader) {
        dumpShader<ID3D11DeviceChild>(json, "GS", pGeometryShader);
    }

    com_ptr<ID3D11PixelShader> pPixelShader;
    pDeviceContext->PSGetShader(&pPixelShader, NULL, NULL);
    if (pPixelShader) {
        dumpShader<ID3D11DeviceChild>(json, "PS", pPixelShader);
    }

    json.endObject();
    json.endMember(); // shaders
}


void
dumpDevice(std::ostream &os, ID3D11DeviceContext *pDeviceContext)
{
    JSONWriter json(os);

    dumpParameters(json, pDeviceContext);

    dumpShaders(json, pDeviceContext);

    // TODO: dump constant buffers
    json.beginMember("uniforms");
    json.beginObject();
    json.endObject();
    json.endMember(); // uniforms

    // TODO: dump stream-out buffer, vertex buffer
    json.beginMember("buffers");
    json.beginObject();
    json.endObject();
    json.endMember(); // buffers

    dumpTextures(json, pDeviceContext);

    dumpFramebuffer(json, pDeviceContext);
}


} /* namespace d3dstate */

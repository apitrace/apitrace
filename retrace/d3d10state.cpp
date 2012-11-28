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

#include "d3d11imports.hpp"
#include "json.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


const GUID
GUID_D3DSTATE = {0x7D71CAC9,0x7F58,0x432C,{0xA9,0x75,0xA1,0x9F,0xCF,0xCE,0xFD,0x14}};


template< class T >
inline void
dumpShader(JSONWriter &json, const char *name, T *pShader) {
    if (!pShader) {
        return;
    }

    HRESULT hr;

    /*
     * There is no method to get the shader byte code, so the creator is supposed to
     * attach it via the SetPrivateData method.
     */
    UINT BytecodeLength = 0;
    char dummy;
    hr = pShader->GetPrivateData(GUID_D3DSTATE, &BytecodeLength, &dummy);
    if (hr != DXGI_ERROR_MORE_DATA) {
        return;
    }

    void *pShaderBytecode = malloc(BytecodeLength);
    if (!pShaderBytecode) {
        return;
    }

    hr = pShader->GetPrivateData(GUID_D3DSTATE, &BytecodeLength, pShaderBytecode);
    if (SUCCEEDED(hr)) {
        IDisassemblyBuffer *pDisassembly = NULL;
        hr = DisassembleShader(pShaderBytecode, BytecodeLength, &pDisassembly);
        if (SUCCEEDED(hr)) {
            json.beginMember(name);
            json.writeString((const char *)pDisassembly->GetBufferPointer() /*, pDisassembly->GetBufferSize() */);
            json.endMember();
            pDisassembly->Release();
        }
    }

    free(pShaderBytecode);
}

static void
dumpShaders(JSONWriter &json, ID3D10Device *pDevice)
{
    json.beginMember("shaders");
    json.beginObject();

    ID3D10VertexShader *pVertexShader = NULL;
    pDevice->VSGetShader(&pVertexShader);
    if (pVertexShader) {
        dumpShader(json, "vertex", pVertexShader);
        pVertexShader->Release();
    }

    ID3D10PixelShader *pPixelShader = NULL;
    pDevice->PSGetShader(&pPixelShader);
    if (pPixelShader) {
        dumpShader(json, "pixel", pPixelShader);
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

    json.beginMember("textures");
    json.beginObject();
    json.endObject();
    json.endMember(); // textures

    dumpFramebuffer(json, pDevice);
}


} /* namespace d3dstate */

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

#include "d3d9imports.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"
#include "json.hpp"


namespace d3dstate {


template< class T >
inline void
dumpShader(JSONWriter &json, const char *name, T *pShader) {
    if (!pShader) {
        return;
    }

    HRESULT hr;

    UINT SizeOfData = 0;

    hr = pShader->GetFunction(NULL, &SizeOfData);
    if (SUCCEEDED(hr)) {
        void *pData;
        pData = malloc(SizeOfData);
        if (pData) {
            hr = pShader->GetFunction(pData, &SizeOfData);
            if (SUCCEEDED(hr)) {
                IDisassemblyBuffer *pDisassembly = NULL;
                hr = DisassembleShader((const DWORD *)pData, &pDisassembly);
                if (SUCCEEDED(hr)) {
                    json.beginMember(name);
                    json.writeString((const char *)pDisassembly->GetBufferPointer() /*, pDisassembly->GetBufferSize() */);
                    json.endMember();
                    pDisassembly->Release();
                }

            }
            free(pData);
        }
    }

    pShader->Release();
}

static void
dumpShaders(JSONWriter &json, IDirect3DDevice9 *pDevice)
{
    json.beginMember("shaders");

    HRESULT hr;
    json.beginObject();

    IDirect3DVertexShader9 *pVertexShader = NULL;
    hr = pDevice->GetVertexShader(&pVertexShader);
    if (SUCCEEDED(hr)) {
        dumpShader(json, "vertex", pVertexShader);
    }

    IDirect3DPixelShader9 *pPixelShader = NULL;
    hr = pDevice->GetPixelShader(&pPixelShader);
    if (SUCCEEDED(hr)) {
        dumpShader(json, "pixel", pPixelShader);
    }

    json.endObject();
    json.endMember(); // shaders
}

void
dumpDevice(std::ostream &os, IDirect3DDevice9 *pDevice)
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

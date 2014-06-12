/**************************************************************************
 *
 * Copyright 2011-2012 VMware, Inc.
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

#include "d3d8imports.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"
#include "json.hpp"


namespace d3dstate {


typedef HRESULT (STDMETHODCALLTYPE IDirect3DDevice8::*GetShaderFunctionMethod)(DWORD Handle, void* pData, DWORD* pSizeOfData);


struct VertexShaderGetter
{
    HRESULT GetShader(IDirect3DDevice8 *pDevice, DWORD *pHandle) {
        return pDevice->GetVertexShader(pHandle);
    }
    HRESULT GetShaderFunction(IDirect3DDevice8 *pDevice, DWORD Handle, void* pData, DWORD* pSizeOfData) {
        return pDevice->GetVertexShaderFunction(Handle, pData, pSizeOfData);
    }
};

struct PixelShaderGetter
{
    HRESULT GetShader(IDirect3DDevice8 *pDevice, DWORD *pHandle) {
        return pDevice->GetPixelShader(pHandle);
    }
    HRESULT GetShaderFunction(IDirect3DDevice8 *pDevice, DWORD Handle, void* pData, DWORD* pSizeOfData) {
        return pDevice->GetPixelShaderFunction(Handle, pData, pSizeOfData);
    }
};

template<class Getter>
inline void
dumpShader(JSONWriter &json, IDirect3DDevice8 *pDevice, const char *name) {
    Getter getter;
    HRESULT hr;

    DWORD dwShader = 0;
    hr = getter.GetShader(pDevice, &dwShader);
    if (FAILED(hr) || !dwShader) {
        return;
    }

    DWORD SizeOfData = 0;

    hr = getter.GetShaderFunction(pDevice, dwShader, NULL, &SizeOfData);
    if (SUCCEEDED(hr)) {
        void *pData;
        pData = malloc(SizeOfData);
        if (pData) {
            hr = getter.GetShaderFunction(pDevice, dwShader, pData, &SizeOfData);
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
}

static void
dumpShaders(JSONWriter &json, IDirect3DDevice8 *pDevice)
{
    json.beginMember("shaders");
    json.beginObject();

    dumpShader<VertexShaderGetter>(json, pDevice, "vertex");
    dumpShader<PixelShaderGetter>(json, pDevice, "pixel");

    json.endObject();
    json.endMember(); // shaders
}

void
dumpDevice(std::ostream &os, IDirect3DDevice8 *pDevice)
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

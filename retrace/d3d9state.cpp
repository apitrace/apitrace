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

#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3d9imports.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


template< class T >
inline void
dumpShader(StateWriter &writer, const char *name, T *pShader) {
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
                com_ptr<IDisassemblyBuffer> pDisassembly;
                hr = DisassembleShader((const DWORD *)pData, &pDisassembly);
                if (SUCCEEDED(hr)) {
                    writer.beginMember(name);
                    writer.writeString((const char *)pDisassembly->GetBufferPointer() /*, pDisassembly->GetBufferSize() */);
                    writer.endMember();
                }

            }
            free(pData);
        }
    }
}

static void
dumpShaders(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    writer.beginMember("shaders");

    HRESULT hr;
    writer.beginObject();

    com_ptr<IDirect3DVertexShader9> pVertexShader;
    hr = pDevice->GetVertexShader(&pVertexShader);
    if (SUCCEEDED(hr)) {
        dumpShader<IDirect3DVertexShader9>(writer, "vertex", pVertexShader);
    }

    com_ptr<IDirect3DPixelShader9> pPixelShader;
    hr = pDevice->GetPixelShader(&pPixelShader);
    if (SUCCEEDED(hr)) {
        dumpShader<IDirect3DPixelShader9>(writer, "pixel", pPixelShader);
    }

    writer.endObject();
    writer.endMember(); // shaders
}

void
dumpDevice(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    /* TODO */
    writer.beginMember("parameters");
    writer.beginObject();
    writer.endObject();
    writer.endMember(); // parameters

    dumpShaders(writer, pDevice);

    dumpTextures(writer, pDevice);

    dumpFramebuffer(writer, pDevice);
}

void
dumpDevice(StateWriter &writer, IDirect3DSwapChain9 *pSwapChain)
{
    com_ptr<IDirect3DDevice9> pDevice;
    HRESULT hr = pSwapChain->GetDevice(&pDevice);
    if (SUCCEEDED(hr)) {
        dumpDevice(writer, pDevice);
    }
}


} /* namespace d3dstate */

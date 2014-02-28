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

#ifndef _D3D10STATE_HPP_
#define _D3D10STATE_HPP_


#include <windows.h>


#include "json.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


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


image::Image *
getSubResourceImage(ID3D10Device *pDevice,
                    ID3D10Resource *pResource,
                    DXGI_FORMAT Format,
                    UINT ArraySlice,
                    UINT MipSlice);


} /* namespace d3dstate */

#endif // _D3D10STATE_HPP_

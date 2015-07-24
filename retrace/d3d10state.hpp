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

#pragma once


#include <assert.h>

#include <windows.h>

#include <vector>
#include <string>

#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"

#include "d3d10imports.hpp"


namespace d3dstate {


template< typename T >
inline std::vector<BYTE>
getPrivateData(T *pObject, REFGUID guid)
{
    std::vector<BYTE> Data;

    if (!pObject) {
        return Data;
    }

    UINT DataLength = 0;
    HRESULT hr;
    hr = pObject->GetPrivateData(guid, &DataLength, NULL);
    if (hr != S_OK) {
        assert(hr == DXGI_ERROR_NOT_FOUND);
        return Data;
    }

    Data.resize(DataLength);

    hr = pObject->GetPrivateData(guid, &DataLength, &Data[0]);
    assert(hr == S_OK);
    if (FAILED(hr)) {
        Data.clear();
    }

    return Data;
}


template< typename T >
inline void
dumpShader(StateWriter &writer, const char *name, T *pShader) {
    if (!pShader) {
        return;
    }

    /*
     * There is no method to get the shader byte code, so the creator is supposed to
     * attach it via the SetPrivateData method.
     */


    std::vector<BYTE> ShaderBytecode = getPrivateData(pShader, GUID_D3DSTATE);
    if (ShaderBytecode.empty()) {
        return;
    }

    com_ptr<IDisassemblyBuffer> pDisassembly;
    HRESULT hr;
    hr = DisassembleShader(&ShaderBytecode[0], ShaderBytecode.size(), &pDisassembly);
    if (SUCCEEDED(hr)) {
        writer.beginMember(name);
        writer.writeString((const char *)pDisassembly->GetBufferPointer() /*, pDisassembly->GetBufferSize() */);
        writer.endMember();
    }
}


template< typename T >
inline std::string
getObjectName(T *pObject)
{
    std::vector<BYTE> ObjectName = getPrivateData(pObject, WKPDID_D3DDebugObjectName);
    return std::string(ObjectName.begin(), ObjectName.end());
}


image::Image *
getSubResourceImage(ID3D10Device *pDevice,
                    ID3D10Resource *pResource,
                    DXGI_FORMAT Format,
                    UINT ArraySlice,
                    UINT MipSlice);


} /* namespace d3dstate */


/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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

#ifndef _D3DSHADER_HPP_
#define _D3DSHADER_HPP_


#include <windows.h>

// Matches ID3DXBuffer, ID3D10Blob, ID3DBlob
struct IDisassemblyBuffer : public IUnknown {
    virtual LPVOID STDMETHODCALLTYPE GetBufferPointer(void) = 0;

    // XXX: ID3D10Blob, ID3DBlob actually return SIZE_T but DWORD should give
    // the same results
    virtual DWORD STDMETHODCALLTYPE GetBufferSize(void) = 0;
};


// D3D9 and earlier
HRESULT
DisassembleShader(const DWORD *tokens, IDisassemblyBuffer **ppDisassembly);

// D3D10 and higher
HRESULT
DisassembleShader(const void *pShader, SIZE_T BytecodeLength, IDisassemblyBuffer **ppDisassembly);


#endif /* _D3DSHADER_HPP_ */

/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright 2008-2009 VMware, Inc.
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

#include "d3d10shader.hpp"


struct ID3D10Blob : public IUnknown {
public:
    virtual LPVOID STDMETHODCALLTYPE GetBufferPointer( void) = 0;
    virtual SIZE_T STDMETHODCALLTYPE GetBufferSize( void) = 0;
};

typedef ID3D10Blob ID3DBlob;
typedef ID3DBlob* LPD3DBLOB;

#define D3D_DISASM_ENABLE_COLOR_CODE            0x00000001
#define D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS  0x00000002
#define D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING 0x00000004
#define D3D_DISASM_ENABLE_INSTRUCTION_CYCLE     0x00000008
#define D3D_DISASM_DISABLE_DEBUG_INFO           0x00000010
#define D3D_DISASM_ENABLE_INSTRUCTION_OFFSET    0x00000020
#define D3D_DISASM_INSTRUCTION_ONLY             0x00000040

typedef HRESULT
(WINAPI *PFND3DDISASSEMBLE)(
    LPCVOID pSrcData,
    SIZE_T SrcDataSize,
    UINT Flags,
    LPCSTR szComments,
    ID3DBlob **ppDisassembly
);

static PFND3DDISASSEMBLE pfnD3DDisassemble = NULL;

typedef HRESULT
(WINAPI *PFND3D10DISASSEMBLESHADER)(
    const void *pShader,
    SIZE_T BytecodeLength,
    BOOL EnableColorCode,
    LPCSTR pComments,
    ID3D10Blob **ppDisassembly
);

static PFND3D10DISASSEMBLESHADER pfnD3D10DisassembleShader = NULL;

void DumpShader(trace::Writer &writer, const void *pShaderBytecode, SIZE_T BytecodeLength)
{
    static bool firsttime = true;

    if (firsttime) {
        char szFilename[MAX_PATH];
        HMODULE hModule = NULL;

        int version;
        for (version = 44; version >= 33; --version) {
            _snprintf(szFilename, sizeof(szFilename), "d3dcompiler_%i.dll", version);
            hModule = LoadLibraryA(szFilename);
            if (hModule) {
                pfnD3DDisassemble = (PFND3DDISASSEMBLE)
                    GetProcAddress(hModule, "D3DDisassemble");
                if (pfnD3DDisassemble) {
                    break;
                }
            }
        }

        if (!pfnD3DDisassemble) {
            /*
             * Fallback to D3D10DisassembleShader, which should be always present.
             */
            if (GetSystemDirectoryA(szFilename, MAX_PATH)) {
                strcat(szFilename, "\\d3d10.dll");
                hModule = LoadLibraryA(szFilename);
                if (hModule) {
                    pfnD3D10DisassembleShader = (PFND3D10DISASSEMBLESHADER)
                        GetProcAddress(hModule, "D3D10DisassembleShader");
                }
            }
        }

        firsttime = false;
    }

    LPD3DBLOB pDisassembly = NULL;
    HRESULT hr = E_FAIL;

    if (pfnD3DDisassemble) {
        hr = pfnD3DDisassemble(pShaderBytecode, BytecodeLength, 0, NULL, &pDisassembly);
    } else if (pfnD3D10DisassembleShader) {
        hr = pfnD3D10DisassembleShader(pShaderBytecode, BytecodeLength, 0, NULL, &pDisassembly);
    }

    if (SUCCEEDED(hr)) {
        writer.beginRepr();
        writer.writeString((const char *)pDisassembly->GetBufferPointer(), pDisassembly->GetBufferSize());
    }

    writer.writeBlob(pShaderBytecode, BytecodeLength);

    if (pDisassembly) {
        pDisassembly->Release();
    }
    
    if (SUCCEEDED(hr)) {
        writer.endRepr();
    }
}

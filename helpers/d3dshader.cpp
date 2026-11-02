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

#include "d3dshader.hpp"
#include "os.hpp"


struct ID3DXBuffer : public IUnknown {
    virtual LPVOID STDMETHODCALLTYPE GetBufferPointer(void) = 0;
    virtual DWORD  STDMETHODCALLTYPE GetBufferSize(void) = 0;
};

typedef HRESULT
(WINAPI *PD3DXDISASSEMBLESHADER)(
    CONST DWORD *pShader,
    BOOL EnableColorCode,
    LPCSTR pComments,
    ID3DXBuffer **ppDisassembly
);


/*
 * Return the size in bytes of a D3D9 and earlier shader.
 *
 * Equivalent to _shaderSize in d3dcommonsize.hpp, which can't be used here as
 * it depends on D3D headers.
 */
static SIZE_T
shaderSize(const DWORD *tokens)
{
    const DWORD OPCODE_MASK = 0x0000ffff;
    const DWORD OPCODE_COMMENT = 0xfffe;
    const DWORD OPCODE_END = 0xffff;
    const DWORD OPCODE_DEF = 0x51;
    const DWORD INSTLENGTH_MASK = 0x0f000000;
    const DWORD INSTLENGTH_SHIFT = 24;
    const DWORD COMMENTSIZE_MASK = 0x7fff0000;
    const DWORD COMMENTSIZE_SHIFT = 16;

    DWORD version = tokens[0];
    bool modern = ((version >> 8) & 0xff) >= 2;
    SIZE_T length = 1;

    while (true) {
        DWORD token = tokens[length++];
        if (!modern && (token & 0x80000000U)) {
            // Destination/Source/Label tokens
            continue;
        }
        DWORD opcode = token & OPCODE_MASK;
        if (opcode == OPCODE_END) {
            if (token == OPCODE_END) {
                return length * sizeof *tokens;
            }
        } else if (opcode == OPCODE_COMMENT) {
            length += (token & COMMENTSIZE_MASK) >> COMMENTSIZE_SHIFT;
        } else if (modern) {
            length += (token & INSTLENGTH_MASK) >> INSTLENGTH_SHIFT;
        } else if (opcode == OPCODE_DEF) {
            // Contains raw floating point so can't be advanced with the 31 bit test.
            length += 5;
        }
    }
}


/*
 * D3DXDisassembleShader from d3dx9_xx.dll, which is part of the DirectX
 * End-User Runtime and therefore not necessarily installed.
 */
static HRESULT
disassembleShaderD3DX(const DWORD *tokens, IDisassemblyBuffer **ppDisassembly)
{
    static BOOL firsttime = TRUE;

    static HMODULE hD3DXModule = NULL;
    static PD3DXDISASSEMBLESHADER pfnD3DXDisassembleShader = NULL;

    if (firsttime) {
        if (!hD3DXModule) {
            unsigned release;
            int version;
            for (release = 0; release <= 1; ++release) {
                /* Version 41 corresponds to Mar 2009 version of DirectX Runtime / SDK */
                for (version = 41; version >= 0; --version) {
                    char filename[256];
                    _snprintf(filename, sizeof(filename),
                              "d3dx9%s%s%u.dll", release ? "" : "d", version ? "_" : "", version);
                    hD3DXModule = LoadLibraryA(filename);
                    if (hD3DXModule)
                        goto found;
                }
            }
found:
            ;
        }

        if (hD3DXModule) {
            if (!pfnD3DXDisassembleShader) {
                pfnD3DXDisassembleShader = (PD3DXDISASSEMBLESHADER)GetProcAddress(hD3DXModule, "D3DXDisassembleShader");
            }
        }

        firsttime = FALSE;
    }

    HRESULT hr = E_FAIL;
    if (pfnD3DXDisassembleShader) {
        hr = pfnD3DXDisassembleShader(tokens, FALSE, NULL,
                                      reinterpret_cast<ID3DXBuffer **>(ppDisassembly));
    }
    return hr;
}


HRESULT
DisassembleShader(const DWORD *tokens, IDisassemblyBuffer **ppDisassembly)
{
    /*
     * Prefer d3dcompiler_xx.dll, which can also disassemble legacy shaders
     * per http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx
     * and is more likely to be present than d3dx9_xx.dll.
     */
    HRESULT hr = DisassembleShader(tokens, shaderSize(tokens), ppDisassembly);
    if (FAILED(hr)) {
        hr = disassembleShaderD3DX(tokens, ppDisassembly);
    }
    return hr;
}


struct ID3D10Blob : public IUnknown {
    virtual LPVOID STDMETHODCALLTYPE GetBufferPointer(void) = 0;
    virtual SIZE_T STDMETHODCALLTYPE GetBufferSize(void) = 0;
};

typedef ID3D10Blob ID3DBlob;

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


HRESULT
DisassembleShader(const void *pShaderBytecode, SIZE_T BytecodeLength, IDisassemblyBuffer **ppDisassembly)
{
    static bool firsttime = true;

    if (firsttime) {
        char szFilename[MAX_PATH];
        HMODULE hModule = NULL;
        int version;
        for (version = 47; version >= 33; --version) {
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
            os::log("apitrace: warning: failed to load d3dcompiler_xx.dll\n");

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

    HRESULT hr = E_FAIL;

    if (pfnD3DDisassemble) {
        hr = pfnD3DDisassemble(pShaderBytecode, BytecodeLength, 0, NULL,
                               reinterpret_cast<ID3DBlob **>(ppDisassembly));
    } else if (pfnD3D10DisassembleShader) {
        hr = pfnD3D10DisassembleShader(pShaderBytecode, BytecodeLength, 0, NULL,
                                       reinterpret_cast<ID3D10Blob **>(ppDisassembly));
    }

    return hr;
}

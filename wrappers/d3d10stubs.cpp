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


/*
 *Stubs for internal D3D10 functions.
 */


#include "os.hpp"

#include "d3d10imports.hpp"


static HMODULE g_hD3D10Module = NULL;


static PROC
_getD3D10ProcAddress(LPCSTR lpProcName) {
    if (!g_hD3D10Module) {
        char szDll[MAX_PATH] = {0};
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        strcat(szDll, "\\\\d3d10.dll");
        g_hD3D10Module = LoadLibraryA(szDll);
        if (!g_hD3D10Module) {
           return NULL;
        }
    }
    return GetProcAddress(g_hD3D10Module, lpProcName);
}


#ifndef NDEBUG
#define LOG() os::log("%s\n", __FUNCTION__)
#else
#define LOG()
#endif


typedef HRESULT (WINAPI *PFN_D3D10COMPILESHADER)(const char *pSrcData, SIZE_T SrcDataSize, const char *pFileName, const D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, const char *pFunctionName, const char *pProfile, UINT Flags, ID3D10Blob **ppShader, ID3D10Blob **ppErrorMsgs);
static PFN_D3D10COMPILESHADER _D3D10CompileShader_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10CompileShader(const char *pSrcData, SIZE_T SrcDataSize, const char *pFileName, const D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, const char *pFunctionName, const char *pProfile, UINT Flags, ID3D10Blob **ppShader, ID3D10Blob **ppErrorMsgs) {
    const char *_name = "D3D10CompileShader";
    LOG();
    if (!_D3D10CompileShader_ptr) {
        _D3D10CompileShader_ptr = (PFN_D3D10COMPILESHADER)_getD3D10ProcAddress(_name);
        if (!_D3D10CompileShader_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10CompileShader_ptr(pSrcData, SrcDataSize, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs);
}

typedef HRESULT (WINAPI *PFN_D3D10DISASSEMBLESHADER)(const void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, const char *pComments, ID3D10Blob **ppDisassembly);
static PFN_D3D10DISASSEMBLESHADER _D3D10DisassembleShader_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10DisassembleShader(const void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, const char *pComments, ID3D10Blob **ppDisassembly) {
    const char *_name = "D3D10DisassembleShader";
    LOG();
    if (!_D3D10DisassembleShader_ptr) {
        _D3D10DisassembleShader_ptr = (PFN_D3D10DISASSEMBLESHADER)_getD3D10ProcAddress(_name);
        if (!_D3D10DisassembleShader_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10DisassembleShader_ptr(pShader, BytecodeLength, EnableColorCode, pComments, ppDisassembly);
}

typedef const char *(WINAPI *PFN_D3D10GETPIXELSHADERPROFILE)(ID3D10Device *pDevice);
static PFN_D3D10GETPIXELSHADERPROFILE _D3D10GetPixelShaderProfile_ptr = NULL;

EXTERN_C const char *WINAPI
D3D10GetPixelShaderProfile(ID3D10Device *pDevice) {
    const char *_name = "D3D10GetPixelShaderProfile";
    LOG();
    if (!_D3D10GetPixelShaderProfile_ptr) {
        _D3D10GetPixelShaderProfile_ptr = (PFN_D3D10GETPIXELSHADERPROFILE)_getD3D10ProcAddress(_name);
        if (!_D3D10GetPixelShaderProfile_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetPixelShaderProfile_ptr(pDevice);
}

typedef const char *(WINAPI *PFN_D3D10GETVERTEXSHADERPROFILE)(ID3D10Device *pDevice);
static PFN_D3D10GETVERTEXSHADERPROFILE _D3D10GetVertexShaderProfile_ptr = NULL;

EXTERN_C const char *WINAPI
D3D10GetVertexShaderProfile(ID3D10Device *pDevice) {
    const char *_name = "D3D10GetVertexShaderProfile";
    LOG();
    if (!_D3D10GetVertexShaderProfile_ptr) {
        _D3D10GetVertexShaderProfile_ptr = (PFN_D3D10GETVERTEXSHADERPROFILE)_getD3D10ProcAddress(_name);
        if (!_D3D10GetVertexShaderProfile_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetVertexShaderProfile_ptr(pDevice);
}

typedef const char *(WINAPI *PFN_D3D10GETGEOMETRYSHADERPROFILE)(ID3D10Device *pDevice);
static PFN_D3D10GETGEOMETRYSHADERPROFILE _D3D10GetGeometryShaderProfile_ptr = NULL;

EXTERN_C const char *WINAPI
D3D10GetGeometryShaderProfile(ID3D10Device *pDevice) {
    const char *_name = "D3D10GetGeometryShaderProfile";
    LOG();
    if (!_D3D10GetGeometryShaderProfile_ptr) {
        _D3D10GetGeometryShaderProfile_ptr = (PFN_D3D10GETGEOMETRYSHADERPROFILE)_getD3D10ProcAddress(_name);
        if (!_D3D10GetGeometryShaderProfile_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetGeometryShaderProfile_ptr(pDevice);
}

typedef HRESULT (WINAPI *PFN_D3D10REFLECTSHADER)(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection **ppReflector);
static PFN_D3D10REFLECTSHADER _D3D10ReflectShader_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10ReflectShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection **ppReflector) {
    const char *_name = "D3D10ReflectShader";
    LOG();
    if (!_D3D10ReflectShader_ptr) {
        _D3D10ReflectShader_ptr = (PFN_D3D10REFLECTSHADER)_getD3D10ProcAddress(_name);
        if (!_D3D10ReflectShader_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10ReflectShader_ptr(pShaderBytecode, BytecodeLength, ppReflector);
}

typedef HRESULT (WINAPI *PFN_D3D10PREPROCESSSHADER)(const char *pSrcData, SIZE_T SrcDataSize, const char *pFileName, const D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, ID3D10Blob **ppShaderText, ID3D10Blob **ppErrorMsgs);
static PFN_D3D10PREPROCESSSHADER _D3D10PreprocessShader_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10PreprocessShader(const char *pSrcData, SIZE_T SrcDataSize, const char *pFileName, const D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, ID3D10Blob **ppShaderText, ID3D10Blob **ppErrorMsgs) {
    const char *_name = "D3D10PreprocessShader";
    LOG();
    if (!_D3D10PreprocessShader_ptr) {
        _D3D10PreprocessShader_ptr = (PFN_D3D10PREPROCESSSHADER)_getD3D10ProcAddress(_name);
        if (!_D3D10PreprocessShader_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10PreprocessShader_ptr(pSrcData, SrcDataSize, pFileName, pDefines, pInclude, ppShaderText, ppErrorMsgs);
}

typedef HRESULT (WINAPI *PFN_D3D10GETINPUTSIGNATUREBLOB)(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
static PFN_D3D10GETINPUTSIGNATUREBLOB _D3D10GetInputSignatureBlob_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10GetInputSignatureBlob(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob) {
    const char *_name = "D3D10GetInputSignatureBlob";
    LOG();
    if (!_D3D10GetInputSignatureBlob_ptr) {
        _D3D10GetInputSignatureBlob_ptr = (PFN_D3D10GETINPUTSIGNATUREBLOB)_getD3D10ProcAddress(_name);
        if (!_D3D10GetInputSignatureBlob_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetInputSignatureBlob_ptr(pShaderBytecode, BytecodeLength, ppSignatureBlob);
}

typedef HRESULT (WINAPI *PFN_D3D10GETOUTPUTSIGNATUREBLOB)(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
static PFN_D3D10GETOUTPUTSIGNATUREBLOB _D3D10GetOutputSignatureBlob_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10GetOutputSignatureBlob(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob) {
    const char *_name = "D3D10GetOutputSignatureBlob";
    LOG();
    if (!_D3D10GetOutputSignatureBlob_ptr) {
        _D3D10GetOutputSignatureBlob_ptr = (PFN_D3D10GETOUTPUTSIGNATUREBLOB)_getD3D10ProcAddress(_name);
        if (!_D3D10GetOutputSignatureBlob_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetOutputSignatureBlob_ptr(pShaderBytecode, BytecodeLength, ppSignatureBlob);
}

typedef HRESULT (WINAPI *PFN_D3D10GETINPUTANDOUTPUTSIGNATUREBLOB)(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
static PFN_D3D10GETINPUTANDOUTPUTSIGNATUREBLOB _D3D10GetInputAndOutputSignatureBlob_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10GetInputAndOutputSignatureBlob(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob) {
    const char *_name = "D3D10GetInputAndOutputSignatureBlob";
    LOG();
    if (!_D3D10GetInputAndOutputSignatureBlob_ptr) {
        _D3D10GetInputAndOutputSignatureBlob_ptr = (PFN_D3D10GETINPUTANDOUTPUTSIGNATUREBLOB)_getD3D10ProcAddress(_name);
        if (!_D3D10GetInputAndOutputSignatureBlob_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetInputAndOutputSignatureBlob_ptr(pShaderBytecode, BytecodeLength, ppSignatureBlob);
}

typedef HRESULT (WINAPI *PFN_D3D10GETSHADERDEBUGINFO)(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppDebugInfo);
static PFN_D3D10GETSHADERDEBUGINFO _D3D10GetShaderDebugInfo_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10GetShaderDebugInfo(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppDebugInfo) {
    const char *_name = "D3D10GetShaderDebugInfo";
    LOG();
    if (!_D3D10GetShaderDebugInfo_ptr) {
        _D3D10GetShaderDebugInfo_ptr = (PFN_D3D10GETSHADERDEBUGINFO)_getD3D10ProcAddress(_name);
        if (!_D3D10GetShaderDebugInfo_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetShaderDebugInfo_ptr(pShaderBytecode, BytecodeLength, ppDebugInfo);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKUNION)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
static PFN_D3D10STATEBLOCKMASKUNION _D3D10StateBlockMaskUnion_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskUnion(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult) {
    const char *_name = "D3D10StateBlockMaskUnion";
    LOG();
    if (!_D3D10StateBlockMaskUnion_ptr) {
        _D3D10StateBlockMaskUnion_ptr = (PFN_D3D10STATEBLOCKMASKUNION)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskUnion_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskUnion_ptr(pA, pB, pResult);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKINTERSECT)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
static PFN_D3D10STATEBLOCKMASKINTERSECT _D3D10StateBlockMaskIntersect_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskIntersect(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult) {
    const char *_name = "D3D10StateBlockMaskIntersect";
    LOG();
    if (!_D3D10StateBlockMaskIntersect_ptr) {
        _D3D10StateBlockMaskIntersect_ptr = (PFN_D3D10STATEBLOCKMASKINTERSECT)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskIntersect_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskIntersect_ptr(pA, pB, pResult);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKDIFFERENCE)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
static PFN_D3D10STATEBLOCKMASKDIFFERENCE _D3D10StateBlockMaskDifference_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskDifference(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult) {
    const char *_name = "D3D10StateBlockMaskDifference";
    LOG();
    if (!_D3D10StateBlockMaskDifference_ptr) {
        _D3D10StateBlockMaskDifference_ptr = (PFN_D3D10STATEBLOCKMASKDIFFERENCE)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskDifference_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskDifference_ptr(pA, pB, pResult);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKENABLECAPTURE)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
static PFN_D3D10STATEBLOCKMASKENABLECAPTURE _D3D10StateBlockMaskEnableCapture_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskEnableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength) {
    const char *_name = "D3D10StateBlockMaskEnableCapture";
    LOG();
    if (!_D3D10StateBlockMaskEnableCapture_ptr) {
        _D3D10StateBlockMaskEnableCapture_ptr = (PFN_D3D10STATEBLOCKMASKENABLECAPTURE)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskEnableCapture_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskEnableCapture_ptr(pMask, StateType, RangeStart, RangeLength);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKDISABLECAPTURE)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
static PFN_D3D10STATEBLOCKMASKDISABLECAPTURE _D3D10StateBlockMaskDisableCapture_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskDisableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength) {
    const char *_name = "D3D10StateBlockMaskDisableCapture";
    LOG();
    if (!_D3D10StateBlockMaskDisableCapture_ptr) {
        _D3D10StateBlockMaskDisableCapture_ptr = (PFN_D3D10STATEBLOCKMASKDISABLECAPTURE)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskDisableCapture_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskDisableCapture_ptr(pMask, StateType, RangeStart, RangeLength);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKENABLEALL)(D3D10_STATE_BLOCK_MASK *pMask);
static PFN_D3D10STATEBLOCKMASKENABLEALL _D3D10StateBlockMaskEnableAll_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskEnableAll(D3D10_STATE_BLOCK_MASK *pMask) {
    const char *_name = "D3D10StateBlockMaskEnableAll";
    LOG();
    if (!_D3D10StateBlockMaskEnableAll_ptr) {
        _D3D10StateBlockMaskEnableAll_ptr = (PFN_D3D10STATEBLOCKMASKENABLEALL)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskEnableAll_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskEnableAll_ptr(pMask);
}

typedef HRESULT (WINAPI *PFN_D3D10STATEBLOCKMASKDISABLEALL)(D3D10_STATE_BLOCK_MASK *pMask);
static PFN_D3D10STATEBLOCKMASKDISABLEALL _D3D10StateBlockMaskDisableAll_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10StateBlockMaskDisableAll(D3D10_STATE_BLOCK_MASK *pMask) {
    const char *_name = "D3D10StateBlockMaskDisableAll";
    LOG();
    if (!_D3D10StateBlockMaskDisableAll_ptr) {
        _D3D10StateBlockMaskDisableAll_ptr = (PFN_D3D10STATEBLOCKMASKDISABLEALL)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskDisableAll_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskDisableAll_ptr(pMask);
}

typedef BOOL (WINAPI *PFN_D3D10STATEBLOCKMASKGETSETTING)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry);
static PFN_D3D10STATEBLOCKMASKGETSETTING _D3D10StateBlockMaskGetSetting_ptr = NULL;

EXTERN_C BOOL WINAPI
D3D10StateBlockMaskGetSetting(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry) {
    const char *_name = "D3D10StateBlockMaskGetSetting";
    LOG();
    if (!_D3D10StateBlockMaskGetSetting_ptr) {
        _D3D10StateBlockMaskGetSetting_ptr = (PFN_D3D10STATEBLOCKMASKGETSETTING)_getD3D10ProcAddress(_name);
        if (!_D3D10StateBlockMaskGetSetting_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10StateBlockMaskGetSetting_ptr(pMask, StateType, Entry);
}

typedef HRESULT (WINAPI *PFN_D3D10CREATESTATEBLOCK)(ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock);
static PFN_D3D10CREATESTATEBLOCK _D3D10CreateStateBlock_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10CreateStateBlock(ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock) {
    const char *_name = "D3D10CreateStateBlock";
    LOG();
    if (!_D3D10CreateStateBlock_ptr) {
        _D3D10CreateStateBlock_ptr = (PFN_D3D10CREATESTATEBLOCK)_getD3D10ProcAddress(_name);
        if (!_D3D10CreateStateBlock_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10CreateStateBlock_ptr(pDevice, pStateBlockMask, ppStateBlock);
}

typedef HRESULT (WINAPI *PFN_D3D10COMPILEEFFECTFROMMEMORY)(void *pData, SIZE_T DataLength, const char *pSrcFileName, const D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, UINT HLSLFlags, UINT FXFlags, ID3D10Blob **ppCompiledEffect, ID3D10Blob **ppErrors);
static PFN_D3D10COMPILEEFFECTFROMMEMORY _D3D10CompileEffectFromMemory_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10CompileEffectFromMemory(void *pData, SIZE_T DataLength, const char *pSrcFileName, const D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, UINT HLSLFlags, UINT FXFlags, ID3D10Blob **ppCompiledEffect, ID3D10Blob **ppErrors) {
    const char *_name = "D3D10CompileEffectFromMemory";
    LOG();
    if (!_D3D10CompileEffectFromMemory_ptr) {
        _D3D10CompileEffectFromMemory_ptr = (PFN_D3D10COMPILEEFFECTFROMMEMORY)_getD3D10ProcAddress(_name);
        if (!_D3D10CompileEffectFromMemory_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10CompileEffectFromMemory_ptr(pData, DataLength, pSrcFileName, pDefines, pInclude, HLSLFlags, FXFlags, ppCompiledEffect, ppErrors);
}

typedef HRESULT (WINAPI *PFN_D3D10CREATEEFFECTFROMMEMORY)(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool *pEffectPool, ID3D10Effect **ppEffect);
static PFN_D3D10CREATEEFFECTFROMMEMORY _D3D10CreateEffectFromMemory_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10CreateEffectFromMemory(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool *pEffectPool, ID3D10Effect **ppEffect) {
    const char *_name = "D3D10CreateEffectFromMemory";
    LOG();
    if (!_D3D10CreateEffectFromMemory_ptr) {
        _D3D10CreateEffectFromMemory_ptr = (PFN_D3D10CREATEEFFECTFROMMEMORY)_getD3D10ProcAddress(_name);
        if (!_D3D10CreateEffectFromMemory_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10CreateEffectFromMemory_ptr(pData, DataLength, FXFlags, pDevice, pEffectPool, ppEffect);
}

typedef HRESULT (WINAPI *PFN_D3D10CREATEEFFECTPOOLFROMMEMORY)(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool **ppEffectPool);
static PFN_D3D10CREATEEFFECTPOOLFROMMEMORY _D3D10CreateEffectPoolFromMemory_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10CreateEffectPoolFromMemory(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool **ppEffectPool) {
    const char *_name = "D3D10CreateEffectPoolFromMemory";
    LOG();
    if (!_D3D10CreateEffectPoolFromMemory_ptr) {
        _D3D10CreateEffectPoolFromMemory_ptr = (PFN_D3D10CREATEEFFECTPOOLFROMMEMORY)_getD3D10ProcAddress(_name);
        if (!_D3D10CreateEffectPoolFromMemory_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10CreateEffectPoolFromMemory_ptr(pData, DataLength, FXFlags, pDevice, ppEffectPool);
}

typedef HRESULT (WINAPI *PFN_D3D10DISASSEMBLEEFFECT)(ID3D10Effect *pEffect, BOOL EnableColorCode, ID3D10Blob **ppDisassembly);
static PFN_D3D10DISASSEMBLEEFFECT _D3D10DisassembleEffect_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10DisassembleEffect(ID3D10Effect *pEffect, BOOL EnableColorCode, ID3D10Blob **ppDisassembly) {
    const char *_name = "D3D10DisassembleEffect";
    LOG();
    if (!_D3D10DisassembleEffect_ptr) {
        _D3D10DisassembleEffect_ptr = (PFN_D3D10DISASSEMBLEEFFECT)_getD3D10ProcAddress(_name);
        if (!_D3D10DisassembleEffect_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10DisassembleEffect_ptr(pEffect, EnableColorCode, ppDisassembly);
}

typedef HRESULT (WINAPI *PFN_D3D10CREATEBLOB)(SIZE_T NumBytes, ID3D10Blob **ppBuffer);
static PFN_D3D10CREATEBLOB _D3D10CreateBlob_ptr = NULL;

EXTERN_C HRESULT WINAPI
D3D10CreateBlob(SIZE_T NumBytes, ID3D10Blob **ppBuffer) {
    const char *_name = "D3D10CreateBlob";
    LOG();
    if (!_D3D10CreateBlob_ptr) {
        _D3D10CreateBlob_ptr = (PFN_D3D10CREATEBLOB)_getD3D10ProcAddress(_name);
        if (!_D3D10CreateBlob_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10CreateBlob_ptr(NumBytes, ppBuffer);
}

typedef DWORD (WINAPI *PFN_D3D10GETVERSION)(void);
static PFN_D3D10GETVERSION _D3D10GetVersion_ptr = NULL;

EXTERN_C DWORD WINAPI
D3D10GetVersion(void) {
    const char *_name = "D3D10GetVersion";
    LOG();
    if (!_D3D10GetVersion_ptr) {
        _D3D10GetVersion_ptr = (PFN_D3D10GETVERSION)_getD3D10ProcAddress(_name);
        if (!_D3D10GetVersion_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10GetVersion_ptr();
}

typedef DWORD (WINAPI *PFN_D3D10REGISTERLAYERS)(void);
static PFN_D3D10REGISTERLAYERS _D3D10RegisterLayers_ptr = NULL;

EXTERN_C DWORD WINAPI
D3D10RegisterLayers(void) {
    const char *_name = "D3D10RegisterLayers";
    LOG();
    if (!_D3D10RegisterLayers_ptr) {
        _D3D10RegisterLayers_ptr = (PFN_D3D10REGISTERLAYERS)_getD3D10ProcAddress(_name);
        if (!_D3D10RegisterLayers_ptr) {
            os::log("error: unavailable function %s\n", _name);
            os::abort();
        }
    }
    return _D3D10RegisterLayers_ptr();
}


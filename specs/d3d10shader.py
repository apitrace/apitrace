##########################################################################
#
# Copyright 2012 Jose Fonseca
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


from d3dcommon import *
from d3d10 import *


D3D10_SHADER = Flags(UINT, [
    "D3D10_SHADER_DEBUG",
    "D3D10_SHADER_SKIP_VALIDATION",
    "D3D10_SHADER_SKIP_OPTIMIZATION",
    "D3D10_SHADER_PACK_MATRIX_ROW_MAJOR",
    "D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR",
    "D3D10_SHADER_PARTIAL_PRECISION",
    "D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT",
    "D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT",
    "D3D10_SHADER_NO_PRESHADER",
    "D3D10_SHADER_AVOID_FLOW_CONTROL",
    "D3D10_SHADER_PREFER_FLOW_CONTROL",
    "D3D10_SHADER_ENABLE_STRICTNESS",
    "D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY",
    "D3D10_SHADER_IEEE_STRICTNESS",
    "D3D10_SHADER_WARNINGS_ARE_ERRORS",
    "D3D10_SHADER_OPTIMIZATION_LEVEL0",
    "D3D10_SHADER_OPTIMIZATION_LEVEL1",
    "D3D10_SHADER_OPTIMIZATION_LEVEL2",
    "D3D10_SHADER_OPTIMIZATION_LEVEL3",
])

D3D10_SHADER_DESC = Struct("D3D10_SHADER_DESC", [
    (UINT, "Version"),
    (LPCSTR, "Creator"),
    (UINT, "Flags"),
    (UINT, "ConstantBuffers"),
    (UINT, "BoundResources"),
    (UINT, "InputParameters"),
    (UINT, "OutputParameters"),
    (UINT, "InstructionCount"),
    (UINT, "TempRegisterCount"),
    (UINT, "TempArrayCount"),
    (UINT, "DefCount"),
    (UINT, "DclCount"),
    (UINT, "TextureNormalInstructions"),
    (UINT, "TextureLoadInstructions"),
    (UINT, "TextureCompInstructions"),
    (UINT, "TextureBiasInstructions"),
    (UINT, "TextureGradientInstructions"),
    (UINT, "FloatInstructionCount"),
    (UINT, "IntInstructionCount"),
    (UINT, "UintInstructionCount"),
    (UINT, "StaticFlowControlCount"),
    (UINT, "DynamicFlowControlCount"),
    (UINT, "MacroInstructionCount"),
    (UINT, "ArrayInstructionCount"),
    (UINT, "CutInstructionCount"),
    (UINT, "EmitInstructionCount"),
    (D3D10_PRIMITIVE_TOPOLOGY, "GSOutputTopology"),
    (UINT, "GSMaxOutputVertexCount"),
])

D3D10_SHADER_BUFFER_DESC = Struct("D3D10_SHADER_BUFFER_DESC", [
    (LPCSTR, "Name"),
    (D3D10_CBUFFER_TYPE, "Type"),
    (UINT, "Variables"),
    (UINT, "Size"),
    (UINT, "uFlags"),
])

D3D10_SHADER_VARIABLE_DESC = Struct("D3D10_SHADER_VARIABLE_DESC", [
    (LPCSTR, "Name"),
    (UINT, "StartOffset"),
    (UINT, "Size"),
    (UINT, "uFlags"),
    (LPVOID, "DefaultValue"),
])

D3D10_SHADER_TYPE_DESC = Struct("D3D10_SHADER_TYPE_DESC", [
    (D3D10_SHADER_VARIABLE_CLASS, "Class"),
    (D3D10_SHADER_VARIABLE_TYPE, "Type"),
    (UINT, "Rows"),
    (UINT, "Columns"),
    (UINT, "Elements"),
    (UINT, "Members"),
    (UINT, "Offset"),
])

D3D10_SHADER_INPUT_BIND_DESC = Struct("D3D10_SHADER_INPUT_BIND_DESC", [
    (LPCSTR, "Name"),
    (D3D10_SHADER_INPUT_TYPE, "Type"),
    (UINT, "BindPoint"),
    (UINT, "BindCount"),
    (UINT, "uFlags"),
    (D3D10_RESOURCE_RETURN_TYPE, "ReturnType"),
    (D3D10_SRV_DIMENSION, "Dimension"),
    (UINT, "NumSamples"),
])

D3D10_SIGNATURE_PARAMETER_DESC = Struct("D3D10_SIGNATURE_PARAMETER_DESC", [
    (LPCSTR, "SemanticName"),
    (UINT, "SemanticIndex"),
    (UINT, "Register"),
    (D3D10_NAME, "SystemValueType"),
    (D3D10_REGISTER_COMPONENT_TYPE, "ComponentType"),
    (BYTE, "Mask"),
    (BYTE, "ReadWriteMask"),
])

ID3D10ShaderReflectionType = Interface("ID3D10ShaderReflectionType")
ID3D10ShaderReflectionType.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_SHADER_TYPE_DESC), "pDesc")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionType), "GetMemberTypeByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionType), "GetMemberTypeByName", [(LPCSTR, "Name")]),
    StdMethod(LPCSTR, "GetMemberTypeName", [(UINT, "Index")]),
]

ID3D10ShaderReflectionVariable = Interface("ID3D10ShaderReflectionVariable")
ID3D10ShaderReflectionVariable.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_SHADER_VARIABLE_DESC), "pDesc")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionType), "GetType", []),
]

ID3D10ShaderReflectionConstantBuffer = Interface("ID3D10ShaderReflectionConstantBuffer")
ID3D10ShaderReflectionConstantBuffer.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_SHADER_BUFFER_DESC), "pDesc")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionVariable), "GetVariableByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionVariable), "GetVariableByName", [(LPCSTR, "Name")]),
]

ID3D10ShaderReflection = Interface("ID3D10ShaderReflection", IUnknown)
ID3D10ShaderReflection.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_SHADER_DESC), "pDesc")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionConstantBuffer), "GetConstantBufferByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10ShaderReflectionConstantBuffer), "GetConstantBufferByName", [(LPCSTR, "Name")]),
    StdMethod(HRESULT, "GetResourceBindingDesc", [(UINT, "ResourceIndex"), Out(Pointer(D3D10_SHADER_INPUT_BIND_DESC), "pDesc")]),
    StdMethod(HRESULT, "GetInputParameterDesc", [(UINT, "ParameterIndex"), Out(Pointer(D3D10_SIGNATURE_PARAMETER_DESC), "pDesc")]),
    StdMethod(HRESULT, "GetOutputParameterDesc", [(UINT, "ParameterIndex"), Out(Pointer(D3D10_SIGNATURE_PARAMETER_DESC), "pDesc")]),
]


d3d10.addFunctions([
    #StdFunction(HRESULT, "D3D10CompileShader", [(LPCSTR, "pSrcData"), (SIZE_T, "SrcDataSize"), (LPCSTR, "pFileName"), (Pointer(Const(D3D10_SHADER_MACRO)), "pDefines"), (LPD3D10INCLUDE, "pInclude"), (LPCSTR, "pFunctionName"), (LPCSTR, "pProfile"), (UINT, "Flags"), Out(Pointer(LPD3D10BLOB), "ppShader"), Out(Pointer(LPD3D10BLOB), "ppErrorMsgs")]),
    #StdFunction(HRESULT, "D3D10DisassembleShader", [(Blob(Const(Void), "BytecodeLength"), "pShader"), (SIZE_T, "BytecodeLength"), (BOOL, "EnableColorCode"), (LPCSTR, "pComments"), Out(Pointer(LPD3D10BLOB), "ppDisassembly")]),
    StdFunction(LPCSTR, "D3D10GetPixelShaderProfile", [(ObjPointer(ID3D10Device), "pDevice")]),
    StdFunction(LPCSTR, "D3D10GetVertexShaderProfile", [(ObjPointer(ID3D10Device), "pDevice")]),
    StdFunction(LPCSTR, "D3D10GetGeometryShaderProfile", [(ObjPointer(ID3D10Device), "pDevice")]),
    #StdFunction(HRESULT, "D3D10ReflectShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(ObjPointer(ID3D10ShaderReflection)), "ppReflector")]),
    #StdFunction(HRESULT, "D3D10PreprocessShader", [(LPCSTR, "pSrcData"), (SIZE_T, "SrcDataSize"), (LPCSTR, "pFileName"), (Pointer(Const(D3D10_SHADER_MACRO)), "pDefines"), (LPD3D10INCLUDE, "pInclude"), Out(Pointer(LPD3D10BLOB), "ppShaderText"), Out(Pointer(LPD3D10BLOB), "ppErrorMsgs")]),
    #StdFunction(HRESULT, "D3D10GetInputSignatureBlob", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(LPD3D10BLOB), "ppSignatureBlob")]),
    #StdFunction(HRESULT, "D3D10GetOutputSignatureBlob", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(LPD3D10BLOB), "ppSignatureBlob")]),
    #StdFunction(HRESULT, "D3D10GetInputAndOutputSignatureBlob", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(LPD3D10BLOB), "ppSignatureBlob")]),
    #StdFunction(HRESULT, "D3D10GetShaderDebugInfo", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(LPD3D10BLOB), "ppDebugInfo")]),
])

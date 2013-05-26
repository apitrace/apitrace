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


ID3D11ShaderReflectionType = Interface("ID3D11ShaderReflectionType", IUnknown)
ID3D11ShaderReflectionVariable = Interface("ID3D11ShaderReflectionVariable", IUnknown)
ID3D11ShaderReflectionConstantBuffer = Interface("ID3D11ShaderReflectionConstantBuffer", IUnknown)
ID3D11ShaderReflection = Interface("ID3D11ShaderReflection", IUnknown)


D3D11_SHADER_VERSION_TYPE = Enum("D3D11_SHADER_VERSION_TYPE", [
    "D3D11_SHVER_PIXEL_SHADER",
    "D3D11_SHVER_VERTEX_SHADER",
    "D3D11_SHVER_GEOMETRY_SHADER",
    "D3D11_SHVER_HULL_SHADER",
    "D3D11_SHVER_DOMAIN_SHADER",
    "D3D11_SHVER_COMPUTE_SHADER",
])

D3D11_SIGNATURE_PARAMETER_DESC = Struct("D3D11_SIGNATURE_PARAMETER_DESC", [
    (LPCSTR, "SemanticName"),
    (UINT, "SemanticIndex"),
    (UINT, "Register"),
    (D3D_NAME, "SystemValueType"),
    (D3D_REGISTER_COMPONENT_TYPE, "ComponentType"),
    (BYTE, "Mask"),
    (BYTE, "ReadWriteMask"),
    (UINT, "Stream"),
])

D3D11_SHADER_BUFFER_DESC = Struct("D3D11_SHADER_BUFFER_DESC", [
    (LPCSTR, "Name"),
    (D3D_CBUFFER_TYPE, "Type"),
    (UINT, "Variables"),
    (UINT, "Size"),
    (UINT, "uFlags"),
])

D3D11_SHADER_VARIABLE_DESC = Struct("D3D11_SHADER_VARIABLE_DESC", [
    (LPCSTR, "Name"),
    (UINT, "StartOffset"),
    (UINT, "Size"),
    (UINT, "uFlags"),
    (LPVOID, "DefaultValue"),
    (UINT, "StartTexture"),
    (UINT, "TextureSize"),
    (UINT, "StartSampler"),
    (UINT, "SamplerSize"),
])

D3D11_SHADER_TYPE_DESC = Struct("D3D11_SHADER_TYPE_DESC", [
    (D3D_SHADER_VARIABLE_CLASS, "Class"),
    (D3D_SHADER_VARIABLE_TYPE, "Type"),
    (UINT, "Rows"),
    (UINT, "Columns"),
    (UINT, "Elements"),
    (UINT, "Members"),
    (UINT, "Offset"),
    (LPCSTR, "Name"),
])

D3D11_SHADER_DESC = Struct("D3D11_SHADER_DESC", [
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
    (D3D_PRIMITIVE_TOPOLOGY, "GSOutputTopology"),
    (UINT, "GSMaxOutputVertexCount"),
    (D3D_PRIMITIVE, "InputPrimitive"),
    (UINT, "PatchConstantParameters"),
    (UINT, "cGSInstanceCount"),
    (UINT, "cControlPoints"),
    (D3D_TESSELLATOR_OUTPUT_PRIMITIVE, "HSOutputPrimitive"),
    (D3D_TESSELLATOR_PARTITIONING, "HSPartitioning"),
    (D3D_TESSELLATOR_DOMAIN, "TessellatorDomain"),
    (UINT, "cBarrierInstructions"),
    (UINT, "cInterlockedInstructions"),
    (UINT, "cTextureStoreInstructions"),
])

D3D11_SHADER_INPUT_BIND_DESC = Struct("D3D11_SHADER_INPUT_BIND_DESC", [
    (LPCSTR, "Name"),
    (D3D_SHADER_INPUT_TYPE, "Type"),
    (UINT, "BindPoint"),
    (UINT, "BindCount"),
    (UINT, "uFlags"),
    (D3D_RESOURCE_RETURN_TYPE, "ReturnType"),
    (D3D_SRV_DIMENSION, "Dimension"),
    (UINT, "NumSamples"),
])

ID3D11ShaderReflectionType.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D11_SHADER_TYPE_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D11ShaderReflectionType), "GetMemberTypeByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D11ShaderReflectionType), "GetMemberTypeByName", [(LPCSTR, "Name")]),
    StdMethod(LPCSTR, "GetMemberTypeName", [(UINT, "Index")], sideeffects=False),
    StdMethod(HRESULT, "IsEqual", [(ObjPointer(ID3D11ShaderReflectionType), "pType")], sideeffects=False),
    StdMethod(ObjPointer(ID3D11ShaderReflectionType), "GetSubType", []),
    StdMethod(ObjPointer(ID3D11ShaderReflectionType), "GetBaseClass", []),
    StdMethod(UINT, "GetNumInterfaces", [], sideeffects=False),
    StdMethod(ObjPointer(ID3D11ShaderReflectionType), "GetInterfaceByIndex", [(UINT, "uIndex")]),
    StdMethod(HRESULT, "IsOfType", [(ObjPointer(ID3D11ShaderReflectionType), "pType")], sideeffects=False),
    StdMethod(HRESULT, "ImplementsInterface", [(ObjPointer(ID3D11ShaderReflectionType), "pBase")], sideeffects=False),
]

ID3D11ShaderReflectionVariable.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D11_SHADER_VARIABLE_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D11ShaderReflectionType), "GetType", []),
    StdMethod(ObjPointer(ID3D11ShaderReflectionConstantBuffer), "GetBuffer", []),
    StdMethod(UINT, "GetInterfaceSlot", [(UINT, "uArrayIndex")], sideeffects=False),
]

ID3D11ShaderReflectionConstantBuffer.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D11_SHADER_BUFFER_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D11ShaderReflectionVariable), "GetVariableByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D11ShaderReflectionVariable), "GetVariableByName", [(LPCSTR, "Name")]),
]

ID3D11ShaderReflection.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D11_SHADER_DESC), "pDesc")]),
    StdMethod(ObjPointer(ID3D11ShaderReflectionConstantBuffer), "GetConstantBufferByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D11ShaderReflectionConstantBuffer), "GetConstantBufferByName", [(LPCSTR, "Name")]),
    StdMethod(HRESULT, "GetResourceBindingDesc", [(UINT, "ResourceIndex"), Out(Pointer(D3D11_SHADER_INPUT_BIND_DESC), "pDesc")]),
    StdMethod(HRESULT, "GetInputParameterDesc", [(UINT, "ParameterIndex"), Out(Pointer(D3D11_SIGNATURE_PARAMETER_DESC), "pDesc")]),
    StdMethod(HRESULT, "GetOutputParameterDesc", [(UINT, "ParameterIndex"), Out(Pointer(D3D11_SIGNATURE_PARAMETER_DESC), "pDesc")]),
    StdMethod(HRESULT, "GetPatchConstantParameterDesc", [(UINT, "ParameterIndex"), Out(Pointer(D3D11_SIGNATURE_PARAMETER_DESC), "pDesc")]),
    StdMethod(ObjPointer(ID3D11ShaderReflectionVariable), "GetVariableByName", [(LPCSTR, "Name")]),
    StdMethod(HRESULT, "GetResourceBindingDescByName", [(LPCSTR, "Name"), Out(Pointer(D3D11_SHADER_INPUT_BIND_DESC), "pDesc")]),
    StdMethod(UINT, "GetMovInstructionCount", [], sideeffects=False),
    StdMethod(UINT, "GetMovcInstructionCount", [], sideeffects=False),
    StdMethod(UINT, "GetConversionInstructionCount", [], sideeffects=False),
    StdMethod(UINT, "GetBitwiseInstructionCount", [], sideeffects=False),
    StdMethod(D3D_PRIMITIVE, "GetGSInputPrimitive", [], sideeffects=False),
    StdMethod(BOOL, "IsSampleFrequencyShader", [], sideeffects=False),
    StdMethod(UINT, "GetNumInterfaceSlots", [], sideeffects=False),
    StdMethod(HRESULT, "GetMinFeatureLevel", [Out(Pointer(D3D_FEATURE_LEVEL), "pLevel")]),
    StdMethod(UINT, "GetThreadGroupSize", [Out(Pointer(UINT), "pSizeX"), Out(Pointer(UINT), "pSizeY"), Out(Pointer(UINT), "pSizeZ")], sideeffects=False),
]


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


from d3d10 import *

from d3d10shader import *


ID3D10StateBlock = Interface("ID3D10StateBlock", IUnknown)
ID3D10EffectType = Interface("ID3D10EffectType")
ID3D10EffectVariable = Interface("ID3D10EffectVariable")
ID3D10EffectScalarVariable = Interface("ID3D10EffectScalarVariable", ID3D10EffectVariable)
ID3D10EffectVectorVariable = Interface("ID3D10EffectVectorVariable", ID3D10EffectVariable)
ID3D10EffectMatrixVariable = Interface("ID3D10EffectMatrixVariable", ID3D10EffectVariable)
ID3D10EffectStringVariable = Interface("ID3D10EffectStringVariable", ID3D10EffectVariable)
ID3D10EffectShaderResourceVariable = Interface("ID3D10EffectShaderResourceVariable", ID3D10EffectVariable)
ID3D10EffectRenderTargetViewVariable = Interface("ID3D10EffectRenderTargetViewVariable", ID3D10EffectVariable)
ID3D10EffectDepthStencilViewVariable = Interface("ID3D10EffectDepthStencilViewVariable", ID3D10EffectVariable)
ID3D10EffectConstantBuffer = Interface("ID3D10EffectConstantBuffer", ID3D10EffectVariable)
ID3D10EffectShaderVariable = Interface("ID3D10EffectShaderVariable", ID3D10EffectVariable)
ID3D10EffectBlendVariable = Interface("ID3D10EffectBlendVariable", ID3D10EffectVariable)
ID3D10EffectDepthStencilVariable = Interface("ID3D10EffectDepthStencilVariable", ID3D10EffectVariable)
ID3D10EffectRasterizerVariable = Interface("ID3D10EffectRasterizerVariable", ID3D10EffectVariable)
ID3D10EffectSamplerVariable = Interface("ID3D10EffectSamplerVariable", ID3D10EffectVariable)
ID3D10EffectPass = Interface("ID3D10EffectPass")
ID3D10EffectTechnique = Interface("ID3D10EffectTechnique")
ID3D10Effect = Interface("ID3D10Effect", IUnknown)
ID3D10EffectPool = Interface("ID3D10EffectPool", IUnknown)


D3D10_DEVICE_STATE_TYPES = Enum("D3D10_DEVICE_STATE_TYPES", [
    "D3D10_DST_SO_BUFFERS",
    "D3D10_DST_OM_RENDER_TARGETS",
    "D3D10_DST_OM_DEPTH_STENCIL_STATE",
    "D3D10_DST_OM_BLEND_STATE",
    "D3D10_DST_VS",
    "D3D10_DST_VS_SAMPLERS",
    "D3D10_DST_VS_SHADER_RESOURCES",
    "D3D10_DST_VS_CONSTANT_BUFFERS",
    "D3D10_DST_GS",
    "D3D10_DST_GS_SAMPLERS",
    "D3D10_DST_GS_SHADER_RESOURCES",
    "D3D10_DST_GS_CONSTANT_BUFFERS",
    "D3D10_DST_PS",
    "D3D10_DST_PS_SAMPLERS",
    "D3D10_DST_PS_SHADER_RESOURCES",
    "D3D10_DST_PS_CONSTANT_BUFFERS",
    "D3D10_DST_IA_VERTEX_BUFFERS",
    "D3D10_DST_IA_INDEX_BUFFER",
    "D3D10_DST_IA_INPUT_LAYOUT",
    "D3D10_DST_IA_PRIMITIVE_TOPOLOGY",
    "D3D10_DST_RS_VIEWPORTS",
    "D3D10_DST_RS_SCISSOR_RECTS",
    "D3D10_DST_RS_RASTERIZER_STATE",
    "D3D10_DST_PREDICATION",
])

D3D10_STATE_BLOCK_MASK = Struct("D3D10_STATE_BLOCK_MASK", [
    (BYTE, "VS"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)"), "VSSamplers"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT)"), "VSShaderResources"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)"), "VSConstantBuffers"),
    (BYTE, "GS"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)"), "GSSamplers"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT)"), "GSShaderResources"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)"), "GSConstantBuffers"),
    (BYTE, "PS"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)"), "PSSamplers"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT)"), "PSShaderResources"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)"), "PSConstantBuffers"),
    (Array(BYTE, "D3D10_BYTES_FROM_BITS(D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)"), "IAVertexBuffers"),
    (BYTE, "IAIndexBuffer"),
    (BYTE, "IAInputLayout"),
    (BYTE, "IAPrimitiveTopology"),
    (BYTE, "OMRenderTargets"),
    (BYTE, "OMDepthStencilState"),
    (BYTE, "OMBlendState"),
    (BYTE, "RSViewports"),
    (BYTE, "RSScissorRects"),
    (BYTE, "RSRasterizerState"),
    (BYTE, "SOBuffers"),
    (BYTE, "Predication"),
])

ID3D10StateBlock.methods += [
    StdMethod(HRESULT, "Capture", []),
    StdMethod(HRESULT, "Apply", []),
    StdMethod(HRESULT, "ReleaseAllDeviceObjects", []),
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(ObjPointer(ID3D10Device)), "ppDevice")]),
]

D3D10_EFFECT = Flags(UINT, [
    "D3D10_EFFECT_COMPILE_CHILD_EFFECT",
    "D3D10_EFFECT_COMPILE_ALLOW_SLOW_OPS",
    "D3D10_EFFECT_SINGLE_THREADED",
    "D3D10_EFFECT_VARIABLE_POOLED",
    "D3D10_EFFECT_VARIABLE_ANNOTATION",
    "D3D10_EFFECT_VARIABLE_EXPLICIT_BIND_POINT",
])

D3D10_EFFECT_TYPE_DESC = Struct("D3D10_EFFECT_TYPE_DESC", [
    (LPCSTR, "TypeName"),
    (D3D10_SHADER_VARIABLE_CLASS, "Class"),
    (D3D10_SHADER_VARIABLE_TYPE, "Type"),
    (UINT, "Elements"),
    (UINT, "Members"),
    (UINT, "Rows"),
    (UINT, "Columns"),
    (UINT, "PackedSize"),
    (UINT, "UnpackedSize"),
    (UINT, "Stride"),
])

ID3D10EffectType.methods += [
    StdMethod(BOOL, "IsValid", [], sideeffects=False),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_EFFECT_TYPE_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D10EffectType), "GetMemberTypeByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectType), "GetMemberTypeByName", [(LPCSTR, "Name")]),
    StdMethod(ObjPointer(ID3D10EffectType), "GetMemberTypeBySemantic", [(LPCSTR, "Semantic")]),
    StdMethod(LPCSTR, "GetMemberName", [(UINT, "Index")], sideeffects=False),
    StdMethod(LPCSTR, "GetMemberSemantic", [(UINT, "Index")], sideeffects=False),
]

D3D10_EFFECT_VARIABLE_DESC = Struct("D3D10_EFFECT_VARIABLE_DESC", [
    (LPCSTR, "Name"),
    (LPCSTR, "Semantic"),
    (UINT, "Flags"),
    (UINT, "Annotations"),
    (UINT, "BufferOffset"),
    (UINT, "ExplicitBindPoint"),
])

ID3D10EffectVariable.methods += [
    StdMethod(BOOL, "IsValid", [], sideeffects=False),
    StdMethod(ObjPointer(ID3D10EffectType), "GetType", []),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_EFFECT_VARIABLE_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetAnnotationByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetAnnotationByName", [(LPCSTR, "Name")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetMemberByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetMemberByName", [(LPCSTR, "Name")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetMemberBySemantic", [(LPCSTR, "Semantic")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetElement", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectConstantBuffer), "GetParentConstantBuffer", []),
    StdMethod(ObjPointer(ID3D10EffectScalarVariable), "AsScalar", []),
    StdMethod(ObjPointer(ID3D10EffectVectorVariable), "AsVector", []),
    StdMethod(ObjPointer(ID3D10EffectMatrixVariable), "AsMatrix", []),
    StdMethod(ObjPointer(ID3D10EffectStringVariable), "AsString", []),
    StdMethod(ObjPointer(ID3D10EffectShaderResourceVariable), "AsShaderResource", []),
    StdMethod(ObjPointer(ID3D10EffectRenderTargetViewVariable), "AsRenderTargetView", []),
    StdMethod(ObjPointer(ID3D10EffectDepthStencilViewVariable), "AsDepthStencilView", []),
    StdMethod(ObjPointer(ID3D10EffectConstantBuffer), "AsConstantBuffer", []),
    StdMethod(ObjPointer(ID3D10EffectShaderVariable), "AsShader", []),
    StdMethod(ObjPointer(ID3D10EffectBlendVariable), "AsBlend", []),
    StdMethod(ObjPointer(ID3D10EffectDepthStencilVariable), "AsDepthStencil", []),
    StdMethod(ObjPointer(ID3D10EffectRasterizerVariable), "AsRasterizer", []),
    StdMethod(ObjPointer(ID3D10EffectSamplerVariable), "AsSampler", []),
    StdMethod(HRESULT, "SetRawValue", [(Blob(Void, "ByteCount"), "pData"), (UINT, "Offset"), (UINT, "ByteCount")]),
    StdMethod(HRESULT, "GetRawValue", [Out(OpaqueBlob(Void, "ByteCount"), "pData"), (UINT, "Offset"), (UINT, "ByteCount")], sideeffects=False),
]

ID3D10EffectScalarVariable.methods += [
    StdMethod(HRESULT, "SetFloat", [(Float, "Value")]),
    StdMethod(HRESULT, "GetFloat", [Out(Pointer(Float), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "SetFloatArray", [(Pointer(Float), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetFloatArray", [Out(Pointer(Float), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
    StdMethod(HRESULT, "SetInt", [(Int, "Value")]),
    StdMethod(HRESULT, "GetInt", [Out(Pointer(Int), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "SetIntArray", [(Pointer(Int), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetIntArray", [Out(Pointer(Int), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
    StdMethod(HRESULT, "SetBool", [(BOOL, "Value")]),
    StdMethod(HRESULT, "GetBool", [Out(Pointer(BOOL), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "SetBoolArray", [(Pointer(BOOL), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetBoolArray", [Out(Pointer(BOOL), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
]

ID3D10EffectVectorVariable.methods += [
    StdMethod(HRESULT, "SetBoolVector", [(Array(BOOL, "4"), "pData")]),
    StdMethod(HRESULT, "SetIntVector", [(Array(Int, "4"), "pData")]),
    StdMethod(HRESULT, "SetFloatVector", [(Array(Float, "4"), "pData")]),
    StdMethod(HRESULT, "GetBoolVector", [Out(Array(BOOL, "4"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "GetIntVector", [Out(Array(Int, "4"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "GetFloatVector", [Out(Array(Float, "4"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetBoolVectorArray", [(Array(BOOL, "4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "SetIntVectorArray", [(Array(Int, "4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "SetFloatVectorArray", [(Array(Float, "4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetBoolVectorArray", [Out(Array(BOOL, "4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
    StdMethod(HRESULT, "GetIntVectorArray", [Out(Array(Int, "4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
    StdMethod(HRESULT, "GetFloatVectorArray", [Out(Array(Float, "4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
]

ID3D10EffectMatrixVariable.methods += [
    StdMethod(HRESULT, "SetMatrix", [(Array(Float, "4*4"), "pData")]),
    StdMethod(HRESULT, "GetMatrix", [Out(Array(Float, "4*4"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetMatrixArray", [(Array(Float, "4*4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetMatrixArray", [Out(Array(Float, "4*4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
    StdMethod(HRESULT, "SetMatrixTranspose", [(Array(Float, "4*4"), "pData")]),
    StdMethod(HRESULT, "GetMatrixTranspose", [Out(Array(Float, "4*4"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetMatrixTransposeArray", [(Array(Float, "4*4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetMatrixTransposeArray", [Out(Array(Float, "4*4*Count"), "pData"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
]

ID3D10EffectStringVariable.methods += [
    StdMethod(HRESULT, "GetString", [Out(Pointer(LPCSTR), "ppString")], sideeffects=False),
    StdMethod(HRESULT, "GetStringArray", [Out(Array(LPCSTR, "Count"), "ppStrings"), (UINT, "Offset"), (UINT, "Count")], sideeffects=False),
]

ID3D10EffectShaderResourceVariable.methods += [
    StdMethod(HRESULT, "SetResource", [(ObjPointer(ID3D10ShaderResourceView), "pResource")]),
    StdMethod(HRESULT, "GetResource", [Out(Pointer(ObjPointer(ID3D10ShaderResourceView)), "ppResource")]),
    StdMethod(HRESULT, "SetResourceArray", [(Array(ObjPointer(ID3D10ShaderResourceView), "Count"), "ppResources"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetResourceArray", [Out(Array(ObjPointer(ID3D10ShaderResourceView), "Count"), "ppResources"), (UINT, "Offset"), (UINT, "Count")]),
]

ID3D10EffectRenderTargetViewVariable.methods += [
    StdMethod(HRESULT, "SetRenderTarget", [(ObjPointer(ID3D10RenderTargetView), "pResource")]),
    StdMethod(HRESULT, "GetRenderTarget", [Out(Pointer(ObjPointer(ID3D10RenderTargetView)), "ppResource")]),
    StdMethod(HRESULT, "SetRenderTargetArray", [(Array(ObjPointer(ID3D10RenderTargetView), "Count"), "ppResources"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetRenderTargetArray", [Out(Array(ObjPointer(ID3D10RenderTargetView), "Count"), "ppResources"), (UINT, "Offset"), (UINT, "Count")]),
]

ID3D10EffectDepthStencilViewVariable.methods += [
    StdMethod(HRESULT, "SetDepthStencil", [(ObjPointer(ID3D10DepthStencilView), "pResource")]),
    StdMethod(HRESULT, "GetDepthStencil", [Out(Pointer(ObjPointer(ID3D10DepthStencilView)), "ppResource")]),
    StdMethod(HRESULT, "SetDepthStencilArray", [(Array(ObjPointer(ID3D10DepthStencilView), "Count"), "ppResources"), (UINT, "Offset"), (UINT, "Count")]),
    StdMethod(HRESULT, "GetDepthStencilArray", [Out(Array(ObjPointer(ID3D10DepthStencilView), "Count"), "ppResources"), (UINT, "Offset"), (UINT, "Count")]),
]

ID3D10EffectConstantBuffer.methods += [
    StdMethod(HRESULT, "SetConstantBuffer", [(ObjPointer(ID3D10Buffer), "pConstantBuffer")]),
    StdMethod(HRESULT, "GetConstantBuffer", [Out(Pointer(ObjPointer(ID3D10Buffer)), "ppConstantBuffer")]),
    StdMethod(HRESULT, "SetTextureBuffer", [(ObjPointer(ID3D10ShaderResourceView), "pTextureBuffer")]),
    StdMethod(HRESULT, "GetTextureBuffer", [Out(Pointer(ObjPointer(ID3D10ShaderResourceView)), "ppTextureBuffer")]),
]

D3D10_EFFECT_SHADER_DESC = Struct("D3D10_EFFECT_SHADER_DESC", [
    (Pointer(Const(BYTE)), "pInputSignature"),
    (BOOL, "IsInline"),
    (Pointer(Const(BYTE)), "pBytecode"),
    (UINT, "BytecodeLength"),
    (LPCSTR, "SODecl"),
    (UINT, "NumInputSignatureEntries"),
    (UINT, "NumOutputSignatureEntries"),
])

ID3D10EffectShaderVariable.methods += [
    StdMethod(HRESULT, "GetShaderDesc", [(UINT, "ShaderIndex"), Out(Pointer(D3D10_EFFECT_SHADER_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetVertexShader", [(UINT, "ShaderIndex"), Out(Pointer(ObjPointer(ID3D10VertexShader)), "ppVS")]),
    StdMethod(HRESULT, "GetGeometryShader", [(UINT, "ShaderIndex"), Out(Pointer(ObjPointer(ID3D10GeometryShader)), "ppGS")]),
    StdMethod(HRESULT, "GetPixelShader", [(UINT, "ShaderIndex"), Out(Pointer(ObjPointer(ID3D10PixelShader)), "ppPS")]),
    StdMethod(HRESULT, "GetInputSignatureElementDesc", [(UINT, "ShaderIndex"), (UINT, "Element"), Out(Pointer(D3D10_SIGNATURE_PARAMETER_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetOutputSignatureElementDesc", [(UINT, "ShaderIndex"), (UINT, "Element"), Out(Pointer(D3D10_SIGNATURE_PARAMETER_DESC), "pDesc")], sideeffects=False),
]

ID3D10EffectBlendVariable.methods += [
    StdMethod(HRESULT, "GetBlendState", [(UINT, "Index"), Out(Pointer(ObjPointer(ID3D10BlendState)), "ppBlendState")]),
    StdMethod(HRESULT, "GetBackingStore", [(UINT, "Index"), Out(Pointer(D3D10_BLEND_DESC), "pBlendDesc")], sideeffects=False),
]

ID3D10EffectDepthStencilVariable.methods += [
    StdMethod(HRESULT, "GetDepthStencilState", [(UINT, "Index"), Out(Pointer(ObjPointer(ID3D10DepthStencilState)), "ppDepthStencilState")]),
    StdMethod(HRESULT, "GetBackingStore", [(UINT, "Index"), Out(Pointer(D3D10_DEPTH_STENCIL_DESC), "pDepthStencilDesc")], sideeffects=False),
]

ID3D10EffectRasterizerVariable.methods += [
    StdMethod(HRESULT, "GetRasterizerState", [(UINT, "Index"), Out(Pointer(ObjPointer(ID3D10RasterizerState)), "ppRasterizerState")]),
    StdMethod(HRESULT, "GetBackingStore", [(UINT, "Index"), Out(Pointer(D3D10_RASTERIZER_DESC), "pRasterizerDesc")], sideeffects=False),
]

ID3D10EffectSamplerVariable.methods += [
    StdMethod(HRESULT, "GetSampler", [(UINT, "Index"), Out(Pointer(ObjPointer(ID3D10SamplerState)), "ppSampler")]),
    StdMethod(HRESULT, "GetBackingStore", [(UINT, "Index"), Out(Pointer(D3D10_SAMPLER_DESC), "pSamplerDesc")], sideeffects=False),
]

D3D10_PASS_DESC = Struct("D3D10_PASS_DESC", [
    (LPCSTR, "Name"),
    (UINT, "Annotations"),
    (Pointer(BYTE), "pIAInputSignature"),
    (SIZE_T, "IAInputSignatureSize"),
    (UINT, "StencilRef"),
    (UINT, "SampleMask"),
    (Array(FLOAT, 4), "BlendFactor"),
])

D3D10_PASS_SHADER_DESC = Struct("D3D10_PASS_SHADER_DESC", [
    (ObjPointer(ID3D10EffectShaderVariable), "pShaderVariable"),
    (UINT, "ShaderIndex"),
])

ID3D10EffectPass.methods += [
    StdMethod(BOOL, "IsValid", [], sideeffects=False),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_PASS_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetVertexShaderDesc", [Out(Pointer(D3D10_PASS_SHADER_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetGeometryShaderDesc", [Out(Pointer(D3D10_PASS_SHADER_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetPixelShaderDesc", [Out(Pointer(D3D10_PASS_SHADER_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetAnnotationByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetAnnotationByName", [(LPCSTR, "Name")]),
    StdMethod(HRESULT, "Apply", [(UINT, "Flags")]),
    StdMethod(HRESULT, "ComputeStateBlockMask", [Out(Pointer(D3D10_STATE_BLOCK_MASK), "pStateBlockMask")], sideeffects=False),
]

D3D10_TECHNIQUE_DESC = Struct("D3D10_TECHNIQUE_DESC", [
    (LPCSTR, "Name"),
    (UINT, "Passes"),
    (UINT, "Annotations"),
])

ID3D10EffectTechnique.methods += [
    StdMethod(BOOL, "IsValid", [], sideeffects=False),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_TECHNIQUE_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetAnnotationByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetAnnotationByName", [(LPCSTR, "Name")]),
    StdMethod(ObjPointer(ID3D10EffectPass), "GetPassByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectPass), "GetPassByName", [(LPCSTR, "Name")]),
    StdMethod(HRESULT, "ComputeStateBlockMask", [Out(Pointer(D3D10_STATE_BLOCK_MASK), "pStateBlockMask")], sideeffects=False),
]

D3D10_EFFECT_DESC = Struct("D3D10_EFFECT_DESC", [
    (BOOL, "IsChildEffect"),
    (UINT, "ConstantBuffers"),
    (UINT, "SharedConstantBuffers"),
    (UINT, "GlobalVariables"),
    (UINT, "SharedGlobalVariables"),
    (UINT, "Techniques"),
])

ID3D10Effect.methods += [
    StdMethod(BOOL, "IsValid", [], sideeffects=False),
    StdMethod(BOOL, "IsPool", [], sideeffects=False),
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(ObjPointer(ID3D10Device)), "ppDevice")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3D10_EFFECT_DESC), "pDesc")], sideeffects=False),
    StdMethod(ObjPointer(ID3D10EffectConstantBuffer), "GetConstantBufferByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectConstantBuffer), "GetConstantBufferByName", [(LPCSTR, "Name")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetVariableByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetVariableByName", [(LPCSTR, "Name")]),
    StdMethod(ObjPointer(ID3D10EffectVariable), "GetVariableBySemantic", [(LPCSTR, "Semantic")]),
    StdMethod(ObjPointer(ID3D10EffectTechnique), "GetTechniqueByIndex", [(UINT, "Index")]),
    StdMethod(ObjPointer(ID3D10EffectTechnique), "GetTechniqueByName", [(LPCSTR, "Name")]),
    StdMethod(HRESULT, "Optimize", []),
    StdMethod(BOOL, "IsOptimized", [], sideeffects=False),
]

ID3D10EffectPool.methods += [
    StdMethod(ObjPointer(ID3D10Effect), "AsEffect", []),
]


d3d10.addFunctions([
    StdFunction(HRESULT, "D3D10StateBlockMaskUnion", [(Pointer(D3D10_STATE_BLOCK_MASK), "pA"), (Pointer(D3D10_STATE_BLOCK_MASK), "pB"), Out(Pointer(D3D10_STATE_BLOCK_MASK), "pResult")]),
    StdFunction(HRESULT, "D3D10StateBlockMaskIntersect", [(Pointer(D3D10_STATE_BLOCK_MASK), "pA"), (Pointer(D3D10_STATE_BLOCK_MASK), "pB"), Out(Pointer(D3D10_STATE_BLOCK_MASK), "pResult")]),
    StdFunction(HRESULT, "D3D10StateBlockMaskDifference", [(Pointer(D3D10_STATE_BLOCK_MASK), "pA"), (Pointer(D3D10_STATE_BLOCK_MASK), "pB"), Out(Pointer(D3D10_STATE_BLOCK_MASK), "pResult")]),
    StdFunction(HRESULT, "D3D10StateBlockMaskEnableCapture", [Out(Pointer(D3D10_STATE_BLOCK_MASK), "pMask"), (D3D10_DEVICE_STATE_TYPES, "StateType"), (UINT, "RangeStart"), (UINT, "RangeLength")]),
    StdFunction(HRESULT, "D3D10StateBlockMaskDisableCapture", [Out(Pointer(D3D10_STATE_BLOCK_MASK), "pMask"), (D3D10_DEVICE_STATE_TYPES, "StateType"), (UINT, "RangeStart"), (UINT, "RangeLength")]),
    StdFunction(HRESULT, "D3D10StateBlockMaskEnableAll", [Out(Pointer(D3D10_STATE_BLOCK_MASK), "pMask")]),
    StdFunction(HRESULT, "D3D10StateBlockMaskDisableAll", [Out(Pointer(D3D10_STATE_BLOCK_MASK), "pMask")]),
    StdFunction(BOOL, "D3D10StateBlockMaskGetSetting", [(Pointer(D3D10_STATE_BLOCK_MASK), "pMask"), (D3D10_DEVICE_STATE_TYPES, "StateType"), (UINT, "Entry")]),
    StdFunction(HRESULT, "D3D10CreateStateBlock", [(ObjPointer(ID3D10Device), "pDevice"), (Pointer(D3D10_STATE_BLOCK_MASK), "pStateBlockMask"), Out(Pointer(ObjPointer(ID3D10StateBlock)), "ppStateBlock")]),
    StdFunction(HRESULT, "D3D10CompileEffectFromMemory", [(Blob(Void, "DataLength"), "pData"), (SIZE_T, "DataLength"), (LPCSTR, "pSrcFileName"), (Pointer(Const(D3D10_SHADER_MACRO)), "pDefines"), (LPD3D10INCLUDE, "pInclude"), (UINT, "HLSLFlags"), (UINT, "FXFlags"), Out(Pointer(LPD3D10BLOB), "ppCompiledEffect"), Out(Pointer(LPD3D10BLOB), "ppErrors")]),
    StdFunction(HRESULT, "D3D10CreateEffectFromMemory", [(Blob(Void, "DataLength"), "pData"), (SIZE_T, "DataLength"), (UINT, "FXFlags"), (ObjPointer(ID3D10Device), "pDevice"), (ObjPointer(ID3D10EffectPool), "pEffectPool"), Out(Pointer(ObjPointer(ID3D10Effect)), "ppEffect")]),
    StdFunction(HRESULT, "D3D10CreateEffectPoolFromMemory", [(Blob(Void, "DataLength"), "pData"), (SIZE_T, "DataLength"), (UINT, "FXFlags"), (ObjPointer(ID3D10Device), "pDevice"), Out(Pointer(ObjPointer(ID3D10EffectPool)), "ppEffectPool")]),
    StdFunction(HRESULT, "D3D10DisassembleEffect", [(ObjPointer(ID3D10Effect), "pEffect"), (BOOL, "EnableColorCode"), Out(Pointer(LPD3D10BLOB), "ppDisassembly")]),
])

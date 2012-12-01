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


from dxgi import *
from d3dcommon import *
from d3d11sdklayers import *


HRESULT = MAKE_HRESULT([
    "D3D11_ERROR_FILE_NOT_FOUND",
    "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS",
    "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS",
    "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD",
    "D3DERR_INVALIDCALL",
    "D3DERR_WASSTILLDRAWING",
])


ID3D11DepthStencilState = Interface("ID3D11DepthStencilState", ID3D11DeviceChild)
ID3D11BlendState = Interface("ID3D11BlendState", ID3D11DeviceChild)
ID3D11RasterizerState = Interface("ID3D11RasterizerState", ID3D11DeviceChild)
ID3D11Resource = Interface("ID3D11Resource", ID3D11DeviceChild)
ID3D11Buffer = Interface("ID3D11Buffer", ID3D11Resource)
ID3D11Texture1D = Interface("ID3D11Texture1D", ID3D11Resource)
ID3D11Texture2D = Interface("ID3D11Texture2D", ID3D11Resource)
ID3D11Texture3D = Interface("ID3D11Texture3D", ID3D11Resource)
ID3D11View = Interface("ID3D11View", ID3D11DeviceChild)
ID3D11ShaderResourceView = Interface("ID3D11ShaderResourceView", ID3D11View)
ID3D11RenderTargetView = Interface("ID3D11RenderTargetView", ID3D11View)
ID3D11DepthStencilView = Interface("ID3D11DepthStencilView", ID3D11View)
ID3D11UnorderedAccessView = Interface("ID3D11UnorderedAccessView", ID3D11View)
ID3D11VertexShader = Interface("ID3D11VertexShader", ID3D11DeviceChild)
ID3D11HullShader = Interface("ID3D11HullShader", ID3D11DeviceChild)
ID3D11DomainShader = Interface("ID3D11DomainShader", ID3D11DeviceChild)
ID3D11GeometryShader = Interface("ID3D11GeometryShader", ID3D11DeviceChild)
ID3D11PixelShader = Interface("ID3D11PixelShader", ID3D11DeviceChild)
ID3D11ComputeShader = Interface("ID3D11ComputeShader", ID3D11DeviceChild)
ID3D11InputLayout = Interface("ID3D11InputLayout", ID3D11DeviceChild)
ID3D11SamplerState = Interface("ID3D11SamplerState", ID3D11DeviceChild)
ID3D11Asynchronous = Interface("ID3D11Asynchronous", ID3D11DeviceChild)
ID3D11Query = Interface("ID3D11Query", ID3D11Asynchronous)
ID3D11Predicate = Interface("ID3D11Predicate", ID3D11Query)
ID3D11Counter = Interface("ID3D11Counter", ID3D11Asynchronous)
ID3D11ClassInstance = Interface("ID3D11ClassInstance", ID3D11DeviceChild)
ID3D11ClassLinkage = Interface("ID3D11ClassLinkage", ID3D11DeviceChild)
ID3D11CommandList = Interface("ID3D11CommandList", ID3D11DeviceChild)
ID3D11Device = Interface("ID3D11Device", IUnknown)


D3D11_INPUT_CLASSIFICATION = Enum("D3D11_INPUT_CLASSIFICATION", [
    "D3D11_INPUT_PER_VERTEX_DATA",
    "D3D11_INPUT_PER_INSTANCE_DATA",
])

D3D11_INPUT_ELEMENT_ALIGNED_BYTE_OFFSET = FakeEnum(UINT, [
    "D3D11_APPEND_ALIGNED_ELEMENT",
])

D3D11_INPUT_ELEMENT_DESC = Struct("D3D11_INPUT_ELEMENT_DESC", [
    (LPCSTR, "SemanticName"),
    (UINT, "SemanticIndex"),
    (DXGI_FORMAT, "Format"),
    (UINT, "InputSlot"),
    (D3D11_INPUT_ELEMENT_ALIGNED_BYTE_OFFSET, "AlignedByteOffset"),
    (D3D11_INPUT_CLASSIFICATION, "InputSlotClass"),
    (UINT, "InstanceDataStepRate"),
])

D3D11_FILL_MODE = Enum("D3D11_FILL_MODE", [
    "D3D11_FILL_WIREFRAME",
    "D3D11_FILL_SOLID",
])

D3D11_PRIMITIVE_TOPOLOGY = Enum("D3D11_PRIMITIVE_TOPOLOGY", [
    "D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED",
    "D3D11_PRIMITIVE_TOPOLOGY_POINTLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_LINELIST",
    "D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP",
    "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST",
    "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP",
    "D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ",
    "D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ",
    "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ",
    "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ",
    "D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST",
    "D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST",
])

D3D11_PRIMITIVE = Enum("D3D11_PRIMITIVE", [
    "D3D11_PRIMITIVE_UNDEFINED",
    "D3D11_PRIMITIVE_POINT",
    "D3D11_PRIMITIVE_LINE",
    "D3D11_PRIMITIVE_TRIANGLE",
    "D3D11_PRIMITIVE_LINE_ADJ",
    "D3D11_PRIMITIVE_TRIANGLE_ADJ",
    "D3D11_PRIMITIVE_1_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_2_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_3_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_4_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_5_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_6_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_7_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_8_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_9_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_10_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_11_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_12_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_13_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_14_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_15_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_16_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_17_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_18_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_19_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_20_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_21_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_22_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_23_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_24_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_25_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_26_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_27_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_28_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_29_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_30_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_31_CONTROL_POINT_PATCH",
    "D3D11_PRIMITIVE_32_CONTROL_POINT_PATCH",
])

D3D11_CULL_MODE = Enum("D3D11_CULL_MODE", [
    "D3D11_CULL_NONE",
    "D3D11_CULL_FRONT",
    "D3D11_CULL_BACK",
])

D3D11_SO_DECLARATION_ENTRY = Struct("D3D11_SO_DECLARATION_ENTRY", [
    (UINT, "Stream"),
    (LPCSTR, "SemanticName"),
    (UINT, "SemanticIndex"),
    (BYTE, "StartComponent"),
    (BYTE, "ComponentCount"),
    (BYTE, "OutputSlot"),
])

D3D11_VIEWPORT = Struct("D3D11_VIEWPORT", [
    (FLOAT, "TopLeftX"),
    (FLOAT, "TopLeftY"),
    (FLOAT, "Width"),
    (FLOAT, "Height"),
    (FLOAT, "MinDepth"),
    (FLOAT, "MaxDepth"),
])

D3D11_RESOURCE_DIMENSION = Enum("D3D11_RESOURCE_DIMENSION", [
    "D3D11_RESOURCE_DIMENSION_UNKNOWN",
    "D3D11_RESOURCE_DIMENSION_BUFFER",
    "D3D11_RESOURCE_DIMENSION_TEXTURE1D",
    "D3D11_RESOURCE_DIMENSION_TEXTURE2D",
    "D3D11_RESOURCE_DIMENSION_TEXTURE3D",
])

D3D11_SRV_DIMENSION = Enum("D3D11_SRV_DIMENSION", [
    "D3D11_SRV_DIMENSION_UNKNOWN",
    "D3D11_SRV_DIMENSION_BUFFER",
    "D3D11_SRV_DIMENSION_TEXTURE1D",
    "D3D11_SRV_DIMENSION_TEXTURE1DARRAY",
    "D3D11_SRV_DIMENSION_TEXTURE2D",
    "D3D11_SRV_DIMENSION_TEXTURE2DARRAY",
    "D3D11_SRV_DIMENSION_TEXTURE2DMS",
    "D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY",
    "D3D11_SRV_DIMENSION_TEXTURE3D",
    "D3D11_SRV_DIMENSION_TEXTURECUBE",
    "D3D11_SRV_DIMENSION_TEXTURECUBEARRAY",
    "D3D11_SRV_DIMENSION_BUFFEREX",
])

D3D11_DSV_DIMENSION = Enum("D3D11_DSV_DIMENSION", [
    "D3D11_DSV_DIMENSION_UNKNOWN",
    "D3D11_DSV_DIMENSION_TEXTURE1D",
    "D3D11_DSV_DIMENSION_TEXTURE1DARRAY",
    "D3D11_DSV_DIMENSION_TEXTURE2D",
    "D3D11_DSV_DIMENSION_TEXTURE2DARRAY",
    "D3D11_DSV_DIMENSION_TEXTURE2DMS",
    "D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY",
])

D3D11_RTV_DIMENSION = Enum("D3D11_RTV_DIMENSION", [
    "D3D11_RTV_DIMENSION_UNKNOWN",
    "D3D11_RTV_DIMENSION_BUFFER",
    "D3D11_RTV_DIMENSION_TEXTURE1D",
    "D3D11_RTV_DIMENSION_TEXTURE1DARRAY",
    "D3D11_RTV_DIMENSION_TEXTURE2D",
    "D3D11_RTV_DIMENSION_TEXTURE2DARRAY",
    "D3D11_RTV_DIMENSION_TEXTURE2DMS",
    "D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY",
    "D3D11_RTV_DIMENSION_TEXTURE3D",
])

D3D11_UAV_DIMENSION = Enum("D3D11_UAV_DIMENSION", [
    "D3D11_UAV_DIMENSION_UNKNOWN",
    "D3D11_UAV_DIMENSION_BUFFER",
    "D3D11_UAV_DIMENSION_TEXTURE1D",
    "D3D11_UAV_DIMENSION_TEXTURE1DARRAY",
    "D3D11_UAV_DIMENSION_TEXTURE2D",
    "D3D11_UAV_DIMENSION_TEXTURE2DARRAY",
    "D3D11_UAV_DIMENSION_TEXTURE3D",
])

D3D11_USAGE = Enum("D3D11_USAGE", [
    "D3D11_USAGE_DEFAULT",
    "D3D11_USAGE_IMMUTABLE",
    "D3D11_USAGE_DYNAMIC",
    "D3D11_USAGE_STAGING",
])

D3D11_BIND_FLAG = Flags(UINT, [
    "D3D11_BIND_VERTEX_BUFFER",
    "D3D11_BIND_INDEX_BUFFER",
    "D3D11_BIND_CONSTANT_BUFFER",
    "D3D11_BIND_SHADER_RESOURCE",
    "D3D11_BIND_STREAM_OUTPUT",
    "D3D11_BIND_RENDER_TARGET",
    "D3D11_BIND_DEPTH_STENCIL",
    "D3D11_BIND_UNORDERED_ACCESS",
])

D3D11_CPU_ACCESS_FLAG = Flags(UINT, [
    "D3D11_CPU_ACCESS_WRITE",
    "D3D11_CPU_ACCESS_READ",
])

D3D11_RESOURCE_MISC_FLAG = Flags(UINT, [
    "D3D11_RESOURCE_MISC_GENERATE_MIPS",
    "D3D11_RESOURCE_MISC_SHARED",
    "D3D11_RESOURCE_MISC_TEXTURECUBE",
    "D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS",
    "D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS",
    "D3D11_RESOURCE_MISC_BUFFER_STRUCTURED",
    "D3D11_RESOURCE_MISC_RESOURCE_CLAMP",
    "D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX",
    "D3D11_RESOURCE_MISC_GDI_COMPATIBLE",
])

D3D11_MAP = Enum("D3D11_MAP", [
    "D3D11_MAP_READ",
    "D3D11_MAP_WRITE",
    "D3D11_MAP_READ_WRITE",
    "D3D11_MAP_WRITE_DISCARD",
    "D3D11_MAP_WRITE_NO_OVERWRITE",
])

D3D11_MAP_FLAG = Flags(UINT, [
    "D3D11_MAP_FLAG_DO_NOT_WAIT",
])

D3D11_RAISE_FLAG = Flags(UINT, [
    "D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR",
])

D3D11_CLEAR_FLAG = Flags(UINT, [
    "D3D11_CLEAR_DEPTH",
    "D3D11_CLEAR_STENCIL",
])

D3D11_RECT = Alias("D3D11_RECT", RECT)
D3D11_BOX = Struct("D3D11_BOX", [
    (UINT, "left"),
    (UINT, "top"),
    (UINT, "front"),
    (UINT, "right"),
    (UINT, "bottom"),
    (UINT, "back"),
])

ID3D11DeviceChild.methods += [
    StdMethod(Void, "GetDevice", [Out(Pointer(ObjPointer(ID3D11Device)), "ppDevice")]),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "guid"), Out(Pointer(UINT), "pDataSize"), Out(OpaquePointer(Void), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "guid"), (UINT, "DataSize"), (OpaqueBlob(Const(Void), "DataSize"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetPrivateDataInterface", [(REFGUID, "guid"), (OpaquePointer(Const(IUnknown)), "pData")], sideeffects=False),
]

D3D11_COMPARISON_FUNC = Enum("D3D11_COMPARISON_FUNC", [
    "D3D11_COMPARISON_NEVER",
    "D3D11_COMPARISON_LESS",
    "D3D11_COMPARISON_EQUAL",
    "D3D11_COMPARISON_LESS_EQUAL",
    "D3D11_COMPARISON_GREATER",
    "D3D11_COMPARISON_NOT_EQUAL",
    "D3D11_COMPARISON_GREATER_EQUAL",
    "D3D11_COMPARISON_ALWAYS",
])

D3D11_DEPTH_WRITE_MASK = Enum("D3D11_DEPTH_WRITE_MASK", [
    "D3D11_DEPTH_WRITE_MASK_ZERO",
    "D3D11_DEPTH_WRITE_MASK_ALL",
])

D3D11_STENCIL_OP = Enum("D3D11_STENCIL_OP", [
    "D3D11_STENCIL_OP_KEEP",
    "D3D11_STENCIL_OP_ZERO",
    "D3D11_STENCIL_OP_REPLACE",
    "D3D11_STENCIL_OP_INCR_SAT",
    "D3D11_STENCIL_OP_DECR_SAT",
    "D3D11_STENCIL_OP_INVERT",
    "D3D11_STENCIL_OP_INCR",
    "D3D11_STENCIL_OP_DECR",
])

D3D11_DEPTH_STENCILOP_DESC = Struct("D3D11_DEPTH_STENCILOP_DESC", [
    (D3D11_STENCIL_OP, "StencilFailOp"),
    (D3D11_STENCIL_OP, "StencilDepthFailOp"),
    (D3D11_STENCIL_OP, "StencilPassOp"),
    (D3D11_COMPARISON_FUNC, "StencilFunc"),
])

D3D11_DEPTH_STENCIL_DESC = Struct("D3D11_DEPTH_STENCIL_DESC", [
    (BOOL, "DepthEnable"),
    (D3D11_DEPTH_WRITE_MASK, "DepthWriteMask"),
    (D3D11_COMPARISON_FUNC, "DepthFunc"),
    (BOOL, "StencilEnable"),
    (UINT8, "StencilReadMask"),
    (UINT8, "StencilWriteMask"),
    (D3D11_DEPTH_STENCILOP_DESC, "FrontFace"),
    (D3D11_DEPTH_STENCILOP_DESC, "BackFace"),
])

ID3D11DepthStencilState.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_DEPTH_STENCIL_DESC), "pDesc")], sideeffects=False),
]

D3D11_BLEND = Enum("D3D11_BLEND", [
    "D3D11_BLEND_ZERO",
    "D3D11_BLEND_ONE",
    "D3D11_BLEND_SRC_COLOR",
    "D3D11_BLEND_INV_SRC_COLOR",
    "D3D11_BLEND_SRC_ALPHA",
    "D3D11_BLEND_INV_SRC_ALPHA",
    "D3D11_BLEND_DEST_ALPHA",
    "D3D11_BLEND_INV_DEST_ALPHA",
    "D3D11_BLEND_DEST_COLOR",
    "D3D11_BLEND_INV_DEST_COLOR",
    "D3D11_BLEND_SRC_ALPHA_SAT",
    "D3D11_BLEND_BLEND_FACTOR",
    "D3D11_BLEND_INV_BLEND_FACTOR",
    "D3D11_BLEND_SRC1_COLOR",
    "D3D11_BLEND_INV_SRC1_COLOR",
    "D3D11_BLEND_SRC1_ALPHA",
    "D3D11_BLEND_INV_SRC1_ALPHA",
])

D3D11_BLEND_OP = Enum("D3D11_BLEND_OP", [
    "D3D11_BLEND_OP_ADD",
    "D3D11_BLEND_OP_SUBTRACT",
    "D3D11_BLEND_OP_REV_SUBTRACT",
    "D3D11_BLEND_OP_MIN",
    "D3D11_BLEND_OP_MAX",
])

D3D11_COLOR_WRITE_ENABLE = Enum("D3D11_COLOR_WRITE_ENABLE", [
    "D3D11_COLOR_WRITE_ENABLE_ALL",
    "D3D11_COLOR_WRITE_ENABLE_RED",
    "D3D11_COLOR_WRITE_ENABLE_GREEN",
    "D3D11_COLOR_WRITE_ENABLE_BLUE",
    "D3D11_COLOR_WRITE_ENABLE_ALPHA",
])

D3D11_RENDER_TARGET_BLEND_DESC = Struct("D3D11_RENDER_TARGET_BLEND_DESC", [
    (BOOL, "BlendEnable"),
    (D3D11_BLEND, "SrcBlend"),
    (D3D11_BLEND, "DestBlend"),
    (D3D11_BLEND_OP, "BlendOp"),
    (D3D11_BLEND, "SrcBlendAlpha"),
    (D3D11_BLEND, "DestBlendAlpha"),
    (D3D11_BLEND_OP, "BlendOpAlpha"),
    (UINT8, "RenderTargetWriteMask"),
])

D3D11_BLEND_DESC = Struct("D3D11_BLEND_DESC", [
    (BOOL, "AlphaToCoverageEnable"),
    (BOOL, "IndependentBlendEnable"),
    (Array(D3D11_RENDER_TARGET_BLEND_DESC, 8), "RenderTarget"),
])

ID3D11BlendState.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_BLEND_DESC), "pDesc")], sideeffects=False),
]

D3D11_RASTERIZER_DESC = Struct("D3D11_RASTERIZER_DESC", [
    (D3D11_FILL_MODE, "FillMode"),
    (D3D11_CULL_MODE, "CullMode"),
    (BOOL, "FrontCounterClockwise"),
    (INT, "DepthBias"),
    (FLOAT, "DepthBiasClamp"),
    (FLOAT, "SlopeScaledDepthBias"),
    (BOOL, "DepthClipEnable"),
    (BOOL, "ScissorEnable"),
    (BOOL, "MultisampleEnable"),
    (BOOL, "AntialiasedLineEnable"),
])

ID3D11RasterizerState.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_RASTERIZER_DESC), "pDesc")], sideeffects=False),
]

D3D11_SUBRESOURCE_DATA = Struct("D3D11_SUBRESOURCE_DATA", [
    (Blob(Const(Void), "_calcSubresourceSize(pDesc, {i}, {self}.SysMemPitch, {self}.SysMemSlicePitch)"), "pSysMem"),
    (UINT, "SysMemPitch"),
    (UINT, "SysMemSlicePitch"),
])

D3D11_MAPPED_SUBRESOURCE = Struct("D3D11_MAPPED_SUBRESOURCE", [
    (LinearPointer(Void, "_MappedSize"), "pData"),
    (UINT, "RowPitch"),
    (UINT, "DepthPitch"),
])

ID3D11Resource.methods += [
    StdMethod(Void, "GetType", [Out(Pointer(D3D11_RESOURCE_DIMENSION), "pResourceDimension")], sideeffects=False),
    StdMethod(Void, "SetEvictionPriority", [(UINT, "EvictionPriority")]),
    StdMethod(UINT, "GetEvictionPriority", [], sideeffects=False),
]

D3D11_BUFFER_DESC = Struct("D3D11_BUFFER_DESC", [
    (UINT, "ByteWidth"),
    (D3D11_USAGE, "Usage"),
    (D3D11_BIND_FLAG, "BindFlags"),
    (D3D11_CPU_ACCESS_FLAG, "CPUAccessFlags"),
    (D3D11_RESOURCE_MISC_FLAG, "MiscFlags"),
    (UINT, "StructureByteStride"),
])

ID3D11Buffer.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_BUFFER_DESC), "pDesc")], sideeffects=False),
]

D3D11_TEXTURE1D_DESC = Struct("D3D11_TEXTURE1D_DESC", [
    (UINT, "Width"),
    (UINT, "MipLevels"),
    (UINT, "ArraySize"),
    (DXGI_FORMAT, "Format"),
    (D3D11_USAGE, "Usage"),
    (D3D11_BIND_FLAG, "BindFlags"),
    (D3D11_CPU_ACCESS_FLAG, "CPUAccessFlags"),
    (D3D11_RESOURCE_MISC_FLAG, "MiscFlags"),
])

ID3D11Texture1D.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_TEXTURE1D_DESC), "pDesc")], sideeffects=False),
]

D3D11_TEXTURE2D_DESC = Struct("D3D11_TEXTURE2D_DESC", [
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "MipLevels"),
    (UINT, "ArraySize"),
    (DXGI_FORMAT, "Format"),
    (DXGI_SAMPLE_DESC, "SampleDesc"),
    (D3D11_USAGE, "Usage"),
    (D3D11_BIND_FLAG, "BindFlags"),
    (D3D11_CPU_ACCESS_FLAG, "CPUAccessFlags"),
    (D3D11_RESOURCE_MISC_FLAG, "MiscFlags"),
])

ID3D11Texture2D.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_TEXTURE2D_DESC), "pDesc")], sideeffects=False),
]

D3D11_TEXTURE3D_DESC = Struct("D3D11_TEXTURE3D_DESC", [
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "Depth"),
    (UINT, "MipLevels"),
    (DXGI_FORMAT, "Format"),
    (D3D11_USAGE, "Usage"),
    (D3D11_BIND_FLAG, "BindFlags"),
    (D3D11_CPU_ACCESS_FLAG, "CPUAccessFlags"),
    (D3D11_RESOURCE_MISC_FLAG, "MiscFlags"),
])

ID3D11Texture3D.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_TEXTURE3D_DESC), "pDesc")], sideeffects=False),
]

D3D11_TEXTURECUBE_FACE = Enum("D3D11_TEXTURECUBE_FACE", [
    "D3D11_TEXTURECUBE_FACE_POSITIVE_X",
    "D3D11_TEXTURECUBE_FACE_NEGATIVE_X",
    "D3D11_TEXTURECUBE_FACE_POSITIVE_Y",
    "D3D11_TEXTURECUBE_FACE_NEGATIVE_Y",
    "D3D11_TEXTURECUBE_FACE_POSITIVE_Z",
    "D3D11_TEXTURECUBE_FACE_NEGATIVE_Z",
])

ID3D11View.methods += [
    StdMethod(Void, "GetResource", [Out(Pointer(ObjPointer(ID3D11Resource)), "ppResource")]),
]

D3D11_BUFFER_SRV = Struct("D3D11_BUFFER_SRV", [
    (UINT, "FirstElement"),
    (UINT, "NumElements"),
])

D3D11_BUFFEREX_SRV_FLAG = Flags(UINT, [
    "D3D11_BUFFEREX_SRV_FLAG_RAW",
])

D3D11_BUFFEREX_SRV = Struct("D3D11_BUFFEREX_SRV", [
    (UINT, "FirstElement"),
    (UINT, "NumElements"),
    (D3D11_BUFFEREX_SRV_FLAG, "Flags"),
])

D3D11_TEX1D_SRV = Struct("D3D11_TEX1D_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D11_TEX1D_ARRAY_SRV = Struct("D3D11_TEX1D_ARRAY_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX2D_SRV = Struct("D3D11_TEX2D_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D11_TEX2D_ARRAY_SRV = Struct("D3D11_TEX2D_ARRAY_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX3D_SRV = Struct("D3D11_TEX3D_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D11_TEXCUBE_SRV = Struct("D3D11_TEXCUBE_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D11_TEXCUBE_ARRAY_SRV = Struct("D3D11_TEXCUBE_ARRAY_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
    (UINT, "First2DArrayFace"),
    (UINT, "NumCubes"),
])

D3D11_TEX2DMS_SRV = Struct("D3D11_TEX2DMS_SRV", [
    (UINT, "UnusedField_NothingToDefine"),
])

D3D11_TEX2DMS_ARRAY_SRV = Struct("D3D11_TEX2DMS_ARRAY_SRV", [
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_SHADER_RESOURCE_VIEW_DESC = Struct("D3D11_SHADER_RESOURCE_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D11_SRV_DIMENSION, "ViewDimension"),
    (Union("{self}.ViewDimension", [
        ("D3D11_SRV_DIMENSION_BUFFER", D3D11_BUFFER_SRV, "Buffer"),
        ("D3D11_SRV_DIMENSION_TEXTURE1D", D3D11_TEX1D_SRV, "Texture1D"),
        ("D3D11_SRV_DIMENSION_TEXTURE1DARRAY", D3D11_TEX1D_ARRAY_SRV, "Texture1DArray"),
        ("D3D11_SRV_DIMENSION_TEXTURE2D", D3D11_TEX2D_SRV, "Texture2D"), 
        ("D3D11_SRV_DIMENSION_TEXTURE2DARRAY", D3D11_TEX2D_ARRAY_SRV, "Texture2DArray"),
        ("D3D11_SRV_DIMENSION_TEXTURE2DMS", D3D11_TEX2DMS_SRV, "Texture2DMS"),
        ("D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY", D3D11_TEX2DMS_ARRAY_SRV, "Texture2DMSArray"),
        ("D3D11_SRV_DIMENSION_TEXTURE3D", D3D11_TEX3D_SRV, "Texture3D"),
        ("D3D11_SRV_DIMENSION_TEXTURECUBE", D3D11_TEXCUBE_SRV, "TextureCube"),
        ("D3D11_SRV_DIMENSION_TEXTURECUBEARRAY", D3D11_TEXCUBE_ARRAY_SRV, "TextureCubeArray"),
        ("D3D11_SRV_DIMENSION_BUFFEREX", D3D11_BUFFEREX_SRV, "BufferEx"),
    ]), None),
])

ID3D11ShaderResourceView.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_SHADER_RESOURCE_VIEW_DESC), "pDesc")], sideeffects=False),
]

D3D11_BUFFER_RTV = Struct("D3D11_BUFFER_RTV", [
    (UINT, "FirstElement"),
    (UINT, "NumElements"),
])

D3D11_TEX1D_RTV = Struct("D3D11_TEX1D_RTV", [
    (UINT, "MipSlice"),
])

D3D11_TEX1D_ARRAY_RTV = Struct("D3D11_TEX1D_ARRAY_RTV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX2D_RTV = Struct("D3D11_TEX2D_RTV", [
    (UINT, "MipSlice"),
])

D3D11_TEX2DMS_RTV = Struct("D3D11_TEX2DMS_RTV", [
    (UINT, "UnusedField_NothingToDefine"),
])

D3D11_TEX2D_ARRAY_RTV = Struct("D3D11_TEX2D_ARRAY_RTV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX2DMS_ARRAY_RTV = Struct("D3D11_TEX2DMS_ARRAY_RTV", [
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX3D_RTV = Struct("D3D11_TEX3D_RTV", [
    (UINT, "MipSlice"),
    (UINT, "FirstWSlice"),
    (UINT, "WSize"),
])

D3D11_RENDER_TARGET_VIEW_DESC = Struct("D3D11_RENDER_TARGET_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D11_RTV_DIMENSION, "ViewDimension"),
    (Union("{self}.ViewDimension", [
        ("D3D11_RTV_DIMENSION_BUFFER", D3D11_BUFFER_RTV, "Buffer"),
        ("D3D11_RTV_DIMENSION_TEXTURE1D", D3D11_TEX1D_RTV, "Texture1D"),
        ("D3D11_RTV_DIMENSION_TEXTURE1DARRAY", D3D11_TEX1D_ARRAY_RTV, "Texture1DArray"),
        ("D3D11_RTV_DIMENSION_TEXTURE2D", D3D11_TEX2D_RTV, "Texture2D"),
        ("D3D11_RTV_DIMENSION_TEXTURE2DARRAY", D3D11_TEX2D_ARRAY_RTV, "Texture2DArray"),
        ("D3D11_RTV_DIMENSION_TEXTURE2DMS", D3D11_TEX2DMS_RTV, "Texture2DMS"),
        ("D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY", D3D11_TEX2DMS_ARRAY_RTV, "Texture2DMSArray"),
        ("D3D11_RTV_DIMENSION_TEXTURE3D", D3D11_TEX3D_RTV, "Texture3D"),
    ]), None),
])

ID3D11RenderTargetView.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_RENDER_TARGET_VIEW_DESC), "pDesc")], sideeffects=False),
]

D3D11_TEX1D_DSV = Struct("D3D11_TEX1D_DSV", [
    (UINT, "MipSlice"),
])

D3D11_TEX1D_ARRAY_DSV = Struct("D3D11_TEX1D_ARRAY_DSV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX2D_DSV = Struct("D3D11_TEX2D_DSV", [
    (UINT, "MipSlice"),
])

D3D11_TEX2D_ARRAY_DSV = Struct("D3D11_TEX2D_ARRAY_DSV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX2DMS_DSV = Struct("D3D11_TEX2DMS_DSV", [
    (UINT, "UnusedField_NothingToDefine"),
])

D3D11_TEX2DMS_ARRAY_DSV = Struct("D3D11_TEX2DMS_ARRAY_DSV", [
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_DSV_FLAG = Flags(UINT, [
    "D3D11_DSV_READ_ONLY_DEPTH",
    "D3D11_DSV_READ_ONLY_STENCIL",
])

D3D11_DEPTH_STENCIL_VIEW_DESC = Struct("D3D11_DEPTH_STENCIL_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D11_DSV_DIMENSION, "ViewDimension"),
    (D3D11_DSV_FLAG, "Flags"),
    (Union("{self}.ViewDimension", [
        ("D3D11_DSV_DIMENSION_TEXTURE1D", D3D11_TEX1D_DSV, "Texture1D"),
        ("D3D11_DSV_DIMENSION_TEXTURE1DARRAY", D3D11_TEX1D_ARRAY_DSV, "Texture1DArray"),
        ("D3D11_DSV_DIMENSION_TEXTURE2D", D3D11_TEX2D_DSV, "Texture2D"),
        ("D3D11_DSV_DIMENSION_TEXTURE2DARRAY", D3D11_TEX2D_ARRAY_DSV, "Texture2DArray"),
        ("D3D11_DSV_DIMENSION_TEXTURE2DMS", D3D11_TEX2DMS_DSV, "Texture2DMS"),
        ("D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY", D3D11_TEX2DMS_ARRAY_DSV, "Texture2DMSArray"),
    ]), None),
])

ID3D11DepthStencilView.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_DEPTH_STENCIL_VIEW_DESC), "pDesc")], sideeffects=False),
]

D3D11_BUFFER_UAV_FLAG = Flags(UINT, [
    "D3D11_BUFFER_UAV_FLAG_RAW",
    "D3D11_BUFFER_UAV_FLAG_APPEND",
    "D3D11_BUFFER_UAV_FLAG_COUNTER",
])

D3D11_BUFFER_UAV = Struct("D3D11_BUFFER_UAV", [
    (UINT, "FirstElement"),
    (UINT, "NumElements"),
    (D3D11_BUFFER_UAV_FLAG, "Flags"),
])

D3D11_TEX1D_UAV = Struct("D3D11_TEX1D_UAV", [
    (UINT, "MipSlice"),
])

D3D11_TEX1D_ARRAY_UAV = Struct("D3D11_TEX1D_ARRAY_UAV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX2D_UAV = Struct("D3D11_TEX2D_UAV", [
    (UINT, "MipSlice"),
])

D3D11_TEX2D_ARRAY_UAV = Struct("D3D11_TEX2D_ARRAY_UAV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D11_TEX3D_UAV = Struct("D3D11_TEX3D_UAV", [
    (UINT, "MipSlice"),
    (UINT, "FirstWSlice"),
    (UINT, "WSize"),
])

D3D11_UNORDERED_ACCESS_VIEW_DESC = Struct("D3D11_UNORDERED_ACCESS_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D11_UAV_DIMENSION, "ViewDimension"),
    (Union("{self}.ViewDimension", [
        ("D3D11_UAV_DIMENSION_BUFFER", D3D11_BUFFER_UAV, "Buffer"),
        ("D3D11_UAV_DIMENSION_TEXTURE1D", D3D11_TEX1D_UAV, "Texture1D"),
        ("D3D11_UAV_DIMENSION_TEXTURE1DARRAY", D3D11_TEX1D_ARRAY_UAV, "Texture1DArray"),
        ("D3D11_UAV_DIMENSION_TEXTURE2D", D3D11_TEX2D_UAV, "Texture2D"),
        ("D3D11_UAV_DIMENSION_TEXTURE2DARRAY", D3D11_TEX2D_ARRAY_UAV, "Texture2DArray"),
        ("D3D11_UAV_DIMENSION_TEXTURE3D", D3D11_TEX3D_UAV, "Texture3D"),
    ]), None),
])

ID3D11UnorderedAccessView.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_UNORDERED_ACCESS_VIEW_DESC), "pDesc")], sideeffects=False),
]

D3D11_FILTER = Enum("D3D11_FILTER", [
    "D3D11_FILTER_MIN_MAG_MIP_POINT",
    "D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR",
    "D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT",
    "D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR",
    "D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT",
    "D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR",
    "D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT",
    "D3D11_FILTER_MIN_MAG_MIP_LINEAR",
    "D3D11_FILTER_ANISOTROPIC",
    "D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT",
    "D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR",
    "D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT",
    "D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR",
    "D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT",
    "D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR",
    "D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT",
    "D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR",
    "D3D11_FILTER_COMPARISON_ANISOTROPIC",
])

D3D11_FILTER_TYPE = Enum("D3D11_FILTER_TYPE", [
    "D3D11_FILTER_TYPE_POINT",
    "D3D11_FILTER_TYPE_LINEAR",
])

D3D11_TEXTURE_ADDRESS_MODE = Enum("D3D11_TEXTURE_ADDRESS_MODE", [
    "D3D11_TEXTURE_ADDRESS_WRAP",
    "D3D11_TEXTURE_ADDRESS_MIRROR",
    "D3D11_TEXTURE_ADDRESS_CLAMP",
    "D3D11_TEXTURE_ADDRESS_BORDER",
    "D3D11_TEXTURE_ADDRESS_MIRROR_ONCE",
])

D3D11_SAMPLER_DESC = Struct("D3D11_SAMPLER_DESC", [
    (D3D11_FILTER, "Filter"),
    (D3D11_TEXTURE_ADDRESS_MODE, "AddressU"),
    (D3D11_TEXTURE_ADDRESS_MODE, "AddressV"),
    (D3D11_TEXTURE_ADDRESS_MODE, "AddressW"),
    (FLOAT, "MipLODBias"),
    (UINT, "MaxAnisotropy"),
    (D3D11_COMPARISON_FUNC, "ComparisonFunc"),
    (Array(FLOAT, 4), "BorderColor"),
    (FLOAT, "MinLOD"),
    (FLOAT, "MaxLOD"),
])

ID3D11SamplerState.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_SAMPLER_DESC), "pDesc")], sideeffects=False),
]

D3D11_FORMAT_SUPPORT = Flags(UINT, [
    "D3D11_FORMAT_SUPPORT_BUFFER",
    "D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER",
    "D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER",
    "D3D11_FORMAT_SUPPORT_SO_BUFFER",
    "D3D11_FORMAT_SUPPORT_TEXTURE1D",
    "D3D11_FORMAT_SUPPORT_TEXTURE2D",
    "D3D11_FORMAT_SUPPORT_TEXTURE3D",
    "D3D11_FORMAT_SUPPORT_TEXTURECUBE",
    "D3D11_FORMAT_SUPPORT_SHADER_LOAD",
    "D3D11_FORMAT_SUPPORT_SHADER_SAMPLE",
    "D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON",
    "D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT",
    "D3D11_FORMAT_SUPPORT_MIP",
    "D3D11_FORMAT_SUPPORT_MIP_AUTOGEN",
    "D3D11_FORMAT_SUPPORT_RENDER_TARGET",
    "D3D11_FORMAT_SUPPORT_BLENDABLE",
    "D3D11_FORMAT_SUPPORT_DEPTH_STENCIL",
    "D3D11_FORMAT_SUPPORT_CPU_LOCKABLE",
    "D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE",
    "D3D11_FORMAT_SUPPORT_DISPLAY",
    "D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT",
    "D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET",
    "D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD",
    "D3D11_FORMAT_SUPPORT_SHADER_GATHER",
    "D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST",
    "D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW",
    "D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON",
])

D3D11_FORMAT_SUPPORT2 = Enum("D3D11_FORMAT_SUPPORT2", [
    "D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_ADD",
    "D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_BITWISE_OPS",
    "D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_COMPARE_STORE_OR_COMPARE_EXCHANGE",
    "D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE",
    "D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_SIGNED_MIN_OR_MAX",
    "D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_UNSIGNED_MIN_OR_MAX",
    "D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD",
    "D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE",
])

ID3D11Asynchronous.methods += [
    StdMethod(UINT, "GetDataSize", [], sideeffects=False),
]

D3D11_ASYNC_GETDATA_FLAG = Flags(UINT, [
    "D3D11_ASYNC_GETDATA_DONOTFLUSH",
])

D3D11_QUERY = Enum("D3D11_QUERY", [
    "D3D11_QUERY_EVENT",
    "D3D11_QUERY_OCCLUSION",
    "D3D11_QUERY_TIMESTAMP",
    "D3D11_QUERY_TIMESTAMP_DISJOINT",
    "D3D11_QUERY_PIPELINE_STATISTICS",
    "D3D11_QUERY_OCCLUSION_PREDICATE",
    "D3D11_QUERY_SO_STATISTICS",
    "D3D11_QUERY_SO_OVERFLOW_PREDICATE",
    "D3D11_QUERY_SO_STATISTICS_STREAM0",
    "D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0",
    "D3D11_QUERY_SO_STATISTICS_STREAM1",
    "D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1",
    "D3D11_QUERY_SO_STATISTICS_STREAM2",
    "D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2",
    "D3D11_QUERY_SO_STATISTICS_STREAM3",
    "D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM3",
])

D3D11_QUERY_MISC_FLAG = Flags(UINT, [
    "D3D11_QUERY_MISC_PREDICATEHINT",
])

D3D11_QUERY_DESC = Struct("D3D11_QUERY_DESC", [
    (D3D11_QUERY, "Query"),
    (D3D11_QUERY_MISC_FLAG, "MiscFlags"),
])

ID3D11Query.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_QUERY_DESC), "pDesc")], sideeffects=False),
]

D3D11_QUERY_DATA_TIMESTAMP_DISJOINT = Struct("D3D11_QUERY_DATA_TIMESTAMP_DISJOINT", [
    (UINT64, "Frequency"),
    (BOOL, "Disjoint"),
])

D3D11_QUERY_DATA_PIPELINE_STATISTICS = Struct("D3D11_QUERY_DATA_PIPELINE_STATISTICS", [
    (UINT64, "IAVertices"),
    (UINT64, "IAPrimitives"),
    (UINT64, "VSInvocations"),
    (UINT64, "GSInvocations"),
    (UINT64, "GSPrimitives"),
    (UINT64, "CInvocations"),
    (UINT64, "CPrimitives"),
    (UINT64, "PSInvocations"),
    (UINT64, "HSInvocations"),
    (UINT64, "DSInvocations"),
    (UINT64, "CSInvocations"),
])

D3D11_QUERY_DATA_SO_STATISTICS = Struct("D3D11_QUERY_DATA_SO_STATISTICS", [
    (UINT64, "NumPrimitivesWritten"),
    (UINT64, "PrimitivesStorageNeeded"),
])

D3D11_COUNTER = Enum("D3D11_COUNTER", [
    "D3D11_COUNTER_DEVICE_DEPENDENT_0",
])

D3D11_COUNTER_TYPE = Enum("D3D11_COUNTER_TYPE", [
    "D3D11_COUNTER_TYPE_FLOAT32",
    "D3D11_COUNTER_TYPE_UINT16",
    "D3D11_COUNTER_TYPE_UINT32",
    "D3D11_COUNTER_TYPE_UINT64",
])

D3D11_COUNTER_DESC = Struct("D3D11_COUNTER_DESC", [
    (D3D11_COUNTER, "Counter"),
    (UINT, "MiscFlags"),
])

D3D11_COUNTER_INFO = Struct("D3D11_COUNTER_INFO", [
    (D3D11_COUNTER, "LastDeviceDependentCounter"),
    (UINT, "NumSimultaneousCounters"),
    (UINT8, "NumDetectableParallelUnits"),
])

ID3D11Counter.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_COUNTER_DESC), "pDesc")], sideeffects=False),
]

D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS = Enum("D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS", [
    "D3D11_STANDARD_MULTISAMPLE_PATTERN",
    "D3D11_CENTER_MULTISAMPLE_PATTERN",
])

D3D11_DEVICE_CONTEXT_TYPE = Enum("D3D11_DEVICE_CONTEXT_TYPE", [
    "D3D11_DEVICE_CONTEXT_IMMEDIATE",
    "D3D11_DEVICE_CONTEXT_DEFERRED",
])

D3D11_CLASS_INSTANCE_DESC = Struct("D3D11_CLASS_INSTANCE_DESC", [
    (UINT, "InstanceId"),
    (UINT, "InstanceIndex"),
    (UINT, "TypeId"),
    (UINT, "ConstantBuffer"),
    (UINT, "BaseConstantBufferOffset"),
    (UINT, "BaseTexture"),
    (UINT, "BaseSampler"),
    (BOOL, "Created"),
])


ID3D11ClassInstance.methods += [
    StdMethod(Void, "GetClassLinkage", [Out(Pointer(ObjPointer(ID3D11ClassLinkage)), "ppLinkage")]),
    StdMethod(Void, "GetDesc", [Out(Pointer(D3D11_CLASS_INSTANCE_DESC), "pDesc")], sideeffects=False),
    StdMethod(Void, "GetInstanceName", [Out(LPSTR, "pInstanceName"), Out(Pointer(SIZE_T), "pBufferLength")], sideeffects=False),
    StdMethod(Void, "GetTypeName", [Out(LPSTR, "pTypeName"), Out(Pointer(SIZE_T), "pBufferLength")], sideeffects=False),
]

ID3D11ClassLinkage.methods += [
    StdMethod(HRESULT, "GetClassInstance", [(LPCSTR, "pClassInstanceName"), (UINT, "InstanceIndex"), Out(Pointer(ObjPointer(ID3D11ClassInstance)), "ppInstance")]),
    StdMethod(HRESULT, "CreateClassInstance", [(LPCSTR, "pClassTypeName"), (UINT, "ConstantBufferOffset"), (UINT, "ConstantVectorOffset"), (UINT, "TextureOffset"), (UINT, "SamplerOffset"), Out(Pointer(ObjPointer(ID3D11ClassInstance)), "ppInstance")]),
]

ID3D11CommandList.methods += [
    StdMethod(UINT, "GetContextFlags", [], sideeffects=False),
]

D3D11_FEATURE_DATA_THREADING = Struct("D3D11_FEATURE_DATA_THREADING", [
    (BOOL, "DriverConcurrentCreates"),
    (BOOL, "DriverCommandLists"),
])

D3D11_FEATURE_DATA_DOUBLES = Struct("D3D11_FEATURE_DATA_DOUBLES", [
    (BOOL, "DoublePrecisionFloatShaderOps"),
])

D3D11_FEATURE_DATA_FORMAT_SUPPORT = Struct("D3D11_FEATURE_DATA_FORMAT_SUPPORT", [
    (DXGI_FORMAT, "InFormat"),
    (D3D11_FORMAT_SUPPORT, "OutFormatSupport"),
])

D3D11_FEATURE_DATA_FORMAT_SUPPORT2 = Struct("D3D11_FEATURE_DATA_FORMAT_SUPPORT2", [
    (DXGI_FORMAT, "InFormat"),
    (D3D11_FORMAT_SUPPORT2, "OutFormatSupport2"),
])

D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS = Struct("D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS", [
    (BOOL, "ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x"),
])

D3D11_FEATURE, D3D11_FEATURE_DATA = EnumPolymorphic("D3D11_FEATURE", "Feature", [
    ("D3D11_FEATURE_THREADING", Pointer(D3D11_FEATURE_DATA_THREADING)),
    ("D3D11_FEATURE_DOUBLES", Pointer(D3D11_FEATURE_DATA_DOUBLES)),
    ("D3D11_FEATURE_FORMAT_SUPPORT", Pointer(D3D11_FEATURE_DATA_FORMAT_SUPPORT)),
    ("D3D11_FEATURE_FORMAT_SUPPORT2", Pointer(D3D11_FEATURE_DATA_FORMAT_SUPPORT2)),
    ("D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS", Pointer(D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS)),
], Blob(Void, "FeatureSupportDataSize"), False)

ID3D11DeviceContext.methods += [
    StdMethod(Void, "VSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "PSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "PSSetShader", [(ObjPointer(ID3D11PixelShader), "pPixelShader"), (Array(Const(ObjPointer(ID3D11ClassInstance)), "NumClassInstances"), "ppClassInstances"), (UINT, "NumClassInstances")]),
    StdMethod(Void, "PSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(ObjPointer(ID3D11SamplerState)), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "VSSetShader", [(ObjPointer(ID3D11VertexShader), "pVertexShader"), (Array(Const(ObjPointer(ID3D11ClassInstance)), "NumClassInstances"), "ppClassInstances"), (UINT, "NumClassInstances")]),
    StdMethod(Void, "DrawIndexed", [(UINT, "IndexCount"), (UINT, "StartIndexLocation"), (INT, "BaseVertexLocation")]),
    StdMethod(Void, "Draw", [(UINT, "VertexCount"), (UINT, "StartVertexLocation")]),
    StdMethod(HRESULT, "Map", [(ObjPointer(ID3D11Resource), "pResource"), (UINT, "Subresource"), (D3D11_MAP, "MapType"), (D3D11_MAP_FLAG, "MapFlags"), Out(Pointer(D3D11_MAPPED_SUBRESOURCE), "pMappedResource")]),
    StdMethod(Void, "Unmap", [(ObjPointer(ID3D11Resource), "pResource"), (UINT, "Subresource")]),
    StdMethod(Void, "PSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "IASetInputLayout", [(ObjPointer(ID3D11InputLayout), "pInputLayout")]),
    StdMethod(Void, "IASetVertexBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppVertexBuffers"), (Array(Const(UINT), "NumBuffers"), "pStrides"), (Array(Const(UINT), "NumBuffers"), "pOffsets")]),
    StdMethod(Void, "IASetIndexBuffer", [(ObjPointer(ID3D11Buffer), "pIndexBuffer"), (DXGI_FORMAT, "Format"), (UINT, "Offset")]),
    StdMethod(Void, "DrawIndexedInstanced", [(UINT, "IndexCountPerInstance"), (UINT, "InstanceCount"), (UINT, "StartIndexLocation"), (INT, "BaseVertexLocation"), (UINT, "StartInstanceLocation")]),
    StdMethod(Void, "DrawInstanced", [(UINT, "VertexCountPerInstance"), (UINT, "InstanceCount"), (UINT, "StartVertexLocation"), (UINT, "StartInstanceLocation")]),
    StdMethod(Void, "GSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "GSSetShader", [(ObjPointer(ID3D11GeometryShader), "pShader"), (Array(Const(ObjPointer(ID3D11ClassInstance)), "NumClassInstances"), "ppClassInstances"), (UINT, "NumClassInstances")]),
    StdMethod(Void, "IASetPrimitiveTopology", [(D3D11_PRIMITIVE_TOPOLOGY, "Topology")]),
    StdMethod(Void, "VSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "VSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(ObjPointer(ID3D11SamplerState)), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "Begin", [(ObjPointer(ID3D11Asynchronous), "pAsync")]),
    StdMethod(Void, "End", [(ObjPointer(ID3D11Asynchronous), "pAsync")]),
    StdMethod(HRESULT, "GetData", [(ObjPointer(ID3D11Asynchronous), "pAsync"), Out(OpaqueBlob(Void, "DataSize"), "pData"), (UINT, "DataSize"), (D3D11_ASYNC_GETDATA_FLAG, "GetDataFlags")], sideeffects=False),
    StdMethod(Void, "SetPredication", [(ObjPointer(ID3D11Predicate), "pPredicate"), (BOOL, "PredicateValue")]),
    StdMethod(Void, "GSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "GSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(ObjPointer(ID3D11SamplerState)), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "OMSetRenderTargets", [(UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11RenderTargetView)), "NumViews"), "ppRenderTargetViews"), (ObjPointer(ID3D11DepthStencilView), "pDepthStencilView")]),
    StdMethod(Void, "OMSetRenderTargetsAndUnorderedAccessViews", [(UINT, "NumRTVs"), (Array(Const(ObjPointer(ID3D11RenderTargetView)), "NumRTVs"), "ppRenderTargetViews"), (ObjPointer(ID3D11DepthStencilView), "pDepthStencilView"), (UINT, "UAVStartSlot"), (UINT, "NumUAVs"), (Array(Const(ObjPointer(ID3D11UnorderedAccessView)), "NumUAVs"), "ppUnorderedAccessViews"), (Array(Const(UINT), "NumUAVs"), "pUAVInitialCounts")]),
    StdMethod(Void, "OMSetBlendState", [(ObjPointer(ID3D11BlendState), "pBlendState"), (Array(Const(FLOAT), 4), "BlendFactor"), (UINT, "SampleMask")]),
    StdMethod(Void, "OMSetDepthStencilState", [(ObjPointer(ID3D11DepthStencilState), "pDepthStencilState"), (UINT, "StencilRef")]),
    StdMethod(Void, "SOSetTargets", [(UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppSOTargets"), (Array(Const(UINT), "NumBuffers"), "pOffsets")]),
    StdMethod(Void, "DrawAuto", []),
    StdMethod(Void, "DrawIndexedInstancedIndirect", [(ObjPointer(ID3D11Buffer), "pBufferForArgs"), (UINT, "AlignedByteOffsetForArgs")]),
    StdMethod(Void, "DrawInstancedIndirect", [(ObjPointer(ID3D11Buffer), "pBufferForArgs"), (UINT, "AlignedByteOffsetForArgs")]),
    StdMethod(Void, "Dispatch", [(UINT, "ThreadGroupCountX"), (UINT, "ThreadGroupCountY"), (UINT, "ThreadGroupCountZ")]),
    StdMethod(Void, "DispatchIndirect", [(ObjPointer(ID3D11Buffer), "pBufferForArgs"), (UINT, "AlignedByteOffsetForArgs")]),
    StdMethod(Void, "RSSetState", [(ObjPointer(ID3D11RasterizerState), "pRasterizerState")]),
    StdMethod(Void, "RSSetViewports", [(UINT, "NumViewports"), (Array(Const(D3D11_VIEWPORT), "NumViewports"), "pViewports")]),
    StdMethod(Void, "RSSetScissorRects", [(UINT, "NumRects"), (Array(Const(D3D11_RECT), "NumRects"), "pRects")]),
    StdMethod(Void, "CopySubresourceRegion", [(ObjPointer(ID3D11Resource), "pDstResource"), (UINT, "DstSubresource"), (UINT, "DstX"), (UINT, "DstY"), (UINT, "DstZ"), (ObjPointer(ID3D11Resource), "pSrcResource"), (UINT, "SrcSubresource"), (Pointer(Const(D3D11_BOX)), "pSrcBox")]),
    StdMethod(Void, "CopyResource", [(ObjPointer(ID3D11Resource), "pDstResource"), (ObjPointer(ID3D11Resource), "pSrcResource")]),
    StdMethod(Void, "UpdateSubresource", [(ObjPointer(ID3D11Resource), "pDstResource"), (UINT, "DstSubresource"), (Pointer(Const(D3D11_BOX)), "pDstBox"), (Blob(Const(Void), "_calcSubresourceSize(pDstResource, DstSubresource, pDstBox, SrcRowPitch, SrcDepthPitch)"), "pSrcData"), (UINT, "SrcRowPitch"), (UINT, "SrcDepthPitch")]),
    StdMethod(Void, "CopyStructureCount", [(ObjPointer(ID3D11Buffer), "pDstBuffer"), (UINT, "DstAlignedByteOffset"), (ObjPointer(ID3D11UnorderedAccessView), "pSrcView")]),
    StdMethod(Void, "ClearRenderTargetView", [(ObjPointer(ID3D11RenderTargetView), "pRenderTargetView"), (Array(Const(FLOAT), 4), "ColorRGBA")]),
    StdMethod(Void, "ClearUnorderedAccessViewUint", [(ObjPointer(ID3D11UnorderedAccessView), "pUnorderedAccessView"), (Array(Const(UINT), 4), "Values")]),
    StdMethod(Void, "ClearUnorderedAccessViewFloat", [(ObjPointer(ID3D11UnorderedAccessView), "pUnorderedAccessView"), (Array(Const(FLOAT), 4), "Values")]),
    StdMethod(Void, "ClearDepthStencilView", [(ObjPointer(ID3D11DepthStencilView), "pDepthStencilView"), (D3D11_CLEAR_FLAG, "ClearFlags"), (FLOAT, "Depth"), (UINT8, "Stencil")]),
    StdMethod(Void, "GenerateMips", [(ObjPointer(ID3D11ShaderResourceView), "pShaderResourceView")]),
    StdMethod(Void, "SetResourceMinLOD", [(ObjPointer(ID3D11Resource), "pResource"), (FLOAT, "MinLOD")]),
    StdMethod(FLOAT, "GetResourceMinLOD", [(ObjPointer(ID3D11Resource), "pResource")], sideeffects=False),
    StdMethod(Void, "ResolveSubresource", [(ObjPointer(ID3D11Resource), "pDstResource"), (UINT, "DstSubresource"), (ObjPointer(ID3D11Resource), "pSrcResource"), (UINT, "SrcSubresource"), (DXGI_FORMAT, "Format")]),
    StdMethod(Void, "ExecuteCommandList", [(ObjPointer(ID3D11CommandList), "pCommandList"), (BOOL, "RestoreContextState")]),
    StdMethod(Void, "HSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "HSSetShader", [(ObjPointer(ID3D11HullShader), "pHullShader"), (Array(Const(ObjPointer(ID3D11ClassInstance)), "NumClassInstances"), "ppClassInstances"), (UINT, "NumClassInstances")]),
    StdMethod(Void, "HSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(ObjPointer(ID3D11SamplerState)), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "HSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "DSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "DSSetShader", [(ObjPointer(ID3D11DomainShader), "pDomainShader"), (Array(Const(ObjPointer(ID3D11ClassInstance)), "NumClassInstances"), "ppClassInstances"), (UINT, "NumClassInstances")]),
    StdMethod(Void, "DSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(ObjPointer(ID3D11SamplerState)), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "DSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "CSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(ObjPointer(ID3D11ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "CSSetUnorderedAccessViews", [(UINT, "StartSlot"), (UINT, "NumUAVs"), (Array(Const(ObjPointer(ID3D11UnorderedAccessView)), "NumUAVs"), "ppUnorderedAccessViews"), (Array(Const(UINT), "NumUAVs"), "pUAVInitialCounts")]),
    StdMethod(Void, "CSSetShader", [(ObjPointer(ID3D11ComputeShader), "pComputeShader"), (Array(Const(ObjPointer(ID3D11ClassInstance)), "NumClassInstances"), "ppClassInstances"), (UINT, "NumClassInstances")]),
    StdMethod(Void, "CSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(ObjPointer(ID3D11SamplerState)), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "CSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(ObjPointer(ID3D11Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "VSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "PSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(ObjPointer(ID3D11ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "PSGetShader", [Out(Pointer(ObjPointer(ID3D11PixelShader)), "ppPixelShader"), Out(Array(ObjPointer(ID3D11ClassInstance), "*pNumClassInstances"), "ppClassInstances"), Out(Pointer(UINT), "pNumClassInstances")]),
    StdMethod(Void, "PSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(ObjPointer(ID3D11SamplerState), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "VSGetShader", [Out(Pointer(ObjPointer(ID3D11VertexShader)), "ppVertexShader"), Out(Array(ObjPointer(ID3D11ClassInstance), "*pNumClassInstances"), "ppClassInstances"), Out(Pointer(UINT), "pNumClassInstances")]),
    StdMethod(Void, "PSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "IAGetInputLayout", [Out(Pointer(ObjPointer(ID3D11InputLayout)), "ppInputLayout")]),
    StdMethod(Void, "IAGetVertexBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppVertexBuffers"), Out(Array(UINT, "NumBuffers"), "pStrides"), Out(Array(UINT, "NumBuffers"), "pOffsets")]),
    StdMethod(Void, "IAGetIndexBuffer", [Out(Pointer(ObjPointer(ID3D11Buffer)), "pIndexBuffer"), Out(Pointer(DXGI_FORMAT), "Format"), Out(Pointer(UINT), "Offset")]),
    StdMethod(Void, "GSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "GSGetShader", [Out(Pointer(ObjPointer(ID3D11GeometryShader)), "ppGeometryShader"), Out(Array(ObjPointer(ID3D11ClassInstance), "*pNumClassInstances"), "ppClassInstances"), Out(Pointer(UINT), "pNumClassInstances")]),
    StdMethod(Void, "IAGetPrimitiveTopology", [Out(Pointer(D3D11_PRIMITIVE_TOPOLOGY), "pTopology")], sideeffects=False),
    StdMethod(Void, "VSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(ObjPointer(ID3D11ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "VSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(ObjPointer(ID3D11SamplerState), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "GetPredication", [Out(Pointer(ObjPointer(ID3D11Predicate)), "ppPredicate"), Out(Pointer(BOOL), "pPredicateValue")]),
    StdMethod(Void, "GSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(ObjPointer(ID3D11ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "GSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(ObjPointer(ID3D11SamplerState), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "OMGetRenderTargets", [(UINT, "NumViews"), Out(Array(ObjPointer(ID3D11RenderTargetView), "NumViews"), "ppRenderTargetViews"), Out(Pointer(ObjPointer(ID3D11DepthStencilView)), "ppDepthStencilView")]),
    StdMethod(Void, "OMGetRenderTargetsAndUnorderedAccessViews", [(UINT, "NumRTVs"), Out(Array(ObjPointer(ID3D11RenderTargetView), "NumRTVs"), "ppRenderTargetViews"), Out(Pointer(ObjPointer(ID3D11DepthStencilView)), "ppDepthStencilView"), (UINT, "UAVStartSlot"), (UINT, "NumUAVs"), Out(Array(ObjPointer(ID3D11UnorderedAccessView), "NumUAVs"), "ppUnorderedAccessViews")]),
    StdMethod(Void, "OMGetBlendState", [Out(Pointer(ObjPointer(ID3D11BlendState)), "ppBlendState"), Out(Array(FLOAT, 4), "BlendFactor"), Out(Pointer(UINT), "pSampleMask")]),
    StdMethod(Void, "OMGetDepthStencilState", [Out(Pointer(ObjPointer(ID3D11DepthStencilState)), "ppDepthStencilState"), Out(Pointer(UINT), "pStencilRef")]),
    StdMethod(Void, "SOGetTargets", [(UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppSOTargets")]),
    StdMethod(Void, "RSGetState", [Out(Pointer(ObjPointer(ID3D11RasterizerState)), "ppRasterizerState")]),
    StdMethod(Void, "RSGetViewports", [Out(Pointer(UINT), "pNumViewports"), Out(Array(D3D11_VIEWPORT, "*pNumViewports"), "pViewports")]),
    StdMethod(Void, "RSGetScissorRects", [Out(Pointer(UINT), "pNumRects"), Out(Array(D3D11_RECT, "*pNumRects"), "pRects")]),
    StdMethod(Void, "HSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(ObjPointer(ID3D11ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "HSGetShader", [Out(Pointer(ObjPointer(ID3D11HullShader)), "ppHullShader"), Out(Array(ObjPointer(ID3D11ClassInstance), "*pNumClassInstances"), "ppClassInstances"), Out(Pointer(UINT), "pNumClassInstances")]),
    StdMethod(Void, "HSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(ObjPointer(ID3D11SamplerState), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "HSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "DSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(ObjPointer(ID3D11ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "DSGetShader", [Out(Pointer(ObjPointer(ID3D11DomainShader)), "ppDomainShader"), Out(Array(ObjPointer(ID3D11ClassInstance), "*pNumClassInstances"), "ppClassInstances"), Out(Pointer(UINT), "pNumClassInstances")]),
    StdMethod(Void, "DSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(ObjPointer(ID3D11SamplerState), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "DSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "CSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(ObjPointer(ID3D11ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    StdMethod(Void, "CSGetUnorderedAccessViews", [(UINT, "StartSlot"), (UINT, "NumUAVs"), Out(Array(ObjPointer(ID3D11UnorderedAccessView), "NumUAVs"), "ppUnorderedAccessViews")]),
    StdMethod(Void, "CSGetShader", [Out(Pointer(ObjPointer(ID3D11ComputeShader)), "ppComputeShader"), Out(Array(ObjPointer(ID3D11ClassInstance), "*pNumClassInstances"), "ppClassInstances"), Out(Pointer(UINT), "pNumClassInstances")]),
    StdMethod(Void, "CSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(ObjPointer(ID3D11SamplerState), "NumSamplers"), "ppSamplers")]),
    StdMethod(Void, "CSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(ObjPointer(ID3D11Buffer), "NumBuffers"), "ppConstantBuffers")]),
    StdMethod(Void, "ClearState", []),
    StdMethod(Void, "Flush", []),
    StdMethod(D3D11_DEVICE_CONTEXT_TYPE, "GetType", [], sideeffects=False),
    StdMethod(UINT, "GetContextFlags", [], sideeffects=False),
    StdMethod(HRESULT, "FinishCommandList", [(BOOL, "RestoreDeferredContextState"), Out(Pointer(ObjPointer(ID3D11CommandList)), "ppCommandList")]),
]

D3D11_CREATE_DEVICE_FLAG = Flags(UINT, [
    "D3D11_CREATE_DEVICE_SINGLETHREADED",
    "D3D11_CREATE_DEVICE_DEBUG",
    "D3D11_CREATE_DEVICE_SWITCH_TO_REF",
    "D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS",
    "D3D11_CREATE_DEVICE_BGRA_SUPPORT",
])

ID3D11Device.methods += [
    StdMethod(HRESULT, "CreateBuffer", [(Pointer(Const(D3D11_BUFFER_DESC)), "pDesc"), (Array(Const(D3D11_SUBRESOURCE_DATA), "1"), "pInitialData"), Out(Pointer(ObjPointer(ID3D11Buffer)), "ppBuffer")]),
    StdMethod(HRESULT, "CreateTexture1D", [(Pointer(Const(D3D11_TEXTURE1D_DESC)), "pDesc"), (Array(Const(D3D11_SUBRESOURCE_DATA), "_getNumSubResources(pDesc)"), "pInitialData"), Out(Pointer(ObjPointer(ID3D11Texture1D)), "ppTexture1D")]),
    StdMethod(HRESULT, "CreateTexture2D", [(Pointer(Const(D3D11_TEXTURE2D_DESC)), "pDesc"), (Array(Const(D3D11_SUBRESOURCE_DATA), "_getNumSubResources(pDesc)"), "pInitialData"), Out(Pointer(ObjPointer(ID3D11Texture2D)), "ppTexture2D")]),
    StdMethod(HRESULT, "CreateTexture3D", [(Pointer(Const(D3D11_TEXTURE3D_DESC)), "pDesc"), (Array(Const(D3D11_SUBRESOURCE_DATA), "_getNumSubResources(pDesc)"), "pInitialData"), Out(Pointer(ObjPointer(ID3D11Texture3D)), "ppTexture3D")]),
    StdMethod(HRESULT, "CreateShaderResourceView", [(ObjPointer(ID3D11Resource), "pResource"), (Pointer(Const(D3D11_SHADER_RESOURCE_VIEW_DESC)), "pDesc"), Out(Pointer(ObjPointer(ID3D11ShaderResourceView)), "ppSRView")]),
    StdMethod(HRESULT, "CreateUnorderedAccessView", [(ObjPointer(ID3D11Resource), "pResource"), (Pointer(Const(D3D11_UNORDERED_ACCESS_VIEW_DESC)), "pDesc"), Out(Pointer(ObjPointer(ID3D11UnorderedAccessView)), "ppUAView")]),
    StdMethod(HRESULT, "CreateRenderTargetView", [(ObjPointer(ID3D11Resource), "pResource"), (Pointer(Const(D3D11_RENDER_TARGET_VIEW_DESC)), "pDesc"), Out(Pointer(ObjPointer(ID3D11RenderTargetView)), "ppRTView")]),
    StdMethod(HRESULT, "CreateDepthStencilView", [(ObjPointer(ID3D11Resource), "pResource"), (Pointer(Const(D3D11_DEPTH_STENCIL_VIEW_DESC)), "pDesc"), Out(Pointer(ObjPointer(ID3D11DepthStencilView)), "ppDepthStencilView")]),
    StdMethod(HRESULT, "CreateInputLayout", [(Array(Const(D3D11_INPUT_ELEMENT_DESC), "NumElements"), "pInputElementDescs"), (UINT, "NumElements"), (Blob(Const(Void), "BytecodeLength"), "pShaderBytecodeWithInputSignature"), (SIZE_T, "BytecodeLength"), Out(Pointer(ObjPointer(ID3D11InputLayout)), "ppInputLayout")]),
    StdMethod(HRESULT, "CreateVertexShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11VertexShader)), "ppVertexShader")]),
    StdMethod(HRESULT, "CreateGeometryShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11GeometryShader)), "ppGeometryShader")]),
    StdMethod(HRESULT, "CreateGeometryShaderWithStreamOutput", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (Array(Const(D3D11_SO_DECLARATION_ENTRY), "NumEntries"), "pSODeclaration"), (UINT, "NumEntries"), (Array(Const(UINT), "NumStrides"), "pBufferStrides"), (UINT, "NumStrides"), (UINT, "RasterizedStream"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11GeometryShader)), "ppGeometryShader")]),
    StdMethod(HRESULT, "CreatePixelShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11PixelShader)), "ppPixelShader")]),
    StdMethod(HRESULT, "CreateHullShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11HullShader)), "ppHullShader")]),
    StdMethod(HRESULT, "CreateDomainShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11DomainShader)), "ppDomainShader")]),
    StdMethod(HRESULT, "CreateComputeShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (ObjPointer(ID3D11ClassLinkage), "pClassLinkage"), Out(Pointer(ObjPointer(ID3D11ComputeShader)), "ppComputeShader")]),
    StdMethod(HRESULT, "CreateClassLinkage", [Out(Pointer(ObjPointer(ID3D11ClassLinkage)), "ppLinkage")]),
    StdMethod(HRESULT, "CreateBlendState", [(Pointer(Const(D3D11_BLEND_DESC)), "pBlendStateDesc"), Out(Pointer(ObjPointer(ID3D11BlendState)), "ppBlendState")]),
    StdMethod(HRESULT, "CreateDepthStencilState", [(Pointer(Const(D3D11_DEPTH_STENCIL_DESC)), "pDepthStencilDesc"), Out(Pointer(ObjPointer(ID3D11DepthStencilState)), "ppDepthStencilState")]),
    StdMethod(HRESULT, "CreateRasterizerState", [(Pointer(Const(D3D11_RASTERIZER_DESC)), "pRasterizerDesc"), Out(Pointer(ObjPointer(ID3D11RasterizerState)), "ppRasterizerState")]),
    StdMethod(HRESULT, "CreateSamplerState", [(Pointer(Const(D3D11_SAMPLER_DESC)), "pSamplerDesc"), Out(Pointer(ObjPointer(ID3D11SamplerState)), "ppSamplerState")]),
    StdMethod(HRESULT, "CreateQuery", [(Pointer(Const(D3D11_QUERY_DESC)), "pQueryDesc"), Out(Pointer(ObjPointer(ID3D11Query)), "ppQuery")]),
    StdMethod(HRESULT, "CreatePredicate", [(Pointer(Const(D3D11_QUERY_DESC)), "pPredicateDesc"), Out(Pointer(ObjPointer(ID3D11Predicate)), "ppPredicate")]),
    StdMethod(HRESULT, "CreateCounter", [(Pointer(Const(D3D11_COUNTER_DESC)), "pCounterDesc"), Out(Pointer(ObjPointer(ID3D11Counter)), "ppCounter")]),
    StdMethod(HRESULT, "CreateDeferredContext", [(UINT, "ContextFlags"), Out(Pointer(ObjPointer(ID3D11DeviceContext)), "ppDeferredContext")]),
    StdMethod(HRESULT, "OpenSharedResource", [(HANDLE, "hResource"), (REFIID, "ReturnedInterface"), Out(Pointer(ObjPointer(Void)), "ppResource")]),
    StdMethod(HRESULT, "CheckFormatSupport", [(DXGI_FORMAT, "Format"), Out(Pointer(D3D11_FORMAT_SUPPORT), "pFormatSupport")], sideeffects=False),
    StdMethod(HRESULT, "CheckMultisampleQualityLevels", [(DXGI_FORMAT, "Format"), (UINT, "SampleCount"), Out(Pointer(UINT), "pNumQualityLevels")], sideeffects=False),
    StdMethod(Void, "CheckCounterInfo", [Out(Pointer(D3D11_COUNTER_INFO), "pCounterInfo")], sideeffects=False),
    StdMethod(HRESULT, "CheckCounter", [(Pointer(Const(D3D11_COUNTER_DESC)), "pDesc"), Out(Pointer(D3D11_COUNTER_TYPE), "pType"), Out(Pointer(UINT), "pActiveCounters"), Out(LPSTR, "szName"), Out(Pointer(UINT), "pNameLength"), Out(LPSTR, "szUnits"), Out(Pointer(UINT), "pUnitsLength"), Out(LPSTR, "szDescription"), Out(Pointer(UINT), "pDescriptionLength")], sideeffects=False),
    StdMethod(HRESULT, "CheckFeatureSupport", [(D3D11_FEATURE, "Feature"), Out(D3D11_FEATURE_DATA, "pFeatureSupportData"), (UINT, "FeatureSupportDataSize")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "guid"), Out(Pointer(UINT), "pDataSize"), Out(OpaquePointer(Void), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "guid"), (UINT, "DataSize"), (OpaqueBlob(Const(Void), "DataSize"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetPrivateDataInterface", [(REFGUID, "guid"), (OpaquePointer(Const(IUnknown)), "pData")], sideeffects=False),
    StdMethod(D3D_FEATURE_LEVEL, "GetFeatureLevel", [], sideeffects=False),
    StdMethod(D3D11_CREATE_DEVICE_FLAG, "GetCreationFlags", [], sideeffects=False),
    StdMethod(HRESULT, "GetDeviceRemovedReason", [], sideeffects=False),
    StdMethod(Void, "GetImmediateContext", [Out(Pointer(ObjPointer(ID3D11DeviceContext)), "ppImmediateContext")]),
    StdMethod(HRESULT, "SetExceptionMode", [(D3D11_RAISE_FLAG, "RaiseFlags")]),
    StdMethod(UINT, "GetExceptionMode", [], sideeffects=False),
]

d3d11 = Module("d3d11")

d3d11.addFunctions([
    StdFunction(HRESULT, "D3D11CreateDevice", [(ObjPointer(IDXGIAdapter), "pAdapter"), (D3D_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (D3D11_CREATE_DEVICE_FLAG, "Flags"), (Array(Const(D3D_FEATURE_LEVEL), "FeatureLevels"), "pFeatureLevels"), (UINT, "FeatureLevels"), (UINT, "SDKVersion"), Out(Pointer(ObjPointer(ID3D11Device)), "ppDevice"), Out(Pointer(D3D_FEATURE_LEVEL), "pFeatureLevel"), Out(Pointer(ObjPointer(ID3D11DeviceContext)), "ppImmediateContext")]),
    StdFunction(HRESULT, "D3D11CreateDeviceAndSwapChain", [(ObjPointer(IDXGIAdapter), "pAdapter"), (D3D_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (D3D11_CREATE_DEVICE_FLAG, "Flags"), (Array(Const(D3D_FEATURE_LEVEL), "FeatureLevels"), "pFeatureLevels"), (UINT, "FeatureLevels"), (UINT, "SDKVersion"), (Pointer(Const(DXGI_SWAP_CHAIN_DESC)), "pSwapChainDesc"), Out(Pointer(ObjPointer(IDXGISwapChain)), "ppSwapChain"), Out(Pointer(ObjPointer(ID3D11Device)), "ppDevice"), Out(Pointer(D3D_FEATURE_LEVEL), "pFeatureLevel"), Out(Pointer(ObjPointer(ID3D11DeviceContext)), "ppImmediateContext")]),
])

d3d11.addInterfaces([
    IDXGIFactory1,
    IDXGIAdapter1,
    IDXGIDevice1,
    IDXGIResource,
    ID3D11Debug,
    ID3D11InfoQueue,
    ID3D11SwitchToRef,
])

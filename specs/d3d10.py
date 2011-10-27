##########################################################################
#
# Copyright 2011 Jose Fonseca
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

D3D10_BLEND = Enum("D3D10_BLEND", [
    "D3D10_BLEND_ZERO",
    "D3D10_BLEND_ONE",
    "D3D10_BLEND_SRC_COLOR",
    "D3D10_BLEND_INV_SRC_COLOR",
    "D3D10_BLEND_SRC_ALPHA",
    "D3D10_BLEND_INV_SRC_ALPHA",
    "D3D10_BLEND_DEST_ALPHA",
    "D3D10_BLEND_INV_DEST_ALPHA",
    "D3D10_BLEND_DEST_COLOR",
    "D3D10_BLEND_INV_DEST_COLOR",
    "D3D10_BLEND_SRC_ALPHA_SAT",
    "D3D10_BLEND_BLEND_FACTOR",
    "D3D10_BLEND_INV_BLEND_FACTOR",
    "D3D10_BLEND_SRC1_COLOR",
    "D3D10_BLEND_INV_SRC1_COLOR",
    "D3D10_BLEND_SRC1_ALPHA",
    "D3D10_BLEND_INV_SRC1_ALPHA",
])

D3D10_BLEND_OP = Enum("D3D10_BLEND_OP", [
    "D3D10_BLEND_OP_ADD",
    "D3D10_BLEND_OP_SUBTRACT",
    "D3D10_BLEND_OP_REV_SUBTRACT",
    "D3D10_BLEND_OP_MIN",
    "D3D10_BLEND_OP_MAX",
])

D3D10_BLEND_DESC = Struct("D3D10_BLEND_DESC", [
    (BOOL, "AlphaToCoverageEnable"),
    (Array(BOOL, 8), "BlendEnable"),
    (D3D10_BLEND, "SrcBlend"),
    (D3D10_BLEND, "DestBlend"),
    (D3D10_BLEND_OP, "BlendOp"),
    (D3D10_BLEND, "SrcBlendAlpha"),
    (D3D10_BLEND, "DestBlendAlpha"),
    (D3D10_BLEND_OP, "BlendOpAlpha"),
    (Array(UINT8, 8), "RenderTargetWriteMask"),
])

D3D10_DEPTH_WRITE_MASK = Enum("D3D10_DEPTH_WRITE_MASK", [
    "D3D10_DEPTH_WRITE_MASK_ZERO",
    "D3D10_DEPTH_WRITE_MASK_ALL",
])

D3D10_COMPARISON_FUNC = Enum("D3D10_COMPARISON_FUNC", [
    "D3D10_COMPARISON_NEVER",
    "D3D10_COMPARISON_LESS",
    "D3D10_COMPARISON_EQUAL",
    "D3D10_COMPARISON_LESS_EQUAL",
    "D3D10_COMPARISON_GREATER",
    "D3D10_COMPARISON_NOT_EQUAL",
    "D3D10_COMPARISON_GREATER_EQUAL",
    "D3D10_COMPARISON_ALWAYS",
])

D3D10_STENCIL_OP = Enum("D3D10_STENCIL_OP", [
    "D3D10_STENCIL_OP_KEEP",
    "D3D10_STENCIL_OP_ZERO",
    "D3D10_STENCIL_OP_REPLACE",
    "D3D10_STENCIL_OP_INCR_SAT",
    "D3D10_STENCIL_OP_DECR_SAT",
    "D3D10_STENCIL_OP_INVERT",
    "D3D10_STENCIL_OP_INCR",
    "D3D10_STENCIL_OP_DECR",
])

D3D10_DEPTH_STENCILOP_DESC = Struct("D3D10_DEPTH_STENCILOP_DESC", [
    (D3D10_STENCIL_OP, "StencilFailOp"),
    (D3D10_STENCIL_OP, "StencilDepthFailOp"),
    (D3D10_STENCIL_OP, "StencilPassOp"),
    (D3D10_COMPARISON_FUNC, "StencilFunc"),
])

D3D10_DEPTH_STENCIL_DESC = Struct("D3D10_DEPTH_STENCIL_DESC", [
    (BOOL, "DepthEnable"),
    (D3D10_DEPTH_WRITE_MASK, "DepthWriteMask"),
    (D3D10_COMPARISON_FUNC, "DepthFunc"),
    (BOOL, "StencilEnable"),
    (UINT8, "StencilReadMask"),
    (UINT8, "StencilWriteMask"),
    (D3D10_DEPTH_STENCILOP_DESC, "FrontFace"),
    (D3D10_DEPTH_STENCILOP_DESC, "BackFace"),
])

D3D10_FILL_MODE = Enum("D3D10_FILL_MODE", [
    "D3D10_FILL_WIREFRAME",
    "D3D10_FILL_SOLID",
])

D3D10_CULL_MODE = Enum("D3D10_CULL_MODE", [
    "D3D10_CULL_NONE",
    "D3D10_CULL_FRONT",
    "D3D10_CULL_BACK",
])

D3D10_RASTERIZER_DESC = Struct("D3D10_RASTERIZER_DESC", [
    (D3D10_FILL_MODE, "FillMode"),
    (D3D10_CULL_MODE, "CullMode"),
    (BOOL, "FrontCounterClockwise"),
    (INT, "DepthBias"),
    (FLOAT, "DepthBiasClamp"),
    (FLOAT, "SlopeScaledDepthBias"),
    (BOOL, "DepthClipEnable"),
    (BOOL, "ScissorEnable"),
    (BOOL, "MultisampleEnable"),
    (BOOL, "AntialiasedLineEnable"),
])

D3D10_FILTER = Enum("D3D10_FILTER", [
    "D3D10_FILTER_MIN_MAG_MIP_POINT",
    "D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR",
    "D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT",
    "D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR",
    "D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT",
    "D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR",
    "D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT",
    "D3D10_FILTER_MIN_MAG_MIP_LINEAR",
    "D3D10_FILTER_ANISOTROPIC",
    "D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT",
    "D3D10_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR",
    "D3D10_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT",
    "D3D10_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR",
    "D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT",
    "D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR",
    "D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT",
    "D3D10_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR",
    "D3D10_FILTER_COMPARISON_ANISOTROPIC",
    "D3D10_FILTER_TEXT_1BIT",
])

D3D10_TEXTURE_ADDRESS_MODE = Enum("D3D10_TEXTURE_ADDRESS_MODE", [
    "D3D10_TEXTURE_ADDRESS_WRAP",
    "D3D10_TEXTURE_ADDRESS_MIRROR",
    "D3D10_TEXTURE_ADDRESS_CLAMP",
    "D3D10_TEXTURE_ADDRESS_BORDER",
    "D3D10_TEXTURE_ADDRESS_MIRROR_ONCE",
])

D3D10_SAMPLER_DESC = Struct("D3D10_SAMPLER_DESC", [
    (D3D10_FILTER, "Filter"),
    (D3D10_TEXTURE_ADDRESS_MODE, "AddressU"),
    (D3D10_TEXTURE_ADDRESS_MODE, "AddressV"),
    (D3D10_TEXTURE_ADDRESS_MODE, "AddressW"),
    (FLOAT, "MipLODBias"),
    (UINT, "MaxAnisotropy"),
    (D3D10_COMPARISON_FUNC, "ComparisonFunc"),
    (Array(FLOAT, 4), "BorderColor"),
    (FLOAT, "MinLOD"),
    (FLOAT, "MaxLOD"),
])

D3D10_COUNTER = Enum("D3D10_COUNTER", [
    "D3D10_COUNTER_GPU_IDLE",
    "D3D10_COUNTER_VERTEX_PROCESSING",
    "D3D10_COUNTER_GEOMETRY_PROCESSING",
    "D3D10_COUNTER_PIXEL_PROCESSING",
    "D3D10_COUNTER_OTHER_GPU_PROCESSING",
    "D3D10_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION",
    "D3D10_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION",
    "D3D10_COUNTER_VERTEX_THROUGHPUT_UTILIZATION",
    "D3D10_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION",
    "D3D10_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION",
    "D3D10_COUNTER_VS_MEMORY_LIMITED",
    "D3D10_COUNTER_VS_COMPUTATION_LIMITED",
    "D3D10_COUNTER_GS_MEMORY_LIMITED",
    "D3D10_COUNTER_GS_COMPUTATION_LIMITED",
    "D3D10_COUNTER_PS_MEMORY_LIMITED",
    "D3D10_COUNTER_PS_COMPUTATION_LIMITED",
    "D3D10_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE",
    "D3D10_COUNTER_TEXTURE_CACHE_HIT_RATE",
    "D3D10_COUNTER_DEVICE_DEPENDENT_0",
])

D3D10_COUNTER_DESC = Struct("D3D10_COUNTER_DESC", [
    (D3D10_COUNTER, "Counter"),
    (UINT, "MiscFlags"),
])

D3D10_COUNTER_TYPE = Enum("D3D10_COUNTER_TYPE", [
    "D3D10_COUNTER_TYPE_FLOAT32",
    "D3D10_COUNTER_TYPE_UINT16",
    "D3D10_COUNTER_TYPE_UINT32",
    "D3D10_COUNTER_TYPE_UINT64",
])

D3D10_COUNTER_INFO = Struct("D3D10_COUNTER_INFO", [
    (D3D10_COUNTER, "LastDeviceDependentCounter"),
    (UINT, "NumSimultaneousCounters"),
    (UINT8, "NumDetectableParallelUnits"),
])

D3D10_RESOURCE_DIMENSION = Enum("D3D10_RESOURCE_DIMENSION", [
    "D3D10_RESOURCE_DIMENSION_UNKNOWN",
    "D3D10_RESOURCE_DIMENSION_BUFFER",
    "D3D10_RESOURCE_DIMENSION_TEXTURE1D",
    "D3D10_RESOURCE_DIMENSION_TEXTURE2D",
    "D3D10_RESOURCE_DIMENSION_TEXTURE3D",
])

D3D10_USAGE = Enum("D3D10_USAGE", [
    "D3D10_USAGE_DEFAULT",
    "D3D10_USAGE_IMMUTABLE",
    "D3D10_USAGE_DYNAMIC",
    "D3D10_USAGE_STAGING",
])

D3D10_BIND_FLAG = Enum("D3D10_BIND_FLAG", [
    "D3D10_BIND_VERTEX_BUFFER",
    "D3D10_BIND_INDEX_BUFFER",
    "D3D10_BIND_CONSTANT_BUFFER",
    "D3D10_BIND_SHADER_RESOURCE",
    "D3D10_BIND_STREAM_OUTPUT",
    "D3D10_BIND_RENDER_TARGET",
    "D3D10_BIND_DEPTH_STENCIL",
])

D3D10_BUFFER_DESC = Struct("D3D10_BUFFER_DESC", [
    (UINT, "ByteWidth"),
    (D3D10_USAGE, "Usage"),
    (UINT, "BindFlags"),
    (UINT, "CPUAccessFlags"),
    (UINT, "MiscFlags"),
])

D3D10_MAP = Enum("D3D10_MAP", [
    "D3D10_MAP_READ",
    "D3D10_MAP_WRITE",
    "D3D10_MAP_READ_WRITE",
    "D3D10_MAP_WRITE_DISCARD",
    "D3D10_MAP_WRITE_NO_OVERWRITE",
])

D3D10_TEXTURE1D_DESC = Struct("D3D10_TEXTURE1D_DESC", [
    (UINT, "Width"),
    (UINT, "MipLevels"),
    (UINT, "ArraySize"),
    (DXGI_FORMAT, "Format"),
    (D3D10_USAGE, "Usage"),
    (UINT, "BindFlags"),
    (UINT, "CPUAccessFlags"),
    (UINT, "MiscFlags"),
])

D3D10_TEXTURE2D_DESC = Struct("D3D10_TEXTURE2D_DESC", [
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "MipLevels"),
    (UINT, "ArraySize"),
    (DXGI_FORMAT, "Format"),
    (DXGI_SAMPLE_DESC, "SampleDesc"),
    (D3D10_USAGE, "Usage"),
    (UINT, "BindFlags"),
    (UINT, "CPUAccessFlags"),
    (UINT, "MiscFlags"),
])

D3D10_TEXTURE3D_DESC = Struct("D3D10_TEXTURE3D_DESC", [
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "Depth"),
    (UINT, "MipLevels"),
    (DXGI_FORMAT, "Format"),
    (D3D10_USAGE, "Usage"),
    (UINT, "BindFlags"),
    (UINT, "CPUAccessFlags"),
    (UINT, "MiscFlags"),
])

D3D10_DSV_DIMENSION = Enum("D3D10_DSV_DIMENSION", [
    "D3D10_DSV_DIMENSION_UNKNOWN",
    "D3D10_DSV_DIMENSION_TEXTURE1D",
    "D3D10_DSV_DIMENSION_TEXTURE1DARRAY",
    "D3D10_DSV_DIMENSION_TEXTURE2D",
    "D3D10_DSV_DIMENSION_TEXTURE2DARRAY",
    "D3D10_DSV_DIMENSION_TEXTURE2DMS",
    "D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY",
])

D3D10_TEX1D_DSV = Struct("D3D10_TEX1D_DSV", [
    (UINT, "MipSlice"),
])

D3D10_TEX1D_ARRAY_DSV = Struct("D3D10_TEX1D_ARRAY_DSV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX2D_DSV = Struct("D3D10_TEX2D_DSV", [
    (UINT, "MipSlice"),
])

D3D10_TEX2D_ARRAY_DSV = Struct("D3D10_TEX2D_ARRAY_DSV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX2DMS_DSV = Struct("D3D10_TEX2DMS_DSV", [
    (UINT, "UnusedField_NothingToDefine"),
])

D3D10_TEX2DMS_ARRAY_DSV = Struct("D3D10_TEX2DMS_ARRAY_DSV", [
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_DEPTH_STENCIL_VIEW_DESC = Struct("D3D10_DEPTH_STENCIL_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D10_DSV_DIMENSION, "ViewDimension"),
    (D3D10_TEX1D_DSV, "Texture1D"),
    (D3D10_TEX1D_ARRAY_DSV, "Texture1DArray"),
    (D3D10_TEX2D_DSV, "Texture2D"),
    (D3D10_TEX2D_ARRAY_DSV, "Texture2DArray"),
    (D3D10_TEX2DMS_DSV, "Texture2DMS"),
    (D3D10_TEX2DMS_ARRAY_DSV, "Texture2DMSArray"),
])

D3D10_RTV_DIMENSION = Enum("D3D10_RTV_DIMENSION", [
    "D3D10_RTV_DIMENSION_UNKNOWN",
    "D3D10_RTV_DIMENSION_BUFFER",
    "D3D10_RTV_DIMENSION_TEXTURE1D",
    "D3D10_RTV_DIMENSION_TEXTURE1DARRAY",
    "D3D10_RTV_DIMENSION_TEXTURE2D",
    "D3D10_RTV_DIMENSION_TEXTURE2DARRAY",
    "D3D10_RTV_DIMENSION_TEXTURE2DMS",
    "D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY",
    "D3D10_RTV_DIMENSION_TEXTURE3D",
])

D3D10_BUFFER_RTV = Struct("D3D10_BUFFER_RTV", [
    (UINT, "ElementOffset"),
    (UINT, "ElementWidth"),
])

D3D10_TEX1D_RTV = Struct("D3D10_TEX1D_RTV", [
    (UINT, "MipSlice"),
])

D3D10_TEX1D_ARRAY_RTV = Struct("D3D10_TEX1D_ARRAY_RTV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX2D_RTV = Struct("D3D10_TEX2D_RTV", [
    (UINT, "MipSlice"),
])

D3D10_TEX2D_ARRAY_RTV = Struct("D3D10_TEX2D_ARRAY_RTV", [
    (UINT, "MipSlice"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX2DMS_RTV = Struct("D3D10_TEX2DMS_RTV", [
    (UINT, "UnusedField_NothingToDefine"),
])

D3D10_TEX2DMS_ARRAY_RTV = Struct("D3D10_TEX2DMS_ARRAY_RTV", [
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX3D_RTV = Struct("D3D10_TEX3D_RTV", [
    (UINT, "MipSlice"),
    (UINT, "FirstWSlice"),
    (UINT, "WSize"),
])

D3D10_RENDER_TARGET_VIEW_DESC = Struct("D3D10_RENDER_TARGET_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D10_RTV_DIMENSION, "ViewDimension"),
    (D3D10_BUFFER_RTV, "Buffer"),
    (D3D10_TEX1D_RTV, "Texture1D"),
    (D3D10_TEX1D_ARRAY_RTV, "Texture1DArray"),
    (D3D10_TEX2D_RTV, "Texture2D"),
    (D3D10_TEX2D_ARRAY_RTV, "Texture2DArray"),
    (D3D10_TEX2DMS_RTV, "Texture2DMS"),
    (D3D10_TEX2DMS_ARRAY_RTV, "Texture2DMSArray"),
    (D3D10_TEX3D_RTV, "Texture3D"),
])

D3D10_SRV_DIMENSION = Enum("D3D10_SRV_DIMENSION", [
    "D3D10_SRV_DIMENSION_UNKNOWN",
	"D3D10_SRV_DIMENSION_BUFFER",
	"D3D10_SRV_DIMENSION_TEXTURE1D",
	"D3D10_SRV_DIMENSION_TEXTURE1DARRAY",
	"D3D10_SRV_DIMENSION_TEXTURE2D",
	"D3D10_SRV_DIMENSION_TEXTURE2DARRAY",
	"D3D10_SRV_DIMENSION_TEXTURE2DMS",
	"D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY",
	"D3D10_SRV_DIMENSION_TEXTURE3D",
	"D3D10_SRV_DIMENSION_TEXTURECUBE",
])

D3D10_BUFFER_SRV = Struct("D3D10_BUFFER_SRV", [
    (UINT, "ElementOffset"),
    (UINT, "ElementWidth"),
])

D3D10_TEX1D_SRV = Struct("D3D10_TEX1D_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D10_TEX1D_ARRAY_SRV = Struct("D3D10_TEX1D_ARRAY_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX2D_SRV = Struct("D3D10_TEX2D_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D10_TEX2D_ARRAY_SRV = Struct("D3D10_TEX2D_ARRAY_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX2DMS_SRV = Struct("D3D10_TEX2DMS_SRV", [
    (UINT, "UnusedField_NothingToDefine"),
])

D3D10_TEX2DMS_ARRAY_SRV = Struct("D3D10_TEX2DMS_ARRAY_SRV", [
    (UINT, "FirstArraySlice"),
    (UINT, "ArraySize"),
])

D3D10_TEX3D_SRV = Struct("D3D10_TEX3D_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D10_TEXCUBE_SRV = Struct("D3D10_TEXCUBE_SRV", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
])

D3D10_SHADER_RESOURCE_VIEW_DESC = Struct("D3D10_SHADER_RESOURCE_VIEW_DESC", [
    (DXGI_FORMAT, "Format"),
    (D3D10_SRV_DIMENSION, "ViewDimension"),
    (D3D10_BUFFER_SRV, "Buffer"),
    (D3D10_TEX1D_SRV, "Texture1D"),
    (D3D10_TEX1D_ARRAY_SRV, "Texture1DArray"),
    (D3D10_TEX2D_SRV, "Texture2D"),
    (D3D10_TEX2D_ARRAY_SRV, "Texture2DArray"),
    (D3D10_TEX2DMS_SRV, "Texture2DMS"),
    (D3D10_TEX2DMS_ARRAY_SRV, "Texture2DMSArray"),
    (D3D10_TEX3D_SRV, "Texture3D"),
    (D3D10_TEXCUBE_SRV, "TextureCube"),
])

D3D10_BOX = Struct("D3D10_BOX", [
    (UINT, "left"),
    (UINT, "top"),
    (UINT, "front"),
    (UINT, "right"),
    (UINT, "bottom"),
    (UINT, "back"),
])

D3D10_SUBRESOURCE_DATA = Struct("D3D10_SUBRESOURCE_DATA", [
    (OpaquePointer(Const(Void)), "pSysMem"),
    (UINT, "SysMemPitch"),
    (UINT, "SysMemSlicePitch"),
])

D3D10_SO_DECLARATION_ENTRY = Struct("D3D10_SO_DECLARATION_ENTRY", [
    (LPCSTR, "SemanticName"),
    (UINT, "SemanticIndex"),
    (BYTE, "StartComponent"),
    (BYTE, "ComponentCount"),
    (BYTE, "OutputSlot"),
])

D3D10_INPUT_CLASSIFICATION = Enum("D3D10_INPUT_CLASSIFICATION", [
    "D3D10_INPUT_PER_VERTEX_DATA",
    "D3D10_INPUT_PER_INSTANCE_DATA",
])

D3D10_INPUT_ELEMENT_DESC = Struct("D3D10_INPUT_ELEMENT_DESC", [
    (LPCSTR, "SemanticName"),
    (UINT, "SemanticIndex"),
    (DXGI_FORMAT, "Format"),
    (UINT, "InputSlot"),
    (UINT, "AlignedByteOffset"),
    (D3D10_INPUT_CLASSIFICATION, "InputSlotClass"),
    (UINT, "InstanceDataStepRate"),
])

D3D10_QUERY = Enum("D3D10_QUERY", [
    "D3D10_QUERY_EVENT",
    "D3D10_QUERY_OCCLUSION",
    "D3D10_QUERY_TIMESTAMP",
    "D3D10_QUERY_TIMESTAMP_DISJOINT",
    "D3D10_QUERY_PIPELINE_STATISTICS",
    "D3D10_QUERY_OCCLUSION_PREDICATE",
    "D3D10_QUERY_SO_STATISTICS",
    "D3D10_QUERY_SO_OVERFLOW_PREDICATE",
])

D3D10_QUERY_DESC = Struct("D3D10_QUERY_DESC", [
    (D3D10_QUERY, "Query"),
    (UINT, "MiscFlags"),
])

D3D10_PRIMITIVE_TOPOLOGY = Enum("D3D10_PRIMITIVE_TOPOLOGY", [
	"D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED",
	"D3D10_PRIMITIVE_TOPOLOGY_POINTLIST",
	"D3D10_PRIMITIVE_TOPOLOGY_LINELIST",
	"D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP",
	"D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST",
	"D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP",
	"D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ",
	"D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ",
	"D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ",
	"D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ",
])

D3D10_PRIMITIVE = Enum("D3D10_PRIMITIVE", [
	"D3D10_PRIMITIVE_UNDEFINED",
	"D3D10_PRIMITIVE_POINT",
	"D3D10_PRIMITIVE_LINE",
	"D3D10_PRIMITIVE_TRIANGLE",
	"D3D10_PRIMITIVE_LINE_ADJ",
	"D3D10_PRIMITIVE_TRIANGLE_ADJ",
])

D3D10_RECT = Alias("D3D10_RECT", RECT)
D3D10_VIEWPORT = Struct("D3D10_VIEWPORT", [
    (INT, "TopLeftX"),
    (INT, "TopLeftY"),
    (UINT, "Width"),
    (UINT, "Height"),
    (FLOAT, "MinDepth"),
    (FLOAT, "MaxDepth"),
])

D3D10_MAPPED_TEXTURE2D = Struct("D3D10_MAPPED_TEXTURE2D", [
    (OpaquePointer(Void), "pData"),
    (UINT, "RowPitch"),
])

D3D10_MAPPED_TEXTURE3D = Struct("D3D10_MAPPED_TEXTURE3D", [
    (OpaquePointer(Void), "pData"),
    (UINT, "RowPitch"),
    (UINT, "DepthPitch"),
])

D3D10_CPU_ACCESS_FLAG = Enum("D3D10_CPU_ACCESS_FLAG", [
    "D3D10_CPU_ACCESS_WRITE",
    "D3D10_CPU_ACCESS_READ",
])

D3D10_RESOURCE_MISC_FLAG = Enum("D3D10_RESOURCE_MISC_FLAG", [
    "D3D10_RESOURCE_MISC_GENERATE_MIPS",
    "D3D10_RESOURCE_MISC_SHARED",
    "D3D10_RESOURCE_MISC_TEXTURECUBE",
    "D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX",
    "D3D10_RESOURCE_MISC_GDI_COMPATIBLE",
])

D3D10_MAP_FLAG = Enum("D3D10_MAP_FLAG", [
    "D3D10_MAP_FLAG_DO_NOT_WAIT",
])

D3D10_CLEAR_FLAG = Enum("D3D10_CLEAR_FLAG", [
    "D3D10_CLEAR_DEPTH",
    "D3D10_CLEAR_STENCIL",
])

D3D10_COLOR_WRITE_ENABLE = Enum("D3D10_COLOR_WRITE_ENABLE", [
    "D3D10_COLOR_WRITE_ENABLE_RED",
    "D3D10_COLOR_WRITE_ENABLE_GREEN",
    "D3D10_COLOR_WRITE_ENABLE_BLUE",
    "D3D10_COLOR_WRITE_ENABLE_ALPHA",
    "D3D10_COLOR_WRITE_ENABLE_ALL",
    "D3D10_COLOR_WRITE_ENABLE_RED",
    "|",
    "D3D10_COLOR_WRITE_ENABLE_GREEN",
    "|",
    "D3D10_COLOR_WRITE_ENABLE_BLUE",
    "|",
    "D3D10_COLOR_WRITE_ENABLE_ALPHA",
    ")",
])

D3D10_TEXTURECUBE_FACE = Enum("D3D10_TEXTURECUBE_FACE", [
    "D3D10_TEXTURECUBE_FACE_POSITIVE_X",
    "D3D10_TEXTURECUBE_FACE_NEGATIVE_X",
    "D3D10_TEXTURECUBE_FACE_POSITIVE_Y",
    "D3D10_TEXTURECUBE_FACE_NEGATIVE_Y",
    "D3D10_TEXTURECUBE_FACE_POSITIVE_Z",
    "D3D10_TEXTURECUBE_FACE_NEGATIVE_Z",
])

D3D10_ASYNC_GETDATA_FLAG = Enum("D3D10_ASYNC_GETDATA_FLAG", [
    "D3D10_ASYNC_GETDATA_DONOTFLUSH",
])

D3D10_FILTER_TYPE = Enum("D3D10_FILTER_TYPE", [
    "D3D10_FILTER_TYPE_POINT",
    "D3D10_FILTER_TYPE_LINEAR",
])

D3D10_QUERY_MISC_FLAG = Enum("D3D10_QUERY_MISC_FLAG", [
    "D3D10_QUERY_MISC_PREDICATEHINT",
])

D3D10_QUERY_DATA_TIMESTAMP_DISJOINT = Struct("D3D10_QUERY_DATA_TIMESTAMP_DISJOINT", [
    (UINT64, "Frequency"),
    (BOOL, "Disjoint"),
])

D3D10_QUERY_DATA_PIPELINE_STATISTICS = Struct("D3D10_QUERY_DATA_PIPELINE_STATISTICS", [
    (UINT64, "IAVertices"),
    (UINT64, "IAPrimitives"),
    (UINT64, "VSInvocations"),
    (UINT64, "GSInvocations"),
    (UINT64, "GSPrimitives"),
    (UINT64, "CInvocations"),
    (UINT64, "CPrimitives"),
    (UINT64, "PSInvocations"),
])

D3D10_QUERY_DATA_SO_STATISTICS = Struct("D3D10_QUERY_DATA_SO_STATISTICS", [
    (UINT64, "NumPrimitivesWritten"),
    (UINT64, "PrimitivesStorageNeeded"),
])

D3D10_CREATE_DEVICE_FLAG = Enum("D3D10_CREATE_DEVICE_FLAG", [
    "D3D10_CREATE_DEVICE_SINGLETHREADED",
    "D3D10_CREATE_DEVICE_DEBUG",
    "D3D10_CREATE_DEVICE_SWITCH_TO_REF",
    "D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS",
    "D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP",
    "D3D10_CREATE_DEVICE_BGRA_SUPPORT",
    "D3D10_CREATE_DEVICE_STRICT_VALIDATION",
])

ID3D10DeviceChild = Interface("ID3D10DeviceChild", IUnknown)
ID3D10Resource = Interface("ID3D10Resource", ID3D10DeviceChild)
ID3D10Buffer = Interface("ID3D10Buffer", ID3D10Resource)
ID3D10Texture1D = Interface("ID3D10Texture1D", ID3D10Resource)
ID3D10Texture2D = Interface("ID3D10Texture2D", ID3D10Resource)
ID3D10Texture3D = Interface("ID3D10Texture3D", ID3D10Resource)
ID3D10View = Interface("ID3D10View", ID3D10DeviceChild)
ID3D10DepthStencilView = Interface("ID3D10DepthStencilView", ID3D10View)
ID3D10RenderTargetView = Interface("ID3D10RenderTargetView", ID3D10View)
ID3D10ShaderResourceView = Interface("ID3D10ShaderResourceView", ID3D10View)
ID3D10BlendState = Interface("ID3D10BlendState", ID3D10DeviceChild)
ID3D10DepthStencilState = Interface("ID3D10DepthStencilState", ID3D10DeviceChild)
ID3D10GeometryShader = Interface("ID3D10GeometryShader", ID3D10DeviceChild)
ID3D10InputLayout = Interface("ID3D10InputLayout", ID3D10DeviceChild)
ID3D10PixelShader = Interface("ID3D10PixelShader", ID3D10DeviceChild)
ID3D10RasterizerState = Interface("ID3D10RasterizerState", ID3D10DeviceChild)
ID3D10SamplerState = Interface("ID3D10SamplerState", ID3D10DeviceChild)
ID3D10VertexShader = Interface("ID3D10VertexShader", ID3D10DeviceChild)
ID3D10Asynchronous = Interface("ID3D10Asynchronous", ID3D10DeviceChild)
ID3D10Counter = Interface("ID3D10Counter", ID3D10Asynchronous)
ID3D10Query = Interface("ID3D10Query", ID3D10Asynchronous)
ID3D10Predicate = Interface("ID3D10Predicate", ID3D10Query)
ID3D10Device = Interface("ID3D10Device", IUnknown)
ID3D10Multithread = Interface("ID3D10Multithread", IUnknown)

ID3D10DeviceChild.methods += [
    Method(Void, "GetDevice", [Out(Pointer(Pointer(ID3D10Device)), "ppDevice")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "guid"), Out(Pointer(UINT), "pDataSize"), Out(OpaquePointer(Void), "pData")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "guid"), (UINT, "DataSize"), (OpaquePointer(Const(Void)), "pData")]),
    Method(HRESULT, "SetPrivateDataInterface", [(REFGUID, "guid"), (OpaquePointer(Const(IUnknown)), "pData")]),
]

ID3D10Resource.methods += [
    Method(Void, "GetType", [Out(Pointer(D3D10_RESOURCE_DIMENSION), "rType")]),
    Method(Void, "SetEvictionPriority", [(UINT, "EvictionPriority")]),
    Method(UINT, "GetEvictionPriority", []),
]

ID3D10Buffer.methods += [
    Method(HRESULT, "Map", [(D3D10_MAP, "MapType"), (UINT, "MapFlags"), Out(Pointer(OpaquePointer(Void)), "ppData")]),
    Method(Void, "Unmap", []),
    Method(Void, "GetDesc", [Out(Pointer(D3D10_BUFFER_DESC), "pDesc")]),
]

ID3D10Texture1D.methods += [
    Method(HRESULT, "Map", [(UINT, "Subresource"), (D3D10_MAP, "MapType"), (UINT, "MapFlags"), Out(Pointer(OpaquePointer(Void)), "ppData")]),
    Method(Void, "Unmap", [(UINT, "Subresource")]),
    Method(Void, "GetDesc", [Out(Pointer(D3D10_TEXTURE1D_DESC), "pDesc")]),
]

ID3D10Texture2D.methods += [
    Method(HRESULT, "Map", [(UINT, "Subresource"), (D3D10_MAP, "MapType"), (UINT, "MapFlags"), Out(Pointer(D3D10_MAPPED_TEXTURE2D), "pMappedTex2D")]),
    Method(Void, "Unmap", [(UINT, "Subresource")]),
    Method(Void, "GetDesc", [Out(Pointer(D3D10_TEXTURE2D_DESC), "pDesc")]),
]

ID3D10Texture3D.methods += [
    Method(HRESULT, "Map", [(UINT, "Subresource"), (D3D10_MAP, "MapType"), (UINT, "MapFlags"), Out(Pointer(D3D10_MAPPED_TEXTURE3D), "pMappedTex3D")]),
    Method(Void, "Unmap", [(UINT, "Subresource")]),
    Method(Void, "GetDesc", [Out(Pointer(D3D10_TEXTURE3D_DESC), "pDesc")]),
]

ID3D10View.methods += [
    Method(Void, "GetResource", [Out(Pointer(Pointer(ID3D10Resource)), "ppResource")]),
]

ID3D10DepthStencilView.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_DEPTH_STENCIL_VIEW_DESC), "pDesc")]),
]

ID3D10RenderTargetView.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_RENDER_TARGET_VIEW_DESC), "pDesc")]),
]

ID3D10ShaderResourceView.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_SHADER_RESOURCE_VIEW_DESC), "pDesc")]),
]

ID3D10BlendState.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_BLEND_DESC), "pDesc")]),
]

ID3D10DepthStencilState.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_DEPTH_STENCIL_DESC), "pDesc")]),
]

ID3D10GeometryShader.methods += [
]

ID3D10InputLayout.methods += [
]

ID3D10PixelShader.methods += [
]

ID3D10RasterizerState.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_RASTERIZER_DESC), "pDesc")]),
]

ID3D10SamplerState.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_SAMPLER_DESC), "pDesc")]),
]

ID3D10VertexShader.methods += [
]

ID3D10Asynchronous.methods += [
    Method(Void, "Begin", []),
    Method(Void, "End", []),
    Method(HRESULT, "GetData", [Out(Blob(Void, "DataSize"), "pData"), (UINT, "DataSize"), (UINT, "GetDataFlags")]),
    Method(UINT, "GetDataSize", []),
]

ID3D10Counter.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_COUNTER_DESC), "pDesc")]),
]

ID3D10Query.methods += [
    Method(Void, "GetDesc", [Out(Pointer(D3D10_QUERY_DESC), "pDesc")]),
]

ID3D10Predicate.methods += [
]

ID3D10Device.methods += [
    Method(Void, "VSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(OpaquePointer(ID3D10Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    Method(Void, "PSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(OpaquePointer(ID3D10ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    Method(Void, "PSSetShader", [(Pointer(ID3D10PixelShader), "pPixelShader")]),
    Method(Void, "PSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(OpaquePointer(ID3D10SamplerState)), "NumSamplers"), "ppSamplers")]),
    Method(Void, "VSSetShader", [(Pointer(ID3D10VertexShader), "pVertexShader")]),
    Method(Void, "DrawIndexed", [(UINT, "IndexCount"), (UINT, "StartIndexLocation"), (INT, "BaseVertexLocation")]),
    Method(Void, "Draw", [(UINT, "VertexCount"), (UINT, "StartVertexLocation")]),
    Method(Void, "PSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(OpaquePointer(ID3D10Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    Method(Void, "IASetInputLayout", [(Pointer(ID3D10InputLayout), "pInputLayout")]),
    Method(Void, "IASetVertexBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(OpaquePointer(ID3D10Buffer)), "NumBuffers"), "ppVertexBuffers"), (Array(Const(UINT), "NumBuffers"), "pStrides"), (Array(Const(UINT), "NumBuffers"), "pOffsets")]),
    Method(Void, "IASetIndexBuffer", [(OpaquePointer(ID3D10Buffer), "pIndexBuffer"), (DXGI_FORMAT, "Format"), (UINT, "Offset")]),
    Method(Void, "DrawIndexedInstanced", [(UINT, "IndexCountPerInstance"), (UINT, "InstanceCount"), (UINT, "StartIndexLocation"), (INT, "BaseVertexLocation"), (UINT, "StartInstanceLocation")]),
    Method(Void, "DrawInstanced", [(UINT, "VertexCountPerInstance"), (UINT, "InstanceCount"), (UINT, "StartVertexLocation"), (UINT, "StartInstanceLocation")]),
    Method(Void, "GSSetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), (Array(Const(OpaquePointer(ID3D10Buffer)), "NumBuffers"), "ppConstantBuffers")]),
    Method(Void, "GSSetShader", [(Pointer(ID3D10GeometryShader), "pShader")]),
    Method(Void, "IASetPrimitiveTopology", [(D3D10_PRIMITIVE_TOPOLOGY, "Topology")]),
    Method(Void, "VSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(OpaquePointer(ID3D10ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    Method(Void, "VSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(OpaquePointer(ID3D10SamplerState)), "NumSamplers"), "ppSamplers")]),
    Method(Void, "SetPredication", [(Pointer(ID3D10Predicate), "pPredicate"), (BOOL, "PredicateValue")]),
    Method(Void, "GSSetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), (Array(Const(OpaquePointer(ID3D10ShaderResourceView)), "NumViews"), "ppShaderResourceViews")]),
    Method(Void, "GSSetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), (Array(Const(OpaquePointer(ID3D10SamplerState)), "NumSamplers"), "ppSamplers")]),
    Method(Void, "OMSetRenderTargets", [(UINT, "NumViews"), (Array(Const(OpaquePointer(ID3D10RenderTargetView)), "NumViews"), "ppRenderTargetViews"), (Pointer(ID3D10DepthStencilView), "pDepthStencilView")]),
    Method(Void, "OMSetBlendState", [(Pointer(ID3D10BlendState), "pBlendState"), (Array(Const(FLOAT), 4), "BlendFactor"), (UINT, "SampleMask")]),
    Method(Void, "OMSetDepthStencilState", [(Pointer(ID3D10DepthStencilState), "pDepthStencilState"), (UINT, "StencilRef")]),
    Method(Void, "SOSetTargets", [(UINT, "NumBuffers"), (Array(Const(OpaquePointer(ID3D10Buffer)), "NumBuffers"), "ppSOTargets"), (Array(Const(UINT), "NumBuffers"), "pOffsets")]),
    Method(Void, "DrawAuto", []),
    Method(Void, "RSSetState", [(Pointer(ID3D10RasterizerState), "pRasterizerState")]),
    Method(Void, "RSSetViewports", [(UINT, "NumViewports"), (Array(Const(D3D10_VIEWPORT), "NumViewports"), "pViewports")]),
    Method(Void, "RSSetScissorRects", [(UINT, "NumRects"), (Array(Const(D3D10_RECT), "NumRects"), "pRects")]),
    Method(Void, "CopySubresourceRegion", [(Pointer(ID3D10Resource), "pDstResource"), (UINT, "DstSubresource"), (UINT, "DstX"), (UINT, "DstY"), (UINT, "DstZ"), (Pointer(ID3D10Resource), "pSrcResource"), (UINT, "SrcSubresource"), (Pointer(Const(D3D10_BOX)), "pSrcBox")]),
    Method(Void, "CopyResource", [(Pointer(ID3D10Resource), "pDstResource"), (Pointer(ID3D10Resource), "pSrcResource")]),
    Method(Void, "UpdateSubresource", [(Pointer(ID3D10Resource), "pDstResource"), (UINT, "DstSubresource"), (Pointer(Const(D3D10_BOX)), "pDstBox"), (OpaquePointer(Const(Void)), "pSrcData"), (UINT, "SrcRowPitch"), (UINT, "SrcDepthPitch")]),
    Method(Void, "ClearRenderTargetView", [(OpaquePointer(ID3D10RenderTargetView), "pRenderTargetView"), (Array(Const(FLOAT), 4), "ColorRGBA")]),
    Method(Void, "ClearDepthStencilView", [(Pointer(ID3D10DepthStencilView), "pDepthStencilView"), (UINT, "ClearFlags"), (FLOAT, "Depth"), (UINT8, "Stencil")]),
    Method(Void, "GenerateMips", [(OpaquePointer(ID3D10ShaderResourceView), "pShaderResourceView")]),
    Method(Void, "ResolveSubresource", [(Pointer(ID3D10Resource), "pDstResource"), (UINT, "DstSubresource"), (Pointer(ID3D10Resource), "pSrcResource"), (UINT, "SrcSubresource"), (DXGI_FORMAT, "Format")]),
    Method(Void, "VSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(OpaquePointer(ID3D10Buffer), "NumBuffers"), "ppConstantBuffers")]),
    Method(Void, "PSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(OpaquePointer(ID3D10ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    Method(Void, "PSGetShader", [Out(Pointer(Pointer(ID3D10PixelShader)), "ppPixelShader")]),
    Method(Void, "PSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(OpaquePointer(ID3D10SamplerState), "NumSamplers"), "ppSamplers")]),
    Method(Void, "VSGetShader", [Out(Pointer(Pointer(ID3D10VertexShader)), "ppVertexShader")]),
    Method(Void, "PSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(OpaquePointer(ID3D10Buffer), "NumBuffers"), "ppConstantBuffers")]),
    Method(Void, "IAGetInputLayout", [Out(Pointer(Pointer(ID3D10InputLayout)), "ppInputLayout")]),
    Method(Void, "IAGetVertexBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(OpaquePointer(ID3D10Buffer), "NumBuffers"), "ppVertexBuffers"), Out(Array(UINT, "NumBuffers"), "pStrides"), Out(Array(UINT, "NumBuffers"), "pOffsets")]),
    Method(Void, "IAGetIndexBuffer", [Out(Pointer(OpaquePointer(ID3D10Buffer)), "pIndexBuffer"), Out(Pointer(DXGI_FORMAT), "Format"), Out(Pointer(UINT), "Offset")]),
    Method(Void, "GSGetConstantBuffers", [(UINT, "StartSlot"), (UINT, "NumBuffers"), Out(Array(OpaquePointer(ID3D10Buffer), "NumBuffers"), "ppConstantBuffers")]),
    Method(Void, "GSGetShader", [Out(Pointer(Pointer(ID3D10GeometryShader)), "ppGeometryShader")]),
    Method(Void, "IAGetPrimitiveTopology", [Out(Pointer(D3D10_PRIMITIVE_TOPOLOGY), "pTopology")]),
    Method(Void, "VSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(OpaquePointer(ID3D10ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    Method(Void, "VSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(OpaquePointer(ID3D10SamplerState), "NumSamplers"), "ppSamplers")]),
    Method(Void, "GetPredication", [Out(Pointer(Pointer(ID3D10Predicate)), "ppPredicate"), Out(Pointer(BOOL), "pPredicateValue")]),
    Method(Void, "GSGetShaderResources", [(UINT, "StartSlot"), (UINT, "NumViews"), Out(Array(OpaquePointer(ID3D10ShaderResourceView), "NumViews"), "ppShaderResourceViews")]),
    Method(Void, "GSGetSamplers", [(UINT, "StartSlot"), (UINT, "NumSamplers"), Out(Array(OpaquePointer(ID3D10SamplerState), "NumSamplers"), "ppSamplers")]),
    Method(Void, "OMGetRenderTargets", [(UINT, "NumViews"), Out(Array(OpaquePointer(ID3D10RenderTargetView), "NumViews"), "ppRenderTargetViews"), Out(Pointer(Pointer(ID3D10DepthStencilView)), "ppDepthStencilView")]),
    Method(Void, "OMGetBlendState", [Out(Pointer(Pointer(ID3D10BlendState)), "ppBlendState"), Out(Array(FLOAT, 4), "BlendFactor"), Out(Pointer(UINT), "pSampleMask")]),
    Method(Void, "OMGetDepthStencilState", [Out(Pointer(Pointer(ID3D10DepthStencilState)), "ppDepthStencilState"), Out(Pointer(UINT), "pStencilRef")]),
    Method(Void, "SOGetTargets", [(UINT, "NumBuffers"), Out(Array(OpaquePointer(ID3D10Buffer), "NumBuffers"), "ppSOTargets"), Out(Array(UINT, "NumBuffers"), "pOffsets")]),
    Method(Void, "RSGetState", [Out(Pointer(Pointer(ID3D10RasterizerState)), "ppRasterizerState")]),
    Method(Void, "RSGetViewports", [Out(Pointer(UINT), "NumViewports"), Out(Array(D3D10_VIEWPORT, "*NumViewports"), "pViewports")]),
    Method(Void, "RSGetScissorRects", [Out(Pointer(UINT), "NumRects"), Out(Array(D3D10_RECT, "*NumRects"), "pRects")]),
    Method(HRESULT, "GetDeviceRemovedReason", []),
    Method(HRESULT, "SetExceptionMode", [(UINT, "RaiseFlags")]),
    Method(UINT, "GetExceptionMode", []),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "guid"), Out(Pointer(UINT), "pDataSize"), Out(OpaquePointer(Void), "pData")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "guid"), (UINT, "DataSize"), (OpaquePointer(Const(Void)), "pData")]),
    Method(HRESULT, "SetPrivateDataInterface", [(REFGUID, "guid"), (OpaquePointer(Const(IUnknown)), "pData")]),
    Method(Void, "ClearState", []),
    Method(Void, "Flush", []),
    Method(HRESULT, "CreateBuffer", [(Pointer(Const(D3D10_BUFFER_DESC)), "pDesc"), (Pointer(Const(D3D10_SUBRESOURCE_DATA)), "pInitialData"), Out(Pointer(OpaquePointer(ID3D10Buffer)), "ppBuffer")]),
    Method(HRESULT, "CreateTexture1D", [(Pointer(Const(D3D10_TEXTURE1D_DESC)), "pDesc"), (Pointer(Const(D3D10_SUBRESOURCE_DATA)), "pInitialData"), Out(Pointer(Pointer(ID3D10Texture1D)), "ppTexture1D")]),
    Method(HRESULT, "CreateTexture2D", [(Pointer(Const(D3D10_TEXTURE2D_DESC)), "pDesc"), (Pointer(Const(D3D10_SUBRESOURCE_DATA)), "pInitialData"), Out(Pointer(Pointer(ID3D10Texture2D)), "ppTexture2D")]),
    Method(HRESULT, "CreateTexture3D", [(Pointer(Const(D3D10_TEXTURE3D_DESC)), "pDesc"), (Pointer(Const(D3D10_SUBRESOURCE_DATA)), "pInitialData"), Out(Pointer(Pointer(ID3D10Texture3D)), "ppTexture3D")]),
    Method(HRESULT, "CreateShaderResourceView", [(Pointer(ID3D10Resource), "pResource"), (Pointer(Const(D3D10_SHADER_RESOURCE_VIEW_DESC)), "pDesc"), Out(Pointer(OpaquePointer(ID3D10ShaderResourceView)), "ppSRView")]),
    Method(HRESULT, "CreateRenderTargetView", [(Pointer(ID3D10Resource), "pResource"), (Pointer(Const(D3D10_RENDER_TARGET_VIEW_DESC)), "pDesc"), Out(Pointer(OpaquePointer(ID3D10RenderTargetView)), "ppRTView")]),
    Method(HRESULT, "CreateDepthStencilView", [(Pointer(ID3D10Resource), "pResource"), (Pointer(Const(D3D10_DEPTH_STENCIL_VIEW_DESC)), "pDesc"), Out(Pointer(Pointer(ID3D10DepthStencilView)), "ppDepthStencilView")]),
    Method(HRESULT, "CreateInputLayout", [(Pointer(Const(D3D10_INPUT_ELEMENT_DESC)), "pInputElementDescs"), (UINT, "NumElements"), (Blob(Const(Void), "BytecodeLength"), "pShaderBytecodeWithInputSignature"), (SIZE_T, "BytecodeLength"), Out(Pointer(Pointer(ID3D10InputLayout)), "ppInputLayout")]),
    Method(HRESULT, "CreateVertexShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(Pointer(ID3D10VertexShader)), "ppVertexShader")]),
    Method(HRESULT, "CreateGeometryShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(Pointer(ID3D10GeometryShader)), "ppGeometryShader")]),
    Method(HRESULT, "CreateGeometryShaderWithStreamOutput", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), (Pointer(Const(D3D10_SO_DECLARATION_ENTRY)), "pSODeclaration"), (UINT, "NumEntries"), (UINT, "OutputStreamStride"), Out(Pointer(Pointer(ID3D10GeometryShader)), "ppGeometryShader")]),
    Method(HRESULT, "CreatePixelShader", [(Blob(Const(Void), "BytecodeLength"), "pShaderBytecode"), (SIZE_T, "BytecodeLength"), Out(Pointer(Pointer(ID3D10PixelShader)), "ppPixelShader")]),
    Method(HRESULT, "CreateBlendState", [(Pointer(Const(D3D10_BLEND_DESC)), "pBlendStateDesc"), Out(Pointer(Pointer(ID3D10BlendState)), "ppBlendState")]),
    Method(HRESULT, "CreateDepthStencilState", [(Pointer(Const(D3D10_DEPTH_STENCIL_DESC)), "pDepthStencilDesc"), Out(Pointer(Pointer(ID3D10DepthStencilState)), "ppDepthStencilState")]),
    Method(HRESULT, "CreateRasterizerState", [(Pointer(Const(D3D10_RASTERIZER_DESC)), "pRasterizerDesc"), Out(Pointer(Pointer(ID3D10RasterizerState)), "ppRasterizerState")]),
    Method(HRESULT, "CreateSamplerState", [(Pointer(Const(D3D10_SAMPLER_DESC)), "pSamplerDesc"), Out(Pointer(OpaquePointer(ID3D10SamplerState)), "ppSamplerState")]),
    Method(HRESULT, "CreateQuery", [(Pointer(Const(D3D10_QUERY_DESC)), "pQueryDesc"), Out(Pointer(Pointer(ID3D10Query)), "ppQuery")]),
    Method(HRESULT, "CreatePredicate", [(Pointer(Const(D3D10_QUERY_DESC)), "pPredicateDesc"), Out(Pointer(Pointer(ID3D10Predicate)), "ppPredicate")]),
    Method(HRESULT, "CreateCounter", [(Pointer(Const(D3D10_COUNTER_DESC)), "pCounterDesc"), Out(Pointer(Pointer(ID3D10Counter)), "ppCounter")]),
    Method(HRESULT, "CheckFormatSupport", [(DXGI_FORMAT, "Format"), Out(Pointer(UINT), "pFormatSupport")]),
    Method(HRESULT, "CheckMultisampleQualityLevels", [(DXGI_FORMAT, "Format"), (UINT, "SampleCount"), Out(Pointer(UINT), "pNumQualityLevels")]),
    Method(Void, "CheckCounterInfo", [Out(Pointer(D3D10_COUNTER_INFO), "pCounterInfo")]),
    Method(HRESULT, "CheckCounter", [(Pointer(Const(D3D10_COUNTER_DESC)), "pDesc"), Out(Pointer(D3D10_COUNTER_TYPE), "pType"), Out(Pointer(UINT), "pActiveCounters"), Out(LPSTR, "szName"), Out(Pointer(UINT), "pNameLength"), Out(LPSTR, "szUnits"), Out(Pointer(UINT), "pUnitsLength"), Out(LPSTR, "szDescription"), Out(Pointer(UINT), "pDescriptionLength")]),
    Method(UINT, "GetCreationFlags", []),
    Method(HRESULT, "OpenSharedResource", [(HANDLE, "hResource"), (REFIID, "ReturnedInterface"), Out(Pointer(OpaquePointer(Void)), "ppResource")]),
    Method(Void, "SetTextFilterSize", [(UINT, "Width"), (UINT, "Height")]),
    Method(Void, "GetTextFilterSize", [Out(Pointer(UINT), "pWidth"), Out(Pointer(UINT), "pHeight")]),
]

ID3D10Multithread.methods += [
    Method(Void, "Enter", []),
    Method(Void, "Leave", []),
    Method(BOOL, "SetMultithreadProtected", [(BOOL, "bMTProtect")]),
    Method(BOOL, "GetMultithreadProtected", []),
]


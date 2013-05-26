##########################################################################
#
# Copyright 2008-2012 VMware, Inc.
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

"""d3d10_1.h"""

from winapi import *
from d3d10 import *

D3D10_DRIVER_TYPE = Enum("D3D10_DRIVER_TYPE", [
    "D3D10_DRIVER_TYPE_HARDWARE",
    "D3D10_DRIVER_TYPE_REFERENCE",
    "D3D10_DRIVER_TYPE_NULL",
    "D3D10_DRIVER_TYPE_SOFTWARE",
    "D3D10_DRIVER_TYPE_WARP",
])

D3D10_FEATURE_LEVEL1 = Enum("D3D10_FEATURE_LEVEL1", [
    "D3D10_FEATURE_LEVEL_10_0",
    "D3D10_FEATURE_LEVEL_10_1",
	"D3D10_FEATURE_LEVEL_9_1",
	"D3D10_FEATURE_LEVEL_9_2",
	"D3D10_FEATURE_LEVEL_9_3",
])

D3D10_RENDER_TARGET_BLEND_DESC1 = Struct("D3D10_RENDER_TARGET_BLEND_DESC1", [
    (BOOL, "BlendEnable"),
    (D3D10_BLEND, "SrcBlend"),
    (D3D10_BLEND, "DestBlend"),
    (D3D10_BLEND_OP, "BlendOp"),
    (D3D10_BLEND, "SrcBlendAlpha"),
    (D3D10_BLEND, "DestBlendAlpha"),
    (D3D10_BLEND_OP, "BlendOpAlpha"),
    (UINT8, "RenderTargetWriteMask"),
])

D3D10_BLEND_DESC1 = Struct("D3D10_BLEND_DESC1", [
    (BOOL, "AlphaToCoverageEnable"),
    (BOOL, "IndependentBlendEnable"),
    (Array(D3D10_RENDER_TARGET_BLEND_DESC1, "D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT"), "RenderTarget"),
])

ID3D10BlendState1 = Interface("ID3D10BlendState1", ID3D10BlendState)
ID3D10BlendState1.methods += [
    StdMethod(Void, "GetDesc1", [Out(Pointer(D3D10_BLEND_DESC1), "pDesc")], sideeffects=False),
]

D3D10_TEXCUBE_ARRAY_SRV1 = Struct("D3D10_TEXCUBE_ARRAY_SRV1", [
    (UINT, "MostDetailedMip"),
    (UINT, "MipLevels"),
    (UINT, "First2DArrayFace"),
    (UINT, "NumCubes"),
])

D3D10_SHADER_RESOURCE_VIEW_DESC1 = Struct("D3D10_SHADER_RESOURCE_VIEW_DESC1", [
    (DXGI_FORMAT, "Format"),
    (D3D10_SRV_DIMENSION1, "ViewDimension"),
    (Union("{self}.ViewDimension", [
        ("D3D10_1_SRV_DIMENSION_BUFFER", D3D10_BUFFER_SRV, "Buffer"),
        ("D3D10_1_SRV_DIMENSION_TEXTURE1D", D3D10_TEX1D_SRV, "Texture1D"),
        ("D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY", D3D10_TEX1D_ARRAY_SRV, "Texture1DArray"),
        ("D3D10_1_SRV_DIMENSION_TEXTURE2D", D3D10_TEX2D_SRV, "Texture2D"), 
        ("D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY", D3D10_TEX2D_ARRAY_SRV, "Texture2DArray"),
        ("D3D10_1_SRV_DIMENSION_TEXTURE2DMS", D3D10_TEX2DMS_SRV, "Texture2DMS"),
        ("D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY", D3D10_TEX2DMS_ARRAY_SRV, "Texture2DMSArray"),
        ("D3D10_1_SRV_DIMENSION_TEXTURE3D", D3D10_TEX3D_SRV, "Texture3D"),
        ("D3D10_1_SRV_DIMENSION_TEXTURECUBE", D3D10_TEXCUBE_SRV, "TextureCube"),
        ("D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY", D3D10_TEXCUBE_ARRAY_SRV1, "TextureCubeArray"),
    ]), None),
])

ID3D10ShaderResourceView1 = Interface("ID3D10ShaderResourceView1", ID3D10ShaderResourceView)
ID3D10ShaderResourceView1.methods += [
    StdMethod(Void, "GetDesc1", [Out(Pointer(D3D10_SHADER_RESOURCE_VIEW_DESC1), "pDesc")], sideeffects=False),
]

ID3D10Device1 = Interface("ID3D10Device1", ID3D10Device)
ID3D10Device1.methods += [
    StdMethod(HRESULT, "CreateShaderResourceView1", [(ObjPointer(ID3D10Resource), "pResource"), (Pointer(Const(D3D10_SHADER_RESOURCE_VIEW_DESC1)), "pDesc"), Out(Pointer(ObjPointer(ID3D10ShaderResourceView1)), "ppSRView")]),
    StdMethod(HRESULT, "CreateBlendState1", [(Pointer(Const(D3D10_BLEND_DESC1)), "pBlendStateDesc"), Out(Pointer(ObjPointer(ID3D10BlendState1)), "ppBlendState")]),
    StdMethod(D3D10_FEATURE_LEVEL1, "GetFeatureLevel", [], sideeffects=False),
]

d3d10_1 = Module("d3d10_1")
d3d10_1.addFunctions([
    StdFunction(HRESULT, "D3D10CreateDevice1", [(ObjPointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (D3D10_CREATE_DEVICE_FLAG, "Flags"), (D3D10_FEATURE_LEVEL1, "HardwareLevel"), (UINT, "SDKVersion"), Out(Pointer(ObjPointer(ID3D10Device1)), "ppDevice")]),
    StdFunction(HRESULT, "D3D10CreateDeviceAndSwapChain1", [(ObjPointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (D3D10_CREATE_DEVICE_FLAG, "Flags"), (D3D10_FEATURE_LEVEL1, "HardwareLevel"), (UINT, "SDKVersion"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pSwapChainDesc"), Out(Pointer(ObjPointer(IDXGISwapChain)), "ppSwapChain"), Out(Pointer(ObjPointer(ID3D10Device1)), "ppDevice")]),
])

d3d10_1.addInterfaces([
    IDXGIAdapter1,
    IDXGIDevice1,
    IDXGIResource,
    ID3D10Debug,
    ID3D10InfoQueue,
    ID3D10Multithread,
    ID3D10SwitchToRef,
])

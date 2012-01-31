##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
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

"""d3d10misc.h"""

from winapi import *
from d3d10 import *


ID3D10Blob = Interface("ID3D10Blob", IUnknown)
LPD3D10BLOB = Pointer(ID3D10Blob)

ID3D10Blob.methods += [
    Method(LPVOID, "GetBufferPointer", []),
    Method(SIZE_T, "GetBufferSize", []),
]

D3D10_DRIVER_TYPE = Enum("D3D10_DRIVER_TYPE", [
    "D3D10_DRIVER_TYPE_HARDWARE",
    "D3D10_DRIVER_TYPE_REFERENCE",
    "D3D10_DRIVER_TYPE_NULL",
    "D3D10_DRIVER_TYPE_SOFTWARE",
    "D3D10_DRIVER_TYPE_WARP",
])


d3d10 = API("d3d10")

d3d10.addFunctions([
    StdFunction(HRESULT, "D3D10CreateDevice", [(Pointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (UINT, "Flags"), (UINT, "SDKVersion"), Out(Pointer(Pointer(ID3D10Device)), "ppDevice")]),
    StdFunction(HRESULT, "D3D10CreateDeviceAndSwapChain", [(Pointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (UINT, "Flags"), (UINT, "SDKVersion"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pSwapChainDesc"), Out(Pointer(Pointer(IDXGISwapChain)), "ppSwapChain"), Out(Pointer(Pointer(ID3D10Device)), "ppDevice")]),
    StdFunction(HRESULT, "D3D10CreateBlob", [(SIZE_T, "NumBytes"), Out(Pointer(LPD3D10BLOB), "ppBuffer")]),
])

d3d10.addInterfaces([
    ID3D10DeviceChild,
    ID3D10Resource,
    ID3D10Buffer,
    ID3D10Texture1D,
    ID3D10Texture2D,
    ID3D10Texture3D,
    ID3D10View,
    ID3D10DepthStencilView,
    ID3D10RenderTargetView,
    ID3D10ShaderResourceView,
    ID3D10BlendState,
    ID3D10DepthStencilState,
    ID3D10GeometryShader,
    ID3D10InputLayout,
    ID3D10PixelShader,
    ID3D10RasterizerState,
    ID3D10SamplerState,
    ID3D10VertexShader,
    ID3D10Asynchronous,
    ID3D10Counter,
    ID3D10Query,
    ID3D10Predicate,
    ID3D10Device,
    ID3D10Multithread,
])

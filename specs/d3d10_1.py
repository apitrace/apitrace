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

"""d3d10_1.h"""

from winapi import *

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

D3D10_FEATURE_LEVEL1 = Enum("D3D10_FEATURE_LEVEL1", [
    "D3D10_FEATURE_LEVEL_10_0",
    "D3D10_FEATURE_LEVEL_10_1",
	"D3D10_FEATURE_LEVEL_9_1",
	"D3D10_FEATURE_LEVEL_9_2",
	"D3D10_FEATURE_LEVEL_9_3",
])

# TODO
IDXGIAdapter = Alias("IDXGIAdapter", Void)
ID3D10Device1 = Alias("ID3D10Device1", Void)
IDXGISwapChain = Alias("IDXGISwapChain", Void)
DXGI_SWAP_CHAIN_DESC = Alias("DXGI_SWAP_CHAIN_DESC", Void)

d3d10_1 = Dll("d3d10")
d3d10_1.functions += [
    StdFunction(HRESULT, "D3D10CreateDevice1", [(Pointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (UINT, "Flags"), (D3D10_FEATURE_LEVEL1, "HardwareLevel"), (UINT, "SDKVersion"), (OutPointer(Pointer(ID3D10Device1)), "ppDevice")]),
    StdFunction(HRESULT, "D3D10CreateDeviceAndSwapChain1", [(Pointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (UINT, "Flags"), (D3D10_FEATURE_LEVEL1, "HardwareLevel"), (UINT, "SDKVersion"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pSwapChainDesc"), (OutPointer(Pointer(IDXGISwapChain)), "ppSwapChain"), (OutPointer(Pointer(ID3D10Device1)), "ppDevice")]),
    StdFunction(HRESULT, "D3D10CreateBlob", [(SIZE_T, "NumBytes"), (OutPointer(LPD3D10BLOB), "ppBuffer")]),
]

if __name__ == '__main__':
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print
    print '#include "compat.h"'
    print
    print '#include <d3d10_1.h>'
    print
    print '#include "trace_writer.hpp"'
    print
    wrap()

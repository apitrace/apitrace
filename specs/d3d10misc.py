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


D3D10_DRIVER_TYPE = Enum("D3D10_DRIVER_TYPE", [
    "D3D10_DRIVER_TYPE_HARDWARE",
    "D3D10_DRIVER_TYPE_REFERENCE",
    "D3D10_DRIVER_TYPE_NULL",
    "D3D10_DRIVER_TYPE_SOFTWARE",
    "D3D10_DRIVER_TYPE_WARP",
])


d3d10.addFunctions([
    StdFunction(HRESULT, "D3D10CreateDevice", [(ObjPointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (D3D10_CREATE_DEVICE_FLAG, "Flags"), (UINT, "SDKVersion"), Out(Pointer(ObjPointer(ID3D10Device)), "ppDevice")]),
    StdFunction(HRESULT, "D3D10CreateDeviceAndSwapChain", [(ObjPointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (D3D10_CREATE_DEVICE_FLAG, "Flags"), (UINT, "SDKVersion"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pSwapChainDesc"), Out(Pointer(ObjPointer(IDXGISwapChain)), "ppSwapChain"), Out(Pointer(ObjPointer(ID3D10Device)), "ppDevice")]),
])

d3d10.addInterfaces([
    IDXGIAdapter1,
    IDXGIDevice1,
    IDXGIResource,
    ID3D10Debug,
    ID3D10InfoQueue,
    ID3D10Multithread,
    ID3D10SwitchToRef,
])


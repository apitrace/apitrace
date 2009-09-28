#############################################################################
#
# Copyright 2009 VMware, Inc.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################

"""d3d10misc.h"""

from windows import *

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

# TODO
IDXGIAdapter = Alias("IDXGIAdapter", Void)
ID3D10Device = Alias("ID3D10Device", Void)
IDXGISwapChain = Alias("IDXGISwapChain", Void)
DXGI_SWAP_CHAIN_DESC = Alias("DXGI_SWAP_CHAIN_DESC", Void)

d3d10 = Dll("d3d10")
d3d10.functions += [
    DllFunction(HRESULT, "D3D10CreateDevice", [(Pointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (UINT, "Flags"), (UINT, "SDKVersion"), (OutPointer(Pointer(ID3D10Device)), "ppDevice")]),
    DllFunction(HRESULT, "D3D10CreateDeviceAndSwapChain", [(Pointer(IDXGIAdapter), "pAdapter"), (D3D10_DRIVER_TYPE, "DriverType"), (HMODULE, "Software"), (UINT, "Flags"), (UINT, "SDKVersion"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pSwapChainDesc"), (OutPointer(Pointer(IDXGISwapChain)), "ppSwapChain"), (OutPointer(Pointer(ID3D10Device)), "ppDevice")]),
    DllFunction(HRESULT, "D3D10CreateBlob", [(SIZE_T, "NumBytes"), (OutPointer(LPD3D10BLOB), "ppBuffer")]),
]

if __name__ == '__main__':
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print
    print '#include "compat.h"'
    print
    print '#include <d3d10.h>'
    print
    print '#include "log.hpp"'
    print
    wrap()

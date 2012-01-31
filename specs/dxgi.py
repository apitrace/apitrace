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

from dxgitype import *

HMONITOR = Alias("HMONITOR", HANDLE)
_LUID = Struct("_LUID", [
    (DWORD, "LowPart"),
    (LONG, "HighPart"),
])

DXGI_USAGE = Alias("DXGI_USAGE", UINT)
DXGI_SWAP_EFFECT = Enum("DXGI_SWAP_EFFECT", [
    "DXGI_SWAP_EFFECT_DISCARD",
    "DXGI_SWAP_EFFECT_SEQUENTIAL",
])

DXGI_RESIDENCY = Enum("DXGI_RESIDENCY", [
    "DXGI_RESIDENCY_FULLY_RESIDENT",
    "DXGI_RESIDENCY_RESIDENT_IN_SHARED_MEMORY",
    "DXGI_RESIDENCY_EVICTED_TO_DISK",
])

DXGI_SURFACE_DESC = Struct("DXGI_SURFACE_DESC", [
    (UINT, "Width"),
    (UINT, "Height"),
    (DXGI_FORMAT, "Format"),
    (DXGI_SAMPLE_DESC, "SampleDesc"),
])

DXGI_MAPPED_RECT = Struct("DXGI_MAPPED_RECT", [
    (INT, "Pitch"),
    (OpaquePointer(BYTE), "pBits"),
])

DXGI_OUTPUT_DESC = Struct("DXGI_OUTPUT_DESC", [
    (WString, "DeviceName"),
    (RECT, "DesktopCoordinates"),
    (BOOL, "AttachedToDesktop"),
    (DXGI_MODE_ROTATION, "Rotation"),
    (HMONITOR, "Monitor"),
])

DXGI_FRAME_STATISTICS = Struct("DXGI_FRAME_STATISTICS", [
    (UINT, "PresentCount"),
    (UINT, "PresentRefreshCount"),
    (UINT, "SyncRefreshCount"),
    (LARGE_INTEGER, "SyncQPCTime"),
    (LARGE_INTEGER, "SyncGPUTime"),
])

DXGI_ADAPTER_DESC = Struct("DXGI_ADAPTER_DESC", [
    (WString, "Description"),
    (UINT, "VendorId"),
    (UINT, "DeviceId"),
    (UINT, "SubSysId"),
    (UINT, "Revision"),
    (SIZE_T, "DedicatedVideoMemory"),
    (SIZE_T, "DedicatedSystemMemory"),
    (SIZE_T, "SharedSystemMemory"),
    (LUID, "AdapterLuid"),
])

DXGI_SWAP_CHAIN_FLAG = Enum("DXGI_SWAP_CHAIN_FLAG", [
    "DXGI_SWAP_CHAIN_FLAG_NONPREROTATED",
    "DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH",
    "DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE",
])

DXGI_SWAP_CHAIN_DESC = Struct("DXGI_SWAP_CHAIN_DESC", [
    (DXGI_MODE_DESC, "BufferDesc"),
    (DXGI_SAMPLE_DESC, "SampleDesc"),
    (DXGI_USAGE, "BufferUsage"),
    (UINT, "BufferCount"),
    (HWND, "OutputWindow"),
    (BOOL, "Windowed"),
    (DXGI_SWAP_EFFECT, "SwapEffect"),
    (UINT, "Flags"),
])

DXGI_SHARED_RESOURCE = Struct("DXGI_SHARED_RESOURCE", [
    (HANDLE, "Handle"),
])

IDXGIObject = Interface("IDXGIObject", IUnknown)
IDXGIObject.methods += [
    Method(HRESULT, "SetPrivateData", [(REFGUID, "guid"), (UINT, "data_size"), (OpaquePointer(Const(Void)), "data")]),
    Method(HRESULT, "SetPrivateDataInterface", [(REFGUID, "guid"), (OpaquePointer(Const(IUnknown)), "object")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "guid"), Out(Pointer(UINT), "data_size"), Out(OpaquePointer(Void), "data")]),
    Method(HRESULT, "GetParent", [(REFIID, "riid"), Out(Pointer(OpaquePointer(Void)), "parent")]),
]

IDXGIDeviceSubObject = Interface("IDXGIDeviceSubObject", IDXGIObject)
IDXGIDeviceSubObject.methods += [
    Method(HRESULT, "GetDevice", [(REFIID, "riid"), Out(Pointer(OpaquePointer(Void)), "device")]),
]

IDXGISurface = Interface("IDXGISurface", IDXGIDeviceSubObject)
IDXGISurface.methods += [
    Method(HRESULT, "GetDesc", [Out(Pointer(DXGI_SURFACE_DESC), "desc")]),
    Method(HRESULT, "Map", [Out(Pointer(DXGI_MAPPED_RECT), "mapped_rect"), (UINT, "flags")]),
    Method(HRESULT, "Unmap", []),
]

IDXGIOutput = Interface("IDXGIOutput", IDXGIObject)
IDXGIOutput.methods += [
    Method(HRESULT, "GetDesc", [Out(Pointer(DXGI_OUTPUT_DESC), "desc")]),
    Method(HRESULT, "GetDisplayModeList", [(DXGI_FORMAT, "format"), (UINT, "flags"), Out(Pointer(UINT), "mode_count"), Out(Array(DXGI_MODE_DESC, "*mode_count"), "desc")]),
    Method(HRESULT, "FindClosestMatchingMode", [(Pointer(Const(DXGI_MODE_DESC)), "mode"), Out(Pointer(DXGI_MODE_DESC), "closest_match"), (Pointer(IUnknown), "device")]),
    Method(HRESULT, "WaitForVBlank", []),
    Method(HRESULT, "TakeOwnership", [(Pointer(IUnknown), "device"), (BOOL, "exclusive")]),
    Method(Void, "ReleaseOwnership", []),
    Method(HRESULT, "GetGammaControlCapabilities", [Out(Pointer(DXGI_GAMMA_CONTROL_CAPABILITIES), "gamma_caps")]),
    Method(HRESULT, "SetGammaControl", [(Pointer(Const(DXGI_GAMMA_CONTROL)), "gamma_control")]),
    Method(HRESULT, "GetGammaControl", [Out(Pointer(DXGI_GAMMA_CONTROL), "gamma_control")]),
    Method(HRESULT, "SetDisplaySurface", [(Pointer(IDXGISurface), "surface")]),
    Method(HRESULT, "GetDisplaySurfaceData", [(Pointer(IDXGISurface), "surface")]),
    Method(HRESULT, "GetFrameStatistics", [Out(Pointer(DXGI_FRAME_STATISTICS), "stats")]),
]

IDXGIAdapter = Interface("IDXGIAdapter", IDXGIObject)
IDXGIAdapter.methods += [
    Method(HRESULT, "EnumOutputs", [(UINT, "output_idx"), Out(Pointer(Pointer(IDXGIOutput)), "output")]),
    Method(HRESULT, "GetDesc", [Out(Pointer(DXGI_ADAPTER_DESC), "desc")]),
    Method(HRESULT, "CheckInterfaceSupport", [(REFGUID, "guid"), Out(Pointer(LARGE_INTEGER), "umd_version")]),
]

IDXGISwapChain = Interface("IDXGISwapChain", IDXGIDeviceSubObject)
IDXGISwapChain.methods += [
    Method(HRESULT, "Present", [(UINT, "sync_interval"), (UINT, "flags")]),
    Method(HRESULT, "GetBuffer", [(UINT, "buffer_idx"), (REFIID, "riid"), Out(Pointer(OpaquePointer(Void)), "surface")]),
    Method(HRESULT, "SetFullscreenState", [(BOOL, "fullscreen"), (Pointer(IDXGIOutput), "target")]),
    Method(HRESULT, "GetFullscreenState", [Out(Pointer(BOOL), "fullscreen"), Out(Pointer(Pointer(IDXGIOutput)), "target")]),
    Method(HRESULT, "GetDesc", [Out(Pointer(DXGI_SWAP_CHAIN_DESC), "desc")]),
    Method(HRESULT, "ResizeBuffers", [(UINT, "buffer_count"), (UINT, "width"), (UINT, "height"), (DXGI_FORMAT, "format"), (UINT, "flags")]),
    Method(HRESULT, "ResizeTarget", [(Pointer(Const(DXGI_MODE_DESC)), "target_mode_desc")]),
    Method(HRESULT, "GetContainingOutput", [Out(Pointer(Pointer(IDXGIOutput)), "output")]),
    Method(HRESULT, "GetFrameStatistics", [Out(Pointer(DXGI_FRAME_STATISTICS), "stats")]),
    Method(HRESULT, "GetLastPresentCount", [Out(Pointer(UINT), "last_present_count")]),
]

IDXGIFactory = Interface("IDXGIFactory", IDXGIObject)
IDXGIFactory.methods += [
    Method(HRESULT, "EnumAdapters", [(UINT, "adapter_idx"), Out(Pointer(Pointer(IDXGIAdapter)), "adapter")]),
    Method(HRESULT, "MakeWindowAssociation", [(HWND, "window"), (UINT, "flags")]),
    Method(HRESULT, "GetWindowAssociation", [Out(Pointer(HWND), "window")]),
    Method(HRESULT, "CreateSwapChain", [(Pointer(IUnknown), "device"), (Pointer(DXGI_SWAP_CHAIN_DESC), "desc"), Out(Pointer(Pointer(IDXGISwapChain)), "swapchain")]),
    Method(HRESULT, "CreateSoftwareAdapter", [(HMODULE, "swrast"), Out(Pointer(Pointer(IDXGIAdapter)), "adapter")]),
]

#StdFunction(HRESULT, "CreateDXGIFactory", [(REFIID, "riid"), (Pointer(OpaquePointer(Void)), "factory")]),

IDXGIDevice = Interface("IDXGIDevice", IDXGIObject)
IDXGIDevice.methods += [
    Method(HRESULT, "GetAdapter", [Out(Pointer(Pointer(IDXGIAdapter)), "adapter")]),
    Method(HRESULT, "CreateSurface", [(Pointer(Const(DXGI_SURFACE_DESC)), "desc"), (UINT, "surface_count"), (DXGI_USAGE, "usage"), (Pointer(Const(DXGI_SHARED_RESOURCE)), "shared_resource"), Out(Pointer(Pointer(IDXGISurface)), "surface")]),
    Method(HRESULT, "QueryResourceResidency", [(Array(Const(OpaquePointer(IUnknown)), "resource_count"), "resources"), Out(Array(DXGI_RESIDENCY, "resource_count"), "residency"), (UINT, "resource_count")]),
    Method(HRESULT, "SetGPUThreadPriority", [(INT, "priority")]),
    Method(HRESULT, "GetGPUThreadPriority", [Out(Pointer(INT), "priority")]),
]


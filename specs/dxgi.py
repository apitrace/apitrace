##########################################################################
#
# Copyright 2014 VMware, Inc
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


from winapi import *


DXGI_FORMAT = Enum("DXGI_FORMAT", [
    "DXGI_FORMAT_UNKNOWN",
    "DXGI_FORMAT_R32G32B32A32_TYPELESS",
    "DXGI_FORMAT_R32G32B32A32_FLOAT",
    "DXGI_FORMAT_R32G32B32A32_UINT",
    "DXGI_FORMAT_R32G32B32A32_SINT",
    "DXGI_FORMAT_R32G32B32_TYPELESS",
    "DXGI_FORMAT_R32G32B32_FLOAT",
    "DXGI_FORMAT_R32G32B32_UINT",
    "DXGI_FORMAT_R32G32B32_SINT",
    "DXGI_FORMAT_R16G16B16A16_TYPELESS",
    "DXGI_FORMAT_R16G16B16A16_FLOAT",
    "DXGI_FORMAT_R16G16B16A16_UNORM",
    "DXGI_FORMAT_R16G16B16A16_UINT",
    "DXGI_FORMAT_R16G16B16A16_SNORM",
    "DXGI_FORMAT_R16G16B16A16_SINT",
    "DXGI_FORMAT_R32G32_TYPELESS",
    "DXGI_FORMAT_R32G32_FLOAT",
    "DXGI_FORMAT_R32G32_UINT",
    "DXGI_FORMAT_R32G32_SINT",
    "DXGI_FORMAT_R32G8X24_TYPELESS",
    "DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
    "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
    "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
    "DXGI_FORMAT_R10G10B10A2_TYPELESS",
    "DXGI_FORMAT_R10G10B10A2_UNORM",
    "DXGI_FORMAT_R10G10B10A2_UINT",
    "DXGI_FORMAT_R11G11B10_FLOAT",
    "DXGI_FORMAT_R8G8B8A8_TYPELESS",
    "DXGI_FORMAT_R8G8B8A8_UNORM",
    "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
    "DXGI_FORMAT_R8G8B8A8_UINT",
    "DXGI_FORMAT_R8G8B8A8_SNORM",
    "DXGI_FORMAT_R8G8B8A8_SINT",
    "DXGI_FORMAT_R16G16_TYPELESS",
    "DXGI_FORMAT_R16G16_FLOAT",
    "DXGI_FORMAT_R16G16_UNORM",
    "DXGI_FORMAT_R16G16_UINT",
    "DXGI_FORMAT_R16G16_SNORM",
    "DXGI_FORMAT_R16G16_SINT",
    "DXGI_FORMAT_R32_TYPELESS",
    "DXGI_FORMAT_D32_FLOAT",
    "DXGI_FORMAT_R32_FLOAT",
    "DXGI_FORMAT_R32_UINT",
    "DXGI_FORMAT_R32_SINT",
    "DXGI_FORMAT_R24G8_TYPELESS",
    "DXGI_FORMAT_D24_UNORM_S8_UINT",
    "DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
    "DXGI_FORMAT_X24_TYPELESS_G8_UINT",
    "DXGI_FORMAT_R8G8_TYPELESS",
    "DXGI_FORMAT_R8G8_UNORM",
    "DXGI_FORMAT_R8G8_UINT",
    "DXGI_FORMAT_R8G8_SNORM",
    "DXGI_FORMAT_R8G8_SINT",
    "DXGI_FORMAT_R16_TYPELESS",
    "DXGI_FORMAT_R16_FLOAT",
    "DXGI_FORMAT_D16_UNORM",
    "DXGI_FORMAT_R16_UNORM",
    "DXGI_FORMAT_R16_UINT",
    "DXGI_FORMAT_R16_SNORM",
    "DXGI_FORMAT_R16_SINT",
    "DXGI_FORMAT_R8_TYPELESS",
    "DXGI_FORMAT_R8_UNORM",
    "DXGI_FORMAT_R8_UINT",
    "DXGI_FORMAT_R8_SNORM",
    "DXGI_FORMAT_R8_SINT",
    "DXGI_FORMAT_A8_UNORM",
    "DXGI_FORMAT_R1_UNORM",
    "DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
    "DXGI_FORMAT_R8G8_B8G8_UNORM",
    "DXGI_FORMAT_G8R8_G8B8_UNORM",
    "DXGI_FORMAT_BC1_TYPELESS",
    "DXGI_FORMAT_BC1_UNORM",
    "DXGI_FORMAT_BC1_UNORM_SRGB",
    "DXGI_FORMAT_BC2_TYPELESS",
    "DXGI_FORMAT_BC2_UNORM",
    "DXGI_FORMAT_BC2_UNORM_SRGB",
    "DXGI_FORMAT_BC3_TYPELESS",
    "DXGI_FORMAT_BC3_UNORM",
    "DXGI_FORMAT_BC3_UNORM_SRGB",
    "DXGI_FORMAT_BC4_TYPELESS",
    "DXGI_FORMAT_BC4_UNORM",
    "DXGI_FORMAT_BC4_SNORM",
    "DXGI_FORMAT_BC5_TYPELESS",
    "DXGI_FORMAT_BC5_UNORM",
    "DXGI_FORMAT_BC5_SNORM",
    "DXGI_FORMAT_B5G6R5_UNORM",
    "DXGI_FORMAT_B5G5R5A1_UNORM",
    "DXGI_FORMAT_B8G8R8A8_UNORM",
    "DXGI_FORMAT_B8G8R8X8_UNORM",
    "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM",
    "DXGI_FORMAT_B8G8R8A8_TYPELESS",
    "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
    "DXGI_FORMAT_B8G8R8X8_TYPELESS",
    "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
    "DXGI_FORMAT_BC6H_TYPELESS",
    "DXGI_FORMAT_BC6H_UF16",
    "DXGI_FORMAT_BC6H_SF16",
    "DXGI_FORMAT_BC7_TYPELESS",
    "DXGI_FORMAT_BC7_UNORM",
    "DXGI_FORMAT_BC7_UNORM_SRGB",
    "DXGI_FORMAT_AYUV",
    "DXGI_FORMAT_Y410",
    "DXGI_FORMAT_Y416",
    "DXGI_FORMAT_NV12",
    "DXGI_FORMAT_P010",
    "DXGI_FORMAT_P016",
    "DXGI_FORMAT_420_OPAQUE",
    "DXGI_FORMAT_YUY2",
    "DXGI_FORMAT_Y210",
    "DXGI_FORMAT_Y216",
    "DXGI_FORMAT_NV11",
    "DXGI_FORMAT_AI44",
    "DXGI_FORMAT_IA44",
    "DXGI_FORMAT_P8",
    "DXGI_FORMAT_A8P8",
    "DXGI_FORMAT_B4G4R4A4_UNORM",
])


HRESULT = MAKE_HRESULT([
    "DXGI_STATUS_OCCLUDED",
    "DXGI_STATUS_CLIPPED",
    "DXGI_STATUS_NO_REDIRECTION",
    "DXGI_STATUS_NO_DESKTOP_ACCESS",
    "DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE",
    "DXGI_STATUS_MODE_CHANGED",
    "DXGI_STATUS_MODE_CHANGE_IN_PROGRESS",
    "DXGI_ERROR_INVALID_CALL",
    "DXGI_ERROR_NOT_FOUND",
    "DXGI_ERROR_MORE_DATA",
    "DXGI_ERROR_UNSUPPORTED",
    "DXGI_ERROR_DEVICE_REMOVED",
    "DXGI_ERROR_DEVICE_HUNG",
    "DXGI_ERROR_DEVICE_RESET",
    "DXGI_ERROR_WAS_STILL_DRAWING",
    "DXGI_ERROR_FRAME_STATISTICS_DISJOINT",
    "DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE",
    "DXGI_ERROR_DRIVER_INTERNAL_ERROR",
    "DXGI_ERROR_NONEXCLUSIVE",
    "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE",
    "DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED",
    "DXGI_ERROR_REMOTE_OUTOFMEMORY",
    # IDXGIKeyedMutex::AcquireSync
    "WAIT_ABANDONED",
    "WAIT_TIMEOUT",
])


DXGI_RGB = Struct("DXGI_RGB", [
    (Float, "Red"),
    (Float, "Green"),
    (Float, "Blue"),
])

DXGI_GAMMA_CONTROL = Struct("DXGI_GAMMA_CONTROL", [
    (DXGI_RGB, "Scale"),
    (DXGI_RGB, "Offset"),
    (Array(DXGI_RGB, 1025), "GammaCurve"),
])

DXGI_GAMMA_CONTROL_CAPABILITIES = Struct("DXGI_GAMMA_CONTROL_CAPABILITIES", [
    (BOOL, "ScaleAndOffsetSupported"),
    (Float, "MaxConvertedValue"),
    (Float, "MinConvertedValue"),
    (UINT, "NumGammaControlPoints"),
    (Array(Float, "{self}.NumGammaControlPoints"), "ControlPointPositions"),
])

DXGI_RATIONAL = Struct("DXGI_RATIONAL", [
    (UINT, "Numerator"),
    (UINT, "Denominator"),
])

DXGI_MODE_SCANLINE_ORDER = Enum("DXGI_MODE_SCANLINE_ORDER", [
    "DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
    "DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
    "DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
    "DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST",
])

DXGI_MODE_SCALING = Enum("DXGI_MODE_SCALING", [
    "DXGI_MODE_SCALING_UNSPECIFIED",
    "DXGI_MODE_SCALING_CENTERED",
    "DXGI_MODE_SCALING_STRETCHED",
])

DXGI_MODE_ROTATION = Enum("DXGI_MODE_ROTATION", [
    "DXGI_MODE_ROTATION_UNSPECIFIED",
    "DXGI_MODE_ROTATION_IDENTITY",
    "DXGI_MODE_ROTATION_ROTATE90",
    "DXGI_MODE_ROTATION_ROTATE180",
    "DXGI_MODE_ROTATION_ROTATE270",
])

DXGI_MODE_DESC = Struct("DXGI_MODE_DESC", [
    (UINT, "Width"),
    (UINT, "Height"),
    (DXGI_RATIONAL, "RefreshRate"),
    (DXGI_FORMAT, "Format"),
    (DXGI_MODE_SCANLINE_ORDER, "ScanlineOrdering"),
    (DXGI_MODE_SCALING, "Scaling"),
])

DXGI_SAMPLE_DESC = Struct("DXGI_SAMPLE_DESC", [
    (UINT, "Count"),
    (UINT, "Quality"),
])

DXGI_RGBA = Struct("DXGI_RGBA", [
    (Float, "r"),
    (Float, "g"),
    (Float, "b"),
    (Float, "a"),
])


IDXGIObject = Interface("IDXGIObject", IUnknown)
IDXGIDeviceSubObject = Interface("IDXGIDeviceSubObject", IDXGIObject)
IDXGIResource = Interface("IDXGIResource", IDXGIDeviceSubObject)
IDXGIKeyedMutex = Interface("IDXGIKeyedMutex", IDXGIDeviceSubObject)
IDXGISurface = Interface("IDXGISurface", IDXGIDeviceSubObject)
IDXGISurface1 = Interface("IDXGISurface1", IDXGISurface)
IDXGIAdapter = Interface("IDXGIAdapter", IDXGIObject)
IDXGIOutput = Interface("IDXGIOutput", IDXGIObject)
IDXGISwapChain = Interface("IDXGISwapChain", IDXGIDeviceSubObject)
IDXGIFactory = Interface("IDXGIFactory", IDXGIObject)
IDXGIDevice = Interface("IDXGIDevice", IDXGIObject)
IDXGIFactory1 = Interface("IDXGIFactory1", IDXGIFactory)
IDXGIAdapter1 = Interface("IDXGIAdapter1", IDXGIAdapter)
IDXGIDevice1 = Interface("IDXGIDevice1", IDXGIDevice)


DXGI_USAGE = Flags(UINT, [
    "DXGI_CPU_ACCESS_NONE", # 0
    "DXGI_CPU_ACCESS_SCRATCH", # 3
    "DXGI_CPU_ACCESS_DYNAMIC", # 1
    "DXGI_CPU_ACCESS_READ_WRITE", # 2
    "DXGI_USAGE_SHADER_INPUT",
    "DXGI_USAGE_RENDER_TARGET_OUTPUT",
    "DXGI_USAGE_BACK_BUFFER",
    "DXGI_USAGE_SHARED",
    "DXGI_USAGE_READ_ONLY",
    "DXGI_USAGE_DISCARD_ON_PRESENT",
    "DXGI_USAGE_UNORDERED_ACCESS",
])

DXGI_FRAME_STATISTICS = Struct("DXGI_FRAME_STATISTICS", [
    (UINT, "PresentCount"),
    (UINT, "PresentRefreshCount"),
    (UINT, "SyncRefreshCount"),
    (LARGE_INTEGER, "SyncQPCTime"),
    (LARGE_INTEGER, "SyncGPUTime"),
])

DXGI_MAPPED_RECT = Struct("DXGI_MAPPED_RECT", [
    (INT, "Pitch"),
    (LinearPointer(BYTE, "_MappedSize"), "pBits"),
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

DXGI_OUTPUT_DESC = Struct("DXGI_OUTPUT_DESC", [
    (WString, "DeviceName"),
    (RECT, "DesktopCoordinates"),
    (BOOL, "AttachedToDesktop"),
    (DXGI_MODE_ROTATION, "Rotation"),
    (HMONITOR, "Monitor"),
])

DXGI_SHARED_RESOURCE = Struct("DXGI_SHARED_RESOURCE", [
    (HANDLE, "Handle"),
])

DXGI_RESOURCE_PRIORITY = FakeEnum(UINT, [
    "DXGI_RESOURCE_PRIORITY_MINIMUM",
    "DXGI_RESOURCE_PRIORITY_LOW",
    "DXGI_RESOURCE_PRIORITY_NORMAL",
    "DXGI_RESOURCE_PRIORITY_HIGH",
    "DXGI_RESOURCE_PRIORITY_MAXIMUM",
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

DXGI_SWAP_EFFECT = Enum("DXGI_SWAP_EFFECT", [
    "DXGI_SWAP_EFFECT_DISCARD",
    "DXGI_SWAP_EFFECT_SEQUENTIAL",
    "DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL",
])

DXGI_SWAP_CHAIN_FLAG = Flags(UINT, [
    "DXGI_SWAP_CHAIN_FLAG_NONPREROTATED",
    "DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH",
    "DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE",
    "DXGI_SWAP_CHAIN_FLAG_RESTRICTED_CONTENT",
    "DXGI_SWAP_CHAIN_FLAG_RESTRICT_SHARED_RESOURCE_DRIVER",
    "DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY",
    "DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT",
    "DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER",
    "DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO",
    "DXGI_SWAP_CHAIN_FLAG_YUV_VIDEO",
])

DXGI_SWAP_CHAIN_DESC = Struct("DXGI_SWAP_CHAIN_DESC", [
    (DXGI_MODE_DESC, "BufferDesc"),
    (DXGI_SAMPLE_DESC, "SampleDesc"),
    (DXGI_USAGE, "BufferUsage"),
    (UINT, "BufferCount"),
    (HWND, "OutputWindow"),
    (BOOL, "Windowed"),
    (DXGI_SWAP_EFFECT, "SwapEffect"),
    (DXGI_SWAP_CHAIN_FLAG, "Flags"),
])


IDXGIObject.methods += [
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "Name"), (UINT, "DataSize"), (OpaqueBlob(Const(Void), "DataSize"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetPrivateDataInterface", [(REFGUID, "Name"), (OpaquePointer(Const(IUnknown)), "pUnknown")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "Name"), InOut(Pointer(UINT), "pDataSize"), Out(OpaquePointer(Void), "pData")], sideeffects=False),
    StdMethod(HRESULT, "GetParent", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppParent")]),
]

IDXGIDeviceSubObject.methods += [
    StdMethod(HRESULT, "GetDevice", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppDevice")]),
]

SHARED_HANDLE = Handle("shared_handle", RAW_HANDLE)

IDXGIResource.methods += [
    StdMethod(HRESULT, "GetSharedHandle", [Out(Pointer(SHARED_HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "GetUsage", [Out(Pointer(DXGI_USAGE), "pUsage")], sideeffects=False),
    StdMethod(HRESULT, "SetEvictionPriority", [(DXGI_RESOURCE_PRIORITY, "EvictionPriority")]),
    StdMethod(HRESULT, "GetEvictionPriority", [Out(Pointer(DXGI_RESOURCE_PRIORITY), "pEvictionPriority")], sideeffects=False),
]

DWORD_TIMEOUT = FakeEnum(DWORD, [
    "INFINITE",
])

IDXGIKeyedMutex.methods += [
    StdMethod(HRESULT, "AcquireSync", [(UINT64, "Key"), (DWORD_TIMEOUT, "dwMilliseconds")]),
    StdMethod(HRESULT, "ReleaseSync", [(UINT64, "Key")]),
]

DXGI_MAP = Flags(UINT, [
    "DXGI_MAP_READ",
    "DXGI_MAP_WRITE",
    "DXGI_MAP_DISCARD",
])

IDXGISurface.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_SURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "Map", [Out(Pointer(DXGI_MAPPED_RECT), "pLockedRect"), (DXGI_MAP, "MapFlags")]),
    StdMethod(HRESULT, "Unmap", []),
]

IDXGISurface1.methods += [
    StdMethod(HRESULT, "GetDC", [(BOOL, "Discard"), Out(Pointer(HDC), "phdc")]),
    StdMethod(HRESULT, "ReleaseDC", [(Pointer(RECT), "pDirtyRect")]),
]

IDXGIAdapter.methods += [
    StdMethod(HRESULT, "EnumOutputs", [(UINT, "Output"), Out(Pointer(ObjPointer(IDXGIOutput)), "ppOutput")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_ADAPTER_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "CheckInterfaceSupport", [(REFGUID, "InterfaceName"), Out(Pointer(LARGE_INTEGER), "pUMDVersion")], sideeffects=False),
]

DXGI_ENUM_MODES = Flags(UINT, [
    "DXGI_ENUM_MODES_INTERLACED",
    "DXGI_ENUM_MODES_SCALING",
    "DXGI_ENUM_MODES_STEREO",
    "DXGI_ENUM_MODES_DISABLED_STEREO",
])

IDXGIOutput.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_OUTPUT_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplayModeList", [(DXGI_FORMAT, "EnumFormat"), (DXGI_ENUM_MODES, "Flags"), InOut(Pointer(UINT), "pNumModes"), Out(Array(DXGI_MODE_DESC, "*pNumModes"), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "FindClosestMatchingMode", [(Pointer(Const(DXGI_MODE_DESC)), "pModeToMatch"), Out(Pointer(DXGI_MODE_DESC), "pClosestMatch"), (ObjPointer(IUnknown), "pConcernedDevice")], sideeffects=False),
    StdMethod(HRESULT, "WaitForVBlank", []),
    StdMethod(HRESULT, "TakeOwnership", [(ObjPointer(IUnknown), "pDevice"), (BOOL, "Exclusive")]),
    StdMethod(Void, "ReleaseOwnership", []),
    StdMethod(HRESULT, "GetGammaControlCapabilities", [Out(Pointer(DXGI_GAMMA_CONTROL_CAPABILITIES), "pGammaCaps")], sideeffects=False),
    StdMethod(HRESULT, "SetGammaControl", [(Pointer(Const(DXGI_GAMMA_CONTROL)), "pArray")]),
    StdMethod(HRESULT, "GetGammaControl", [Out(Pointer(DXGI_GAMMA_CONTROL), "pArray")], sideeffects=False),
    StdMethod(HRESULT, "SetDisplaySurface", [(ObjPointer(IDXGISurface), "pScanoutSurface")]),
    StdMethod(HRESULT, "GetDisplaySurfaceData", [(ObjPointer(IDXGISurface), "pDestination")]),
    StdMethod(HRESULT, "GetFrameStatistics", [Out(Pointer(DXGI_FRAME_STATISTICS), "pStats")], sideeffects=False),
]

DXGI_PRESENT = Flags(UINT, [
    "DXGI_PRESENT_TEST",
    "DXGI_PRESENT_DO_NOT_SEQUENCE",
    "DXGI_PRESENT_RESTART",
    "DXGI_PRESENT_DO_NOT_WAIT",
    "DXGI_PRESENT_STEREO_PREFER_RIGHT",
    "DXGI_PRESENT_STEREO_TEMPORARY_MONO",
    "DXGI_PRESENT_RESTRICT_TO_OUTPUT",
    "DXGI_PRESENT_USE_DURATION",
])

IDXGISwapChain.methods += [
    StdMethod(HRESULT, "Present", [(UINT, "SyncInterval"), (DXGI_PRESENT, "Flags")]),
    StdMethod(HRESULT, "GetBuffer", [(UINT, "Buffer"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppSurface")]),
    StdMethod(HRESULT, "SetFullscreenState", [(BOOL, "Fullscreen"), (ObjPointer(IDXGIOutput), "pTarget")]),
    StdMethod(HRESULT, "GetFullscreenState", [Out(Pointer(BOOL), "pFullscreen"), Out(Pointer(ObjPointer(IDXGIOutput)), "ppTarget")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_SWAP_CHAIN_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "ResizeBuffers", [(UINT, "BufferCount"), (UINT, "Width"), (UINT, "Height"), (DXGI_FORMAT, "NewFormat"), (DXGI_SWAP_CHAIN_FLAG, "SwapChainFlags")]),
    StdMethod(HRESULT, "ResizeTarget", [(Pointer(Const(DXGI_MODE_DESC)), "pNewTargetParameters")]),
    StdMethod(HRESULT, "GetContainingOutput", [Out(Pointer(ObjPointer(IDXGIOutput)), "ppOutput")]),
    StdMethod(HRESULT, "GetFrameStatistics", [Out(Pointer(DXGI_FRAME_STATISTICS), "pStats")], sideeffects=False),
    StdMethod(HRESULT, "GetLastPresentCount", [Out(Pointer(UINT), "pLastPresentCount")], sideeffects=False),
]

DXGI_MWA = Flags(UINT, [
    "DXGI_MWA_NO_WINDOW_CHANGES",
    "DXGI_MWA_NO_ALT_ENTER",
    "DXGI_MWA_NO_PRINT_SCREEN",
    "DXGI_MWA_VALID",
])

IDXGIFactory.methods += [
    StdMethod(HRESULT, "EnumAdapters", [(UINT, "Adapter"), Out(Pointer(ObjPointer(IDXGIAdapter)), "ppAdapter")]),
    StdMethod(HRESULT, "MakeWindowAssociation", [(HWND, "WindowHandle"), (DXGI_MWA, "Flags")], sideeffects=False),
    StdMethod(HRESULT, "GetWindowAssociation", [Out(Pointer(HWND), "pWindowHandle")], sideeffects=False),
    StdMethod(HRESULT, "CreateSwapChain", [(ObjPointer(IUnknown), "pDevice"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pDesc"), Out(Pointer(ObjPointer(IDXGISwapChain)), "ppSwapChain")]),
    StdMethod(HRESULT, "CreateSoftwareAdapter", [(HMODULE, "Module"), Out(Pointer(ObjPointer(IDXGIAdapter)), "ppAdapter")]),
]

IDXGIDevice.methods += [
    StdMethod(HRESULT, "GetAdapter", [Out(Pointer(ObjPointer(IDXGIAdapter)), "pAdapter")]),
    StdMethod(HRESULT, "CreateSurface", [(Pointer(Const(DXGI_SURFACE_DESC)), "pDesc"), (UINT, "NumSurfaces"), (DXGI_USAGE, "Usage"), (Pointer(Const(DXGI_SHARED_RESOURCE)), "pSharedResource"), Out(Pointer(ObjPointer(IDXGISurface)), "ppSurface")]),
    StdMethod(HRESULT, "QueryResourceResidency", [(Array(Const(ObjPointer(IUnknown)), "NumResources"), "ppResources"), Out(Array(DXGI_RESIDENCY, "NumResources"), "pResidencyStatus"), (UINT, "NumResources")], sideeffects=False),
    StdMethod(HRESULT, "SetGPUThreadPriority", [(INT, "Priority")]),
    StdMethod(HRESULT, "GetGPUThreadPriority", [Out(Pointer(INT), "pPriority")], sideeffects=False),
]

DXGI_ADAPTER_FLAG = FakeEnum(UINT, [
    "DXGI_ADAPTER_FLAG_NONE",
    "DXGI_ADAPTER_FLAG_REMOTE",
    "DXGI_ADAPTER_FLAG_SOFTWARE",
])

DXGI_ADAPTER_DESC1 = Struct("DXGI_ADAPTER_DESC1", [
    (WString, "Description"),
    (UINT, "VendorId"),
    (UINT, "DeviceId"),
    (UINT, "SubSysId"),
    (UINT, "Revision"),
    (SIZE_T, "DedicatedVideoMemory"),
    (SIZE_T, "DedicatedSystemMemory"),
    (SIZE_T, "SharedSystemMemory"),
    (LUID, "AdapterLuid"),
    (DXGI_SWAP_CHAIN_FLAG, "Flags"),
])

DXGI_DISPLAY_COLOR_SPACE = Struct("DXGI_DISPLAY_COLOR_SPACE", [
    (Array(Array(FLOAT, 8), 2), "PrimaryCoordinates"),
    (Array(Array(FLOAT, 16), 2), "WhitePoints"),
])

IDXGIFactory1.methods += [
    StdMethod(HRESULT, "EnumAdapters1", [(UINT, "Adapter"), Out(Pointer(ObjPointer(IDXGIAdapter1)), "ppAdapter")]),
    StdMethod(BOOL, "IsCurrent", []),
]

IDXGIAdapter1.methods += [
    StdMethod(HRESULT, "GetDesc1", [Out(Pointer(DXGI_ADAPTER_DESC1), "pDesc")], sideeffects=False),
]

IDXGIDevice1.methods += [
    StdMethod(HRESULT, "SetMaximumFrameLatency", [(UINT, "MaxLatency")]),
    StdMethod(HRESULT, "GetMaximumFrameLatency", [Out(Pointer(UINT), "pMaxLatency")], sideeffects=False),
]


dxgi = Module('dxgi')
dxgi.addInterfaces([
    IDXGIKeyedMutex,
    IDXGIFactory1,
    IDXGIDevice1,
    IDXGIAdapter1,
    IDXGIResource,
])
dxgi.addFunctions([
    StdFunction(HRESULT, "CreateDXGIFactory", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppFactory")]),
    StdFunction(HRESULT, "CreateDXGIFactory1", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppFactory")]),
])



#
# DXGI 1.2
#

IDXGIDisplayControl = Interface("IDXGIDisplayControl", IUnknown)
IDXGIDisplayControl.methods += [
    StdMethod(BOOL, "IsStereoEnabled", [], sideeffects=False),
    StdMethod(Void, "SetStereoEnabled", [(BOOL, "enabled")]),
]

DXGI_OUTDUPL_MOVE_RECT = Struct("DXGI_OUTDUPL_MOVE_RECT", [
    (POINT, "SourcePoint"),
    (RECT, "DestinationRect"),
])

DXGI_OUTDUPL_DESC = Struct("DXGI_OUTDUPL_DESC", [
    (DXGI_MODE_DESC, "ModeDesc"),
    (DXGI_MODE_ROTATION, "Rotation"),
    (BOOL, "DesktopImageInSystemMemory"),
])

DXGI_OUTDUPL_POINTER_POSITION = Struct("DXGI_OUTDUPL_POINTER_POSITION", [
    (POINT, "Position"),
    (BOOL, "Visible"),
])

DXGI_OUTDUPL_POINTER_SHAPE_TYPE = Enum("DXGI_OUTDUPL_POINTER_SHAPE_TYPE", [
    "DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME",
    "DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR",
    "DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR",
])

DXGI_OUTDUPL_POINTER_SHAPE_INFO = Struct("DXGI_OUTDUPL_POINTER_SHAPE_INFO", [
    (UINT, "Type"),
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "Pitch"),
    (POINT, "HotSpot"),
])

DXGI_OUTDUPL_FRAME_INFO = Struct("DXGI_OUTDUPL_FRAME_INFO", [
    (LARGE_INTEGER, "LastPresentTime"),
    (LARGE_INTEGER, "LastMouseUpdateTime"),
    (UINT, "AccumulatedFrames"),
    (BOOL, "RectsCoalesced"),
    (BOOL, "ProtectedContentMaskedOut"),
    (DXGI_OUTDUPL_POINTER_POSITION, "PointerPosition"),
    (UINT, "TotalMetadataBufferSize"),
    (UINT, "PointerShapeBufferSize"),
])

IDXGIOutputDuplication = Interface("IDXGIOutputDuplication", IDXGIObject)
IDXGIOutputDuplication.methods += [
    StdMethod(Void, "GetDesc", [Out(Pointer(DXGI_OUTDUPL_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "AcquireNextFrame", [(UINT, "TimeoutInMilliseconds"), Out(Pointer(DXGI_OUTDUPL_FRAME_INFO), "pFrameInfo"), Out(Pointer(ObjPointer(IDXGIResource)), "ppDesktopResource")]),
    StdMethod(HRESULT, "GetFrameDirtyRects", [(UINT, "DirtyRectsBufferSize"), Out(Array(RECT, "DirtyRectsBufferSize"), "pDirtyRectsBuffer"), Out(Pointer(UINT), "pDirtyRectsBufferSizeRequired")], sideeffects=False),
    StdMethod(HRESULT, "GetFrameMoveRects", [(UINT, "MoveRectsBufferSize"), Out(Array(DXGI_OUTDUPL_MOVE_RECT, "MoveRectsBufferSize"), "pMoveRectBuffer"), Out(Pointer(UINT), "pMoveRectsBufferSizeRequired")], sideeffects=False),
    StdMethod(HRESULT, "GetFramePointerShape", [(UINT, "PointerShapeBufferSize"), Out(OpaqueBlob(Void, "PointerShapeBufferSize"), "pPointerShapeBuffer"), Out(Pointer(UINT), "pPointerShapeBufferSizeRequired"), Out(Pointer(DXGI_OUTDUPL_POINTER_SHAPE_INFO), "pPointerShapeInfo")], sideeffects=False),
    StdMethod(HRESULT, "MapDesktopSurface", [Out(Pointer(DXGI_MAPPED_RECT), "pLockedRect")], sideeffects=False),
    StdMethod(HRESULT, "UnMapDesktopSurface", [], sideeffects=False),
    StdMethod(HRESULT, "ReleaseFrame", []),
]

DXGI_ALPHA_MODE = Enum("DXGI_ALPHA_MODE", [
    "DXGI_ALPHA_MODE_UNSPECIFIED",
    "DXGI_ALPHA_MODE_PREMULTIPLIED",
    "DXGI_ALPHA_MODE_STRAIGHT",
    "DXGI_ALPHA_MODE_IGNORE",
])

IDXGISurface2 = Interface("IDXGISurface2", IDXGISurface1)
IDXGISurface2.methods += [
    StdMethod(HRESULT, "GetResource", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppParentResource"), Out(Pointer(UINT), "pSubresourceIndex")]),
]

DXGI_SHARED_RESOURCE_FLAG = Flags(DWORD, [
    "DXGI_SHARED_RESOURCE_READ",
    "DXGI_SHARED_RESOURCE_WRITE",
])

IDXGIResource1 = Interface("IDXGIResource1", IDXGIResource)
IDXGIResource1.methods += [
    StdMethod(HRESULT, "CreateSubresourceSurface", [(UINT, "index"), Out(Pointer(ObjPointer(IDXGISurface2)), "ppSurface")]),
    StdMethod(HRESULT, "CreateSharedHandle", [(Pointer(Const(SECURITY_ATTRIBUTES)), "pAttributes"), (DXGI_SHARED_RESOURCE_FLAG, "dwAccess"), (LPCWSTR, "lpName"), Out(Pointer(HANDLE), "pHandle")]),
]

DXGI_OFFER_RESOURCE_PRIORITY = Enum("DXGI_OFFER_RESOURCE_PRIORITY", [
    "DXGI_OFFER_RESOURCE_PRIORITY_LOW",
    "DXGI_OFFER_RESOURCE_PRIORITY_NORMAL",
    "DXGI_OFFER_RESOURCE_PRIORITY_HIGH",
])

IDXGIDevice2 = Interface("IDXGIDevice2", IDXGIDevice1)
IDXGIDevice2.methods += [
    StdMethod(HRESULT, "OfferResources", [(UINT, "NumResources"), (Array(Const(ObjPointer(IDXGIResource)), "NumResources"), "ppResources"), (DXGI_OFFER_RESOURCE_PRIORITY, "Priority")]),
    StdMethod(HRESULT, "ReclaimResources", [(UINT, "NumResources"), (Array(Const(ObjPointer(IDXGIResource)), "NumResources"), "ppResources"), Out(Pointer(BOOL), "pDiscarded")]),
    StdMethod(HRESULT, "EnqueueSetEvent", [(HANDLE, "hEvent")]),
]

DXGI_MODE_DESC1 = Struct("DXGI_MODE_DESC1", [
    (UINT, "Width"),
    (UINT, "Height"),
    (DXGI_RATIONAL, "RefreshRate"),
    (DXGI_FORMAT, "Format"),
    (DXGI_MODE_SCANLINE_ORDER, "ScanlineOrdering"),
    (DXGI_MODE_SCALING, "Scaling"),
    (BOOL, "Stereo"),
])

DXGI_SCALING = Enum("DXGI_SCALING", [
    "DXGI_SCALING_STRETCH",
    "DXGI_SCALING_NONE",
    "DXGI_SCALING_ASPECT_RATIO_STRETCH",
])

DXGI_SWAP_CHAIN_DESC1 = Struct("DXGI_SWAP_CHAIN_DESC1", [
    (UINT, "Width"),
    (UINT, "Height"),
    (DXGI_FORMAT, "Format"),
    (BOOL, "Stereo"),
    (DXGI_SAMPLE_DESC, "SampleDesc"),
    (DXGI_USAGE, "BufferUsage"),
    (UINT, "BufferCount"),
    (DXGI_SCALING, "Scaling"),
    (DXGI_SWAP_EFFECT, "SwapEffect"),
    (DXGI_ALPHA_MODE, "AlphaMode"),
    (DXGI_SWAP_CHAIN_FLAG, "Flags"),
])

DXGI_SWAP_CHAIN_FULLSCREEN_DESC = Struct("DXGI_SWAP_CHAIN_FULLSCREEN_DESC", [
    (DXGI_RATIONAL, "RefreshRate"),
    (DXGI_MODE_SCANLINE_ORDER, "ScanlineOrdering"),
    (DXGI_MODE_SCALING, "Scaling"),
    (BOOL, "Windowed"),
])

DXGI_PRESENT_PARAMETERS = Struct("DXGI_PRESENT_PARAMETERS", [
    (UINT, "DirtyRectsCount"),
    (Array(RECT, "{self}.DirtyRectsCount"), "pDirtyRects"),
    (Pointer(RECT), "pScrollRect"),
    (Pointer(POINT), "pScrollOffset"),
])

IDXGISwapChain1 = Interface("IDXGISwapChain1", IDXGISwapChain)
IDXGISwapChain1.methods += [
    StdMethod(HRESULT, "GetDesc1", [(Out(Pointer(DXGI_SWAP_CHAIN_DESC1), "pDesc"))], sideeffects=False),
    StdMethod(HRESULT, "GetFullscreenDesc", [(Out(Pointer(DXGI_SWAP_CHAIN_FULLSCREEN_DESC), "pDesc"))], sideeffects=False),
    StdMethod(HRESULT, "GetHwnd", [(Out(Pointer(HWND), "pHwnd"))], sideeffects=False),
    StdMethod(HRESULT, "GetCoreWindow", [(REFIID, "riid"), (Out(Pointer(ObjPointer(Void)), "ppUnk"))]),
    StdMethod(HRESULT, "Present1", [(UINT, "SyncInterval"), (DXGI_PRESENT, "Flags"), (Pointer(Const(DXGI_PRESENT_PARAMETERS)), "pPresentParameters")]),
    StdMethod(BOOL, "IsTemporaryMonoSupported", [], sideeffects=False),
    StdMethod(HRESULT, "GetRestrictToOutput", [(Out(Pointer(ObjPointer(IDXGIOutput)), "ppRestrictToOutput"))]),
    StdMethod(HRESULT, "SetBackgroundColor", [(Pointer(Const(DXGI_RGBA)), "pColor")]),
    StdMethod(HRESULT, "GetBackgroundColor", [(Out(Pointer(DXGI_RGBA), "pColor"))], sideeffects=False),
    StdMethod(HRESULT, "SetRotation", [(DXGI_MODE_ROTATION, "Rotation")]),
    StdMethod(HRESULT, "GetRotation", [(Out(Pointer(DXGI_MODE_ROTATION), "pRotation"))], sideeffects=False),
]

IDXGIFactory2 = Interface("IDXGIFactory2", IDXGIFactory1)
IDXGIFactory2.methods += [
    StdMethod(BOOL, "IsWindowedStereoEnabled", [], sideeffects=False),
    StdMethod(HRESULT, "CreateSwapChainForHwnd", [(ObjPointer(IUnknown), "pDevice"), (HWND, "WindowHandle"), (Pointer(Const(DXGI_SWAP_CHAIN_DESC1)), "pDesc"), (Pointer(Const(DXGI_SWAP_CHAIN_FULLSCREEN_DESC)), "pFullscreenDesc"), (ObjPointer(IDXGIOutput), "pRestrictToOutput"), Out(Pointer(ObjPointer(IDXGISwapChain1)), "ppSwapChain")]),
    StdMethod(HRESULT, "CreateSwapChainForCoreWindow", [(ObjPointer(IUnknown), "pDevice"), (ObjPointer(IUnknown), "pWindow"), (Pointer(Const(DXGI_SWAP_CHAIN_DESC1)), "pDesc"), (ObjPointer(IDXGIOutput), "pRestrictToOutput"), Out(Pointer(ObjPointer(IDXGISwapChain1)), "ppSwapChain")]),
    StdMethod(HRESULT, "GetSharedResourceAdapterLuid", [(HANDLE, "hResource"), Out(Pointer(LUID), "pLuid")], sideeffects=False),
    StdMethod(HRESULT, "RegisterStereoStatusWindow", [(HWND, "WindowHandle"), (UINT, "wMsg"), Out(Pointer(DWORD), "pdwCookie")], sideeffects=False),
    StdMethod(HRESULT, "RegisterStereoStatusEvent", [(HANDLE, "hEvent"), Out(Pointer(DWORD), "pdwCookie")], sideeffects=False),
    StdMethod(Void, "UnregisterStereoStatus", [(DWORD, "dwCookie")], sideeffects=False),
    StdMethod(HRESULT, "RegisterOcclusionStatusWindow", [(HWND, "WindowHandle"), (UINT, "wMsg"), Out(Pointer(DWORD), "pdwCookie")], sideeffects=False),
    StdMethod(HRESULT, "RegisterOcclusionStatusEvent", [(HANDLE, "hEvent"), Out(Pointer(DWORD), "pdwCookie")], sideeffects=False),
    StdMethod(Void, "UnregisterOcclusionStatus", [(DWORD, "dwCookie")], sideeffects=False),
    StdMethod(HRESULT, "CreateSwapChainForComposition", [(ObjPointer(IUnknown), "pDevice"), (Pointer(Const(DXGI_SWAP_CHAIN_DESC1)), "pDesc"), (ObjPointer(IDXGIOutput), "pRestrictToOutput"), Out(Pointer(ObjPointer(IDXGISwapChain1)), "ppSwapChain")]),
]

DXGI_GRAPHICS_PREEMPTION_GRANULARITY = Enum("DXGI_GRAPHICS_PREEMPTION_GRANULARITY", [
    "DXGI_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY",
    "DXGI_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY",
    "DXGI_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY",
    "DXGI_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY",
    "DXGI_GRAPHICS_PREEMPTION_INSTRUCTION_BOUNDARY",
])

DXGI_COMPUTE_PREEMPTION_GRANULARITY = Enum("DXGI_COMPUTE_PREEMPTION_GRANULARITY", [
    "DXGI_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY",
    "DXGI_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY",
    "DXGI_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY",
    "DXGI_COMPUTE_PREEMPTION_THREAD_BOUNDARY",
    "DXGI_COMPUTE_PREEMPTION_INSTRUCTION_BOUNDARY",
])

DXGI_ADAPTER_DESC2 = Struct("DXGI_ADAPTER_DESC2", [
    (WString, "Description"),
    (UINT, "VendorId"),
    (UINT, "DeviceId"),
    (UINT, "SubSysId"),
    (UINT, "Revision"),
    (SIZE_T, "DedicatedVideoMemory"),
    (SIZE_T, "DedicatedSystemMemory"),
    (SIZE_T, "SharedSystemMemory"),
    (LUID, "AdapterLuid"),
    (DXGI_ADAPTER_FLAG, "Flags"),
    (DXGI_GRAPHICS_PREEMPTION_GRANULARITY, "GraphicsPreemptionGranularity"),
    (DXGI_COMPUTE_PREEMPTION_GRANULARITY, "ComputePreemptionGranularity"),
])

IDXGIAdapter2 = Interface("IDXGIAdapter2", IDXGIAdapter1)
IDXGIAdapter2.methods += [
    StdMethod(HRESULT, "GetDesc2", [Out(Pointer(DXGI_ADAPTER_DESC2), "pDesc")], sideeffects=False),
]

IDXGIOutput1 = Interface("IDXGIOutput1", IDXGIOutput)
IDXGIOutput1.methods += [
    StdMethod(HRESULT, "GetDisplayModeList1", [(DXGI_FORMAT, "EnumFormat"), (DXGI_ENUM_MODES, "Flags"), InOut(Pointer(UINT), "pNumModes"), Out(Array(DXGI_MODE_DESC1, "*pNumModes"), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "FindClosestMatchingMode1", [(Pointer(Const(DXGI_MODE_DESC1)), "pModeToMatch"), Out(Pointer(DXGI_MODE_DESC1), "pClosestMatch"), (ObjPointer(IUnknown), "pConcernedDevice")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplaySurfaceData1", [(ObjPointer(IDXGIResource), "pDestination")]),
    StdMethod(HRESULT, "DuplicateOutput", [(ObjPointer(IUnknown), "pDevice"), Out(Pointer(ObjPointer(IDXGIOutputDuplication)), "ppOutputDuplication")]),
]

dxgi.addInterfaces([
    IDXGIDisplayControl,
    IDXGIDevice2,
    IDXGISwapChain1,
    IDXGIFactory2,
    IDXGIResource1,
    IDXGIAdapter2,
    IDXGIOutput1,
])

#
# DXGI 1.3
#

DXGI_CREATE_FACTORY_FLAGS = Flags(UINT, [
    "DXGI_CREATE_FACTORY_DEBUG",
])

dxgi.addFunctions([
    StdFunction(HRESULT, "CreateDXGIFactory2", [(DXGI_CREATE_FACTORY_FLAGS, "Flags"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppFactory")]),
])

IDXGIDevice3 = Interface("IDXGIDevice3", IDXGIDevice2)
IDXGIDevice3.methods += [
    StdMethod(Void, "Trim", []),
]

DXGI_MATRIX_3X2_F = Struct("DXGI_MATRIX_3X2_F", [
    (FLOAT, "_11"),
    (FLOAT, "_12"),
    (FLOAT, "_21"),
    (FLOAT, "_22"),
    (FLOAT, "_31"),
    (FLOAT, "_32"),
])

IDXGISwapChain2 = Interface("IDXGISwapChain2", IDXGISwapChain1)
IDXGISwapChain2.methods += [
    StdMethod(HRESULT, "SetSourceSize", [(UINT, "Width"), (UINT, "Height")]),
    StdMethod(HRESULT, "GetSourceSize", [Out(Pointer(UINT), "pWidth"), Out(Pointer(UINT), "pHeight")], sideeffects=False),
    StdMethod(HRESULT, "SetMaximumFrameLatency", [(UINT, "MaxLatency")]),
    StdMethod(HRESULT, "GetMaximumFrameLatency", [Out(Pointer(UINT), "pMaxLatency")], sideeffects=False),
    StdMethod(HANDLE, "GetFrameLatencyWaitableObject", [], sideeffects=False),
    StdMethod(HRESULT, "SetMatrixTransform", [(Pointer(Const(DXGI_MATRIX_3X2_F)), "pMatrix")]),
    StdMethod(HRESULT, "GetMatrixTransform", [Out(Pointer(DXGI_MATRIX_3X2_F), "pMatrix")], sideeffects=False),
]

IDXGIOutput2 = Interface("IDXGIOutput2", IDXGIOutput1)
IDXGIOutput2.methods += [
    StdMethod(BOOL, "SupportsOverlays", [], sideeffects=False),
]

IDXGIFactory3 = Interface("IDXGIFactory3", IDXGIFactory2)
IDXGIFactory3.methods += [
    StdMethod(DXGI_CREATE_FACTORY_FLAGS, "GetCreationFlags", [], sideeffects=False),
]

DXGI_DECODE_SWAP_CHAIN_DESC = Struct("DXGI_DECODE_SWAP_CHAIN_DESC", [
    (UINT, "Flags"),
])

# XXX: Flags
DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS = Enum("DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS", [
    "DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_NOMINAL_RANGE",
    "DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_BT709",
    "DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_xvYCC",
])

IDXGIDecodeSwapChain = Interface("IDXGIDecodeSwapChain", IUnknown)
IDXGIDecodeSwapChain.methods += [
    StdMethod(HRESULT, "PresentBuffer", [(UINT, "BufferToPresent"), (UINT, "SyncInterval"), (DXGI_PRESENT, "Flags")]),
    StdMethod(HRESULT, "SetSourceRect", [(Pointer(Const(RECT)), "pRect")]),
    StdMethod(HRESULT, "SetTargetRect", [(Pointer(Const(RECT)), "pRect")]),
    StdMethod(HRESULT, "SetDestSize", [(UINT, "Width"), (UINT, "Height")]),
    StdMethod(HRESULT, "GetSourceRect", [Out(Pointer(RECT), "pRect")], sideeffects=False),
    StdMethod(HRESULT, "GetTargetRect", [Out(Pointer(RECT), "pRect")], sideeffects=False),
    StdMethod(HRESULT, "GetDestSize", [Out(Pointer(UINT), "pWidth"), Out(Pointer(UINT), "pHeight")], sideeffects=False),
    StdMethod(HRESULT, "SetColorSpace", [(DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS, "ColorSpace")]),
    StdMethod(DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS, "GetColorSpace", [], sideeffects=False),
]

IDXGIFactoryMedia = Interface("IDXGIFactoryMedia", IUnknown)
IDXGIFactoryMedia.methods += [
    StdMethod(HRESULT, "CreateSwapChainForCompositionSurfaceHandle", [(ObjPointer(IUnknown), "pDevice"), (HANDLE, "hSurface"), (Pointer(Const(DXGI_SWAP_CHAIN_DESC1)), "pDesc"), (ObjPointer(IDXGIOutput), "pRestrictToOutput"), Out(Pointer(ObjPointer(IDXGISwapChain1)), "ppSwapChain")]),
    StdMethod(HRESULT, "CreateDecodeSwapChainForCompositionSurfaceHandle", [(ObjPointer(IUnknown), "pDevice"), (HANDLE, "hSurface"), (Pointer(DXGI_DECODE_SWAP_CHAIN_DESC), "pDesc"), (ObjPointer(IDXGIResource), "pYuvDecodeBuffers"), (ObjPointer(IDXGIOutput), "pRestrictToOutput"), Out(Pointer(ObjPointer(IDXGIDecodeSwapChain)), "ppSwapChain")]),
]

DXGI_FRAME_PRESENTATION_MODE = Enum("DXGI_FRAME_PRESENTATION_MODE", [
    "DXGI_FRAME_PRESENTATION_MODE_COMPOSED",
    "DXGI_FRAME_PRESENTATION_MODE_OVERLAY",
    "DXGI_FRAME_PRESENTATION_MODE_NONE",
])

DXGI_FRAME_STATISTICS_MEDIA = Struct("DXGI_FRAME_STATISTICS_MEDIA", [
    (UINT, "PresentCount"),
    (UINT, "PresentRefreshCount"),
    (UINT, "SyncRefreshCount"),
    (LARGE_INTEGER, "SyncQPCTime"),
    (LARGE_INTEGER, "SyncGPUTime"),
    (DXGI_FRAME_PRESENTATION_MODE, "CompositionMode"),
    (UINT, "ApprovedPresentDuration"),
])

IDXGISwapChainMedia = Interface("IDXGISwapChainMedia", IUnknown)
IDXGISwapChainMedia.methods += [
    StdMethod(HRESULT, "GetFrameStatisticsMedia", [Out(Pointer(DXGI_FRAME_STATISTICS_MEDIA), "pStats")], sideeffects=False),
    StdMethod(HRESULT, "SetPresentDuration", [(UINT, "Duration")]),
    StdMethod(HRESULT, "CheckPresentDurationSupport", [(UINT, "DesiredPresentDuration"), Out(Pointer(UINT), "pClosestSmallerPresentDuration"), Out(Pointer(UINT), "pClosestLargerPresentDuration")], sideeffects=False),
]

DXGI_OVERLAY_SUPPORT_FLAG = FakeEnum(UINT, [
    "DXGI_OVERLAY_SUPPORT_FLAG_DIRECT",
    "DXGI_OVERLAY_SUPPORT_FLAG_SCALING",
])

IDXGIOutput3 = Interface("IDXGIOutput3", IDXGIOutput2)
IDXGIOutput3.methods += [
    StdMethod(HRESULT, "CheckOverlaySupport", [(DXGI_FORMAT, "EnumFormat"), (ObjPointer(IUnknown), "pConcernedDevice"), Out(Pointer(DXGI_OVERLAY_SUPPORT_FLAG), "pFlags")], sideeffects=False),
]

dxgi.addInterfaces([
    IDXGIDevice3,
    IDXGISwapChain2,
    IDXGISwapChainMedia,
    IDXGIOutput3,
    IDXGIFactory3,
    IDXGIFactoryMedia,
])



#
# Undocumented interfaces
#

IDXGIFactoryDWM = Interface("IDXGIFactoryDWM", IUnknown)
IDXGISwapChainDWM = Interface("IDXGISwapChainDWM", IDXGIDeviceSubObject)

IDXGIFactoryDWM.methods += [
    StdMethod(HRESULT, "CreateSwapChain", [(ObjPointer(IUnknown), "pDevice"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pDesc"), (ObjPointer(IDXGIOutput), "pOutput"), Out(Pointer(ObjPointer(IDXGISwapChainDWM)), "ppSwapChain")]),
]

# http://shchetinin.blogspot.co.uk/2012/04/dwm-graphics-directx-win8win7.html
IDXGISwapChainDWM.methods += [
    StdMethod(HRESULT, "Present", [(UINT, "SyncInterval"), (DXGI_PRESENT, "Flags")]),
    StdMethod(HRESULT, "GetBuffer", [(UINT, "Buffer"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppSurface")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_SWAP_CHAIN_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "ResizeBuffers", [(UINT, "BufferCount"), (UINT, "Width"), (UINT, "Height"), (DXGI_FORMAT, "NewFormat"), (DXGI_SWAP_CHAIN_FLAG, "SwapChainFlags")]),
    StdMethod(HRESULT, "ResizeTarget", [(Pointer(Const(DXGI_MODE_DESC)), "pNewTargetParameters")]),
    StdMethod(HRESULT, "GetContainingOutput", [Out(Pointer(ObjPointer(IDXGIOutput)), "ppOutput")]),
    StdMethod(HRESULT, "GetFrameStatistics", [Out(Pointer(DXGI_FRAME_STATISTICS), "pStats")], sideeffects=False),
    StdMethod(HRESULT, "GetLastPresentCount", [Out(Pointer(UINT), "pLastPresentCount")], sideeffects=False),
]

dxgi.addInterfaces([
    IDXGIFactoryDWM,
])



#
# DXGI 1.4
#

DXGI_COLOR_SPACE_TYPE = Enum('DXGI_COLOR_SPACE_TYPE', [
    'DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709',
    'DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709',
    'DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709',
    'DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020',
    'DXGI_COLOR_SPACE_RESERVED',
    'DXGI_COLOR_SPACE_YCBCR_FULL_G22_NONE_P709_X601',
    'DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P601',
    'DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P601',
    'DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709',
    'DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P709',
    'DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P2020',
    'DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P2020',
    'DXGI_COLOR_SPACE_CUSTOM',
])


DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG = Enum('DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG', [
    'DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT',
    'DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_OVERLAY_PRESENT',
])

DXGI_OVERLAY_COLOR_SPACE_SUPPORT_FLAG = Enum('DXGI_OVERLAY_COLOR_SPACE_SUPPORT_FLAG', [
    'DXGI_OVERLAY_COLOR_SPACE_SUPPORT_FLAG_PRESENT',
])

DXGI_MEMORY_SEGMENT_GROUP = Enum('DXGI_MEMORY_SEGMENT_GROUP', [
    'DXGI_MEMORY_SEGMENT_GROUP_LOCAL',
    'DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL',
])

DXGI_QUERY_VIDEO_MEMORY_INFO = Struct('DXGI_QUERY_VIDEO_MEMORY_INFO', [
    (UINT64, 'Budget'),
    (UINT64, 'CurrentUsage'),
    (UINT64, 'AvailableForReservation'),
    (UINT64, 'CurrentReservation'),
])

IDXGISwapChain3 = Interface('IDXGISwapChain3', IDXGISwapChain2)
IDXGIOutput4 = Interface('IDXGIOutput4', IDXGIOutput3)
IDXGIFactory4 = Interface('IDXGIFactory4', IDXGIFactory3)
IDXGIAdapter3 = Interface('IDXGIAdapter3', IDXGIAdapter2)

IDXGISwapChain3.methods += [
    StdMethod(UINT, 'GetCurrentBackBufferIndex', []),
    StdMethod(HRESULT, 'CheckColorSpaceSupport', [(DXGI_COLOR_SPACE_TYPE, 'ColorSpace'), Out(Pointer(UINT), 'pColorSpaceSupport')], sideeffects=False),
    StdMethod(HRESULT, 'SetColorSpace1', [(DXGI_COLOR_SPACE_TYPE, 'ColorSpace')]),
    StdMethod(HRESULT, 'ResizeBuffers1', [(UINT, 'BufferCount'), (UINT, 'Width'), (UINT, 'Height'), (DXGI_FORMAT, 'Format'), (DXGI_SWAP_CHAIN_FLAG, 'SwapChainFlags'), (Pointer(Const(UINT)), 'pCreationNodeMask'), (Array(Const(ObjPointer(IUnknown)), 'BufferCount'), 'ppPresentQueue')]),
]

IDXGIOutput4.methods += [
    StdMethod(HRESULT, 'CheckOverlayColorSpaceSupport', [(DXGI_FORMAT, 'Format'), (DXGI_COLOR_SPACE_TYPE, 'ColorSpace'), (ObjPointer(IUnknown), 'pConcernedDevice'), Out(Pointer(UINT), 'pFlags')], sideeffects=False),
]

IDXGIFactory4.methods += [
    StdMethod(HRESULT, 'EnumAdapterByLuid', [(LUID, 'AdapterLuid'), (REFIID, 'riid'), Out(Pointer(ObjPointer(Void)), 'ppvAdapter')]),
    StdMethod(HRESULT, 'EnumWarpAdapter', [(REFIID, 'riid'), Out(Pointer(ObjPointer(Void)), 'ppvAdapter')]),
]

IDXGIAdapter3.methods += [
    StdMethod(HRESULT, 'RegisterHardwareContentProtectionTeardownStatusEvent', [(HANDLE, 'hEvent'), Out(Pointer(DWORD), 'pdwCookie')], sideeffects=False),
    StdMethod(Void, 'UnregisterHardwareContentProtectionTeardownStatus', [(DWORD, 'dwCookie')], sideeffects=False),
    StdMethod(HRESULT, 'QueryVideoMemoryInfo', [(UINT, 'NodeIndex'), (DXGI_MEMORY_SEGMENT_GROUP, 'MemorySegmentGroup'), Out(Pointer(DXGI_QUERY_VIDEO_MEMORY_INFO), 'pVideoMemoryInfo')], sideeffects=False),
    StdMethod(HRESULT, 'SetVideoMemoryReservation', [(UINT, 'NodeIndex'), (DXGI_MEMORY_SEGMENT_GROUP, 'MemorySegmentGroup'), (UINT64, 'Reservation')]),
    StdMethod(HRESULT, 'RegisterVideoMemoryBudgetChangeNotificationEvent', [(HANDLE, 'hEvent'), Out(Pointer(DWORD), 'pdwCookie')], sideeffects=False),
    StdMethod(Void, 'UnregisterVideoMemoryBudgetChangeNotification', [(DWORD, 'dwCookie')], sideeffects=False),
]

dxgi.addInterfaces([
    IDXGISwapChain3,
    IDXGIOutput4,
    IDXGIFactory4,
    IDXGIAdapter3,
])


#
# DXGI 1.5
#

DXGI_HDR_METADATA_TYPE = Enum('DXGI_HDR_METADATA_TYPE', [
    'DXGI_HDR_METADATA_TYPE_NONE',
    'DXGI_HDR_METADATA_TYPE_HDR10',
])

DXGI_HDR_METADATA_HDR10 = Struct('DXGI_HDR_METADATA_HDR10', [
    (Array(UINT16, 2), 'RedPrimary'),
    (Array(UINT16, 2), 'GreenPrimary'),
    (Array(UINT16, 2), 'BluePrimary'),
    (Array(UINT16, 2), 'WhitePoint'),
    (UINT, 'MaxMasteringLuminance'),
    (UINT, 'MinMasteringLuminance'),
    (UINT16, 'MaxContentLightLevel'),
    (UINT16, 'MaxFrameAverageLightLevel'),
])

DXGI_OFFER_RESOURCE_FLAGS = FakeEnum(UINT, [
    'DXGI_OFFER_RESOURCE_FLAG_ALLOW_DECOMMIT',
])

DXGI_RECLAIM_RESOURCE_RESULTS = Enum('DXGI_RECLAIM_RESOURCE_RESULTS', [
    'DXGI_RECLAIM_RESOURCE_RESULT_OK',
    'DXGI_RECLAIM_RESOURCE_RESULT_DISCARDED',
    'DXGI_RECLAIM_RESOURCE_RESULT_NOT_COMMITTED',
])

DXGI_FEATURE, DXGI_FEATURE_DATA = EnumPolymorphic('DXGI_FEATURE', 'Feature', [
    ('DXGI_FEATURE_PRESENT_ALLOW_TEARING', Pointer(BOOL)),
], Blob(Void, "FeatureSupportDataSize"), False)

IDXGIOutput5 = Interface('IDXGIOutput5', IDXGIOutput4)
IDXGISwapChain4 = Interface('IDXGISwapChain4', IDXGISwapChain3)
IDXGIDevice4 = Interface('IDXGIDevice4', IDXGIDevice3)
IDXGIFactory5 = Interface('IDXGIFactory5', IDXGIFactory4)

IDXGIOutput5.methods += [
    StdMethod(HRESULT, 'DuplicateOutput1', [(ObjPointer(IUnknown), 'pDevice'), (UINT, 'Flags'), (UINT, 'SupportedFormatsCount'), (Array(Const(DXGI_FORMAT), 'SupportedFormatsCount'), 'pSupportedFormats'), Out(Pointer(ObjPointer(IDXGIOutputDuplication)), 'ppOutputDuplication')]),
]

IDXGISwapChain4.methods += [
    StdMethod(HRESULT, 'SetHDRMetaData', [(DXGI_HDR_METADATA_TYPE, 'Type'), (UINT, 'Size'), (Blob(Void, 'Size'), 'pMetaData')]),
]

IDXGIDevice4.methods += [
    StdMethod(HRESULT, 'OfferResources1', [(UINT, 'NumResources'), (Array(Const(ObjPointer(IDXGIResource)), 'NumResources'), 'ppResources'), (DXGI_OFFER_RESOURCE_PRIORITY, 'Priority'), (DXGI_OFFER_RESOURCE_FLAGS, 'Flags')]),
    StdMethod(HRESULT, 'ReclaimResources1', [(UINT, 'NumResources'), (Array(Const(ObjPointer(IDXGIResource)), 'NumResources'), 'ppResources'), Out(Array(DXGI_RECLAIM_RESOURCE_RESULTS, 'NumResources'), 'pResults')]),
]

IDXGIFactory5.methods += [
    StdMethod(HRESULT, 'CheckFeatureSupport', [(DXGI_FEATURE, 'Feature'), Out(DXGI_FEATURE_DATA, 'pFeatureSupportData'), (UINT, 'FeatureSupportDataSize')], sideeffects=False),
]

dxgi.addInterfaces([
    IDXGIOutput5,
    IDXGISwapChain4,
    IDXGIDevice4,
    IDXGIFactory5,
])

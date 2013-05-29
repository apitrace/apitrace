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
])

DXGI_SWAP_CHAIN_FLAG = Flags(UINT, [
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
    (DXGI_SWAP_CHAIN_FLAG, "Flags"),
])

IDXGIObject.methods += [
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "Name"), (UINT, "DataSize"), (OpaqueBlob(Const(Void), "DataSize"), "pData")], sideeffects=False),
    StdMethod(HRESULT, "SetPrivateDataInterface", [(REFGUID, "Name"), (OpaquePointer(Const(IUnknown)), "pUnknown")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "Name"), Out(Pointer(UINT), "pDataSize"), Out(OpaquePointer(Void), "pData")], sideeffects=False),
    StdMethod(HRESULT, "GetParent", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppParent")]),
]

IDXGIDeviceSubObject.methods += [
    StdMethod(HRESULT, "GetDevice", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppDevice")]),
]

IDXGIResource.methods += [
    StdMethod(HRESULT, "GetSharedHandle", [Out(Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "GetUsage", [Out(Pointer(DXGI_USAGE), "pUsage")], sideeffects=False),
    StdMethod(HRESULT, "SetEvictionPriority", [(DXGI_RESOURCE_PRIORITY, "EvictionPriority")]),
    StdMethod(HRESULT, "GetEvictionPriority", [Out(Pointer(DXGI_RESOURCE_PRIORITY), "pEvictionPriority")], sideeffects=False),
]

DXGI_SHARED_RESOURCE_FLAG = Flags(DWORD, [
    "DXGI_SHARED_RESOURCE_READ",
    "DXGI_SHARED_RESOURCE_WRITE",
])


IDXGIKeyedMutex.methods += [
    StdMethod(HRESULT, "AcquireSync", [(UINT64, "Key"), (DWORD, "dwMilliseconds")]),
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
])

IDXGIOutput.methods += [
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_OUTPUT_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplayModeList", [(DXGI_FORMAT, "EnumFormat"), (DXGI_ENUM_MODES, "Flags"), Out(Pointer(UINT), "pNumModes"), Out(Array(DXGI_MODE_DESC, "*pNumModes"), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "FindClosestMatchingMode", [(Pointer(Const(DXGI_MODE_DESC)), "pModeToMatch"), Out(Pointer(DXGI_MODE_DESC), "pClosestMatch"), (ObjPointer(IUnknown), "pConcernedDevice")]),
    StdMethod(HRESULT, "WaitForVBlank", []),
    StdMethod(HRESULT, "TakeOwnership", [(ObjPointer(IUnknown), "pDevice"), (BOOL, "Exclusive")]),
    StdMethod(Void, "ReleaseOwnership", []),
    StdMethod(HRESULT, "GetGammaControlCapabilities", [Out(Pointer(DXGI_GAMMA_CONTROL_CAPABILITIES), "pGammaCaps")]),
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
    StdMethod(HRESULT, "MakeWindowAssociation", [(HWND, "WindowHandle"), (DXGI_MWA, "Flags")]),
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

DXGI_ADAPTER_FLAG = Enum("DXGI_ADAPTER_FLAG", [
    "DXGI_ADAPTER_FLAG_NONE",
    "DXGI_ADAPTER_FLAG_REMOTE",
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
    (UINT, "Flags"),
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


IDXGIFactoryDWM = Interface("IDXGIFactoryDWM", IUnknown)
IDXGISwapChainDWM = Interface("IDXGISwapChainDWM", IDXGIDeviceSubObject)

IDXGIFactoryDWM.methods += [
    StdMethod(HRESULT, "CreateSwapChain", [(ObjPointer(IUnknown), "pDevice"), (Pointer(DXGI_SWAP_CHAIN_DESC), "pDesc"), (ObjPointer(IDXGIOutput), "pOutput"), Out(Pointer(ObjPointer(IDXGISwapChainDWM)), "ppSwapChain")]),
]

IDXGISwapChainDWM.methods += [
    StdMethod(HRESULT, "Present", [(UINT, "SyncInterval"), (DXGI_PRESENT, "Flags")]),
    StdMethod(HRESULT, "GetBuffer", [(UINT, "Buffer"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppSurface")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(DXGI_SWAP_CHAIN_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "ResizeBuffers", [(UINT, "BufferCount"), (UINT, "Width"), (UINT, "Height"), (DXGI_FORMAT, "NewFormat"), (DXGI_SWAP_CHAIN_FLAG, "SwapChainFlags")]),
    StdMethod(HRESULT, "ResizeTarget", [(Pointer(Const(DXGI_MODE_DESC)), "pNewTargetParameters")]),
    StdMethod(HRESULT, "GetContainingOutput", [Out(Pointer(ObjPointer(IDXGIOutput)), "ppOutput")]),
    StdMethod(HRESULT, "GetFrameStatistics", [(Pointer(DXGI_FRAME_STATISTICS), "pStats")], sideeffects=False),
    StdMethod(HRESULT, "GetLastPresentCount", [(Pointer(UINT), "pLastPresentCount")], sideeffects=False),
    StdMethod(HRESULT, "SetFullscreenState", [(BOOL, "Fullscreen"), (ObjPointer(IDXGIOutput), "pTarget")]),
    StdMethod(HRESULT, "GetFullscreenState", [Out(Pointer(BOOL), "pFullscreen"), Out(Pointer(ObjPointer(IDXGIOutput)), "ppTarget")]),
]


dxgi = Module('dxgi')
dxgi.addInterfaces([
    IDXGIFactory1,
    IDXGIFactoryDWM,
])
dxgi.addFunctions([
    StdFunction(HRESULT, "CreateDXGIFactory", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppFactory")]),
    StdFunction(HRESULT, "CreateDXGIFactory1", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppFactory")]),
])

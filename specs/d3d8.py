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

"""d3d8.h"""

from winapi import *
from d3d8types import *
from d3d8caps import *

HRESULT = Enum("HRESULT", [
    "D3D_OK",
    "D3DERR_WRONGTEXTUREFORMAT",
    "D3DERR_UNSUPPORTEDCOLOROPERATION",
    "D3DERR_UNSUPPORTEDCOLORARG",
    "D3DERR_UNSUPPORTEDALPHAOPERATION",
    "D3DERR_UNSUPPORTEDALPHAARG",
    "D3DERR_TOOMANYOPERATIONS",
    "D3DERR_CONFLICTINGTEXTUREFILTER",
    "D3DERR_UNSUPPORTEDFACTORVALUE",
    "D3DERR_CONFLICTINGRENDERSTATE",
    "D3DERR_UNSUPPORTEDTEXTUREFILTER",
    "D3DERR_CONFLICTINGTEXTUREPALETTE",
    "D3DERR_DRIVERINTERNALERROR",
    "D3DERR_NOTFOUND",
    "D3DERR_MOREDATA",
    "D3DERR_DEVICELOST",
    "D3DERR_DEVICENOTRESET",
    "D3DERR_NOTAVAILABLE",
    "D3DERR_OUTOFVIDEOMEMORY",
    "D3DERR_INVALIDDEVICE",
    "D3DERR_INVALIDCALL",
    "D3DERR_DRIVERINVALIDCALL",
])

IDirect3D8 = Interface("IDirect3D8", IUnknown)
IDirect3DDevice8 = Interface("IDirect3DDevice8", IUnknown)
IDirect3DSwapChain8 = Interface("IDirect3DSwapChain8", IUnknown)
IDirect3DResource8 = Interface("IDirect3DResource8", IUnknown)
IDirect3DBaseTexture8 = Interface("IDirect3DBaseTexture8", IDirect3DResource8)
IDirect3DTexture8 = Interface("IDirect3DTexture8", IDirect3DBaseTexture8)
IDirect3DVolumeTexture8 = Interface("IDirect3DVolumeTexture8", IDirect3DBaseTexture8)
IDirect3DCubeTexture8 = Interface("IDirect3DCubeTexture8", IDirect3DBaseTexture8)
IDirect3DVertexBuffer8 = Interface("IDirect3DVertexBuffer8", IDirect3DResource8)
IDirect3DIndexBuffer8 = Interface("IDirect3DIndexBuffer8", IDirect3DResource8)
IDirect3DSurface8 = Interface("IDirect3DSurface8", IUnknown)
IDirect3DVolume8 = Interface("IDirect3DVolume8", IUnknown)

PDIRECT3D8 = Pointer(IDirect3D8)
PDIRECT3DDEVICE8 = Pointer(IDirect3DDevice8)
PDIRECT3DSWAPCHAIN8 = Pointer(IDirect3DSwapChain8)
PDIRECT3DRESOURCE8 = Pointer(IDirect3DResource8)
PDIRECT3DBASETEXTURE8 = Pointer(IDirect3DBaseTexture8)
PDIRECT3DTEXTURE8 = Pointer(IDirect3DTexture8)
PDIRECT3DVOLUMETEXTURE8 = Pointer(IDirect3DVolumeTexture8)
PDIRECT3DCUBETEXTURE8 = Pointer(IDirect3DCubeTexture8)
PDIRECT3DVERTEXBUFFER8 = Pointer(IDirect3DVertexBuffer8)
PDIRECT3DINDEXBUFFER8 = Pointer(IDirect3DIndexBuffer8)
PDIRECT3DSURFACE8 = Pointer(IDirect3DSurface8)
PDIRECT3DVOLUME8 = Pointer(IDirect3DVolume8)

IDirect3D8.methods += [
    Method(HRESULT, "RegisterSoftwareDevice", [(OpaquePointer(Void), "pInitializeFunction")]),
    Method(UINT, "GetAdapterCount", []),
    Method(HRESULT, "GetAdapterIdentifier", [(UINT, "Adapter"), (DWORD, "Flags"), Out(Pointer(D3DADAPTER_IDENTIFIER8), "pIdentifier")]),
    Method(UINT, "GetAdapterModeCount", [(UINT, "Adapter")]),
    Method(HRESULT, "EnumAdapterModes", [(UINT, "Adapter"), (UINT, "Mode"), Out(Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetAdapterDisplayMode", [(UINT, "Adapter"), Out(Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "CheckDeviceType", [(UINT, "Adapter"), (D3DDEVTYPE, "CheckType"), (D3DFORMAT, "DisplayFormat"), (D3DFORMAT, "BackBufferFormat"), (BOOL, "Windowed")]),
    Method(HRESULT, "CheckDeviceFormat", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (DWORD, "Usage"), (D3DRESOURCETYPE, "RType"), (D3DFORMAT, "CheckFormat")]),
    Method(HRESULT, "CheckDeviceMultiSampleType", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SurfaceFormat"), (BOOL, "Windowed"), (D3DMULTISAMPLE_TYPE, "MultiSampleType")]),
    Method(HRESULT, "CheckDepthStencilMatch", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "RenderTargetFormat"), (D3DFORMAT, "DepthStencilFormat")]),
    Method(HRESULT, "GetDeviceCaps", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), Out(Pointer(D3DCAPS8), "pCaps")]),
    Method(HMONITOR, "GetAdapterMonitor", [(UINT, "Adapter")]),
    Method(HRESULT, "CreateDevice", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (DWORD, "BehaviorFlags"), Out(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(PDIRECT3DDEVICE8), "ppReturnedDeviceInterface")]),
]

IDirect3DDevice8.methods += [
    Method(HRESULT, "TestCooperativeLevel", []),
    Method(UINT, "GetAvailableTextureMem", []),
    Method(HRESULT, "ResourceManagerDiscardBytes", [(DWORD, "Bytes")]),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(PDIRECT3D8), "ppD3D8")]),
    Method(HRESULT, "GetDeviceCaps", [Out(Pointer(D3DCAPS8), "pCaps")]),
    Method(HRESULT, "GetDisplayMode", [Out(Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetCreationParameters", [Out(Pointer(D3DDEVICE_CREATION_PARAMETERS), "pParameters")]),
    Method(HRESULT, "SetCursorProperties", [(UINT, "XHotSpot"), (UINT, "YHotSpot"), (PDIRECT3DSURFACE8, "pCursorBitmap")]),
    # XXX: There are different signatures of
    # IDirect3DDevice8::SetCursorPosition depending on the DXSDK version
    Method(Void, "SetCursorPosition", [(UINT, "XScreenSpace"), (UINT, "YScreenSpace"), (DWORD, "Flags")]),
    Method(Void, "SetCursorPosition", [(Int, "X"), (Int, "Y"), (DWORD, "Flags")]),
    Method(BOOL, "ShowCursor", [(BOOL, "bShow")]),
    Method(HRESULT, "CreateAdditionalSwapChain", [Out(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(PDIRECT3DSWAPCHAIN8), "pSwapChain")]),
    Method(HRESULT, "Reset", [Out(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
    Method(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion")]),
    Method(HRESULT, "GetBackBuffer", [(UINT, "BackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), Out(Pointer(PDIRECT3DSURFACE8), "ppBackBuffer")]),
    Method(HRESULT, "GetRasterStatus", [Out(Pointer(D3DRASTER_STATUS), "pRasterStatus")]),
    Method(Void, "SetGammaRamp", [(DWORD, "Flags"), (ConstPointer(D3DGAMMARAMP), "pRamp")]),
    Method(Void, "GetGammaRamp", [Out(Pointer(D3DGAMMARAMP), "pRamp")]),
    Method(HRESULT, "CreateTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DTEXTURE8), "ppTexture")]),
    Method(HRESULT, "CreateVolumeTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Depth"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DVOLUMETEXTURE8), "ppVolumeTexture")]),
    Method(HRESULT, "CreateCubeTexture", [(UINT, "EdgeLength"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DCUBETEXTURE8), "ppCubeTexture")]),
    Method(HRESULT, "CreateVertexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (DWORD, "FVF"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DVERTEXBUFFER8), "ppVertexBuffer")]),
    Method(HRESULT, "CreateIndexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DINDEXBUFFER8), "ppIndexBuffer")]),
    Method(HRESULT, "CreateRenderTarget", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (BOOL, "Lockable"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurface")]),
    Method(HRESULT, "CreateDepthStencilSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurface")]),
    Method(HRESULT, "CreateImageSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurface")]),
    Method(HRESULT, "CopyRects", [(PDIRECT3DSURFACE8, "pSourceSurface"), (ConstPointer(RECT), "pSourceRectsArray"), (UINT, "cRects"), (PDIRECT3DSURFACE8, "pDestinationSurface"), (ConstPointer(POINT), "pDestPointsArray")]),
    Method(HRESULT, "UpdateTexture", [(PDIRECT3DBASETEXTURE8, "pSourceTexture"), (PDIRECT3DBASETEXTURE8, "pDestinationTexture")]),
    Method(HRESULT, "GetFrontBuffer", [(PDIRECT3DSURFACE8, "pDestSurface")]),
    Method(HRESULT, "SetRenderTarget", [(PDIRECT3DSURFACE8, "pRenderTarget"), (PDIRECT3DSURFACE8, "pNewZStencil")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(PDIRECT3DSURFACE8), "ppRenderTarget")]),
    Method(HRESULT, "GetDepthStencilSurface", [Out(Pointer(PDIRECT3DSURFACE8), "ppZStencilSurface")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "Clear", [(DWORD, "Count"), (ConstPointer(D3DRECT), "pRects"), (D3DCLEAR, "Flags"), (D3DCOLOR, "Color"), (Float, "Z"), (DWORD, "Stencil")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "State"), Out(Pointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "SetViewport", [(ConstPointer(D3DVIEWPORT8), "pViewport")]),
    Method(HRESULT, "GetViewport", [Out(Pointer(D3DVIEWPORT8), "pViewport")]),
    Method(HRESULT, "SetMaterial", [(ConstPointer(D3DMATERIAL8), "pMaterial")]),
    Method(HRESULT, "GetMaterial", [Out(Pointer(D3DMATERIAL8), "pMaterial")]),
    Method(HRESULT, "SetLight", [(DWORD, "Index"), (ConstPointer(D3DLIGHT8), "pLight")]),
    Method(HRESULT, "GetLight", [(DWORD, "Index"), Out(Pointer(D3DLIGHT8), "pLight")]),
    Method(HRESULT, "LightEnable", [(DWORD, "Index"), (BOOL, "Enable")]),
    Method(HRESULT, "GetLightEnable", [(DWORD, "Index"), Out(Pointer(BOOL), "pEnable")]),
    Method(HRESULT, "SetClipPlane", [(DWORD, "Index"), (ConstPointer(Float), "pPlane")]),
    Method(HRESULT, "GetClipPlane", [(DWORD, "Index"), Out(Pointer(Float), "pPlane")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "State"), (DWORD, "Value")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "State"), Out(Pointer(DWORD), "pValue")]),
    Method(HRESULT, "BeginStateBlock", []),
    Method(HRESULT, "EndStateBlock", [Out(Pointer(DWORD), "pToken")]),
    Method(HRESULT, "ApplyStateBlock", [(DWORD, "Token")]),
    Method(HRESULT, "CaptureStateBlock", [(DWORD, "Token")]),
    Method(HRESULT, "DeleteStateBlock", [(DWORD, "Token")]),
    Method(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "Type"), Out(Pointer(DWORD), "pToken")]),
    Method(HRESULT, "SetClipStatus", [(ConstPointer(D3DCLIPSTATUS8), "pClipStatus")]),
    Method(HRESULT, "GetClipStatus", [Out(Pointer(D3DCLIPSTATUS8), "pClipStatus")]),
    Method(HRESULT, "GetTexture", [(DWORD, "Stage"), Out(Pointer(PDIRECT3DBASETEXTURE8), "ppTexture")]),
    Method(HRESULT, "SetTexture", [(DWORD, "Stage"), (PDIRECT3DBASETEXTURE8, "pTexture")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), Out(Pointer(DWORD), "pValue")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (DWORD, "Value")]),
    Method(HRESULT, "ValidateDevice", [Out(Pointer(DWORD), "pNumPasses")]),
    Method(HRESULT, "GetInfo", [(DWORD, "DevInfoID"), Out(OpaquePointer(Void), "pDevInfoStruct"), (DWORD, "DevInfoStructSize")]),
    Method(HRESULT, "SetPaletteEntries", [(UINT, "PaletteNumber"), (ConstPointer(PALETTEENTRY), "pEntries")]),
    Method(HRESULT, "GetPaletteEntries", [(UINT, "PaletteNumber"), Out(Pointer(PALETTEENTRY), "pEntries")]),
    Method(HRESULT, "SetCurrentTexturePalette", [(UINT, "PaletteNumber")]),
    Method(HRESULT, "GetCurrentTexturePalette", [Out(Pointer(UINT), "PaletteNumber")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "StartVertex"), (UINT, "PrimitiveCount")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "minIndex"), (UINT, "NumVertices"), (UINT, "startIndex"), (UINT, "primCount")]),
    Method(HRESULT, "DrawPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "PrimitiveCount"), (OpaquePointer(Const(Void)), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    Method(HRESULT, "DrawIndexedPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "MinVertexIndex"), (UINT, "NumVertexIndices"), (UINT, "PrimitiveCount"), (OpaquePointer(Const(Void)), "pIndexData"), (D3DFORMAT, "IndexDataFormat"), (OpaquePointer(Const(Void)), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    Method(HRESULT, "ProcessVertices", [(UINT, "SrcStartIndex"), (UINT, "DestIndex"), (UINT, "VertexCount"), (PDIRECT3DVERTEXBUFFER8, "pDestBuffer"), (DWORD, "Flags")]),
    Method(HRESULT, "CreateVertexShader", [(ConstPointer(DWORD), "pDeclaration"), (ConstPointer(DWORD), "pFunction"), Out(Pointer(DWORD), "pHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "SetVertexShader", [(DWORD, "Handle")]),
    Method(HRESULT, "GetVertexShader", [Out(Pointer(DWORD), "pHandle")]),
    Method(HRESULT, "DeleteVertexShader", [(DWORD, "Handle")]),
    Method(HRESULT, "SetVertexShaderConstant", [(DWORD, "Register"), (OpaquePointer(Const(Void)), "pConstantData"), (DWORD, "ConstantCount")]),
    Method(HRESULT, "GetVertexShaderConstant", [(DWORD, "Register"), Out(OpaquePointer(Void), "pConstantData"), (DWORD, "ConstantCount")]),
    Method(HRESULT, "GetVertexShaderDeclaration", [(DWORD, "Handle"), Out(OpaquePointer(Void), "pData"), Out(Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "GetVertexShaderFunction", [(DWORD, "Handle"), Out(OpaquePointer(Void), "pData"), Out(Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "SetStreamSource", [(UINT, "StreamNumber"), (PDIRECT3DVERTEXBUFFER8, "pStreamData"), (UINT, "Stride")]),
    Method(HRESULT, "GetStreamSource", [(UINT, "StreamNumber"), Out(Pointer(PDIRECT3DVERTEXBUFFER8), "ppStreamData"), Out(Pointer(UINT), "pStride")]),
    Method(HRESULT, "SetIndices", [(PDIRECT3DINDEXBUFFER8, "pIndexData"), (UINT, "BaseVertexIndex")]),
    Method(HRESULT, "GetIndices", [Out(Pointer(PDIRECT3DINDEXBUFFER8), "ppIndexData"), Out(Pointer(UINT), "pBaseVertexIndex")]),
    Method(HRESULT, "CreatePixelShader", [(ConstPointer(DWORD), "pFunction"), Out(Pointer(DWORD), "pHandle")]),
    Method(HRESULT, "SetPixelShader", [(DWORD, "Handle")]),
    Method(HRESULT, "GetPixelShader", [Out(Pointer(DWORD), "pHandle")]),
    Method(HRESULT, "DeletePixelShader", [(DWORD, "Handle")]),
    Method(HRESULT, "SetPixelShaderConstant", [(DWORD, "Register"), (OpaquePointer(Const(Void)), "pConstantData"), (DWORD, "ConstantCount")]),
    Method(HRESULT, "GetPixelShaderConstant", [(DWORD, "Register"), Out(OpaquePointer(Void), "pConstantData"), (DWORD, "ConstantCount")]),
    Method(HRESULT, "GetPixelShaderFunction", [(DWORD, "Handle"), Out(OpaquePointer(Void), "pData"), Out(Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "DrawRectPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DRECTPATCH_INFO), "pRectPatchInfo")]),
    Method(HRESULT, "DrawTriPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DTRIPATCH_INFO), "pTriPatchInfo")]),
    Method(HRESULT, "DeletePatch", [(UINT, "Handle")]),
]

IDirect3DSwapChain8.methods += [
    Method(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion")]),
    Method(HRESULT, "GetBackBuffer", [(UINT, "BackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), Out(Pointer(PDIRECT3DSURFACE8), "ppBackBuffer")]),
]

IDirect3DResource8.methods += [
    Method(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE8), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaquePointer(Const(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaquePointer(Void), "pData"), Out(Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(DWORD, "SetPriority", [(DWORD, "PriorityNew")]),
    Method(DWORD, "GetPriority", []),
    Method(Void, "PreLoad", []),
    Method(D3DRESOURCETYPE, "GetType", []),
]

IDirect3DBaseTexture8.methods += [
    Method(DWORD, "SetLOD", [(DWORD, "LODNew")]),
    Method(DWORD, "GetLOD", []),
    Method(DWORD, "GetLevelCount", []),
]

IDirect3DTexture8.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "GetSurfaceLevel", [(UINT, "Level"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurfaceLevel")]),
    Method(HRESULT, "LockRect", [(UINT, "Level"), Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", [(UINT, "Level")]),
    Method(HRESULT, "AddDirtyRect", [(ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVolumeTexture8.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DVOLUME_DESC), "pDesc")]),
    Method(HRESULT, "GetVolumeLevel", [(UINT, "Level"), Out(Pointer(PDIRECT3DVOLUME8), "ppVolumeLevel")]),
    Method(HRESULT, "LockBox", [(UINT, "Level"), Out(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockBox", [(UINT, "Level")]),
    Method(HRESULT, "AddDirtyBox", [(ConstPointer(D3DBOX), "pDirtyBox")]),
]

IDirect3DCubeTexture8.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "GetCubeMapSurface", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), Out(Pointer(PDIRECT3DSURFACE8), "ppCubeMapSurface")]),
    Method(HRESULT, "LockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level")]),
    Method(HRESULT, "AddDirtyRect", [(D3DCUBEMAP_FACES, "FaceType"), (ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVertexBuffer8.methods += [
    Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), Out(Pointer(Pointer(BYTE)), "ppbData"), (DWORD, "Flags")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "GetDesc", [Out(Pointer(D3DVERTEXBUFFER_DESC), "pDesc")]),
]

IDirect3DIndexBuffer8.methods += [
    Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), Out(Pointer(Pointer(BYTE)), "ppbData"), (DWORD, "Flags")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "GetDesc", [Out(Pointer(D3DINDEXBUFFER_DESC), "pDesc")]),
]

IDirect3DSurface8.methods += [
    Method(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE8), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaquePointer(Const(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaquePointer(Void), "pData"), Out(Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(HRESULT, "GetContainer", [(REFIID, "riid"), Out(Pointer(OpaquePointer(Void)), "ppContainer")]),
    Method(HRESULT, "GetDesc", [Out(Pointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "LockRect", [Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", []),
]

IDirect3DVolume8.methods += [
    Method(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE8), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaquePointer(Const(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaquePointer(Void), "pData"), Out(Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(HRESULT, "GetContainer", [(REFIID, "riid"), Out(Pointer(OpaquePointer(Void)), "ppContainer")]),
    Method(HRESULT, "GetDesc", [Out(Pointer(D3DVOLUME_DESC), "pDesc")]),
    Method(HRESULT, "LockBox", [Out(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockBox", []),
]

d3d8 = API("d3d8")
d3d8.addFunctions([
    StdFunction(PDIRECT3D8, "Direct3DCreate8", [(UINT, "SDKVersion")]),
])

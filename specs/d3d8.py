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


D3DSHADER8 = Blob(Const(DWORD), "_shaderSize(pFunction)")

D3DSPD = Flags(DWORD, [
    "D3DSPD_IUNKNOWN",
])

D3DADAPTER = FakeEnum(UINT, [
    "D3DADAPTER_DEFAULT",
])

D3DENUM = FakeEnum(DWORD, [
    "D3DENUM_NO_WHQL_LEVEL",
])

D3DSGR = Flags(DWORD, [
    "D3DSGR_NO_CALIBRATION",
    "D3DSGR_CALIBRATE",
])

D3DCURSOR = Flags(DWORD, [
    "D3DCURSOR_IMMEDIATE_UPDATE",
])

HRESULT = MAKE_HRESULT(ok = "D3D_OK", errors = [
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

PDIRECT3D8 = ObjPointer(IDirect3D8)
PDIRECT3DDEVICE8 = ObjPointer(IDirect3DDevice8)
PDIRECT3DSWAPCHAIN8 = ObjPointer(IDirect3DSwapChain8)
PDIRECT3DRESOURCE8 = ObjPointer(IDirect3DResource8)
PDIRECT3DBASETEXTURE8 = ObjPointer(IDirect3DBaseTexture8)
PDIRECT3DTEXTURE8 = ObjPointer(IDirect3DTexture8)
PDIRECT3DVOLUMETEXTURE8 = ObjPointer(IDirect3DVolumeTexture8)
PDIRECT3DCUBETEXTURE8 = ObjPointer(IDirect3DCubeTexture8)
PDIRECT3DVERTEXBUFFER8 = ObjPointer(IDirect3DVertexBuffer8)
PDIRECT3DINDEXBUFFER8 = ObjPointer(IDirect3DIndexBuffer8)
PDIRECT3DSURFACE8 = ObjPointer(IDirect3DSurface8)
PDIRECT3DVOLUME8 = ObjPointer(IDirect3DVolume8)

IDirect3D8.methods += [
    StdMethod(HRESULT, "RegisterSoftwareDevice", [(OpaquePointer(Void), "pInitializeFunction")], sideeffects=False),
    StdMethod(UINT, "GetAdapterCount", [], sideeffects=False),
    StdMethod(HRESULT, "GetAdapterIdentifier", [(D3DADAPTER, "Adapter"), (D3DENUM, "Flags"), Out(Pointer(D3DADAPTER_IDENTIFIER8), "pIdentifier")], sideeffects=False),
    StdMethod(UINT, "GetAdapterModeCount", [(D3DADAPTER, "Adapter")], sideeffects=False),
    StdMethod(HRESULT, "EnumAdapterModes", [(D3DADAPTER, "Adapter"), (UINT, "Mode"), Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "GetAdapterDisplayMode", [(D3DADAPTER, "Adapter"), Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceType", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "CheckType"), (D3DFORMAT, "DisplayFormat"), (D3DFORMAT, "BackBufferFormat"), (BOOL, "Windowed")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceFormat", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DUSAGE, "Usage"), (D3DRESOURCETYPE, "RType"), (D3DFORMAT, "CheckFormat")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceMultiSampleType", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SurfaceFormat"), (BOOL, "Windowed"), (D3DMULTISAMPLE_TYPE, "MultiSampleType")], sideeffects=False),
    StdMethod(HRESULT, "CheckDepthStencilMatch", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "RenderTargetFormat"), (D3DFORMAT, "DepthStencilFormat")], sideeffects=False),
    StdMethod(HRESULT, "GetDeviceCaps", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), Out(Pointer(D3DCAPS8), "pCaps")], sideeffects=False),
    StdMethod(HMONITOR, "GetAdapterMonitor", [(D3DADAPTER, "Adapter")], sideeffects=False),
    StdMethod(HRESULT, "CreateDevice", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (D3DCREATE, "BehaviorFlags"), InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(PDIRECT3DDEVICE8), "ppReturnedDeviceInterface")]),
]

IDirect3DDevice8.methods += [
    StdMethod(HRESULT, "TestCooperativeLevel", []),
    StdMethod(UINT, "GetAvailableTextureMem", [], sideeffects=False),
    StdMethod(HRESULT, "ResourceManagerDiscardBytes", [(DWORD, "Bytes")]),
    StdMethod(HRESULT, "GetDirect3D", [Out(Pointer(PDIRECT3D8), "ppD3D8")]),
    StdMethod(HRESULT, "GetDeviceCaps", [Out(Pointer(D3DCAPS8), "pCaps")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplayMode", [Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "GetCreationParameters", [Out(Pointer(D3DDEVICE_CREATION_PARAMETERS), "pParameters")], sideeffects=False),
    StdMethod(HRESULT, "SetCursorProperties", [(UINT, "XHotSpot"), (UINT, "YHotSpot"), (PDIRECT3DSURFACE8, "pCursorBitmap")]),
    # XXX: There are different signatures of
    # IDirect3DDevice8::SetCursorPosition depending on the DXSDK version
    #StdMethod(Void, "SetCursorPosition", [(UINT, "XScreenSpace"), (UINT, "YScreenSpace"), (D3DCURSOR, "Flags")]),
    StdMethod(Void, "SetCursorPosition", [(Int, "X"), (Int, "Y"), (D3DCURSOR, "Flags")]),
    StdMethod(BOOL, "ShowCursor", [(BOOL, "bShow")]),
    StdMethod(HRESULT, "CreateAdditionalSwapChain", [InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(PDIRECT3DSWAPCHAIN8), "pSwapChain")]),
    StdMethod(HRESULT, "Reset", [InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
    StdMethod(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion")]),
    StdMethod(HRESULT, "GetBackBuffer", [(UINT, "BackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), Out(Pointer(PDIRECT3DSURFACE8), "ppBackBuffer")]),
    StdMethod(HRESULT, "GetRasterStatus", [Out(Pointer(D3DRASTER_STATUS), "pRasterStatus")], sideeffects=False),
    StdMethod(Void, "SetGammaRamp", [(D3DSGR, "Flags"), (ConstPointer(D3DGAMMARAMP), "pRamp")]),
    StdMethod(Void, "GetGammaRamp", [Out(Pointer(D3DGAMMARAMP), "pRamp")], sideeffects=False),
    StdMethod(HRESULT, "CreateTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Levels"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DTEXTURE8), "ppTexture")]),
    StdMethod(HRESULT, "CreateVolumeTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Depth"), (UINT, "Levels"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DVOLUMETEXTURE8), "ppVolumeTexture")]),
    StdMethod(HRESULT, "CreateCubeTexture", [(UINT, "EdgeLength"), (UINT, "Levels"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DCUBETEXTURE8), "ppCubeTexture")]),
    StdMethod(HRESULT, "CreateVertexBuffer", [(UINT, "Length"), (D3DUSAGE, "Usage"), (D3DFVF, "FVF"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DVERTEXBUFFER8), "ppVertexBuffer")]),
    StdMethod(HRESULT, "CreateIndexBuffer", [(UINT, "Length"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DINDEXBUFFER8), "ppIndexBuffer")]),
    StdMethod(HRESULT, "CreateRenderTarget", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (BOOL, "Lockable"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurface")]),
    StdMethod(HRESULT, "CreateDepthStencilSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurface")]),
    StdMethod(HRESULT, "CreateImageSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurface")]),
    StdMethod(HRESULT, "CopyRects", [(PDIRECT3DSURFACE8, "pSourceSurface"), (ConstPointer(RECT), "pSourceRectsArray"), (UINT, "cRects"), (PDIRECT3DSURFACE8, "pDestinationSurface"), (ConstPointer(POINT), "pDestPointsArray")]),
    StdMethod(HRESULT, "UpdateTexture", [(PDIRECT3DBASETEXTURE8, "pSourceTexture"), (PDIRECT3DBASETEXTURE8, "pDestinationTexture")]),
    StdMethod(HRESULT, "GetFrontBuffer", [(PDIRECT3DSURFACE8, "pDestSurface")]),
    StdMethod(HRESULT, "SetRenderTarget", [(PDIRECT3DSURFACE8, "pRenderTarget"), (PDIRECT3DSURFACE8, "pNewZStencil")]),
    StdMethod(HRESULT, "GetRenderTarget", [Out(Pointer(PDIRECT3DSURFACE8), "ppRenderTarget")]),
    StdMethod(HRESULT, "GetDepthStencilSurface", [Out(Pointer(PDIRECT3DSURFACE8), "ppZStencilSurface")]),
    StdMethod(HRESULT, "BeginScene", []),
    StdMethod(HRESULT, "EndScene", []),
    StdMethod(HRESULT, "Clear", [(DWORD, "Count"), (ConstPointer(D3DRECT), "pRects"), (D3DCLEAR, "Flags"), (D3DCOLOR, "Color"), (Float, "Z"), (DWORD, "Stencil")]),
    StdMethod(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    StdMethod(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "State"), Out(Pointer(D3DMATRIX), "pMatrix")], sideeffects=False),
    StdMethod(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    StdMethod(HRESULT, "SetViewport", [(ConstPointer(D3DVIEWPORT8), "pViewport")]),
    StdMethod(HRESULT, "GetViewport", [Out(Pointer(D3DVIEWPORT8), "pViewport")], sideeffects=False),
    StdMethod(HRESULT, "SetMaterial", [(ConstPointer(D3DMATERIAL8), "pMaterial")]),
    StdMethod(HRESULT, "GetMaterial", [Out(Pointer(D3DMATERIAL8), "pMaterial")], sideeffects=False),
    StdMethod(HRESULT, "SetLight", [(DWORD, "Index"), (ConstPointer(D3DLIGHT8), "pLight")]),
    StdMethod(HRESULT, "GetLight", [(DWORD, "Index"), Out(Pointer(D3DLIGHT8), "pLight")], sideeffects=False),
    StdMethod(HRESULT, "LightEnable", [(DWORD, "Index"), (BOOL, "Enable")]),
    StdMethod(HRESULT, "GetLightEnable", [(DWORD, "Index"), Out(Pointer(BOOL), "pEnable")], sideeffects=False),
    StdMethod(HRESULT, "SetClipPlane", [(DWORD, "Index"), (ConstPointer(Float), "pPlane")]),
    StdMethod(HRESULT, "GetClipPlane", [(DWORD, "Index"), Out(Pointer(Float), "pPlane")], sideeffects=False),
    StdMethod(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "State"), (D3DRENDERSTATEVALUE, "Value")]),
    StdMethod(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "State"), Out(Pointer(D3DRENDERSTATEVALUE), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "BeginStateBlock", []),
    StdMethod(HRESULT, "EndStateBlock", [Out(Pointer(DWORD), "pToken")]),
    StdMethod(HRESULT, "ApplyStateBlock", [(DWORD, "Token")]),
    StdMethod(HRESULT, "CaptureStateBlock", [(DWORD, "Token")]),
    StdMethod(HRESULT, "DeleteStateBlock", [(DWORD, "Token")]),
    StdMethod(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "Type"), Out(Pointer(DWORD), "pToken")]),
    StdMethod(HRESULT, "SetClipStatus", [(ConstPointer(D3DCLIPSTATUS8), "pClipStatus")]),
    StdMethod(HRESULT, "GetClipStatus", [Out(Pointer(D3DCLIPSTATUS8), "pClipStatus")], sideeffects=False),
    StdMethod(HRESULT, "GetTexture", [(DWORD, "Stage"), Out(Pointer(PDIRECT3DBASETEXTURE8), "ppTexture")]),
    StdMethod(HRESULT, "SetTexture", [(DWORD, "Stage"), (PDIRECT3DBASETEXTURE8, "pTexture")]),
    StdMethod(HRESULT, "GetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), Out(Pointer(D3DTEXTURESTAGESTATEVALUE), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "SetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (D3DTEXTURESTAGESTATEVALUE, "Value")]),
    StdMethod(HRESULT, "ValidateDevice", [Out(Pointer(DWORD), "pNumPasses")]),
    StdMethod(HRESULT, "GetInfo", [(D3DDEVINFOID, "DevInfoID"), Out(OpaqueBlob(Void, "DevInfoStructSize"), "pDevInfoStruct"), (DWORD, "DevInfoStructSize")], sideeffects=False),
    StdMethod(HRESULT, "SetPaletteEntries", [(UINT, "PaletteNumber"), (ConstPointer(PALETTEENTRY), "pEntries")]),
    StdMethod(HRESULT, "GetPaletteEntries", [(UINT, "PaletteNumber"), Out(Pointer(PALETTEENTRY), "pEntries")], sideeffects=False),
    StdMethod(HRESULT, "SetCurrentTexturePalette", [(UINT, "PaletteNumber")]),
    StdMethod(HRESULT, "GetCurrentTexturePalette", [Out(Pointer(UINT), "PaletteNumber")], sideeffects=False),
    StdMethod(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "StartVertex"), (UINT, "PrimitiveCount")]),
    StdMethod(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "minIndex"), (UINT, "NumVertices"), (UINT, "startIndex"), (UINT, "primCount")]),
    StdMethod(HRESULT, "DrawPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "PrimitiveCount"), (Blob(Const(Void), "_vertexDataSize(PrimitiveType, PrimitiveCount, VertexStreamZeroStride)"), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    StdMethod(HRESULT, "DrawIndexedPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "MinVertexIndex"), (UINT, "NumVertexIndices"), (UINT, "PrimitiveCount"), (Blob(Const(Void), "_indexDataSize(PrimitiveType, PrimitiveCount, IndexDataFormat)"), "pIndexData"), (D3DFORMAT, "IndexDataFormat"), (Blob(Const(Void), "NumVertexIndices*VertexStreamZeroStride"), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    StdMethod(HRESULT, "ProcessVertices", [(UINT, "SrcStartIndex"), (UINT, "DestIndex"), (UINT, "VertexCount"), (PDIRECT3DVERTEXBUFFER8, "pDestBuffer"), (D3DPV, "Flags")]),
    StdMethod(HRESULT, "CreateVertexShader", [(Array(Const(DWORD), "_declCount(pDeclaration)"), "pDeclaration"), (D3DSHADER8, "pFunction"), Out(Pointer(DWORD), "pHandle"), (D3DUSAGE, "Usage")]),
    StdMethod(HRESULT, "SetVertexShader", [(DWORD, "Handle")]),
    StdMethod(HRESULT, "GetVertexShader", [Out(Pointer(DWORD), "pHandle")], sideeffects=False),
    StdMethod(HRESULT, "DeleteVertexShader", [(DWORD, "Handle")]),
    StdMethod(HRESULT, "SetVertexShaderConstant", [(DWORD, "Register"), (Blob(Const(Void), "ConstantCount*4*sizeof(float)"), "pConstantData"), (DWORD, "ConstantCount")]),
    StdMethod(HRESULT, "GetVertexShaderConstant", [(DWORD, "Register"), Out(OpaqueBlob(Void, "ConstantCount*4*sizeof(float)"), "pConstantData"), (DWORD, "ConstantCount")], sideeffects=False),
    StdMethod(HRESULT, "GetVertexShaderDeclaration", [(DWORD, "Handle"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "GetVertexShaderFunction", [(DWORD, "Handle"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "SetStreamSource", [(UINT, "StreamNumber"), (PDIRECT3DVERTEXBUFFER8, "pStreamData"), (UINT, "Stride")]),
    StdMethod(HRESULT, "GetStreamSource", [(UINT, "StreamNumber"), Out(Pointer(PDIRECT3DVERTEXBUFFER8), "ppStreamData"), Out(Pointer(UINT), "pStride")]),
    StdMethod(HRESULT, "SetIndices", [(PDIRECT3DINDEXBUFFER8, "pIndexData"), (UINT, "BaseVertexIndex")]),
    StdMethod(HRESULT, "GetIndices", [Out(Pointer(PDIRECT3DINDEXBUFFER8), "ppIndexData"), Out(Pointer(UINT), "pBaseVertexIndex")]),
    StdMethod(HRESULT, "CreatePixelShader", [(D3DSHADER8, "pFunction"), Out(Pointer(DWORD), "pHandle")]),
    StdMethod(HRESULT, "SetPixelShader", [(DWORD, "Handle")]),
    StdMethod(HRESULT, "GetPixelShader", [Out(Pointer(DWORD), "pHandle")], sideeffects=False),
    StdMethod(HRESULT, "DeletePixelShader", [(DWORD, "Handle")]),
    StdMethod(HRESULT, "SetPixelShaderConstant", [(DWORD, "Register"), (Blob(Const(Void), "ConstantCount*4*sizeof(float)"), "pConstantData"), (DWORD, "ConstantCount")]),
    StdMethod(HRESULT, "GetPixelShaderConstant", [(DWORD, "Register"), Out(OpaqueBlob(Void, "ConstantCount*4*sizeof(float)"), "pConstantData"), (DWORD, "ConstantCount")], sideeffects=False),
    StdMethod(HRESULT, "GetPixelShaderFunction", [(DWORD, "Handle"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "DrawRectPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DRECTPATCH_INFO), "pRectPatchInfo")]),
    StdMethod(HRESULT, "DrawTriPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DTRIPATCH_INFO), "pTriPatchInfo")]),
    StdMethod(HRESULT, "DeletePatch", [(UINT, "Handle")]),
]

IDirect3DSwapChain8.methods += [
    StdMethod(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion")]),
    StdMethod(HRESULT, "GetBackBuffer", [(UINT, "BackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), Out(Pointer(PDIRECT3DSURFACE8), "ppBackBuffer")]),
]

IDirect3DResource8.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE8), "ppDevice")]),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaqueBlob(Const(Void), "SizeOfData"), "pData"), (DWORD, "SizeOfData"), (D3DSPD, "Flags")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "FreePrivateData", [(REFGUID, "refguid")], sideeffects=False),
    StdMethod(DWORD, "SetPriority", [(DWORD, "PriorityNew")]),
    StdMethod(DWORD, "GetPriority", [], sideeffects=False),
    StdMethod(Void, "PreLoad", []),
    StdMethod(D3DRESOURCETYPE, "GetType", [], sideeffects=False),
]

IDirect3DBaseTexture8.methods += [
    StdMethod(DWORD, "SetLOD", [(DWORD, "LODNew")]),
    StdMethod(DWORD, "GetLOD", [], sideeffects=False),
    StdMethod(DWORD, "GetLevelCount", [], sideeffects=False),
]

IDirect3DTexture8.methods += [
    StdMethod(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DSURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetSurfaceLevel", [(UINT, "Level"), Out(Pointer(PDIRECT3DSURFACE8), "ppSurfaceLevel")]),
    StdMethod(HRESULT, "LockRect", [(UINT, "Level"), Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockRect", [(UINT, "Level")]),
    StdMethod(HRESULT, "AddDirtyRect", [(ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVolumeTexture8.methods += [
    StdMethod(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DVOLUME_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetVolumeLevel", [(UINT, "Level"), Out(Pointer(PDIRECT3DVOLUME8), "ppVolumeLevel")]),
    StdMethod(HRESULT, "LockBox", [(UINT, "Level"), Out(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockBox", [(UINT, "Level")]),
    StdMethod(HRESULT, "AddDirtyBox", [(ConstPointer(D3DBOX), "pDirtyBox")]),
]

IDirect3DCubeTexture8.methods += [
    StdMethod(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DSURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetCubeMapSurface", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), Out(Pointer(PDIRECT3DSURFACE8), "ppCubeMapSurface")]),
    StdMethod(HRESULT, "LockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level")]),
    StdMethod(HRESULT, "AddDirtyRect", [(D3DCUBEMAP_FACES, "FaceType"), (ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVertexBuffer8.methods += [
    StdMethod(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), Out(Pointer(LinearPointer(BYTE, "_MappedSize")), "ppbData"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DVERTEXBUFFER_DESC), "pDesc")], sideeffects=False),
]

IDirect3DIndexBuffer8.methods += [
    StdMethod(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), Out(Pointer(LinearPointer(BYTE, "_MappedSize")), "ppbData"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DINDEXBUFFER_DESC), "pDesc")], sideeffects=False),
]

IDirect3DSurface8.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE8), "ppDevice")]),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaqueBlob(Const(Void), "SizeOfData"), "pData"), (DWORD, "SizeOfData"), (D3DSPD, "Flags")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "FreePrivateData", [(REFGUID, "refguid")], sideeffects=False),
    StdMethod(HRESULT, "GetContainer", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppContainer")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DSURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "LockRect", [Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockRect", []),
]

IDirect3DVolume8.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE8), "ppDevice")]),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaqueBlob(Const(Void), "SizeOfData"), "pData"), (DWORD, "SizeOfData"), (D3DSPD, "Flags")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "FreePrivateData", [(REFGUID, "refguid")], sideeffects=False),
    StdMethod(HRESULT, "GetContainer", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppContainer")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DVOLUME_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "LockBox", [Out(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockBox", []),
]

d3d8 = Module("d3d8")
d3d8.addFunctions([
    StdFunction(PDIRECT3D8, "Direct3DCreate8", [(UINT, "SDKVersion")]),
])

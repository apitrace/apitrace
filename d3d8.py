#############################################################################
#
# Copyright 2008 Jose Fonseca
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

"""d3d8.h"""

from windows import *
from d3d8types import *
from d3d8caps import *

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

PDIRECT3D8 = WrapPointer(IDirect3D8)
PDIRECT3DDEVICE8 = WrapPointer(IDirect3DDevice8)
PDIRECT3DSWAPCHAIN8 = WrapPointer(IDirect3DSwapChain8)
PDIRECT3DRESOURCE8 = WrapPointer(IDirect3DResource8)
PDIRECT3DBASETEXTURE8 = WrapPointer(IDirect3DBaseTexture8)
PDIRECT3DTEXTURE8 = WrapPointer(IDirect3DTexture8)
PDIRECT3DVOLUMETEXTURE8 = WrapPointer(IDirect3DVolumeTexture8)
PDIRECT3DCUBETEXTURE8 = WrapPointer(IDirect3DCubeTexture8)
PDIRECT3DVERTEXBUFFER8 = WrapPointer(IDirect3DVertexBuffer8)
PDIRECT3DINDEXBUFFER8 = WrapPointer(IDirect3DIndexBuffer8)
PDIRECT3DSURFACE8 = WrapPointer(IDirect3DSurface8)
PDIRECT3DVOLUME8 = WrapPointer(IDirect3DVolume8)

IDirect3D8.methods += [
	Method(HRESULT, "RegisterSoftwareDevice", [(Pointer(Void), "pInitializeFunction")]),
	Method(UINT, "GetAdapterCount", []),
	Method(HRESULT, "GetAdapterIdentifier", [(UINT, "Adapter"), (DWORD, "Flags"), (OutPointer(D3DADAPTER_IDENTIFIER8), "pIdentifier")]),
	Method(UINT, "GetAdapterModeCount", [(UINT, "Adapter")]),
	Method(HRESULT, "EnumAdapterModes", [(UINT, "Adapter"), (UINT, "Mode"), (OutPointer(D3DDISPLAYMODE), "pMode")]),
	Method(HRESULT, "GetAdapterDisplayMode", [(UINT, "Adapter"), (OutPointer(D3DDISPLAYMODE), "pMode")]),
	Method(HRESULT, "CheckDeviceType", [(UINT, "Adapter"), (D3DDEVTYPE, "CheckType"), (D3DFORMAT, "DisplayFormat"), (D3DFORMAT, "BackBufferFormat"), (BOOL, "Windowed")]),
	Method(HRESULT, "CheckDeviceFormat", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (DWORD, "Usage"), (D3DRESOURCETYPE, "RType"), (D3DFORMAT, "CheckFormat")]),
	Method(HRESULT, "CheckDeviceMultiSampleType", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SurfaceFormat"), (BOOL, "Windowed"), (D3DMULTISAMPLE_TYPE, "MultiSampleType")]),
	Method(HRESULT, "CheckDepthStencilMatch", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "RenderTargetFormat"), (D3DFORMAT, "DepthStencilFormat")]),
	Method(HRESULT, "GetDeviceCaps", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (OutPointer(D3DCAPS8), "pCaps")]),
	Method(HMONITOR, "GetAdapterMonitor", [(UINT, "Adapter")]),
	Method(HRESULT, "CreateDevice", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (DWORD, "BehaviorFlags"), (OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (OutPointer(PDIRECT3DDEVICE8), "ppReturnedDeviceInterface")]),
]

IDirect3DDevice8.methods += [
	Method(HRESULT, "TestCooperativeLevel", []),
	Method(UINT, "GetAvailableTextureMem", []),
	Method(HRESULT, "ResourceManagerDiscardBytes", [(DWORD, "Bytes")]),
	Method(HRESULT, "GetDirect3D", [(OutPointer(PDIRECT3D8), "ppD3D8")]),
	Method(HRESULT, "GetDeviceCaps", [(OutPointer(D3DCAPS8), "pCaps")]),
	Method(HRESULT, "GetDisplayMode", [(OutPointer(D3DDISPLAYMODE), "pMode")]),
	Method(HRESULT, "GetCreationParameters", [(OutPointer(D3DDEVICE_CREATION_PARAMETERS), "pParameters")]),
	Method(HRESULT, "SetCursorProperties", [(UINT, "XHotSpot"), (UINT, "YHotSpot"), (PDIRECT3DSURFACE8, "pCursorBitmap")]),
	Method(Void, "SetCursorPosition", [(Int, "X"), (Int, "Y"), (DWORD, "Flags")]),
	Method(BOOL, "ShowCursor", [(BOOL, "bShow")]),
	Method(HRESULT, "CreateAdditionalSwapChain", [(OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (OutPointer(PDIRECT3DSWAPCHAIN8), "pSwapChain")]),
	Method(HRESULT, "Reset", [(OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
	Method(HRESULT, "Present", [(Pointer(Const(RECT)), "pSourceRect"), (Pointer(Const(RECT)), "pDestRect"), (HWND, "hDestWindowOverride"), (Const(Pointer(RGNDATA)), "pDirtyRegion")]),
	Method(HRESULT, "GetBackBuffer", [(UINT, "BackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), (OutPointer(PDIRECT3DSURFACE8), "ppBackBuffer")]),
	Method(HRESULT, "GetRasterStatus", [(OutPointer(D3DRASTER_STATUS), "pRasterStatus")]),
	Method(Void, "SetGammaRamp", [(DWORD, "Flags"), (Pointer(Const(D3DGAMMARAMP)), "pRamp")]),
	Method(Void, "GetGammaRamp", [(OutPointer(D3DGAMMARAMP), "pRamp")]),
	Method(HRESULT, "CreateTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DTEXTURE8), "ppTexture")]),
	Method(HRESULT, "CreateVolumeTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Depth"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DVOLUMETEXTURE8), "ppVolumeTexture")]),
	Method(HRESULT, "CreateCubeTexture", [(UINT, "EdgeLength"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DCUBETEXTURE8), "ppCubeTexture")]),
	Method(HRESULT, "CreateVertexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (DWORD, "FVF"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DVERTEXBUFFER8), "ppVertexBuffer")]),
	Method(HRESULT, "CreateIndexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DINDEXBUFFER8), "ppIndexBuffer")]),
	Method(HRESULT, "CreateRenderTarget", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (BOOL, "Lockable"), (OutPointer(PDIRECT3DSURFACE8), "ppSurface")]),
	Method(HRESULT, "CreateDepthStencilSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (OutPointer(PDIRECT3DSURFACE8), "ppSurface")]),
	Method(HRESULT, "CreateImageSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (OutPointer(PDIRECT3DSURFACE8), "ppSurface")]),
	Method(HRESULT, "CopyRects", [(PDIRECT3DSURFACE8, "pSourceSurface"), (Pointer(Const(RECT)), "pSourceRectsArray"), (UINT, "cRects"), (PDIRECT3DSURFACE8, "pDestinationSurface"), (Pointer(Const(POINT)), "pDestPointsArray")]),
	Method(HRESULT, "UpdateTexture", [(PDIRECT3DBASETEXTURE8, "pSourceTexture"), (PDIRECT3DBASETEXTURE8, "pDestinationTexture")]),
	Method(HRESULT, "GetFrontBuffer", [(PDIRECT3DSURFACE8, "pDestSurface")]),
	Method(HRESULT, "SetRenderTarget", [(PDIRECT3DSURFACE8, "pRenderTarget"), (PDIRECT3DSURFACE8, "pNewZStencil")]),
	Method(HRESULT, "GetRenderTarget", [(OutPointer(PDIRECT3DSURFACE8), "ppRenderTarget")]),
	Method(HRESULT, "GetDepthStencilSurface", [(OutPointer(PDIRECT3DSURFACE8), "ppZStencilSurface")]),
	Method(HRESULT, "BeginScene", []),
	Method(HRESULT, "EndScene", []),
	Method(HRESULT, "Clear", [(DWORD, "Count"), (Pointer(Const(D3DRECT)), "pRects"), (DWORD, "Flags"), (D3DCOLOR, "Color"), (Float, "Z"), (DWORD, "Stencil")]),
	Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (Pointer(Const(D3DMATRIX)), "pMatrix")]),
	Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (OutPointer(D3DMATRIX), "pMatrix")]),
	Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "State"), (Pointer(Const(D3DMATRIX)), "pMatrix")]),
	Method(HRESULT, "SetViewport", [(Pointer(Const(D3DVIEWPORT8)), "pViewport")]),
	Method(HRESULT, "GetViewport", [(OutPointer(D3DVIEWPORT8), "pViewport")]),
	Method(HRESULT, "SetMaterial", [(Pointer(Const(D3DMATERIAL8)), "pMaterial")]),
	Method(HRESULT, "GetMaterial", [(OutPointer(D3DMATERIAL8), "pMaterial")]),
	Method(HRESULT, "SetLight", [(DWORD, "Index"), (Pointer(Const(D3DLIGHT8)), "pLight")]),
	Method(HRESULT, "GetLight", [(DWORD, "Index"), (OutPointer(D3DLIGHT8), "pLight")]),
	Method(HRESULT, "LightEnable", [(DWORD, "Index"), (BOOL, "Enable")]),
	Method(HRESULT, "GetLightEnable", [(DWORD, "Index"), (OutPointer(BOOL), "pEnable")]),
	Method(HRESULT, "SetClipPlane", [(DWORD, "Index"), (Pointer(Const(Float)), "pPlane")]),
	Method(HRESULT, "GetClipPlane", [(DWORD, "Index"), (OutPointer(Float), "pPlane")]),
	Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "State"), (DWORD, "Value")]),
	Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "State"), (OutPointer(DWORD), "pValue")]),
	Method(HRESULT, "BeginStateBlock", []),
	Method(HRESULT, "EndStateBlock", [(OutPointer(DWORD), "pToken")]),
	Method(HRESULT, "ApplyStateBlock", [(DWORD, "Token")]),
	Method(HRESULT, "CaptureStateBlock", [(DWORD, "Token")]),
	Method(HRESULT, "DeleteStateBlock", [(DWORD, "Token")]),
	Method(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "Type"), (OutPointer(DWORD), "pToken")]),
	Method(HRESULT, "SetClipStatus", [(Pointer(Const(D3DCLIPSTATUS8)), "pClipStatus")]),
	Method(HRESULT, "GetClipStatus", [(OutPointer(D3DCLIPSTATUS8), "pClipStatus")]),
	Method(HRESULT, "GetTexture", [(DWORD, "Stage"), (OutPointer(PDIRECT3DBASETEXTURE8), "ppTexture")]),
	Method(HRESULT, "SetTexture", [(DWORD, "Stage"), (PDIRECT3DBASETEXTURE8, "pTexture")]),
	Method(HRESULT, "GetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (OutPointer(DWORD), "pValue")]),
	Method(HRESULT, "SetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (DWORD, "Value")]),
	Method(HRESULT, "ValidateDevice", [(OutPointer(DWORD), "pNumPasses")]),
	Method(HRESULT, "GetInfo", [(DWORD, "DevInfoID"), (OutPointer(Void), "pDevInfoStruct"), (DWORD, "DevInfoStructSize")]),
	Method(HRESULT, "SetPaletteEntries", [(UINT, "PaletteNumber"), (Pointer(Const(PALETTEENTRY)), "pEntries")]),
	Method(HRESULT, "GetPaletteEntries", [(UINT, "PaletteNumber"), (OutPointer(PALETTEENTRY), "pEntries")]),
	Method(HRESULT, "SetCurrentTexturePalette", [(UINT, "PaletteNumber")]),
	Method(HRESULT, "GetCurrentTexturePalette", [(OutPointer(UINT), "PaletteNumber")]),
	Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "StartVertex"), (UINT, "PrimitiveCount")]),
	Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "minIndex"), (UINT, "NumVertices"), (UINT, "startIndex"), (UINT, "primCount")]),
	Method(HRESULT, "DrawPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "PrimitiveCount"), (Const(Pointer(Void)), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
	Method(HRESULT, "DrawIndexedPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "MinVertexIndex"), (UINT, "NumVertexIndices"), (UINT, "PrimitiveCount"), (Const(Pointer(Void)), "pIndexData"), (D3DFORMAT, "IndexDataFormat"), (Const(Pointer(Void)), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
	Method(HRESULT, "ProcessVertices", [(UINT, "SrcStartIndex"), (UINT, "DestIndex"), (UINT, "VertexCount"), (PDIRECT3DVERTEXBUFFER8, "pDestBuffer"), (DWORD, "Flags")]),
	Method(HRESULT, "CreateVertexShader", [(Pointer(Const(DWORD)), "pDeclaration"), (Pointer(Const(DWORD)), "pFunction"), (OutPointer(DWORD), "pHandle"), (DWORD, "Usage")]),
	Method(HRESULT, "SetVertexShader", [(DWORD, "Handle")]),
	Method(HRESULT, "GetVertexShader", [(OutPointer(DWORD), "pHandle")]),
	Method(HRESULT, "DeleteVertexShader", [(DWORD, "Handle")]),
	Method(HRESULT, "SetVertexShaderConstant", [(DWORD, "Register"), (Const(Pointer(Void)), "pConstantData"), (DWORD, "ConstantCount")]),
	Method(HRESULT, "GetVertexShaderConstant", [(DWORD, "Register"), (OutPointer(Void), "pConstantData"), (DWORD, "ConstantCount")]),
	Method(HRESULT, "GetVertexShaderDeclaration", [(DWORD, "Handle"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
	Method(HRESULT, "GetVertexShaderFunction", [(DWORD, "Handle"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
	Method(HRESULT, "SetStreamSource", [(UINT, "StreamNumber"), (PDIRECT3DVERTEXBUFFER8, "pStreamData"), (UINT, "Stride")]),
	Method(HRESULT, "GetStreamSource", [(UINT, "StreamNumber"), (OutPointer(PDIRECT3DVERTEXBUFFER8), "ppStreamData"), (OutPointer(UINT), "pStride")]),
	Method(HRESULT, "SetIndices", [(PDIRECT3DINDEXBUFFER8, "pIndexData"), (UINT, "BaseVertexIndex")]),
	Method(HRESULT, "GetIndices", [(OutPointer(PDIRECT3DINDEXBUFFER8), "ppIndexData"), (OutPointer(UINT), "pBaseVertexIndex")]),
	Method(HRESULT, "CreatePixelShader", [(Pointer(Const(DWORD)), "pFunction"), (OutPointer(DWORD), "pHandle")]),
	Method(HRESULT, "SetPixelShader", [(DWORD, "Handle")]),
	Method(HRESULT, "GetPixelShader", [(OutPointer(DWORD), "pHandle")]),
	Method(HRESULT, "DeletePixelShader", [(DWORD, "Handle")]),
	Method(HRESULT, "SetPixelShaderConstant", [(DWORD, "Register"), (Const(Pointer(Void)), "pConstantData"), (DWORD, "ConstantCount")]),
	Method(HRESULT, "GetPixelShaderConstant", [(DWORD, "Register"), (OutPointer(Void), "pConstantData"), (DWORD, "ConstantCount")]),
	Method(HRESULT, "GetPixelShaderFunction", [(DWORD, "Handle"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
	Method(HRESULT, "DrawRectPatch", [(UINT, "Handle"), (Pointer(Const(Float)), "pNumSegs"), (Pointer(Const(D3DRECTPATCH_INFO)), "pRectPatchInfo")]),
	Method(HRESULT, "DrawTriPatch", [(UINT, "Handle"), (Pointer(Const(Float)), "pNumSegs"), (Pointer(Const(D3DTRIPATCH_INFO)), "pTriPatchInfo")]),
	Method(HRESULT, "DeletePatch", [(UINT, "Handle")]),
]

IDirect3DSwapChain8.methods += [
	Method(HRESULT, "Present", [(Pointer(Const(RECT)), "pSourceRect"), (Pointer(Const(RECT)), "pDestRect"), (HWND, "hDestWindowOverride"), (Const(Pointer(RGNDATA)), "pDirtyRegion")]),
	Method(HRESULT, "GetBackBuffer", [(UINT, "BackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), (OutPointer(PDIRECT3DSURFACE8), "ppBackBuffer")]),
]

IDirect3DResource8.methods += [
	Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE8), "ppDevice")]),
	Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (Const(Pointer(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
	Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
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
	Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (OutPointer(D3DSURFACE_DESC), "pDesc")]),
	Method(HRESULT, "GetSurfaceLevel", [(UINT, "Level"), (OutPointer(PDIRECT3DSURFACE8), "ppSurfaceLevel")]),
	Method(HRESULT, "LockRect", [(UINT, "Level"), (OutPointer(D3DLOCKED_RECT), "pLockedRect"), (Pointer(Const(RECT)), "pRect"), (DWORD, "Flags")]),
	Method(HRESULT, "UnlockRect", [(UINT, "Level")]),
	Method(HRESULT, "AddDirtyRect", [(Pointer(Const(RECT)), "pDirtyRect")]),
]

IDirect3DVolumeTexture8.methods += [
	Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (OutPointer(D3DVOLUME_DESC), "pDesc")]),
	Method(HRESULT, "GetVolumeLevel", [(UINT, "Level"), (OutPointer(PDIRECT3DVOLUME8), "ppVolumeLevel")]),
	Method(HRESULT, "LockBox", [(UINT, "Level"), (OutPointer(D3DLOCKED_BOX), "pLockedVolume"), (Pointer(Const(D3DBOX)), "pBox"), (DWORD, "Flags")]),
	Method(HRESULT, "UnlockBox", [(UINT, "Level")]),
	Method(HRESULT, "AddDirtyBox", [(Pointer(Const(D3DBOX)), "pDirtyBox")]),
]

IDirect3DCubeTexture8.methods += [
	Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (OutPointer(D3DSURFACE_DESC), "pDesc")]),
	Method(HRESULT, "GetCubeMapSurface", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), (OutPointer(PDIRECT3DSURFACE8), "ppCubeMapSurface")]),
	Method(HRESULT, "LockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), (OutPointer(D3DLOCKED_RECT), "pLockedRect"), (Pointer(Const(RECT)), "pRect"), (DWORD, "Flags")]),
	Method(HRESULT, "UnlockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level")]),
	Method(HRESULT, "AddDirtyRect", [(D3DCUBEMAP_FACES, "FaceType"), (Pointer(Const(RECT)), "pDirtyRect")]),
]

IDirect3DVertexBuffer8.methods += [
	Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), (OutPointer(Pointer(BYTE)), "ppbData"), (DWORD, "Flags")]),
	Method(HRESULT, "Unlock", []),
	Method(HRESULT, "GetDesc", [(OutPointer(D3DVERTEXBUFFER_DESC), "pDesc")]),
]

IDirect3DIndexBuffer8.methods += [
	Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), (OutPointer(Pointer(BYTE)), "ppbData"), (DWORD, "Flags")]),
	Method(HRESULT, "Unlock", []),
	Method(HRESULT, "GetDesc", [(OutPointer(D3DINDEXBUFFER_DESC), "pDesc")]),
]

IDirect3DSurface8.methods += [
	Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE8), "ppDevice")]),
	Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (Const(Pointer(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
	Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
	Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
	Method(HRESULT, "GetContainer", [(REFIID, "riid"), (OutPointer(Pointer(Void)), "ppContainer")]),
	Method(HRESULT, "GetDesc", [(OutPointer(D3DSURFACE_DESC), "pDesc")]),
	Method(HRESULT, "LockRect", [(OutPointer(D3DLOCKED_RECT), "pLockedRect"), (Pointer(Const(RECT)), "pRect"), (DWORD, "Flags")]),
	Method(HRESULT, "UnlockRect", []),
]

IDirect3DVolume8.methods += [
	Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE8), "ppDevice")]),
	Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (Const(Pointer(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
	Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
	Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
	Method(HRESULT, "GetContainer", [(REFIID, "riid"), (OutPointer(Pointer(Void)), "ppContainer")]),
	Method(HRESULT, "GetDesc", [(OutPointer(D3DVOLUME_DESC), "pDesc")]),
	Method(HRESULT, "LockBox", [(OutPointer(D3DLOCKED_BOX), "pLockedVolume"), (Pointer(Const(D3DBOX)), "pBox"), (DWORD, "Flags")]),
	Method(HRESULT, "UnlockBox", []),
]

d3d8 = Dll("d3d8")
d3d8.functions += [
    Function(PDIRECT3D8, "Direct3DCreate8", [(UINT, "SDKVersion")]),
]

if __name__ == '__main__':
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print '#include <d3d8.h>'
    print
    print '#include "log.hpp"'
    print
    wrap()


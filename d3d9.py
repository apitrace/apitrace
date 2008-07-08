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

"""d3d9.h"""

from windows import *
from d3d9types import *
from d3d9caps import *

IDirect3D9 = Interface("IDirect3D9", IUnknown)
IDirect3DDevice9 = Interface("IDirect3DDevice9", IUnknown)
IDirect3DStateBlock9 = Interface("IDirect3DStateBlock9", IUnknown)
IDirect3DSwapChain9 = Interface("IDirect3DSwapChain9", IUnknown)
IDirect3DResource9 = Interface("IDirect3DResource9", IUnknown)
IDirect3DVertexDeclaration9 = Interface("IDirect3DVertexDeclaration9", IUnknown)
IDirect3DVertexShader9 = Interface("IDirect3DVertexShader9", IUnknown)
IDirect3DPixelShader9 = Interface("IDirect3DPixelShader9", IUnknown)
IDirect3DBaseTexture9 = Interface("IDirect3DBaseTexture9", IDirect3DResource9)
IDirect3DTexture9 = Interface("IDirect3DTexture9", IDirect3DBaseTexture9)
IDirect3DVolumeTexture9 = Interface("IDirect3DVolumeTexture9", IDirect3DBaseTexture9)
IDirect3DCubeTexture9 = Interface("IDirect3DCubeTexture9", IDirect3DBaseTexture9)
IDirect3DVertexBuffer9 = Interface("IDirect3DVertexBuffer9", IDirect3DResource9)
IDirect3DIndexBuffer9 = Interface("IDirect3DIndexBuffer9", IDirect3DResource9)
IDirect3DSurface9 = Interface("IDirect3DSurface9", IDirect3DResource9)
IDirect3DVolume9 = Interface("IDirect3DVolume9", IUnknown)
IDirect3DQuery9 = Interface("IDirect3DQuery9", IUnknown)
IDirect3D9Ex = Interface("IDirect3D9Ex", IDirect3D9)
IDirect3DDevice9Ex = Interface("IDirect3DDevice9Ex", IDirect3DDevice9)
IDirect3DSwapChain9Ex = Interface("IDirect3DSwapChain9Ex", IDirect3DSwapChain9)

PDIRECT3D9 = WrapPointer(IDirect3D9)
PDIRECT3DDEVICE9 = WrapPointer(IDirect3DDevice9)
PDIRECT3DSTATEBLOCK9 = WrapPointer(IDirect3DStateBlock9)
PDIRECT3DSWAPCHAIN9 = WrapPointer(IDirect3DSwapChain9)
PDIRECT3DRESOURCE9 = WrapPointer(IDirect3DResource9)
PDIRECT3DVERTEXDECLARATION9 = WrapPointer(IDirect3DVertexDeclaration9)
PDIRECT3DVERTEXSHADER9 = WrapPointer(IDirect3DVertexShader9)
PDIRECT3DPIXELSHADER9 = WrapPointer(IDirect3DPixelShader9)
PDIRECT3DBASETEXTURE9 = WrapPointer(IDirect3DBaseTexture9)
PDIRECT3DTEXTURE9 = WrapPointer(IDirect3DTexture9)
PDIRECT3DVOLUMETEXTURE9 = WrapPointer(IDirect3DVolumeTexture9)
PDIRECT3DCUBETEXTURE9 = WrapPointer(IDirect3DCubeTexture9)
PDIRECT3DVERTEXBUFFER9 = WrapPointer(IDirect3DVertexBuffer9)
PDIRECT3DINDEXBUFFER9 = WrapPointer(IDirect3DIndexBuffer9)
PDIRECT3DSURFACE9 = WrapPointer(IDirect3DSurface9)
PDIRECT3DVOLUME9 = WrapPointer(IDirect3DVolume9)
PDIRECT3DQUERY9 = WrapPointer(IDirect3DQuery9)
PDIRECT3D9EX = WrapPointer(IDirect3D9Ex)
PDIRECT3DDEVICE9EX = WrapPointer(IDirect3DDevice9Ex)
PDIRECT3DSWAPCHAIN9EX = WrapPointer(IDirect3DSwapChain9Ex)

IDirect3D9.methods += [
    Method(HRESULT, "RegisterSoftwareDevice", [(Pointer(Void), "pInitializeFunction")]),
    Method(UINT, "GetAdapterCount", []),
    Method(HRESULT, "GetAdapterIdentifier", [(UINT, "Adapter"), (DWORD, "Flags"), (Pointer(D3DADAPTER_IDENTIFIER9), "pIdentifier")]),
    Method(UINT, "GetAdapterModeCount", [(UINT, "Adapter"), (D3DFORMAT, "Format")]),
    Method(HRESULT, "EnumAdapterModes", [(UINT, "Adapter"), (D3DFORMAT, "Format"), (UINT, "Mode"), (Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetAdapterDisplayMode", [(UINT, "Adapter"), (Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "CheckDeviceType", [(UINT, "Adapter"), (D3DDEVTYPE, "DevType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "BackBufferFormat"), (BOOL, "bWindowed")]),
    Method(HRESULT, "CheckDeviceFormat", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (DWORD, "Usage"), (D3DRESOURCETYPE, "RType"), (D3DFORMAT, "CheckFormat")]),
    Method(HRESULT, "CheckDeviceMultiSampleType", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SurfaceFormat"), (BOOL, "Windowed"), (D3DMULTISAMPLE_TYPE, "MultiSampleType"), (Pointer(DWORD), "pQualityLevels")]),
    Method(HRESULT, "CheckDepthStencilMatch", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "RenderTargetFormat"), (D3DFORMAT, "DepthStencilFormat")]),
    Method(HRESULT, "CheckDeviceFormatConversion", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SourceFormat"), (D3DFORMAT, "TargetFormat")]),
    Method(HRESULT, "GetDeviceCaps", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (Pointer(D3DCAPS9), "pCaps")]),
    Method(HMONITOR, "GetAdapterMonitor", [(UINT, "Adapter")]),
    Method(HRESULT, "CreateDevice", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (DWORD, "BehaviorFlags"), (Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (Pointer(Pointer(IDirect3DDevice9)), "ppReturnedDeviceInterface")]),
]

IDirect3DDevice9.methods += [
    Method(HRESULT, "TestCooperativeLevel", []),
    Method(UINT, "GetAvailableTextureMem", []),
    Method(HRESULT, "EvictManagedResources", []),
    Method(HRESULT, "GetDirect3D", [(Pointer(Pointer(IDirect3D9)), "ppD3D9")]),
    Method(HRESULT, "GetDeviceCaps", [(Pointer(D3DCAPS9), "pCaps")]),
    Method(HRESULT, "GetDisplayMode", [(UINT, "iSwapChain"), (Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetCreationParameters", [(Pointer(D3DDEVICE_CREATION_PARAMETERS), "pParameters")]),
    Method(HRESULT, "SetCursorProperties", [(UINT, "XHotSpot"), (UINT, "YHotSpot"), (Pointer(IDirect3DSurface9), "pCursorBitmap")]),
    Method(Void, "SetCursorPosition", [(Int, "X"), (Int, "Y"), (DWORD, "Flags")]),
    Method(BOOL, "ShowCursor", [(BOOL, "bShow")]),
    Method(HRESULT, "CreateAdditionalSwapChain", [(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (Pointer(Pointer(IDirect3DSwapChain9)), "pSwapChain")]),
    Method(HRESULT, "GetSwapChain", [(UINT, "iSwapChain"), (Pointer(Pointer(IDirect3DSwapChain9)), "pSwapChain")]),
    Method(UINT, "GetNumberOfSwapChains", []),
    Method(HRESULT, "Reset", [(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
    Method(HRESULT, "Present", [(Pointer(Const(RECT)), "pSourceRect"), (Pointer(Const(RECT)), "pDestRect"), (HWND, "hDestWindowOverride"), (Pointer(Const(RGNDATA)), "pDirtyRegion")]),
    Method(HRESULT, "GetBackBuffer", [(UINT, "iSwapChain"), (UINT, "iBackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), (Pointer(Pointer(IDirect3DSurface9)), "ppBackBuffer")]),
    Method(HRESULT, "GetRasterStatus", [(UINT, "iSwapChain"), (Pointer(D3DRASTER_STATUS), "pRasterStatus")]),
    Method(HRESULT, "SetDialogBoxMode", [(BOOL, "bEnableDialogs")]),
    Method(Void, "SetGammaRamp", [(UINT, "iSwapChain"), (DWORD, "Flags"), (Pointer(Const(D3DGAMMARAMP)), "pRamp")]),
    Method(Void, "GetGammaRamp", [(UINT, "iSwapChain"), (Pointer(D3DGAMMARAMP), "pRamp")]),
    Method(HRESULT, "CreateTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DTexture9)), "ppTexture"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateVolumeTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Depth"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DVolumeTexture9)), "ppVolumeTexture"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateCubeTexture", [(UINT, "EdgeLength"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DCubeTexture9)), "ppCubeTexture"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateVertexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (DWORD, "FVF"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DVertexBuffer9)), "ppVertexBuffer"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateIndexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DIndexBuffer9)), "ppIndexBuffer"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateRenderTarget", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Lockable"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateDepthStencilSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Discard"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "UpdateSurface", [(Pointer(IDirect3DSurface9), "pSourceSurface"), (Pointer(Const(RECT)), "pSourceRect"), (Pointer(IDirect3DSurface9), "pDestinationSurface"), (Pointer(Const(POINT)), "pDestPoint")]),
    Method(HRESULT, "UpdateTexture", [(Pointer(IDirect3DBaseTexture9), "pSourceTexture"), (Pointer(IDirect3DBaseTexture9), "pDestinationTexture")]),
    Method(HRESULT, "GetRenderTargetData", [(Pointer(IDirect3DSurface9), "pRenderTarget"), (Pointer(IDirect3DSurface9), "pDestSurface")]),
    Method(HRESULT, "GetFrontBufferData", [(UINT, "iSwapChain"), (Pointer(IDirect3DSurface9), "pDestSurface")]),
    Method(HRESULT, "StretchRect", [(Pointer(IDirect3DSurface9), "pSourceSurface"), (Pointer(Const(RECT)), "pSourceRect"), (Pointer(IDirect3DSurface9), "pDestSurface"), (Pointer(Const(RECT)), "pDestRect"), (D3DTEXTUREFILTERTYPE, "Filter")]),
    Method(HRESULT, "ColorFill", [(Pointer(IDirect3DSurface9), "pSurface"), (Pointer(Const(RECT)), "pRect"), (D3DCOLOR, "color")]),
    Method(HRESULT, "CreateOffscreenPlainSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "SetRenderTarget", [(DWORD, "RenderTargetIndex"), (Pointer(IDirect3DSurface9), "pRenderTarget")]),
    Method(HRESULT, "GetRenderTarget", [(DWORD, "RenderTargetIndex"), (Pointer(Pointer(IDirect3DSurface9)), "ppRenderTarget")]),
    Method(HRESULT, "SetDepthStencilSurface", [(Pointer(IDirect3DSurface9), "pNewZStencil")]),
    Method(HRESULT, "GetDepthStencilSurface", [(Pointer(Pointer(IDirect3DSurface9)), "ppZStencilSurface")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "Clear", [(DWORD, "Count"), (Pointer(Const(D3DRECT)), "pRects"), (DWORD, "Flags"), (D3DCOLOR, "Color"), (Float, "Z"), (DWORD, "Stencil")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (Pointer(Const(D3DMATRIX)), "pMatrix")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (Pointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "State"), (Pointer(Const(D3DMATRIX)), "pMatrix")]),
    Method(HRESULT, "SetViewport", [(Pointer(Const(D3DVIEWPORT9)), "pViewport")]),
    Method(HRESULT, "GetViewport", [(Pointer(D3DVIEWPORT9), "pViewport")]),
    Method(HRESULT, "SetMaterial", [(Pointer(Const(D3DMATERIAL9)), "pMaterial")]),
    Method(HRESULT, "GetMaterial", [(Pointer(D3DMATERIAL9), "pMaterial")]),
    Method(HRESULT, "SetLight", [(DWORD, "Index"), (Pointer(Const(D3DLIGHT9)), "pLight")]),
    Method(HRESULT, "GetLight", [(DWORD, "Index"), (Pointer(D3DLIGHT9), "pLight")]),
    Method(HRESULT, "LightEnable", [(DWORD, "Index"), (BOOL, "Enable")]),
    Method(HRESULT, "GetLightEnable", [(DWORD, "Index"), (Pointer(BOOL), "pEnable")]),
    Method(HRESULT, "SetClipPlane", [(DWORD, "Index"), (Pointer(Const(Float)), "pPlane")]),
    Method(HRESULT, "GetClipPlane", [(DWORD, "Index"), (Pointer(Float), "pPlane")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "State"), (DWORD, "Value")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "State"), (Pointer(DWORD), "pValue")]),
    Method(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "Type"), (Pointer(Pointer(IDirect3DStateBlock9)), "ppSB")]),
    Method(HRESULT, "BeginStateBlock", []),
    Method(HRESULT, "EndStateBlock", [(Pointer(Pointer(IDirect3DStateBlock9)), "ppSB")]),
    Method(HRESULT, "SetClipStatus", [(Pointer(Const(D3DCLIPSTATUS9)), "pClipStatus")]),
    Method(HRESULT, "GetClipStatus", [(Pointer(D3DCLIPSTATUS9), "pClipStatus")]),
    Method(HRESULT, "GetTexture", [(DWORD, "Stage"), (Pointer(Pointer(IDirect3DBaseTexture9)), "ppTexture")]),
    Method(HRESULT, "SetTexture", [(DWORD, "Stage"), (Pointer(IDirect3DBaseTexture9), "pTexture")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (Pointer(DWORD), "pValue")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (DWORD, "Value")]),
    Method(HRESULT, "GetSamplerState", [(DWORD, "Sampler"), (D3DSAMPLERSTATETYPE, "Type"), (Pointer(DWORD), "pValue")]),
    Method(HRESULT, "SetSamplerState", [(DWORD, "Sampler"), (D3DSAMPLERSTATETYPE, "Type"), (DWORD, "Value")]),
    Method(HRESULT, "ValidateDevice", [(Pointer(DWORD), "pNumPasses")]),
    Method(HRESULT, "SetPaletteEntries", [(UINT, "PaletteNumber"), (Pointer(Const(PALETTEENTRY)), "pEntries")]),
    Method(HRESULT, "GetPaletteEntries", [(UINT, "PaletteNumber"), (Pointer(PALETTEENTRY), "pEntries")]),
    Method(HRESULT, "SetCurrentTexturePalette", [(UINT, "PaletteNumber")]),
    Method(HRESULT, "GetCurrentTexturePalette", [(Pointer(UINT), "PaletteNumber")]),
    Method(HRESULT, "SetScissorRect", [(Pointer(Const(RECT)), "pRect")]),
    Method(HRESULT, "GetScissorRect", [(Pointer(RECT), "pRect")]),
    Method(HRESULT, "SetSoftwareVertexProcessing", [(BOOL, "bSoftware")]),
    Method(BOOL, "GetSoftwareVertexProcessing", []),
    Method(HRESULT, "SetNPatchMode", [(Float, "nSegments")]),
    Method(Float, "GetNPatchMode", []),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "StartVertex"), (UINT, "PrimitiveCount")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (INT, "BaseVertexIndex"), (UINT, "MinVertexIndex"), (UINT, "NumVertices"), (UINT, "startIndex"), (UINT, "primCount")]),
    Method(HRESULT, "DrawPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "PrimitiveCount"), (Pointer(Const(Void)), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    Method(HRESULT, "DrawIndexedPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "MinVertexIndex"), (UINT, "NumVertices"), (UINT, "PrimitiveCount"), (Pointer(Const(Void)), "pIndexData"), (D3DFORMAT, "IndexDataFormat"), (Pointer(Const(Void)), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    Method(HRESULT, "ProcessVertices", [(UINT, "SrcStartIndex"), (UINT, "DestIndex"), (UINT, "VertexCount"), (Pointer(IDirect3DVertexBuffer9), "pDestBuffer"), (Pointer(IDirect3DVertexDeclaration9), "pVertexDecl"), (DWORD, "Flags")]),
    Method(HRESULT, "CreateVertexDeclaration", [(Pointer(Const(D3DVERTEXELEMENT9)), "pVertexElements"), (Pointer(Pointer(IDirect3DVertexDeclaration9)), "ppDecl")]),
    Method(HRESULT, "SetVertexDeclaration", [(Pointer(IDirect3DVertexDeclaration9), "pDecl")]),
    Method(HRESULT, "GetVertexDeclaration", [(Pointer(Pointer(IDirect3DVertexDeclaration9)), "ppDecl")]),
    Method(HRESULT, "SetFVF", [(DWORD, "FVF")]),
    Method(HRESULT, "GetFVF", [(Pointer(DWORD), "pFVF")]),
    Method(HRESULT, "CreateVertexShader", [(Pointer(Const(DWORD)), "pFunction"), (Pointer(Pointer(IDirect3DVertexShader9)), "ppShader")]),
    Method(HRESULT, "SetVertexShader", [(Pointer(IDirect3DVertexShader9), "pShader")]),
    Method(HRESULT, "GetVertexShader", [(Pointer(Pointer(IDirect3DVertexShader9)), "ppShader")]),
    Method(HRESULT, "SetVertexShaderConstantF", [(UINT, "StartRegister"), (Pointer(Const(Float)), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "GetVertexShaderConstantF", [(UINT, "StartRegister"), (Pointer(Float), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "SetVertexShaderConstantI", [(UINT, "StartRegister"), (Pointer(Const(Int)), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "GetVertexShaderConstantI", [(UINT, "StartRegister"), (Pointer(Int), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "SetVertexShaderConstantB", [(UINT, "StartRegister"), (Pointer(Const(BOOL)), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "GetVertexShaderConstantB", [(UINT, "StartRegister"), (Pointer(BOOL), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "SetStreamSource", [(UINT, "StreamNumber"), (Pointer(IDirect3DVertexBuffer9), "pStreamData"), (UINT, "OffsetInBytes"), (UINT, "Stride")]),
    Method(HRESULT, "GetStreamSource", [(UINT, "StreamNumber"), (Pointer(Pointer(IDirect3DVertexBuffer9)), "ppStreamData"), (Pointer(UINT), "pOffsetInBytes"), (Pointer(UINT), "pStride")]),
    Method(HRESULT, "SetStreamSourceFreq", [(UINT, "StreamNumber"), (UINT, "Setting")]),
    Method(HRESULT, "GetStreamSourceFreq", [(UINT, "StreamNumber"), (Pointer(UINT), "pSetting")]),
    Method(HRESULT, "SetIndices", [(Pointer(IDirect3DIndexBuffer9), "pIndexData")]),
    Method(HRESULT, "GetIndices", [(Pointer(Pointer(IDirect3DIndexBuffer9)), "ppIndexData")]),
    Method(HRESULT, "CreatePixelShader", [(Pointer(Const(DWORD)), "pFunction"), (Pointer(Pointer(IDirect3DPixelShader9)), "ppShader")]),
    Method(HRESULT, "SetPixelShader", [(Pointer(IDirect3DPixelShader9), "pShader")]),
    Method(HRESULT, "GetPixelShader", [(Pointer(Pointer(IDirect3DPixelShader9)), "ppShader")]),
    Method(HRESULT, "SetPixelShaderConstantF", [(UINT, "StartRegister"), (Pointer(Const(Float)), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "GetPixelShaderConstantF", [(UINT, "StartRegister"), (Pointer(Float), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "SetPixelShaderConstantI", [(UINT, "StartRegister"), (Pointer(Const(Int)), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "GetPixelShaderConstantI", [(UINT, "StartRegister"), (Pointer(Int), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "SetPixelShaderConstantB", [(UINT, "StartRegister"), (Pointer(Const(BOOL)), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "GetPixelShaderConstantB", [(UINT, "StartRegister"), (Pointer(BOOL), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "DrawRectPatch", [(UINT, "Handle"), (Pointer(Const(Float)), "pNumSegs"), (Pointer(Const(D3DRECTPATCH_INFO)), "pRectPatchInfo")]),
    Method(HRESULT, "DrawTriPatch", [(UINT, "Handle"), (Pointer(Const(Float)), "pNumSegs"), (Pointer(Const(D3DTRIPATCH_INFO)), "pTriPatchInfo")]),
    Method(HRESULT, "DeletePatch", [(UINT, "Handle")]),
    Method(HRESULT, "CreateQuery", [(D3DQUERYTYPE, "Type"), (Pointer(Pointer(IDirect3DQuery9)), "ppQuery")]),
]

IDirect3DStateBlock9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "Capture", []),
    Method(HRESULT, "Apply", []),
]

IDirect3DSwapChain9.methods += [
    Method(HRESULT, "Present", [(Pointer(Const(RECT)), "pSourceRect"), (Pointer(Const(RECT)), "pDestRect"), (HWND, "hDestWindowOverride"), (Pointer(Const(RGNDATA)), "pDirtyRegion"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetFrontBufferData", [(Pointer(IDirect3DSurface9), "pDestSurface")]),
    Method(HRESULT, "GetBackBuffer", [(UINT, "iBackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), (Pointer(Pointer(IDirect3DSurface9)), "ppBackBuffer")]),
    Method(HRESULT, "GetRasterStatus", [(Pointer(D3DRASTER_STATUS), "pRasterStatus")]),
    Method(HRESULT, "GetDisplayMode", [(Pointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "GetPresentParameters", [(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
]

IDirect3DResource9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (Pointer(Const(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (Pointer(Void), "pData"), (Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(DWORD, "SetPriority", [(DWORD, "PriorityNew")]),
    Method(DWORD, "GetPriority", []),
    Method(Void, "PreLoad", []),
    Method(D3DRESOURCETYPE, "GetType", []),
]

IDirect3DVertexDeclaration9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "GetDeclaration", [(Pointer(D3DVERTEXELEMENT9), "pElement"), (Pointer(UINT), "pNumElements")]),
]

IDirect3DVertexShader9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "GetFunction", [(Pointer(Void), "pData"), (Pointer(UINT), "pSizeOfData")]),
]

IDirect3DPixelShader9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "GetFunction", [(Pointer(Void), "pData"), (Pointer(UINT), "pSizeOfData")]),
]

IDirect3DBaseTexture9.methods += [
    Method(DWORD, "SetLOD", [(DWORD, "LODNew")]),
    Method(DWORD, "GetLOD", []),
    Method(DWORD, "GetLevelCount", []),
    Method(HRESULT, "SetAutoGenFilterType", [(D3DTEXTUREFILTERTYPE, "FilterType")]),
    Method(D3DTEXTUREFILTERTYPE, "GetAutoGenFilterType", []),
    Method(Void, "GenerateMipSubLevels", []),
]

IDirect3DTexture9.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (Pointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "GetSurfaceLevel", [(UINT, "Level"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurfaceLevel")]),
    Method(HRESULT, "LockRect", [(UINT, "Level"), (Pointer(D3DLOCKED_RECT), "pLockedRect"), (Pointer(Const(RECT)), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", [(UINT, "Level")]),
    Method(HRESULT, "AddDirtyRect", [(Pointer(Const(RECT)), "pDirtyRect")]),
]

IDirect3DVolumeTexture9.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (Pointer(D3DVOLUME_DESC), "pDesc")]),
    Method(HRESULT, "GetVolumeLevel", [(UINT, "Level"), (Pointer(Pointer(IDirect3DVolume9)), "ppVolumeLevel")]),
    Method(HRESULT, "LockBox", [(UINT, "Level"), (Pointer(D3DLOCKED_BOX), "pLockedVolume"), (Pointer(Const(D3DBOX)), "pBox"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockBox", [(UINT, "Level")]),
    Method(HRESULT, "AddDirtyBox", [(Pointer(Const(D3DBOX)), "pDirtyBox")]),
]

IDirect3DCubeTexture9.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (Pointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "GetCubeMapSurface", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), (Pointer(Pointer(IDirect3DSurface9)), "ppCubeMapSurface")]),
    Method(HRESULT, "LockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), (Pointer(D3DLOCKED_RECT), "pLockedRect"), (Pointer(Const(RECT)), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level")]),
    Method(HRESULT, "AddDirtyRect", [(D3DCUBEMAP_FACES, "FaceType"), (Pointer(Const(RECT)), "pDirtyRect")]),
]

IDirect3DVertexBuffer9.methods += [
    Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), (Pointer(Pointer(Void)), "ppbData"), (DWORD, "Flags")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "GetDesc", [(Pointer(D3DVERTEXBUFFER_DESC), "pDesc")]),
]

IDirect3DIndexBuffer9.methods += [
    Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), (Pointer(Pointer(Void)), "ppbData"), (DWORD, "Flags")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "GetDesc", [(Pointer(D3DINDEXBUFFER_DESC), "pDesc")]),
]

IDirect3DSurface9.methods += [
    Method(HRESULT, "GetContainer", [(REFIID, "riid"), (Pointer(Pointer(Void)), "ppContainer")]),
    Method(HRESULT, "GetDesc", [(Pointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "LockRect", [(Pointer(D3DLOCKED_RECT), "pLockedRect"), (Pointer(Const(RECT)), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", []),
    Method(HRESULT, "GetDC", [(Pointer(HDC), "phdc")]),
    Method(HRESULT, "ReleaseDC", [(HDC, "hdc")]),
]

IDirect3DVolume9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (Pointer(Const(Void)), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (Pointer(Void), "pData"), (Pointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(HRESULT, "GetContainer", [(REFIID, "riid"), (Pointer(Pointer(Void)), "ppContainer")]),
    Method(HRESULT, "GetDesc", [(Pointer(D3DVOLUME_DESC), "pDesc")]),
    Method(HRESULT, "LockBox", [(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (Pointer(Const(D3DBOX)), "pBox"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockBox", []),
]

IDirect3DQuery9.methods += [
    Method(HRESULT, "GetDevice", [(Pointer(Pointer(IDirect3DDevice9)), "ppDevice")]),
    Method(D3DQUERYTYPE, "GetType", []),
    Method(DWORD, "GetDataSize", []),
    Method(HRESULT, "Issue", [(DWORD, "dwIssueFlags")]),
    Method(HRESULT, "GetData", [(Pointer(Void), "pData"), (DWORD, "dwSize"), (DWORD, "dwGetDataFlags")]),
]

IDirect3D9Ex.methods += [
    Method(UINT, "GetAdapterModeCountEx", [(UINT, "Adapter"), (Pointer(Const(D3DDISPLAYMODEFILTER)), "pFilter") ]),
    Method(HRESULT, "EnumAdapterModesEx", [(UINT, "Adapter"), (Pointer(Const(D3DDISPLAYMODEFILTER)), "pFilter"), (UINT, "Mode"), (Pointer(D3DDISPLAYMODEEX), "pMode")]),
    Method(HRESULT, "GetAdapterDisplayModeEx", [(UINT, "Adapter"), (Pointer(D3DDISPLAYMODEEX), "pMode"), (Pointer(D3DDISPLAYROTATION), "pRotation")]),
    Method(HRESULT, "CreateDeviceEx", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (DWORD, "BehaviorFlags"), (Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (Pointer(D3DDISPLAYMODEEX), "pFullscreenDisplayMode"), (Pointer(Pointer(IDirect3DDevice9Ex)), "ppReturnedDeviceInterface")]),
    Method(HRESULT, "GetAdapterLUID", [(UINT, "Adapter"), (Pointer(LUID), "pLUID")]),
]

IDirect3DDevice9Ex.methods += [
    Method(HRESULT, "SetConvolutionMonoKernel", [(UINT, "width"), (UINT, "height"), (Pointer(Float), "rows"), (Pointer(Float), "columns")]),
    Method(HRESULT, "ComposeRects", [(Pointer(IDirect3DSurface9), "pSrc"), (Pointer(IDirect3DSurface9), "pDst"), (Pointer(IDirect3DVertexBuffer9), "pSrcRectDescs"), (UINT, "NumRects"), (Pointer(IDirect3DVertexBuffer9), "pDstRectDescs"), (D3DCOMPOSERECTSOP, "Operation"), (Int, "Xoffset"), (Int, "Yoffset")]),
    Method(HRESULT, "PresentEx", [(Pointer(Const(RECT)), "pSourceRect"), (Pointer(Const(RECT)), "pDestRect"), (HWND, "hDestWindowOverride"), (Pointer(Const(RGNDATA)), "pDirtyRegion"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetGPUThreadPriority", [(Pointer(INT), "pPriority")]),
    Method(HRESULT, "SetGPUThreadPriority", [(INT, "Priority")]),
    Method(HRESULT, "WaitForVBlank", [(UINT, "iSwapChain")]),
    Method(HRESULT, "CheckResourceResidency", [(Pointer(Pointer(IDirect3DResource9)), "pResourceArray"), (UINT32, "NumResources")]),
    Method(HRESULT, "SetMaximumFrameLatency", [(UINT, "MaxLatency")]),
    Method(HRESULT, "GetMaximumFrameLatency", [(Pointer(UINT), "pMaxLatency")]),
    Method(HRESULT, "CheckDeviceState", [(HWND, "hDestinationWindow")]),
    Method(HRESULT, "CreateRenderTargetEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Lockable"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurface"), (Pointer(HANDLE), "pSharedHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "CreateOffscreenPlainSurfaceEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurface"), (Pointer(HANDLE), "pSharedHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "CreateDepthStencilSurfaceEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Discard"), (Pointer(Pointer(IDirect3DSurface9)), "ppSurface"), (Pointer(HANDLE), "pSharedHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "ResetEx", [(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (Pointer(D3DDISPLAYMODEEX), "pFullscreenDisplayMode")]),
    Method(HRESULT, "GetDisplayModeEx", [(UINT, "iSwapChain"), (Pointer(D3DDISPLAYMODEEX), "pMode"), (Pointer(D3DDISPLAYROTATION), "pRotation")]),
]

IDirect3DSwapChain9Ex.methods += [
    Method(HRESULT, "GetLastPresentCount", [(Pointer(UINT), "pLastPresentCount")]),
    Method(HRESULT, "GetPresentStats", [(Pointer(D3DPRESENTSTATS), "pPresentationStatistics")]),
    Method(HRESULT, "GetDisplayModeEx", [(Pointer(D3DDISPLAYMODEEX), "pMode"), (Pointer(D3DDISPLAYROTATION), "pRotation")]),
]

d3d9 = Dll("d3d9")
d3d9.functions += [
    Function(PDIRECT3D9, "Direct3DCreate9", [(UINT, "SDKVersion")]),
    Function(HRESULT, "Direct3DCreate9Ex", [(UINT, "SDKVersion"), (Pointer(Pointer(IDirect3D9Ex)), "ppD3D")]),
]

if __name__ == '__main__':
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print '#include <d3d9.h>'
    print
    print '#include "log.hpp"'
    print
    wrap()


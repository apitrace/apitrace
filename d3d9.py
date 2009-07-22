#############################################################################
#
# Copyright 2008 Tungsten Graphics, Inc.
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
from d3dshader import *
from d3d9types import *
from d3d9caps import *

D3DSHADER9 = D3DShader(9)

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
    "D3DERR_WASSTILLDRAWING",
    "D3DOK_NOAUTOGEN",
    "D3DERR_DEVICEREMOVED",
    "S_NOT_RESIDENT",
    "S_RESIDENT_IN_SHARED_MEMORY",
    "S_PRESENT_MODE_CHANGED",
    "S_PRESENT_OCCLUDED",
    "D3DERR_DEVICEHUNG",
])

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
    Method(HRESULT, "GetAdapterIdentifier", [(UINT, "Adapter"), (DWORD, "Flags"), (OutPointer(D3DADAPTER_IDENTIFIER9), "pIdentifier")]),
    Method(UINT, "GetAdapterModeCount", [(UINT, "Adapter"), (D3DFORMAT, "Format")]),
    Method(HRESULT, "EnumAdapterModes", [(UINT, "Adapter"), (D3DFORMAT, "Format"), (UINT, "Mode"), (OutPointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetAdapterDisplayMode", [(UINT, "Adapter"), (OutPointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "CheckDeviceType", [(UINT, "Adapter"), (D3DDEVTYPE, "DevType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "BackBufferFormat"), (BOOL, "bWindowed")]),
    Method(HRESULT, "CheckDeviceFormat", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (DWORD, "Usage"), (D3DRESOURCETYPE, "RType"), (D3DFORMAT, "CheckFormat")]),
    Method(HRESULT, "CheckDeviceMultiSampleType", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SurfaceFormat"), (BOOL, "Windowed"), (D3DMULTISAMPLE_TYPE, "MultiSampleType"), (OutPointer(DWORD), "pQualityLevels")]),
    Method(HRESULT, "CheckDepthStencilMatch", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "RenderTargetFormat"), (D3DFORMAT, "DepthStencilFormat")]),
    Method(HRESULT, "CheckDeviceFormatConversion", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SourceFormat"), (D3DFORMAT, "TargetFormat")]),
    Method(HRESULT, "GetDeviceCaps", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (OutPointer(D3DCAPS9), "pCaps")]),
    Method(HMONITOR, "GetAdapterMonitor", [(UINT, "Adapter")]),
    Method(HRESULT, "CreateDevice", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (DWORD, "BehaviorFlags"), (OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (OutPointer(PDIRECT3DDEVICE9), "ppReturnedDeviceInterface")]),
]

IDirect3DDevice9.methods += [
    Method(HRESULT, "TestCooperativeLevel", []),
    Method(UINT, "GetAvailableTextureMem", []),
    Method(HRESULT, "EvictManagedResources", []),
    Method(HRESULT, "GetDirect3D", [(OutPointer(PDIRECT3D9), "ppD3D9")]),
    Method(HRESULT, "GetDeviceCaps", [(OutPointer(D3DCAPS9), "pCaps")]),
    Method(HRESULT, "GetDisplayMode", [(UINT, "iSwapChain"), (OutPointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetCreationParameters", [(OutPointer(D3DDEVICE_CREATION_PARAMETERS), "pParameters")]),
    Method(HRESULT, "SetCursorProperties", [(UINT, "XHotSpot"), (UINT, "YHotSpot"), (PDIRECT3DSURFACE9, "pCursorBitmap")]),
    Method(Void, "SetCursorPosition", [(Int, "X"), (Int, "Y"), (DWORD, "Flags")]),
    Method(BOOL, "ShowCursor", [(BOOL, "bShow")]),
    Method(HRESULT, "CreateAdditionalSwapChain", [(OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (OutPointer(PDIRECT3DSWAPCHAIN9), "pSwapChain")]),
    Method(HRESULT, "GetSwapChain", [(UINT, "iSwapChain"), (OutPointer(PDIRECT3DSWAPCHAIN9), "pSwapChain")]),
    Method(UINT, "GetNumberOfSwapChains", []),
    Method(HRESULT, "Reset", [(OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
    Method(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion")]),
    Method(HRESULT, "GetBackBuffer", [(UINT, "iSwapChain"), (UINT, "iBackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), (OutPointer(PDIRECT3DSURFACE9), "ppBackBuffer")]),
    Method(HRESULT, "GetRasterStatus", [(UINT, "iSwapChain"), (OutPointer(D3DRASTER_STATUS), "pRasterStatus")]),
    Method(HRESULT, "SetDialogBoxMode", [(BOOL, "bEnableDialogs")]),
    Method(Void, "SetGammaRamp", [(UINT, "iSwapChain"), (DWORD, "Flags"), (ConstPointer(D3DGAMMARAMP), "pRamp")]),
    Method(Void, "GetGammaRamp", [(UINT, "iSwapChain"), (OutPointer(D3DGAMMARAMP), "pRamp")]),
    Method(HRESULT, "CreateTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DTEXTURE9), "ppTexture"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateVolumeTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Depth"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DVOLUMETEXTURE9), "ppVolumeTexture"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateCubeTexture", [(UINT, "EdgeLength"), (UINT, "Levels"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DCUBETEXTURE9), "ppCubeTexture"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateVertexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (DWORD, "FVF"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DVERTEXBUFFER9), "ppVertexBuffer"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateIndexBuffer", [(UINT, "Length"), (DWORD, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DINDEXBUFFER9), "ppIndexBuffer"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateRenderTarget", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Lockable"), (OutPointer(PDIRECT3DSURFACE9), "ppSurface"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "CreateDepthStencilSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Discard"), (OutPointer(PDIRECT3DSURFACE9), "ppSurface"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "UpdateSurface", [(PDIRECT3DSURFACE9, "pSourceSurface"), (ConstPointer(RECT), "pSourceRect"), (PDIRECT3DSURFACE9, "pDestinationSurface"), (ConstPointer(POINT), "pDestPoint")]),
    Method(HRESULT, "UpdateTexture", [(PDIRECT3DBASETEXTURE9, "pSourceTexture"), (PDIRECT3DBASETEXTURE9, "pDestinationTexture")]),
    Method(HRESULT, "GetRenderTargetData", [(PDIRECT3DSURFACE9, "pRenderTarget"), (PDIRECT3DSURFACE9, "pDestSurface")]),
    Method(HRESULT, "GetFrontBufferData", [(UINT, "iSwapChain"), (PDIRECT3DSURFACE9, "pDestSurface")]),
    Method(HRESULT, "StretchRect", [(PDIRECT3DSURFACE9, "pSourceSurface"), (ConstPointer(RECT), "pSourceRect"), (PDIRECT3DSURFACE9, "pDestSurface"), (ConstPointer(RECT), "pDestRect"), (D3DTEXTUREFILTERTYPE, "Filter")]),
    Method(HRESULT, "ColorFill", [(PDIRECT3DSURFACE9, "pSurface"), (ConstPointer(RECT), "pRect"), (D3DCOLOR, "color")]),
    Method(HRESULT, "CreateOffscreenPlainSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DSURFACE9), "ppSurface"), (OutPointer(HANDLE), "pSharedHandle")]),
    Method(HRESULT, "SetRenderTarget", [(DWORD, "RenderTargetIndex"), (PDIRECT3DSURFACE9, "pRenderTarget")]),
    Method(HRESULT, "GetRenderTarget", [(DWORD, "RenderTargetIndex"), (OutPointer(PDIRECT3DSURFACE9), "ppRenderTarget")]),
    Method(HRESULT, "SetDepthStencilSurface", [(PDIRECT3DSURFACE9, "pNewZStencil")]),
    Method(HRESULT, "GetDepthStencilSurface", [(OutPointer(PDIRECT3DSURFACE9), "ppZStencilSurface")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "Clear", [(DWORD, "Count"), (ConstPointer(D3DRECT), "pRects"), (DWORD, "Flags"), (D3DCOLOR, "Color"), (Float, "Z"), (DWORD, "Stencil")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (OutPointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    Method(HRESULT, "SetViewport", [(ConstPointer(D3DVIEWPORT9), "pViewport")]),
    Method(HRESULT, "GetViewport", [(OutPointer(D3DVIEWPORT9), "pViewport")]),
    Method(HRESULT, "SetMaterial", [(ConstPointer(D3DMATERIAL9), "pMaterial")]),
    Method(HRESULT, "GetMaterial", [(OutPointer(D3DMATERIAL9), "pMaterial")]),
    Method(HRESULT, "SetLight", [(DWORD, "Index"), (ConstPointer(D3DLIGHT9), "pLight")]),
    Method(HRESULT, "GetLight", [(DWORD, "Index"), (OutPointer(D3DLIGHT9), "pLight")]),
    Method(HRESULT, "LightEnable", [(DWORD, "Index"), (BOOL, "Enable")]),
    Method(HRESULT, "GetLightEnable", [(DWORD, "Index"), (OutPointer(BOOL), "pEnable")]),
    Method(HRESULT, "SetClipPlane", [(DWORD, "Index"), (ConstPointer(Float), "pPlane")]),
    Method(HRESULT, "GetClipPlane", [(DWORD, "Index"), (OutPointer(Float), "pPlane")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "State"), (DWORD, "Value")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "State"), (OutPointer(DWORD), "pValue")]),
    Method(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "Type"), (OutPointer(PDIRECT3DSTATEBLOCK9), "ppSB")]),
    Method(HRESULT, "BeginStateBlock", []),
    Method(HRESULT, "EndStateBlock", [(OutPointer(PDIRECT3DSTATEBLOCK9), "ppSB")]),
    Method(HRESULT, "SetClipStatus", [(ConstPointer(D3DCLIPSTATUS9), "pClipStatus")]),
    Method(HRESULT, "GetClipStatus", [(OutPointer(D3DCLIPSTATUS9), "pClipStatus")]),
    Method(HRESULT, "GetTexture", [(DWORD, "Stage"), (OutPointer(PDIRECT3DBASETEXTURE9), "ppTexture")]),
    Method(HRESULT, "SetTexture", [(DWORD, "Stage"), (PDIRECT3DBASETEXTURE9, "pTexture")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (OutPointer(DWORD), "pValue")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (DWORD, "Value")]),
    Method(HRESULT, "GetSamplerState", [(DWORD, "Sampler"), (D3DSAMPLERSTATETYPE, "Type"), (OutPointer(DWORD), "pValue")]),
    Method(HRESULT, "SetSamplerState", [(DWORD, "Sampler"), (D3DSAMPLERSTATETYPE, "Type"), (DWORD, "Value")]),
    Method(HRESULT, "ValidateDevice", [(OutPointer(DWORD), "pNumPasses")]),
    Method(HRESULT, "SetPaletteEntries", [(UINT, "PaletteNumber"), (ConstPointer(PALETTEENTRY), "pEntries")]),
    Method(HRESULT, "GetPaletteEntries", [(UINT, "PaletteNumber"), (OutPointer(PALETTEENTRY), "pEntries")]),
    Method(HRESULT, "SetCurrentTexturePalette", [(UINT, "PaletteNumber")]),
    Method(HRESULT, "GetCurrentTexturePalette", [(OutPointer(UINT), "PaletteNumber")]),
    Method(HRESULT, "SetScissorRect", [(ConstPointer(RECT), "pRect")]),
    Method(HRESULT, "GetScissorRect", [(OutPointer(RECT), "pRect")]),
    Method(HRESULT, "SetSoftwareVertexProcessing", [(BOOL, "bSoftware")]),
    Method(BOOL, "GetSoftwareVertexProcessing", []),
    Method(HRESULT, "SetNPatchMode", [(Float, "nSegments")]),
    Method(Float, "GetNPatchMode", []),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "StartVertex"), (UINT, "PrimitiveCount")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (INT, "BaseVertexIndex"), (UINT, "MinVertexIndex"), (UINT, "NumVertices"), (UINT, "startIndex"), (UINT, "primCount")]),
    Method(HRESULT, "DrawPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "PrimitiveCount"), (ConstPointer(Void), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    Method(HRESULT, "DrawIndexedPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "MinVertexIndex"), (UINT, "NumVertices"), (UINT, "PrimitiveCount"), (ConstPointer(Void), "pIndexData"), (D3DFORMAT, "IndexDataFormat"), (ConstPointer(Void), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    Method(HRESULT, "ProcessVertices", [(UINT, "SrcStartIndex"), (UINT, "DestIndex"), (UINT, "VertexCount"), (PDIRECT3DVERTEXBUFFER9, "pDestBuffer"), (PDIRECT3DVERTEXDECLARATION9, "pVertexDecl"), (DWORD, "Flags")]),
    Method(HRESULT, "CreateVertexDeclaration", [(ConstPointer(D3DVERTEXELEMENT9), "pVertexElements"), (OutPointer(PDIRECT3DVERTEXDECLARATION9), "ppDecl")]),
    Method(HRESULT, "SetVertexDeclaration", [(PDIRECT3DVERTEXDECLARATION9, "pDecl")]),
    Method(HRESULT, "GetVertexDeclaration", [(OutPointer(PDIRECT3DVERTEXDECLARATION9), "ppDecl")]),
    Method(HRESULT, "SetFVF", [(DWORD, "FVF")]),
    Method(HRESULT, "GetFVF", [(OutPointer(DWORD), "pFVF")]),
    Method(HRESULT, "CreateVertexShader", [(D3DSHADER9, "pFunction"), (OutPointer(PDIRECT3DVERTEXSHADER9), "ppShader")]),
    Method(HRESULT, "SetVertexShader", [(PDIRECT3DVERTEXSHADER9, "pShader")]),
    Method(HRESULT, "GetVertexShader", [(OutPointer(PDIRECT3DVERTEXSHADER9), "ppShader")]),
    Method(HRESULT, "SetVertexShaderConstantF", [(UINT, "StartRegister"), (ConstPointer(Float), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "GetVertexShaderConstantF", [(UINT, "StartRegister"), (OutPointer(Float), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "SetVertexShaderConstantI", [(UINT, "StartRegister"), (ConstPointer(Int), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "GetVertexShaderConstantI", [(UINT, "StartRegister"), (OutPointer(Int), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "SetVertexShaderConstantB", [(UINT, "StartRegister"), (ConstPointer(BOOL), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "GetVertexShaderConstantB", [(UINT, "StartRegister"), (OutPointer(BOOL), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "SetStreamSource", [(UINT, "StreamNumber"), (PDIRECT3DVERTEXBUFFER9, "pStreamData"), (UINT, "OffsetInBytes"), (UINT, "Stride")]),
    Method(HRESULT, "GetStreamSource", [(UINT, "StreamNumber"), (OutPointer(PDIRECT3DVERTEXBUFFER9), "ppStreamData"), (OutPointer(UINT), "pOffsetInBytes"), (OutPointer(UINT), "pStride")]),
    Method(HRESULT, "SetStreamSourceFreq", [(UINT, "StreamNumber"), (UINT, "Setting")]),
    Method(HRESULT, "GetStreamSourceFreq", [(UINT, "StreamNumber"), (OutPointer(UINT), "pSetting")]),
    Method(HRESULT, "SetIndices", [(PDIRECT3DINDEXBUFFER9, "pIndexData")]),
    Method(HRESULT, "GetIndices", [(OutPointer(PDIRECT3DINDEXBUFFER9), "ppIndexData")]),
    Method(HRESULT, "CreatePixelShader", [(D3DSHADER9, "pFunction"), (OutPointer(PDIRECT3DPIXELSHADER9), "ppShader")]),
    Method(HRESULT, "SetPixelShader", [(PDIRECT3DPIXELSHADER9, "pShader")]),
    Method(HRESULT, "GetPixelShader", [(OutPointer(PDIRECT3DPIXELSHADER9), "ppShader")]),
    Method(HRESULT, "SetPixelShaderConstantF", [(UINT, "StartRegister"), (ConstPointer(Float), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "GetPixelShaderConstantF", [(UINT, "StartRegister"), (OutPointer(Float), "pConstantData"), (UINT, "Vector4fCount")]),
    Method(HRESULT, "SetPixelShaderConstantI", [(UINT, "StartRegister"), (ConstPointer(Int), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "GetPixelShaderConstantI", [(UINT, "StartRegister"), (OutPointer(Int), "pConstantData"), (UINT, "Vector4iCount")]),
    Method(HRESULT, "SetPixelShaderConstantB", [(UINT, "StartRegister"), (ConstPointer(BOOL), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "GetPixelShaderConstantB", [(UINT, "StartRegister"), (OutPointer(BOOL), "pConstantData"), (UINT, "BoolCount")]),
    Method(HRESULT, "DrawRectPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DRECTPATCH_INFO), "pRectPatchInfo")]),
    Method(HRESULT, "DrawTriPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DTRIPATCH_INFO), "pTriPatchInfo")]),
    Method(HRESULT, "DeletePatch", [(UINT, "Handle")]),
    Method(HRESULT, "CreateQuery", [(D3DQUERYTYPE, "Type"), (OutPointer(PDIRECT3DQUERY9), "ppQuery")]),
]

IDirect3DStateBlock9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "Capture", []),
    Method(HRESULT, "Apply", []),
]

IDirect3DSwapChain9.methods += [
    Method(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetFrontBufferData", [(PDIRECT3DSURFACE9, "pDestSurface")]),
    Method(HRESULT, "GetBackBuffer", [(UINT, "iBackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), (OutPointer(PDIRECT3DSURFACE9), "ppBackBuffer")]),
    Method(HRESULT, "GetRasterStatus", [(OutPointer(D3DRASTER_STATUS), "pRasterStatus")]),
    Method(HRESULT, "GetDisplayMode", [(OutPointer(D3DDISPLAYMODE), "pMode")]),
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "GetPresentParameters", [(OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
]

IDirect3DResource9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (ConstPointer(Void), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(DWORD, "SetPriority", [(DWORD, "PriorityNew")]),
    Method(DWORD, "GetPriority", []),
    Method(Void, "PreLoad", []),
    Method(D3DRESOURCETYPE, "GetType", []),
]

IDirect3DVertexDeclaration9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "GetDeclaration", [(OutPointer(D3DVERTEXELEMENT9), "pElement"), (OutPointer(UINT), "pNumElements")]),
]

IDirect3DVertexShader9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "GetFunction", [(OutPointer(Void), "pData"), (OutPointer(UINT), "pSizeOfData")]),
]

IDirect3DPixelShader9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "GetFunction", [(OutPointer(Void), "pData"), (OutPointer(UINT), "pSizeOfData")]),
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
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (OutPointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "GetSurfaceLevel", [(UINT, "Level"), (OutPointer(PDIRECT3DSURFACE9), "ppSurfaceLevel")]),
    Method(HRESULT, "LockRect", [(UINT, "Level"), (OutPointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", [(UINT, "Level")]),
    Method(HRESULT, "AddDirtyRect", [(ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVolumeTexture9.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (OutPointer(D3DVOLUME_DESC), "pDesc")]),
    Method(HRESULT, "GetVolumeLevel", [(UINT, "Level"), (OutPointer(PDIRECT3DVOLUME9), "ppVolumeLevel")]),
    Method(HRESULT, "LockBox", [(UINT, "Level"), (OutPointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockBox", [(UINT, "Level")]),
    Method(HRESULT, "AddDirtyBox", [(ConstPointer(D3DBOX), "pDirtyBox")]),
]

IDirect3DCubeTexture9.methods += [
    Method(HRESULT, "GetLevelDesc", [(UINT, "Level"), (OutPointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "GetCubeMapSurface", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), (OutPointer(PDIRECT3DSURFACE9), "ppCubeMapSurface")]),
    Method(HRESULT, "LockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), (OutPointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level")]),
    Method(HRESULT, "AddDirtyRect", [(D3DCUBEMAP_FACES, "FaceType"), (ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVertexBuffer9.methods += [
    Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), (OutPointer(Pointer(Void)), "ppbData"), (DWORD, "Flags")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "GetDesc", [(OutPointer(D3DVERTEXBUFFER_DESC), "pDesc")]),
]

IDirect3DIndexBuffer9.methods += [
    Method(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), (OutPointer(Pointer(Void)), "ppbData"), (DWORD, "Flags")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "GetDesc", [(OutPointer(D3DINDEXBUFFER_DESC), "pDesc")]),
]

IDirect3DSurface9.methods += [
    Method(HRESULT, "GetContainer", [(REFIID, "riid"), (OutPointer(Pointer(Void)), "ppContainer")]),
    Method(HRESULT, "GetDesc", [(OutPointer(D3DSURFACE_DESC), "pDesc")]),
    Method(HRESULT, "LockRect", [(OutPointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockRect", []),
    Method(HRESULT, "GetDC", [(OutPointer(HDC), "phdc")]),
    Method(HRESULT, "ReleaseDC", [(HDC, "hdc")]),
]

IDirect3DVolume9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (ConstPointer(Void), "pData"), (DWORD, "SizeOfData"), (DWORD, "Flags")]),
    Method(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), (OutPointer(Void), "pData"), (OutPointer(DWORD), "pSizeOfData")]),
    Method(HRESULT, "FreePrivateData", [(REFGUID, "refguid")]),
    Method(HRESULT, "GetContainer", [(REFIID, "riid"), (OutPointer(Pointer(Void)), "ppContainer")]),
    Method(HRESULT, "GetDesc", [(OutPointer(D3DVOLUME_DESC), "pDesc")]),
    Method(HRESULT, "LockBox", [(OutPointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (DWORD, "Flags")]),
    Method(HRESULT, "UnlockBox", []),
]

IDirect3DQuery9.methods += [
    Method(HRESULT, "GetDevice", [(OutPointer(PDIRECT3DDEVICE9), "ppDevice")]),
    Method(D3DQUERYTYPE, "GetType", []),
    Method(DWORD, "GetDataSize", []),
    Method(HRESULT, "Issue", [(DWORD, "dwIssueFlags")]),
    Method(HRESULT, "GetData", [(OutPointer(Void), "pData"), (DWORD, "dwSize"), (DWORD, "dwGetDataFlags")]),
]

IDirect3D9Ex.methods += [
    Method(UINT, "GetAdapterModeCountEx", [(UINT, "Adapter"), (ConstPointer(D3DDISPLAYMODEFILTER), "pFilter") ]),
    Method(HRESULT, "EnumAdapterModesEx", [(UINT, "Adapter"), (ConstPointer(D3DDISPLAYMODEFILTER), "pFilter"), (UINT, "Mode"), (OutPointer(D3DDISPLAYMODEEX), "pMode")]),
    Method(HRESULT, "GetAdapterDisplayModeEx", [(UINT, "Adapter"), (OutPointer(D3DDISPLAYMODEEX), "pMode"), (OutPointer(D3DDISPLAYROTATION), "pRotation")]),
    Method(HRESULT, "CreateDeviceEx", [(UINT, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (DWORD, "BehaviorFlags"), (OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (OutPointer(D3DDISPLAYMODEEX), "pFullscreenDisplayMode"), (OutPointer(PDIRECT3DDEVICE9EX), "ppReturnedDeviceInterface")]),
    Method(HRESULT, "GetAdapterLUID", [(UINT, "Adapter"), (OutPointer(LUID), "pLUID")]),
]

IDirect3DDevice9Ex.methods += [
    Method(HRESULT, "SetConvolutionMonoKernel", [(UINT, "width"), (UINT, "height"), (OutPointer(Float), "rows"), (OutPointer(Float), "columns")]),
    Method(HRESULT, "ComposeRects", [(PDIRECT3DSURFACE9, "pSrc"), (PDIRECT3DSURFACE9, "pDst"), (PDIRECT3DVERTEXBUFFER9, "pSrcRectDescs"), (UINT, "NumRects"), (PDIRECT3DVERTEXBUFFER9, "pDstRectDescs"), (D3DCOMPOSERECTSOP, "Operation"), (Int, "Xoffset"), (Int, "Yoffset")]),
    Method(HRESULT, "PresentEx", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetGPUThreadPriority", [(OutPointer(INT), "pPriority")]),
    Method(HRESULT, "SetGPUThreadPriority", [(INT, "Priority")]),
    Method(HRESULT, "WaitForVBlank", [(UINT, "iSwapChain")]),
    Method(HRESULT, "CheckResourceResidency", [(OutPointer(PDIRECT3DRESOURCE9), "pResourceArray"), (UINT32, "NumResources")]),
    Method(HRESULT, "SetMaximumFrameLatency", [(UINT, "MaxLatency")]),
    Method(HRESULT, "GetMaximumFrameLatency", [(OutPointer(UINT), "pMaxLatency")]),
    Method(HRESULT, "CheckDeviceState", [(HWND, "hDestinationWindow")]),
    Method(HRESULT, "CreateRenderTargetEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Lockable"), (OutPointer(PDIRECT3DSURFACE9), "ppSurface"), (OutPointer(HANDLE), "pSharedHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "CreateOffscreenPlainSurfaceEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (OutPointer(PDIRECT3DSURFACE9), "ppSurface"), (OutPointer(HANDLE), "pSharedHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "CreateDepthStencilSurfaceEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Discard"), (OutPointer(PDIRECT3DSURFACE9), "ppSurface"), (OutPointer(HANDLE), "pSharedHandle"), (DWORD, "Usage")]),
    Method(HRESULT, "ResetEx", [(OutPointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), (OutPointer(D3DDISPLAYMODEEX), "pFullscreenDisplayMode")]),
    Method(HRESULT, "GetDisplayModeEx", [(UINT, "iSwapChain"), (OutPointer(D3DDISPLAYMODEEX), "pMode"), (OutPointer(D3DDISPLAYROTATION), "pRotation")]),
]

IDirect3DSwapChain9Ex.methods += [
    Method(HRESULT, "GetLastPresentCount", [(OutPointer(UINT), "pLastPresentCount")]),
    Method(HRESULT, "GetPresentStats", [(OutPointer(D3DPRESENTSTATS), "pPresentationStatistics")]),
    Method(HRESULT, "GetDisplayModeEx", [(OutPointer(D3DDISPLAYMODEEX), "pMode"), (OutPointer(D3DDISPLAYROTATION), "pRotation")]),
]

d3d9 = Dll("d3d9")
d3d9.functions += [
    DllFunction(PDIRECT3D9, "Direct3DCreate9", [(UINT, "SDKVersion")], fail='NULL'),
    DllFunction(HRESULT, "Direct3DCreate9Ex", [(UINT, "SDKVersion"), (OutPointer(PDIRECT3D9EX), "ppD3D")], fail='D3DERR_NOTAVAILABLE'),
    DllFunction(Int, "D3DPERF_BeginEvent", [(D3DCOLOR, "col"), (LPCWSTR, "wszName")], fail='-1'),
    DllFunction(Int, "D3DPERF_EndEvent", [], fail='-1'),
    DllFunction(Void, "D3DPERF_SetMarker", [(D3DCOLOR, "col"), (LPCWSTR, "wszName")], fail=''),
    DllFunction(Void, "D3DPERF_SetRegion", [(D3DCOLOR, "col"), (LPCWSTR, "wszName")], fail=''),
    DllFunction(BOOL, "D3DPERF_QueryRepeatFrame", [], fail='FALSE'),
    DllFunction(Void, "D3DPERF_SetOptions", [(DWORD, "dwOptions")], fail=''),
    DllFunction(DWORD, "D3DPERF_GetStatus", [], fail='0'),
]

if __name__ == '__main__':
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print '#include <d3d9.h>'
    print
    print '#include "log.hpp"'
    print
    wrap()


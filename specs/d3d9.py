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

"""d3d9.h"""

from winapi import *
from d3d9types import *
from d3d9caps import *


D3DSHADER9 = Blob(Const(DWORD), "_shaderSize(pFunction)")

D3DSPD = Flags(DWORD, [
    "D3DSPD_IUNKNOWN",
])

D3DADAPTER = FakeEnum(UINT, [
    "D3DADAPTER_DEFAULT",
])

D3DENUM = FakeEnum(DWORD, [
    "D3DENUM_WHQL_LEVEL",
])

D3DSGR = Flags(DWORD, [
    "D3DSGR_NO_CALIBRATION",
    "D3DSGR_CALIBRATE",
])

D3DCURSOR = Flags(DWORD, [
    "D3DCURSOR_IMMEDIATE_UPDATE",
])

D3DPRESENT = Flags(DWORD, [
    "D3DPRESENT_DONOTWAIT",
    "D3DPRESENT_LINEAR_CONTENT",
    "D3DPRESENT_DONOTFLIP",
    "D3DPRESENT_FLIPRESTART",
    "D3DPRESENT_VIDEO_RESTRICT_TO_MONITOR",
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

PDIRECT3D9 = ObjPointer(IDirect3D9)
PDIRECT3DDEVICE9 = ObjPointer(IDirect3DDevice9)
PDIRECT3DSTATEBLOCK9 = ObjPointer(IDirect3DStateBlock9)
PDIRECT3DSWAPCHAIN9 = ObjPointer(IDirect3DSwapChain9)
PDIRECT3DRESOURCE9 = ObjPointer(IDirect3DResource9)
PDIRECT3DVERTEXDECLARATION9 = ObjPointer(IDirect3DVertexDeclaration9)
PDIRECT3DVERTEXSHADER9 = ObjPointer(IDirect3DVertexShader9)
PDIRECT3DPIXELSHADER9 = ObjPointer(IDirect3DPixelShader9)
PDIRECT3DBASETEXTURE9 = ObjPointer(IDirect3DBaseTexture9)
PDIRECT3DTEXTURE9 = ObjPointer(IDirect3DTexture9)
PDIRECT3DVOLUMETEXTURE9 = ObjPointer(IDirect3DVolumeTexture9)
PDIRECT3DCUBETEXTURE9 = ObjPointer(IDirect3DCubeTexture9)
PDIRECT3DVERTEXBUFFER9 = ObjPointer(IDirect3DVertexBuffer9)
PDIRECT3DINDEXBUFFER9 = ObjPointer(IDirect3DIndexBuffer9)
PDIRECT3DSURFACE9 = ObjPointer(IDirect3DSurface9)
PDIRECT3DVOLUME9 = ObjPointer(IDirect3DVolume9)
PDIRECT3DQUERY9 = ObjPointer(IDirect3DQuery9)
PDIRECT3D9EX = ObjPointer(IDirect3D9Ex)
PDIRECT3DDEVICE9EX = ObjPointer(IDirect3DDevice9Ex)
PDIRECT3DSWAPCHAIN9EX = ObjPointer(IDirect3DSwapChain9Ex)

IDirect3D9.methods += [
    StdMethod(HRESULT, "RegisterSoftwareDevice", [(OpaquePointer(Void), "pInitializeFunction")], sideeffects=False),
    StdMethod(UINT, "GetAdapterCount", [], sideeffects=False),
    StdMethod(HRESULT, "GetAdapterIdentifier", [(D3DADAPTER, "Adapter"), (D3DENUM, "Flags"), Out(Pointer(D3DADAPTER_IDENTIFIER9), "pIdentifier")], sideeffects=False),
    StdMethod(UINT, "GetAdapterModeCount", [(D3DADAPTER, "Adapter"), (D3DFORMAT, "Format")], sideeffects=False),
    StdMethod(HRESULT, "EnumAdapterModes", [(D3DADAPTER, "Adapter"), (D3DFORMAT, "Format"), (UINT, "Mode"), Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "GetAdapterDisplayMode", [(D3DADAPTER, "Adapter"), Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceType", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DevType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "BackBufferFormat"), (BOOL, "bWindowed")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceFormat", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DUSAGE, "Usage"), (D3DRESOURCETYPE, "RType"), (D3DFORMAT, "CheckFormat")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceMultiSampleType", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SurfaceFormat"), (BOOL, "Windowed"), (D3DMULTISAMPLE_TYPE, "MultiSampleType"), Out(Pointer(DWORD), "pQualityLevels")], sideeffects=False),
    StdMethod(HRESULT, "CheckDepthStencilMatch", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "AdapterFormat"), (D3DFORMAT, "RenderTargetFormat"), (D3DFORMAT, "DepthStencilFormat")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceFormatConversion", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (D3DFORMAT, "SourceFormat"), (D3DFORMAT, "TargetFormat")], sideeffects=False),
    StdMethod(HRESULT, "GetDeviceCaps", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), Out(Pointer(D3DCAPS9), "pCaps")], sideeffects=False),
    StdMethod(HMONITOR, "GetAdapterMonitor", [(D3DADAPTER, "Adapter")], sideeffects=False),
    StdMethod(HRESULT, "CreateDevice", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (D3DCREATE, "BehaviorFlags"), InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(PDIRECT3DDEVICE9), "ppReturnedDeviceInterface")]),
]

IDirect3DDevice9.methods += [
    StdMethod(HRESULT, "TestCooperativeLevel", []),
    StdMethod(UINT, "GetAvailableTextureMem", [], sideeffects=False),
    StdMethod(HRESULT, "EvictManagedResources", []),
    StdMethod(HRESULT, "GetDirect3D", [Out(Pointer(PDIRECT3D9), "ppD3D9")]),
    StdMethod(HRESULT, "GetDeviceCaps", [Out(Pointer(D3DCAPS9), "pCaps")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplayMode", [(UINT, "iSwapChain"), Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "GetCreationParameters", [Out(Pointer(D3DDEVICE_CREATION_PARAMETERS), "pParameters")], sideeffects=False),
    StdMethod(HRESULT, "SetCursorProperties", [(UINT, "XHotSpot"), (UINT, "YHotSpot"), (PDIRECT3DSURFACE9, "pCursorBitmap")]),
    StdMethod(Void, "SetCursorPosition", [(Int, "X"), (Int, "Y"), (D3DCURSOR, "Flags")]),
    StdMethod(BOOL, "ShowCursor", [(BOOL, "bShow")]),
    StdMethod(HRESULT, "CreateAdditionalSwapChain", [InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(PDIRECT3DSWAPCHAIN9), "pSwapChain")]),
    StdMethod(HRESULT, "GetSwapChain", [(UINT, "iSwapChain"), Out(Pointer(PDIRECT3DSWAPCHAIN9), "pSwapChain")]),
    StdMethod(UINT, "GetNumberOfSwapChains", [], sideeffects=False),
    StdMethod(HRESULT, "Reset", [InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")]),
    StdMethod(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion")]),
    StdMethod(HRESULT, "GetBackBuffer", [(UINT, "iSwapChain"), (UINT, "iBackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), Out(Pointer(PDIRECT3DSURFACE9), "ppBackBuffer")]),
    StdMethod(HRESULT, "GetRasterStatus", [(UINT, "iSwapChain"), Out(Pointer(D3DRASTER_STATUS), "pRasterStatus")], sideeffects=False),
    StdMethod(HRESULT, "SetDialogBoxMode", [(BOOL, "bEnableDialogs")]),
    StdMethod(Void, "SetGammaRamp", [(UINT, "iSwapChain"), (D3DSGR, "Flags"), (ConstPointer(D3DGAMMARAMP), "pRamp")]),
    StdMethod(Void, "GetGammaRamp", [(UINT, "iSwapChain"), Out(Pointer(D3DGAMMARAMP), "pRamp")], sideeffects=False),
    StdMethod(HRESULT, "CreateTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Levels"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DTEXTURE9), "ppTexture"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "CreateVolumeTexture", [(UINT, "Width"), (UINT, "Height"), (UINT, "Depth"), (UINT, "Levels"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DVOLUMETEXTURE9), "ppVolumeTexture"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "CreateCubeTexture", [(UINT, "EdgeLength"), (UINT, "Levels"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DCUBETEXTURE9), "ppCubeTexture"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "CreateVertexBuffer", [(UINT, "Length"), (D3DUSAGE, "Usage"), (D3DFVF, "FVF"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DVERTEXBUFFER9), "ppVertexBuffer"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "CreateIndexBuffer", [(UINT, "Length"), (D3DUSAGE, "Usage"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DINDEXBUFFER9), "ppIndexBuffer"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "CreateRenderTarget", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Lockable"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "CreateDepthStencilSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Discard"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "UpdateSurface", [(PDIRECT3DSURFACE9, "pSourceSurface"), (ConstPointer(RECT), "pSourceRect"), (PDIRECT3DSURFACE9, "pDestinationSurface"), (ConstPointer(POINT), "pDestPoint")]),
    StdMethod(HRESULT, "UpdateTexture", [(PDIRECT3DBASETEXTURE9, "pSourceTexture"), (PDIRECT3DBASETEXTURE9, "pDestinationTexture")]),
    StdMethod(HRESULT, "GetRenderTargetData", [(PDIRECT3DSURFACE9, "pRenderTarget"), (PDIRECT3DSURFACE9, "pDestSurface")], sideeffects=False),
    StdMethod(HRESULT, "GetFrontBufferData", [(UINT, "iSwapChain"), (PDIRECT3DSURFACE9, "pDestSurface")], sideeffects=False),
    StdMethod(HRESULT, "StretchRect", [(PDIRECT3DSURFACE9, "pSourceSurface"), (ConstPointer(RECT), "pSourceRect"), (PDIRECT3DSURFACE9, "pDestSurface"), (ConstPointer(RECT), "pDestRect"), (D3DTEXTUREFILTERTYPE, "Filter")]),
    StdMethod(HRESULT, "ColorFill", [(PDIRECT3DSURFACE9, "pSurface"), (ConstPointer(RECT), "pRect"), (D3DCOLOR, "color")]),
    StdMethod(HRESULT, "CreateOffscreenPlainSurface", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "SetRenderTarget", [(DWORD, "RenderTargetIndex"), (PDIRECT3DSURFACE9, "pRenderTarget")]),
    StdMethod(HRESULT, "GetRenderTarget", [(DWORD, "RenderTargetIndex"), Out(Pointer(PDIRECT3DSURFACE9), "ppRenderTarget")]),
    StdMethod(HRESULT, "SetDepthStencilSurface", [(PDIRECT3DSURFACE9, "pNewZStencil")]),
    StdMethod(HRESULT, "GetDepthStencilSurface", [Out(Pointer(PDIRECT3DSURFACE9), "ppZStencilSurface")]),
    StdMethod(HRESULT, "BeginScene", []),
    StdMethod(HRESULT, "EndScene", []),
    StdMethod(HRESULT, "Clear", [(DWORD, "Count"), (ConstPointer(D3DRECT), "pRects"), (D3DCLEAR, "Flags"), (D3DCOLOR, "Color"), (Float, "Z"), (DWORD, "Stencil")]),
    StdMethod(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    StdMethod(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "State"), Out(Pointer(D3DMATRIX), "pMatrix")], sideeffects=False),
    StdMethod(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "State"), (ConstPointer(D3DMATRIX), "pMatrix")]),
    StdMethod(HRESULT, "SetViewport", [(ConstPointer(D3DVIEWPORT9), "pViewport")]),
    StdMethod(HRESULT, "GetViewport", [Out(Pointer(D3DVIEWPORT9), "pViewport")], sideeffects=False),
    StdMethod(HRESULT, "SetMaterial", [(ConstPointer(D3DMATERIAL9), "pMaterial")]),
    StdMethod(HRESULT, "GetMaterial", [Out(Pointer(D3DMATERIAL9), "pMaterial")], sideeffects=False),
    StdMethod(HRESULT, "SetLight", [(DWORD, "Index"), (ConstPointer(D3DLIGHT9), "pLight")]),
    StdMethod(HRESULT, "GetLight", [(DWORD, "Index"), Out(Pointer(D3DLIGHT9), "pLight")], sideeffects=False),
    StdMethod(HRESULT, "LightEnable", [(DWORD, "Index"), (BOOL, "Enable")]),
    StdMethod(HRESULT, "GetLightEnable", [(DWORD, "Index"), Out(Pointer(BOOL), "pEnable")], sideeffects=False),
    StdMethod(HRESULT, "SetClipPlane", [(DWORD, "Index"), (ConstPointer(Float), "pPlane")]),
    StdMethod(HRESULT, "GetClipPlane", [(DWORD, "Index"), Out(Pointer(Float), "pPlane")], sideeffects=False),
    StdMethod(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "State"), (D3DRENDERSTATEVALUE, "Value")]),
    StdMethod(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "State"), Out(Pointer(D3DRENDERSTATEVALUE), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "Type"), Out(Pointer(PDIRECT3DSTATEBLOCK9), "ppSB")]),
    StdMethod(HRESULT, "BeginStateBlock", []),
    StdMethod(HRESULT, "EndStateBlock", [Out(Pointer(PDIRECT3DSTATEBLOCK9), "ppSB")]),
    StdMethod(HRESULT, "SetClipStatus", [(ConstPointer(D3DCLIPSTATUS9), "pClipStatus")]),
    StdMethod(HRESULT, "GetClipStatus", [Out(Pointer(D3DCLIPSTATUS9), "pClipStatus")], sideeffects=False),
    StdMethod(HRESULT, "GetTexture", [(DWORD, "Stage"), Out(Pointer(PDIRECT3DBASETEXTURE9), "ppTexture")]),
    StdMethod(HRESULT, "SetTexture", [(DWORD, "Stage"), (PDIRECT3DBASETEXTURE9, "pTexture")]),
    StdMethod(HRESULT, "GetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), Out(Pointer(D3DTEXTURESTAGESTATEVALUE), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "SetTextureStageState", [(DWORD, "Stage"), (D3DTEXTURESTAGESTATETYPE, "Type"), (D3DTEXTURESTAGESTATEVALUE, "Value")]),
    StdMethod(HRESULT, "GetSamplerState", [(DWORD, "Sampler"), (D3DSAMPLERSTATETYPE, "Type"), Out(Pointer(D3DSAMPLERSTATEVALUE), "pValue")], sideeffects=False),
    StdMethod(HRESULT, "SetSamplerState", [(DWORD, "Sampler"), (D3DSAMPLERSTATETYPE, "Type"), (D3DSAMPLERSTATEVALUE, "Value")]),
    StdMethod(HRESULT, "ValidateDevice", [Out(Pointer(DWORD), "pNumPasses")]),
    StdMethod(HRESULT, "SetPaletteEntries", [(UINT, "PaletteNumber"), (ConstPointer(PALETTEENTRY), "pEntries")]),
    StdMethod(HRESULT, "GetPaletteEntries", [(UINT, "PaletteNumber"), Out(Pointer(PALETTEENTRY), "pEntries")], sideeffects=False),
    StdMethod(HRESULT, "SetCurrentTexturePalette", [(UINT, "PaletteNumber")]),
    StdMethod(HRESULT, "GetCurrentTexturePalette", [Out(Pointer(UINT), "PaletteNumber")], sideeffects=False),
    StdMethod(HRESULT, "SetScissorRect", [(ConstPointer(RECT), "pRect")]),
    StdMethod(HRESULT, "GetScissorRect", [Out(Pointer(RECT), "pRect")]),
    StdMethod(HRESULT, "SetSoftwareVertexProcessing", [(BOOL, "bSoftware")]),
    StdMethod(BOOL, "GetSoftwareVertexProcessing", [], sideeffects=False),
    StdMethod(HRESULT, "SetNPatchMode", [(Float, "nSegments")]),
    StdMethod(Float, "GetNPatchMode", [], sideeffects=False),
    StdMethod(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "StartVertex"), (UINT, "PrimitiveCount")]),
    StdMethod(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "PrimitiveType"), (INT, "BaseVertexIndex"), (UINT, "MinVertexIndex"), (UINT, "NumVertices"), (UINT, "startIndex"), (UINT, "primCount")]),
    StdMethod(HRESULT, "DrawPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "PrimitiveCount"), (Blob(Const(Void), "_vertexDataSize(PrimitiveType, PrimitiveCount, VertexStreamZeroStride)"), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    StdMethod(HRESULT, "DrawIndexedPrimitiveUP", [(D3DPRIMITIVETYPE, "PrimitiveType"), (UINT, "MinVertexIndex"), (UINT, "NumVertices"), (UINT, "PrimitiveCount"), (Blob(Const(Void), "_indexDataSize(PrimitiveType, PrimitiveCount, IndexDataFormat)"), "pIndexData"), (D3DFORMAT, "IndexDataFormat"), (Blob(Const(Void), "NumVertices*VertexStreamZeroStride"), "pVertexStreamZeroData"), (UINT, "VertexStreamZeroStride")]),
    StdMethod(HRESULT, "ProcessVertices", [(UINT, "SrcStartIndex"), (UINT, "DestIndex"), (UINT, "VertexCount"), (PDIRECT3DVERTEXBUFFER9, "pDestBuffer"), (PDIRECT3DVERTEXDECLARATION9, "pVertexDecl"), (D3DPV, "Flags")]),
    StdMethod(HRESULT, "CreateVertexDeclaration", [(Array(Const(D3DVERTEXELEMENT9), "_declCount(pVertexElements)"), "pVertexElements"), Out(Pointer(PDIRECT3DVERTEXDECLARATION9), "ppDecl")]),
    StdMethod(HRESULT, "SetVertexDeclaration", [(PDIRECT3DVERTEXDECLARATION9, "pDecl")]),
    StdMethod(HRESULT, "GetVertexDeclaration", [Out(Pointer(PDIRECT3DVERTEXDECLARATION9), "ppDecl")]),
    StdMethod(HRESULT, "SetFVF", [(D3DFVF, "FVF")]),
    StdMethod(HRESULT, "GetFVF", [Out(Pointer(D3DFVF), "pFVF")], sideeffects=False),
    StdMethod(HRESULT, "CreateVertexShader", [(D3DSHADER9, "pFunction"), Out(Pointer(PDIRECT3DVERTEXSHADER9), "ppShader")]),
    StdMethod(HRESULT, "SetVertexShader", [(PDIRECT3DVERTEXSHADER9, "pShader")]),
    StdMethod(HRESULT, "GetVertexShader", [Out(Pointer(PDIRECT3DVERTEXSHADER9), "ppShader")]),
    StdMethod(HRESULT, "SetVertexShaderConstantF", [(UINT, "StartRegister"), (Array(Const(Float), "4*Vector4fCount"), "pConstantData"), (UINT, "Vector4fCount")]),
    StdMethod(HRESULT, "GetVertexShaderConstantF", [(UINT, "StartRegister"), Out(Array(Float, "4*Vector4fCount"), "pConstantData"), (UINT, "Vector4fCount")], sideeffects=False),
    StdMethod(HRESULT, "SetVertexShaderConstantI", [(UINT, "StartRegister"), (Array(Const(Int), "4*Vector4iCount"), "pConstantData"), (UINT, "Vector4iCount")]),
    StdMethod(HRESULT, "GetVertexShaderConstantI", [(UINT, "StartRegister"), Out(Array(Int, "4*Vector4iCount"), "pConstantData"), (UINT, "Vector4iCount")], sideeffects=False),
    StdMethod(HRESULT, "SetVertexShaderConstantB", [(UINT, "StartRegister"), (Array(Const(BOOL), "BoolCount"), "pConstantData"), (UINT, "BoolCount")]),
    StdMethod(HRESULT, "GetVertexShaderConstantB", [(UINT, "StartRegister"), Out(Array(BOOL, "BoolCount"), "pConstantData"), (UINT, "BoolCount")], sideeffects=False),
    StdMethod(HRESULT, "SetStreamSource", [(UINT, "StreamNumber"), (PDIRECT3DVERTEXBUFFER9, "pStreamData"), (UINT, "OffsetInBytes"), (UINT, "Stride")]),
    StdMethod(HRESULT, "GetStreamSource", [(UINT, "StreamNumber"), Out(Pointer(PDIRECT3DVERTEXBUFFER9), "ppStreamData"), Out(Pointer(UINT), "pOffsetInBytes"), Out(Pointer(UINT), "pStride")]),
    StdMethod(HRESULT, "SetStreamSourceFreq", [(UINT, "StreamNumber"), (UINT, "Setting")]),
    StdMethod(HRESULT, "GetStreamSourceFreq", [(UINT, "StreamNumber"), Out(Pointer(UINT), "pSetting")], sideeffects=False),
    StdMethod(HRESULT, "SetIndices", [(PDIRECT3DINDEXBUFFER9, "pIndexData")]),
    StdMethod(HRESULT, "GetIndices", [Out(Pointer(PDIRECT3DINDEXBUFFER9), "ppIndexData")]),
    StdMethod(HRESULT, "CreatePixelShader", [(D3DSHADER9, "pFunction"), Out(Pointer(PDIRECT3DPIXELSHADER9), "ppShader")]),
    StdMethod(HRESULT, "SetPixelShader", [(PDIRECT3DPIXELSHADER9, "pShader")]),
    StdMethod(HRESULT, "GetPixelShader", [Out(Pointer(PDIRECT3DPIXELSHADER9), "ppShader")]),
    StdMethod(HRESULT, "SetPixelShaderConstantF", [(UINT, "StartRegister"), (Array(Const(Float), "4*Vector4fCount"), "pConstantData"), (UINT, "Vector4fCount")]),
    StdMethod(HRESULT, "GetPixelShaderConstantF", [(UINT, "StartRegister"), Out(Array(Float, "4*Vector4fCount"), "pConstantData"), (UINT, "Vector4fCount")], sideeffects=False),
    StdMethod(HRESULT, "SetPixelShaderConstantI", [(UINT, "StartRegister"), (Array(Const(Int), "4*Vector4iCount"), "pConstantData"), (UINT, "Vector4iCount")]),
    StdMethod(HRESULT, "GetPixelShaderConstantI", [(UINT, "StartRegister"), Out(Array(Int, "4*Vector4iCount"), "pConstantData"), (UINT, "Vector4iCount")], sideeffects=False),
    StdMethod(HRESULT, "SetPixelShaderConstantB", [(UINT, "StartRegister"), (Array(Const(BOOL), "BoolCount"), "pConstantData"), (UINT, "BoolCount")]),
    StdMethod(HRESULT, "GetPixelShaderConstantB", [(UINT, "StartRegister"), Out(Array(BOOL, "BoolCount"), "pConstantData"), (UINT, "BoolCount")], sideeffects=False),
    StdMethod(HRESULT, "DrawRectPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DRECTPATCH_INFO), "pRectPatchInfo")]),
    StdMethod(HRESULT, "DrawTriPatch", [(UINT, "Handle"), (ConstPointer(Float), "pNumSegs"), (ConstPointer(D3DTRIPATCH_INFO), "pTriPatchInfo")]),
    StdMethod(HRESULT, "DeletePatch", [(UINT, "Handle")]),
    StdMethod(HRESULT, "CreateQuery", [(D3DQUERYTYPE, "Type"), Out(Pointer(PDIRECT3DQUERY9), "ppQuery")]),
]

IDirect3DStateBlock9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "Capture", []),
    StdMethod(HRESULT, "Apply", []),
]

IDirect3DSwapChain9.methods += [
    StdMethod(HRESULT, "Present", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion"), (D3DPRESENT, "dwFlags")]),
    StdMethod(HRESULT, "GetFrontBufferData", [(PDIRECT3DSURFACE9, "pDestSurface")], sideeffects=False),
    StdMethod(HRESULT, "GetBackBuffer", [(UINT, "iBackBuffer"), (D3DBACKBUFFER_TYPE, "Type"), Out(Pointer(PDIRECT3DSURFACE9), "ppBackBuffer")]),
    StdMethod(HRESULT, "GetRasterStatus", [Out(Pointer(D3DRASTER_STATUS), "pRasterStatus")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplayMode", [Out(Pointer(D3DDISPLAYMODE), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "GetPresentParameters", [Out(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters")], sideeffects=False),
]

IDirect3DResource9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaqueBlob(Const(Void), "SizeOfData"), "pData"), (DWORD, "SizeOfData"), (D3DSPD, "Flags")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "FreePrivateData", [(REFGUID, "refguid")], sideeffects=False),
    StdMethod(D3D9_RESOURCE_PRIORITY, "SetPriority", [(D3D9_RESOURCE_PRIORITY, "PriorityNew")]),
    StdMethod(D3D9_RESOURCE_PRIORITY, "GetPriority", [], sideeffects=False),
    StdMethod(Void, "PreLoad", []),
    StdMethod(D3DRESOURCETYPE, "GetType", [], sideeffects=False),
]

IDirect3DVertexDeclaration9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "GetDeclaration", [Out(Array(D3DVERTEXELEMENT9, "*pNumElements"), "pElement"), Out(Pointer(UINT), "pNumElements")], sideeffects=False),
]

IDirect3DVertexShader9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "GetFunction", [Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(UINT), "pSizeOfData")], sideeffects=False),
]

IDirect3DPixelShader9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "GetFunction", [Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(UINT), "pSizeOfData")], sideeffects=False),
]

IDirect3DBaseTexture9.methods += [
    StdMethod(DWORD, "SetLOD", [(DWORD, "LODNew")]),
    StdMethod(DWORD, "GetLOD", [], sideeffects=False),
    StdMethod(DWORD, "GetLevelCount", [], sideeffects=False),
    StdMethod(HRESULT, "SetAutoGenFilterType", [(D3DTEXTUREFILTERTYPE, "FilterType")]),
    StdMethod(D3DTEXTUREFILTERTYPE, "GetAutoGenFilterType", [], sideeffects=False),
    StdMethod(Void, "GenerateMipSubLevels", []),
]

IDirect3DTexture9.methods += [
    StdMethod(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DSURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetSurfaceLevel", [(UINT, "Level"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurfaceLevel")]),
    StdMethod(HRESULT, "LockRect", [(UINT, "Level"), Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockRect", [(UINT, "Level")]),
    StdMethod(HRESULT, "AddDirtyRect", [(ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVolumeTexture9.methods += [
    StdMethod(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DVOLUME_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetVolumeLevel", [(UINT, "Level"), Out(Pointer(PDIRECT3DVOLUME9), "ppVolumeLevel")]),
    StdMethod(HRESULT, "LockBox", [(UINT, "Level"), Out(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockBox", [(UINT, "Level")]),
    StdMethod(HRESULT, "AddDirtyBox", [(ConstPointer(D3DBOX), "pDirtyBox")]),
]

IDirect3DCubeTexture9.methods += [
    StdMethod(HRESULT, "GetLevelDesc", [(UINT, "Level"), Out(Pointer(D3DSURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "GetCubeMapSurface", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), Out(Pointer(PDIRECT3DSURFACE9), "ppCubeMapSurface")]),
    StdMethod(HRESULT, "LockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level"), Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockRect", [(D3DCUBEMAP_FACES, "FaceType"), (UINT, "Level")]),
    StdMethod(HRESULT, "AddDirtyRect", [(D3DCUBEMAP_FACES, "FaceType"), (ConstPointer(RECT), "pDirtyRect")]),
]

IDirect3DVertexBuffer9.methods += [
    StdMethod(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), Out(Pointer(LinearPointer(Void, "_MappedSize")), "ppbData"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DVERTEXBUFFER_DESC), "pDesc")], sideeffects=False),
]

IDirect3DIndexBuffer9.methods += [
    StdMethod(HRESULT, "Lock", [(UINT, "OffsetToLock"), (UINT, "SizeToLock"), Out(Pointer(LinearPointer(Void, "_MappedSize")), "ppbData"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DINDEXBUFFER_DESC), "pDesc")], sideeffects=False),
]

IDirect3DSurface9.methods += [
    StdMethod(HRESULT, "GetContainer", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppContainer")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DSURFACE_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "LockRect", [Out(Pointer(D3DLOCKED_RECT), "pLockedRect"), (ConstPointer(RECT), "pRect"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockRect", []),
    StdMethod(HRESULT, "GetDC", [Out(Pointer(HDC), "phdc")]),
    StdMethod(HRESULT, "ReleaseDC", [(HDC, "hdc")]),
]

IDirect3DVolume9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(HRESULT, "SetPrivateData", [(REFGUID, "refguid"), (OpaqueBlob(Const(Void), "SizeOfData"), "pData"), (DWORD, "SizeOfData"), (D3DSPD, "Flags")], sideeffects=False),
    StdMethod(HRESULT, "GetPrivateData", [(REFGUID, "refguid"), Out(OpaqueBlob(Void, "*pSizeOfData"), "pData"), Out(Pointer(DWORD), "pSizeOfData")], sideeffects=False),
    StdMethod(HRESULT, "FreePrivateData", [(REFGUID, "refguid")], sideeffects=False),
    StdMethod(HRESULT, "GetContainer", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppContainer")]),
    StdMethod(HRESULT, "GetDesc", [Out(Pointer(D3DVOLUME_DESC), "pDesc")], sideeffects=False),
    StdMethod(HRESULT, "LockBox", [Out(Pointer(D3DLOCKED_BOX), "pLockedVolume"), (ConstPointer(D3DBOX), "pBox"), (D3DLOCK, "Flags")]),
    StdMethod(HRESULT, "UnlockBox", []),
]

IDirect3DQuery9.methods += [
    StdMethod(HRESULT, "GetDevice", [Out(Pointer(PDIRECT3DDEVICE9), "ppDevice")]),
    StdMethod(D3DQUERYTYPE, "GetType", [], sideeffects=False),
    StdMethod(DWORD, "GetDataSize", [], sideeffects=False),
    StdMethod(HRESULT, "Issue", [(D3DISSUE, "dwIssueFlags")]),
    StdMethod(HRESULT, "GetData", [Out(Blob(Void, "dwSize"), "pData"), (DWORD, "dwSize"), (D3DGETDATA, "dwGetDataFlags")], sideeffects=False),
]

IDirect3D9Ex.methods += [
    StdMethod(UINT, "GetAdapterModeCountEx", [(D3DADAPTER, "Adapter"), (ConstPointer(D3DDISPLAYMODEFILTER), "pFilter") ], sideeffects=False),
    StdMethod(HRESULT, "EnumAdapterModesEx", [(D3DADAPTER, "Adapter"), (ConstPointer(D3DDISPLAYMODEFILTER), "pFilter"), (UINT, "Mode"), Out(Pointer(D3DDISPLAYMODEEX), "pMode")], sideeffects=False),
    StdMethod(HRESULT, "GetAdapterDisplayModeEx", [(D3DADAPTER, "Adapter"), Out(Pointer(D3DDISPLAYMODEEX), "pMode"), Out(Pointer(D3DDISPLAYROTATION), "pRotation")], sideeffects=False),
    StdMethod(HRESULT, "CreateDeviceEx", [(D3DADAPTER, "Adapter"), (D3DDEVTYPE, "DeviceType"), (HWND, "hFocusWindow"), (D3DCREATE, "BehaviorFlags"), InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(D3DDISPLAYMODEEX), "pFullscreenDisplayMode"), Out(Pointer(PDIRECT3DDEVICE9EX), "ppReturnedDeviceInterface")]),
    StdMethod(HRESULT, "GetAdapterLUID", [(D3DADAPTER, "Adapter"), Out(Pointer(LUID), "pLUID")], sideeffects=False),
]

IDirect3DDevice9Ex.methods += [
    StdMethod(HRESULT, "SetConvolutionMonoKernel", [(UINT, "width"), (UINT, "height"), (Array(Float, "width"), "rows"), (Array(Float, "height"), "columns")]),
    StdMethod(HRESULT, "ComposeRects", [(PDIRECT3DSURFACE9, "pSrc"), (PDIRECT3DSURFACE9, "pDst"), (PDIRECT3DVERTEXBUFFER9, "pSrcRectDescs"), (UINT, "NumRects"), (PDIRECT3DVERTEXBUFFER9, "pDstRectDescs"), (D3DCOMPOSERECTSOP, "Operation"), (Int, "Xoffset"), (Int, "Yoffset")]),
    StdMethod(HRESULT, "PresentEx", [(ConstPointer(RECT), "pSourceRect"), (ConstPointer(RECT), "pDestRect"), (HWND, "hDestWindowOverride"), (ConstPointer(RGNDATA), "pDirtyRegion"), (D3DPRESENT, "dwFlags")]),
    StdMethod(HRESULT, "GetGPUThreadPriority", [Out(Pointer(INT), "pPriority")], sideeffects=False),
    StdMethod(HRESULT, "SetGPUThreadPriority", [(INT, "Priority")]),
    StdMethod(HRESULT, "WaitForVBlank", [(UINT, "iSwapChain")]),
    StdMethod(HRESULT, "CheckResourceResidency", [(Array(PDIRECT3DRESOURCE9, "NumResources"), "pResourceArray"), (UINT32, "NumResources")]),
    StdMethod(HRESULT, "SetMaximumFrameLatency", [(UINT, "MaxLatency")]),
    StdMethod(HRESULT, "GetMaximumFrameLatency", [Out(Pointer(UINT), "pMaxLatency")], sideeffects=False),
    StdMethod(HRESULT, "CheckDeviceState", [(HWND, "hDestinationWindow")], sideeffects=False),
    StdMethod(HRESULT, "CreateRenderTargetEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Lockable"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurface"), (Pointer(HANDLE), "pSharedHandle"), (D3DUSAGE, "Usage")]),
    StdMethod(HRESULT, "CreateOffscreenPlainSurfaceEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurface"), Out(Pointer(HANDLE), "pSharedHandle"), (D3DUSAGE, "Usage")]),
    StdMethod(HRESULT, "CreateDepthStencilSurfaceEx", [(UINT, "Width"), (UINT, "Height"), (D3DFORMAT, "Format"), (D3DMULTISAMPLE_TYPE, "MultiSample"), (DWORD, "MultisampleQuality"), (BOOL, "Discard"), Out(Pointer(PDIRECT3DSURFACE9), "ppSurface"), (Pointer(HANDLE), "pSharedHandle"), (D3DUSAGE, "Usage")]),
    StdMethod(HRESULT, "ResetEx", [InOut(Pointer(D3DPRESENT_PARAMETERS), "pPresentationParameters"), Out(Pointer(D3DDISPLAYMODEEX), "pFullscreenDisplayMode")]),
    StdMethod(HRESULT, "GetDisplayModeEx", [(UINT, "iSwapChain"), Out(Pointer(D3DDISPLAYMODEEX), "pMode"), Out(Pointer(D3DDISPLAYROTATION), "pRotation")], sideeffects=False),
]

IDirect3DSwapChain9Ex.methods += [
    StdMethod(HRESULT, "GetLastPresentCount", [Out(Pointer(UINT), "pLastPresentCount")], sideeffects=False),
    StdMethod(HRESULT, "GetPresentStats", [Out(Pointer(D3DPRESENTSTATS), "pPresentationStatistics")], sideeffects=False),
    StdMethod(HRESULT, "GetDisplayModeEx", [Out(Pointer(D3DDISPLAYMODEEX), "pMode"), Out(Pointer(D3DDISPLAYROTATION), "pRotation")], sideeffects=False),
]

d3d9 = Module("d3d9")
d3d9.addFunctions([
    StdFunction(PDIRECT3D9, "Direct3DCreate9", [(UINT, "SDKVersion")], fail='NULL'),
    StdFunction(HRESULT, "Direct3DCreate9Ex", [(UINT, "SDKVersion"), Out(Pointer(PDIRECT3D9EX), "ppD3D")], fail='D3DERR_NOTAVAILABLE'),
    StdFunction(Int, "D3DPERF_BeginEvent", [(D3DCOLOR, "col"), (LPCWSTR, "wszName")], fail='-1', sideeffects=False),
    StdFunction(Int, "D3DPERF_EndEvent", [], fail='-1', sideeffects=False),
    StdFunction(Void, "D3DPERF_SetMarker", [(D3DCOLOR, "col"), (LPCWSTR, "wszName")], sideeffects=False),
    StdFunction(Void, "D3DPERF_SetRegion", [(D3DCOLOR, "col"), (LPCWSTR, "wszName")], sideeffects=False),
    StdFunction(BOOL, "D3DPERF_QueryRepeatFrame", [], fail='FALSE', sideeffects=False),
    StdFunction(Void, "D3DPERF_SetOptions", [(DWORD, "dwOptions")], sideeffects=False),
    StdFunction(DWORD, "D3DPERF_GetStatus", [], fail='0', sideeffects=False),
])
d3d9.addInterfaces([
    IDirect3DSwapChain9Ex,
])

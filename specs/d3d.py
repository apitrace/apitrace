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

"""d3d.h"""

from winapi import *
from ddraw import *
from d3dtypes import *
from d3dcaps import *

def OutPointer(type):
    return Out(Pointer(type), "out")

d3dnextFlags = Flags(DWORD, [
    "D3DNEXT_NEXT",
    "D3DNEXT_HEAD",
    "D3DNEXT_TAIL",
])

direct3dFlags = Flags(DWORD, [
    "DIRECT3D_VERSION",
])

d3ddpFlags = Flags(DWORD, [
    "D3DDP_WAIT",
    "D3DDP_OUTOFORDER",
    "D3DDP_DONOTCLIP",
    "D3DDP_DONOTUPDATEEXTENTS",
    "D3DDP_DONOTLIGHT",
])

HRESULT = Enum("HRESULT", [
    "D3D_OK",
    "D3DERR_BADMAJORVERSION",
    "D3DERR_BADMINORVERSION",
    "D3DERR_INVALID_DEVICE",
    "D3DERR_INITFAILED",
    "D3DERR_DEVICEAGGREGATED",
    "D3DERR_EXECUTE_CREATE_FAILED",
    "D3DERR_EXECUTE_DESTROY_FAILED",
    "D3DERR_EXECUTE_LOCK_FAILED",
    "D3DERR_EXECUTE_UNLOCK_FAILED",
    "D3DERR_EXECUTE_LOCKED",
    "D3DERR_EXECUTE_NOT_LOCKED",
    "D3DERR_EXECUTE_FAILED",
    "D3DERR_EXECUTE_CLIPPED_FAILED",
    "D3DERR_TEXTURE_NO_SUPPORT",
    "D3DERR_TEXTURE_CREATE_FAILED",
    "D3DERR_TEXTURE_DESTROY_FAILED",
    "D3DERR_TEXTURE_LOCK_FAILED",
    "D3DERR_TEXTURE_UNLOCK_FAILED",
    "D3DERR_TEXTURE_LOAD_FAILED",
    "D3DERR_TEXTURE_SWAP_FAILED",
    "D3DERR_TEXTURE_LOCKED",
    "D3DERR_TEXTURE_NOT_LOCKED",
    "D3DERR_TEXTURE_GETSURF_FAILED",
    "D3DERR_MATRIX_CREATE_FAILED",
    "D3DERR_MATRIX_DESTROY_FAILED",
    "D3DERR_MATRIX_SETDATA_FAILED",
    "D3DERR_MATRIX_GETDATA_FAILED",
    "D3DERR_SETVIEWPORTDATA_FAILED",
    "D3DERR_INVALIDCURRENTVIEWPORT",
    "D3DERR_INVALIDPRIMITIVETYPE",
    "D3DERR_INVALIDVERTEXTYPE",
    "D3DERR_TEXTURE_BADSIZE",
    "D3DERR_INVALIDRAMPTEXTURE",
    "D3DERR_MATERIAL_CREATE_FAILED",
    "D3DERR_MATERIAL_DESTROY_FAILED",
    "D3DERR_MATERIAL_SETDATA_FAILED",
    "D3DERR_MATERIAL_GETDATA_FAILED",
    "D3DERR_INVALIDPALETTE",
    "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY",
    "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY",
    "D3DERR_SURFACENOTINVIDMEM",
    "D3DERR_LIGHT_SET_FAILED",
    "D3DERR_LIGHTHASVIEWPORT",
    "D3DERR_LIGHTNOTINTHISVIEWPORT",
    "D3DERR_SCENE_IN_SCENE",
    "D3DERR_SCENE_NOT_IN_SCENE",
    "D3DERR_SCENE_BEGIN_FAILED",
    "D3DERR_SCENE_END_FAILED",
    "D3DERR_INBEGIN",
    "D3DERR_NOTINBEGIN",
    "D3DERR_NOVIEWPORTS",
    "D3DERR_VIEWPORTDATANOTSET",
    "D3DERR_VIEWPORTHASNODEVICE",
    "D3DERR_NOCURRENTVIEWPORT",
    "D3DERR_INVALIDVERTEXFORMAT",
    "D3DERR_COLORKEYATTACHED",
    "D3DERR_VERTEXBUFFEROPTIMIZED",
    "D3DERR_VBUF_CREATE_FAILED",
    "D3DERR_VERTEXBUFFERLOCKED",
    "D3DERR_VERTEXBUFFERUNLOCKFAILED",
    "D3DERR_ZBUFFER_NOTPRESENT",
    "D3DERR_STENCILBUFFER_NOTPRESENT",
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
    "D3DERR_TOOMANYPRIMITIVES",
    "D3DERR_INVALIDMATRIX",
    "D3DERR_TOOMANYVERTICES",
    "D3DERR_CONFLICTINGTEXTUREPALETTE",
    "D3DERR_INVALIDSTATEBLOCK",
    "D3DERR_INBEGINSTATEBLOCK",
    "D3DERR_NOTINBEGINSTATEBLOCK",
])

IDirect3D = Interface("IDirect3D", IUnknown)
IDirect3D2 = Interface("IDirect3D2", IUnknown)
IDirect3D3 = Interface("IDirect3D3", IUnknown)
IDirect3D7 = Interface("IDirect3D7", IUnknown)
IDirect3DDevice = Interface("IDirect3DDevice", IUnknown)
IDirect3DDevice2 = Interface("IDirect3DDevice2", IUnknown)
IDirect3DDevice3 = Interface("IDirect3DDevice3", IUnknown)
IDirect3DDevice7 = Interface("IDirect3DDevice7", IUnknown)
IDirect3DExecuteBuffer = Interface("IDirect3DExecuteBuffer", IUnknown)
IDirect3DLight = Interface("IDirect3DLight", IUnknown)
IDirect3DMaterial = Interface("IDirect3DMaterial", IUnknown)
IDirect3DMaterial2 = Interface("IDirect3DMaterial2", IUnknown)
IDirect3DMaterial3 = Interface("IDirect3DMaterial3", IUnknown)
IDirect3DTexture = Interface("IDirect3DTexture", IUnknown)
IDirect3DTexture2 = Interface("IDirect3DTexture2", IUnknown)
IDirect3DViewport = Interface("IDirect3DViewport", IUnknown)
IDirect3DViewport2 = Interface("IDirect3DViewport2", IDirect3DViewport)
IDirect3DViewport3 = Interface("IDirect3DViewport3", IDirect3DViewport2)
IDirect3DVertexBuffer = Interface("IDirect3DVertexBuffer", IUnknown)
IDirect3DVertexBuffer7 = Interface("IDirect3DVertexBuffer7", IUnknown)

LPUNKNOWN = Pointer(IUnknown)

LPDIRECT3D = Pointer(IDirect3D)
LPDIRECT3DDEVICE = Pointer(IDirect3DDevice)
LPDIRECT3DEXECUTEBUFFER = Pointer(IDirect3DExecuteBuffer)
LPDIRECT3DLIGHT = Pointer(IDirect3DLight)
LPDIRECT3DMATERIAL = Pointer(IDirect3DMaterial)
LPDIRECT3DTEXTURE = Pointer(IDirect3DTexture)
LPDIRECT3DVIEWPORT = Pointer(IDirect3DViewport)

LPDIRECT3D2 = Pointer(IDirect3D2)
LPDIRECT3DDEVICE2 = Pointer(IDirect3DDevice2)
LPDIRECT3DMATERIAL2 = Pointer(IDirect3DMaterial2)
LPDIRECT3DTEXTURE2 = Pointer(IDirect3DTexture2)
LPDIRECT3DVIEWPORT2 = Pointer(IDirect3DViewport2)

LPDIRECT3D3 = Pointer(IDirect3D3)
LPDIRECT3DDEVICE3 = Pointer(IDirect3DDevice3)
LPDIRECT3DMATERIAL3 = Pointer(IDirect3DMaterial3)
LPDIRECT3DVIEWPORT3 = Pointer(IDirect3DViewport3)
LPDIRECT3DVERTEXBUFFER = Pointer(IDirect3DVertexBuffer)

LPDIRECT3D7 = Pointer(IDirect3D7)
LPDIRECT3DDEVICE7 = Pointer(IDirect3DDevice7)
LPDIRECT3DVERTEXBUFFER7 = Pointer(IDirect3DVertexBuffer7)

IDirect3D.methods += [
    Method(HRESULT, "Initialize", [(REFCLSID, "riid")]),
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    Method(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL), "lplpDirect3DMaterial"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT), "lplpD3DViewport"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "lpD3DDFS"), (LPD3DFINDDEVICERESULT, "lplpD3DDevice")]),
]

IDirect3D2.methods += [
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    Method(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL2), "lplpDirect3DMaterial2"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT2), "lplpD3DViewport2"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "lpD3DDFS"), (LPD3DFINDDEVICERESULT, "lpD3DFDR")]),
    Method(HRESULT, "CreateDevice", [(REFCLSID, "rclsid"), (LPDIRECTDRAWSURFACE, "lpDDS"), Out(Pointer(LPDIRECT3DDEVICE2), "lplpD3DDevice2")]),
]

IDirect3D3.methods += [
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    Method(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL3), "lplpDirect3DMaterial3"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT3), "lplpD3DViewport3"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "lpD3DDFS"), Out(LPD3DFINDDEVICERESULT, "lpD3DFDR")]),
    Method(HRESULT, "CreateDevice", [(REFCLSID, "rclsid"), (LPDIRECTDRAWSURFACE4, "lpDDS"), Out(Pointer(LPDIRECT3DDEVICE3), "lplpD3DDevice3"), (LPUNKNOWN, "lpUnk")]),
    Method(HRESULT, "CreateVertexBuffer", [(LPD3DVERTEXBUFFERDESC, "lpD3DVertBufDesc"), Out(Pointer(LPDIRECT3DVERTEXBUFFER), "lplpD3DVertBuf"), (DWORD, "dwFlags"), (LPUNKNOWN, "lpUnk")]),
    Method(HRESULT, "EnumZBufferFormats", [(REFCLSID, "riidDevice"), (LPD3DENUMPIXELFORMATSCALLBACK, "lpEnumCallback"), (LPVOID, "lpContext")]),
    Method(HRESULT, "EvictManagedTextures", []),
]

IDirect3D7.methods += [
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK7, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    Method(HRESULT, "CreateDevice", [(REFCLSID, "rclsid"), (LPDIRECTDRAWSURFACE7, "lpDDS"), Out(Pointer(LPDIRECT3DDEVICE7), "lplpD3DDevice")]),
    Method(HRESULT, "CreateVertexBuffer", [(LPD3DVERTEXBUFFERDESC, "lpD3DVertBufDesc"), Out(Pointer(LPDIRECT3DVERTEXBUFFER7), "lplpD3DVertBuf"), (DWORD, "dwFlags")]),
    Method(HRESULT, "EnumZBufferFormats", [(REFCLSID, "riidDevice"), (LPD3DENUMPIXELFORMATSCALLBACK, "lpEnumCallback"), (LPVOID, "lpContext")]),
    Method(HRESULT, "EvictManagedTextures", []),
]

IDirect3DDevice.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D"), (LPGUID, "lpGUID"), (LPD3DDEVICEDESC, "lpD3DDVDesc")]),
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "lpD3DHWDevDesc"), Out(LPD3DDEVICEDESC, "lpD3DHELDevDesc")]),
    Method(HRESULT, "SwapTextureHandles", [(LPDIRECT3DTEXTURE, "lpD3Dtex1"), (LPDIRECT3DTEXTURE, "lpD3DTex2")]),
    Method(HRESULT, "CreateExecuteBuffer", [(LPD3DEXECUTEBUFFERDESC, "lpDesc"), Out(Pointer(LPDIRECT3DEXECUTEBUFFER), "lplpDirect3DExecuteBuffer"), (LPUNKNOWN, "pUnkOuter")]),
    Method(HRESULT, "GetStats", [(LPD3DSTATS, "lpD3DStats")]),
    Method(HRESULT, "Execute", [(LPDIRECT3DEXECUTEBUFFER, "lpDirect3DExecuteBuffer"), (LPDIRECT3DVIEWPORT, "lpDirect3DViewport"), (DWORD, "dwFlags")]),
    Method(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT, "lpDirect3DViewport")]),
    Method(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT, "lpDirect3DViewport")]),
    Method(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT, "lpDirect3DViewport"), Out(Pointer(LPDIRECT3DVIEWPORT), "lplpDirect3DViewport"), (DWORD, "dwFlags")]),
    Method(HRESULT, "Pick", [(LPDIRECT3DEXECUTEBUFFER, "lpDirect3DExecuteBuffer"), (LPDIRECT3DVIEWPORT, "lpDirect3DViewport"), (DWORD, "dwFlags"), (LPD3DRECT, "lpRect")]),
    Method(HRESULT, "GetPickRecords", [(LPDWORD, "lpCount"), (LPD3DPICKRECORD, "lpD3DPickRec")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMTEXTUREFORMATSCALLBACK, "lpD3DEnumTextureProc"), (LPVOID, "lpArg")]),
    Method(HRESULT, "CreateMatrix", [Out(LPD3DMATRIXHANDLE, "lpD3DMatHandle")]),
    Method(HRESULT, "SetMatrix", [(D3DMATRIXHANDLE, "D3DMatHandle"), (Const(LPD3DMATRIX), "lpD3DMatrix")]),
    Method(HRESULT, "GetMatrix", [(D3DMATRIXHANDLE, "D3DMatHandle"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "DeleteMatrix", [(D3DMATRIXHANDLE, "D3DMatHandle")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D), "lplpDirect3D")]),
]

IDirect3DDevice2.methods += [
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "lpD3DHWDevDesc"), Out(LPD3DDEVICEDESC, "lpD3DHELDevDesc")]),
    Method(HRESULT, "SwapTextureHandles", [(LPDIRECT3DTEXTURE2, "lpD3DTex1"), (LPDIRECT3DTEXTURE2, "lpD3DTex2")]),
    Method(HRESULT, "GetStats", [Out(LPD3DSTATS, "lpD3DStats")]),
    Method(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2")]),
    Method(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2")]),
    Method(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2"), Out(Pointer(LPDIRECT3DVIEWPORT2), "lplpDirect3DViewport2"), (DWORD, "dwFlags")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMTEXTUREFORMATSCALLBACK, "lpD3DEnumTextureProc"), (LPVOID, "lpArg")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D2), "lplpDirect3D2")]),
    Method(HRESULT, "SetCurrentViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2")]),
    Method(HRESULT, "GetCurrentViewport", [Out(Pointer(LPDIRECT3DVIEWPORT2), "lplpDirect3DViewport2")]),
    Method(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE, "lpNewRenderTarget"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE), "lplpRenderTarget")]),
    Method(HRESULT, "Begin", [(D3DPRIMITIVETYPE, "d3dpt"), (D3DVERTEXTYPE, "dwVertexTypeDesc"), (DWORD, "dwFlags")]),
    Method(HRESULT, "BeginIndexed", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DVERTEXTYPE, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    Method(HRESULT, "Vertex", [(LPVOID, "lpVertexType")]),
    Method(HRESULT, "Index", [(WORD, "wVertexIndex")]),
    Method(HRESULT, "End", [(DWORD, "dwFlags")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), Out(LPDWORD, "lpdwRenderState")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), (DWORD, "dwRenderState")]),
    Method(HRESULT, "GetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), Out(LPDWORD, "lpdwLightState")]),
    Method(HRESULT, "SetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), (DWORD, "dwLightState")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DVERTEXTYPE, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DVERTEXTYPE, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (LPWORD, "dwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    Method(HRESULT, "GetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
]

IDirect3DDevice3.methods += [
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "lpD3DHWDevDesc"), Out(LPD3DDEVICEDESC, "lpD3DHELDevDesc")]),
    Method(HRESULT, "GetStats", [Out(LPD3DSTATS, "lpD3DStats")]),
    Method(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3")]),
    Method(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3")]),
    Method(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3"), Out(Pointer(LPDIRECT3DVIEWPORT3), "lplpDirect3DViewport3"), (DWORD, "dwFlags")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMPIXELFORMATSCALLBACK, "lpD3DEnumPixelProc"), (LPVOID, "lpArg")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D3), "lplpDirect3D3")]),
    Method(HRESULT, "SetCurrentViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3")]),
    Method(HRESULT, "GetCurrentViewport", [Out(Pointer(LPDIRECT3DVIEWPORT3), "lplpDirect3DViewport3")]),
    Method(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE4, "lpNewRenderTarget"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpRenderTarget")]),
    Method(HRESULT, "Begin", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexTypeDesc"), (DWORD, "dwFlags")]),
    Method(HRESULT, "BeginIndexed", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    Method(HRESULT, "Vertex", [(LPVOID, "lpVertexType")]),
    Method(HRESULT, "Index", [(WORD, "wVertexIndex")]),
    Method(HRESULT, "End", [(DWORD, "dwFlags")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), Out(LPDWORD, "lpdwRenderState")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), (DWORD, "dwRenderState")]),
    Method(HRESULT, "GetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), Out(LPDWORD, "lpdwLightState")]),
    Method(HRESULT, "SetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), (DWORD, "dwLightState")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (LPWORD, "dwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    Method(HRESULT, "GetClipStatus", [Out(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    Method(HRESULT, "DrawPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (LPWORD, "lpIndex"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER, "lpD3DVertexBuf"), (DWORD, "dwStartVertex"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER, "lpD3DVertexBuf"), (LPWORD, "lpwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "ComputeSphereVisibility", [(LPD3DVECTOR, "lpCenters"), (LPD3DVALUE, "lpRadii"), (DWORD, "dwNumSpheres"), (DWORD, "dwFlags"), (LPDWORD, "lpdwReturnValues")]),
    Method(HRESULT, "GetTexture", [(DWORD, "dwStage"), Out(Pointer(LPDIRECT3DTEXTURE2), "lplpTexture2")]),
    Method(HRESULT, "SetTexture", [(DWORD, "dwStage"), (LPDIRECT3DTEXTURE2, "lpTexture2")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), Out(LPDWORD, "lpdwState")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), (DWORD, "dwState")]),
    Method(HRESULT, "ValidateDevice", [(LPDWORD, "lpdwPasses")]),
]

IDirect3DDevice7.methods += [
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC7, "lpD3DHELDevDesc")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMPIXELFORMATSCALLBACK, "lpD3DEnumPixelProc"), (LPVOID, "lpArg")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D7), "lplpDirect3D3")]),
    Method(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE7, "lpNewRenderTarget"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE7), "lplpRenderTarget")]),
    Method(HRESULT, "Clear", [(DWORD, "dwCount"), (LPD3DRECT, "lpRects"), (DWORD, "dwFlags"), (D3DCOLOR, "dwColor"), (D3DVALUE, "dvZ"), (DWORD, "dwStencil")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "SetViewport", [(LPD3DVIEWPORT7, "lpData")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    Method(HRESULT, "GetViewport", [Out(LPD3DVIEWPORT7, "lpData")]),
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL7, "lpMat")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL7, "lpMat")]),
    Method(HRESULT, "SetLight", [(DWORD, "dwLightIndex"), (LPD3DLIGHT7, "lpLight")]),
    Method(HRESULT, "GetLight", [(DWORD, "dwLightIndex"), (LPD3DLIGHT7, "lpLight")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), (DWORD, "dwRenderState")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), Out(LPDWORD, "lpdwRenderState")]),
    Method(HRESULT, "BeginStateBlock", []),
    Method(HRESULT, "EndStateBlock", [Out(LPDWORD, "lpdwBlockHandle")]),
    Method(HRESULT, "PreLoad", [(LPDIRECTDRAWSURFACE7, "lpddsTexture")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (LPWORD, "dwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    Method(HRESULT, "GetClipStatus", [Out(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    Method(HRESULT, "DrawPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (LPWORD, "lpIndex"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER7, "lpD3DVertexBuf"), (DWORD, "dwStartVertex"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    Method(HRESULT, "DrawIndexedPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER7, "lpD3DVertexBuf"), (DWORD, "dwStartVertex"), (DWORD, "dwNumVertices"), (LPWORD, "lpwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    Method(HRESULT, "ComputeSphereVisibility", [(LPD3DVECTOR, "lpCenters"), (LPD3DVALUE, "lpRadii"), (DWORD, "dwNumSpheres"), (DWORD, "dwFlags"), (LPDWORD, "lpdwReturnValues")]),
    Method(HRESULT, "GetTexture", [(DWORD, "dwStage"), Out(Pointer(LPDIRECTDRAWSURFACE7), "lpTexture")]),
    Method(HRESULT, "SetTexture", [(DWORD, "dwStage"), (LPDIRECTDRAWSURFACE7, "lpTexture")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), Out(LPDWORD, "lpdwState")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), (DWORD, "dwState")]),
    Method(HRESULT, "ValidateDevice", [Out(LPDWORD, "lpdwPasses")]),
    Method(HRESULT, "ApplyStateBlock", [(DWORD, "dwBlockHandle")]),
    Method(HRESULT, "CaptureStateBlock", [(DWORD, "dwBlockHandle")]),
    Method(HRESULT, "DeleteStateBlock", [(DWORD, "dwBlockHandle")]),
    Method(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "d3dsbType"), Out(LPDWORD, "lpdwBlockHandle")]),
    Method(HRESULT, "Load", [(LPDIRECTDRAWSURFACE7, "lpDestTex"), (LPPOINT, "lpDestPoint"), (LPDIRECTDRAWSURFACE7, "lpSrcTex"), (LPRECT, "lprcSrcRect"), (DWORD, "dwFlags")]),
    Method(HRESULT, "LightEnable", [(DWORD, "dwLightIndex"), (BOOL, "bEnable")]),
    Method(HRESULT, "GetLightEnable", [(DWORD, "dwLightIndex"), Out(Pointer(BOOL), "pbEnable")]),
    Method(HRESULT, "SetClipPlane", [(DWORD, "dwIndex"), (Pointer(D3DVALUE), "pPlaneEquation")]),
    Method(HRESULT, "GetClipPlane", [(DWORD, "dwIndex"), Out(Pointer(D3DVALUE), "pPlaneEquation")]),
    Method(HRESULT, "GetInfo", [(DWORD, "dwDevInfoID"), Out(LPVOID, "pDevInfoStruct"), (DWORD, "dwSize")]),
]

IDirect3DExecuteBuffer.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), (LPD3DEXECUTEBUFFERDESC, "lpDesc")]),
    Method(HRESULT, "Lock", [(LPD3DEXECUTEBUFFERDESC, "lpDesc")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "SetExecuteData", [(LPD3DEXECUTEDATA, "lpData")]),
    Method(HRESULT, "GetExecuteData", [Out(LPD3DEXECUTEDATA, "lpData")]),
    Method(HRESULT, "Validate", [(LPDWORD, "lpdwOffset"), (LPD3DVALIDATECALLBACK, "lpFunc"), (LPVOID, "lpUserArg"), (DWORD, "dwReserved")]),
    Method(HRESULT, "Optimize", [(DWORD, "dwDummy")]),
]

IDirect3DLight.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D")]),
    Method(HRESULT, "SetLight", [(LPD3DLIGHT, "lpLight")]),
    Method(HRESULT, "GetLight", [Out(LPD3DLIGHT, "lpLight")]),
]

IDirect3DMaterial.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D")]),
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "lpMat")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "lpMat")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), Out(LPD3DMATERIALHANDLE, "lpHandle")]),
    Method(HRESULT, "Reserve", []),
    Method(HRESULT, "Unreserve", []),
]

IDirect3DMaterial2.methods += [
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "lpMat")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "lpMat")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE2, "lpDirect3DDevice2"), Out(LPD3DMATERIALHANDLE, "lpHandle")]),
]

IDirect3DMaterial3.methods += [
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "lpMat")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "lpMat")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE3, "lpDirect3DDevice3"), Out(LPD3DMATERIALHANDLE, "lpHandle")]),
]

IDirect3DTexture.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), (LPDIRECTDRAWSURFACE, "lpDDSurface")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), Out(LPD3DTEXTUREHANDLE, "lpHandle")]),
    Method(HRESULT, "PaletteChanged", [(DWORD, "dwStart"), (DWORD, "dwCount")]),
    Method(HRESULT, "Load", [(LPDIRECT3DTEXTURE, "lpD3DTexture")]),
    Method(HRESULT, "Unload", []),
]

IDirect3DTexture2.methods += [
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE2, "lpDirect3DDevice2"), Out(LPD3DTEXTUREHANDLE, "lpHandle")]),
    Method(HRESULT, "PaletteChanged", [(DWORD, "dwStart"), (DWORD, "dwCount")]),
    Method(HRESULT, "Load", [(LPDIRECT3DTEXTURE2, "lpD3DTexture2")]),
]

IDirect3DViewport.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D")]),
    Method(HRESULT, "GetViewport", [Out(LPD3DVIEWPORT, "lpData")]),
    Method(HRESULT, "SetViewport", [(LPD3DVIEWPORT, "lpData")]),
    Method(HRESULT, "TransformVertices", [(DWORD, "dwVertexCount"), (LPD3DTRANSFORMDATA, "lpData"), (DWORD, "dwFlags"), (LPDWORD, "lpOffScreen")]),
    Method(HRESULT, "LightElements", [(DWORD, "dwElementCount"), (LPD3DLIGHTDATA, "lpData")]),
    Method(HRESULT, "SetBackground", [(D3DMATERIALHANDLE, "hMat")]),
    Method(HRESULT, "GetBackground", [Out(LPD3DMATERIALHANDLE, "lphMat"), Out(LPBOOL, "lpValid")]),
    Method(HRESULT, "SetBackgroundDepth", [(LPDIRECTDRAWSURFACE, "lpDDSurface")]),
    Method(HRESULT, "GetBackgroundDepth", [Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDDSurface"), Out(LPBOOL, "lpValid")]),
    Method(HRESULT, "Clear", [(DWORD, "dwCount"), (LPD3DRECT, "lpRects"), (DWORD, "dwFlags")]),
    Method(HRESULT, "AddLight", [(LPDIRECT3DLIGHT, "lpDirect3DLight")]),
    Method(HRESULT, "DeleteLight", [(LPDIRECT3DLIGHT, "lpDirect3DLight")]),
    Method(HRESULT, "NextLight", [(LPDIRECT3DLIGHT, "lpDirect3DLight"), Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (DWORD, "dwFlags")]),
]

IDirect3DViewport2.methods += [
   Method(HRESULT, "GetViewport2", [Out(LPD3DVIEWPORT2, "lpData")]),
    Method(HRESULT, "SetViewport2", [(LPD3DVIEWPORT2, "lpData")]),
]

IDirect3DViewport3.methods += [
    Method(HRESULT, "SetBackgroundDepth2", [(LPDIRECTDRAWSURFACE4, "lpDDS")]),
    Method(HRESULT, "GetBackgroundDepth2", [Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpDDS"), (LPBOOL, "lpValid")]),
    Method(HRESULT, "Clear2", [(DWORD, "dwCount"), (LPD3DRECT, "lpRects"), (DWORD, "dwFlags"), (D3DCOLOR, "dwColor"), (D3DVALUE, "dvZ"), (DWORD, "dwStencil")]),
]

IDirect3DVertexBuffer.methods += [
    Method(HRESULT, "Lock", [(DWORD, "dwFlags"), Out(Pointer(LPVOID), "lplpData"), (LPDWORD, "lpdwSize")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "ProcessVertices", [(DWORD, "dwVertexOp"), (DWORD, "dwDestIndex"), (DWORD, "dwCount"), (LPDIRECT3DVERTEXBUFFER, "lpSrcBuffer"), (DWORD, "dwSrcIndex"), (LPDIRECT3DDEVICE3, "lpD3DDevice"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetVertexBufferDesc", [Out(LPD3DVERTEXBUFFERDESC, "lpD3DVertexBufferDesc")]),
    Method(HRESULT, "Optimize", [(LPDIRECT3DDEVICE3, "lpD3DDevice"), (DWORD, "dwFlags")]),
]

IDirect3DVertexBuffer7.methods += [
    Method(HRESULT, "Lock", [(DWORD, "dwFlags"), Out(Pointer(LPVOID), "lplpData"), (LPDWORD, "lpdwSize")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "ProcessVertices", [(DWORD, "dwVertexOp"), (DWORD, "dwDestIndex"), (DWORD, "dwCount"), (LPDIRECT3DVERTEXBUFFER7, "lpSrcBuffer"), (DWORD, "dwSrcIndex"), (LPDIRECT3DDEVICE7, "lpD3DDevice"), (DWORD, "dwFlags")]),
    Method(HRESULT, "GetVertexBufferDesc", [Out(LPD3DVERTEXBUFFERDESC, "lpD3DVertexBufferDesc")]),
    Method(HRESULT, "Optimize", [(LPDIRECT3DDEVICE7, "lpD3DDevice"), (DWORD, "dwFlags")]),
    Method(HRESULT, "ProcessVerticesStrided", [(DWORD, "dwVertexOp"), (DWORD, "dwDestIndex"), (DWORD, "dwCount"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpStrideData"), (DWORD, "dwVertexTypeDesc"), (LPDIRECT3DDEVICE7, "lpD3DDevice"), (DWORD, "dwFlags")]),
]

interfaces = [
    IDirectDraw,
    IDirectDraw2,
    IDirectDraw4,
    IDirectDraw7,
    IDirect3D,
    IDirect3D2,
    IDirect3D3,
    IDirect3D7,
]

ddraw.addInterfaces(interfaces)


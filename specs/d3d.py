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

HRESULT = MAKE_HRESULT(ok = "D3D_OK", errors = [
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

LPUNKNOWN = ObjPointer(IUnknown)

LPDIRECT3D = ObjPointer(IDirect3D)
LPDIRECT3DDEVICE = ObjPointer(IDirect3DDevice)
LPDIRECT3DEXECUTEBUFFER = ObjPointer(IDirect3DExecuteBuffer)
LPDIRECT3DLIGHT = ObjPointer(IDirect3DLight)
LPDIRECT3DMATERIAL = ObjPointer(IDirect3DMaterial)
LPDIRECT3DTEXTURE = ObjPointer(IDirect3DTexture)
LPDIRECT3DVIEWPORT = ObjPointer(IDirect3DViewport)

LPDIRECT3D2 = ObjPointer(IDirect3D2)
LPDIRECT3DDEVICE2 = ObjPointer(IDirect3DDevice2)
LPDIRECT3DMATERIAL2 = ObjPointer(IDirect3DMaterial2)
LPDIRECT3DTEXTURE2 = ObjPointer(IDirect3DTexture2)
LPDIRECT3DVIEWPORT2 = ObjPointer(IDirect3DViewport2)

LPDIRECT3D3 = ObjPointer(IDirect3D3)
LPDIRECT3DDEVICE3 = ObjPointer(IDirect3DDevice3)
LPDIRECT3DMATERIAL3 = ObjPointer(IDirect3DMaterial3)
LPDIRECT3DVIEWPORT3 = ObjPointer(IDirect3DViewport3)
LPDIRECT3DVERTEXBUFFER = ObjPointer(IDirect3DVertexBuffer)

LPDIRECT3D7 = ObjPointer(IDirect3D7)
LPDIRECT3DDEVICE7 = ObjPointer(IDirect3DDevice7)
LPDIRECT3DVERTEXBUFFER7 = ObjPointer(IDirect3DVertexBuffer7)

IDirect3D.methods += [
    StdMethod(HRESULT, "Initialize", [(REFCLSID, "riid")]),
    StdMethod(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    StdMethod(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL), "lplpDirect3DMaterial"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT), "lplpD3DViewport"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "lpD3DDFS"), (LPD3DFINDDEVICERESULT, "lplpD3DDevice")]),
]

IDirect3D2.methods += [
    StdMethod(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    StdMethod(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL2), "lplpDirect3DMaterial2"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT2), "lplpD3DViewport2"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "lpD3DDFS"), (LPD3DFINDDEVICERESULT, "lpD3DFDR")]),
    StdMethod(HRESULT, "CreateDevice", [(REFCLSID, "rclsid"), (LPDIRECTDRAWSURFACE, "lpDDS"), Out(Pointer(LPDIRECT3DDEVICE2), "lplpD3DDevice2")]),
]

IDirect3D3.methods += [
    StdMethod(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    StdMethod(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL3), "lplpDirect3DMaterial3"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT3), "lplpD3DViewport3"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "lpD3DDFS"), Out(LPD3DFINDDEVICERESULT, "lpD3DFDR")]),
    StdMethod(HRESULT, "CreateDevice", [(REFCLSID, "rclsid"), (LPDIRECTDRAWSURFACE4, "lpDDS"), Out(Pointer(LPDIRECT3DDEVICE3), "lplpD3DDevice3"), (LPUNKNOWN, "lpUnk")]),
    StdMethod(HRESULT, "CreateVertexBuffer", [(LPD3DVERTEXBUFFERDESC, "lpD3DVertBufDesc"), Out(Pointer(LPDIRECT3DVERTEXBUFFER), "lplpD3DVertBuf"), (DWORD, "dwFlags"), (LPUNKNOWN, "lpUnk")]),
    StdMethod(HRESULT, "EnumZBufferFormats", [(REFCLSID, "riidDevice"), (LPD3DENUMPIXELFORMATSCALLBACK, "lpEnumCallback"), (LPVOID, "lpContext")]),
    StdMethod(HRESULT, "EvictManagedTextures", []),
]

IDirect3D7.methods += [
    StdMethod(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK7, "lpEnumDevicesCallback"), (LPVOID, "lpUserArg")]),
    StdMethod(HRESULT, "CreateDevice", [(REFCLSID, "rclsid"), (LPDIRECTDRAWSURFACE7, "lpDDS"), Out(Pointer(LPDIRECT3DDEVICE7), "lplpD3DDevice")]),
    StdMethod(HRESULT, "CreateVertexBuffer", [(LPD3DVERTEXBUFFERDESC, "lpD3DVertBufDesc"), Out(Pointer(LPDIRECT3DVERTEXBUFFER7), "lplpD3DVertBuf"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "EnumZBufferFormats", [(REFCLSID, "riidDevice"), (LPD3DENUMPIXELFORMATSCALLBACK, "lpEnumCallback"), (LPVOID, "lpContext")]),
    StdMethod(HRESULT, "EvictManagedTextures", []),
]

IDirect3DDevice.methods += [
    StdMethod(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D"), (LPGUID, "lpGUID"), (LPD3DDEVICEDESC, "lpD3DDVDesc")]),
    StdMethod(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "lpD3DHWDevDesc"), Out(LPD3DDEVICEDESC, "lpD3DHELDevDesc")]),
    StdMethod(HRESULT, "SwapTextureHandles", [(LPDIRECT3DTEXTURE, "lpD3Dtex1"), (LPDIRECT3DTEXTURE, "lpD3DTex2")]),
    StdMethod(HRESULT, "CreateExecuteBuffer", [(LPD3DEXECUTEBUFFERDESC, "lpDesc"), Out(Pointer(LPDIRECT3DEXECUTEBUFFER), "lplpDirect3DExecuteBuffer"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(HRESULT, "GetStats", [(LPD3DSTATS, "lpD3DStats")]),
    StdMethod(HRESULT, "Execute", [(LPDIRECT3DEXECUTEBUFFER, "lpDirect3DExecuteBuffer"), (LPDIRECT3DVIEWPORT, "lpDirect3DViewport"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT, "lpDirect3DViewport")]),
    StdMethod(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT, "lpDirect3DViewport")]),
    StdMethod(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT, "lpDirect3DViewport"), Out(Pointer(LPDIRECT3DVIEWPORT), "lplpDirect3DViewport"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "Pick", [(LPDIRECT3DEXECUTEBUFFER, "lpDirect3DExecuteBuffer"), (LPDIRECT3DVIEWPORT, "lpDirect3DViewport"), (DWORD, "dwFlags"), (LPD3DRECT, "lpRect")]),
    StdMethod(HRESULT, "GetPickRecords", [(LPDWORD, "lpCount"), (LPD3DPICKRECORD, "lpD3DPickRec")]),
    StdMethod(HRESULT, "EnumTextureFormats", [(LPD3DENUMTEXTUREFORMATSCALLBACK, "lpD3DEnumTextureProc"), (LPVOID, "lpArg")]),
    StdMethod(HRESULT, "CreateMatrix", [Out(LPD3DMATRIXHANDLE, "lpD3DMatHandle")]),
    StdMethod(HRESULT, "SetMatrix", [(D3DMATRIXHANDLE, "D3DMatHandle"), (Const(LPD3DMATRIX), "lpD3DMatrix")]),
    StdMethod(HRESULT, "GetMatrix", [(D3DMATRIXHANDLE, "D3DMatHandle"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "DeleteMatrix", [(D3DMATRIXHANDLE, "D3DMatHandle")]),
    StdMethod(HRESULT, "BeginScene", []),
    StdMethod(HRESULT, "EndScene", []),
    StdMethod(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D), "lplpDirect3D")]),
]

IDirect3DDevice2.methods += [
    StdMethod(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "lpD3DHWDevDesc"), Out(LPD3DDEVICEDESC, "lpD3DHELDevDesc")]),
    StdMethod(HRESULT, "SwapTextureHandles", [(LPDIRECT3DTEXTURE2, "lpD3DTex1"), (LPDIRECT3DTEXTURE2, "lpD3DTex2")]),
    StdMethod(HRESULT, "GetStats", [Out(LPD3DSTATS, "lpD3DStats")]),
    StdMethod(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2")]),
    StdMethod(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2")]),
    StdMethod(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2"), Out(Pointer(LPDIRECT3DVIEWPORT2), "lplpDirect3DViewport2"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "EnumTextureFormats", [(LPD3DENUMTEXTUREFORMATSCALLBACK, "lpD3DEnumTextureProc"), (LPVOID, "lpArg")]),
    StdMethod(HRESULT, "BeginScene", []),
    StdMethod(HRESULT, "EndScene", []),
    StdMethod(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D2), "lplpDirect3D2")]),
    StdMethod(HRESULT, "SetCurrentViewport", [(LPDIRECT3DVIEWPORT2, "lpDirect3DViewport2")]),
    StdMethod(HRESULT, "GetCurrentViewport", [Out(Pointer(LPDIRECT3DVIEWPORT2), "lplpDirect3DViewport2")]),
    StdMethod(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE, "lpNewRenderTarget"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE), "lplpRenderTarget")]),
    StdMethod(HRESULT, "Begin", [(D3DPRIMITIVETYPE, "d3dpt"), (D3DVERTEXTYPE, "dwVertexTypeDesc"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "BeginIndexed", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DVERTEXTYPE, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "Vertex", [(LPVOID, "lpVertexType")]),
    StdMethod(HRESULT, "Index", [(WORD, "wVertexIndex")]),
    StdMethod(HRESULT, "End", [(DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), Out(LPDWORD, "lpdwRenderState")]),
    StdMethod(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), (DWORD, "dwRenderState")]),
    StdMethod(HRESULT, "GetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), Out(LPDWORD, "lpdwLightState")]),
    StdMethod(HRESULT, "SetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), (DWORD, "dwLightState")]),
    StdMethod(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DVERTEXTYPE, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DVERTEXTYPE, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (LPWORD, "dwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    StdMethod(HRESULT, "GetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
]

IDirect3DDevice3.methods += [
    StdMethod(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "lpD3DHWDevDesc"), Out(LPD3DDEVICEDESC, "lpD3DHELDevDesc")]),
    StdMethod(HRESULT, "GetStats", [Out(LPD3DSTATS, "lpD3DStats")]),
    StdMethod(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3")]),
    StdMethod(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3")]),
    StdMethod(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3"), Out(Pointer(LPDIRECT3DVIEWPORT3), "lplpDirect3DViewport3"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "EnumTextureFormats", [(LPD3DENUMPIXELFORMATSCALLBACK, "lpD3DEnumPixelProc"), (LPVOID, "lpArg")]),
    StdMethod(HRESULT, "BeginScene", []),
    StdMethod(HRESULT, "EndScene", []),
    StdMethod(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D3), "lplpDirect3D3")]),
    StdMethod(HRESULT, "SetCurrentViewport", [(LPDIRECT3DVIEWPORT3, "lpDirect3DViewport3")]),
    StdMethod(HRESULT, "GetCurrentViewport", [Out(Pointer(LPDIRECT3DVIEWPORT3), "lplpDirect3DViewport3")]),
    StdMethod(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE4, "lpNewRenderTarget"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpRenderTarget")]),
    StdMethod(HRESULT, "Begin", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexTypeDesc"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "BeginIndexed", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "Vertex", [(LPVOID, "lpVertexType")]),
    StdMethod(HRESULT, "Index", [(WORD, "wVertexIndex")]),
    StdMethod(HRESULT, "End", [(DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), Out(LPDWORD, "lpdwRenderState")]),
    StdMethod(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), (DWORD, "dwRenderState")]),
    StdMethod(HRESULT, "GetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), Out(LPDWORD, "lpdwLightState")]),
    StdMethod(HRESULT, "SetLightState", [(D3DLIGHTSTATETYPE, "dwLightStateType"), (DWORD, "dwLightState")]),
    StdMethod(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (LPWORD, "dwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    StdMethod(HRESULT, "GetClipStatus", [Out(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    StdMethod(HRESULT, "DrawPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (DWORD, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (LPWORD, "lpIndex"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER, "lpD3DVertexBuf"), (DWORD, "dwStartVertex"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER, "lpD3DVertexBuf"), (LPWORD, "lpwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "ComputeSphereVisibility", [(LPD3DVECTOR, "lpCenters"), (LPD3DVALUE, "lpRadii"), (DWORD, "dwNumSpheres"), (DWORD, "dwFlags"), (LPDWORD, "lpdwReturnValues")]),
    StdMethod(HRESULT, "GetTexture", [(DWORD, "dwStage"), Out(Pointer(LPDIRECT3DTEXTURE2), "lplpTexture2")]),
    StdMethod(HRESULT, "SetTexture", [(DWORD, "dwStage"), (LPDIRECT3DTEXTURE2, "lpTexture2")]),
    StdMethod(HRESULT, "GetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), Out(LPDWORD, "lpdwState")]),
    StdMethod(HRESULT, "SetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), (DWORD, "dwState")]),
    StdMethod(HRESULT, "ValidateDevice", [(LPDWORD, "lpdwPasses")]),
]

IDirect3DDevice7.methods += [
    StdMethod(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC7, "lpD3DHELDevDesc")]),
    StdMethod(HRESULT, "EnumTextureFormats", [(LPD3DENUMPIXELFORMATSCALLBACK, "lpD3DEnumPixelProc"), (LPVOID, "lpArg")]),
    StdMethod(HRESULT, "BeginScene", []),
    StdMethod(HRESULT, "EndScene", []),
    StdMethod(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D7), "lplpDirect3D3")]),
    StdMethod(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE7, "lpNewRenderTarget"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE7), "lplpRenderTarget")]),
    StdMethod(HRESULT, "Clear", [(DWORD, "dwCount"), (LPD3DRECT, "lpRects"), (DWORD, "dwFlags"), (D3DCOLOR, "dwColor"), (D3DVALUE, "dvZ"), (DWORD, "dwStencil")]),
    StdMethod(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), Out(LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "SetViewport", [(LPD3DVIEWPORT7, "lpData")]),
    StdMethod(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "dtstTransformStateType"), (LPD3DMATRIX, "lpD3DMatrix")]),
    StdMethod(HRESULT, "GetViewport", [Out(LPD3DVIEWPORT7, "lpData")]),
    StdMethod(HRESULT, "SetMaterial", [(LPD3DMATERIAL7, "lpMat")]),
    StdMethod(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL7, "lpMat")]),
    StdMethod(HRESULT, "SetLight", [(DWORD, "dwLightIndex"), (LPD3DLIGHT7, "lpLight")]),
    StdMethod(HRESULT, "GetLight", [(DWORD, "dwLightIndex"), (LPD3DLIGHT7, "lpLight")]),
    StdMethod(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), (DWORD, "dwRenderState")]),
    StdMethod(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "dwRenderStateType"), Out(LPDWORD, "lpdwRenderState")]),
    StdMethod(HRESULT, "BeginStateBlock", []),
    StdMethod(HRESULT, "EndStateBlock", [Out(LPDWORD, "lpdwBlockHandle")]),
    StdMethod(HRESULT, "PreLoad", [(LPDIRECTDRAWSURFACE7, "lpddsTexture")]),
    StdMethod(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DFVF, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DFVF, "d3dvtVertexType"), (LPVOID, "lpvVertices"), (DWORD, "dwVertexCount"), (LPWORD, "dwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    StdMethod(HRESULT, "GetClipStatus", [Out(LPD3DCLIPSTATUS, "lpD3DClipStatus")]),
    StdMethod(HRESULT, "DrawPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DFVF, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitiveStrided", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (D3DFVF, "dwVertexType"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpD3DDrawPrimStrideData"), (DWORD, "dwVertexCount"), (LPWORD, "lpIndex"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER7, "lpD3DVertexBuf"), (DWORD, "dwStartVertex"), (DWORD, "dwNumVertices"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "DrawIndexedPrimitiveVB", [(D3DPRIMITIVETYPE, "d3dptPrimitiveType"), (LPDIRECT3DVERTEXBUFFER7, "lpD3DVertexBuf"), (DWORD, "dwStartVertex"), (DWORD, "dwNumVertices"), (LPWORD, "lpwIndices"), (DWORD, "dwIndexCount"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "ComputeSphereVisibility", [(LPD3DVECTOR, "lpCenters"), (LPD3DVALUE, "lpRadii"), (DWORD, "dwNumSpheres"), (DWORD, "dwFlags"), (LPDWORD, "lpdwReturnValues")]),
    StdMethod(HRESULT, "GetTexture", [(DWORD, "dwStage"), Out(Pointer(LPDIRECTDRAWSURFACE7), "lpTexture")]),
    StdMethod(HRESULT, "SetTexture", [(DWORD, "dwStage"), (LPDIRECTDRAWSURFACE7, "lpTexture")]),
    StdMethod(HRESULT, "GetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), Out(LPDWORD, "lpdwState")]),
    StdMethod(HRESULT, "SetTextureStageState", [(DWORD, "dwStage"), (D3DTEXTURESTAGESTATETYPE, "d3dTexStageStateType"), (DWORD, "dwState")]),
    StdMethod(HRESULT, "ValidateDevice", [Out(LPDWORD, "lpdwPasses")]),
    StdMethod(HRESULT, "ApplyStateBlock", [(DWORD, "dwBlockHandle")]),
    StdMethod(HRESULT, "CaptureStateBlock", [(DWORD, "dwBlockHandle")]),
    StdMethod(HRESULT, "DeleteStateBlock", [(DWORD, "dwBlockHandle")]),
    StdMethod(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "d3dsbType"), Out(LPDWORD, "lpdwBlockHandle")]),
    StdMethod(HRESULT, "Load", [(LPDIRECTDRAWSURFACE7, "lpDestTex"), (LPPOINT, "lpDestPoint"), (LPDIRECTDRAWSURFACE7, "lpSrcTex"), (LPRECT, "lprcSrcRect"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "LightEnable", [(DWORD, "dwLightIndex"), (BOOL, "bEnable")]),
    StdMethod(HRESULT, "GetLightEnable", [(DWORD, "dwLightIndex"), Out(Pointer(BOOL), "pbEnable")]),
    StdMethod(HRESULT, "SetClipPlane", [(DWORD, "dwIndex"), (Pointer(D3DVALUE), "pPlaneEquation")]),
    StdMethod(HRESULT, "GetClipPlane", [(DWORD, "dwIndex"), Out(Pointer(D3DVALUE), "pPlaneEquation")]),
    StdMethod(HRESULT, "GetInfo", [(DWORD, "dwDevInfoID"), Out(LPVOID, "pDevInfoStruct"), (DWORD, "dwSize")]),
]

IDirect3DExecuteBuffer.methods += [
    StdMethod(HRESULT, "Initialize", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), (LPD3DEXECUTEBUFFERDESC, "lpDesc")]),
    StdMethod(HRESULT, "Lock", [(LPD3DEXECUTEBUFFERDESC, "lpDesc")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "SetExecuteData", [(LPD3DEXECUTEDATA, "lpData")]),
    StdMethod(HRESULT, "GetExecuteData", [Out(LPD3DEXECUTEDATA, "lpData")]),
    StdMethod(HRESULT, "Validate", [(LPDWORD, "lpdwOffset"), (LPD3DVALIDATECALLBACK, "lpFunc"), (LPVOID, "lpUserArg"), (DWORD, "dwReserved")]),
    StdMethod(HRESULT, "Optimize", [(DWORD, "dwDummy")]),
]

IDirect3DLight.methods += [
    StdMethod(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D")]),
    StdMethod(HRESULT, "SetLight", [(LPD3DLIGHT, "lpLight")]),
    StdMethod(HRESULT, "GetLight", [Out(LPD3DLIGHT, "lpLight")]),
]

IDirect3DMaterial.methods += [
    StdMethod(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D")]),
    StdMethod(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "lpMat")]),
    StdMethod(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "lpMat")]),
    StdMethod(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), Out(LPD3DMATERIALHANDLE, "lpHandle")]),
    StdMethod(HRESULT, "Reserve", []),
    StdMethod(HRESULT, "Unreserve", []),
]

IDirect3DMaterial2.methods += [
    StdMethod(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "lpMat")]),
    StdMethod(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "lpMat")]),
    StdMethod(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE2, "lpDirect3DDevice2"), Out(LPD3DMATERIALHANDLE, "lpHandle")]),
]

IDirect3DMaterial3.methods += [
    StdMethod(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "lpMat")]),
    StdMethod(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "lpMat")]),
    StdMethod(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE3, "lpDirect3DDevice3"), Out(LPD3DMATERIALHANDLE, "lpHandle")]),
]

IDirect3DTexture.methods += [
    StdMethod(HRESULT, "Initialize", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), (LPDIRECTDRAWSURFACE, "lpDDSurface")]),
    StdMethod(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE, "lpDirect3DDevice"), Out(LPD3DTEXTUREHANDLE, "lpHandle")]),
    StdMethod(HRESULT, "PaletteChanged", [(DWORD, "dwStart"), (DWORD, "dwCount")]),
    StdMethod(HRESULT, "Load", [(LPDIRECT3DTEXTURE, "lpD3DTexture")]),
    StdMethod(HRESULT, "Unload", []),
]

IDirect3DTexture2.methods += [
    StdMethod(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE2, "lpDirect3DDevice2"), Out(LPD3DTEXTUREHANDLE, "lpHandle")]),
    StdMethod(HRESULT, "PaletteChanged", [(DWORD, "dwStart"), (DWORD, "dwCount")]),
    StdMethod(HRESULT, "Load", [(LPDIRECT3DTEXTURE2, "lpD3DTexture2")]),
]

IDirect3DViewport.methods += [
    StdMethod(HRESULT, "Initialize", [(LPDIRECT3D, "lpDirect3D")]),
    StdMethod(HRESULT, "GetViewport", [Out(LPD3DVIEWPORT, "lpData")]),
    StdMethod(HRESULT, "SetViewport", [(LPD3DVIEWPORT, "lpData")]),
    StdMethod(HRESULT, "TransformVertices", [(DWORD, "dwVertexCount"), (LPD3DTRANSFORMDATA, "lpData"), (DWORD, "dwFlags"), (LPDWORD, "lpOffScreen")]),
    StdMethod(HRESULT, "LightElements", [(DWORD, "dwElementCount"), (LPD3DLIGHTDATA, "lpData")]),
    StdMethod(HRESULT, "SetBackground", [(D3DMATERIALHANDLE, "hMat")]),
    StdMethod(HRESULT, "GetBackground", [Out(LPD3DMATERIALHANDLE, "lphMat"), Out(LPBOOL, "lpValid")]),
    StdMethod(HRESULT, "SetBackgroundDepth", [(LPDIRECTDRAWSURFACE, "lpDDSurface")]),
    StdMethod(HRESULT, "GetBackgroundDepth", [Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDDSurface"), Out(LPBOOL, "lpValid")]),
    StdMethod(HRESULT, "Clear", [(DWORD, "dwCount"), (LPD3DRECT, "lpRects"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "AddLight", [(LPDIRECT3DLIGHT, "lpDirect3DLight")]),
    StdMethod(HRESULT, "DeleteLight", [(LPDIRECT3DLIGHT, "lpDirect3DLight")]),
    StdMethod(HRESULT, "NextLight", [(LPDIRECT3DLIGHT, "lpDirect3DLight"), Out(Pointer(LPDIRECT3DLIGHT), "lplpDirect3DLight"), (DWORD, "dwFlags")]),
]

IDirect3DViewport2.methods += [
   StdMethod(HRESULT, "GetViewport2", [Out(LPD3DVIEWPORT2, "lpData")]),
    StdMethod(HRESULT, "SetViewport2", [(LPD3DVIEWPORT2, "lpData")]),
]

IDirect3DViewport3.methods += [
    StdMethod(HRESULT, "SetBackgroundDepth2", [(LPDIRECTDRAWSURFACE4, "lpDDS")]),
    StdMethod(HRESULT, "GetBackgroundDepth2", [Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpDDS"), (LPBOOL, "lpValid")]),
    StdMethod(HRESULT, "Clear2", [(DWORD, "dwCount"), (LPD3DRECT, "lpRects"), (DWORD, "dwFlags"), (D3DCOLOR, "dwColor"), (D3DVALUE, "dvZ"), (DWORD, "dwStencil")]),
]

IDirect3DVertexBuffer.methods += [
    StdMethod(HRESULT, "Lock", [(DWORD, "dwFlags"), Out(Pointer(LPVOID), "lplpData"), (LPDWORD, "lpdwSize")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "ProcessVertices", [(DWORD, "dwVertexOp"), (DWORD, "dwDestIndex"), (DWORD, "dwCount"), (LPDIRECT3DVERTEXBUFFER, "lpSrcBuffer"), (DWORD, "dwSrcIndex"), (LPDIRECT3DDEVICE3, "lpD3DDevice"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetVertexBufferDesc", [Out(LPD3DVERTEXBUFFERDESC, "lpD3DVertexBufferDesc")]),
    StdMethod(HRESULT, "Optimize", [(LPDIRECT3DDEVICE3, "lpD3DDevice"), (DWORD, "dwFlags")]),
]

IDirect3DVertexBuffer7.methods += [
    StdMethod(HRESULT, "Lock", [(DWORD, "dwFlags"), Out(Pointer(LPVOID), "lplpData"), (LPDWORD, "lpdwSize")]),
    StdMethod(HRESULT, "Unlock", []),
    StdMethod(HRESULT, "ProcessVertices", [(DWORD, "dwVertexOp"), (DWORD, "dwDestIndex"), (DWORD, "dwCount"), (LPDIRECT3DVERTEXBUFFER7, "lpSrcBuffer"), (DWORD, "dwSrcIndex"), (LPDIRECT3DDEVICE7, "lpD3DDevice"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "GetVertexBufferDesc", [Out(LPD3DVERTEXBUFFERDESC, "lpD3DVertexBufferDesc")]),
    StdMethod(HRESULT, "Optimize", [(LPDIRECT3DDEVICE7, "lpD3DDevice"), (DWORD, "dwFlags")]),
    StdMethod(HRESULT, "ProcessVerticesStrided", [(DWORD, "dwVertexOp"), (DWORD, "dwDestIndex"), (DWORD, "dwCount"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "lpStrideData"), (DWORD, "dwVertexTypeDesc"), (LPDIRECT3DDEVICE7, "lpD3DDevice"), (DWORD, "dwFlags")]),
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


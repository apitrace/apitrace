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

LPUNKNOWN = WrapPointer(IUnknown)

LPDIRECT3D = WrapPointer(IDirect3D)
LPDIRECT3DDEVICE = WrapPointer(IDirect3DDevice)
LPDIRECT3DEXECUTEBUFFER = WrapPointer(IDirect3DExecuteBuffer)
LPDIRECT3DLIGHT = WrapPointer(IDirect3DLight)
LPDIRECT3DMATERIAL = WrapPointer(IDirect3DMaterial)
LPDIRECT3DTEXTURE = WrapPointer(IDirect3DTexture)
LPDIRECT3DVIEWPORT = WrapPointer(IDirect3DViewport)

LPDIRECT3D2 = WrapPointer(IDirect3D2)
LPDIRECT3DDEVICE2 = WrapPointer(IDirect3DDevice2)
LPDIRECT3DMATERIAL2 = WrapPointer(IDirect3DMaterial2)
LPDIRECT3DTEXTURE2 = WrapPointer(IDirect3DTexture2)
LPDIRECT3DVIEWPORT2 = WrapPointer(IDirect3DViewport2)

LPDIRECT3D3 = WrapPointer(IDirect3D3)
LPDIRECT3DDEVICE3 = WrapPointer(IDirect3DDevice3)
LPDIRECT3DMATERIAL3 = WrapPointer(IDirect3DMaterial3)
LPDIRECT3DVIEWPORT3 = WrapPointer(IDirect3DViewport3)
LPDIRECT3DVERTEXBUFFER = WrapPointer(IDirect3DVertexBuffer)

LPDIRECT3D7 = WrapPointer(IDirect3D7)
LPDIRECT3DDEVICE7 = WrapPointer(IDirect3DDevice7)
LPDIRECT3DVERTEXBUFFER7 = WrapPointer(IDirect3DVertexBuffer7)

IDirect3D.methods += [
    Method(HRESULT, "Initialize", [(REFCLSID, "arg0")]),
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "arg0"), (LPD3DFINDDEVICERESULT, "arg1")]),
]

IDirect3D2.methods += [
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL2), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT2), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "arg0"), (LPD3DFINDDEVICERESULT, "arg1")]),
    Method(HRESULT, "CreateDevice", [(REFCLSID, "arg0"), (LPDIRECTDRAWSURFACE, "arg1"), Out(Pointer(LPDIRECT3DDEVICE2), "arg2")]),
]

IDirect3D3.methods += [
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "CreateLight", [Out(Pointer(LPDIRECT3DLIGHT), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "CreateMaterial", [Out(Pointer(LPDIRECT3DMATERIAL3), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "CreateViewport", [Out(Pointer(LPDIRECT3DVIEWPORT3), "arg0"), (LPUNKNOWN, "arg1")]),
    Method(HRESULT, "FindDevice", [(LPD3DFINDDEVICESEARCH, "arg0"), Out(LPD3DFINDDEVICERESULT, "arg1")]),
    Method(HRESULT, "CreateDevice", [(REFCLSID, "arg0"), (LPDIRECTDRAWSURFACE4, "arg1"), Out(Pointer(LPDIRECT3DDEVICE3), "arg2"), (LPUNKNOWN, "arg3")]),
    Method(HRESULT, "CreateVertexBuffer", [(LPD3DVERTEXBUFFERDESC, "arg0"), Out(Pointer(LPDIRECT3DVERTEXBUFFER), "arg1"), (DWORD, "arg2"), (LPUNKNOWN, "arg3")]),
    Method(HRESULT, "EnumZBufferFormats", [(REFCLSID, "arg0"), (LPD3DENUMPIXELFORMATSCALLBACK, "arg1"), (LPVOID, "arg2")]),
    Method(HRESULT, "EvictManagedTextures", []),
]

IDirect3D7.methods += [
    Method(HRESULT, "EnumDevices", [(LPD3DENUMDEVICESCALLBACK7, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "CreateDevice", [(REFCLSID, "arg0"), (LPDIRECTDRAWSURFACE7, "arg1"), Out(Pointer(LPDIRECT3DDEVICE7), "arg2")]),
    Method(HRESULT, "CreateVertexBuffer", [(LPD3DVERTEXBUFFERDESC, "arg0"), Out(Pointer(LPDIRECT3DVERTEXBUFFER7), "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "EnumZBufferFormats", [(REFCLSID, "arg0"), (LPD3DENUMPIXELFORMATSCALLBACK, "arg1"), (LPVOID, "arg2")]),
    Method(HRESULT, "EvictManagedTextures", []),
]

IDirect3DDevice.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "arg0"), (LPGUID, "arg1"), (LPD3DDEVICEDESC, "arg2")]),
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "arg0"), Out(LPD3DDEVICEDESC, "arg1")]),
    Method(HRESULT, "SwapTextureHandles", [(LPDIRECT3DTEXTURE, "arg0"), (LPDIRECT3DTEXTURE, "arg1")]),
    Method(HRESULT, "CreateExecuteBuffer", [(LPD3DEXECUTEBUFFERDESC, "arg0"), Out(Pointer(LPDIRECT3DEXECUTEBUFFER), "arg1"), (LPUNKNOWN, "arg2")]),
    Method(HRESULT, "GetStats", [(LPD3DSTATS, "arg0")]),
    Method(HRESULT, "Execute", [(LPDIRECT3DEXECUTEBUFFER, "arg0"), (LPDIRECT3DVIEWPORT, "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT, "arg0")]),
    Method(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT, "arg0")]),
    Method(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT, "arg0"), Out(Pointer(LPDIRECT3DVIEWPORT), "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "Pick", [(LPDIRECT3DEXECUTEBUFFER, "arg0"), (LPDIRECT3DVIEWPORT, "arg1"), (DWORD, "arg2"), (LPD3DRECT, "arg3")]),
    Method(HRESULT, "GetPickRecords", [(LPDWORD, "arg0"), (LPD3DPICKRECORD, "arg1")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMTEXTUREFORMATSCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "CreateMatrix", [Out(LPD3DMATRIXHANDLE, "arg0")]),
    Method(HRESULT, "SetMatrix", [(D3DMATRIXHANDLE, "arg0"), (Const(LPD3DMATRIX), "arg1")]),
    Method(HRESULT, "GetMatrix", [(D3DMATRIXHANDLE, "arg0"), Out(LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "DeleteMatrix", [(D3DMATRIXHANDLE, "arg0")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [OutPointer(LPDIRECT3D)]),
]

IDirect3DDevice2.methods += [
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "arg0"), Out(LPD3DDEVICEDESC, "arg1")]),
    Method(HRESULT, "SwapTextureHandles", [(LPDIRECT3DTEXTURE2, "arg0"), (LPDIRECT3DTEXTURE2, "arg1")]),
    Method(HRESULT, "GetStats", [Out(LPD3DSTATS, "arg0")]),
    Method(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT2, "arg0")]),
    Method(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT2, "arg0")]),
    Method(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT2, "arg0"), Out(Pointer(LPDIRECT3DVIEWPORT2), "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMTEXTUREFORMATSCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D2), "arg0")]),
    Method(HRESULT, "SetCurrentViewport", [(LPDIRECT3DVIEWPORT2, "arg0")]),
    Method(HRESULT, "GetCurrentViewport", [Out(Pointer(LPDIRECT3DVIEWPORT2), "arg0")]),
    Method(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE), "arg0")]),
    Method(HRESULT, "Begin", [(D3DPRIMITIVETYPE, "arg0"), (D3DVERTEXTYPE, "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "BeginIndexed", [(D3DPRIMITIVETYPE, "arg0"), (D3DVERTEXTYPE, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "Vertex", [(LPVOID, "arg0")]),
    Method(HRESULT, "Index", [(WORD, "arg0")]),
    Method(HRESULT, "End", [(DWORD, "arg0")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "arg0"), Out(LPDWORD, "arg1")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "GetLightState", [(D3DLIGHTSTATETYPE, "arg0"), Out(LPDWORD, "arg1")]),
    Method(HRESULT, "SetLightState", [(D3DLIGHTSTATETYPE, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), (LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), Out(LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), Out(LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "arg0"), (D3DVERTEXTYPE, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "arg0"), (D3DVERTEXTYPE, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (LPWORD, "arg4"), (DWORD, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "arg0")]),
    Method(HRESULT, "GetClipStatus", [(LPD3DCLIPSTATUS, "arg0")]),
]

IDirect3DDevice3.methods += [
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC, "arg0"), Out(LPD3DDEVICEDESC, "arg1")]),
    Method(HRESULT, "GetStats", [Out(LPD3DSTATS, "arg0")]),
    Method(HRESULT, "AddViewport", [(LPDIRECT3DVIEWPORT3, "arg0")]),
    Method(HRESULT, "DeleteViewport", [(LPDIRECT3DVIEWPORT3, "arg0")]),
    Method(HRESULT, "NextViewport", [(LPDIRECT3DVIEWPORT3, "arg0"), Out(Pointer(LPDIRECT3DVIEWPORT3), "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMPIXELFORMATSCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D3), "arg0")]),
    Method(HRESULT, "SetCurrentViewport", [(LPDIRECT3DVIEWPORT3, "arg0")]),
    Method(HRESULT, "GetCurrentViewport", [Out(Pointer(LPDIRECT3DVIEWPORT3), "arg0")]),
    Method(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE4, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE4), "arg0")]),
    Method(HRESULT, "Begin", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "BeginIndexed", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "Vertex", [(LPVOID, "arg0")]),
    Method(HRESULT, "Index", [(WORD, "arg0")]),
    Method(HRESULT, "End", [(DWORD, "arg0")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "arg0"), Out(LPDWORD, "arg1")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "GetLightState", [(D3DLIGHTSTATETYPE, "arg0"), Out(LPDWORD, "arg1")]),
    Method(HRESULT, "SetLightState", [(D3DLIGHTSTATETYPE, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), (LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), Out(LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), Out(LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (LPWORD, "arg4"), (DWORD, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "arg0")]),
    Method(HRESULT, "GetClipStatus", [Out(LPD3DCLIPSTATUS, "arg0")]),
    Method(HRESULT, "DrawPrimitiveStrided", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitiveStrided", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "arg2"), (DWORD, "arg3"), (LPWORD, "arg4"), (DWORD, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "DrawPrimitiveVB", [(D3DPRIMITIVETYPE, "arg0"), (LPDIRECT3DVERTEXBUFFER, "arg1"), (DWORD, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitiveVB", [(D3DPRIMITIVETYPE, "arg0"), (LPDIRECT3DVERTEXBUFFER, "arg1"), (LPWORD, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "ComputeSphereVisibility", [(LPD3DVECTOR, "arg0"), (LPD3DVALUE, "arg1"), (DWORD, "arg2"), (DWORD, "arg3"), (LPDWORD, "arg4")]),
    Method(HRESULT, "GetTexture", [(DWORD, "arg0"), Out(Pointer(LPDIRECT3DTEXTURE2), "arg1")]),
    Method(HRESULT, "SetTexture", [(DWORD, "arg0"), (LPDIRECT3DTEXTURE2, "arg1")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "arg0"), (D3DTEXTURESTAGESTATETYPE, "arg1"), Out(LPDWORD, "arg2")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "arg0"), (D3DTEXTURESTAGESTATETYPE, "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "ValidateDevice", [(LPDWORD, "arg0")]),
]

IDirect3DDevice7.methods += [
    Method(HRESULT, "GetCaps", [Out(LPD3DDEVICEDESC7, "arg0")]),
    Method(HRESULT, "EnumTextureFormats", [(LPD3DENUMPIXELFORMATSCALLBACK, "arg0"), (LPVOID, "arg1")]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [Out(Pointer(LPDIRECT3D7), "arg0")]),
    Method(HRESULT, "SetRenderTarget", [(LPDIRECTDRAWSURFACE7, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "GetRenderTarget", [Out(Pointer(LPDIRECTDRAWSURFACE7), "arg0")]),
    Method(HRESULT, "Clear", [(DWORD, "arg0"), (LPD3DRECT, "arg1"), (DWORD, "arg2"), (D3DCOLOR, "arg3"), (D3DVALUE, "arg4"), (DWORD, "arg5")]),
    Method(HRESULT, "SetTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), (LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "GetTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), Out(LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "SetViewport", [(LPD3DVIEWPORT7, "arg0")]),
    Method(HRESULT, "MultiplyTransform", [(D3DTRANSFORMSTATETYPE, "arg0"), (LPD3DMATRIX, "arg1")]),
    Method(HRESULT, "GetViewport", [Out(LPD3DVIEWPORT7, "arg0")]),
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL7, "arg0")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL7, "arg0")]),
    Method(HRESULT, "SetLight", [(DWORD, "arg0"), (LPD3DLIGHT7, "arg1")]),
    Method(HRESULT, "GetLight", [(DWORD, "arg0"), (LPD3DLIGHT7, "arg1")]),
    Method(HRESULT, "SetRenderState", [(D3DRENDERSTATETYPE, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "GetRenderState", [(D3DRENDERSTATETYPE, "arg0"), Out(LPDWORD, "arg1")]),
    Method(HRESULT, "BeginStateBlock", []),
    Method(HRESULT, "EndStateBlock", [Out(LPDWORD, "arg0")]),
    Method(HRESULT, "PreLoad", [(LPDIRECTDRAWSURFACE7, "arg0")]),
    Method(HRESULT, "DrawPrimitive", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitive", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3"), (LPWORD, "arg4"), (DWORD, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "SetClipStatus", [(LPD3DCLIPSTATUS, "arg0")]),
    Method(HRESULT, "GetClipStatus", [Out(LPD3DCLIPSTATUS, "arg0")]),
    Method(HRESULT, "DrawPrimitiveStrided", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitiveStrided", [(D3DPRIMITIVETYPE, "arg0"), (DWORD, "arg1"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "arg2"), (DWORD, "arg3"), (LPWORD, "arg4"), (DWORD, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "DrawPrimitiveVB", [(D3DPRIMITIVETYPE, "arg0"), (LPDIRECT3DVERTEXBUFFER7, "arg1"), (DWORD, "arg2"), (DWORD, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "DrawIndexedPrimitiveVB", [(D3DPRIMITIVETYPE, "arg0"), (LPDIRECT3DVERTEXBUFFER7, "arg1"), (DWORD, "arg2"), (DWORD, "arg3"), (LPWORD, "arg4"), (DWORD, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "ComputeSphereVisibility", [(LPD3DVECTOR, "arg0"), (LPD3DVALUE, "arg1"), (DWORD, "arg2"), (DWORD, "arg3"), (LPDWORD, "arg4")]),
    Method(HRESULT, "GetTexture", [(DWORD, "arg0"), Out(Pointer(LPDIRECTDRAWSURFACE7), "arg1")]),
    Method(HRESULT, "SetTexture", [(DWORD, "arg0"), (LPDIRECTDRAWSURFACE7, "arg1")]),
    Method(HRESULT, "GetTextureStageState", [(DWORD, "arg0"), (D3DTEXTURESTAGESTATETYPE, "arg1"), Out(LPDWORD, "arg2")]),
    Method(HRESULT, "SetTextureStageState", [(DWORD, "arg0"), (D3DTEXTURESTAGESTATETYPE, "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "ValidateDevice", [Out(LPDWORD, "arg0")]),
    Method(HRESULT, "ApplyStateBlock", [(DWORD, "arg0")]),
    Method(HRESULT, "CaptureStateBlock", [(DWORD, "arg0")]),
    Method(HRESULT, "DeleteStateBlock", [(DWORD, "arg0")]),
    Method(HRESULT, "CreateStateBlock", [(D3DSTATEBLOCKTYPE, "arg0"), Out(LPDWORD, "arg1")]),
    Method(HRESULT, "Load", [(LPDIRECTDRAWSURFACE7, "arg0"), (LPPOINT, "arg1"), (LPDIRECTDRAWSURFACE7, "arg2"), (LPRECT, "arg3"), (DWORD, "arg4")]),
    Method(HRESULT, "LightEnable", [(DWORD, "arg0"), (BOOL, "arg1")]),
    Method(HRESULT, "GetLightEnable", [(DWORD, "arg0"), Out(Pointer(BOOL), "arg1")]),
    Method(HRESULT, "SetClipPlane", [(DWORD, "arg0"), (Pointer(D3DVALUE), "arg1")]),
    Method(HRESULT, "GetClipPlane", [(DWORD, "arg0"), Out(Pointer(D3DVALUE), "arg1")]),
    Method(HRESULT, "GetInfo", [(DWORD, "arg0"), Out(LPVOID, "arg1"), (DWORD, "arg2")]),
]

IDirect3DExecuteBuffer.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3DDEVICE, "arg0"), (LPD3DEXECUTEBUFFERDESC, "arg1")]),
    Method(HRESULT, "Lock", [(LPD3DEXECUTEBUFFERDESC, "arg0")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "SetExecuteData", [(LPD3DEXECUTEDATA, "arg0")]),
    Method(HRESULT, "GetExecuteData", [Out(LPD3DEXECUTEDATA, "arg0")]),
    Method(HRESULT, "Validate", [(LPDWORD, "arg0"), (LPD3DVALIDATECALLBACK, "arg1"), (LPVOID, "arg2"), (DWORD, "arg3")]),
    Method(HRESULT, "Optimize", [(DWORD, "arg0")]),
]

IDirect3DLight.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "arg0")]),
    Method(HRESULT, "SetLight", [(LPD3DLIGHT, "arg0")]),
    Method(HRESULT, "GetLight", [Out(LPD3DLIGHT, "arg0")]),
]

IDirect3DMaterial.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "arg0")]),
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "arg0")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "arg0")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE, "arg0"), Out(LPD3DMATERIALHANDLE, "arg1")]),
    Method(HRESULT, "Reserve", []),
    Method(HRESULT, "Unreserve", []),
]

IDirect3DMaterial2.methods += [
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "arg0")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "arg0")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE2, "arg0"), Out(LPD3DMATERIALHANDLE, "arg1")]),
]

IDirect3DMaterial3.methods += [
    Method(HRESULT, "SetMaterial", [(LPD3DMATERIAL, "arg0")]),
    Method(HRESULT, "GetMaterial", [Out(LPD3DMATERIAL, "arg0")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE3, "arg0"), Out(LPD3DMATERIALHANDLE, "arg1")]),
]

IDirect3DTexture.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3DDEVICE, "arg0"), (LPDIRECTDRAWSURFACE, "arg1")]),
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE, "arg0"), (LPD3DTEXTUREHANDLE, "arg1")]),
    Method(HRESULT, "PaletteChanged", [(DWORD, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "Load", [(LPDIRECT3DTEXTURE, "arg0")]),
    Method(HRESULT, "Unload", []),
]

IDirect3DTexture2.methods += [
    Method(HRESULT, "GetHandle", [(LPDIRECT3DDEVICE2, "arg0"), (LPD3DTEXTUREHANDLE, "arg1")]),
    Method(HRESULT, "PaletteChanged", [(DWORD, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "Load", [(LPDIRECT3DTEXTURE2, "arg0")]),
]

IDirect3DViewport.methods += [
    Method(HRESULT, "Initialize", [(LPDIRECT3D, "arg0")]),
    Method(HRESULT, "GetViewport", [Out(LPD3DVIEWPORT, "arg0")]),
    Method(HRESULT, "SetViewport", [(LPD3DVIEWPORT, "arg0")]),
    Method(HRESULT, "TransformVertices", [(DWORD, "arg0"), (LPD3DTRANSFORMDATA, "arg1"), (DWORD, "arg2"), (LPDWORD, "arg3")]),
    Method(HRESULT, "LightElements", [(DWORD, "arg0"), (LPD3DLIGHTDATA, "arg1")]),
    Method(HRESULT, "SetBackground", [(D3DMATERIALHANDLE, "arg0")]),
    Method(HRESULT, "GetBackground", [Out(LPD3DMATERIALHANDLE, "arg0"), Out(LPBOOL, "arg1")]),
    Method(HRESULT, "SetBackgroundDepth", [(LPDIRECTDRAWSURFACE, "arg0")]),
    Method(HRESULT, "GetBackgroundDepth", [Out(Pointer(LPDIRECTDRAWSURFACE), "arg0"), Out(LPBOOL, "arg1")]),
    Method(HRESULT, "Clear", [(DWORD, "arg0"), (LPD3DRECT, "arg1"), (DWORD, "arg2")]),
    Method(HRESULT, "AddLight", [(LPDIRECT3DLIGHT, "arg0")]),
    Method(HRESULT, "DeleteLight", [(LPDIRECT3DLIGHT, "arg0")]),
    Method(HRESULT, "NextLight", [(LPDIRECT3DLIGHT, "arg0"), Out(Pointer(LPDIRECT3DLIGHT), "arg1"), (DWORD, "arg2")]),
]

IDirect3DViewport2.methods += [
    Method(HRESULT, "GetViewport2", [Out(LPD3DVIEWPORT2, "arg0")]),
    Method(HRESULT, "SetViewport2", [(LPD3DVIEWPORT2, "arg0")]),
]

IDirect3DViewport3.methods += [
    Method(HRESULT, "SetBackgroundDepth2", [(LPDIRECTDRAWSURFACE4, "arg0")]),
    Method(HRESULT, "GetBackgroundDepth2", [Out(Pointer(LPDIRECTDRAWSURFACE4), "arg0"), (LPBOOL, "arg1")]),
    Method(HRESULT, "Clear2", [(DWORD, "arg0"), (LPD3DRECT, "arg1"), (DWORD, "arg2"), (D3DCOLOR, "arg3"), (D3DVALUE, "arg4"), (DWORD, "arg5")]),
]

IDirect3DVertexBuffer.methods += [
    Method(HRESULT, "Lock", [(DWORD, "arg0"), Out(Pointer(LPVOID), "arg1"), (LPDWORD, "arg2")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "ProcessVertices", [(DWORD, "arg0"), (DWORD, "arg1"), (DWORD, "arg2"), (LPDIRECT3DVERTEXBUFFER, "arg3"), (DWORD, "arg4"), (LPDIRECT3DDEVICE3, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "GetVertexBufferDesc", [Out(LPD3DVERTEXBUFFERDESC, "arg0")]),
    Method(HRESULT, "Optimize", [(LPDIRECT3DDEVICE3, "arg0"), (DWORD, "arg1")]),
]

IDirect3DVertexBuffer7.methods += [
    Method(HRESULT, "Lock", [(DWORD, "arg0"), Out(Pointer(LPVOID), "arg1"), (LPDWORD, "arg2")]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "ProcessVertices", [(DWORD, "arg0"), (DWORD, "arg1"), (DWORD, "arg2"), (LPDIRECT3DVERTEXBUFFER7, "arg3"), (DWORD, "arg4"), (LPDIRECT3DDEVICE7, "arg5"), (DWORD, "arg6")]),
    Method(HRESULT, "GetVertexBufferDesc", [Out(LPD3DVERTEXBUFFERDESC, "arg0")]),
    Method(HRESULT, "Optimize", [(LPDIRECT3DDEVICE7, "arg0"), (DWORD, "arg1")]),
    Method(HRESULT, "ProcessVerticesStrided", [(DWORD, "arg0"), (DWORD, "arg1"), (DWORD, "arg2"), (LPD3DDRAWPRIMITIVESTRIDEDDATA, "arg3"), (DWORD, "arg4"), (LPDIRECT3DDEVICE7, "arg5"), (DWORD, "arg6")]),
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

ddraw.add_interfaces(interfaces)


class DDrawTracer(DllTracer):

    def trace_function_impl_body(self, function):
        if function.name in ('AcquireDDThreadLock', 'ReleaseDDThreadLock'):
            self.dispatch_function(function)
            return

        DllTracer.trace_function_impl_body(self, function)

    def wrap_arg(self, function, arg):
        if function.name == 'DirectDrawCreateEx' and arg.name == 'lplpDD':
            print '    if (*lplpDD) {'
            for iface in interfaces:
                print '        if (iid == IID_%s) {' % iface.name
                print '            *lplpDD = (LPVOID) new Wrap%s((%s *)*lplpDD);' % (iface.name, iface.name)
                print '        }'
            print '    }'

        DllTracer.wrap_arg(self, function, arg)


if __name__ == '__main__':
    print '#define INITGUID'
    print '#include <windows.h>'
    print '#include <ddraw.h>'
    print '#include <d3d.h>'
    print
    print '''

#ifndef D3DLIGHT_PARALLELPOINT
#define D3DLIGHT_PARALLELPOINT (D3DLIGHTTYPE)4
#endif

#ifndef D3DLIGHT_GLSPOT
#define D3DLIGHT_GLSPOT (D3DLIGHTTYPE)5
#endif

'''
    print '#include "trace_writer.hpp"'
    print '#include "os.hpp"'
    print
    tracer = DDrawTracer('ddraw.dll')
    tracer.trace_api(ddraw)

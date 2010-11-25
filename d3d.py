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
from d3dtypes import *
from d3dcaps import *

D3DNEXT = Flags(DWORD, [
    "D3DNEXT_NEXT",
    "D3DNEXT_HEAD",
    "D3DNEXT_TAIL",
])

D3DDP = Flags(DWORD, [
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
    Method(HRESULT, "Initialize", [REFCLSID]),
    Method(HRESULT, "EnumDevices", [LPD3DENUMDEVICESCALLBACK,LPVOID]),
    Method(HRESULT, "CreateLight", [OutPointer(LPDIRECT3DLIGHT),LPUNKNOWN]),
    Method(HRESULT, "CreateMaterial", [OutPointer(LPDIRECT3DMATERIAL),LPUNKNOWN]),
    Method(HRESULT, "CreateViewport", [OutPointer(LPDIRECT3DVIEWPORT),LPUNKNOWN]),
    Method(HRESULT, "FindDevice", [LPD3DFINDDEVICESEARCH,LPD3DFINDDEVICERESULT]),
]

IDirect3D2.methods += [
    Method(HRESULT, "EnumDevices", [LPD3DENUMDEVICESCALLBACK,LPVOID]),
    Method(HRESULT, "CreateLight", [OutPointer(LPDIRECT3DLIGHT),LPUNKNOWN]),
    Method(HRESULT, "CreateMaterial", [OutPointer(LPDIRECT3DMATERIAL2),LPUNKNOWN]),
    Method(HRESULT, "CreateViewport", [OutPointer(LPDIRECT3DVIEWPORT2),LPUNKNOWN]),
    Method(HRESULT, "FindDevice", [LPD3DFINDDEVICESEARCH,LPD3DFINDDEVICERESULT]),
    Method(HRESULT, "CreateDevice", [REFCLSID,LPDIRECTDRAWSURFACE,OutPointer(LPDIRECT3DDEVICE2)]),
]

IDirect3D3.methods += [
    Method(HRESULT, "EnumDevices", [LPD3DENUMDEVICESCALLBACK,LPVOID]),
    Method(HRESULT, "CreateLight", [OutPointer(LPDIRECT3DLIGHT),LPUNKNOWN]),
    Method(HRESULT, "CreateMaterial", [OutPointer(LPDIRECT3DMATERIAL3),LPUNKNOWN]),
    Method(HRESULT, "CreateViewport", [OutPointer(LPDIRECT3DVIEWPORT3),LPUNKNOWN]),
    Method(HRESULT, "FindDevice", [LPD3DFINDDEVICESEARCH,LPD3DFINDDEVICERESULT]),
    Method(HRESULT, "CreateDevice", [REFCLSID,LPDIRECTDRAWSURFACE4,OutPointer(LPDIRECT3DDEVICE3),LPUNKNOWN]),
    Method(HRESULT, "CreateVertexBuffer", [LPD3DVERTEXBUFFERDESC,OutPointer(LPDIRECT3DVERTEXBUFFER),DWORD,LPUNKNOWN]),
    Method(HRESULT, "EnumZBufferFormats", [REFCLSID,LPD3DENUMPIXELFORMATSCALLBACK,LPVOID]),
    Method(HRESULT, "EvictManagedTextures", []),
]

IDirect3D7.methods += [
    Method(HRESULT, "EnumDevices", [LPD3DENUMDEVICESCALLBACK7,LPVOID]),
    Method(HRESULT, "CreateDevice", [REFCLSID,LPDIRECTDRAWSURFACE7,OutPointer(LPDIRECT3DDEVICE7)]),
    Method(HRESULT, "CreateVertexBuffer", [LPD3DVERTEXBUFFERDESC,OutPointer(LPDIRECT3DVERTEXBUFFER7),DWORD]),
    Method(HRESULT, "EnumZBufferFormats", [REFCLSID,LPD3DENUMPIXELFORMATSCALLBACK,LPVOID]),
    Method(HRESULT, "EvictManagedTextures", []),
]

IDirect3DDevice.methods += [
    Method(HRESULT, "Initialize", [LPDIRECT3D,LPGUID,LPD3DDEVICEDESC]),
    Method(HRESULT, "GetCaps", [LPD3DDEVICEDESC,LPD3DDEVICEDESC]),
    Method(HRESULT, "SwapTextureHandles", [LPDIRECT3DTEXTURE,LPDIRECT3DTEXTURE]),
    Method(HRESULT, "CreateExecuteBuffer", [LPD3DEXECUTEBUFFERDESC,OutPointer(LPDIRECT3DEXECUTEBUFFER),LPUNKNOWN]),
    Method(HRESULT, "GetStats", [LPD3DSTATS]),
    Method(HRESULT, "Execute", [LPDIRECT3DEXECUTEBUFFER,LPDIRECT3DVIEWPORT,DWORD]),
    Method(HRESULT, "AddViewport", [LPDIRECT3DVIEWPORT]),
    Method(HRESULT, "DeleteViewport", [LPDIRECT3DVIEWPORT]),
    Method(HRESULT, "NextViewport", [LPDIRECT3DVIEWPORT,OutPointer(LPDIRECT3DVIEWPORT),DWORD]),
    Method(HRESULT, "Pick", [LPDIRECT3DEXECUTEBUFFER,LPDIRECT3DVIEWPORT,DWORD,LPD3DRECT]),
    Method(HRESULT, "GetPickRecords", [LPDWORD,LPD3DPICKRECORD]),
    Method(HRESULT, "EnumTextureFormats", [LPD3DENUMTEXTUREFORMATSCALLBACK,LPVOID]),
    Method(HRESULT, "CreateMatrix", [LPD3DMATRIXHANDLE]),
    Method(HRESULT, "SetMatrix", [D3DMATRIXHANDLE,Const(LPD3DMATRIX)]),
    Method(HRESULT, "GetMatrix", [D3DMATRIXHANDLE,LPD3DMATRIX]),
    Method(HRESULT, "DeleteMatrix", [D3DMATRIXHANDLE]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [OutPointer(LPDIRECT3D)]),
]

IDirect3DDevice2.methods += [
    Method(HRESULT, "GetCaps", [LPD3DDEVICEDESC,LPD3DDEVICEDESC]),
    Method(HRESULT, "SwapTextureHandles", [LPDIRECT3DTEXTURE2,LPDIRECT3DTEXTURE2]),
    Method(HRESULT, "GetStats", [LPD3DSTATS]),
    Method(HRESULT, "AddViewport", [LPDIRECT3DVIEWPORT2]),
    Method(HRESULT, "DeleteViewport", [LPDIRECT3DVIEWPORT2]),
    Method(HRESULT, "NextViewport", [LPDIRECT3DVIEWPORT2,OutPointer(LPDIRECT3DVIEWPORT2),DWORD]),
    Method(HRESULT, "EnumTextureFormats", [LPD3DENUMTEXTUREFORMATSCALLBACK,LPVOID]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [OutPointer(LPDIRECT3D2)]),
    Method(HRESULT, "SetCurrentViewport", [LPDIRECT3DVIEWPORT2]),
    Method(HRESULT, "GetCurrentViewport", [OutPointer(LPDIRECT3DVIEWPORT2)]),
    Method(HRESULT, "SetRenderTarget", [LPDIRECTDRAWSURFACE,DWORD]),
    Method(HRESULT, "GetRenderTarget", [OutPointer(LPDIRECTDRAWSURFACE)]),
    Method(HRESULT, "Begin", [D3DPRIMITIVETYPE,D3DVERTEXTYPE,DWORD]),
    Method(HRESULT, "BeginIndexed", [D3DPRIMITIVETYPE,D3DVERTEXTYPE,LPVOID,DWORD,DWORD]),
    Method(HRESULT, "Vertex", [LPVOID]),
    Method(HRESULT, "Index", [WORD]),
    Method(HRESULT, "End", [DWORD]),
    Method(HRESULT, "GetRenderState", [D3DRENDERSTATETYPE,LPDWORD]),
    Method(HRESULT, "SetRenderState", [D3DRENDERSTATETYPE,DWORD]),
    Method(HRESULT, "GetLightState", [D3DLIGHTSTATETYPE,LPDWORD]),
    Method(HRESULT, "SetLightState", [D3DLIGHTSTATETYPE,DWORD]),
    Method(HRESULT, "SetTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "GetTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "MultiplyTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "DrawPrimitive", [D3DPRIMITIVETYPE,D3DVERTEXTYPE,LPVOID,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitive", [D3DPRIMITIVETYPE,D3DVERTEXTYPE,LPVOID,DWORD,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "SetClipStatus", [LPD3DCLIPSTATUS]),
    Method(HRESULT, "GetClipStatus", [LPD3DCLIPSTATUS]),
]

IDirect3DDevice3.methods += [
    Method(HRESULT, "GetCaps", [LPD3DDEVICEDESC,LPD3DDEVICEDESC]),
    Method(HRESULT, "GetStats", [LPD3DSTATS]),
    Method(HRESULT, "AddViewport", [LPDIRECT3DVIEWPORT3]),
    Method(HRESULT, "DeleteViewport", [LPDIRECT3DVIEWPORT3]),
    Method(HRESULT, "NextViewport", [LPDIRECT3DVIEWPORT3,OutPointer(LPDIRECT3DVIEWPORT3),DWORD]),
    Method(HRESULT, "EnumTextureFormats", [LPD3DENUMPIXELFORMATSCALLBACK,LPVOID]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [OutPointer(LPDIRECT3D3)]),
    Method(HRESULT, "SetCurrentViewport", [LPDIRECT3DVIEWPORT3]),
    Method(HRESULT, "GetCurrentViewport", [OutPointer(LPDIRECT3DVIEWPORT3)]),
    Method(HRESULT, "SetRenderTarget", [LPDIRECTDRAWSURFACE4,DWORD]),
    Method(HRESULT, "GetRenderTarget", [OutPointer(LPDIRECTDRAWSURFACE4)]),
    Method(HRESULT, "Begin", [D3DPRIMITIVETYPE,DWORD,DWORD]),
    Method(HRESULT, "BeginIndexed", [D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD]),
    Method(HRESULT, "Vertex", [LPVOID]),
    Method(HRESULT, "Index", [WORD]),
    Method(HRESULT, "End", [DWORD]),
    Method(HRESULT, "GetRenderState", [D3DRENDERSTATETYPE,LPDWORD]),
    Method(HRESULT, "SetRenderState", [D3DRENDERSTATETYPE,DWORD]),
    Method(HRESULT, "GetLightState", [D3DLIGHTSTATETYPE,LPDWORD]),
    Method(HRESULT, "SetLightState", [D3DLIGHTSTATETYPE,DWORD]),
    Method(HRESULT, "SetTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "GetTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "MultiplyTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "DrawPrimitive", [D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitive", [D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "SetClipStatus", [LPD3DCLIPSTATUS]),
    Method(HRESULT, "GetClipStatus", [LPD3DCLIPSTATUS]),
    Method(HRESULT, "DrawPrimitiveStrided", [D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitiveStrided", [D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "DrawPrimitiveVB", [D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER,DWORD,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitiveVB", [D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "ComputeSphereVisibility", [LPD3DVECTOR,LPD3DVALUE,DWORD,DWORD,LPDWORD]),
    Method(HRESULT, "GetTexture", [DWORD,OutPointer(LPDIRECT3DTEXTURE2)]),
    Method(HRESULT, "SetTexture", [DWORD,LPDIRECT3DTEXTURE2]),
    Method(HRESULT, "GetTextureStageState", [DWORD,D3DTEXTURESTAGESTATETYPE,LPDWORD]),
    Method(HRESULT, "SetTextureStageState", [DWORD,D3DTEXTURESTAGESTATETYPE,DWORD]),
    Method(HRESULT, "ValidateDevice", [LPDWORD]),
]

IDirect3DDevice7.methods += [
    Method(HRESULT, "GetCaps", [LPD3DDEVICEDESC7]),
    Method(HRESULT, "EnumTextureFormats", [LPD3DENUMPIXELFORMATSCALLBACK,LPVOID]),
    Method(HRESULT, "BeginScene", []),
    Method(HRESULT, "EndScene", []),
    Method(HRESULT, "GetDirect3D", [OutPointer(LPDIRECT3D7)]),
    Method(HRESULT, "SetRenderTarget", [LPDIRECTDRAWSURFACE7,DWORD]),
    Method(HRESULT, "GetRenderTarget", [OutPointer(LPDIRECTDRAWSURFACE7)]),
    Method(HRESULT, "Clear", [DWORD,LPD3DRECT,DWORD,D3DCOLOR,D3DVALUE,DWORD]),
    Method(HRESULT, "SetTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "GetTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "SetViewport", [LPD3DVIEWPORT7]),
    Method(HRESULT, "MultiplyTransform", [D3DTRANSFORMSTATETYPE,LPD3DMATRIX]),
    Method(HRESULT, "GetViewport", [LPD3DVIEWPORT7]),
    Method(HRESULT, "SetMaterial", [LPD3DMATERIAL7]),
    Method(HRESULT, "GetMaterial", [LPD3DMATERIAL7]),
    Method(HRESULT, "SetLight", [DWORD,LPD3DLIGHT7]),
    Method(HRESULT, "GetLight", [DWORD,LPD3DLIGHT7]),
    Method(HRESULT, "SetRenderState", [D3DRENDERSTATETYPE,DWORD]),
    Method(HRESULT, "GetRenderState", [D3DRENDERSTATETYPE,LPDWORD]),
    Method(HRESULT, "BeginStateBlock", []),
    Method(HRESULT, "EndStateBlock", [LPDWORD]),
    Method(HRESULT, "PreLoad", [LPDIRECTDRAWSURFACE7]),
    Method(HRESULT, "DrawPrimitive", [D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitive", [D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "SetClipStatus", [LPD3DCLIPSTATUS]),
    Method(HRESULT, "GetClipStatus", [LPD3DCLIPSTATUS]),
    Method(HRESULT, "DrawPrimitiveStrided", [D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitiveStrided", [D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "DrawPrimitiveVB", [D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER7,DWORD,DWORD,DWORD]),
    Method(HRESULT, "DrawIndexedPrimitiveVB", [D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER7,DWORD,DWORD,LPWORD,DWORD,DWORD]),
    Method(HRESULT, "ComputeSphereVisibility", [LPD3DVECTOR,LPD3DVALUE,DWORD,DWORD,LPDWORD]),
    Method(HRESULT, "GetTexture", [DWORD,OutPointer(LPDIRECTDRAWSURFACE7)]),
    Method(HRESULT, "SetTexture", [DWORD,LPDIRECTDRAWSURFACE7]),
    Method(HRESULT, "GetTextureStageState", [DWORD,D3DTEXTURESTAGESTATETYPE,LPDWORD]),
    Method(HRESULT, "SetTextureStageState", [DWORD,D3DTEXTURESTAGESTATETYPE,DWORD]),
    Method(HRESULT, "ValidateDevice", [LPDWORD]),
    Method(HRESULT, "ApplyStateBlock", [DWORD]),
    Method(HRESULT, "CaptureStateBlock", [DWORD]),
    Method(HRESULT, "DeleteStateBlock", [DWORD]),
    Method(HRESULT, "CreateStateBlock", [D3DSTATEBLOCKTYPE,LPDWORD]),
    Method(HRESULT, "Load", [LPDIRECTDRAWSURFACE7,LPPOINT,LPDIRECTDRAWSURFACE7,LPRECT,DWORD]),
    Method(HRESULT, "LightEnable", [DWORD,BOOL]),
    Method(HRESULT, "GetLightEnable", [DWORD,OutPointer(BOOL)]),
    Method(HRESULT, "SetClipPlane", [DWORD,OutPointer(D3DVALUE)]),
    Method(HRESULT, "GetClipPlane", [DWORD,OutPointer(D3DVALUE)]),
    Method(HRESULT, "GetInfo", [DWORD,LPVOID,DWORD]),
]

IDirect3DExecuteBuffer.methods += [
    Method(HRESULT, "Initialize", [LPDIRECT3DDEVICE,LPD3DEXECUTEBUFFERDESC]),
    Method(HRESULT, "Lock", [LPD3DEXECUTEBUFFERDESC]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "SetExecuteData", [LPD3DEXECUTEDATA]),
    Method(HRESULT, "GetExecuteData", [LPD3DEXECUTEDATA]),
    Method(HRESULT, "Validate", [LPDWORD,LPD3DVALIDATECALLBACK,LPVOID,DWORD]),
    Method(HRESULT, "Optimize", [DWORD]),
]

IDirect3DLight.methods += [
    Method(HRESULT, "Initialize", [LPDIRECT3D]),
    Method(HRESULT, "SetLight", [LPD3DLIGHT]),
    Method(HRESULT, "GetLight", [LPD3DLIGHT]),
]

IDirect3DMaterial.methods += [
    Method(HRESULT, "Initialize", [LPDIRECT3D]),
    Method(HRESULT, "SetMaterial", [LPD3DMATERIAL]),
    Method(HRESULT, "GetMaterial", [LPD3DMATERIAL]),
    Method(HRESULT, "GetHandle", [LPDIRECT3DDEVICE,LPD3DMATERIALHANDLE]),
    Method(HRESULT, "Reserve", []),
    Method(HRESULT, "Unreserve", []),
]

IDirect3DMaterial2.methods += [
    Method(HRESULT, "SetMaterial", [LPD3DMATERIAL]),
    Method(HRESULT, "GetMaterial", [LPD3DMATERIAL]),
    Method(HRESULT, "GetHandle", [LPDIRECT3DDEVICE2,LPD3DMATERIALHANDLE]),
]

IDirect3DMaterial3.methods += [
    Method(HRESULT, "SetMaterial", [LPD3DMATERIAL]),
    Method(HRESULT, "GetMaterial", [LPD3DMATERIAL]),
    Method(HRESULT, "GetHandle", [LPDIRECT3DDEVICE3,LPD3DMATERIALHANDLE]),
]

IDirect3DTexture.methods += [
    Method(HRESULT, "Initialize", [LPDIRECT3DDEVICE,LPDIRECTDRAWSURFACE]),
    Method(HRESULT, "GetHandle", [LPDIRECT3DDEVICE,LPD3DTEXTUREHANDLE]),
    Method(HRESULT, "PaletteChanged", [DWORD,DWORD]),
    Method(HRESULT, "Load", [LPDIRECT3DTEXTURE]),
    Method(HRESULT, "Unload", []),
]

IDirect3DTexture2.methods += [
    Method(HRESULT, "GetHandle", [LPDIRECT3DDEVICE2,LPD3DTEXTUREHANDLE]),
    Method(HRESULT, "PaletteChanged", [DWORD,DWORD]),
    Method(HRESULT, "Load", [LPDIRECT3DTEXTURE2]),
]

IDirect3DViewport.methods += [
    Method(HRESULT, "Initialize", [LPDIRECT3D]),
    Method(HRESULT, "GetViewport", [LPD3DVIEWPORT]),
    Method(HRESULT, "SetViewport", [LPD3DVIEWPORT]),
    Method(HRESULT, "TransformVertices", [DWORD,LPD3DTRANSFORMDATA,DWORD,LPDWORD]),
    Method(HRESULT, "LightElements", [DWORD,LPD3DLIGHTDATA]),
    Method(HRESULT, "SetBackground", [D3DMATERIALHANDLE]),
    Method(HRESULT, "GetBackground", [LPD3DMATERIALHANDLE,LPBOOL]),
    Method(HRESULT, "SetBackgroundDepth", [LPDIRECTDRAWSURFACE]),
    Method(HRESULT, "GetBackgroundDepth", [OutPointer(LPDIRECTDRAWSURFACE),LPBOOL]),
    Method(HRESULT, "Clear", [DWORD,LPD3DRECT,DWORD]),
    Method(HRESULT, "AddLight", [LPDIRECT3DLIGHT]),
    Method(HRESULT, "DeleteLight", [LPDIRECT3DLIGHT]),
    Method(HRESULT, "NextLight", [LPDIRECT3DLIGHT,OutPointer(LPDIRECT3DLIGHT),DWORD]),
]

IDirect3DViewport2.methods += [
    Method(HRESULT, "GetViewport2", [LPD3DVIEWPORT2]),
    Method(HRESULT, "SetViewport2", [LPD3DVIEWPORT2]),
]

IDirect3DViewport3.methods += [
    Method(HRESULT, "SetBackgroundDepth2", [LPDIRECTDRAWSURFACE4]),
    Method(HRESULT, "GetBackgroundDepth2", [OutPointer(LPDIRECTDRAWSURFACE4),LPBOOL]),
    Method(HRESULT, "Clear2", [DWORD,LPD3DRECT,DWORD,D3DCOLOR,D3DVALUE,DWORD]),
]

IDirect3DVertexBuffer.methods += [
    Method(HRESULT, "Lock", [DWORD,OutPointer(LPVOID),LPDWORD]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "ProcessVertices", [DWORD,DWORD,DWORD,LPDIRECT3DVERTEXBUFFER,DWORD,LPDIRECT3DDEVICE3,DWORD]),
    Method(HRESULT, "GetVertexBufferDesc", [LPD3DVERTEXBUFFERDESC]),
    Method(HRESULT, "Optimize", [LPDIRECT3DDEVICE3,DWORD]),
]

IDirect3DVertexBuffer7.methods += [
    Method(HRESULT, "Lock", [DWORD,OutPointer(LPVOID),LPDWORD]),
    Method(HRESULT, "Unlock", []),
    Method(HRESULT, "ProcessVertices", [DWORD,DWORD,DWORD,LPDIRECT3DVERTEXBUFFER7,DWORD,LPDIRECT3DDEVICE7,DWORD]),
    Method(HRESULT, "GetVertexBufferDesc", [LPD3DVERTEXBUFFERDESC]),
    Method(HRESULT, "Optimize", [LPDIRECT3DDEVICE7,DWORD]),
    Method(HRESULT, "ProcessVerticesStrided", [DWORD,DWORD,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,LPDIRECT3DDEVICE7,DWORD]),
]

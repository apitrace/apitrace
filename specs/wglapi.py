##########################################################################
#
# Copyright 2008-2010 VMware, Inc.
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


"""WGL API description"""


from glapi import *
from winapi import *
from wglenum import *


wglapi = Module("WGL")


HGLRC = Alias("HGLRC", HANDLE)
PROC = Opaque("PROC")

PFD = Flags(DWORD, [
    "PFD_DOUBLEBUFFER",
    "PFD_STEREO",
    "PFD_DRAW_TO_WINDOW",
    "PFD_DRAW_TO_BITMAP",
    "PFD_SUPPORT_GDI",
    "PFD_SUPPORT_OPENGL",
    "PFD_GENERIC_FORMAT",
    "PFD_NEED_PALETTE",
    "PFD_NEED_SYSTEM_PALETTE",
    "PFD_SWAP_EXCHANGE",
    "PFD_SWAP_COPY",
    "PFD_SWAP_LAYER_BUFFERS",
    "PFD_GENERIC_ACCELERATED",
    "PFD_SUPPORT_DIRECTDRAW",
    "PFD_SUPPORT_COMPOSITION",
    "PFD_DEPTH_DONTCARE",
    "PFD_DOUBLEBUFFER_DONTCARE",
    "PFD_STEREO_DONTCARE",
])

PIXELFORMATDESCRIPTOR = Struct("PIXELFORMATDESCRIPTOR", [
    (WORD, "nSize"),
    (WORD, "nVersion"),
    (PFD, "dwFlags"),
    (BYTE, "iPixelType"),
    (BYTE, "cColorBits"),
    (BYTE, "cRedBits"),
    (BYTE, "cRedShift"),
    (BYTE, "cGreenBits"),
    (BYTE, "cGreenShift"),
    (BYTE, "cBlueBits"),
    (BYTE, "cBlueShift"),
    (BYTE, "cAlphaBits"),
    (BYTE, "cAlphaShift"),
    (BYTE, "cAccumBits"),
    (BYTE, "cAccumRedBits"),
    (BYTE, "cAccumGreenBits"),
    (BYTE, "cAccumBlueBits"),
    (BYTE, "cAccumAlphaBits"),
    (BYTE, "cDepthBits"),
    (BYTE, "cStencilBits"),
    (BYTE, "cAuxBuffers"),
    (BYTE, "iLayerType"),
    (BYTE, "bReserved"),
    (DWORD, "dwLayerMask"),
    (DWORD, "dwVisibleMask"),
    (DWORD, "dwDamageMask"),
])

POINTFLOAT = Struct("POINTFLOAT", [
    (FLOAT, "x"),
    (FLOAT, "y"),
])

GLYPHMETRICSFLOAT = Struct("GLYPHMETRICSFLOAT", [
    (FLOAT, "gmfBlackBoxX"),
    (FLOAT, "gmfBlackBoxY"),
    (POINTFLOAT, "gmfptGlyphOrigin"),
    (FLOAT, "gmfCellIncX"),
    (FLOAT, "gmfCellIncY"),
])
LPGLYPHMETRICSFLOAT = Pointer(GLYPHMETRICSFLOAT)

COLORREF = Alias("COLORREF", DWORD)


LAYERPLANEDESCRIPTOR = Struct("LAYERPLANEDESCRIPTOR", [
    (WORD, "nSize"),
    (WORD, "nVersion"),
    (DWORD, "dwFlags"),
    (BYTE, "iPixelType"),
    (BYTE, "cColorBits"),
    (BYTE, "cRedBits"),
    (BYTE, "cRedShift"),
    (BYTE, "cGreenBits"),
    (BYTE, "cGreenShift"),
    (BYTE, "cBlueBits"),
    (BYTE, "cBlueShift"),
    (BYTE, "cAlphaBits"),
    (BYTE, "cAlphaShift"),
    (BYTE, "cAccumBits"),
    (BYTE, "cAccumRedBits"),
    (BYTE, "cAccumGreenBits"),
    (BYTE, "cAccumBlueBits"),
    (BYTE, "cAccumAlphaBits"),
    (BYTE, "cDepthBits"),
    (BYTE, "cStencilBits"),
    (BYTE, "cAuxBuffers"),
    (BYTE, "iLayerPlane"),
    (BYTE, "bReserved"),
    (COLORREF, "crTransparent"),
])
LPLAYERPLANEDESCRIPTOR = Pointer(LAYERPLANEDESCRIPTOR)

WGLSWAP = Struct("WGLSWAP", [
    (HDC, "hdc"),
    (UINT, "uiFlags"),
])

WGLContextAttribs = AttribArray(Const(WGLenum), [
    ('WGL_CONTEXT_MAJOR_VERSION_ARB', Int),
    ('WGL_CONTEXT_MINOR_VERSION_ARB', Int),
    ('WGL_CONTEXT_LAYER_PLANE_ARB', Int),
    ('WGL_CONTEXT_FLAGS_ARB', Flags(Int, [
        "WGL_CONTEXT_DEBUG_BIT_ARB",
        "WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB",
        "WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB",
    ])),
    ('WGL_CONTEXT_PROFILE_MASK_ARB', Flags(Int, [
        "WGL_CONTEXT_CORE_PROFILE_BIT_ARB",
        "WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB",
        "WGL_CONTEXT_ES_PROFILE_BIT_EXT",
    ]))
])

WGLPixelFormatAttribsList = [
    ('WGL_DRAW_TO_WINDOW_ARB', BOOL),
    ('WGL_DRAW_TO_BITMAP_ARB', BOOL),
    ('WGL_ACCELERATION_ARB', FakeEnum(Int, ['WGL_NO_ACCELERATION_ARB', 'WGL_GENERIC_ACCELERATION_ARB', 'WGL_FULL_ACCELERATION_ARB'])),
    ('WGL_NEED_PALETTE_ARB', BOOL),
    ('WGL_NEED_SYSTEM_PALETTE_ARB', BOOL),
    ('WGL_SWAP_LAYER_BUFFERS_ARB', BOOL),
    ('WGL_SWAP_METHOD_ARB', FakeEnum(Int, ['WGL_SWAP_EXCHANGE_ARB', 'WGL_SWAP_COPY_ARB', 'WGL_SWAP_UNDEFINED_ARB'])),
    ('WGL_NUMBER_OVERLAYS_ARB', Int),
    ('WGL_NUMBER_UNDERLAYS_ARB', Int),
    ('WGL_TRANSPARENT_EXT', BOOL),
    ('WGL_TRANSPARENT_VALUE_EXT', Int),
    ('WGL_SHARE_DEPTH_ARB', BOOL),
    ('WGL_SHARE_STENCIL_ARB', BOOL),
    ('WGL_SHARE_ACCUM_ARB', BOOL),
    ('WGL_SUPPORT_GDI_ARB', BOOL),
    ('WGL_SUPPORT_OPENGL_ARB', BOOL),
    ('WGL_DOUBLE_BUFFER_ARB', BOOL),
    ('WGL_STEREO_ARB', BOOL),
    ('WGL_PIXEL_TYPE_ARB', FakeEnum(Int, ['WGL_TYPE_RGBA_ARB', 'WGL_TYPE_COLORINDEX_ARB'])),
    ('WGL_COLOR_BITS_ARB', Int),
    ('WGL_RED_BITS_ARB', Int),
    ('WGL_RED_SHIFT_EXT', Int),
    ('WGL_GREEN_BITS_ARB', Int),
    ('WGL_GREEN_SHIFT_EXT', Int),
    ('WGL_BLUE_BITS_ARB', Int),
    ('WGL_BLUE_SHIFT_EXT', Int),
    ('WGL_ALPHA_BITS_ARB', Int),
    ('WGL_ALPHA_SHIFT_EXT', Int),
    ('WGL_ACCUM_BITS_ARB', Int),
    ('WGL_ACCUM_RED_BITS_ARB', Int),
    ('WGL_ACCUM_GREEN_BITS_ARB', Int),
    ('WGL_ACCUM_BLUE_BITS_ARB', Int),
    ('WGL_ACCUM_ALPHA_BITS_ARB', Int),
    ('WGL_DEPTH_BITS_ARB', Int),
    ('WGL_STENCIL_BITS_ARB', Int),
    ('WGL_AUX_BUFFERS_ARB', Int),
    ('WGL_SAMPLE_BUFFERS_ARB', Int),
    ('WGL_SAMPLES_ARB', Int),
]

WGLPixelFormatAttribs = AttribArray(Const(WGLenum), WGLPixelFormatAttribsList)
WGLPixelFormatFloatAttribs = AttribArray(Const(FLOAT), WGLPixelFormatAttribsList)

WGLCreatePbufferARBAttribs = AttribArray(Const(WGLenum), [
    ('WGL_PBUFFER_LARGEST_ARB', Int)
])

CubeFaceEnum = FakeEnum(Int, [
    'WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB',
    'WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB',
    'WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB',
    'WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB',
    'WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB',
    'WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB'
])

WGLSetPbufferARBAttribs = AttribArray(Const(WGLenum), [
    ('WGL_MIPMAP_LEVEL_ARB', Int),
    ('WGL_CUBE_MAP_FACE_ARB', CubeFaceEnum)
])

HPBUFFERARB = Alias("HPBUFFERARB", HANDLE)
HPBUFFEREXT = Alias("HPBUFFEREXT", HANDLE)
HPVIDEODEV = Alias("HPVIDEODEV", HANDLE)
HVIDEOOUTPUTDEVICENV = Alias("HVIDEOOUTPUTDEVICENV", HANDLE)
HVIDEOINPUTDEVICENV = Alias("HVIDEOINPUTDEVICENV", HANDLE)
HGPUNV = Alias("HGPUNV", HANDLE)

GPU_DEVICE = Struct("_GPU_DEVICE", [
    (DWORD, "cb"),
    (CString, "DeviceName"),
    (CString, "DeviceString"),
    (DWORD, "Flags"),
    (RECT, "rcVirtualScreen"),
])


wglapi.addFunctions([
    # WGL
    StdFunction(HGLRC, "wglCreateContext", [(HDC, "hdc")]),
    StdFunction(BOOL, "wglDeleteContext", [(HGLRC, "hglrc")]),
    StdFunction(HGLRC, "wglGetCurrentContext", [], sideeffects=False),
    StdFunction(BOOL, "wglMakeCurrent", [(HDC, "hdc"), (HGLRC, "hglrc")]),
    StdFunction(BOOL, "wglCopyContext", [(HGLRC, "hglrcSrc"), (HGLRC, "hglrcDst"), (UINT, "mask")]),
    StdFunction(Int, "wglChoosePixelFormat", [(HDC, "hdc"), (Pointer(Const(PIXELFORMATDESCRIPTOR)), "ppfd")]), 
    StdFunction(Int, "wglDescribePixelFormat", [(HDC, "hdc"), (Int, "iPixelFormat"), (UINT, "nBytes"), Out(Pointer(PIXELFORMATDESCRIPTOR), "ppfd")]),
    StdFunction(HDC, "wglGetCurrentDC", [], sideeffects=False),
    StdFunction(PROC, "wglGetDefaultProcAddress", [(LPCSTR, "lpszProc")], sideeffects=False),
    StdFunction(Int, "wglGetPixelFormat", [(HDC, "hdc")], sideeffects=False),
    StdFunction(BOOL, "wglSetPixelFormat", [(HDC, "hdc"), (Int, "iPixelFormat"), (Pointer(Const(PIXELFORMATDESCRIPTOR)), "ppfd")]),
    StdFunction(BOOL, "wglSwapBuffers", [(HDC, "hdc")]),
    StdFunction(BOOL, "wglShareLists", [(HGLRC, "hglrc1"), (HGLRC, "hglrc2")]),
    StdFunction(HGLRC, "wglCreateLayerContext", [(HDC, "hdc"), (Int, "iLayerPlane")]),
    StdFunction(BOOL, "wglDescribeLayerPlane", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nBytes"), Out(Pointer(LAYERPLANEDESCRIPTOR), "plpd")]),
    StdFunction(Int, "wglSetLayerPaletteEntries", [(HDC, "hdc"), (Int, "iLayerPlane"), (Int, "iStart"), (Int, "cEntries"), (Array(Const(COLORREF), "cEntries"), "pcr")]),
    StdFunction(Int, "wglGetLayerPaletteEntries", [(HDC, "hdc"), (Int, "iLayerPlane"), (Int, "iStart"), (Int, "cEntries"), Out(Array(COLORREF, "cEntries"), "pcr")], sideeffects=False),
    StdFunction(BOOL, "wglRealizeLayerPalette", [(HDC, "hdc"), (Int, "iLayerPlane"), (BOOL, "bRealize")]),
    StdFunction(BOOL, "wglSwapLayerBuffers", [(HDC, "hdc"), (UINT, "fuPlanes")]),
    StdFunction(BOOL, "wglUseFontBitmapsA", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase")]),
    StdFunction(BOOL, "wglUseFontBitmapsW", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase")]),
    StdFunction(DWORD, "wglSwapMultipleBuffers", [(UINT, "n"), (Array(Const(WGLSWAP), "n"), "ps")]),
    StdFunction(BOOL, "wglUseFontOutlinesA", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase"), (FLOAT, "deviation"), (FLOAT, "extrusion"), (Int, "format"), (LPGLYPHMETRICSFLOAT, "lpgmf")]),
    StdFunction(BOOL, "wglUseFontOutlinesW", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase"), (FLOAT, "deviation"), (FLOAT, "extrusion"), (Int, "format"), (LPGLYPHMETRICSFLOAT, "lpgmf")]),

    # WGL_ARB_buffer_region
    StdFunction(HANDLE, "wglCreateBufferRegionARB", [(HDC, "hDC"), (Int, "iLayerPlane"), (UINT, "uType")]),
    StdFunction(VOID, "wglDeleteBufferRegionARB", [(HANDLE, "hRegion")]),
    StdFunction(BOOL, "wglSaveBufferRegionARB", [(HANDLE, "hRegion"), (Int, "x"), (Int, "y"), (Int, "width"), (Int, "height")]),
    StdFunction(BOOL, "wglRestoreBufferRegionARB", [(HANDLE, "hRegion"), (Int, "x"), (Int, "y"), (Int, "width"), (Int, "height"), (Int, "xSrc"), (Int, "ySrc")]),

    # WGL_ARB_extensions_string
    StdFunction(ConstCString, "wglGetExtensionsStringARB", [(HDC, "hdc")], sideeffects=False),

    # WGL_ARB_pixel_format
    StdFunction(BOOL, "wglGetPixelFormatAttribivARB", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(Int, "nAttributes"), "piValues")], sideeffects=False),
    StdFunction(BOOL, "wglGetPixelFormatAttribfvARB", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(FLOAT, "nAttributes"), "pfValues")], sideeffects=False),
    StdFunction(BOOL, "wglChoosePixelFormatARB", [(HDC, "hdc"), (WGLPixelFormatAttribs, "piAttribIList"), (WGLPixelFormatFloatAttribs, "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "(*nNumFormats)"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),

    # WGL_ARB_make_current_read
    StdFunction(BOOL, "wglMakeContextCurrentARB", [(HDC, "hDrawDC"), (HDC, "hReadDC"), (HGLRC, "hglrc")]),
    StdFunction(HDC, "wglGetCurrentReadDCARB", [], sideeffects=False),

    # WGL_ARB_pbuffer
    StdFunction(HPBUFFERARB, "wglCreatePbufferARB", [(HDC, "hDC"), (Int, "iPixelFormat"), (Int, "iWidth"), (Int, "iHeight"), (WGLCreatePbufferARBAttribs, "piAttribList")]),
    StdFunction(HDC, "wglGetPbufferDCARB", [(HPBUFFERARB, "hPbuffer")]),
    StdFunction(Int, "wglReleasePbufferDCARB", [(HPBUFFERARB, "hPbuffer"), (HDC, "hDC")]),
    StdFunction(BOOL, "wglDestroyPbufferARB", [(HPBUFFERARB, "hPbuffer")]),
    StdFunction(BOOL, "wglQueryPbufferARB", [(HPBUFFERARB, "hPbuffer"), (WGLenum, "iAttribute"), Out(Pointer(Int), "piValue")], sideeffects=False),

    # WGL_ARB_render_texture
    StdFunction(BOOL, "wglBindTexImageARB", [(HPBUFFERARB, "hPbuffer"), (Int, "iBuffer")]),
    StdFunction(BOOL, "wglReleaseTexImageARB", [(HPBUFFERARB, "hPbuffer"), (Int, "iBuffer")]),
    StdFunction(BOOL, "wglSetPbufferAttribARB", [(HPBUFFERARB, "hPbuffer"), (WGLSetPbufferARBAttribs, "piAttribList")]),

    # WGL_ARB_create_context
    StdFunction(HGLRC, "wglCreateContextAttribsARB", [(HDC, "hDC"), (HGLRC, "hShareContext"), (WGLContextAttribs, "attribList")]),

    # WGL_EXT_display_color_table
    StdFunction(GLboolean, "wglCreateDisplayColorTableEXT", [(GLushort, "id")]),
    StdFunction(GLboolean, "wglLoadDisplayColorTableEXT", [(Array(Const(GLushort), "length"), "table"), (GLuint, "length")]),
    StdFunction(GLboolean, "wglBindDisplayColorTableEXT", [(GLushort, "id")]),
    StdFunction(VOID, "wglDestroyDisplayColorTableEXT", [(GLushort, "id")]),

    # WGL_EXT_extensions_string
    StdFunction(ConstCString, "wglGetExtensionsStringEXT", [], sideeffects=False),

    # WGL_EXT_make_current_read
    StdFunction(BOOL, "wglMakeContextCurrentEXT", [(HDC, "hDrawDC"), (HDC, "hReadDC"), (HGLRC, "hglrc")]),
    StdFunction(HDC, "wglGetCurrentReadDCEXT", [], sideeffects=False),

    # WGL_EXT_pbuffer
    StdFunction(HPBUFFEREXT, "wglCreatePbufferEXT", [(HDC, "hDC"), (Int, "iPixelFormat"), (Int, "iWidth"), (Int, "iHeight"), (WGLCreatePbufferARBAttribs, "piAttribList")]),
    StdFunction(HDC, "wglGetPbufferDCEXT", [(HPBUFFEREXT, "hPbuffer")], sideeffects=False),
    StdFunction(Int, "wglReleasePbufferDCEXT", [(HPBUFFEREXT, "hPbuffer"), (HDC, "hDC")]),
    StdFunction(BOOL, "wglDestroyPbufferEXT", [(HPBUFFEREXT, "hPbuffer")]),
    StdFunction(BOOL, "wglQueryPbufferEXT", [(HPBUFFEREXT, "hPbuffer"), (WGLenum, "iAttribute"), Out(Pointer(Int), "piValue")], sideeffects=False),

    # WGL_EXT_pixel_format
    StdFunction(BOOL, "wglGetPixelFormatAttribivEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(Int, "nAttributes"), "piValues")], sideeffects=False),
    StdFunction(BOOL, "wglGetPixelFormatAttribfvEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(FLOAT, "nAttributes"), "pfValues")], sideeffects=False),
    StdFunction(BOOL, "wglChoosePixelFormatEXT", [(HDC, "hdc"), (WGLPixelFormatAttribs, "piAttribIList"), (WGLPixelFormatFloatAttribs, "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "*nNumFormats"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),

    # WGL_EXT_swap_control
    StdFunction(BOOL, "wglSwapIntervalEXT", [(Int, "interval")]),
    StdFunction(Int, "wglGetSwapIntervalEXT", [], sideeffects=False),

    # WGL_NV_vertex_array_range
    StdFunction(OpaquePointer(Void), "wglAllocateMemoryNV", [(GLsizei, "size"), (GLfloat, "readfreq"), (GLfloat, "writefreq"), (GLfloat, "priority")]),
    StdFunction(Void, "wglFreeMemoryNV", [(OpaquePointer(Void), "pointer")]),

    # WGL_OML_sync_control
    StdFunction(BOOL, "wglGetSyncValuesOML", [(HDC, "hdc"), Out(Pointer(INT64), "ust"), Out(Pointer(INT64), "msc"), Out(Pointer(INT64), "sbc")], sideeffects=False),
    StdFunction(BOOL, "wglGetMscRateOML", [(HDC, "hdc"), Out(Pointer(INT32), "numerator"), Out(Pointer(INT32), "denominator")], sideeffects=False),
    StdFunction(INT64, "wglSwapBuffersMscOML", [(HDC, "hdc"), (INT64, "target_msc"), (INT64, "divisor"), (INT64, "remainder")]),
    StdFunction(INT64, "wglSwapLayerBuffersMscOML", [(HDC, "hdc"), (Int, "fuPlanes"), (INT64, "target_msc"), (INT64, "divisor"), (INT64, "remainder")]),
    StdFunction(BOOL, "wglWaitForMscOML", [(HDC, "hdc"), (INT64, "target_msc"), (INT64, "divisor"), (INT64, "remainder"), Out(Pointer(INT64), "ust"), Out(Pointer(INT64), "msc"), Out(Pointer(INT64), "sbc")]),
    StdFunction(BOOL, "wglWaitForSbcOML", [(HDC, "hdc"), (INT64, "target_sbc"), Out(Pointer(INT64), "ust"), Out(Pointer(INT64), "msc"), Out(Pointer(INT64), "sbc")]),

    # WGL_I3D_digital_video_control
    StdFunction(BOOL, "wglGetDigitalVideoParametersI3D", [(HDC, "hDC"), (Int, "iAttribute"), Out(OpaqueArray(Int, "_wglGetDigitalVideoParametersI3D_size(iAttribute)"), "piValue")], sideeffects=False),
    StdFunction(BOOL, "wglSetDigitalVideoParametersI3D", [(HDC, "hDC"), (Int, "iAttribute"), (OpaqueArray(Const(Int), "_wglSetDigitalVideoParametersI3D_size(iAttribute)"), "piValue")]),

    # WGL_I3D_gamma
    StdFunction(BOOL, "wglGetGammaTableParametersI3D", [(HDC, "hDC"), (Int, "iAttribute"), Out(OpaqueArray(Int, "_wglGetGammaTableParametersI3D_size(iAttribute)"), "piValue")], sideeffects=False),
    StdFunction(BOOL, "wglSetGammaTableParametersI3D", [(HDC, "hDC"), (Int, "iAttribute"), (OpaqueArray(Const(Int), "_wglSetGammaTableParametersI3D_size(iAttribute)"), "piValue")]),
    StdFunction(BOOL, "wglGetGammaTableI3D", [(HDC, "hDC"), (Int, "iEntries"), Out(Array(USHORT, "iEntries"), "puRed"), Out(Array(USHORT, "iEntries"), "puGreen"), Out(Array(USHORT, "iEntries"), "puBlue")], sideeffects=False),
    StdFunction(BOOL, "wglSetGammaTableI3D", [(HDC, "hDC"), (Int, "iEntries"), (Array(Const(USHORT), "iEntries"), "puRed"), (Array(Const(USHORT), "iEntries"), "puGreen"), (Array(Const(USHORT), "iEntries"), "puBlue")]),

    # WGL_I3D_genlock
    StdFunction(BOOL, "wglEnableGenlockI3D", [(HDC, "hDC")]),
    StdFunction(BOOL, "wglDisableGenlockI3D", [(HDC, "hDC")]),
    StdFunction(BOOL, "wglIsEnabledGenlockI3D", [(HDC, "hDC"), Out(Pointer(BOOL), "pFlag")], sideeffects=False),
    StdFunction(BOOL, "wglGenlockSourceI3D", [(HDC, "hDC"), (UINT, "uSource")]),
    StdFunction(BOOL, "wglGetGenlockSourceI3D", [(HDC, "hDC"), Out(Pointer(UINT), "uSource")], sideeffects=False),
    StdFunction(BOOL, "wglGenlockSourceEdgeI3D", [(HDC, "hDC"), (UINT, "uEdge")]),
    StdFunction(BOOL, "wglGetGenlockSourceEdgeI3D", [(HDC, "hDC"), Out(Pointer(UINT), "uEdge")], sideeffects=False),
    StdFunction(BOOL, "wglGenlockSampleRateI3D", [(HDC, "hDC"), (UINT, "uRate")]),
    StdFunction(BOOL, "wglGetGenlockSampleRateI3D", [(HDC, "hDC"), Out(Pointer(UINT), "uRate")], sideeffects=False),
    StdFunction(BOOL, "wglGenlockSourceDelayI3D", [(HDC, "hDC"), (UINT, "uDelay")]),
    StdFunction(BOOL, "wglGetGenlockSourceDelayI3D", [(HDC, "hDC"), Out(Pointer(UINT), "uDelay")], sideeffects=False),
    StdFunction(BOOL, "wglQueryGenlockMaxSourceDelayI3D", [(HDC, "hDC"), Out(Pointer(UINT), "uMaxLineDelay"), Out(Pointer(UINT), "uMaxPixelDelay")]),

    # WGL_I3D_image_buffer
    StdFunction(LPVOID, "wglCreateImageBufferI3D", [(HDC, "hDC"), (DWORD, "dwSize"), (UINT, "uFlags")]),
    StdFunction(BOOL, "wglDestroyImageBufferI3D", [(HDC, "hDC"), (LPVOID, "pAddress")]),
    StdFunction(BOOL, "wglAssociateImageBufferEventsI3D", [(HDC, "hDC"), (Array(Const(HANDLE), "count"), "pEvent"), (Array(Const(LPVOID), "count"), "pAddress"), (Array(Const(DWORD), "count"), "pSize"), (UINT, "count")]),
    StdFunction(BOOL, "wglReleaseImageBufferEventsI3D", [(HDC, "hDC"), (Array(Const(LPVOID), "count"), "pAddress"), (UINT, "count")]),

    # WGL_I3D_swap_frame_lock
    StdFunction(BOOL, "wglEnableFrameLockI3D", []),
    StdFunction(BOOL, "wglDisableFrameLockI3D", []),
    StdFunction(BOOL, "wglIsEnabledFrameLockI3D", [Out(Pointer(BOOL), "pFlag")], sideeffects=False),
    StdFunction(BOOL, "wglQueryFrameLockMasterI3D", [Out(Pointer(BOOL), "pFlag")]),

    # WGL_I3D_swap_frame_usage
    StdFunction(BOOL, "wglGetFrameUsageI3D", [Out(Pointer(Float), "pUsage")], sideeffects=False),
    StdFunction(BOOL, "wglBeginFrameTrackingI3D", []),
    StdFunction(BOOL, "wglEndFrameTrackingI3D", []),
    StdFunction(BOOL, "wglQueryFrameTrackingI3D", [Out(Pointer(DWORD), "pFrameCount"), Out(Pointer(DWORD), "pMissedFrames"), Out(Pointer(Float), "pLastMissedUsage")]),

    # WGL_3DL_stereo_control
    StdFunction(BOOL, "wglSetStereoEmitterState3DL", [(HDC, "hDC"), (UINT, "uState")]),

    # WGL_NV_present_video
    StdFunction(Int, "wglEnumerateVideoDevicesNV", [(HDC, "hDC"), Out(OpaquePointer(HVIDEOOUTPUTDEVICENV), "phDeviceList")]),
    StdFunction(BOOL, "wglBindVideoDeviceNV", [(HDC, "hDC"), (UInt, "uVideoSlot"), (HVIDEOOUTPUTDEVICENV, "hVideoDevice"), (OpaqueArray(Const(Int), "_wglBindVideoDeviceNV_size()"), "piAttribList")]),
    StdFunction(BOOL, "wglQueryCurrentContextNV", [(Int, "iAttribute"), Out(OpaqueArray(Int, "_wglQueryCurrentContextNV_size()"), "piValue")]),

    # WGL_NV_video_output
    StdFunction(BOOL, "wglGetVideoDeviceNV", [(HDC, "hDC"), (Int, "numDevices"), Out(Pointer(HPVIDEODEV), "hVideoDevice")], sideeffects=False),
    StdFunction(BOOL, "wglReleaseVideoDeviceNV", [(HPVIDEODEV, "hVideoDevice")]),
    StdFunction(BOOL, "wglBindVideoImageNV", [(HPVIDEODEV, "hVideoDevice"), (HPBUFFERARB, "hPbuffer"), (Int, "iVideoBuffer")]),
    StdFunction(BOOL, "wglReleaseVideoImageNV", [(HPBUFFERARB, "hPbuffer"), (Int, "iVideoBuffer")]),
    StdFunction(BOOL, "wglSendPbufferToVideoNV", [(HPBUFFERARB, "hPbuffer"), (Int, "iBufferType"), Out(Pointer(ULong), "pulCounterPbuffer"), (BOOL, "bBlock")]),
    StdFunction(BOOL, "wglGetVideoInfoNV", [(HPVIDEODEV, "hpVideoDevice"), Out(Pointer(ULong), "pulCounterOutputPbuffer"), Out(Pointer(ULong), "pulCounterOutputVideo")], sideeffects=False),

    # WGL_NV_swap_group
    StdFunction(BOOL, "wglJoinSwapGroupNV", [(HDC, "hDC"), (GLuint, "group")]),
    StdFunction(BOOL, "wglBindSwapBarrierNV", [(GLuint, "group"), (GLuint, "barrier")]),
    StdFunction(BOOL, "wglQuerySwapGroupNV", [(HDC, "hDC"), Out(Pointer(GLuint), "group"), Out(Pointer(GLuint), "barrier")]),
    StdFunction(BOOL, "wglQueryMaxSwapGroupsNV", [(HDC, "hDC"), Out(Pointer(GLuint), "maxGroups"), Out(Pointer(GLuint), "maxBarriers")]),
    StdFunction(BOOL, "wglQueryFrameCountNV", [(HDC, "hDC"), Out(Pointer(GLuint), "count")]),
    StdFunction(BOOL, "wglResetFrameCountNV", [(HDC, "hDC")]),

    # WGL_NV_gpu_affinity
    StdFunction(BOOL, "wglEnumGpusNV", [(UINT, "iGpuIndex"), Out(Pointer(HGPUNV), "phGpu")]),
    StdFunction(BOOL, "wglEnumGpuDevicesNV", [(HGPUNV, "hGpu"), (UINT, "iDeviceIndex"), Out(Pointer(GPU_DEVICE), "lpGpuDevice")]),
    StdFunction(HDC, "wglCreateAffinityDCNV", [(OpaqueArray(Const(HGPUNV), "_wglCreateAffinityDCNV_size()"), "phGpuList")]),
    StdFunction(BOOL, "wglEnumGpusFromAffinityDCNV", [(HDC, "hAffinityDC"), (UINT, "iGpuIndex"), Out(Pointer(HGPUNV), "hGpu")]),
    StdFunction(BOOL, "wglDeleteDCNV", [(HDC, "hdc")]),

    # WGL_AMD_gpu_association
    StdFunction(UINT, "wglGetGPUIDsAMD", [(UINT, "maxCount"), Out(Array(UINT, "maxCount"), "ids")], sideeffects=False),
    StdFunction(INT, "wglGetGPUInfoAMD", [(UINT, "id"), (Int, "property"), (GLenum, "dataType"), (UINT, "size"), Out(OpaqueBlob(Void, "_wglGetGPUInfoAMD_size(dataType,size)"), "data")], sideeffects=False),
    StdFunction(UINT, "wglGetContextGPUIDAMD", [(HGLRC, "hglrc")], sideeffects=False),
    StdFunction(HGLRC, "wglCreateAssociatedContextAMD", [(UINT, "id")]),
    StdFunction(HGLRC, "wglCreateAssociatedContextAttribsAMD", [(UINT, "id"), (HGLRC, "hShareContext"), (OpaqueArray(Const(Int), "_wglCreateAssociatedContextAttribsAMD_size()"), "attribList")]),
    StdFunction(BOOL, "wglDeleteAssociatedContextAMD", [(HGLRC, "hglrc")]),
    StdFunction(BOOL, "wglMakeAssociatedContextCurrentAMD", [(HGLRC, "hglrc")]),
    StdFunction(HGLRC, "wglGetCurrentAssociatedContextAMD", [], sideeffects=False),
    StdFunction(VOID, "wglBlitContextFramebufferAMD", [(HGLRC, "dstCtx"), (GLint, "srcX0"), (GLint, "srcY0"), (GLint, "srcX1"), (GLint, "srcY1"), (GLint, "dstX0"), (GLint, "dstY0"), (GLint, "dstX1"), (GLint, "dstY1"), (GLbitfield, "mask"), (GLenum, "filter")]),

    # WGL_NV_video_capture
    StdFunction(BOOL, "wglBindVideoCaptureDeviceNV", [(UINT, "uVideoSlot"), (HVIDEOINPUTDEVICENV, "hDevice")]),
    StdFunction(UINT, "wglEnumerateVideoCaptureDevicesNV", [(HDC, "hDc"), Out(Pointer(HVIDEOINPUTDEVICENV), "phDeviceList")]),
    StdFunction(BOOL, "wglLockVideoCaptureDeviceNV", [(HDC, "hDc"), (HVIDEOINPUTDEVICENV, "hDevice")]),
    StdFunction(BOOL, "wglQueryVideoCaptureDeviceNV", [(HDC, "hDc"), (HVIDEOINPUTDEVICENV, "hDevice"), (Int, "iAttribute"), Out(Pointer(Int), "piValue")]),
    StdFunction(BOOL, "wglReleaseVideoCaptureDeviceNV", [(HDC, "hDc"), (HVIDEOINPUTDEVICENV, "hDevice")]),

    # WGL_NV_copy_image
    StdFunction(BOOL, "wglCopyImageSubDataNV", [(HGLRC, "hSrcRC"), (GLuint, "srcName"), (GLenum, "srcTarget"), (GLint, "srcLevel"), (GLint, "srcX"), (GLint, "srcY"), (GLint, "srcZ"), (HGLRC, "hDstRC"), (GLuint, "dstName"), (GLenum, "dstTarget"), (GLint, "dstLevel"), (GLint, "dstX"), (GLint, "dstY"), (GLint, "dstZ"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth")]),

    # WGL_NV_DX_interop
    StdFunction(BOOL, "wglDXSetResourceShareHandleNV", [(OpaquePointer(Void), "dxObject"), (HANDLE, "shareHandle")]),
    StdFunction(HANDLE, "wglDXOpenDeviceNV", [(OpaquePointer(Void), "dxDevice")]),
    StdFunction(BOOL, "wglDXCloseDeviceNV", [(HANDLE, "hDevice")]),
    StdFunction(HANDLE, "wglDXRegisterObjectNV", [(HANDLE, "hDevice"), (OpaquePointer(Void), "dxObject"), (GLuint, "name"), (GLenum, "type"), (GLenum, "access")]),
    StdFunction(BOOL, "wglDXUnregisterObjectNV", [(HANDLE, "hDevice"), (HANDLE, "hObject")]),
    StdFunction(BOOL, "wglDXObjectAccessNV", [(HANDLE, "hObject"), (GLenum, "access")]),
    StdFunction(BOOL, "wglDXLockObjectsNV", [(HANDLE, "hDevice"), (GLint, "count"), Out(Array(HANDLE, "count"), "hObjects")]),
    StdFunction(BOOL, "wglDXUnlockObjectsNV", [(HANDLE, "hDevice"), (GLint, "count"), Out(Array(HANDLE, "count"), "hObjects")]),

    # WGL_NV_delay_before_swap
    StdFunction(BOOL, "wglDelayBeforeSwapNV", [(HDC, "hDC"), (GLfloat, "seconds")]),

    # must be last
    StdFunction(PROC, "wglGetProcAddress", [(LPCSTR, "lpszProc")]),
])

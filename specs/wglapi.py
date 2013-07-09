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

WGLContextAttribs = AttribArray(WGLenum, [
    ('WGL_CONTEXT_MAJOR_VERSION_ARB', Int),
    ('WGL_CONTEXT_MINOR_VERSION_ARB', Int),
    ('WGL_CONTEXT_LAYER_PLANE_ARB', Int),
    ('WGL_CONTEXT_FLAGS_ARB', Flags(Int, ["WGL_CONTEXT_DEBUG_BIT_ARB", "WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB"])),
    ('WGL_CONTEXT_PROFILE_MASK_ARB', Flags(Int, ["WGL_CONTEXT_CORE_PROFILE_BIT_ARB", "WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB"]))
])

#ignoring the float attribs because it's doubtful that anyone uses them
WGLPixelFormatARBAttribs = AttribArray(WGLenum, [
    ('WGL_DRAW_TO_WINDOW_ARB', BOOL),
    ('WGL_DRAW_TO_BITMAP_ARB', BOOL),
    ('WGL_ACCELERATION_ARB', FakeEnum(Int, ['WGL_NO_ACCELERATION_ARB', 'WGL_GENERIC_ACCELERATION_ARB', 'WGL_FULL_ACCELERATION_ARB'])),
    ('WGL_NEED_PALETTE_ARB', BOOL),
    ('WGL_NEED_SYSTEM_PALETTE_ARB', BOOL),
    ('WGL_SWAP_LAYER_BUFFERS_ARB', BOOL),
    ('WGL_SWAP_METHOD_ARB', FakeEnum(Int, ['WGL_SWAP_EXCHANGE_ARB', 'WGL_SWAP_COPY_ARB', 'WGL_SWAP_UNDEFINED_ARB'])),
    ('WGL_NUMBER_OVERLAYS_ARB', Int),
    ('WGL_NUMBER_UNDERLAYS_ARB', Int),
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
    ('WGL_GREEN_BITS_ARB', Int),
    ('WGL_BLUE_BITS_ARB', Int),
    ('WGL_ALPHA_BITS_ARB', Int),
    ('WGL_ACCUM_BITS_ARB', Int),
    ('WGL_ACCUM_RED_BITS_ARB', Int),
    ('WGL_ACCUM_GREEN_BITS_ARB', Int),
    ('WGL_ACCUM_BLUE_BITS_ARB', Int),
    ('WGL_ACCUM_ALPHA_BITS_ARB', Int),
    ('WGL_DEPTH_BITS_ARB', Int),
    ('WGL_STENCIL_BITS_ARB', Int),
    ('WGL_AUX_BUFFERS_ARB', Int)
])

WGLPixelFormatEXTAttribs = AttribArray(WGLenum, [
#    ('WGL_NUMBER_PIXEL_FORMATS_EXT', Int), # not in ...ARB # only allowed in wglGetPixelFormatAttribiv
    ('WGL_DRAW_TO_WINDOW_ARB', BOOL),
    ('WGL_DRAW_TO_BITMAP_ARB', BOOL),
    ('WGL_ACCELERATION_ARB', FakeEnum(Int, ['WGL_NO_ACCELERATION_ARB', 'WGL_GENERIC_ACCELERATION_ARB', 'WGL_FULL_ACCELERATION_ARB'])),
    ('WGL_NEED_PALETTE_ARB', BOOL),
    ('WGL_NEED_SYSTEM_PALETTE_ARB', BOOL),
    ('WGL_SWAP_LAYER_BUFFERS_ARB', BOOL),
    ('WGL_SWAP_METHOD_ARB', FakeEnum(Int, ['WGL_SWAP_EXCHANGE_ARB', 'WGL_SWAP_COPY_ARB', 'WGL_SWAP_UNDEFINED_ARB'])),
    ('WGL_NUMBER_OVERLAYS_ARB', Int),
    ('WGL_NUMBER_UNDERLAYS_ARB', Int),
    ('WGL_TRANSPARENT_EXT', BOOL), # not in ...ARB
    ('WGL_TRANSPARENT_VALUE_EXT', Int), # not in ...ARB
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
    ('WGL_RED_SHIFT_EXT', Int), # not in ...ARB
    ('WGL_GREEN_BITS_ARB', Int),
    ('WGL_GREEN_SHIFT_EXT', Int), # not in ...ARB
    ('WGL_BLUE_BITS_ARB', Int),
    ('WGL_BLUE_SHIFT_EXT', Int), # not in ...ARB
    ('WGL_ALPHA_BITS_ARB', Int),
    ('WGL_ALPHA_SHIFT_EXT', Int), # not in ...ARB
    ('WGL_ACCUM_BITS_ARB', Int),
    ('WGL_ACCUM_RED_BITS_ARB', Int),
    ('WGL_ACCUM_GREEN_BITS_ARB', Int),
    ('WGL_ACCUM_BLUE_BITS_ARB', Int),
    ('WGL_ACCUM_ALPHA_BITS_ARB', Int),
    ('WGL_DEPTH_BITS_ARB', Int),
    ('WGL_STENCIL_BITS_ARB', Int),
    ('WGL_AUX_BUFFERS_ARB', Int)
])

WGLCreatePbufferARBAttribs = AttribArray(WGLenum, [
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

WGLSetPbufferARBAttribs = AttribArray(WGLenum, [
    ('WGL_MIPMAP_LEVEL_ARB', Int),
    ('WGL_CUBE_MAP_FACE_ARB', CubeFaceEnum)
])

HPBUFFERARB = Alias("HPBUFFERARB", HANDLE)


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
    StdFunction(BOOL, "wglChoosePixelFormatARB", [(HDC, "hdc"), (WGLPixelFormatARBAttribs, "piAttribIList"), (Array(Const(FLOAT), "_AttribPairList_size(pfAttribFList)"), "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "(*nNumFormats)"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),

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

    # WGL_EXT_extensions_string
    StdFunction(ConstCString, "wglGetExtensionsStringEXT", [], sideeffects=False),

    # WGL_EXT_make_current_read
    StdFunction(BOOL, "wglMakeContextCurrentEXT", [(HDC, "hDrawDC"), (HDC, "hReadDC"), (HGLRC, "hglrc")]),
    StdFunction(HDC, "wglGetCurrentReadDCEXT", [], sideeffects=False),

    # WGL_EXT_pixel_format
    StdFunction(BOOL, "wglGetPixelFormatAttribivEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(Int, "nAttributes"), "piValues")], sideeffects=False),
    StdFunction(BOOL, "wglGetPixelFormatAttribfvEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(FLOAT, "nAttributes"), "pfValues")], sideeffects=False),
    StdFunction(BOOL, "wglChoosePixelFormatEXT", [(HDC, "hdc"), (WGLPixelFormatEXTAttribs, "piAttribIList"), (Array(Const(FLOAT), "_AttribPairList_size(pfAttribFList)"), "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "*nNumFormats"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),

    # WGL_EXT_swap_control
    StdFunction(BOOL, "wglSwapIntervalEXT", [(Int, "interval")]),
    StdFunction(Int, "wglGetSwapIntervalEXT", [], sideeffects=False),

    # WGL_NV_vertex_array_range
    StdFunction(OpaquePointer(Void), "wglAllocateMemoryNV", [(GLsizei, "size"), (GLfloat, "readfreq"), (GLfloat, "writefreq"), (GLfloat, "priority")]),
    StdFunction(Void, "wglFreeMemoryNV", [(OpaquePointer(Void), "pointer")]),

    # must be last
    StdFunction(PROC, "wglGetProcAddress", [(LPCSTR, "lpszProc")]),
])

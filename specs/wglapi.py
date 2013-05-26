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
    StdFunction(BOOL, "wglChoosePixelFormatARB", [(HDC, "hdc"), (Array(Const(WGLenum), "_AttribPairList_size(piAttribIList)"), "piAttribIList"), (Array(Const(FLOAT), "_AttribPairList_size(pfAttribFList)"), "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "(*nNumFormats)"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),

    # WGL_ARB_make_current_read
    StdFunction(BOOL, "wglMakeContextCurrentARB", [(HDC, "hDrawDC"), (HDC, "hReadDC"), (HGLRC, "hglrc")]),
    StdFunction(HDC, "wglGetCurrentReadDCARB", [], sideeffects=False),

    # WGL_ARB_pbuffer
    StdFunction(HPBUFFERARB, "wglCreatePbufferARB", [(HDC, "hDC"), (Int, "iPixelFormat"), (Int, "iWidth"), (Int, "iHeight"), (Array(Const(WGLenum), "_AttribPairList_size(piAttribList)"), "piAttribList")]),
    StdFunction(HDC, "wglGetPbufferDCARB", [(HPBUFFERARB, "hPbuffer")]),
    StdFunction(Int, "wglReleasePbufferDCARB", [(HPBUFFERARB, "hPbuffer"), (HDC, "hDC")]),
    StdFunction(BOOL, "wglDestroyPbufferARB", [(HPBUFFERARB, "hPbuffer")]),
    StdFunction(BOOL, "wglQueryPbufferARB", [(HPBUFFERARB, "hPbuffer"), (WGLenum, "iAttribute"), Out(Pointer(Int), "piValue")], sideeffects=False),

    # WGL_ARB_render_texture
    StdFunction(BOOL, "wglBindTexImageARB", [(HPBUFFERARB, "hPbuffer"), (Int, "iBuffer")]),
    StdFunction(BOOL, "wglReleaseTexImageARB", [(HPBUFFERARB, "hPbuffer"), (Int, "iBuffer")]),
    StdFunction(BOOL, "wglSetPbufferAttribARB", [(HPBUFFERARB, "hPbuffer"), (Array(Const(WGLenum), "_AttribPairList_size(piAttribList)"), "piAttribList")]),

    # WGL_ARB_create_context
    StdFunction(HGLRC, "wglCreateContextAttribsARB", [(HDC, "hDC"), (HGLRC, "hShareContext"), (Array(Const(WGLenum), "_AttribPairList_size(attribList)"), "attribList")]),

    # WGL_EXT_extensions_string
    StdFunction(ConstCString, "wglGetExtensionsStringEXT", [], sideeffects=False),

    # WGL_EXT_make_current_read
    StdFunction(BOOL, "wglMakeContextCurrentEXT", [(HDC, "hDrawDC"), (HDC, "hReadDC"), (HGLRC, "hglrc")]),
    StdFunction(HDC, "wglGetCurrentReadDCEXT", [], sideeffects=False),

    # WGL_EXT_pixel_format
    StdFunction(BOOL, "wglGetPixelFormatAttribivEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(Int, "nAttributes"), "piValues")], sideeffects=False),
    StdFunction(BOOL, "wglGetPixelFormatAttribfvEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(WGLenum, "nAttributes"), "piAttributes"), Out(Array(FLOAT, "nAttributes"), "pfValues")], sideeffects=False),
    StdFunction(BOOL, "wglChoosePixelFormatEXT", [(HDC, "hdc"), (Array(Const(WGLenum), "_AttribPairList_size(piAttribIList)"), "piAttribIList"), (Array(Const(FLOAT), "_AttribPairList_size(pfAttribFList)"), "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "*nNumFormats"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),

    # WGL_EXT_swap_control
    StdFunction(BOOL, "wglSwapIntervalEXT", [(Int, "interval")]),
    StdFunction(Int, "wglGetSwapIntervalEXT", [], sideeffects=False),

    # WGL_NV_vertex_array_range
    StdFunction(OpaquePointer(Void), "wglAllocateMemoryNV", [(GLsizei, "size"), (GLfloat, "readfreq"), (GLfloat, "writefreq"), (GLfloat, "priority")]),
    StdFunction(Void, "wglFreeMemoryNV", [(OpaquePointer(Void), "pointer")]),

    # must be last
    StdFunction(PROC, "wglGetProcAddress", [(LPCSTR, "lpszProc")]),
])

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

from gl import *
from windows import *

opengl32 = Dll("opengl32")
opengl32.functions += basic_functions(DllFunction)

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

opengl32.functions += [
    DllFunction(BOOL, "wglCopyContext", [(HGLRC, "hglrcSrc"), (HGLRC, "hglrcDst"), (UINT, "mask")]),
    DllFunction(HGLRC, "wglCreateContext", [(HDC, "hdc")]),
    DllFunction(HGLRC, "wglCreateLayerContext", [(HDC, "hdc"), (Int, "iLayerPlane")]),
    DllFunction(BOOL, "wglDeleteContext", [(HGLRC, "hglrc")]),
    DllFunction(HGLRC, "wglGetCurrentContext", [], sideeffects=False),
    DllFunction(HDC, "wglGetCurrentDC", [], sideeffects=False),
    DllFunction(PROC, "wglGetDefaultProcAddress", [(LPCSTR, "lpszProc")], sideeffects=False),
    DllFunction(Int, "wglChoosePixelFormat", [(HDC, "hdc"), (Pointer(Const(PIXELFORMATDESCRIPTOR)), "ppfd")]), 
    DllFunction(Int, "wglDescribePixelFormat", [(HDC, "hdc"), (Int, "iPixelFormat"), (UINT, "nBytes"), Out(Pointer(PIXELFORMATDESCRIPTOR), "ppfd")]),
    DllFunction(Int, "wglGetPixelFormat", [(HDC, "hdc")], sideeffects=False),
    DllFunction(BOOL, "wglSetPixelFormat", [(HDC, "hdc"), (Int, "iPixelFormat"), (Pointer(Const(PIXELFORMATDESCRIPTOR)), "ppfd")]),
    DllFunction(BOOL, "wglMakeCurrent", [(HDC, "hdc"), (HGLRC, "hglrc")]),
    DllFunction(BOOL, "wglShareLists", [(HGLRC, "hglrc1"), (HGLRC, "hglrc2")]),
    DllFunction(BOOL, "wglUseFontBitmapsA", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase")]),
    DllFunction(BOOL, "wglUseFontBitmapsW", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase")]),
    DllFunction(BOOL, "wglSwapBuffers", [(HDC, "hdc")]),
    DllFunction(BOOL, "wglUseFontOutlinesA", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase"), (FLOAT, "deviation"), (FLOAT, "extrusion"), (Int, "format"), (LPGLYPHMETRICSFLOAT, "lpgmf")]),
    DllFunction(BOOL, "wglUseFontOutlinesW", [(HDC, "hdc"), (DWORD, "first"), (DWORD, "count"), (DWORD, "listBase"), (FLOAT, "deviation"), (FLOAT, "extrusion"), (Int, "format"), (LPGLYPHMETRICSFLOAT, "lpgmf")]),
    DllFunction(BOOL , "wglDescribeLayerPlane", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nBytes"), Out(Pointer(LAYERPLANEDESCRIPTOR), "plpd")]),
    DllFunction(Int  , "wglSetLayerPaletteEntries", [(HDC, "hdc"), (Int, "iLayerPlane"), (Int, "iStart"), (Int, "cEntries"), (Array(Const(COLORREF), "cEntries"), "pcr")]),
    DllFunction(Int  , "wglGetLayerPaletteEntries", [(HDC, "hdc"), (Int, "iLayerPlane"), (Int, "iStart"), (Int, "cEntries"), Out(Array(COLORREF, "cEntries"), "pcr")], sideeffects=False),
    DllFunction(BOOL , "wglRealizeLayerPalette", [(HDC, "hdc"), (Int, "iLayerPlane"), (BOOL, "bRealize")]),
    DllFunction(BOOL , "wglSwapLayerBuffers", [(HDC, "hdc"), (UINT, "fuPlanes")]),
    DllFunction(DWORD, "wglSwapMultipleBuffers", [(UINT, "n"), (Array(Const(WGLSWAP), "n"), "ps")]),
]


class WglGetProcAddressFunction(DllFunction):

    def __init__(self, type, name, args **kwargs):
        DllFunction.__init__(self, type, name, args **kwargs)
        self.functions = []

    def wrap_decl(self):
        for function in self.functions:
            function.wrap_decl()
        DllFunction.wrap_decl(self)

    def wrap_impl(self):
        for function in self.functions:
            function.wrap_impl()
        DllFunction.wrap_impl(self)

    def post_call_impl(self):
        print '    if(result) {'
        for function in self.functions:
            ptype = function.pointer_type()
            pvalue = function.pointer_value()
            print '        if(!strcmp("%s", lpszProc)) {' % function.name
            print '            %s = (%s)result;' % (pvalue, ptype)
            print '            result = (PROC)&%s;' % function.name;
            print '        }'
        print '    }'


wglgetprocaddress = WglGetProcAddressFunction(PROC, "wglGetProcAddress", [(LPCSTR, "lpszProc")])
opengl32.functions.append(wglgetprocaddress)

class WglFunction(Function):

    def get_true_pointer(self):
        ptype = self.pointer_type()
        pvalue = self.pointer_value()
        print '    if(!%s)' % (pvalue,)
        self.fail_impl()

attribute = FakeEnum(Int, [
    "WGL_NUMBER_PIXEL_FORMATS_EXT",
    "WGL_DRAW_TO_WINDOW_EXT",
    "WGL_DRAW_TO_BITMAP_EXT",
    "WGL_ACCELERATION_EXT",
    "WGL_NEED_PALETTE_EXT",
    "WGL_NEED_SYSTEM_PALETTE_EXT",
    "WGL_SWAP_LAYER_BUFFERS_EXT",
    "WGL_SWAP_METHOD_EXT",
    "WGL_NUMBER_OVERLAYS_EXT",
    "WGL_NUMBER_UNDERLAYS_EXT",
    "WGL_TRANSPARENT_EXT",
    "WGL_TRANSPARENT_VALUE_EXT",
    "WGL_SHARE_DEPTH_EXT",
    "WGL_SHARE_STENCIL_EXT",
    "WGL_SHARE_ACCUM_EXT",
    "WGL_SUPPORT_GDI_EXT",
    "WGL_SUPPORT_OPENGL_EXT",
    "WGL_DOUBLE_BUFFER_EXT",
    "WGL_STEREO_EXT",
    "WGL_PIXEL_TYPE_EXT",
    "WGL_COLOR_BITS_EXT",
    "WGL_RED_BITS_EXT",
    "WGL_RED_SHIFT_EXT",
    "WGL_GREEN_BITS_EXT",
    "WGL_GREEN_SHIFT_EXT",
    "WGL_BLUE_BITS_EXT",
    "WGL_BLUE_SHIFT_EXT",
    "WGL_ALPHA_BITS_EXT",
    "WGL_ALPHA_SHIFT_EXT",
    "WGL_ACCUM_BITS_EXT",
    "WGL_ACCUM_RED_BITS_EXT",
    "WGL_ACCUM_GREEN_BITS_EXT",
    "WGL_ACCUM_BLUE_BITS_EXT",
    "WGL_ACCUM_ALPHA_BITS_EXT",
    "WGL_DEPTH_BITS_EXT",
    "WGL_STENCIL_BITS_EXT",
    "WGL_AUX_BUFFERS_EXT",
    "WGL_NO_ACCELERATION_EXT",
    "WGL_GENERIC_ACCELERATION_EXT",
    "WGL_FULL_ACCELERATION_EXT",
    "WGL_SWAP_EXCHANGE_EXT",
    "WGL_SWAP_COPY_EXT",
    "WGL_SWAP_UNDEFINED_EXT",
    "WGL_TYPE_RGBA_EXT",
    "WGL_TYPE_COLORINDEX_EXT",
])

HPBUFFERARB = Alias("HPBUFFERARB", HANDLE)

wglgetprocaddress.functions += extended_functions(WglFunction)

wglgetprocaddress.functions += [
    # WGL_ARB_extensions_string
    WglFunction(Const(String), "wglGetExtensionsStringARB", [(HDC, "hdc")], sideeffects=False),
    # WGL_ARB_pbuffer
    WglFunction(HPBUFFERARB, "wglCreatePbufferARB", [(HDC, "hDC"), (Int, "iPixelFormat"), (Int, "iWidth"), (Int, "iHeight"), (Pointer(Const(Int)), "piAttribList")]), 
    WglFunction(HDC, "wglGetPbufferDCARB", [(HPBUFFERARB, "hPbuffer")], sideeffects=False),
    WglFunction(Int, "wglReleasePbufferDCARB", [(HPBUFFERARB, "hPbuffer"), (HDC, "hDC")]),
    WglFunction(BOOL, "wglDestroyPbufferARB", [(HPBUFFERARB, "hPbuffer")]), 
    WglFunction(BOOL, "wglQueryPbufferARB", [(HPBUFFERARB, "hPbuffer"), (Int, "iAttribute"), Out(Pointer(Int), "piValue")]),
    # WGL_ARB_pixel_format
    WglFunction(BOOL, "wglGetPixelFormatAttribivARB", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(attribute, "nAttributes"), "piAttributes"), Out(Array(Int, "nAttributes"), "piValues")], sideeffects=False),
    WglFunction(BOOL, "wglGetPixelFormatAttribfvARB", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(attribute, "nAttributes"), "piAttributes"), Out(Array(FLOAT, "nAttributes"), "pfValues")], sideeffects=False),
    WglFunction(BOOL, "wglChoosePixelFormatARB", [(HDC, "hdc"), (Pointer(Const(Int)), "piAttribIList"), (Pointer(Const(FLOAT)), "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "nMaxFormats"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),
    # WGL_EXT_extensions_string
    WglFunction(Const(String), "wglGetExtensionsStringEXT", [], sideeffects=False),
    # WGL_EXT_pixel_format
    WglFunction(BOOL, "wglGetPixelFormatAttribivEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(attribute, "nAttributes"), "piAttributes"), Out(Array(Int, "nAttributes"), "piValues")], sideeffects=False),
    WglFunction(BOOL, "wglGetPixelFormatAttribfvEXT", [(HDC, "hdc"), (Int, "iPixelFormat"), (Int, "iLayerPlane"), (UINT, "nAttributes"), (Array(attribute, "nAttributes"), "piAttributes"), Out(Array(FLOAT, "nAttributes"), "pfValues")], sideeffects=False),
    WglFunction(BOOL, "wglChoosePixelFormatEXT", [(HDC, "hdc"), (Pointer(Const(Int)), "piAttribIList"), (Pointer(Const(FLOAT)), "pfAttribFList"), (UINT, "nMaxFormats"), Out(Array(Int, "nMaxFormats"), "piFormats"), Out(Pointer(UINT), "nNumFormats")]),
]

if __name__ == '__main__':
    print
    print '#define _GDI32_'
    print
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print '#include <GL/gl.h>'
    print '#include "glext.h"'
    print '#include "wglext.h"'
    print
    print '#include "glhelpers.hpp"'
    print '#include "log.hpp"'
    print
    print '#ifndef PFD_SUPPORT_DIRECTDRAW'
    print '#define PFD_SUPPORT_DIRECTDRAW 0x00002000'
    print '#endif'
    print '#ifndef PFD_SUPPORT_COMPOSITION'
    print '#define PFD_SUPPORT_COMPOSITION 0x00008000'
    print '#endif'
    print
    print '#ifdef __MINGW32__'
    print ''
    print 'typedef struct _WGLSWAP'
    print '{'
    print '    HDC hdc;'
    print '    UINT uiFlags;'
    print '} WGLSWAP, *PWGLSWAP, FAR *LPWGLSWAP;'
    print ''
    print '#define WGL_SWAPMULTIPLE_MAX 16'
    print ''
    print '#endif'
    print
    print 'extern "C" {'
    print
    wrap()
    print
    print '}'

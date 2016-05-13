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


"""WGL tracing code generator."""


from gltrace import GlTracer
from specs.stdapi import Module, API, Void
from specs.glapi import glapi
from specs.wglapi import wglapi


class WglTracer(GlTracer):

    getProcAddressFunctionNames = [
        "wglGetProcAddress",
    ]

    createContextFunctionNames = [
        'wglCreateContext',
        'wglCreateContextAttribsARB',
        'wglCreateLayerContext',
    ]

    destroyContextFunctionNames = [
        'wglDeleteContext',
    ]

    makeCurrentFunctionNames = [
        'wglMakeCurrent',
        'wglMakeContextCurrentARB',
        'wglMakeContextCurrentEXT',
    ]

    def traceFunctionImplBody(self, function):
        if function.name.startswith('wgl'):
            # When implementing WGL extensions OpenGL ICDs often have no
            # alternative to calling back into OPENGL32.DLL's wgl* entry points
            # due to lack of extensibility in the ICD interface.  These
            # internal calls are not only visually confusing but can actually
            # cause problems when tracing, and replaying.  A particularly nasty
            # case is wglCreateContextAttribsARB which ends up calling
            # wglCreateContext/wglCreateLayerContext to obtain a HGLRC that's
            # recognizable by OPENGL32.DLL.  Therefore we need to detect and
            # dispatch internal calls, without further ado.
            print r'    if (_reentrant) {'
            self.invokeFunction(function)
            if function.type is not Void:
                print '    return _result;'
            print r'    }'
            print r'    ReentryScope _reentry;'
            print r'    (void)_reentry;'
            print

        if function.name in self.destroyContextFunctionNames:
            # Unlike other GL APIs like EGL or GLX, WGL will make the context
            # inactive if it's currently the active context.
            print '    if (_wglGetCurrentContext() == hglrc) {'
            print '        gltrace::clearContext();'
            print '    }'
            print '    gltrace::releaseContext((uintptr_t)hglrc);'

        # Emit fake glBitmap calls in the trace on wglUseFontBitmapsA.
        # This enables to capture the real bitmaps and replay them outside Windows.
        #
        # XXX: In spite what
        # https://msdn.microsoft.com/en-us/library/windows/desktop/dd374392.aspx
        # implies, GetGlyphOutline(GGO_BITMAP) does not seem to work with
        # certain fonts.  The only solution is to draw the font charactors with
        # a HBITMAP like the old Mesa fxwgl.c code used to do.  That too, seems
        # to be the way that opengl32.dll implements wglUseFontBitmaps.
        #
        if function.name == 'wglUseFontBitmapsA':

            self.invokeFunction(function)

            # Emit a fake string marker with the original call
            print r'    {'
            print r'        std::ostringstream ss;'
            print r'        ss << __FUNCTION__ << "(hdc = " << hdc << ", first = " << first << ", count = " << count << ", listBase = " << listBase << ") = " << (_result ? "TRUE" : "FALSE");'
            print r'        _fakeStringMarker(ss.str());'
            print r'    }'
            print

            glNewList = glapi.getFunctionByName('glNewList')
            glBitmap = glapi.getFunctionByName('glBitmap')
            glEndList = glapi.getFunctionByName('glEndList')

            print r'    if (_result) {'

            print r'        HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);'
            print r'        assert (hFont != nullptr);'
            print r'        LOGFONT lf;'
            print r'        if (GetObject(hFont, sizeof lf, &lf) != 0) {'
            print r'            std::ostringstream ss;'
            print r'            ss << "lfFaceName = " << lf.lfFaceName'
            print r'               << ", lfHeight = " << lf.lfHeight'
            print r'               << ", lfWeight = " << lf.lfWeight;'
            print r'            if (lf.lfItalic) ss << ", lfItalic = 1";'
            print r'            if (lf.lfUnderline) ss << ", lfUnderline = 1";'
            print r'            if (lf.lfStrikeOut) ss << ", lfStrikeOut = 1";'
            print r'            _fakeStringMarker(ss.str());'
            print r'        }'
            print

            print r'        BOOL bRet;'
            print r'        TEXTMETRIC tm;'
            print r'        bRet = GetTextMetricsA(hdc, &tm);'
            print r'        assert(bRet);'
            print
            print r'        HDC memDC = CreateCompatibleDC(hdc);'
            print
            print r'        SetMapMode(memDC, MM_TEXT);'
            print r'        SetTextAlign(memDC, TA_BASELINE);'
            print r'        SetBkColor(memDC, RGB(0, 0, 0));'
            print r'        SetBkMode(memDC, OPAQUE);'
            print r'        SetTextColor(memDC, RGB(255,255,255));'
            print
            print r'        BITMAPINFO * bmi = (BITMAPINFO *)malloc(offsetof(BITMAPINFO, bmiColors[2]));'
            print r'        ZeroMemory(&bmi->bmiHeader, sizeof bmi->bmiHeader);'
            print r'        bmi->bmiHeader.biSize = sizeof bmi->bmiHeader;'
            print r'        bmi->bmiHeader.biPlanes = 1;'
            print r'        bmi->bmiHeader.biBitCount = 1;'
            print r'        bmi->bmiHeader.biCompression = BI_RGB;'
            print r'        bmi->bmiColors[0].rgbBlue     = 0;'
            print r'        bmi->bmiColors[0].rgbGreen    = 0;'
            print r'        bmi->bmiColors[0].rgbRed      = 0;'
            print r'        bmi->bmiColors[0].rgbReserved = 0;'
            print r'        bmi->bmiColors[1].rgbBlue     = 255;'
            print r'        bmi->bmiColors[1].rgbGreen    = 255;'
            print r'        bmi->bmiColors[1].rgbRed      = 255;'
            print r'        bmi->bmiColors[1].rgbReserved = 0;'
            print
            print r'        for (DWORD i = 0; i < count; ++i) {'
            print r'            _fake_glNewList(listBase + i, GL_COMPILE);'
            print
            print r'            char cChar = first + i;'
            print
            print r'            // TODO: Use GetCharABSWidths'
            print r'            // http://www.codeproject.com/Articles/14915/Width-of-text-in-italic-font'
            print r'            // https://support.microsoft.com/en-us/kb/94646'
            print r'            SIZE size;'
            print r'            bRet = GetTextExtentPoint32A(hdc, &cChar, 1, &size);'
            print r'            if (bRet) {'
            print r'                assert(size.cx >= 0);'
            print r'                assert(size.cy >= 0);'
            print
            print r'                // Round width to 32 pixels'
            print r'                int nWidth = (size.cx + 0x1f) & ~0x1f;'
            print r'                int nHeight = size.cy;'
            print
            print r'                DWORD dwBytes = nWidth / 8 * nHeight;'
            print r'                LPVOID lpvBits = NULL;'
            print r'                if (dwBytes) {'
            print r'                    lpvBits = malloc(dwBytes);'
            print
            print r'                    HBITMAP memBM = CreateCompatibleBitmap(memDC, nWidth, nHeight);'
            print
            print r'                    HGDIOBJ origBM = SelectObject(memDC, memBM);'
            print
            print r'                    PatBlt(memDC, 0, 0, nWidth, nHeight, BLACKNESS);'
            print r'                    SelectObject(memDC, hFont);'
            print
            print r'                    bmi->bmiHeader.biWidth  = nWidth;'
            print r'                    bmi->bmiHeader.biHeight = nHeight;'
            print
            print r'                    bRet = TextOutA(memDC, 0, tm.tmAscent, &cChar, 1);'
            print r'                    assert(bRet);'
            print
            print r'                    SelectObject(memDC, origBM);'
            print
            print r'                    int nScanLines = GetDIBits(memDC, memBM, 0, nHeight, lpvBits, bmi, DIB_RGB_COLORS);'
            print r'                    assert(nScanLines == nHeight);'
            print
            print r'                    DeleteObject(memBM);'
            print r'                }'
            print
            print r'                GLsizei width  = nWidth;'
            print r'                GLfloat height = nHeight;'
            print r'                GLfloat xorig  = 0;'
            print r'                GLfloat yorig  = tm.tmDescent;'
            print r'                GLfloat xmove  = size.cx;'
            print r'                GLfloat ymove  = 0;'
            print r'                const GLubyte *bitmap = (const GLubyte *)lpvBits;'
            print r'                if (bitmap == NULL) {'
            print r'                    // We still need to emit an empty glBitmap for empty characters like spaces;'
            print r'                    width = height = xorig = yorig = 0;'
            print r'                }'

            # FIXME: glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
            # FIXME: glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            print
            print r'                _fake_glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);'
            print
            print r'                free(lpvBits);'
            print r'            }'
            print r'            _fake_glEndList();'
            print r'        }'
            print
            print r'        DeleteDC(memDC);'
            print r'        free(bmi);'

            print r'    } // _result'
            return

        GlTracer.traceFunctionImplBody(self, function)

        if function.name in self.createContextFunctionNames:
            print '    if (_result)'
            print '        gltrace::createContext((uintptr_t)_result);'

        if function.name in self.makeCurrentFunctionNames:
            print '    if (_result) {'
            print '        if (hglrc != NULL)'
            print '            gltrace::setContext((uintptr_t)hglrc);'
            print '        else'
            print '            gltrace::clearContext();'
            print '    }'


if __name__ == '__main__':
    print
    print '#define _GDI32_'
    print
    print '#include <string.h>'
    print '#include <windows.h>'
    print
    print '#include <sstream>'
    print
    print '#include "trace_writer_local.hpp"'
    print '#include "os.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print '#define WGL_GLXEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print
    print 'static OS_THREAD_SPECIFIC(uintptr_t) _reentrant;'
    print
    print '// Helper class to track reentries in function scope.'
    print 'struct ReentryScope {'
    print 'inline ReentryScope() { _reentrant = 1; }'
    print 'inline ~ReentryScope() { _reentrant = 0; }'
    print '};'
    print
    module = Module()
    module.mergeModule(glapi)
    module.mergeModule(wglapi)
    api = API()
    api.addModule(module)
    tracer = WglTracer()
    tracer.traceApi(api)

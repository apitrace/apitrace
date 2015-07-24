/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


/*
 * WGL bindings.
 */


#include <iostream>

#include "glproc.hpp"
#include "glws.hpp"
#include "ws_win32.hpp"


namespace glws {


/*
 * Several WGL functions come in two flavors:
 * - GDI (ChoosePixelFormat, SetPixelFormat, SwapBuffers, etc)
 * - WGL (wglChoosePixelFormat, wglSetPixelFormat, wglSwapBuffers, etc)
 *
 * The GDI entrypoints will inevitably dispatch to the first module named
 * "OPENGL32", loading "C:\Windows\System32\opengl32.dll" if none was loaded so
 * far.
 *
 * In order to use a implementation other than the one installed in the system
 * (when specified via the TRACE_LIBGL environment variable), we need to use
 * WGL entrypoints.
 *
 * See also:
 * - http://www.opengl.org/archives/resources/faq/technical/mswindows.htm
 */
static PFN_WGLCHOOSEPIXELFORMAT pfnChoosePixelFormat = &ChoosePixelFormat;
static PFN_WGLDESCRIBEPIXELFORMAT pfnDescribePixelFormat = &DescribePixelFormat;
static PFN_WGLSETPIXELFORMAT pfnSetPixelFormat = &SetPixelFormat;
static PFN_WGLSWAPBUFFERS pfnSwapBuffers = &SwapBuffers;


class WglDrawable : public Drawable
{
public:
    HWND hWnd;
    HDC hDC;
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormat;

    WglDrawable(const Visual *vis, int width, int height, bool pbuffer) :
        Drawable(vis, width, height, pbuffer)
    {
        BOOL bRet;

        hWnd = ws::createWindow("glretrace", width, height);

        hDC = GetDC(hWnd);
   
        ZeroMemory(&pfd, sizeof pfd);
        pfd.nSize = sizeof pfd;
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pfd.cColorBits = 3;
        // XXX: ChoosePixelFormat will return a software pixelformat if we
        // request alpha bits and the hardware driver doesn't advertise any
        // pixel format with alpha bits (e.g. NVIDIA OpenGL driver on 16bpp
        // screens.)
        pfd.cAlphaBits = 1;
        pfd.cDepthBits = 1;
        pfd.cStencilBits = 1;
        pfd.iLayerType = PFD_MAIN_PLANE;

        if (visual->doubleBuffer) {
           pfd.dwFlags |= PFD_DOUBLEBUFFER;
        }

        iPixelFormat = pfnChoosePixelFormat(hDC, &pfd);
        if (iPixelFormat <= 0) {
            std::cerr << "error: ChoosePixelFormat failed\n";
            exit(1);
        }

        pfnDescribePixelFormat(hDC, iPixelFormat, sizeof pfd, &pfd);
        assert(pfd.dwFlags & PFD_SUPPORT_OPENGL);
        if (pfd.dwFlags & PFD_GENERIC_FORMAT) {
            std::cerr << "warning: ChoosePixelFormat returned a pixel format supported by the GDI software implementation\n";
        }

        bRet = pfnSetPixelFormat(hDC, iPixelFormat, &pfd);
        if (!bRet) {
            std::cerr << "error: SetPixelFormat failed\n";
            exit(1);
        }
    }

    ~WglDrawable() {
        ReleaseDC(hWnd, hDC);
        DestroyWindow(hWnd);
    }
    
    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }

        Drawable::resize(w, h);

        ws::resizeWindow(hWnd, w, h);
    }

    void show(void) {
        if (visible) {
            return;
        }

        ShowWindow(hWnd, SW_SHOW);

        Drawable::show();
    }

    void swapBuffers(void) {
        BOOL bRet;
        bRet = pfnSwapBuffers(hDC);
        if (!bRet) {
            std::cerr << "warning: SwapBuffers failed\n";
        }
    }
};


class WglContext : public Context
{
public:
    HGLRC hglrc;
    WglContext *shareContext;
    bool debug;

    WglContext(const Visual *vis, WglContext *share, bool _debug) :
        Context(vis),
        hglrc(0),
        shareContext(share),
        debug(_debug)
    {}

    ~WglContext() {
        if (hglrc) {
            wglDeleteContext(hglrc);
        }
    }

    bool
    createARB(HDC hDC) {
        bool required = profile.api != glprofile::API_GL ||
                        profile.versionGreaterOrEqual(3, 1);

        // We need to create context through WGL_ARB_create_context.  This
        // implies binding a temporary context to get the extensions strings
        // and function pointers.

        // This function is only called inside makeCurrent, so we don't need
        // to save and restore the previous current context/drawable.
        BOOL bRet = wglMakeCurrent(hDC, hglrc);
        if (!bRet) {
            std::cerr << "error: wglMakeCurrent failed\n";
            exit(1);
        }

        PFNWGLGETEXTENSIONSSTRINGARBPROC pfnWglGetExtensionsStringARB =
            (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
        if (!pfnWglGetExtensionsStringARB) {
            if (required) {
                std::cerr << "error: WGL_ARB_extensions_string not supported\n";
                exit(1);
            } else {
                return false;
            }
        }
        const char * extensionsString = pfnWglGetExtensionsStringARB(hDC);
        if (!checkExtension("WGL_ARB_create_context", extensionsString)) {
            if (required) {
                std::cerr << "error: WGL_ARB_create_context not supported\n";
                exit(1);
            } else {
                return false;
            }
        }

        PFNWGLCREATECONTEXTATTRIBSARBPROC pfnWglCreateContextAttribsARB =
            (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
        if (!pfnWglCreateContextAttribsARB) {
            if (required) {
                std::cerr << "error: failed to get pointer to wglCreateContextAttribsARB\n";
                exit(1);
            } else {
                return false;
            }
        }

        wglMakeCurrent(hDC, NULL);

        Attributes<int> attribs;
        int contextFlags = 0;
        if (profile.api == glprofile::API_GL) {
            attribs.add(WGL_CONTEXT_MAJOR_VERSION_ARB, profile.major);
            attribs.add(WGL_CONTEXT_MINOR_VERSION_ARB, profile.minor);
            if (profile.versionGreaterOrEqual(3, 2)) {
                if (!checkExtension("WGL_ARB_create_context_profile", extensionsString)) {
                    assert(required);
                    std::cerr << "error: WGL_ARB_create_context_profile not supported\n";
                    exit(1);
                }
                int profileMask = profile.core ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                attribs.add(WGL_CONTEXT_PROFILE_MASK_ARB, profileMask);
                if (profile.forwardCompatible) {
                    contextFlags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
                }
            }
        } else if (profile.api == glprofile::API_GLES) {
            if (checkExtension("WGL_EXT_create_context_es_profile", extensionsString)) {
                attribs.add(WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES_PROFILE_BIT_EXT);
                attribs.add(WGL_CONTEXT_MAJOR_VERSION_ARB, profile.major);
                attribs.add(WGL_CONTEXT_MINOR_VERSION_ARB, profile.minor);
            } else if (profile.major != 2) {
                std::cerr << "warning: OpenGL ES " << profile.major << " requested but WGL_EXT_create_context_es_profile not supported\n";
            } else if (checkExtension("WGL_EXT_create_context_es2_profile", extensionsString)) {
                assert(profile.major == 2);
                attribs.add(WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES2_PROFILE_BIT_EXT);
                attribs.add(WGL_CONTEXT_MAJOR_VERSION_ARB, profile.major);
                attribs.add(WGL_CONTEXT_MINOR_VERSION_ARB, profile.minor);
            } else {
                std::cerr << "warning: OpenGL ES " << profile.major << " requested but WGL_EXT_create_context_es_profile or WGL_EXT_create_context_es2_profile not supported\n";
            }
        } else {
            assert(0);
        }
        if (debug) {
            contextFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        }
        if (contextFlags) {
            attribs.add(WGL_CONTEXT_FLAGS_ARB, contextFlags);
        }
        attribs.end();


        HGLRC new_hglrc;
        new_hglrc = pfnWglCreateContextAttribsARB(hDC,
                                                  shareContext ? shareContext->hglrc : 0,
                                                  attribs);
        if (!new_hglrc) {
            if (required) {
                std::cerr << "error: wglCreateContextAttribsARB failed\n";
                exit(1);
            } else {
                return false;
            }
        }

        wglDeleteContext(hglrc);
        hglrc = new_hglrc;

        return true;
    }

    bool
    create(WglDrawable *wglDrawable) {
        if (!hglrc) {
            HDC hDC = wglDrawable->hDC;

            hglrc = wglCreateContext(hDC);
            if (!hglrc) {
                std::cerr << "error: wglCreateContext failed\n";
                exit(1);
            }

            if (shareContext) {
                shareContext->create(wglDrawable);
            }

            if (!createARB(hDC)) {
                if (shareContext) {
                    BOOL bRet;
                    bRet = wglShareLists(shareContext->hglrc,
                                         hglrc);
                    if (!bRet) {
                        std::cerr
                            << "warning: wglShareLists failed: "
                            << std::hex << GetLastError() << std::dec
                            << "\n";
                    }
                }
            }
        }

        return true;
    }

};


void
init(void) {
    /*
     * OpenGL library must be loaded by the time we call GDI.
     */

    const char * libgl_filename = getenv("TRACE_LIBGL");

    if (libgl_filename) {
        _libGlHandle = LoadLibraryA(libgl_filename);
        if (_libGlHandle) {
            pfnChoosePixelFormat = (PFN_WGLCHOOSEPIXELFORMAT)GetProcAddress(_libGlHandle, "wglChoosePixelFormat");
            assert(pfnChoosePixelFormat);
            pfnDescribePixelFormat = (PFN_WGLDESCRIBEPIXELFORMAT)GetProcAddress(_libGlHandle, "wglDescribePixelFormat");
            assert(pfnDescribePixelFormat);
            pfnSetPixelFormat = (PFN_WGLSETPIXELFORMAT)GetProcAddress(_libGlHandle, "wglSetPixelFormat");
            assert(pfnSetPixelFormat);
            pfnSwapBuffers = (PFN_WGLSWAPBUFFERS)GetProcAddress(_libGlHandle, "wglSwapBuffers");
            assert(pfnSwapBuffers);
        }
    } else {
        libgl_filename = "OPENGL32";
        _libGlHandle = LoadLibraryA(libgl_filename);
    }

    if (!_libGlHandle) {
        std::cerr << "error: unable to open " << libgl_filename << "\n";
        exit(1);
    }
}

void
cleanup(void) {
}

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {
    Visual *visual = new Visual(profile);

    visual->doubleBuffer = doubleBuffer;

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    return new WglDrawable(visual, width, height, pbuffer);
}

Context *
createContext(const Visual *visual, Context *shareContext, bool debug)
{
    return new WglContext(visual, static_cast<WglContext *>(shareContext), debug);
}

bool
makeCurrentInternal(Drawable *drawable, Context *context)
{
    if (!drawable || !context) {
        return wglMakeCurrent(NULL, NULL);
    } else {
        WglDrawable *wglDrawable = static_cast<WglDrawable *>(drawable);
        WglContext *wglContext = static_cast<WglContext *>(context);

        wglContext->create(wglDrawable);

        return wglMakeCurrent(wglDrawable->hDC, wglContext->hglrc);
    }
}

bool
processEvents(void)
{
    return ws::processEvents();
}


} /* namespace glws */

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
static PFN_WGLSETPIXELFORMAT pfnSetPixelFormat = &SetPixelFormat;
static PFN_WGLSWAPBUFFERS pfnSwapBuffers = &SwapBuffers;


static LRESULT CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MINMAXINFO *pMMI;
    switch (uMsg) {
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case WM_GETMINMAXINFO:
        // Allow to create a window bigger than the desktop
        pMMI = (MINMAXINFO *)lParam;
        pMMI->ptMaxSize.x = 60000;
        pMMI->ptMaxSize.y = 60000;
        pMMI->ptMaxTrackSize.x = 60000;
        pMMI->ptMaxTrackSize.y = 60000;
        break;
    case WM_CLOSE:
        exit(0);
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


class WglDrawable : public Drawable
{
public:
    DWORD dwExStyle;
    DWORD dwStyle;
    HWND hWnd;
    HDC hDC;
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormat;

    WglDrawable(const Visual *vis, int width, int height, bool pbuffer) :
        Drawable(vis, width, height, pbuffer)
    {
        static bool first = TRUE;
        RECT rect;
        BOOL bRet;

        if (first) {
            WNDCLASS wc;
            memset(&wc, 0, sizeof wc);
            wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            wc.lpfnWndProc = WndProc;
            wc.lpszClassName = "glretrace";
            wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            RegisterClass(&wc);
            first = FALSE;
        }

        dwExStyle = 0;
        dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

        int x = 0, y = 0;

        rect.left = x;
        rect.top = y;
        rect.right = rect.left + width;
        rect.bottom = rect.top + height;

        AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

        hWnd = CreateWindowEx(dwExStyle,
                              "glretrace", /* wc.lpszClassName */
                              NULL,
                              dwStyle,
                              0, /* x */
                              0, /* y */
                              rect.right - rect.left, /* width */
                              rect.bottom - rect.top, /* height */
                              NULL,
                              NULL,
                              NULL,
                              NULL);
        hDC = GetDC(hWnd);
   
        memset(&pfd, 0, sizeof pfd);
        pfd.cColorBits = 4;
        pfd.cRedBits = 1;
        pfd.cGreenBits = 1;
        pfd.cBlueBits = 1;
        pfd.cAlphaBits = 1;
        pfd.cDepthBits = 1;
        pfd.cStencilBits = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;

        if (visual->doubleBuffer) {
           pfd.dwFlags |= PFD_DOUBLEBUFFER;
        }

        iPixelFormat = pfnChoosePixelFormat(hDC, &pfd);
        if (iPixelFormat <= 0) {
            std::cerr << "error: ChoosePixelFormat failed\n";
            exit(1);
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

        RECT rClient, rWindow;
        GetClientRect(hWnd, &rClient);
        GetWindowRect(hWnd, &rWindow);
        w += (rWindow.right  - rWindow.left) - rClient.right;
        h += (rWindow.bottom - rWindow.top)  - rClient.bottom;
        SetWindowPos(hWnd, NULL, rWindow.left, rWindow.top, w, h, SWP_NOMOVE);

        Drawable::resize(w, h);
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

        // Drain message queue to prevent window from being considered
        // non-responsive
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
};


class WglContext : public Context
{
public:
    HGLRC hglrc;
    WglContext *shareContext;

    WglContext(const Visual *vis, Profile prof, WglContext *share) :
        Context(vis, prof),
        hglrc(0),
        shareContext(share)
    {}

    ~WglContext() {
        if (hglrc) {
            wglDeleteContext(hglrc);
        }
    }

    bool
    create(WglDrawable *wglDrawable) {
        if (!hglrc) {
            hglrc = wglCreateContext(wglDrawable->hDC);
            if (!hglrc) {
                std::cerr << "error: wglCreateContext failed\n";
                exit(1);
                return false;
            }
            if (shareContext) {
                if (shareContext->create(wglDrawable)) {
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
        pfnChoosePixelFormat = &wglChoosePixelFormat;
        pfnSetPixelFormat = &wglSetPixelFormat;
        pfnSwapBuffers = &wglSwapBuffers;
    } else {
        libgl_filename = "OPENGL32";
    }

    _libGlHandle = LoadLibraryA(libgl_filename);
    if (!_libGlHandle) {
        std::cerr << "error: unable to open " << libgl_filename << "\n";
        exit(1);
    }
}

void
cleanup(void) {
}

Visual *
createVisual(bool doubleBuffer, Profile profile) {
    if (profile != PROFILE_COMPAT &&
        profile != PROFILE_CORE) {
        return NULL;
    }

    Visual *visual = new Visual();

    visual->doubleBuffer = doubleBuffer;

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    return new WglDrawable(visual, width, height, pbuffer);
}

Context *
createContext(const Visual *visual, Context *shareContext, Profile profile, bool debug)
{
    if (profile != PROFILE_COMPAT &&
        profile != PROFILE_CORE) {
        return NULL;
    }

    if (profile == PROFILE_CORE) {
        std::cerr << "warning: ignoring OpenGL core profile request\n";
    }

    return new WglContext(visual, profile, static_cast<WglContext *>(shareContext));
}

bool
makeCurrent(Drawable *drawable, Context *context)
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
processEvents(void) {
    MSG uMsg;
    while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE)) {
        if (uMsg.message == WM_QUIT) {
            return false;
        }

        if (!TranslateAccelerator(uMsg.hwnd, NULL, &uMsg)) {
            TranslateMessage(&uMsg);
            DispatchMessage(&uMsg);
        }
    }
    return true;
}


} /* namespace glws */

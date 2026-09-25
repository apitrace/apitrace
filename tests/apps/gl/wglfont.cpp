/*
 * Copyright (C) 2016, VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <windows.h>
#include <stdlib.h>
#include <GL/gl.h>

int
main(int argc, char *argv[])
{
    WNDCLASS wc;
    HWND hwnd;
    HDC hdc;
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormat;
    HGLRC hglrc;

    ZeroMemory(&wc, sizeof wc);
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProc;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpszClassName = "wglfont";

    DWORD dwExStyle = 0;
    DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

    RECT rect;
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = 640;
    rect.bottom = 480;
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
    int nWidth  = rect.right  - rect.left;
    int nHeight = rect.bottom - rect.top;

    if (!RegisterClass(&wc)) {
        abort();
    }

    hwnd = CreateWindowEx(dwExStyle,
                          wc.lpszClassName,
                          "wglfont",
                          dwStyle,
                          0, 0, nWidth, nHeight,
                          NULL, NULL,
                          wc.hInstance,
                          NULL);
    if (!hwnd) {
        abort();
    }

    hdc = GetDC(hwnd);
    if (!hdc) {
        abort();
    }

    ZeroMemory(&pfd, sizeof pfd);
    pfd.nSize = sizeof pfd;
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iPixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (!iPixelFormat) {
        abort();
    }

    if (!SetPixelFormat(hdc, iPixelFormat, &pfd)) {
        abort();
    }

    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        abort();
    }

    wglMakeCurrent(hdc, hglrc);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    SelectObject(hdc, GetStockObject(SYSTEM_FONT));

    wglUseFontBitmaps(hdc, 0, 256, 1000);

    glListBase(1000);

    glCallLists(12, GL_UNSIGNED_BYTE, "Hello World!");

    SwapBuffers(hdc);

    wglMakeCurrent(NULL, NULL);

    wglDeleteContext(hglrc);

    ReleaseDC(hwnd, hdc);

    DestroyWindow(hwnd);

    return 0;
}

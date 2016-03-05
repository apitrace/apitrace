/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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


#include "ws_win32.hpp"

#include <assert.h>
#include <process.h>

#include "ws.hpp"


namespace ws {


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
        // This is run on a thread other than the main thread and the retrace
        // threads, which are still alive and running, so we must bypass
        // destructors here to avoid crashes.
        _exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


static const DWORD dwExStyle = 0;
static const DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

static LPCSTR g_lpszClassName = "retrace";


struct WindowThreadParam
{
    LPCSTR lpszWindowName;
    int nWidth;
    int nHeight;

    HANDLE hEvent;

    HWND hWnd;
};


static unsigned __stdcall
windowThreadFunction( LPVOID _lpParam )
{
    WindowThreadParam *lpParam = (WindowThreadParam *)_lpParam;

    // Actually create the window
    lpParam->hWnd = CreateWindowExA(dwExStyle,
                                    g_lpszClassName,
                                    lpParam->lpszWindowName,
                                    dwStyle,
                                    0, // x
                                    0, // y
                                    lpParam->nWidth,
                                    lpParam->nHeight,
                                    NULL, // hWndParent
                                    NULL, // hMenu
                                    NULL, // hInstance
                                    NULL); // lpParam

    // Notify parent thread that window has been created
    SetEvent(lpParam->hEvent);

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0 )) != FALSE) {
        if (bRet == -1) {
            // handle the error and possibly exit
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Return the exit code
    return msg.wParam;
}


/**
 * Create a window on a separate thread.
 *
 * Each window is associated with one thread, which must process its events,
 * and certain operations like SetWindowPos will block until the message is
 * process, so the only way to guarantee that the window messages are processed
 * timely is by dedicating a thread to processing window events.
 *
 * TODO: Handle multiple windows on a single thread, instead of creating one
 * thread per each window.
 */
HWND
createWindow(LPCSTR lpszWindowName, int nWidth, int nHeight)
{
    // Create window class
    static bool first = TRUE;
    if (first) {
        WNDCLASS wc;
        ZeroMemory(&wc, sizeof wc);
        wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.lpfnWndProc = WndProc;
        wc.lpszClassName = g_lpszClassName;
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        RegisterClass(&wc);
        first = FALSE;
    }

    RECT rect;
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = nWidth;
    rect.bottom = nHeight;
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

    // Spawn window thread
    WindowThreadParam param;
    param.lpszWindowName = lpszWindowName;
    param.nWidth  = rect.right  - rect.left;
    param.nHeight = rect.bottom - rect.top;
    param.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    _beginthreadex(NULL, 0, &windowThreadFunction, (LPVOID)&param, 0, NULL);

    // Wait for window creation event
    WaitForSingleObject(param.hEvent, INFINITE);
    CloseHandle(param.hEvent);

    RECT rClient;
    GetClientRect(param.hWnd, &rClient);
    assert(rClient.right  - rClient.left >= nWidth);
    assert(rClient.bottom - rClient.top  >= nHeight);

    return param.hWnd;
}


void
showWindow(HWND hWnd)
{
    if (!headless) {
        ShowWindow(hWnd, SW_SHOW);
    }
}


void
resizeWindow(HWND hWnd, int width, int height)
{
    RECT rClient;
    GetClientRect(hWnd, &rClient);
    if (width  == rClient.right  - rClient.left &&
        height == rClient.bottom - rClient.top) {
        return;
    }

    RECT rWindow;
    GetWindowRect(hWnd, &rWindow);
    width  += (rWindow.right  - rWindow.left) - rClient.right;
    height += (rWindow.bottom - rWindow.top)  - rClient.bottom;

    UINT uFlags = SWP_NOMOVE
                | SWP_NOZORDER
                | SWP_NOACTIVATE
                | SWP_NOOWNERZORDER;

    SetWindowPos(hWnd, NULL, rWindow.left, rWindow.top, width, height, uFlags);
}


bool
processEvents(void)
{
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


} // namespace ws


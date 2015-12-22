/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
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


#include <assert.h>

#include "d3dretrace.hpp"

#include "ws_win32.hpp"


namespace d3dretrace {


HWND
createWindow(int width, int height)
{
    HWND hWnd;

    hWnd = ws::createWindow("d3dretrace", width, height);

    ws::showWindow(hWnd);

    return hWnd;
}


typedef std::map<HWND, HWND> HWND_MAP;
static HWND_MAP g_hWndMap;


HWND
createWindow(HWND hWnd, int width, int height)
{
    HWND_MAP::iterator it;
    it = g_hWndMap.find(hWnd);
    if (it == g_hWndMap.end()) {
        // Create a new window
        hWnd = createWindow(width, height);
        g_hWndMap[hWnd] = hWnd;
    } else {
        // Reuse the existing window
        hWnd = it->second;
        ws::resizeWindow(hWnd, width, height);
    }
    return hWnd;
}


void
resizeWindow(HWND hWnd, int width, int height)
{
    ws::resizeWindow(hWnd, width, height);
}


bool
processEvents(void)
{
    return ws::processEvents();
}


} /* namespace d3dretrace */

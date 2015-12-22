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

#pragma once


#include <windows.h>

#include "retrace.hpp"
#include "d3dstate.hpp"


namespace d3dretrace {


extern const retrace::Entry ddraw_callbacks[];
extern const retrace::Entry d3d8_callbacks[];
extern const retrace::Entry d3d9_callbacks[];
extern const retrace::Entry dxgi_callbacks[];


template< class Device >
class D3DDumper : public retrace::Dumper {
public:
    Device *pLastDevice;

    D3DDumper() :
        pLastDevice(NULL)
    {}

    image::Image *
    getSnapshot(void) {
        if (!pLastDevice) {
            return NULL;
        }
        return d3dstate::getRenderTargetImage(pLastDevice);
    }

    bool
    canDump(void) {
        return pLastDevice;
    }

    void
    dumpState(StateWriter &writer) {
        d3dstate::dumpDevice(writer, pLastDevice);
    }

    inline void
    bindDevice(Device *pDevice) {
        pLastDevice = pDevice;
        retrace::dumper = this;
    }
    
    inline void
    unbindDevice(Device *pDevice) {
        if (pLastDevice == pDevice) {
            pLastDevice = NULL;
        }
    }
};


HWND
createWindow(int width, int height);

HWND
createWindow(HWND hWnd, int width, int height);

void
resizeWindow(HWND hWnd, int width, int height);

bool
processEvents(void);


} /* namespace d3dretrace */



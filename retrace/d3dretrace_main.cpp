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


#include <string.h>

#include "os_string.hpp"

#include "d3dstate.hpp"
#include "retrace.hpp"
#include "d3dretrace.hpp"


IDirect3DDevice9 *
d3dretrace::pLastDirect3DDevice9 = NULL;


void
retrace::setUp(void) {
    if (retrace::debug) {
        /* 
         * XXX: D3D9D only works for simple things, it often introduces errors
         * on complex traces, or traces which use unofficial D3D9 features.
         */
        if (0) {
            g_szD3D9DllName = "d3d9d.dll";
        }
    }
}


void
retrace::addCallbacks(retrace::Retracer &retracer)
{
    retracer.addCallbacks(d3dretrace::d3d9_callbacks);
}


image::Image *
retrace::getSnapshot(void) {
    if (!d3dretrace::pLastDirect3DDevice9) {
        return NULL;
    }

    return d3dstate::getRenderTargetImage(d3dretrace::pLastDirect3DDevice9);
}


bool
retrace::dumpState(std::ostream &os)
{
    if (!d3dretrace::pLastDirect3DDevice9) {
        return false;
    }

    d3dstate::dumpDevice(os, d3dretrace::pLastDirect3DDevice9);

    return true;
}


void
retrace::flushRendering(void) {
}

void
retrace::waitForInput(void) {
}

void
retrace::cleanUp(void) {
}

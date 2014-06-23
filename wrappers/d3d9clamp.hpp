/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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
 * Clamp capabilities reported by the D3D9 implementation, to make the
 * generated trace more portable.
 *
 * TODO: We should have a command line option / environment variable to enable/disable clamping
 */


#pragma once


#include <algorithm>

#include "d3d9imports.hpp"


void
clampCaps(D3DCAPS9 * pCaps)
{
    /* Some implementations only support these presentation intervals */
    pCaps->PresentationIntervals &= D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_IMMEDIATE;

    /* FIXME: Clamp other members of the D3DCAPS9 */
}


bool
clampFormat(DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
    switch ((DWORD)CheckFormat) {

    case D3DFMT_R3G3B2:
        return false;

    /* ATOC is not a true format, but an unofficial feature advertised by certain D3D9 implementations.
     *
     * FIXME: There are many more that need to be checked, sumarized in http://aras-p.info/texts/D3D9GPUHacks.html, but some of them
     * are actually widely available.
     */
    case MAKEFOURCC('A','T','O','C'):
        return false;

    /* FIXME: Much more formats necessary here.
     *
     * Maybe instead of blacklisting formats, it would be better to white-list just the supported formats
     *
     * We also need to take in consideration the Usage and RType arguments -- as some formats are only support
     * for certain Usage/RType combinations.
     */

    default:
        return true;
    }
}


bool
clampMultiSampleType(D3DFORMAT SurfaceFormat, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD * pQualityLevels)
{
    /* Never advertise MSAA support. */
    if (MultiSampleType != D3DMULTISAMPLE_NONE) {
        return false;
    }

    /* Ensure advertised formats are consistent with clampFormat() */
    if (!clampFormat(D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, SurfaceFormat)) {
        return false;
    }

    /* XXX: Anything else we need to check? */

    return true;
}

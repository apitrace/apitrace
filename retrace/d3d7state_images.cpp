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


#include <assert.h>
#include <stdint.h>

#include "image.hpp"
#include "json.hpp"
#include "com_ptr.hpp"
#include "d3dimports.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


static image::Image *
getRenderTargetImage(IDirect3DDevice7 *pDevice,
                     IDirectDrawSurface7 *pRenderTarget) 
{
    std::cerr << "warning: D3D7 snapshots not yet implemented\n";
    return NULL;
}


image::Image *
getRenderTargetImage(IDirect3DDevice7 *pDevice) {
    HRESULT hr;

    com_ptr<IDirectDrawSurface7> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (FAILED(hr)) {
        return NULL;
    }
    assert(pRenderTarget);

    return getRenderTargetImage(pDevice, pRenderTarget);
}


void
dumpTextures(JSONWriter &json, IDirect3DDevice7 *pDevice)
{
    json.beginMember("textures");
    json.beginObject();
    json.endObject();
    json.endMember(); // textures
}


void
dumpFramebuffer(JSONWriter &json, IDirect3DDevice7 *pDevice)
{
    HRESULT hr;

    json.beginMember("framebuffer");
    json.beginObject();

    com_ptr<IDirectDrawSurface7> pRenderTarget;
    hr = pDevice->GetRenderTarget(&pRenderTarget);
    if (SUCCEEDED(hr) && pRenderTarget) {
        image::Image *image;
        image = getRenderTargetImage(pDevice, pRenderTarget);
        if (image) {
            json.beginMember("RENDER_TARGET_0");
            json.writeImage(image);
            json.endMember(); // RENDER_TARGET_*
        }
    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */

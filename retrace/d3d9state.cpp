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


#include <assert.h>
#include <stdio.h>

#include <iostream>
#include <sstream>
#include <memory>

#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3d9imports.hpp"
#include "d3dshader.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


template< class T >
inline void
dumpShader(StateWriter &writer, const char *name, T *pShader) {
    if (!pShader) {
        return;
    }

    HRESULT hr;

    UINT SizeOfData = 0;

    hr = pShader->GetFunction(NULL, &SizeOfData);
    if (SUCCEEDED(hr)) {
        assert(SizeOfData % sizeof(DWORD) == 0);
        std::unique_ptr<DWORD []> pData(new DWORD[SizeOfData / sizeof(DWORD)]);
        hr = pShader->GetFunction(pData.get(), &SizeOfData);
        if (SUCCEEDED(hr)) {
            com_ptr<IDisassemblyBuffer> pDisassembly;
            hr = DisassembleShader(pData.get(), &pDisassembly);
            if (SUCCEEDED(hr)) {
                writer.beginMember(name);
                writer.writeString((const char *)pDisassembly->GetBufferPointer() /*, pDisassembly->GetBufferSize() */);
                writer.endMember();
            }
        }
    }
}

static void
dumpShaders(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    writer.beginMember("shaders");

    HRESULT hr;
    writer.beginObject();

    com_ptr<IDirect3DVertexShader9> pVertexShader;
    hr = pDevice->GetVertexShader(&pVertexShader);
    if (SUCCEEDED(hr)) {
        dumpShader<IDirect3DVertexShader9>(writer, "vertex", pVertexShader);
    }

    com_ptr<IDirect3DPixelShader9> pPixelShader;
    hr = pDevice->GetPixelShader(&pPixelShader);
    if (SUCCEEDED(hr)) {
        dumpShader<IDirect3DPixelShader9>(writer, "pixel", pPixelShader);
    }

    writer.endObject();
    writer.endMember(); // shaders
}

static void
dumpTextureStates(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
#define _DUMP_TS_INT(x) { \
    DWORD rsDword = 0; \
    pDevice->GetTextureStageState(i, x, &rsDword); \
    writer.writeIntMember(#x, rsDword); \
}

#define _DUMP_TS_FLOAT(x) { \
    float rsFloat = 0.0f; \
    pDevice->GetTextureStageState(i, x, (DWORD *)&rsFloat); \
    writer.writeFloatMember(#x, rsFloat); \
}

    for (int i = 0; i < 8; i++)
    {
        std::ostringstream oss;
        oss << "TextureStageState" << i;
        writer.beginMember(oss.str());
        writer.beginObject();

        _DUMP_TS_INT(D3DTSS_COLOROP);
        _DUMP_TS_INT(D3DTSS_COLORARG1);
        _DUMP_TS_INT(D3DTSS_COLORARG2);
        _DUMP_TS_INT(D3DTSS_ALPHAOP);
        _DUMP_TS_INT(D3DTSS_ALPHAARG1);
        _DUMP_TS_INT(D3DTSS_ALPHAARG2);
        _DUMP_TS_FLOAT(D3DTSS_BUMPENVMAT00);
        _DUMP_TS_FLOAT(D3DTSS_BUMPENVMAT01);
        _DUMP_TS_FLOAT(D3DTSS_BUMPENVMAT10);
        _DUMP_TS_FLOAT(D3DTSS_BUMPENVMAT11);
        _DUMP_TS_INT(D3DTSS_TEXCOORDINDEX);
        _DUMP_TS_FLOAT(D3DTSS_BUMPENVLSCALE);
        _DUMP_TS_FLOAT(D3DTSS_BUMPENVLOFFSET);
        _DUMP_TS_INT(D3DTSS_TEXTURETRANSFORMFLAGS);
        _DUMP_TS_INT(D3DTSS_COLORARG0);
        _DUMP_TS_INT(D3DTSS_ALPHAARG0);
        _DUMP_TS_INT(D3DTSS_RESULTARG);
        _DUMP_TS_INT(D3DTSS_CONSTANT);

        writer.endObject();
        writer.endMember();
    }

#undef _DUMP_TS_INT
#undef _DUMP_TS_FLOAT
}

static void
dumpViewport(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    writer.beginMember("Viewport");
    writer.beginObject();

    D3DVIEWPORT9 vp;
    pDevice->GetViewport(&vp);
    writer.writeIntMember("X", vp.X);
    writer.writeIntMember("Y", vp.Y);
    writer.writeIntMember("Width", vp.Width);
    writer.writeIntMember("Height", vp.Height);
    writer.writeFloatMember("MinZ", vp.MinZ);
    writer.writeFloatMember("MaxZ", vp.MaxZ);

    writer.endObject();
    writer.endMember();
}

static void
dumpRenderstate(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
#define _DUMP_RS_INT(x) { \
    DWORD rsDword = 0; \
    pDevice->GetRenderState(x, &rsDword); \
    writer.writeIntMember(#x, rsDword); \
}

#define _DUMP_RS_FLOAT(x) { \
    float rsFloat = 0.0f; \
    pDevice->GetRenderState(x, (DWORD *)&rsFloat); \
    writer.writeFloatMember(#x, rsFloat); \
}

    writer.beginMember("parameters");
    writer.beginObject();

    _DUMP_RS_INT(D3DRS_ZENABLE);
    _DUMP_RS_INT(D3DRS_FILLMODE);
    _DUMP_RS_INT(D3DRS_SHADEMODE);
    _DUMP_RS_INT(D3DRS_ZWRITEENABLE);
    _DUMP_RS_INT(D3DRS_ALPHATESTENABLE);
    _DUMP_RS_INT(D3DRS_LASTPIXEL);
    _DUMP_RS_INT(D3DRS_SRCBLEND);
    _DUMP_RS_INT(D3DRS_DESTBLEND);
    _DUMP_RS_INT(D3DRS_CULLMODE);
    _DUMP_RS_INT(D3DRS_ZFUNC);
    _DUMP_RS_INT(D3DRS_ALPHAREF);
    _DUMP_RS_INT(D3DRS_ALPHAFUNC);
    _DUMP_RS_INT(D3DRS_DITHERENABLE);
    _DUMP_RS_INT(D3DRS_ALPHABLENDENABLE);
    _DUMP_RS_INT(D3DRS_FOGENABLE);
    _DUMP_RS_INT(D3DRS_SPECULARENABLE);
    _DUMP_RS_INT(D3DRS_FOGCOLOR);
    _DUMP_RS_INT(D3DRS_FOGTABLEMODE);
    _DUMP_RS_FLOAT(D3DRS_FOGSTART);
    _DUMP_RS_FLOAT(D3DRS_FOGEND);
    _DUMP_RS_FLOAT(D3DRS_FOGDENSITY);
    _DUMP_RS_INT(D3DRS_RANGEFOGENABLE);
    _DUMP_RS_INT(D3DRS_STENCILENABLE);
    _DUMP_RS_INT(D3DRS_STENCILFAIL);
    _DUMP_RS_INT(D3DRS_STENCILZFAIL);
    _DUMP_RS_INT(D3DRS_STENCILPASS);
    _DUMP_RS_INT(D3DRS_STENCILFUNC);
    _DUMP_RS_INT(D3DRS_STENCILREF);
    _DUMP_RS_INT(D3DRS_STENCILMASK);
    _DUMP_RS_INT(D3DRS_STENCILWRITEMASK);
    _DUMP_RS_INT(D3DRS_TEXTUREFACTOR);
    _DUMP_RS_INT(D3DRS_WRAP0);
    _DUMP_RS_INT(D3DRS_WRAP1);
    _DUMP_RS_INT(D3DRS_WRAP2);
    _DUMP_RS_INT(D3DRS_WRAP3);
    _DUMP_RS_INT(D3DRS_WRAP4);
    _DUMP_RS_INT(D3DRS_WRAP5);
    _DUMP_RS_INT(D3DRS_WRAP6);
    _DUMP_RS_INT(D3DRS_WRAP7);
    _DUMP_RS_INT(D3DRS_CLIPPING);
    _DUMP_RS_INT(D3DRS_LIGHTING);
    _DUMP_RS_FLOAT(D3DRS_AMBIENT);
    _DUMP_RS_INT(D3DRS_FOGVERTEXMODE);
    _DUMP_RS_INT(D3DRS_COLORVERTEX);
    _DUMP_RS_INT(D3DRS_LOCALVIEWER);
    _DUMP_RS_INT(D3DRS_NORMALIZENORMALS);
    _DUMP_RS_INT(D3DRS_DIFFUSEMATERIALSOURCE);
    _DUMP_RS_INT(D3DRS_SPECULARMATERIALSOURCE);
    _DUMP_RS_INT(D3DRS_AMBIENTMATERIALSOURCE);
    _DUMP_RS_INT(D3DRS_EMISSIVEMATERIALSOURCE);
    _DUMP_RS_INT(D3DRS_VERTEXBLEND);
    _DUMP_RS_INT(D3DRS_CLIPPLANEENABLE);
    _DUMP_RS_FLOAT(D3DRS_POINTSIZE);
    _DUMP_RS_FLOAT(D3DRS_POINTSIZE_MIN);
    _DUMP_RS_INT(D3DRS_POINTSPRITEENABLE);
    _DUMP_RS_INT(D3DRS_POINTSCALEENABLE);
    _DUMP_RS_FLOAT(D3DRS_POINTSCALE_A);
    _DUMP_RS_FLOAT(D3DRS_POINTSCALE_B);
    _DUMP_RS_FLOAT(D3DRS_POINTSCALE_C);
    _DUMP_RS_INT(D3DRS_MULTISAMPLEANTIALIAS);
    _DUMP_RS_INT(D3DRS_MULTISAMPLEMASK);
    _DUMP_RS_INT(D3DRS_PATCHEDGESTYLE);
    _DUMP_RS_INT(D3DRS_DEBUGMONITORTOKEN);
    _DUMP_RS_FLOAT(D3DRS_POINTSIZE_MAX);
    _DUMP_RS_INT(D3DRS_INDEXEDVERTEXBLENDENABLE);
    _DUMP_RS_INT(D3DRS_COLORWRITEENABLE);
    _DUMP_RS_INT(D3DRS_TWEENFACTOR);
    _DUMP_RS_INT(D3DRS_BLENDOP);
    _DUMP_RS_INT(D3DRS_POSITIONDEGREE);
    _DUMP_RS_INT(D3DRS_NORMALDEGREE);
    _DUMP_RS_INT(D3DRS_SCISSORTESTENABLE);
    _DUMP_RS_INT(D3DRS_SLOPESCALEDEPTHBIAS);
    _DUMP_RS_INT(D3DRS_ANTIALIASEDLINEENABLE);
    _DUMP_RS_INT(D3DRS_MINTESSELLATIONLEVEL);
    _DUMP_RS_INT(D3DRS_MAXTESSELLATIONLEVEL);
    _DUMP_RS_FLOAT(D3DRS_ADAPTIVETESS_X);
    _DUMP_RS_FLOAT(D3DRS_ADAPTIVETESS_Y);
    _DUMP_RS_FLOAT(D3DRS_ADAPTIVETESS_Z);
    _DUMP_RS_FLOAT(D3DRS_ADAPTIVETESS_W);
    _DUMP_RS_INT(D3DRS_ENABLEADAPTIVETESSELLATION);
    _DUMP_RS_INT(D3DRS_TWOSIDEDSTENCILMODE);
    _DUMP_RS_INT(D3DRS_CCW_STENCILFAIL);
    _DUMP_RS_INT(D3DRS_CCW_STENCILZFAIL);
    _DUMP_RS_INT(D3DRS_CCW_STENCILPASS);
    _DUMP_RS_INT(D3DRS_CCW_STENCILFUNC);
    _DUMP_RS_INT(D3DRS_COLORWRITEENABLE1);
    _DUMP_RS_INT(D3DRS_COLORWRITEENABLE2);
    _DUMP_RS_INT(D3DRS_COLORWRITEENABLE3);
    _DUMP_RS_INT(D3DRS_BLENDFACTOR);
    _DUMP_RS_INT(D3DRS_SRGBWRITEENABLE);
    _DUMP_RS_FLOAT(D3DRS_DEPTHBIAS);
    _DUMP_RS_INT(D3DRS_WRAP8);
    _DUMP_RS_INT(D3DRS_WRAP9);
    _DUMP_RS_INT(D3DRS_WRAP10);
    _DUMP_RS_INT(D3DRS_WRAP11);
    _DUMP_RS_INT(D3DRS_WRAP12);
    _DUMP_RS_INT(D3DRS_WRAP13);
    _DUMP_RS_INT(D3DRS_WRAP14);
    _DUMP_RS_INT(D3DRS_WRAP15);
    _DUMP_RS_INT(D3DRS_SEPARATEALPHABLENDENABLE);
    _DUMP_RS_INT(D3DRS_SRCBLENDALPHA);
    _DUMP_RS_INT(D3DRS_DESTBLENDALPHA);
    _DUMP_RS_INT(D3DRS_BLENDOPALPHA);

#undef _DUMP_RS_INT
#undef _DUMP_RS_FLOAT

    dumpViewport(writer, pDevice);

    dumpTextureStates(writer, pDevice);

    writer.endObject();
    writer.endMember();
}

void
dumpDevice(StateWriter &writer, IDirect3DDevice9 *pDevice)
{
    dumpRenderstate(writer, pDevice);

    dumpShaders(writer, pDevice);

    dumpTextures(writer, pDevice);

    dumpFramebuffer(writer, pDevice);
}

void
dumpDevice(StateWriter &writer, IDirect3DSwapChain9 *pSwapChain)
{
    com_ptr<IDirect3DDevice9> pDevice;
    HRESULT hr = pSwapChain->GetDevice(&pDevice);
    if (SUCCEEDED(hr)) {
        dumpDevice(writer, pDevice);
    }
}


} /* namespace d3dstate */

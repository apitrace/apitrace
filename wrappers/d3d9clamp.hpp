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


/**
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

    /* FIXME: Much more clamping necessary here!! */
}

/*
 * D3DFORMAT_OP_* flags
 * http://msdn.microsoft.com/en-us/library/windows/hardware/ff550274.aspx
 */
#define D3DFORMAT_OP_TEXTURE                              0x00000001L
#define D3DFORMAT_OP_VOLUMETEXTURE                        0x00000002L
#define D3DFORMAT_OP_CUBETEXTURE                          0x00000004L
#define D3DFORMAT_OP_OFFSCREEN_RENDERTARGET               0x00000008L
#define D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET             0x00000010L
#define D3DFORMAT_OP_ZSTENCIL                             0x00000040L
#define D3DFORMAT_OP_ZSTENCIL_WITH_ARBITRARY_COLOR_DEPTH  0x00000080L
#define D3DFORMAT_OP_SAME_FORMAT_UP_TO_ALPHA_RENDERTARGET 0x00000100L
#define D3DFORMAT_OP_DISPLAYMODE                          0x00000400L
#define D3DFORMAT_OP_3DACCELERATION                       0x00000800L
#define D3DFORMAT_OP_PIXELSIZE                            0x00001000L
#define D3DFORMAT_OP_CONVERT_TO_ARGB                      0x00002000L
#define D3DFORMAT_OP_OFFSCREENPLAIN                       0x00004000L
#define D3DFORMAT_OP_SRGBREAD                             0x00008000L
#define D3DFORMAT_OP_BUMPMAP                              0x00010000L
#define D3DFORMAT_OP_DMAP                                 0x00020000L
#define D3DFORMAT_OP_NOFILTER                             0x00040000L
#define D3DFORMAT_MEMBEROFGROUP_ARGB                      0x00080000L
#define D3DFORMAT_OP_SRGBWRITE                            0x00100000L
#define D3DFORMAT_OP_NOALPHABLEND                         0x00200000L
#define D3DFORMAT_OP_AUTOGENMIPMAP                        0x00400000L
#define D3DFORMAT_OP_VERTEXTEXTURE                        0x00800000L
#define D3DFORMAT_OP_NOTEXCOORDWRAPNORMIP	          0x01000000L


#define D3DFMT_X8R8G8B8_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_VOLUMETEXTURE | \
                              D3DFORMAT_OP_DISPLAYMODE | \
                              D3DFORMAT_OP_3DACCELERATION | \
                              D3DFORMAT_OP_CONVERT_TO_ARGB | \
                              D3DFORMAT_MEMBEROFGROUP_ARGB | \
                              D3DFORMAT_OP_OFFSCREENPLAIN | \
                              D3DFORMAT_OP_SRGBREAD | \
                              D3DFORMAT_OP_SRGBWRITE | \
                              D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                              D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_A8R8G8B8_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_VOLUMETEXTURE | \
                              D3DFORMAT_OP_CONVERT_TO_ARGB | \
                              D3DFORMAT_MEMBEROFGROUP_ARGB | \
                              D3DFORMAT_OP_OFFSCREENPLAIN | \
                              D3DFORMAT_OP_SRGBREAD | \
                              D3DFORMAT_OP_SRGBWRITE | \
                              D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                              D3DFORMAT_OP_SAME_FORMAT_UP_TO_ALPHA_RENDERTARGET | \
                              D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_A2R10G10B10_LVL1 (D3DFORMAT_OP_TEXTURE | \
                                 D3DFORMAT_OP_CUBETEXTURE | \
                                 D3DFORMAT_OP_VOLUMETEXTURE | \
                                 D3DFORMAT_OP_CONVERT_TO_ARGB | \
                                 D3DFORMAT_MEMBEROFGROUP_ARGB | \
                                 D3DFORMAT_OP_OFFSCREENPLAIN | \
                                 D3DFORMAT_OP_SRGBREAD | \
                                 D3DFORMAT_OP_SRGBWRITE | \
                                 D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                                 D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_X1R5G5B5_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_VOLUMETEXTURE | \
                              D3DFORMAT_OP_CONVERT_TO_ARGB | \
                              D3DFORMAT_MEMBEROFGROUP_ARGB | \
                              D3DFORMAT_OP_OFFSCREENPLAIN | \
                              D3DFORMAT_OP_SRGBREAD | \
                              D3DFORMAT_OP_SRGBWRITE | \
                              D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                              D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_A1R5G5B5_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_VOLUMETEXTURE | \
                              D3DFORMAT_OP_CONVERT_TO_ARGB | \
                              D3DFORMAT_MEMBEROFGROUP_ARGB | \
                              D3DFORMAT_OP_OFFSCREENPLAIN | \
                              D3DFORMAT_OP_SRGBREAD | \
                              D3DFORMAT_OP_SRGBWRITE | \
                              D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                              D3DFORMAT_OP_SAME_FORMAT_UP_TO_ALPHA_RENDERTARGET | \
                              D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_A4R4G4B4_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_VOLUMETEXTURE | \
                              D3DFORMAT_OP_OFFSCREENPLAIN | \
                              D3DFORMAT_OP_SRGBREAD | \
                              D3DFORMAT_OP_SRGBWRITE | \
                              D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                              D3DFORMAT_OP_SAME_FORMAT_UP_TO_ALPHA_RENDERTARGET | \
                              D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_R5G6B5_LVL1 (D3DFORMAT_OP_TEXTURE | \
                            D3DFORMAT_OP_CUBETEXTURE | \
                            D3DFORMAT_OP_VOLUMETEXTURE | \
                            D3DFORMAT_OP_DISPLAYMODE | \
                            D3DFORMAT_OP_3DACCELERATION | \
                            D3DFORMAT_OP_CONVERT_TO_ARGB | \
                            D3DFORMAT_MEMBEROFGROUP_ARGB | \
                            D3DFORMAT_OP_OFFSCREENPLAIN | \
                            D3DFORMAT_OP_SRGBREAD | \
                            D3DFORMAT_OP_SRGBWRITE | \
                            D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                            D3DFORMAT_OP_OFFSCREEN_RENDERTARGET)

#define D3DFMT_LA_LVL1 (D3DFORMAT_OP_TEXTURE | \
                        D3DFORMAT_OP_CUBETEXTURE | \
                        D3DFORMAT_OP_VOLUMETEXTURE | \
                        D3DFORMAT_OP_OFFSCREENPLAIN)


#define D3DFMT_DS_LVL1 (D3DFORMAT_OP_TEXTURE |  \
                        D3DFORMAT_OP_ZSTENCIL |                      \
                        D3DFORMAT_OP_ZSTENCIL_WITH_ARBITRARY_COLOR_DEPTH)

#define D3DFMT_DXTC_LVL1 (D3DFORMAT_OP_TEXTURE | \
                          D3DFORMAT_OP_SRGBREAD | \
                          D3DFORMAT_OP_CUBETEXTURE | \
                          D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_X8L8V8U8_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_BUMPMAP | \
                              D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_A2W10V10U10_LVL1 (D3DFORMAT_OP_TEXTURE | \
                                 D3DFORMAT_OP_BUMPMAP | \
                                 D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_U8V8_LVL1 (D3DFORMAT_OP_TEXTURE | \
                          D3DFORMAT_OP_CUBETEXTURE | \
                          D3DFORMAT_OP_VOLUMETEXTURE | \
                          D3DFORMAT_OP_BUMPMAP | \
                          D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_Q8W8V8U8_LVL1 (D3DFORMAT_OP_TEXTURE | \
                              D3DFORMAT_OP_CUBETEXTURE | \
                              D3DFORMAT_OP_BUMPMAP | \
                              D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_CxV8U8_LVL1 (D3DFORMAT_OP_TEXTURE | \
                            D3DFORMAT_OP_BUMPMAP | \
                            D3DFORMAT_OP_OFFSCREENPLAIN)


#define D3DFMT_S10E5_LVL1 (D3DFORMAT_OP_TEXTURE |                  \
                           D3DFORMAT_OP_VOLUMETEXTURE |            \
                           D3DFORMAT_OP_CUBETEXTURE |              \
                           D3DFORMAT_OP_OFFSCREEN_RENDERTARGET |   \
                           D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                           D3DFORMAT_OP_VERTEXTEXTURE)

#define D3DFMT_S23E8_LVL1 (D3DFORMAT_OP_TEXTURE |                  \
                           D3DFORMAT_OP_VOLUMETEXTURE |            \
                           D3DFORMAT_OP_CUBETEXTURE |              \
                           D3DFORMAT_OP_OFFSCREEN_RENDERTARGET |   \
                           D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                           D3DFORMAT_OP_NOFILTER |                 \
                           D3DFORMAT_OP_VERTEXTEXTURE)

#define D3DFMT_V16U16_LVL1 (D3DFORMAT_OP_TEXTURE |        \
                            D3DFORMAT_OP_CUBETEXTURE |    \
                            D3DFORMAT_OP_OFFSCREENPLAIN | \
                            D3DFORMAT_OP_BUMPMAP)

#define D3DFMT_G16R16_LVL1 (D3DFORMAT_OP_TEXTURE |                  \
                            D3DFORMAT_OP_VOLUMETEXTURE |            \
                            D3DFORMAT_OP_CUBETEXTURE |              \
                            D3DFORMAT_OP_OFFSCREEN_RENDERTARGET |   \
                            D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                            D3DFORMAT_OP_OFFSCREENPLAIN |           \
                            D3DFORMAT_OP_NOALPHABLEND)

#define D3DFMT_YUV_LVL1 (D3DFORMAT_OP_CONVERT_TO_ARGB | \
                         D3DFORMAT_OP_OFFSCREENPLAIN |  \
                         D3DFORMAT_OP_NOFILTER |        \
                         D3DFORMAT_OP_NOALPHABLEND |    \
                         D3DFORMAT_OP_NOTEXCOORDWRAPNORMIP)

#define D3DFMT_RGTC_LVL2 (D3DFORMAT_OP_TEXTURE | \
                          D3DFORMAT_OP_CUBETEXTURE | \
                          D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_S10E5_LVL3 (D3DFMT_S10E5_LVL1 | \
                           D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_S23E8_LVL3 (D3DFMT_S23E8_LVL1 | \
                           D3DFORMAT_OP_OFFSCREENPLAIN)

#define D3DFMT_A4R4G4B4_LVL4 (D3DFMT_A4R4G4B4_LVL1 |            \
                              D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_LA_LVL4 (D3DFMT_LA_LVL1 |                        \
                        D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_DXTC_LVL4 (D3DFMT_DXTC_LVL1 |                    \
                          D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_S10E5_LVL4 (D3DFMT_S10E5_LVL3 |                  \
                           D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_S23E8_LVL4 (D3DFMT_S23E8_LVL3 |                  \
                           D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_V16U16_LVL4 (D3DFMT_V16U16_LVL1 |                \
                            D3DFORMAT_OP_VOLUMETEXTURE)

#define D3DFMT_G16R16_LVL4 (D3DFMT_G16R16_LVL1 |                \
                            D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_RGTC_LVL4 (D3DFMT_RGTC_LVL2 |                    \
                          D3DFORMAT_OP_CONVERT_TO_ARGB)

#define D3DFMT_S23E8_LVL5 (D3DFORMAT_OP_TEXTURE |                  \
                           D3DFORMAT_OP_VOLUMETEXTURE |            \
                           D3DFORMAT_OP_CUBETEXTURE |              \
                           D3DFORMAT_OP_OFFSCREEN_RENDERTARGET |   \
                           D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                           D3DFORMAT_OP_VERTEXTEXTURE)

#define D3DFMT_G16R16_LVL5 (D3DFORMAT_OP_TEXTURE |                  \
                            D3DFORMAT_OP_VOLUMETEXTURE |            \
                            D3DFORMAT_OP_CUBETEXTURE |              \
                            D3DFORMAT_OP_OFFSCREEN_RENDERTARGET |   \
                            D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET | \
                            D3DFORMAT_OP_OFFSCREENPLAIN)

static struct {
    D3DFORMAT Format;
    DWORD Ops;
} FormatOps[] = {
    {D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8_LVL1},
    {D3DFMT_A8R8G8B8, D3DFMT_A8R8G8B8_LVL1},
    {D3DFMT_A2R10G10B10, D3DFMT_A2R10G10B10_LVL1},
    {D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5_LVL1},
    {D3DFMT_A1R5G5B5, D3DFMT_A1R5G5B5_LVL1},
    {D3DFMT_A4R4G4B4, D3DFMT_A4R4G4B4_LVL4},
    {D3DFMT_R5G6B5, D3DFMT_R5G6B5_LVL1},
    {D3DFMT_L16, D3DFMT_LA_LVL4},
    {D3DFMT_A8L8, D3DFMT_LA_LVL4},
    {D3DFMT_A8, D3DFMT_LA_LVL4},
    {D3DFMT_L8, D3DFMT_LA_LVL4},
    {D3DFMT_D16, D3DFMT_DS_LVL1},
    {D3DFMT_D24S8, D3DFMT_DS_LVL1},
    {D3DFMT_D24X8, D3DFMT_DS_LVL1},
    {D3DFMT_DXT1, D3DFMT_DXTC_LVL4},
    {D3DFMT_DXT2, D3DFMT_DXTC_LVL4},
    {D3DFMT_DXT3, D3DFMT_DXTC_LVL4},
    {D3DFMT_DXT4, D3DFMT_DXTC_LVL4},
    {D3DFMT_DXT5, D3DFMT_DXTC_LVL4},
    {D3DFMT_X8L8V8U8, D3DFMT_X8L8V8U8_LVL1},
    {D3DFMT_A2W10V10U10, D3DFMT_A2W10V10U10_LVL1},
    {D3DFMT_V8U8, D3DFMT_U8V8_LVL1},
    {D3DFMT_Q8W8V8U8, D3DFMT_Q8W8V8U8_LVL1},
    {D3DFMT_CxV8U8, D3DFMT_CxV8U8_LVL1},
    {D3DFMT_R16F, D3DFMT_S10E5_LVL4},
    {D3DFMT_R32F, D3DFMT_S23E8_LVL5},
    {D3DFMT_G16R16F, D3DFMT_S10E5_LVL4},
    {D3DFMT_G32R32F, D3DFMT_S23E8_LVL5},
    {D3DFMT_A16B16G16R16F, D3DFMT_S10E5_LVL4},
    {D3DFMT_A32B32G32R32F, D3DFMT_S23E8_LVL5},
    {D3DFMT_V16U16, D3DFMT_V16U16_LVL4},
    {D3DFMT_G16R16, D3DFMT_G16R16_LVL5},
    {D3DFMT_A16B16G16R16, D3DFMT_G16R16_LVL5},
    {D3DFMT_UYVY, D3DFMT_YUV_LVL1},
    {D3DFMT_YUY2, D3DFMT_YUV_LVL1},
    {D3DFMT_NV12, D3DFMT_YUV_LVL1},
    {D3DFMT_DF16, D3DFMT_DS_LVL1},
    {D3DFMT_DF24, D3DFMT_DS_LVL1},
    {D3DFMT_INTZ, D3DFMT_DS_LVL1},
    {D3DFMT_ATI1N, D3DFMT_RGTC_LVL4},
    {D3DFMT_ATI2N, D3DFMT_RGTC_LVL4},
    {D3DFMT_YV12, D3DFMT_YUV_LVL1},
};


bool
clampFormat(D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
    /* Doesn't really matter */
    (void)AdapterFormat;

    DWORD RequiredOps = 0;
    DWORD ForbiddenOps = 0;

    switch (RType) {
    case D3DRTYPE_SURFACE:
        RequiredOps |= D3DFORMAT_OP_OFFSCREENPLAIN;
        break;
    case D3DRTYPE_TEXTURE:
        RequiredOps |= D3DFORMAT_OP_TEXTURE;
        break;
    case D3DRTYPE_CUBETEXTURE:
        RequiredOps |= D3DFORMAT_OP_CUBETEXTURE;
        break;
    case D3DRTYPE_VOLUME:
    case D3DRTYPE_VOLUMETEXTURE:
        RequiredOps |= D3DFORMAT_OP_VOLUMETEXTURE;
        break;
    /* FIXME: Handle other D3DRTYPE_* */
    default:
        assert(0);
    }

    /* Only a subset of D3DUSAGE_* is passed here.
     * See http://msdn.microsoft.com/en-gb/library/windows/desktop/bb172625.aspx */
    if (Usage & D3DUSAGE_RENDERTARGET) RequiredOps |= D3DFORMAT_OP_OFFSCREEN_RENDERTARGET;
    if (Usage & D3DUSAGE_DEPTHSTENCIL) RequiredOps |= D3DFORMAT_OP_ZSTENCIL;
    if (Usage & D3DUSAGE_SOFTWAREPROCESSING) /* XXX: ignore? */;
    if (Usage & D3DUSAGE_DYNAMIC) /* XXX: ignore? */;
    if (Usage & D3DUSAGE_AUTOGENMIPMAP) RequiredOps |= D3DFORMAT_OP_AUTOGENMIPMAP;
    if (Usage & D3DUSAGE_DMAP) RequiredOps |= D3DFORMAT_OP_DMAP;

    if (Usage & D3DUSAGE_QUERY_LEGACYBUMPMAP) RequiredOps |= D3DFORMAT_OP_BUMPMAP;
    if (Usage & D3DUSAGE_QUERY_SRGBREAD) RequiredOps |= D3DFORMAT_OP_SRGBREAD;
    if (Usage & D3DUSAGE_QUERY_FILTER) ForbiddenOps |= D3DFORMAT_OP_NOFILTER;
    if (Usage & D3DUSAGE_QUERY_SRGBWRITE) RequiredOps |= D3DFORMAT_OP_SRGBWRITE;
    if (Usage & D3DUSAGE_QUERY_VERTEXTEXTURE) RequiredOps |= D3DFORMAT_OP_VERTEXTEXTURE;
    if (Usage & D3DUSAGE_QUERY_WRAPANDMIP) ForbiddenOps |= D3DFORMAT_OP_NOTEXCOORDWRAPNORMIP;


    for (DWORD i = 0; i < ARRAYSIZE(FormatOps); ++i) {
        if (FormatOps[i].Format == CheckFormat) {
            DWORD SupportedOps = FormatOps[i].Ops;

            if ((SupportedOps & RequiredOps) != RequiredOps) {
                /* Some of the required ops are not supported. */
                return false;
            }

            /* Handle negated D3DFORMAT_OP_NO* flags */
            if (SupportedOps & ForbiddenOps) {
                /* Some of the negated ops are advertised. */
                return false;
            }

            return true;
        }
    }

    /* Unsupported format */
    return false;
}


bool
clampMultiSampleType(D3DFORMAT SurfaceFormat, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD * pQualityLevels)
{
    // Don't advertise MSAA
    if (MultiSampleType != D3DMULTISAMPLE_NONE) {
        return false;
    }

    return true;
}

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


/**
 * Label functions based on their name.
 */


#include "trace_lookup.hpp"
#include "trace_parser.hpp"


using namespace trace;


/**
 * Shortcut for SwapBuffers, which terminate and swap bound render buffer.
 */
#define CALL_FLAG_SWAPBUFFERS (CALL_FLAG_END_FRAME | CALL_FLAG_SWAP_RENDERTARGET)



/** 
 * Default call flags.
 */
const CallFlags
defaultCallFlags = 0;


/**
 * Call flags lookup table.
 */
const Entry<CallFlags>
callFlagTable[] = {
    { "CGLFlushDrawable",                              CALL_FLAG_END_FRAME },
    { "CGLGetCurrentContext",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "ID3D10Device1::CheckMultisampleQualityLevels",  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "ID3D10Device1::Draw",                           CALL_FLAG_RENDER },
    { "ID3D10Device1::DrawAuto",                       CALL_FLAG_RENDER },
    { "ID3D10Device1::DrawIndexed",                    CALL_FLAG_RENDER },
    { "ID3D10Device1::DrawIndexedInstanced",           CALL_FLAG_RENDER },
    { "ID3D10Device1::DrawInstanced",                  CALL_FLAG_RENDER },
    { "ID3D10Device1::OMSetRenderTargets",             CALL_FLAG_SWAP_RENDERTARGET },
    { "ID3D10Device::CheckMultisampleQualityLevels",   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "ID3D10Device::Draw",                            CALL_FLAG_RENDER },
    { "ID3D10Device::DrawAuto",                        CALL_FLAG_RENDER },
    { "ID3D10Device::DrawIndexed",                     CALL_FLAG_RENDER },
    { "ID3D10Device::DrawIndexedInstanced",            CALL_FLAG_RENDER },
    { "ID3D10Device::DrawInstanced",                   CALL_FLAG_RENDER },
    { "ID3D10Device::OMSetRenderTargets",              CALL_FLAG_SWAP_RENDERTARGET },
    { "ID3D11Device::CheckMultisampleQualityLevels",   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "ID3D11DeviceContext::Draw",                          CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::DrawAuto",                      CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::DrawIndexed",                   CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::DrawIndexedInstanced",          CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::DrawIndexedInstancedIndirect",  CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::DrawInstanced",                 CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::DrawInstancedIndirect",         CALL_FLAG_RENDER },
    { "ID3D11DeviceContext::OMSetRenderTargets",       CALL_FLAG_SWAP_RENDERTARGET },
    { "ID3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews", CALL_FLAG_SWAP_RENDERTARGET },
    { "IDXGISwapChain::Present",                       CALL_FLAG_SWAPBUFFERS },
    { "IDXGISwapChainDWM::Present",                    CALL_FLAG_SWAPBUFFERS },
    { "IDirect3D9::CheckDeviceFormat",                 CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::EnumAdapterModes",                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::GetAdapterModeCount",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::GetDeviceCaps",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::CheckDeviceFormat",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::EnumAdapterModes",                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::GetAdapterModeCount",             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::GetDeviceCaps",                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3DDevice9::Clear",                       CALL_FLAG_RENDER },
    { "IDirect3DDevice9::DrawIndexedPrimitive",        CALL_FLAG_RENDER },
    { "IDirect3DDevice9::DrawIndexedPrimitiveUP",      CALL_FLAG_RENDER },
    { "IDirect3DDevice9::DrawPrimitive",               CALL_FLAG_RENDER },
    { "IDirect3DDevice9::DrawPrimitiveUP",             CALL_FLAG_RENDER },
    { "IDirect3DDevice9::DrawRectPatch",               CALL_FLAG_RENDER },
    { "IDirect3DDevice9::DrawTriPatch",                CALL_FLAG_RENDER },
    { "IDirect3DDevice9::GetDeviceCaps",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3DDevice9::GetRenderTargetData",         CALL_FLAG_END_FRAME },
    { "IDirect3DDevice9::Present",                     CALL_FLAG_SWAPBUFFERS },
    { "IDirect3DDevice9::SetRenderTarget",             CALL_FLAG_SWAP_RENDERTARGET },
    { "IDirect3DDevice9Ex::Clear",                     CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::DrawIndexedPrimitive",      CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::DrawIndexedPrimitiveUP",    CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::DrawPrimitive",             CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::DrawPrimitiveUP",           CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::DrawRectPatch",             CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::DrawTriPatch",              CALL_FLAG_RENDER },
    { "IDirect3DDevice9Ex::GetDeviceCaps",             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3DDevice9Ex::GetRenderTargetData",       CALL_FLAG_END_FRAME },
    { "IDirect3DDevice9Ex::Present",                   CALL_FLAG_SWAPBUFFERS },
    { "IDirect3DDevice9Ex::PresentEx",                 CALL_FLAG_SWAPBUFFERS },
    { "IDirect3DDevice9Ex::SetRenderTarget",           CALL_FLAG_SWAP_RENDERTARGET },
    { "IDirect3DSwapChain9::Present",                  CALL_FLAG_SWAPBUFFERS },
    { "IDirect3DSwapChain9Ex::Present",                CALL_FLAG_SWAPBUFFERS },
    { "eglGetProcAddress",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "eglQueryString",                                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "eglSwapBuffers",                                CALL_FLAG_SWAPBUFFERS },
    { "glBindFramebuffer",                             CALL_FLAG_SWAP_RENDERTARGET },
    { "glBindFramebufferEXT",                          CALL_FLAG_SWAP_RENDERTARGET },
    { "glBindFramebufferOES",                          CALL_FLAG_SWAP_RENDERTARGET },
    { "glBlitFramebuffer",                             CALL_FLAG_RENDER },
    { "glBlitFramebufferEXT",                          CALL_FLAG_RENDER },
    { "glCallList",                                    CALL_FLAG_RENDER },
    { "glCallLists",                                   CALL_FLAG_RENDER },
    { "glClear",                                       CALL_FLAG_RENDER },
    { "glDrawArrays",                                  CALL_FLAG_RENDER },
    { "glDrawArraysEXT",                               CALL_FLAG_RENDER },
    { "glDrawArraysIndirect",                          CALL_FLAG_RENDER },
    { "glDrawArraysInstanced",                         CALL_FLAG_RENDER },
    { "glDrawArraysInstancedARB",                      CALL_FLAG_RENDER },
    { "glDrawArraysInstancedBaseInstance",             CALL_FLAG_RENDER },
    { "glDrawArraysInstancedEXT",                      CALL_FLAG_RENDER },
    { "glDrawElements",                                CALL_FLAG_RENDER },
    { "glDrawElementsBaseVertex",                      CALL_FLAG_RENDER },
    { "glDrawElementsIndirect",                        CALL_FLAG_RENDER },
    { "glDrawElementsInstanced",                       CALL_FLAG_RENDER },
    { "glDrawElementsInstancedARB",                    CALL_FLAG_RENDER },
    { "glDrawElementsInstancedBaseInstance",           CALL_FLAG_RENDER },
    { "glDrawElementsInstancedBaseVertex",             CALL_FLAG_RENDER },
    { "glDrawElementsInstancedBaseVertexBaseInstance", CALL_FLAG_RENDER },
    { "glDrawElementsInstancedEXT",                    CALL_FLAG_RENDER },
    { "glDrawMeshArraysSUN",                           CALL_FLAG_RENDER },
    { "glDrawPixels",                                  CALL_FLAG_RENDER },
    { "glDrawRangeElements",                           CALL_FLAG_RENDER },
    { "glDrawRangeElementsBaseVertex",                 CALL_FLAG_RENDER },
    { "glDrawRangeElementsEXT",                        CALL_FLAG_RENDER },
    { "glEnd",                                         CALL_FLAG_RENDER },
    { "glFrameTerminatorGREMEDY",                      CALL_FLAG_END_FRAME },
    { "glGetError",                                    CALL_FLAG_NO_SIDE_EFFECTS }, // verbose will be set later for GL_NO_ERROR 
    { "glGetString",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glGetStringi",                                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsAsyncMarkerSGIX",                           CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsBuffer",                                    CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsBufferARB",                                 CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsBufferResidentNV",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsEnabled",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsEnabledIndexedEXT",                         CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsEnabledi",                                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsFenceAPPLE",                                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsFenceNV",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsFramebuffer",                               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsFramebufferEXT",                            CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsList",                                      CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsNameAMD",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsNamedBufferResidentNV",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsNamedStringARB",                            CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsObjectBufferATI",                           CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsOcclusionQueryNV",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsProgram",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsProgramARB",                                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsProgramNV",                                 CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsProgramPipeline",                           CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsQuery",                                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsQueryARB",                                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsRenderbuffer",                              CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsRenderbufferEXT",                           CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsSampler",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsShader",                                    CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsSync",                                      CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsTexture",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsTextureEXT",                                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsTransformFeedback",                         CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsTransformFeedbackNV",                       CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsVariantEnabledEXT",                         CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsVertexArray",                               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsVertexArrayAPPLE",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glIsVertexAttribEnabledAPPLE",                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glMultiDrawArrays",                             CALL_FLAG_RENDER },
    { "glMultiDrawArraysEXT",                          CALL_FLAG_RENDER },
    { "glMultiDrawElements",                           CALL_FLAG_RENDER },
    { "glMultiDrawElementsBaseVertex",                 CALL_FLAG_RENDER },
    { "glMultiDrawElementsEXT",                        CALL_FLAG_RENDER },
    { "glMultiModeDrawArraysIBM",                      CALL_FLAG_RENDER },
    { "glMultiModeDrawElementsIBM",                    CALL_FLAG_RENDER },
    { "glXGetClientString",                            CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentContext",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentDisplay",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentDrawable",                         CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetFBConfigAttrib",                          CALL_FLAG_VERBOSE },
    { "glXGetFBConfigAttribSGIX",                      CALL_FLAG_VERBOSE },
    { "glXGetProcAddress",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetProcAddressARB",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXIsDirect",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXQueryExtension",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXQueryExtensionsString",                      CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXQueryVersion",                               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXSwapBuffers",                                CALL_FLAG_SWAPBUFFERS },
    { "wglGetCurrentContext",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetCurrentDC",                               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetDefaultProcAddress",                      CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetProcAddress",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglSwapBuffers",                                CALL_FLAG_SWAPBUFFERS },
    { "wglSwapLayerBuffers",                           CALL_FLAG_SWAPBUFFERS },
    { "wglSwapMultipleBuffers",                        CALL_FLAG_SWAPBUFFERS },
    // NOTE: New entries must be sorted alphabetically
};


/**
 * Lookup call flags by name.
 */
CallFlags
Parser::lookupCallFlags(const char *name) {
    return entryLookup(name, callFlagTable, defaultCallFlags);
}

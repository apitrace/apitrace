/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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


/*
 * Label functions based on their name.
 */


#include "trace_parser.hpp"

#include <assert.h>

#include <regex>

#include "trace_lookup.hpp"


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
    { "D3DPERF_BeginEvent",                            /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_PUSH },
    { "D3DPERF_EndEvent",                              /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_POP },
    { "D3DPERF_SetMarker",                             /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER },
    { "ID3DUserDefinedAnnotation::BeginEvent",         /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_PUSH },
    { "ID3DUserDefinedAnnotation::EndEvent",           /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_POP },
    { "ID3DUserDefinedAnnotation::SetMarker",          /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER },
    { "IDirect3D8::CheckDeviceFormat",                 CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D8::EnumAdapterModes",                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D8::GetAdapterModeCount",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D8::GetDeviceCaps",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::CheckDeviceFormat",                 CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::EnumAdapterModes",                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::GetAdapterModeCount",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9::GetDeviceCaps",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::CheckDeviceFormat",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::EnumAdapterModes",                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::GetAdapterModeCount",             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3D9Ex::GetDeviceCaps",                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3DDevice2::DrawIndexedPrimitive",        CALL_FLAG_RENDER },
    { "IDirect3DDevice2::DrawPrimitive",               CALL_FLAG_RENDER },
    { "IDirect3DDevice3::DrawIndexedPrimitive",        CALL_FLAG_RENDER },
    { "IDirect3DDevice3::DrawIndexedPrimitiveStrided", CALL_FLAG_RENDER },
    { "IDirect3DDevice3::DrawIndexedPrimitiveVB",      CALL_FLAG_RENDER },
    { "IDirect3DDevice3::DrawPrimitive",               CALL_FLAG_RENDER },
    { "IDirect3DDevice3::DrawPrimitiveStrided",        CALL_FLAG_RENDER },
    { "IDirect3DDevice3::DrawPrimitiveVB",             CALL_FLAG_RENDER },
    { "IDirect3DDevice7::Clear",                       CALL_FLAG_RENDER },
    { "IDirect3DDevice7::DrawIndexedPrimitive",        CALL_FLAG_RENDER },
    { "IDirect3DDevice7::DrawIndexedPrimitiveStrided", CALL_FLAG_RENDER },
    { "IDirect3DDevice7::DrawIndexedPrimitiveVB",      CALL_FLAG_RENDER },
    { "IDirect3DDevice7::DrawPrimitive",               CALL_FLAG_RENDER },
    { "IDirect3DDevice7::DrawPrimitiveStrided",        CALL_FLAG_RENDER },
    { "IDirect3DDevice7::DrawPrimitiveVB",             CALL_FLAG_RENDER },
    { "IDirect3DDevice8::Clear",                       CALL_FLAG_RENDER },
    { "IDirect3DDevice8::DrawIndexedPrimitive",        CALL_FLAG_RENDER },
    { "IDirect3DDevice8::DrawIndexedPrimitiveUP",      CALL_FLAG_RENDER },
    { "IDirect3DDevice8::DrawPrimitive",               CALL_FLAG_RENDER },
    { "IDirect3DDevice8::DrawPrimitiveUP",             CALL_FLAG_RENDER },
    { "IDirect3DDevice8::DrawRectPatch",               CALL_FLAG_RENDER },
    { "IDirect3DDevice8::DrawTriPatch",                CALL_FLAG_RENDER },
    { "IDirect3DDevice8::GetDeviceCaps",               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "IDirect3DDevice8::Present",                     CALL_FLAG_SWAPBUFFERS },
    { "IDirect3DDevice8::SetRenderTarget",             CALL_FLAG_SWAP_RENDERTARGET },
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
    { "IDirect3DViewport2::Clear",                     CALL_FLAG_RENDER },
    { "IDirect3DViewport3::Clear",                     CALL_FLAG_RENDER },
    { "IDirect3DViewport3::Clear2",                    CALL_FLAG_RENDER },
    { "IDirect3DViewport::Clear",                      CALL_FLAG_RENDER },
    { "eglGetConfigAttrib",                            CALL_FLAG_VERBOSE },
    { "eglGetProcAddress",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "eglQueryString",                                CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "eglSwapBuffers",                                CALL_FLAG_SWAPBUFFERS },
    { "eglSwapBuffersWithDamageEXT",                   CALL_FLAG_SWAPBUFFERS },
    { "eglSwapBuffersWithDamageKHR",                   CALL_FLAG_SWAPBUFFERS },
    { "glAreProgramsResidentNV",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glAreTexturesResident",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glAreTexturesResidentEXT",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glBufferRegionEnabled",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glDebugMessageControl",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glDebugMessageControlARB",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glDebugMessageEnableAMD",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glDebugMessageInsert",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_MARKER },
    { "glDebugMessageInsertAMD",                       CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_MARKER },
    { "glDebugMessageInsertARB",                       CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_MARKER },
    { "glDebugMessageInsertKHR",                       CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_MARKER },
    { "glFrameTerminatorGREMEDY",                      CALL_FLAG_END_FRAME },
    { "glGetActiveAtomicCounterBufferiv",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveAttrib",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveAttribARB",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveSubroutineName",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveSubroutineUniformName",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveSubroutineUniformiv",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveUniform",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveUniformARB",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveUniformBlockName",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveUniformBlockiv",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveUniformName",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveUniformsiv",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetActiveVaryingNV",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetArrayObjectfvATI",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetArrayObjectivATI",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetAttachedObjectsARB",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetAttachedShaders",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBooleanIndexedvEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBooleani_v",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBooleanv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferParameteri64v",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferParameteriv",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferParameterivARB",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferParameterui64vNV",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferPointerv",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferPointervARB",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferSubData",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetBufferSubDataARB",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetClipPlane",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTable",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableEXT",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableParameterfv",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableParameterfvEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableParameterfvSGI",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableParameteriv",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableParameterivEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableParameterivSGI",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetColorTableSGI",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetCombinerInputParameterfvNV",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetCombinerInputParameterivNV",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetCombinerOutputParameterfvNV",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetCombinerOutputParameterivNV",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetCombinerStageParameterfvNV",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetConvolutionFilterEXT",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetConvolutionParameterfv",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetConvolutionParameterfvEXT",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetConvolutionParameteriv",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetConvolutionParameterivEXT",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetDetailTexFuncSGIS",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetDoubleIndexedvEXT",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetDoublei_v",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetDoublev",                                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetError",                                    CALL_FLAG_NO_SIDE_EFFECTS }, // verbose will be set later for GL_NO_ERROR 
    { "glGetFenceivNV",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFinalCombinerInputParameterfvNV",          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFinalCombinerInputParameterivNV",          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFogFuncSGIS",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFragDataIndex",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFragmentLightfvSGIX",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFragmentLightivSGIX",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFragmentMaterialfvSGIX",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFragmentMaterialivSGIX",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFramebufferAttachmentParameteriv",         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFramebufferAttachmentParameterivEXT",      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFramebufferParameteriv",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetFramebufferParameterivEXT",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetGraphicsResetStatusARB",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetHandleARB",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetHistogramEXT",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetHistogramParameterfv",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetHistogramParameterfvEXT",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetHistogramParameteriv",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetHistogramParameterivEXT",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetImageTransformParameterfvHP",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetImageTransformParameterivHP",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInfoLogARB",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInstrumentsSGIX",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInternalformati64v",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInternalformativ",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInvariantBooleanvEXT",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInvariantFloatvEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetInvariantIntegervEXT",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetLightfv",                                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetLightiv",                                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetListParameterfvSGIX",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetListParameterivSGIX",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetLocalConstantBooleanvEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetLocalConstantFloatvEXT",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetLocalConstantIntegervEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapAttribParameterfvNV",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapAttribParameterivNV",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapControlPointsNV",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapParameterfvNV",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapParameterivNV",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapdv",                                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapfv",                                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMapiv",                                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMaterialfv",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMaterialiv",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMinmaxEXT",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMinmaxParameterfv",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMinmaxParameterfvEXT",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMinmaxParameteriv",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMinmaxParameterivEXT",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexEnvfvEXT",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexEnvivEXT",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexGendvEXT",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexGenfvEXT",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexGenivEXT",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexLevelParameterfvEXT",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexLevelParameterivEXT",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexParameterIivEXT",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexParameterIuivEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexParameterfvEXT",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultiTexParameterivEXT",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultisamplefv",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetMultisamplefvNV",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedBufferParameterivEXT",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedBufferParameterui64vNV",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedBufferPointervEXT",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedBufferSubDataEXT",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedFramebufferAttachmentParameterivEXT", CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedFramebufferParameterivEXT",           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedProgramLocalParameterIivEXT",         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedProgramLocalParameterIuivEXT",        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedProgramLocalParameterdvEXT",          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedProgramLocalParameterfvEXT",          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedProgramStringEXT",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedProgramivEXT",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedRenderbufferParameterivEXT",          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedStringARB",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetNamedStringivARB",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectBufferfvATI",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectBufferivATI",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectLabel",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectParameterfvARB",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectParameterivAPPLE",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectParameterivARB",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetObjectPtrLabel",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetOcclusionQueryivNV",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetOcclusionQueryuivNV",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPerfMonitorCounterDataAMD",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPerfMonitorCounterInfoAMD",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPerfMonitorCounterStringAMD",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPerfMonitorCountersAMD",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPerfMonitorGroupStringAMD",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPerfMonitorGroupsAMD",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPixelTexGenParameterfvSGIS",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPixelTexGenParameterivSGIS",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPointerIndexedvEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPointerv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetPointervEXT",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramBinary",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramEnvParameterIivNV",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramEnvParameterIuivNV",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramEnvParameterdvARB",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramEnvParameterfvARB",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramInfoLog",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramInterfaceiv",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramLocalParameterIivNV",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramLocalParameterIuivNV",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramLocalParameterdvARB",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramLocalParameterfvARB",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramNamedParameterdvNV",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramNamedParameterfvNV",                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramParameterdvNV",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramParameterfvNV",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramPipelineInfoLog",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramPipelineiv",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramResourceIndex",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramResourceLocation",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramResourceLocationIndex",             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramResourceName",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramResourceiv",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramStageiv",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramStringARB",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramStringNV",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramSubroutineParameteruivNV",          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramiv",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramivARB",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetProgramivNV",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetQueryIndexediv",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetQueryiv",                                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetQueryivARB",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetRenderbufferParameteriv",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetRenderbufferParameterivEXT",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetSamplerParameterIiv",                      CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetSamplerParameterIuiv",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetSamplerParameterfv",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetSamplerParameteriv",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetSeparableFilterEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetShaderInfoLog",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetShaderPrecisionFormat",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetShaderSource",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetShaderSourceARB",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetShaderiv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetSharpenTexFuncSGIS",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetString",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glGetStringi",                                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glGetSynciv",                                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexBumpParameterfvATI",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexBumpParameterivATI",                    CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexEnvfv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexEnviv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexFilterFuncSGIS",                        CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexGendv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexGenfv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTexGeniv",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTrackMatrixivNV",                          CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTransformFeedbackVarying",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTransformFeedbackVaryingEXT",              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetTransformFeedbackVaryingNV",               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformIndices",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformSubroutineuiv",                     CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformdv",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformfv",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformfvARB",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformi64vNV",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformiv",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformivARB",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformui64vNV",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformuiv",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetUniformuivEXT",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVariantArrayObjectfvATI",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVariantArrayObjectivATI",                  CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVariantBooleanvEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVariantFloatvEXT",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVariantIntegervEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVariantPointervEXT",                       CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVertexArrayIntegeri_vEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVertexArrayIntegervEXT",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVertexArrayPointeri_vEXT",                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVertexArrayPointervEXT",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoCaptureStreamdvNV",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoCaptureStreamfvNV",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoCaptureStreamivNV",                   CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoCaptureivNV",                         CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoi64vNV",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoivNV",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideoui64vNV",                             CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetVideouivNV",                               CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnMapdvARB",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnMapfvARB",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnMapivARB",                                CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnUniformdvARB",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnUniformfvARB",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnUniformivARB",                            CALL_FLAG_NO_SIDE_EFFECTS },
    { "glGetnUniformuivARB",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glInsertEventMarkerEXT",                        CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_MARKER },
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
    { "glObjectLabel",                                 CALL_FLAG_NO_SIDE_EFFECTS },
    { "glObjectLabelKHR",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glObjectPtrLabel",                              CALL_FLAG_NO_SIDE_EFFECTS },
    { "glObjectPtrLabelKHR",                           CALL_FLAG_NO_SIDE_EFFECTS },
    { "glPopDebugGroup",                               /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_POP},
    { "glPopDebugGroupKHR",                            /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_POP},
    { "glPopGroupMarkerEXT",                           /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_POP},
    { "glPushDebugGroup",                              /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_PUSH },
    { "glPushDebugGroupKHR",                           /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_PUSH },
    { "glPushGroupMarkerEXT",                          /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER | CALL_FLAG_MARKER_PUSH },
    { "glStringMarkerGREMEDY",                         /* CALL_FLAG_NO_SIDE_EFFECTS | */ CALL_FLAG_MARKER },
    { "glXGetClientString",                            CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetConfig",                                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentContext",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentDisplay",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentDisplayEXT",                       CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentDrawable",                         CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentReadDrawable",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetCurrentReadDrawableSGI",                  CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetFBConfigAttrib",                          CALL_FLAG_VERBOSE },
    { "glXGetFBConfigAttribSGIX",                      CALL_FLAG_VERBOSE },
    { "glXGetProcAddress",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXGetProcAddressARB",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXIsDirect",                                   CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXQueryExtension",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXQueryExtensionsString",                      CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXQueryVersion",                               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "glXSwapBuffers",                                CALL_FLAG_SWAPBUFFERS },
    { "wglDescribePixelFormat",                        CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetCurrentContext",                          CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetCurrentDC",                               CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetDefaultProcAddress",                      CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetExtensionsStringARB",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetExtensionsStringEXT",                     CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetPixelFormat",                             CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE },
    { "wglGetPixelFormatAttribivARB",                  CALL_FLAG_VERBOSE },
    { "wglGetPixelFormatAttribivEXT",                  CALL_FLAG_VERBOSE },
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
Parser::lookupCallFlags(const char *name)
{
    using namespace std;

    if (name[0] == 'g') {
        static const regex draw(
            "^gl([A-Z][a-z]+)*Draw(Range|Mesh)?(Arrays|Elements)([A-Z][a-zA-Z]*)?$"
        );

        static const regex miscDraw(
            "^gl("
                "CallLists?|"
                "Clear|"
                "End|"
                "DrawPixels|"
                "DrawTransformFeedback([A-Z][a-zA-Z]*)?|"
                "BlitFramebuffer|"
                "Rect[dfis]v?|"
                "EvalMesh[0-9]+"
            ")[0-9A-Z]*$"
        );
        if (regex_match(name, draw) ||
            regex_match(name, miscDraw)) {
            return CALL_FLAG_RENDER;
        }

        static const regex fbo( "^glBindFramebuffer[0-9A-Z]*");
        if (regex_match(name, fbo)) {
            return CALL_FLAG_SWAP_RENDERTARGET;
        }

        static const regex get(
            "^gl("
                "GetFloat|"
                "GetInteger|"
                "GetVertexAttrib|"
                "GetTex(ture)?(Level)?Parameter"
            ")\\w+$"
        );
        if (regex_match(name, get)) {
            return CALL_FLAG_NO_SIDE_EFFECTS;
        }
    }

    if (name[0] == 'I') {
        static const regex present("^IDXGI(Decode)?SwapChain\\w*::Present\\w*$");
        static const regex draw   ("^ID3D1(0Device|1DeviceContext)\\d*::(Draw\\w*|ExecuteCommandList)$");
        static const regex srt    ("^ID3D1(0Device|1DeviceContext)\\d*::OMSetRenderTargets\\w*$");
        static const regex cmql   ("^ID3D1[01]Device\\d*::CheckMultisampleQualityLevels$");

        if (regex_match(name, draw))    return CALL_FLAG_RENDER;
        if (regex_match(name, srt))     return CALL_FLAG_SWAP_RENDERTARGET;
        if (regex_match(name, present)) return CALL_FLAG_END_FRAME /* | CALL_FLAG_SWAPBUFFERS */;
        if (regex_match(name, cmql))    return CALL_FLAG_NO_SIDE_EFFECTS | CALL_FLAG_VERBOSE;
    }

    return entryLookup(name, callFlagTable, defaultCallFlags);
}

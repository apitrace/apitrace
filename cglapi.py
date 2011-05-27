##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/

"""CGL API description."""


from stdapi import *
from glapi import *
from glapi import glapi

IOSurfaceRef = Opaque("IOSurfaceRef")
CGLContextObj = Opaque("CGLContextObj")
CGLPixelFormatObj = Opaque("CGLPixelFormatObj")
CGLRendererInfoObj = Opaque("CGLRendererInfoObj")
CGLPBufferObj = Opaque("CGLPBufferObj")
CGLShareGroup = Opaque("CGLShareGroup")
CGLShareGroupObj = Opaque("CGLShareGroupObj")
CGSConnectionID = Alias("CGSConnectionID", Int)
CGSWindowID = Alias("CGSWindowID", Int)
CGSSurfaceID = Alias("CGSSurfaceID", Int)

CGLPixelFormatAttribute = Enum("CGLPixelFormatAttribute", [
    "kCGLPFAAllRenderers",
    "kCGLPFADoubleBuffer",
    "kCGLPFAStereo",
    "kCGLPFAAuxBuffers",
    "kCGLPFAColorSize",
    "kCGLPFAAlphaSize",
    "kCGLPFADepthSize",
    "kCGLPFAStencilSize",
    "kCGLPFAAccumSize",
    "kCGLPFAMinimumPolicy",
    "kCGLPFAMaximumPolicy",
    "kCGLPFAOffScreen",
    "kCGLPFAFullScreen",
    "kCGLPFASampleBuffers",
    "kCGLPFASamples",
    "kCGLPFAAuxDepthStencil",
    "kCGLPFAColorFloat",
    "kCGLPFAMultisample",
    "kCGLPFASupersample",
    "kCGLPFASampleAlpha",
    "kCGLPFARendererID",
    "kCGLPFASingleRenderer",
    "kCGLPFANoRecovery",
    "kCGLPFAAccelerated",
    "kCGLPFAClosestPolicy",
    "kCGLPFABackingStore",
    "kCGLPFAWindow",
    "kCGLPFACompliant",
    "kCGLPFADisplayMask",
    "kCGLPFAPBuffer",
    "kCGLPFARemotePBuffer",
    "kCGLPFAAllowOfflineRenderers",
    "kCGLPFAAcceleratedCompute",
    "kCGLPFAVirtualScreenCount",
    "kCGLPFARobust",
    "kCGLPFAMPSafe",
    "kCGLPFAMultiScreen",
])

CGLRendererProperty = Enum("CGLRendererProperty", [
    "kCGLRPOffScreen",
    "kCGLRPFullScreen",
    "kCGLRPRendererID",
    "kCGLRPAccelerated",
    "kCGLRPRobust",
    "kCGLRPBackingStore",
    "kCGLRPMPSafe",
    "kCGLRPWindow",
    "kCGLRPMultiScreen",
    "kCGLRPCompliant",
    "kCGLRPDisplayMask",
    "kCGLRPBufferModes",
    "kCGLRPColorModes",
    "kCGLRPAccumModes",
    "kCGLRPDepthModes",
    "kCGLRPStencilModes",
    "kCGLRPMaxAuxBuffers",
    "kCGLRPMaxSampleBuffers",
    "kCGLRPMaxSamples",
    "kCGLRPSampleModes",
    "kCGLRPSampleAlpha",
    "kCGLRPVideoMemory",
    "kCGLRPTextureMemory",
    "kCGLRPGPUVertProcCapable",
    "kCGLRPGPUFragProcCapable",
    "kCGLRPRendererCount",
    "kCGLRPOnline",
    "kCGLRPAcceleratedCompute",
])

CGLContextEnable = Enum("CGLContextEnable", [
    "kCGLCESwapRectangle",
    "kCGLCESwapLimit",
    "kCGLCERasterization",
    "kCGLCEStateValidation",
    "kCGLCESurfaceBackingSize",
    "kCGLCEDisplayListOptimization",
    "kCGLCEMPEngine",
])

CGLContextParameter = Enum("CGLContextParameter", [
    "kCGLCPSwapRectangle",
    "kCGLCPSwapInterval",
    "kCGLCPDispatchTableSize",
    "kCGLCPClientStorage",
    "kCGLCPSurfaceTexture",
    "kCGLCPSurfaceOrder",
    "kCGLCPSurfaceOpacity",
    "kCGLCPSurfaceBackingSize",
    "kCGLCPSurfaceSurfaceVolatile",
    "kCGLCPReclaimResources",
    "kCGLCPCurrentRendererID",
    "kCGLCPGPUVertexProcessing",
    "kCGLCPGPUFragmentProcessing",
    "kCGLCPHasDrawable",
    "kCGLCPMPSwapsInFlight",
])

CGLGlobalOption = Enum("CGLGlobalOption", [
    "kCGLGOFormatCacheSize",
    "kCGLGOClearFormatCache",
    "kCGLGORetainRenderers",
    "kCGLGOResetLibrary",
    "kCGLGOUseErrorHandler",
    "kCGLGOUseBuildCache",
])

CGLError = Enum("CGLError", [
    "kCGLNoError",
    "kCGLBadAttribute",
    "kCGLBadProperty",
    "kCGLBadPixelFormat",
    "kCGLBadRendererInfo",
    "kCGLBadContext",
    "kCGLBadDrawable",
    "kCGLBadDisplay",
    "kCGLBadState",
    "kCGLBadValue",
    "kCGLBadMatch",
    "kCGLBadEnumeration",
    "kCGLBadOffScreen",
    "kCGLBadFullScreen",
    "kCGLBadWindow",
    "kCGLBadAddress",
    "kCGLBadCodeModule",
    "kCGLBadAlloc",
    "kCGLBadConnection",
])

CGLContextObj = Opaque("CGLContextObj")

cglapi = API("CGL")

cglapi.add_functions([
    # CGLCurrent.h, libGL.dylib
    Function(CGLError, "CGLSetCurrentContext", [(CGLContextObj, "ctx")]),
    Function(CGLContextObj, "CGLGetCurrentContext", []),

    # OpenGL.h, OpenGL framework
    Function(CGLError, "CGLChoosePixelFormat", [(Const(OpaquePointer(CGLPixelFormatAttribute)), "attribs"), (OpaquePointer(CGLPixelFormatObj), "pix"), (OpaquePointer(GLint), "npix")]),
    Function(CGLError, "CGLDestroyPixelFormat", [(CGLPixelFormatObj, "pix")]),
    Function(CGLError, "CGLDescribePixelFormat", [(CGLPixelFormatObj, "pix"), (GLint, "pix_num"), (CGLPixelFormatAttribute, "attrib"), (OpaquePointer(GLint), "value")]),
    Function(Void, "CGLReleasePixelFormat", [(CGLPixelFormatObj, "pix")]),
    Function(CGLPixelFormatObj, "CGLRetainPixelFormat", [(CGLPixelFormatObj, "pix")]),
    Function(GLuint, "CGLGetPixelFormatRetainCount", [(CGLPixelFormatObj, "pix")]),
    Function(CGLError, "CGLQueryRendererInfo", [(GLuint, "display_mask"), (OpaquePointer(CGLRendererInfoObj), "rend"), (OpaquePointer(GLint), "nrend")]),
    Function(CGLError, "CGLDestroyRendererInfo", [(CGLRendererInfoObj, "rend")]),
    Function(CGLError, "CGLDescribeRenderer", [(CGLRendererInfoObj, "rend"), (GLint, "rend_num"), (CGLRendererProperty, "prop"), (OpaquePointer(GLint), "value")]),
    Function(CGLError, "CGLCreateContext", [(CGLPixelFormatObj, "pix"), (CGLContextObj, "share"), (OpaquePointer(CGLContextObj), "ctx")]),
    Function(CGLError, "CGLDestroyContext", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLCopyContext", [(CGLContextObj, "src"), (CGLContextObj, "dst"), (GLbitfield, "mask")]),
    Function(CGLContextObj, "CGLRetainContext", [(CGLContextObj, "ctx")]),
    Function(Void, "CGLReleaseContext", [(CGLContextObj, "ctx")]),
    Function(GLuint, "CGLGetContextRetainCount", [(CGLContextObj, "ctx")]),
    Function(CGLPixelFormatObj, "CGLGetPixelFormat", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLCreatePBuffer", [(GLsizei, "width"), (GLsizei, "height"), (GLenum, "target"), (GLenum, "internalFormat"), (GLint, "max_level"), (OpaquePointer(CGLPBufferObj), "pbuffer")]),
    Function(CGLError, "CGLDestroyPBuffer", [(CGLPBufferObj, "pbuffer")]),
    Function(CGLError, "CGLDescribePBuffer", [(CGLPBufferObj, "obj"), (OpaquePointer(GLsizei), "width"), (OpaquePointer(GLsizei), "height"), (OpaquePointer(GLenum), "target"), (OpaquePointer(GLenum), "internalFormat"), (OpaquePointer(GLint), "mipmap")]),
    Function(CGLError, "CGLTexImagePBuffer", [(CGLContextObj, "ctx"), (CGLPBufferObj, "pbuffer"), (GLenum, "source")]),
    Function(CGLPBufferObj, "CGLRetainPBuffer", [(CGLPBufferObj, "pbuffer")]),
    Function(Void, "CGLReleasePBuffer", [(CGLPBufferObj, "pbuffer")]),
    Function(GLuint, "CGLGetPBufferRetainCount", [(CGLPBufferObj, "pbuffer")]),
    Function(CGLError, "CGLSetOffScreen", [(CGLContextObj, "ctx"), (GLsizei, "width"), (GLsizei, "height"), (GLint, "rowbytes"), (OpaquePointer(Void), "baseaddr")]),
    Function(CGLError, "CGLGetOffScreen", [(CGLContextObj, "ctx"), (OpaquePointer(GLsizei), "width"), (OpaquePointer(GLsizei), "height"), (OpaquePointer(GLint), "rowbytes"), (OpaquePointer(OpaquePointer(Void)), "baseaddr")]),
    Function(CGLError, "CGLSetFullScreen", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLSetFullScreenOnDisplay", [(CGLContextObj, "ctx"), (GLuint, "display_mask")]),
    Function(CGLError, "CGLSetPBuffer", [(CGLContextObj, "ctx"), (CGLPBufferObj, "pbuffer"), (GLenum, "face"), (GLint, "level"), (GLint, "screen")]),
    Function(CGLError, "CGLGetPBuffer", [(CGLContextObj, "ctx"), (OpaquePointer(CGLPBufferObj), "pbuffer"), (OpaquePointer(GLenum), "face"), (OpaquePointer(GLint), "level"), (OpaquePointer(GLint), "screen")]),
    Function(CGLError, "CGLClearDrawable", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLFlushDrawable", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLEnable", [(CGLContextObj, "ctx"), (CGLContextEnable, "pname")]),
    Function(CGLError, "CGLDisable", [(CGLContextObj, "ctx"), (CGLContextEnable, "pname")]),
    Function(CGLError, "CGLIsEnabled", [(CGLContextObj, "ctx"), (CGLContextEnable, "pname"), (OpaquePointer(GLint), "enable")]),
    Function(CGLError, "CGLSetParameter", [(CGLContextObj, "ctx"), (CGLContextParameter, "pname"), (Const(OpaquePointer(GLint)), "params")]),
    Function(CGLError, "CGLGetParameter", [(CGLContextObj, "ctx"), (CGLContextParameter, "pname"), (OpaquePointer(GLint), "params")]),
    Function(CGLError, "CGLSetVirtualScreen", [(CGLContextObj, "ctx"), (GLint, "screen")]),
    Function(CGLError, "CGLGetVirtualScreen", [(CGLContextObj, "ctx"), (OpaquePointer(GLint), "screen")]),
    Function(CGLError, "CGLSetGlobalOption", [(CGLGlobalOption, "pname"), (Const(OpaquePointer(GLint)), "params")]),
    Function(CGLError, "CGLGetGlobalOption", [(CGLGlobalOption, "pname"), (OpaquePointer(GLint), "params")]),
    Function(CGLError, "CGLSetOption", [(CGLGlobalOption, "pname"), (GLint, "param")]),
    Function(CGLError, "CGLGetOption", [(CGLGlobalOption, "pname"), (OpaquePointer(GLint), "param")]),
    Function(CGLError, "CGLLockContext", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLUnlockContext", [(CGLContextObj, "ctx")]),
    Function(Void, "CGLGetVersion", [(OpaquePointer(GLint), "majorvers"), (OpaquePointer(GLint), "minorvers")]),
    Function(Const(CString), "CGLErrorString", [(CGLError, "error")]),

    # CGLIOSurface.h, OpenGL framework
    Function(CGLError, "CGLTexImageIOSurface2D", [(CGLContextObj, "ctx"), (GLenum, "target"), (GLenum, "internal_format"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), (IOSurfaceRef, "ioSurface"), (GLuint, "plane")]),

    # CGLDevice.h, OpenGL framework
    Function(CGLShareGroupObj, "CGLGetShareGroup", [(CGLContextObj, "ctx")]),

    # Undocumented, OpenGL framework
    Function(CGLError, "CGLSetSurface", [(CGLContextObj, "ctx"), (CGSConnectionID, "cid"), (CGSWindowID, "wid"), (CGSSurfaceID, "sid")]),
    Function(CGLError, "CGLGetSurface", [(CGLContextObj, "ctx"), (OpaquePointer(CGSConnectionID), "cid"), (OpaquePointer(CGSWindowID), "wid"), (OpaquePointer(CGSSurfaceID), "sid")]),
    Function(CGLError, "CGLUpdateContext", [(CGLContextObj, "ctx")]),
    # XXX: Confirm CGLOpenCLMuxLockDown
    Function(CGLError, "CGLOpenCLMuxLockDown", []),
    # FIXME: CGLAreContextsShared
    # FIXME: CGLBackDispatch
    # FIXME: CGLFrontDispatch
    # FIXME: CGLGetNextContext
    # FIXME: CGLRestoreDispatch
    # FIXME: CGLRestoreDispatchFunction
    # FIXME: CGLSelectDispatch
    # FIXME: CGLSelectDispatchBounded
    # FIXME: CGLSelectDispatchFunction
    # FIXME: CGLSetPBufferVolatileState
])


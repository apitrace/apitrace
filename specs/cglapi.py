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

"""CGL API description.

http://developer.apple.com/library/mac/#documentation/GraphicsImaging/Reference/CGL_OpenGL/Reference/reference.html
"""


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
CGSConnectionID = Opaque("CGSConnectionID")
CGSWindowID = Alias("CGSWindowID", Int)
CGSSurfaceID = Alias("CGSSurfaceID", Int)

CGLOpenGLProfile = Enum("CGLOpenGLProfile", [
    'kCGLOGLPVersion_Legacy',
    'kCGLOGLPVersion_3_2_Core',
    #'kCGLOGLPVersion_GL3_Core', # Same as kCGLOGLPVersion_3_2_Core
    'kCGLOGLPVersion_GL4_Core',
])

CGLPixelFormatAttributes = [
    ("kCGLPFAAllRenderers", None),
    ("kCGLPFATripleBuffer", None),
    ("kCGLPFADoubleBuffer", None),
    ("kCGLPFAStereo", None),
    ("kCGLPFAColorSize", Int),
    ("kCGLPFAAlphaSize", Int),
    ("kCGLPFADepthSize", Int),
    ("kCGLPFAStencilSize", Int),
    ("kCGLPFAMinimumPolicy", None),
    ("kCGLPFAMaximumPolicy", None),
    ("kCGLPFASampleBuffers", Int),
    ("kCGLPFASamples", Int),
    ("kCGLPFAColorFloat", None),
    ("kCGLPFAMultisample", None),
    ("kCGLPFASupersample", None),
    ("kCGLPFASampleAlpha", None),
    ("kCGLPFARendererID", Int),
    ("kCGLPFANoRecovery", None),
    ("kCGLPFAAccelerated", None),
    ("kCGLPFAClosestPolicy", None),
    ("kCGLPFABackingStore", None),
    ("kCGLPFABackingVolatile", None),
    ("kCGLPFADisplayMask", Int),
    ("kCGLPFAAllowOfflineRenderers", None),
    ("kCGLPFAAcceleratedCompute", None),
    ("kCGLPFAOpenGLProfile", CGLOpenGLProfile),
    ("kCGLPFASupportsAutomaticGraphicsSwitching", None),
    ("kCGLPFAVirtualScreenCount", Int),
    ("kCGLPFAAuxBuffers", None),
    ("kCGLPFAAccumSize", Int),
    ("kCGLPFAAuxDepthStencil", None),
    ("kCGLPFAOffScreen", None),
    ("kCGLPFAWindow", None),
    ("kCGLPFACompliant", None),
    ("kCGLPFAPBuffer", None),
    ("kCGLPFARemotePBuffer", None),
    ("kCGLPFASingleRenderer", None),
    ("kCGLPFARobust", None),
    ("kCGLPFAMPSafe", None),
    ("kCGLPFAMultiScreen", None),
    ("kCGLPFAFullScreen", None),
]

CGLPixelFormatAttribute = Enum("CGLPixelFormatAttribute",
    [attrib for attrib, _ in CGLPixelFormatAttributes]
)

CGLPixelFormatAttribs = AttribArray(
    Const(CGLPixelFormatAttribute),
    CGLPixelFormatAttributes
)

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
    "kCGLRPVideoMemoryMegabytes",
    "kCGLRPTextureMemoryMegabytes",
    "kCGLRPMajorGLVersion",
])

CGLContextEnable = Enum("CGLContextEnable", [
    "kCGLCESwapRectangle",
    "kCGLCESwapLimit",
    "kCGLCERasterization",
    "kCGLCEStateValidation",
    "kCGLCESurfaceBackingSize",
    "kCGLCEDisplayListOptimization",
    "kCGLCEMPEngine",
    "kCGLCECrashOnRemovedFunctions",
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

cglapi = Module("CGL")

cglapi.addFunctions([
    # CGLCurrent.h, libGL.dylib
    Function(CGLError, "CGLSetCurrentContext", [(CGLContextObj, "ctx")]),
    Function(CGLContextObj, "CGLGetCurrentContext", []),

    # OpenGL.h, OpenGL framework
    Function(CGLError, "CGLChoosePixelFormat", [(CGLPixelFormatAttribs, "attribs"), Out(Pointer(CGLPixelFormatObj), "pix"), Out(Pointer(GLint), "npix")]),
    Function(CGLError, "CGLDestroyPixelFormat", [(CGLPixelFormatObj, "pix")]),
    Function(CGLError, "CGLDescribePixelFormat", [(CGLPixelFormatObj, "pix"), (GLint, "pix_num"), (CGLPixelFormatAttribute, "attrib"), Out(Pointer(GLint), "value")]),
    Function(Void, "CGLReleasePixelFormat", [(CGLPixelFormatObj, "pix")]),
    Function(CGLPixelFormatObj, "CGLRetainPixelFormat", [(CGLPixelFormatObj, "pix")]),
    Function(GLuint, "CGLGetPixelFormatRetainCount", [(CGLPixelFormatObj, "pix")]),
    Function(CGLError, "CGLQueryRendererInfo", [(GLuint, "display_mask"), Out(Pointer(CGLRendererInfoObj), "rend"), Out(Pointer(GLint), "nrend")]),
    Function(CGLError, "CGLDestroyRendererInfo", [(CGLRendererInfoObj, "rend")]),
    Function(CGLError, "CGLDescribeRenderer", [(CGLRendererInfoObj, "rend"), (GLint, "rend_num"), (CGLRendererProperty, "prop"), (OpaquePointer(GLint), "value")]),
    Function(CGLError, "CGLCreateContext", [(CGLPixelFormatObj, "pix"), (CGLContextObj, "share"), Out(Pointer(CGLContextObj), "ctx")]),
    Function(CGLError, "CGLDestroyContext", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLCopyContext", [(CGLContextObj, "src"), (CGLContextObj, "dst"), (GLbitfield, "mask")]),
    Function(CGLContextObj, "CGLRetainContext", [(CGLContextObj, "ctx")]),
    Function(Void, "CGLReleaseContext", [(CGLContextObj, "ctx")]),
    Function(GLuint, "CGLGetContextRetainCount", [(CGLContextObj, "ctx")]),
    Function(CGLPixelFormatObj, "CGLGetPixelFormat", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLCreatePBuffer", [(GLsizei, "width"), (GLsizei, "height"), (GLenum, "target"), (GLenum, "internalFormat"), (GLint, "max_level"), Out(Pointer(CGLPBufferObj), "pbuffer")]),
    Function(CGLError, "CGLDestroyPBuffer", [(CGLPBufferObj, "pbuffer")]),
    Function(CGLError, "CGLDescribePBuffer", [(CGLPBufferObj, "obj"), Out(Pointer(GLsizei), "width"), Out(Pointer(GLsizei), "height"), Out(Pointer(GLenum), "target"), Out(Pointer(GLenum), "internalFormat"), Out(Pointer(GLint), "mipmap")]),
    Function(CGLError, "CGLTexImagePBuffer", [(CGLContextObj, "ctx"), (CGLPBufferObj, "pbuffer"), (GLenum, "source")]),
    Function(CGLPBufferObj, "CGLRetainPBuffer", [(CGLPBufferObj, "pbuffer")]),
    Function(Void, "CGLReleasePBuffer", [(CGLPBufferObj, "pbuffer")]),
    Function(GLuint, "CGLGetPBufferRetainCount", [(CGLPBufferObj, "pbuffer")]),
    Function(CGLError, "CGLSetOffScreen", [(CGLContextObj, "ctx"), (GLsizei, "width"), (GLsizei, "height"), (GLint, "rowbytes"), (OpaquePointer(Void), "baseaddr")]),
    Function(CGLError, "CGLGetOffScreen", [(CGLContextObj, "ctx"), Out(Pointer(GLsizei), "width"), Out(Pointer(GLsizei), "height"), Out(Pointer(GLint), "rowbytes"), Out(Pointer(OpaquePointer(Void)), "baseaddr")]),
    Function(CGLError, "CGLSetFullScreen", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLSetFullScreenOnDisplay", [(CGLContextObj, "ctx"), (GLuint, "display_mask")]),
    Function(CGLError, "CGLSetPBuffer", [(CGLContextObj, "ctx"), (CGLPBufferObj, "pbuffer"), (GLenum, "face"), (GLint, "level"), (GLint, "screen")]),
    Function(CGLError, "CGLGetPBuffer", [(CGLContextObj, "ctx"), Out(Pointer(CGLPBufferObj), "pbuffer"), Out(Pointer(GLenum), "face"), Out(Pointer(GLint), "level"), Out(Pointer(GLint), "screen")]),
    Function(CGLError, "CGLClearDrawable", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLFlushDrawable", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLEnable", [(CGLContextObj, "ctx"), (CGLContextEnable, "pname")]),
    Function(CGLError, "CGLDisable", [(CGLContextObj, "ctx"), (CGLContextEnable, "pname")]),
    Function(CGLError, "CGLIsEnabled", [(CGLContextObj, "ctx"), (CGLContextEnable, "pname"), Out(Pointer(GLint), "enable")], sideeffects=False),
    Function(CGLError, "CGLSetParameter", [(CGLContextObj, "ctx"), (CGLContextParameter, "pname"), (Array(Const(GLint), 1), "params")]),
    Function(CGLError, "CGLGetParameter", [(CGLContextObj, "ctx"), (CGLContextParameter, "pname"), Out(Array(GLint, 1), "params")], sideeffects=False),
    Function(CGLError, "CGLSetVirtualScreen", [(CGLContextObj, "ctx"), (GLint, "screen")]),
    Function(CGLError, "CGLGetVirtualScreen", [(CGLContextObj, "ctx"), Out(Pointer(GLint), "screen")], sideeffects=False),
    Function(CGLError, "CGLSetGlobalOption", [(CGLGlobalOption, "pname"), (OpaquePointer(Const(GLint)), "params")]),
    Function(CGLError, "CGLGetGlobalOption", [(CGLGlobalOption, "pname"), Out(OpaquePointer(GLint), "params")]),
    Function(CGLError, "CGLSetOption", [(CGLGlobalOption, "pname"), (GLint, "param")]),
    Function(CGLError, "CGLGetOption", [(CGLGlobalOption, "pname"), Out(Pointer(GLint), "param")], sideeffects=False),
    Function(CGLError, "CGLLockContext", [(CGLContextObj, "ctx")]),
    Function(CGLError, "CGLUnlockContext", [(CGLContextObj, "ctx")]),
    Function(Void, "CGLGetVersion", [Out(Pointer(GLint), "majorvers"), Out(Pointer(GLint), "minorvers")], sideeffects=False),
    Function(ConstCString, "CGLErrorString", [(CGLError, "error")], sideeffects=False),

    # CGLIOSurface.h, OpenGL framework
    Function(CGLError, "CGLTexImageIOSurface2D", [(CGLContextObj, "ctx"), (GLenum, "target"), (GLenum, "internal_format"), (GLsizei, "width"), (GLsizei, "height"), (GLenum, "format"), (GLenum, "type"), (IOSurfaceRef, "ioSurface"), (GLuint, "plane")]),

    # CGLDevice.h, OpenGL framework
    Function(CGLShareGroupObj, "CGLGetShareGroup", [(CGLContextObj, "ctx")]),

    # Undocumented, OpenGL framework
    Function(CGLError, "CGLSetSurface", [(CGLContextObj, "ctx"), (CGSConnectionID, "cid"), (CGSWindowID, "wid"), (CGSSurfaceID, "sid")]),
    Function(CGLError, "CGLGetSurface", [(CGLContextObj, "ctx"), (Pointer(CGSConnectionID), "cid"), (Pointer(CGSWindowID), "wid"), (Pointer(CGSSurfaceID), "sid")]),
    Function(CGLError, "CGLUpdateContext", [(CGLContextObj, "ctx")]),
    # XXX: All the following prototypes are little more than guesses
    # TODO: A potentially simpler alternative would be to use the
    # DYLD_INTERPOSE mechanism and only intercept the calls that we
    # really care about
    Function(CGLError, "CGLOpenCLMuxLockDown", [], internal=True),
    Function(GLboolean, "CGLAreContextsShared", [(CGLContextObj, "ctx1"), (CGLContextObj, "ctx2")], internal=True),
    Function(CGLContextObj, "CGLGetNextContext", [(CGLContextObj, "ctx")], internal=True),
    Function(OpaquePointer(Void), "CGLFrontDispatch", [(CGLContextObj, "ctx")], internal=True),
    Function(OpaquePointer(Void), "CGLBackDispatch", [(CGLContextObj, "ctx")], internal=True),
    Function(Void, "CGLSelectDispatch", [(CGLContextObj, "ctx"), (OpaquePointer(Void), "dispatch")], internal=True),
    Function(Void, "CGLSelectDispatchBounded", [(CGLContextObj, "ctx"), (OpaquePointer(Void), "dispatch"), (GLint, "size")], internal=True),
    Function(Void, "CGLSelectDispatchFunction", [(CGLContextObj, "ctx"), (OpaquePointer(Void), "functionPtr"), (GLint, "functionId")], internal=True),
    Function(Void, "CGLRestoreDispatch", [(CGLContextObj, "ctx")], internal=True),
    Function(Void, "CGLRestoreDispatchFunction", [(CGLContextObj, "ctx"), (GLint, "functionId")], internal=True),
    Function(CGLError, "CGLSetPBufferVolatileState", [(CGLPBufferObj, "pbuffer"), (OpaquePointer(Void), "state")], internal=True),
])


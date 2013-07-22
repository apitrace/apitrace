##########################################################################
#
# Copyright 2011 LunarG, Inc.
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

"""EGL API description."""


from stdapi import *
from gltypes import *
from eglenum import *

EGLNativeDisplayType = Opaque("EGLNativeDisplayType")
EGLNativeWindowType = Opaque("EGLNativeWindowType")
EGLNativePixmapType = Opaque("EGLNativePixmapType")

EGLDisplay = Opaque("EGLDisplay")
EGLConfig = Opaque("EGLConfig")
EGLContext = Opaque("EGLContext")
EGLSurface = Opaque("EGLSurface")

EGLClientBuffer = Opaque("EGLClientBuffer")

EGLBoolean = Enum("EGLBoolean", [
    "EGL_FALSE",
    "EGL_TRUE",
])

EGLint = Alias("EGLint", Int32)

# an alias of EGLenum
EGLint_enum = Alias("EGLint", EGLenum)
EGLattrib = EGLint_enum

# EGL_KHR_image_base
EGLImageKHR = Opaque("EGLImageKHR")

# EGL_KHR_reusable_sync
EGLSyncKHR = Opaque("EGLSyncKHR")
EGLTimeKHR = Alias("EGLTimeKHR", UInt64)

# EGL_NV_sync
EGLSyncNV = Alias("EGLSyncNV", EGLSyncKHR)
EGLTimeNV = Alias("EGLTimeNV", EGLTimeKHR)

# EGL_HI_clientpixmap
EGLClientPixmapHI = Struct("struct EGLClientPixmapHI", [
  (OpaquePointer(Void), "pData"),
  (EGLint, "iWidth"),
  (EGLint, "iHeight"),
  (EGLint, "iStride"),
])

# EGL_NV_system_time
EGLuint64NV = Alias("EGLuint64NV", UInt64)

eglapi = Module("EGL")

EGLSurfaceFlags = Flags(Int, [
    'EGL_MULTISAMPLE_RESOLVE_BOX_BIT',
    'EGL_PBUFFER_BIT',
    'EGL_PIXMAP_BIT',
    'EGL_SWAP_BEHAVIOR_PRESERVED_BIT',
    'EGL_VG_ALPHA_FORMAT_PRE_BIT',
    'EGL_VG_COLORSPACE_LINEAR_BIT',
    'EGL_WINDOW_BIT'
])

EGLConformantFlags = Flags(Int, ['EGL_OPENGL_BIT','EGL_OPENGL_ES_BIT', 'EGL_OPENGL_ES2_BIT', 'EGL_OPENVG_BIT'])

EGLVGAlphaFormat = FakeEnum(Int, ['EGL_VG_ALPHA_FORMAT_NONPRE', 'EGL_VG_ALPHA_FORMAT_PRE'])
EGLVGColorspace = FakeEnum(Int, ['EGL_VG_COLORSPACE_sRGB', 'EGL_VG_COLORSPACE_LINEAR'])
EGLTextureFormat = FakeEnum(Int, ['EGL_NO_TEXTURE', 'EGL_TEXTURE_RGB', 'EGL_TEXTURE_RGBA'])
EGLTextureTarget = FakeEnum(Int, ['EGL_TEXTURE_2D', 'EGL_NO_TEXTURE'])

def EGLAttribArray(values):
    return AttribArray(Const(EGLint_enum), values, terminator = 'EGL_NONE')

EGLConfigAttribs = EGLAttribArray([
    ('EGL_ALPHA_MASK_SIZE', UInt),
    ('EGL_ALPHA_SIZE', UInt),
    ('EGL_BIND_TO_TEXTURE_RGB', EGLBoolean),
    ('EGL_BIND_TO_TEXTURE_RGBA', EGLBoolean),
    ('EGL_BLUE_SIZE', UInt),
    ('EGL_BUFFER_SIZE', UInt),
    ('EGL_COLOR_BUFFER_TYPE', FakeEnum(Int, ['EGL_RGB_BUFFER', 'EGL_LUMINANCE_BUFFER'])),
    ('EGL_CONFIG_CAVEAT', FakeEnum(Int, ['EGL_DONT_CARE', 'EGL_NONE', 'EGL_SLOW_CONFIG', 'EGL_NON_CONFORMANT_CONFIG'])),
    ('EGL_CONFIG_ID', Int),
    ('EGL_CONFORMANT', EGLConformantFlags),
    ('EGL_DEPTH_SIZE', Int),
    ('EGL_GREEN_SIZE', UInt),
    ('EGL_LEVEL', Int),
    ('EGL_LUMINANCE_SIZE', UInt),
    ('EGL_MATCH_NATIVE_PIXMAP', Int),
    ('EGL_NATIVE_RENDERABLE', FakeEnum(Int, ['EGL_DONT_CARE', 'EGL_TRUE', 'EGL_FALSE'])),
    ('EGL_MAX_SWAP_INTERVAL', Int),
    ('EGL_MIN_SWAP_INTERVAL', Int),
    ('EGL_RED_SIZE', UInt),
    ('EGL_SAMPLE_BUFFERS', Int),
    ('EGL_SAMPLES', Int),
    ('EGL_STENCIL_SIZE', UInt),
    ('EGL_RENDERABLE_TYPE', EGLConformantFlags),
    ('EGL_SURFACE_TYPE', EGLSurfaceFlags),
    ('EGL_TRANSPARENT_TYPE', FakeEnum(Int, ['EGL_NONE', 'EGL_TRANSPARENT_RGB'])),
    ('EGL_TRANSPARENT_RED_VALUE', Int),
    ('EGL_TRANSPARENT_GREEN_VALUE', Int),
    ('EGL_TRANSPARENT_BLUE_VALUE ', Int)
])

EGLWindowsSurfaceAttribs = EGLAttribArray([
    ('EGL_RENDER_BUFFER', FakeEnum(Int, ['EGL_SINGLE_BUFFER', 'EGL_BACK_BUFFER'])),
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace)])

EGLPixmapSurfaceAttribs = EGLAttribArray([
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace)
])

EGLPbufferAttribs = EGLAttribArray([
    ('EGL_HEIGHT', Int),
    ('EGL_LARGEST_PBUFFER', EGLBoolean),
    ('EGL_MIPMAP_TEXTURE', UInt),
    ('EGL_TEXTURE_FORMAT', EGLTextureFormat),
    ('EGL_TEXTURE_TARGET', EGLTextureTarget),
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace),
    ('EGL_WIDTH', Int)
])

EGLPbufferFromClientBufferAttribs = EGLAttribArray([
    ('EGL_MIPMAP_TEXTURE', EGLBoolean),
    ('EGL_TEXTURE_FORMAT', EGLTextureFormat),
    ('EGL_TEXTURE_TARGET', EGLTextureTarget)
])

EGLDrmImageMesaAttribs = EGLAttribArray([
    ('EGL_DRM_BUFFER_FORMAT_MESA', FakeEnum(Int, ['EGL_DRM_BUFFER_FORMAT_ARGB32_MESA'])),
    ('EGL_DRM_BUFFER_USE_MESA', Flags(Int, ['EGL_DRM_BUFFER_USE_SCANOUT_MESA', 'EGL_DRM_BUFFER_USE_SHARE_MESA']))
])

EGLLockSurfaceKHRAttribs = EGLAttribArray([
    ('EGL_MAP_PRESERVE_PIXELS_KHR', EGLBoolean),
    ('EGL_LOCK_USAGE_HINT_KHR', Flags(Int, ['EGL_READ_SURFACE_BIT_KHR', 'EGL_WRITE_SURFACE_BIT_KHR']))
])

EGLFenceSyncNVAttribs = EGLAttribArray([
    ('EGL_SYNC_STATUS_NV', Flags(Int, ['EGL_SIGNALED_NV', 'EGL_UNSIGNALED_NV']))
])

EGLProc = Opaque("__eglMustCastToProperFunctionPointerType")

def GlFunction(*args, **kwargs):
    kwargs.setdefault('call', 'GL_APIENTRY')
    return Function(*args, **kwargs)

eglapi.addFunctions([
    # EGL 1.4
    Function(EGLint_enum, "eglGetError", [], sideeffects=False),

    Function(EGLDisplay, "eglGetDisplay", [(EGLNativeDisplayType, "display_id")]),
    Function(EGLBoolean, "eglInitialize", [(EGLDisplay, "dpy"), Out(Pointer(EGLint), "major"), Out(Pointer(EGLint), "minor")]),
    Function(EGLBoolean, "eglTerminate", [(EGLDisplay, "dpy")]),

    Function(ConstCString, "eglQueryString", [(EGLDisplay, "dpy"), (EGLint_enum, "name")], sideeffects=False),

    Function(EGLBoolean, "eglGetConfigs", [(EGLDisplay, "dpy"), (Array(EGLConfig, "config_size"), "configs"), (EGLint, "config_size"), Out(Pointer(EGLint), "num_config")]),
    Function(EGLBoolean, "eglChooseConfig", [(EGLDisplay, "dpy"), (EGLConfigAttribs, "attrib_list"), (Array(EGLConfig, "config_size"), "configs"), (EGLint, "config_size"), Out(Pointer(EGLint), "num_config")]),
    Function(EGLBoolean, "eglGetConfigAttrib", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    Function(EGLSurface, "eglCreateWindowSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativeWindowType, "win"), (EGLWindowsSurfaceAttribs, "attrib_list")]),
    Function(EGLSurface, "eglCreatePbufferSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLPbufferAttribs, "attrib_list")]),
    Function(EGLSurface, "eglCreatePixmapSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativePixmapType, "pixmap"), (EGLPixmapSurfaceAttribs, "attrib_list")]),
    Function(EGLBoolean, "eglDestroySurface", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),
    Function(EGLBoolean, "eglQuerySurface", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    Function(EGLBoolean, "eglBindAPI", [(EGLenum, "api")]),
    Function(EGLenum, "eglQueryAPI", [], sideeffects=False),

    Function(EGLBoolean, "eglWaitClient", []),

    Function(EGLBoolean, "eglReleaseThread", []),

    Function(EGLSurface, "eglCreatePbufferFromClientBuffer", [(EGLDisplay, "dpy"), (EGLenum, "buftype"), (EGLClientBuffer, "buffer"), (EGLConfig, "config"), (EGLPbufferFromClientBufferAttribs, "attrib_list")]),

    Function(EGLBoolean, "eglSurfaceAttrib", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "attribute"), (EGLint, "value")]),
    Function(EGLBoolean, "eglBindTexImage", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "buffer")]),
    Function(EGLBoolean, "eglReleaseTexImage", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "buffer")]),

    Function(EGLBoolean, "eglSwapInterval", [(EGLDisplay, "dpy"), (EGLint, "interval")]),

    Function(EGLContext, "eglCreateContext", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLContext, "share_context"), (EGLAttribArray([('EGL_CONTEXT_CLIENT_VERSION', Int)]), "attrib_list")]),
    Function(EGLBoolean, "eglDestroyContext", [(EGLDisplay, "dpy"), (EGLContext, "ctx")]),
    Function(EGLBoolean, "eglMakeCurrent", [(EGLDisplay, "dpy"), (EGLSurface, "draw"), (EGLSurface, "read"), (EGLContext, "ctx")]),

    Function(EGLContext, "eglGetCurrentContext", [], sideeffects=False),
    Function(EGLSurface, "eglGetCurrentSurface", [(EGLattrib, "readdraw")], sideeffects=False),
    Function(EGLDisplay, "eglGetCurrentDisplay", [], sideeffects=False),

    Function(EGLBoolean, "eglQueryContext", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    Function(EGLBoolean, "eglWaitGL", []),
    Function(EGLBoolean, "eglWaitNative", [(EGLattrib, "engine")]),
    Function(EGLBoolean, "eglSwapBuffers", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),
    Function(EGLBoolean, "eglCopyBuffers", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLNativePixmapType, "target")]),

    Function(EGLProc, "eglGetProcAddress", [(ConstCString, "procname")]),

    # EGL_KHR_lock_surface
    # EGL_KHR_lock_surface2
    Function(EGLBoolean, "eglLockSurfaceKHR", [(EGLDisplay, "display"), (EGLSurface, "surface"), (EGLLockSurfaceKHRAttribs, "attrib_list")]),
    Function(EGLBoolean, "eglUnlockSurfaceKHR", [(EGLDisplay, "display"), (EGLSurface, "surface")]),

    # EGL_KHR_image_base
    Function(EGLImageKHR, "eglCreateImageKHR", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLenum, "target"), (EGLClientBuffer, "buffer"), (EGLAttribArray([('EGL_IMAGE_PRESERVED_KHR', EGLBoolean)]), "attrib_list")]),
    Function(EGLBoolean, "eglDestroyImageKHR", [(EGLDisplay, "dpy"), (EGLImageKHR, "image")]),

    # EGL_KHR_fence_sync
    # EGL_KHR_reusable_sync
    Function(EGLSyncKHR, "eglCreateSyncKHR", [(EGLDisplay, "dpy"), (EGLenum, "type"), (EGLAttribArray([]), "attrib_list")]),
    Function(EGLBoolean, "eglDestroySyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync")]),
    Function(EGLint, "eglClientWaitSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint, "flags"), (EGLTimeKHR, "timeout")]),
    Function(EGLBoolean, "eglSignalSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLenum, "mode")]),
    Function(EGLBoolean, "eglGetSyncAttribKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_NV_sync
    Function(EGLSyncNV, "eglCreateFenceSyncNV", [(EGLDisplay, "dpy"), (EGLenum, "condition"), (EGLFenceSyncNVAttribs, "attrib_list")]),
    Function(EGLBoolean, "eglDestroySyncNV", [(EGLSyncNV, "sync")]),
    Function(EGLBoolean, "eglFenceNV", [(EGLSyncNV, "sync")]),
    Function(EGLint, "eglClientWaitSyncNV", [(EGLSyncNV, "sync"), (EGLint, "flags"), (EGLTimeNV, "timeout")]),
    Function(EGLBoolean, "eglSignalSyncNV", [(EGLSyncNV, "sync"), (EGLenum, "mode")]),
    Function(EGLBoolean, "eglGetSyncAttribNV", [(EGLSyncNV, "sync"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_HI_clientpixmap
    Function(EGLSurface, "eglCreatePixmapSurfaceHI", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (Pointer(EGLClientPixmapHI), "pixmap")]),

    # EGL_MESA_drm_image
    Function(EGLImageKHR, "eglCreateDRMImageMESA", [(EGLDisplay, "dpy"), (EGLDrmImageMesaAttribs, "attrib_list")]),
    Function(EGLBoolean, "eglExportDRMImageMESA", [(EGLDisplay, "dpy"), (EGLImageKHR, "image"), Out(Pointer(EGLint), "name"), Out(Pointer(EGLint), "handle"), Out(Pointer(EGLint), "stride")]),

    # EGL_NV_post_sub_buffer
    Function(EGLBoolean, "eglPostSubBufferNV", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint, "x"), (EGLint, "y"), (EGLint, "width"), (EGLint, "height")]),

    # EGL_ANGLE_query_surface_pointer
    Function(EGLBoolean, "eglQuerySurfacePointerANGLE", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "attribute"), Out(Pointer(OpaquePointer(Void)), "value")], sideeffects=False),

    # EGL_NV_system_time
    Function(EGLuint64NV, "eglGetSystemTimeFrequencyNV", [], sideeffects=False),
    Function(EGLuint64NV, "eglGetSystemTimeNV", [], sideeffects=False),

    # GL_OES_EGL_image
    GlFunction(Void, "glEGLImageTargetTexture2DOES", [(GLenum, "target"), (EGLImageKHR, "image")]),
    GlFunction(Void, "glEGLImageTargetRenderbufferStorageOES", [(GLenum, "target"), (EGLImageKHR, "image")]),
])

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

# An EGLenum disguised as an int
EGLint_enum = Alias("EGLint", EGLenum)
EGLAttrib = Alias("EGLAttrib", IntPtr)

EGLSync = Opaque("EGLSync")
EGLImage = Opaque("EGLImage")
EGLTime = Alias("EGLTime", UInt64)

# EGL_KHR_image_base
EGLImageKHR = Alias("EGLImageKHR", EGLImage)

# EGL_KHR_reusable_sync
EGLSyncKHR = Alias("EGLSyncKHR", EGLSync)
EGLTimeKHR = Alias("EGLTimeKHR", EGLTime)

# EGL_NV_sync
EGLSyncNV = Alias("EGLSyncNV", EGLSync)
EGLTimeNV = Alias("EGLTimeNV", EGLTime)

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

EGLConformantFlags = Flags(Int, [
    'EGL_OPENGL_ES_BIT',  # 0x0001
    'EGL_OPENVG_BIT',     # 0x0002
    'EGL_OPENGL_ES2_BIT', # 0x0004
    'EGL_OPENGL_BIT',     # 0x0008
    'EGL_OPENGL_ES3_BIT', # 0x0040
])

EGLVGAlphaFormat = FakeEnum(Int, ['EGL_VG_ALPHA_FORMAT_NONPRE', 'EGL_VG_ALPHA_FORMAT_PRE'])
EGLVGColorspace = FakeEnum(Int, ['EGL_VG_COLORSPACE_sRGB', 'EGL_VG_COLORSPACE_LINEAR'])
EGLTextureFormat = FakeEnum(Int, ['EGL_NO_TEXTURE', 'EGL_TEXTURE_RGB', 'EGL_TEXTURE_RGBA'])
EGLTextureTarget = FakeEnum(Int, ['EGL_TEXTURE_2D', 'EGL_NO_TEXTURE'])

def EGLIntArray(values):
    return AttribArray(Const(EGLint_enum), values, terminator = 'EGL_NONE')

def EGLAttribArray(values):
    return AttribArray(Const(EGLAttrib), values, terminator = 'EGL_NONE')

EGLConfigAttribs = EGLIntArray([
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

EGLWindowsSurfaceAttribs = EGLIntArray([
    ('EGL_RENDER_BUFFER', FakeEnum(Int, ['EGL_SINGLE_BUFFER', 'EGL_BACK_BUFFER'])),
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace)])

EGLPixmapSurfaceAttribs = EGLIntArray([
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace)
])

EGLPbufferAttribs = EGLIntArray([
    ('EGL_HEIGHT', Int),
    ('EGL_LARGEST_PBUFFER', EGLBoolean),
    ('EGL_MIPMAP_TEXTURE', UInt),
    ('EGL_TEXTURE_FORMAT', EGLTextureFormat),
    ('EGL_TEXTURE_TARGET', EGLTextureTarget),
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace),
    ('EGL_WIDTH', Int)
])

EGLPbufferFromClientBufferAttribs = EGLIntArray([
    ('EGL_MIPMAP_TEXTURE', EGLBoolean),
    ('EGL_TEXTURE_FORMAT', EGLTextureFormat),
    ('EGL_TEXTURE_TARGET', EGLTextureTarget)
])

EGLContextAttribs = EGLIntArray([
    ('EGL_CONTEXT_OPENGL_ROBUST_ACCESS_EXT', EGLBoolean),
    ('EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_EXT', EGLenum),
    ('EGL_CONTEXT_MAJOR_VERSION', Int), # Alias for EGL_CONTEXT_CLIENT_VERSION
    ('EGL_CONTEXT_MINOR_VERSION', Int),
    ('EGL_CONTEXT_FLAGS_KHR', Flags(Int, [
        'EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR',
        'EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR',
        'EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR',
    ])),
    ('EGL_CONTEXT_OPENGL_PROFILE_MASK', Flags(Int, [
        'EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT',
        'EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT',
    ])),
    ('EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY', EGLenum),
])

EGLDrmImageMesaAttribs = EGLIntArray([
    ('EGL_DRM_BUFFER_FORMAT_MESA', FakeEnum(Int, ['EGL_DRM_BUFFER_FORMAT_ARGB32_MESA'])),
    ('EGL_DRM_BUFFER_USE_MESA', Flags(Int, ['EGL_DRM_BUFFER_USE_SCANOUT_MESA', 'EGL_DRM_BUFFER_USE_SHARE_MESA']))
])

EGLLockSurfaceKHRAttribs = EGLIntArray([
    ('EGL_MAP_PRESERVE_PIXELS_KHR', EGLBoolean),
    ('EGL_LOCK_USAGE_HINT_KHR', Flags(Int, ['EGL_READ_SURFACE_BIT_KHR', 'EGL_WRITE_SURFACE_BIT_KHR']))
])

EGLFenceSyncNVAttribs = EGLIntArray([
    ('EGL_SYNC_STATUS_NV', Flags(Int, ['EGL_SIGNALED_NV', 'EGL_UNSIGNALED_NV']))
])

EGLPlatformDisplayAttribs = EGLAttribArray([
    ('EGL_PLATFORM_X11_SCREEN_EXT', Int),
])

EGLPlatformDisplayAttribsEXT = EGLIntArray([
    ('EGL_PLATFORM_X11_SCREEN_EXT', Int),
])

EGLProc = Opaque("__eglMustCastToProperFunctionPointerType")

def GlFunction(*args, **kwargs):
    kwargs.setdefault('call', 'EGLAPIENTRY')
    return Function(*args, **kwargs)

eglapi.addFunctions([
    # EGL_VERSION_1_0
    GlFunction(EGLBoolean, "eglChooseConfig", [(EGLDisplay, "dpy"), (EGLConfigAttribs, "attrib_list"), Out(Array(EGLConfig, "*num_config"), "configs"), (EGLint, "config_size"), Out(Pointer(EGLint), "num_config")]),
    GlFunction(EGLBoolean, "eglCopyBuffers", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLNativePixmapType, "target")]),
    GlFunction(EGLContext, "eglCreateContext", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLContext, "share_context"), (EGLContextAttribs, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePbufferSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLPbufferAttribs, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePixmapSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativePixmapType, "pixmap"), (EGLPixmapSurfaceAttribs, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreateWindowSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativeWindowType, "win"), (EGLWindowsSurfaceAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroyContext", [(EGLDisplay, "dpy"), (EGLContext, "ctx")]),
    GlFunction(EGLBoolean, "eglDestroySurface", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),
    GlFunction(EGLBoolean, "eglGetConfigAttrib", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),
    GlFunction(EGLBoolean, "eglGetConfigs", [(EGLDisplay, "dpy"), Out(Array(EGLConfig, "*num_config"), "configs"), (EGLint, "config_size"), Out(Pointer(EGLint), "num_config")]),
    GlFunction(EGLDisplay, "eglGetCurrentDisplay", [], sideeffects=False),
    GlFunction(EGLSurface, "eglGetCurrentSurface", [(EGLint_enum, "readdraw")], sideeffects=False),
    GlFunction(EGLDisplay, "eglGetDisplay", [(EGLNativeDisplayType, "display_id")]),
    GlFunction(EGLint_enum, "eglGetError", [], sideeffects=False),
    GlFunction(EGLProc, "eglGetProcAddress", [(ConstCString, "procname")]),
    GlFunction(EGLBoolean, "eglInitialize", [(EGLDisplay, "dpy"), Out(Pointer(EGLint), "major"), Out(Pointer(EGLint), "minor")]),
    GlFunction(EGLBoolean, "eglMakeCurrent", [(EGLDisplay, "dpy"), (EGLSurface, "draw"), (EGLSurface, "read"), (EGLContext, "ctx")]),
    GlFunction(EGLBoolean, "eglQueryContext", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),
    GlFunction(ConstCString, "eglQueryString", [(EGLDisplay, "dpy"), (EGLint_enum, "name")], sideeffects=False),
    GlFunction(EGLBoolean, "eglQuerySurface", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),
    GlFunction(EGLBoolean, "eglSwapBuffers", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),
    GlFunction(EGLBoolean, "eglTerminate", [(EGLDisplay, "dpy")]),
    GlFunction(EGLBoolean, "eglWaitGL", []),
    GlFunction(EGLBoolean, "eglWaitNative", [(EGLint_enum, "engine")]),

    # EGL_VERSION_1_1
    GlFunction(EGLBoolean, "eglBindTexImage", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint_enum, "buffer")]),
    GlFunction(EGLBoolean, "eglReleaseTexImage", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint_enum, "buffer")]),
    GlFunction(EGLBoolean, "eglSurfaceAttrib", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint_enum, "attribute"), (EGLint, "value")]),
    GlFunction(EGLBoolean, "eglSwapInterval", [(EGLDisplay, "dpy"), (EGLint, "interval")]),

    # EGL_VERSION_1_2
    GlFunction(EGLBoolean, "eglBindAPI", [(EGLenum, "api")]),
    GlFunction(EGLenum, "eglQueryAPI", [], sideeffects=False),
    GlFunction(EGLSurface, "eglCreatePbufferFromClientBuffer", [(EGLDisplay, "dpy"), (EGLenum, "buftype"), (EGLClientBuffer, "buffer"), (EGLConfig, "config"), (EGLPbufferFromClientBufferAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglReleaseThread", []),
    GlFunction(EGLBoolean, "eglWaitClient", []),

    # EGL_VERSION_1_4
    GlFunction(EGLContext, "eglGetCurrentContext", [], sideeffects=False),

    # EGL_VERSION_1_5
    GlFunction(EGLSync, "eglCreateSync", [(EGLDisplay, "dpy"), (EGLenum, "type"), (EGLAttribArray([]), "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroySync", [(EGLDisplay, "dpy"), (EGLSync, "sync")]),
    GlFunction(EGLint, "eglClientWaitSync", [(EGLDisplay, "dpy"), (EGLSync, "sync"), (EGLint, "flags"), (EGLTime, "timeout")]),
    GlFunction(EGLBoolean, "eglGetSyncAttrib", [(EGLDisplay, "dpy"), (EGLSync, "sync"), (EGLint_enum, "attribute"), Out(Pointer(EGLAttrib), "value")], sideeffects=False),
    GlFunction(EGLImage, "eglCreateImage", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLenum, "target"), (EGLClientBuffer, "buffer"), (EGLAttribArray([('EGL_IMAGE_PRESERVED', EGLBoolean)]), "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroyImage", [(EGLDisplay, "dpy"), (EGLImage, "image")]),
    GlFunction(EGLDisplay, "eglGetPlatformDisplay", [(EGLenum, "platform"), (OpaquePointer(Void), "native_display"), (EGLPlatformDisplayAttribs, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformWindowSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_window"), (EGLAttribArray([]), "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformPixmapSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_pixmap"), (EGLAttribArray([]), "attrib_list")]),
    GlFunction(EGLBoolean, "eglWaitSync", [(EGLDisplay, "dpy"), (EGLSync, "sync"), (EGLint, "flags")]),

    # EGL_ANGLE_query_surface_pointer
    GlFunction(EGLBoolean, "eglQuerySurfacePointerANGLE", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint_enum, "attribute"), Out(Pointer(OpaquePointer(Void)), "value")], sideeffects=False),

    # EGL_EXT_platform_base
    GlFunction(EGLDisplay, "eglGetPlatformDisplayEXT", [(EGLenum, "platform"), (OpaquePointer(Void), "native_display"), (EGLPlatformDisplayAttribsEXT, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformWindowSurfaceEXT", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_window"), (EGLIntArray([]), "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformPixmapSurfaceEXT", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_pixmap"), (EGLIntArray([]), "attrib_list")]),

    # EGL_EXT_swap_buffers_with_damage
    GlFunction(EGLBoolean, "eglSwapBuffersWithDamageEXT", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (Array(EGLint, "n_rects*4"), "rects"), (EGLint, "n_rects")]),

    # EGL_HI_clientpixmap
    GlFunction(EGLSurface, "eglCreatePixmapSurfaceHI", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (Pointer(EGLClientPixmapHI), "pixmap")]),

    # EGL_KHR_fence_sync
    GlFunction(EGLSyncKHR, "eglCreateSyncKHR", [(EGLDisplay, "dpy"), (EGLenum, "type"), (EGLIntArray([]), "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroySyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync")]),
    GlFunction(EGLint, "eglClientWaitSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint, "flags"), (EGLTimeKHR, "timeout")]),
    GlFunction(EGLBoolean, "eglGetSyncAttribKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_KHR_image
    GlFunction(EGLImageKHR, "eglCreateImageKHR", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLenum, "target"), (EGLClientBuffer, "buffer"), (EGLIntArray([('EGL_IMAGE_PRESERVED_KHR', EGLBoolean)]), "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroyImageKHR", [(EGLDisplay, "dpy"), (EGLImageKHR, "image")]),

    # EGL_KHR_image_base

    # EGL_KHR_lock_surface
    GlFunction(EGLBoolean, "eglLockSurfaceKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLLockSurfaceKHRAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglUnlockSurfaceKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),

    # EGL_KHR_reusable_sync
    GlFunction(EGLBoolean, "eglSignalSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLenum, "mode")]),

    # EGL_KHR_swap_buffers_with_damage
    GlFunction(EGLBoolean, "eglSwapBuffersWithDamageKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (Array(EGLint, "n_rects*4"), "rects"), (EGLint, "n_rects")]),

    # EGL_KHR_wait_sync
    GlFunction(EGLint, "eglWaitSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint, "flags")]),

    # EGL_NV_sync
    GlFunction(EGLSyncNV, "eglCreateFenceSyncNV", [(EGLDisplay, "dpy"), (EGLenum, "condition"), (EGLFenceSyncNVAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroySyncNV", [(EGLSyncNV, "sync")]),
    GlFunction(EGLBoolean, "eglFenceNV", [(EGLSyncNV, "sync")]),
    GlFunction(EGLint, "eglClientWaitSyncNV", [(EGLSyncNV, "sync"), (EGLint, "flags"), (EGLTimeNV, "timeout")]),
    GlFunction(EGLBoolean, "eglSignalSyncNV", [(EGLSyncNV, "sync"), (EGLenum, "mode")]),
    GlFunction(EGLBoolean, "eglGetSyncAttribNV", [(EGLSyncNV, "sync"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_MESA_drm_image
    GlFunction(EGLImageKHR, "eglCreateDRMImageMESA", [(EGLDisplay, "dpy"), (EGLDrmImageMesaAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglExportDRMImageMESA", [(EGLDisplay, "dpy"), (EGLImageKHR, "image"), Out(Pointer(EGLint), "name"), Out(Pointer(EGLint), "handle"), Out(Pointer(EGLint), "stride")]),

    # EGL_NV_post_sub_buffer
    GlFunction(EGLBoolean, "eglPostSubBufferNV", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint, "x"), (EGLint, "y"), (EGLint, "width"), (EGLint, "height")]),

    # EGL_NV_system_time
    GlFunction(EGLuint64NV, "eglGetSystemTimeFrequencyNV", [], sideeffects=False),
    GlFunction(EGLuint64NV, "eglGetSystemTimeNV", [], sideeffects=False),

    # GL_OES_EGL_image
    GlFunction(Void, "glEGLImageTargetTexture2DOES", [(GLenum, "target"), (EGLImageKHR, "image")]),
    GlFunction(Void, "glEGLImageTargetRenderbufferStorageOES", [(GLenum, "target"), (EGLImageKHR, "image")]),
])

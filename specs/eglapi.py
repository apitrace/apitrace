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


from .stdapi import *
from .gltypes import *
from .eglenum import *

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

# EGL_EXT_device_query
EGLDeviceEXT = Opaque("EGLDeviceEXT")

# EGL_EXT_image_dma_buf_import_modifiers
EGLuint64KHR = Alias("EGLuint64KHR", UInt64)
EGLModifier = Bitmask(EGLuint64KHR, [])
Hex = Bitmask(Int, [])

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

# EGL_WL_bind_wayland_display
WlDisplay = Opaque("struct wl_display*")
WlResource = Opaque("struct wl_resource*")

# EGL_KHR_debug
EGLDEBUGPROCKHR = Opaque("EGLDEBUGPROCKHR")
EGLLabelKHR = Opaque("EGLLabelKHR")
EGLObjectKHR = Opaque("EGLObjectKHR")

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
EGLTextureTarget = FakeEnum(Int, ['EGL_TEXTURE_2D', 'EGL_NO_TEXTURE'])

EGLTextureFormat = FakeEnum(Int, [
    'EGL_NO_TEXTURE',
    'EGL_TEXTURE_RGB',
    'EGL_TEXTURE_RGBA',

    # EGL_WL_bind_wayland_display
    'EGL_TEXTURE_Y_U_V_WL',
    'EGL_TEXTURE_Y_UV_WL',
    'EGL_TEXTURE_Y_XUXV_WL',
    'EGL_TEXTURE_EXTERNAL_WL',
])

EGLObjectType = FakeEnum(Int, [
    'EGL_OBJECT_THREAD_KHR',
    'EGL_OBJECT_DISPLAY_KHR',
    'EGL_OBJECT_CONTEXT_KHR',
    'EGL_OBJECT_SURFACE_KHR',
    'EGL_OBJECT_IMAGE_KHR',
    'EGL_OBJECT_SYNC_KHR',
    'EGL_OBJECT_STREAM_KHR',
])

EGLDebugMessageType = FakeEnum(Int, [
    'EGL_DEBUG_MSG_CRITICAL_KHR',
    'EGL_DEBUG_MSG_ERROR_KHR',
    'EGL_DEBUG_MSG_WARN_KHR',
    'EGL_DEBUG_MSG_INFO_KHR',
])

EGLSurfaceCompressionRate = FakeEnum(Int, [
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_NONE_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_DEFAULT_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_1BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_2BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_3BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_4BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_5BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_6BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_7BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_8BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_9BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_10BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_11BPC_EXT',
    'EGL_SURFACE_COMPRESSION_FIXED_RATE_12BPC_EXT',
])

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

WindowSurfaceAttribsValues = [
    ('EGL_RENDER_BUFFER', FakeEnum(Int, ['EGL_SINGLE_BUFFER', 'EGL_BACK_BUFFER'])),
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace),
    ('EGL_POST_SUB_BUFFER_SUPPORTED_NV', EGLBoolean),
    ('EGL_SURFACE_COMPRESSION_EXT', EGLSurfaceCompressionRate),
    ('EGL_SURFACE_COMPRESSION_PLANE1_EXT', EGLSurfaceCompressionRate),
    ('EGL_SURFACE_COMPRESSION_PLANE2_EXT', EGLSurfaceCompressionRate),
]
EGLWindowSurfaceAttribs = EGLAttribArray(WindowSurfaceAttribsValues)
EGLWindowSurfaceAttribs_int = EGLIntArray(WindowSurfaceAttribsValues)


EGLPixmapSurfaceAttribsValues = [
    ('EGL_VG_ALPHA_FORMAT', EGLVGAlphaFormat),
    ('EGL_VG_COLORSPACE', EGLVGColorspace)
]
EGLPixmapSurfaceAttribs = EGLAttribArray(EGLPixmapSurfaceAttribsValues)
EGLPixmapSurfaceAttribs_int = EGLIntArray(EGLPixmapSurfaceAttribsValues)

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
    ('EGL_GENERATE_RESET_ON_VIDEO_MEMORY_PURGE_NV', EGLBoolean),

    # EGL_IMG_context_priority
    ('EGL_CONTEXT_PRIORITY_LEVEL_IMG', Flags(Int, [
        'EGL_CONTEXT_PRIORITY_HIGH_IMG',
        'EGL_CONTEXT_PRIORITY_MEDIUM_IMG',
        'EGL_CONTEXT_PRIORITY_LOW_IMG',

        # EGL_NV_context_priority_realtime
        'EGL_CONTEXT_PRIORITY_REALTIME_NV',
    ])),
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

EGLFenceSyncKHRAttribs = EGLIntArray([
    ('EGL_SYNC_NATIVE_FENCE_FD_ANDROID', Int)
])

EGLPlatformDisplayAttribs = EGLAttribArray([
    ('EGL_PLATFORM_X11_SCREEN_EXT', Int),
])

EGLPlatformDisplayAttribsEXT = EGLIntArray([
    ('EGL_PLATFORM_X11_SCREEN_EXT', Int),
])

eglImageAttribs = [
    ('EGL_IMAGE_PRESERVED', EGLBoolean),

    ('EGL_GL_TEXTURE_LEVEL', Int),
    ('EGL_GL_TEXTURE_ZOFFSET', Int),

    # EGL_EXT_image_dma_buf_import
    ('EGL_LINUX_DRM_FOURCC_EXT', Int),
    ('EGL_DMA_BUF_PLANE0_FD_EXT', Int),
    ('EGL_DMA_BUF_PLANE0_OFFSET_EXT', Int),
    ('EGL_DMA_BUF_PLANE0_PITCH_EXT', Int),
    ('EGL_DMA_BUF_PLANE1_FD_EXT', Int),
    ('EGL_DMA_BUF_PLANE1_OFFSET_EXT', Int),
    ('EGL_DMA_BUF_PLANE1_PITCH_EXT', Int),
    ('EGL_DMA_BUF_PLANE2_FD_EXT', Int),
    ('EGL_DMA_BUF_PLANE2_OFFSET_EXT', Int),
    ('EGL_DMA_BUF_PLANE2_PITCH_EXT', Int),
    ('EGL_YUV_COLOR_SPACE_HINT_EXT', FakeEnum(Int, ['EGL_ITU_REC601_EXT', 'EGL_ITU_REC709_EXT', 'EGL_ITU_REC2020_EXT'])),
    ('EGL_SAMPLE_RANGE_HINT_EXT', FakeEnum(Int, ['EGL_YUV_FULL_RANGE_EXT', 'EGL_YUV_NARROW_RANGE_EXT'])),
    ('EGL_YUV_CHROMA_HORIZONTAL_SITING_HINT_EXT', FakeEnum(Int, ['EGL_YUV_CHROMA_SITING_0_EXT', 'EGL_YUV_CHROMA_SITING_0_5_EXT'])),
    ('EGL_YUV_CHROMA_VERTICAL_SITING_HINT_EXT', FakeEnum(Int, ['EGL_YUV_CHROMA_SITING_0_EXT', 'EGL_YUV_CHROMA_SITING_0_5_EXT'])),

    # EGL_EXT_image_dma_buf_import_modifiers
    ('EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT', Hex),
    ('EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT', Hex),
    ('EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT', Hex),
    ('EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT', Hex),
    ('EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT', Hex),
    ('EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT', Hex),
    ('EGL_DMA_BUF_PLANE3_FD_EXT', Int),
    ('EGL_DMA_BUF_PLANE3_OFFSET_EXT', Int),
    ('EGL_DMA_BUF_PLANE3_PITCH_EXT', Int),
    ('EGL_DMA_BUF_PLANE3_MODIFIER_LO_EXT', Hex),
    ('EGL_DMA_BUF_PLANE3_MODIFIER_HI_EXT', Hex),

    # EGL_WL_bind_wayland_display
    ('EGL_WAYLAND_PLANE_WL', Int),
]
EGLImageAttribs = EGLAttribArray(eglImageAttribs)
EGLImageAttribsKHR = EGLIntArray(eglImageAttribs)

EGLDebugAttribs = EGLAttribArray([
    ('EGL_DEBUG_MSG_CRITICAL_KHR', EGLBoolean),
    ('EGL_DEBUG_MSG_ERROR_KHR', EGLBoolean),
    ('EGL_DEBUG_MSG_WARN_KHR', EGLBoolean),
    ('EGL_DEBUG_MSG_INFO_KHR', EGLBoolean),
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
    GlFunction(EGLSurface, "eglCreatePixmapSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativePixmapType, "pixmap"), (EGLPixmapSurfaceAttribs_int, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreateWindowSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativeWindowType, "win"), (EGLWindowSurfaceAttribs_int, "attrib_list")]),
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
    GlFunction(EGLImage, "eglCreateImage", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLenum, "target"), (EGLClientBuffer, "buffer"), (EGLImageAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroyImage", [(EGLDisplay, "dpy"), (EGLImage, "image")]),
    GlFunction(EGLDisplay, "eglGetPlatformDisplay", [(EGLenum, "platform"), (OpaquePointer(Void), "native_display"), (EGLPlatformDisplayAttribs, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformWindowSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_window"), (EGLWindowSurfaceAttribs, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformPixmapSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_pixmap"), (EGLPixmapSurfaceAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglWaitSync", [(EGLDisplay, "dpy"), (EGLSync, "sync"), (EGLint, "flags")]),

    # EGL_ANDROID_native_fence_sync
    GlFunction(EGLint, "eglDupNativeFenceFDANDROID", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync")]),

    # EGL_ANGLE_query_surface_pointer
    GlFunction(EGLBoolean, "eglQuerySurfacePointerANGLE", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint_enum, "attribute"), Out(Pointer(OpaquePointer(Void)), "value")], sideeffects=False),

    # EGL_CHROMIUM_get_sync_values
    GlFunction(Bool, "eglGetSyncValuesCHROMIUM", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), Out(Pointer(Int64), "ust"), Out(Pointer(Int64), "msc"), Out(Pointer(Int64), "sbc")], sideeffects=False),

    # EGL_EXT_device_enumeration
    GlFunction(EGLBoolean, "eglQueryDevicesEXT", [(EGLint, "max_devices"), Out(Array(EGLDeviceEXT, "max_devices"), "devices"), Out(Pointer(EGLint), "num_devices")], sideeffects=False),

    # EGL_EXT_device_query
    GlFunction(EGLBoolean, "eglQueryDeviceAttribEXT", [(EGLDeviceEXT, "device"), (EGLint_enum, "attribute"), Out(Pointer(EGLAttrib), "value")], sideeffects=False),
    GlFunction(ConstCString, "eglQueryDeviceStringEXT", [(EGLDeviceEXT, "device"), (EGLint_enum, "name")], sideeffects=False),
    GlFunction(EGLBoolean, "eglQueryDisplayAttribEXT", [(EGLDisplay, "dpy"), (EGLint_enum, "attribute"), Out(Pointer(EGLAttrib), "value")], sideeffects=False),

    # EGL_EXT_image_dma_buf_import_modifiers
    GlFunction(EGLBoolean, "eglQueryDmaBufFormatsEXT", [(EGLDisplay, "dpy"), (EGLint, "max_formats"), Out(Array(EGLint, "max_formats"), "formats"), Out(Pointer(EGLint), "num_formats")], sideeffects=False),
    GlFunction(EGLBoolean, "eglQueryDmaBufModifiersEXT", [(EGLDisplay, "dpy"), (EGLint, "format"), (EGLint, "max_modifiers"), Out(Array(EGLModifier, "max_modifiers"), "modifiers"), Out(Array(EGLBoolean, "max_modifiers"), "external_only"), Out(Pointer(EGLint), "num_modifiers")], sideeffects=False),

    # EGL_EXT_platform_base
    GlFunction(EGLDisplay, "eglGetPlatformDisplayEXT", [(EGLenum, "platform"), (OpaquePointer(Void), "native_display"), (EGLPlatformDisplayAttribsEXT, "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformWindowSurfaceEXT", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_window"), (EGLIntArray([]), "attrib_list")]),
    GlFunction(EGLSurface, "eglCreatePlatformPixmapSurfaceEXT", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (OpaquePointer(Void), "native_pixmap"), (EGLIntArray([]), "attrib_list")]),

    # EGL_KHR_partial_update
    GlFunction(EGLBoolean, "eglSetDamageRegionKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (Array(EGLint, "n_rects*4"), "rects"), (EGLint, "n_rects")]),

    # EGL_EXT_swap_buffers_with_damage
    GlFunction(EGLBoolean, "eglSwapBuffersWithDamageEXT", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (Array(Const(EGLint), "n_rects*4"), "rects"), (EGLint, "n_rects")]),

    # EGL_HI_clientpixmap
    GlFunction(EGLSurface, "eglCreatePixmapSurfaceHI", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (Pointer(EGLClientPixmapHI), "pixmap")]),

    # EGL_KHR_fence_sync
    GlFunction(EGLSyncKHR, "eglCreateSyncKHR", [(EGLDisplay, "dpy"), (EGLenum, "type"), (EGLFenceSyncKHRAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroySyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync")]),
    GlFunction(EGLint, "eglClientWaitSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint, "flags"), (EGLTimeKHR, "timeout")]),
    GlFunction(EGLBoolean, "eglGetSyncAttribKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_KHR_image
    GlFunction(EGLImageKHR, "eglCreateImageKHR", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLenum, "target"), (EGLClientBuffer, "buffer"), (EGLImageAttribsKHR, "attrib_list")]),
    GlFunction(EGLBoolean, "eglDestroyImageKHR", [(EGLDisplay, "dpy"), (EGLImageKHR, "image")]),

    # EGL_KHR_image_base

    # EGL_KHR_lock_surface
    GlFunction(EGLBoolean, "eglLockSurfaceKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLLockSurfaceKHRAttribs, "attrib_list")]),
    GlFunction(EGLBoolean, "eglUnlockSurfaceKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),

    # EGL_KHR_reusable_sync
    GlFunction(EGLBoolean, "eglSignalSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLenum, "mode")]),

    # EGL_KHR_swap_buffers_with_damage
    GlFunction(EGLBoolean, "eglSwapBuffersWithDamageKHR", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (Array(Const(EGLint), "n_rects*4"), "rects"), (EGLint, "n_rects")]),

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

    # EGL_MESA_image_dma_buf_export
    GlFunction(EGLBoolean, "eglExportDMABUFImageQueryMESA", [(EGLDisplay, "dpy"), (EGLImageKHR, "image"), Out(Pointer(EGLint), "fourcc"), Out(Pointer(EGLint), "num_planes"), Out(Pointer(EGLModifier), "modifiers")]),
    GlFunction(EGLBoolean, "eglExportDMABUFImageMESA", [(EGLDisplay, "dpy"), (EGLImageKHR, "image"), Out(Array(EGLint, 4), "fds"), Out(Array(EGLint, 4), "strides"), Out(Array(EGLint, 4), "offsets")]),

    # EGL_NV_post_sub_buffer
    GlFunction(EGLBoolean, "eglPostSubBufferNV", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLint, "x"), (EGLint, "y"), (EGLint, "width"), (EGLint, "height")]),

    # EGL_NV_system_time
    GlFunction(EGLuint64NV, "eglGetSystemTimeFrequencyNV", [], sideeffects=False),
    GlFunction(EGLuint64NV, "eglGetSystemTimeNV", [], sideeffects=False),

    # GL_OES_EGL_image
    GlFunction(Void, "glEGLImageTargetTexture2DOES", [(GLenum, "target"), (EGLImageKHR, "image")]),
    GlFunction(Void, "glEGLImageTargetRenderbufferStorageOES", [(GLenum, "target"), (EGLImageKHR, "image")]),

    # EGL_EXT_surface_compression
    GlFunction(EGLBoolean, "eglQuerySupportedCompressionRatesEXT", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLWindowSurfaceAttribs, "attrib_list"), Out(Array(EGLSurfaceCompressionRate, "*num_rates"), "rates"), (EGLint, "rate_size"), Out(Pointer(EGLint), "num_rates")], sideeffects=False),

    # EGL_WL_bind_wayland_display
    GlFunction(EGLBoolean, "eglBindWaylandDisplayWL", [(EGLDisplay, "dpy"), (WlDisplay, "display")]),
    GlFunction(EGLBoolean, "eglUnbindWaylandDisplayWL", [(EGLDisplay, "dpy"), (WlDisplay, "display")]),
    GlFunction(EGLBoolean, "eglQueryWaylandBufferWL", [(EGLDisplay, "dpy"), (WlResource, "buffer"), (EGLint_enum, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_KHR_debug
    GlFunction(EGLint, "eglDebugMessageControlKHR", [(EGLDEBUGPROCKHR, "callback"), (EGLDebugAttribs, "attrib_list")], sideeffects=False),
    GlFunction(EGLBoolean, "eglQueryDebugKHR", [(EGLint, "attribute"), (OpaquePointer(EGLAttrib), "value")], sideeffects=False),
    GlFunction(EGLint, "eglLabelObjectKHR", [(EGLDisplay, "display"), (EGLenum, "objectType"), (EGLObjectKHR, "object"), (EGLLabelKHR, "label")]),
])

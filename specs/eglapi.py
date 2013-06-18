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

EGLAttribList = Array(Const(EGLattrib), "_AttribPairList_size(attrib_list, EGL_NONE)")

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
    Function(EGLBoolean, "eglChooseConfig", [(EGLDisplay, "dpy"), (EGLAttribList, "attrib_list"), (Array(EGLConfig, "config_size"), "configs"), (EGLint, "config_size"), Out(Pointer(EGLint), "num_config")]),
    Function(EGLBoolean, "eglGetConfigAttrib", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    Function(EGLSurface, "eglCreateWindowSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativeWindowType, "win"), (EGLAttribList, "attrib_list")]),
    Function(EGLSurface, "eglCreatePbufferSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLAttribList, "attrib_list")]),
    Function(EGLSurface, "eglCreatePixmapSurface", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLNativePixmapType, "pixmap"), (EGLAttribList, "attrib_list")]),
    Function(EGLBoolean, "eglDestroySurface", [(EGLDisplay, "dpy"), (EGLSurface, "surface")]),
    Function(EGLBoolean, "eglQuerySurface", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    Function(EGLBoolean, "eglBindAPI", [(EGLenum, "api")]),
    Function(EGLenum, "eglQueryAPI", [], sideeffects=False),

    Function(EGLBoolean, "eglWaitClient", []),

    Function(EGLBoolean, "eglReleaseThread", []),

    Function(EGLSurface, "eglCreatePbufferFromClientBuffer", [(EGLDisplay, "dpy"), (EGLenum, "buftype"), (EGLClientBuffer, "buffer"), (EGLConfig, "config"), (EGLAttribList, "attrib_list")]),

    Function(EGLBoolean, "eglSurfaceAttrib", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "attribute"), (EGLint, "value")]),
    Function(EGLBoolean, "eglBindTexImage", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "buffer")]),
    Function(EGLBoolean, "eglReleaseTexImage", [(EGLDisplay, "dpy"), (EGLSurface, "surface"), (EGLattrib, "buffer")]),

    Function(EGLBoolean, "eglSwapInterval", [(EGLDisplay, "dpy"), (EGLint, "interval")]),

    Function(EGLContext, "eglCreateContext", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (EGLContext, "share_context"), (EGLAttribList, "attrib_list")]),
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
    Function(EGLBoolean, "eglLockSurfaceKHR", [(EGLDisplay, "display"), (EGLSurface, "surface"), (EGLAttribList, "attrib_list")]),
    Function(EGLBoolean, "eglUnlockSurfaceKHR", [(EGLDisplay, "display"), (EGLSurface, "surface")]),

    # EGL_KHR_image_base
    Function(EGLImageKHR, "eglCreateImageKHR", [(EGLDisplay, "dpy"), (EGLContext, "ctx"), (EGLenum, "target"), (EGLClientBuffer, "buffer"), (EGLAttribList, "attrib_list")]),
    Function(EGLBoolean, "eglDestroyImageKHR", [(EGLDisplay, "dpy"), (EGLImageKHR, "image")]),

    # EGL_KHR_fence_sync
    # EGL_KHR_reusable_sync
    Function(EGLSyncKHR, "eglCreateSyncKHR", [(EGLDisplay, "dpy"), (EGLenum, "type"), (EGLAttribList, "attrib_list")]),
    Function(EGLBoolean, "eglDestroySyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync")]),
    Function(EGLint, "eglClientWaitSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLint, "flags"), (EGLTimeKHR, "timeout")]),
    Function(EGLBoolean, "eglSignalSyncKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLenum, "mode")]),
    Function(EGLBoolean, "eglGetSyncAttribKHR", [(EGLDisplay, "dpy"), (EGLSyncKHR, "sync"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_NV_sync
    Function(EGLSyncNV, "eglCreateFenceSyncNV", [(EGLDisplay, "dpy"), (EGLenum, "condition"), (EGLAttribList, "attrib_list")]),
    Function(EGLBoolean, "eglDestroySyncNV", [(EGLSyncNV, "sync")]),
    Function(EGLBoolean, "eglFenceNV", [(EGLSyncNV, "sync")]),
    Function(EGLint, "eglClientWaitSyncNV", [(EGLSyncNV, "sync"), (EGLint, "flags"), (EGLTimeNV, "timeout")]),
    Function(EGLBoolean, "eglSignalSyncNV", [(EGLSyncNV, "sync"), (EGLenum, "mode")]),
    Function(EGLBoolean, "eglGetSyncAttribNV", [(EGLSyncNV, "sync"), (EGLattrib, "attribute"), Out(Pointer(EGLint), "value")], sideeffects=False),

    # EGL_HI_clientpixmap
    Function(EGLSurface, "eglCreatePixmapSurfaceHI", [(EGLDisplay, "dpy"), (EGLConfig, "config"), (Pointer(EGLClientPixmapHI), "pixmap")]),

    # EGL_MESA_drm_image
    Function(EGLImageKHR, "eglCreateDRMImageMESA", [(EGLDisplay, "dpy"), (EGLAttribList, "attrib_list")]),
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

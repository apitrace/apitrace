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

"""GLX API description."""


from stdapi import *
from glapi import *

VisualID = Alias("VisualID", UInt32)
Display = Opaque("Display *")
Visual = Opaque("Visual *")
Font = Alias("Font", UInt32)
Pixmap = Alias("Pixmap", UInt32)
Window = Alias("Window", UInt32)
Colormap = Alias("Colormap", UInt32)

GLXContext = Opaque("GLXContext")
GLXPixmap = Alias("GLXPixmap", UInt32)
GLXDrawable = Alias("GLXDrawable", UInt32)
GLXFBConfig = Opaque("GLXFBConfig")
GLXFBConfigSGIX = Opaque("GLXFBConfigSGIX")
GLXFBConfigID = Alias("GLXFBConfigID", UInt32)
GLXContextID = Alias("GLXContextID", UInt32)
GLXWindow = Alias("GLXWindow", UInt32)
GLXPbuffer = Alias("GLXPbuffer", UInt32)
GLXPbufferSGIX = Alias("GLXPbufferSGIX", UInt32)
GLXVideoSourceSGIX = Alias("GLXVideoSourceSGIX", UInt32)
GLXVideoDeviceNV = Alias("GLXVideoDeviceNV", UInt32)
GLXVideoCaptureDeviceNV = Alias("GLXVideoCaptureDeviceNV", UInt32)

XVisualInfo = Struct("XVisualInfo", [
  (Visual, "visual"),
  (VisualID, "visualid"),
  (Int, "screen"),
  (Int, "depth"),
  (Int, "c_class"),
  (ULong, "red_mask"),
  (ULong, "green_mask"),
  (ULong, "blue_mask"),
  (Int, "colormap_size"),
  (Int, "bits_per_rgb"),
])

Bool = FakeEnum(Int, [
    "False",
    "True",
])

GLXEnum = FakeEnum(Int, [
    #"GLX_USE_GL", 		# 1
    "GLX_BUFFER_SIZE", 		# 2
    "GLX_LEVEL", 		# 3
    "GLX_RGBA", 		# 4
    "GLX_DOUBLEBUFFER", 	# 5
    "GLX_STEREO", 		# 6
    "GLX_AUX_BUFFERS", 		# 7
    "GLX_RED_SIZE", 		# 8
    "GLX_GREEN_SIZE", 		# 9
    "GLX_BLUE_SIZE", 		# 10
    "GLX_ALPHA_SIZE", 		# 11
    "GLX_DEPTH_SIZE", 		# 12
    "GLX_STENCIL_SIZE", 	# 13
    "GLX_ACCUM_RED_SIZE", 	# 14
    "GLX_ACCUM_GREEN_SIZE", 	# 15
    "GLX_ACCUM_BLUE_SIZE", 	# 16
    "GLX_ACCUM_ALPHA_SIZE", 	# 17

    "GLX_CONFIG_CAVEAT",		# 0x0020
    "GLX_X_VISUAL_TYPE",		# 0x0022
    "GLX_TRANSPARENT_TYPE",		# 0x0023
    "GLX_TRANSPARENT_INDEX_VALUE",		# 0x0024
    "GLX_TRANSPARENT_RED_VALUE",		# 0x0025
    "GLX_TRANSPARENT_GREEN_VALUE",		# 0x0026
    "GLX_TRANSPARENT_BLUE_VALUE",		# 0x0027
    "GLX_TRANSPARENT_ALPHA_VALUE",		# 0x0028
    "GLX_HYPERPIPE_PIPE_NAME_LENGTH_SGIX",		# 0x0050
    "GLX_BAD_HYPERPIPE_CONFIG_SGIX",		# 0x005B
    "GLX_BAD_HYPERPIPE_SGIX",		# 0x005C
    "GLX_GPU_VENDOR_AMD",		# 0x1F00
    "GLX_GPU_RENDERER_STRING_AMD",		# 0x1F01
    "GLX_GPU_OPENGL_VERSION_STRING_AMD",		# 0x1F02
    "GLX_CONTEXT_MAJOR_VERSION_ARB",		# 0x2091
    "GLX_CONTEXT_MINOR_VERSION_ARB",		# 0x2092
    "GLX_CONTEXT_FLAGS_ARB",		# 0x2094
    "GLX_CONTEXT_ALLOW_BUFFER_BYTE_ORDER_MISMATCH_ARB",		# 0x2095
    "GLX_FLOAT_COMPONENTS_NV",		# 0x20B0
    "GLX_RGBA_UNSIGNED_FLOAT_TYPE_EXT",		# 0x20B1
    "GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB",		# 0x20B2
    "GLX_COLOR_SAMPLES_NV",		# 0x20B3
    "GLX_RGBA_FLOAT_TYPE_ARB",		# 0x20B9
    "GLX_VIDEO_OUT_COLOR_NV",		# 0x20C3
    "GLX_VIDEO_OUT_ALPHA_NV",		# 0x20C4
    "GLX_VIDEO_OUT_DEPTH_NV",		# 0x20C5
    "GLX_VIDEO_OUT_COLOR_AND_ALPHA_NV",		# 0x20C6
    "GLX_VIDEO_OUT_COLOR_AND_DEPTH_NV",		# 0x20C7
    "GLX_VIDEO_OUT_FRAME_NV",		# 0x20C8
    "GLX_VIDEO_OUT_FIELD_1_NV",		# 0x20C9
    "GLX_VIDEO_OUT_FIELD_2_NV",		# 0x20CA
    "GLX_VIDEO_OUT_STACKED_FIELDS_1_2_NV",		# 0x20CB
    "GLX_VIDEO_OUT_STACKED_FIELDS_2_1_NV",		# 0x20CC
    "GLX_DEVICE_ID_NV",		# 0x20CD
    "GLX_UNIQUE_ID_NV",		# 0x20CE
    "GLX_NUM_VIDEO_CAPTURE_SLOTS_NV",		# 0x20CF
    "GLX_BIND_TO_TEXTURE_RGB_EXT",		# 0x20D0
    "GLX_BIND_TO_TEXTURE_RGBA_EXT",		# 0x20D1
    "GLX_BIND_TO_MIPMAP_TEXTURE_EXT",		# 0x20D2
    "GLX_BIND_TO_TEXTURE_TARGETS_EXT",		# 0x20D3
    "GLX_Y_INVERTED_EXT",		# 0x20D4
    "GLX_TEXTURE_FORMAT_EXT",		# 0x20D5
    "GLX_TEXTURE_TARGET_EXT",		# 0x20D6
    "GLX_MIPMAP_TEXTURE_EXT",		# 0x20D7
    "GLX_TEXTURE_FORMAT_NONE_EXT",		# 0x20D8
    "GLX_TEXTURE_FORMAT_RGB_EXT",		# 0x20D9
    "GLX_TEXTURE_FORMAT_RGBA_EXT",		# 0x20DA
    "GLX_TEXTURE_1D_EXT",		# 0x20DB
    "GLX_TEXTURE_2D_EXT",		# 0x20DC
    "GLX_TEXTURE_RECTANGLE_EXT",		# 0x20DD
    "GLX_FRONT_LEFT_EXT",		# 0x20DE
    "GLX_FRONT_RIGHT_EXT",		# 0x20DF
    "GLX_BACK_LEFT_EXT",		# 0x20E0
    "GLX_BACK_RIGHT_EXT",		# 0x20E1
    "GLX_AUX0_EXT",		# 0x20E2
    "GLX_AUX1_EXT",		# 0x20E3
    "GLX_AUX2_EXT",		# 0x20E4
    "GLX_AUX3_EXT",		# 0x20E5
    "GLX_AUX4_EXT",		# 0x20E6
    "GLX_AUX5_EXT",		# 0x20E7
    "GLX_AUX6_EXT",		# 0x20E8
    "GLX_AUX7_EXT",		# 0x20E9
    "GLX_AUX8_EXT",		# 0x20EA
    "GLX_AUX9_EXT",		# 0x20EB
    "GLX_NUM_VIDEO_SLOTS_NV",		# 0x20F0
    "GLX_SWAP_INTERVAL_EXT",		# 0x20F1
    "GLX_MAX_SWAP_INTERVAL_EXT",		# 0x20F2
    "GLX_LATE_SWAPS_TEAR_EXT",		# 0x20F3
    "GLX_BACK_BUFFER_AGE_EXT",		# 0x20F4
    "GLX_STEREO_TREE_EXT",		# 0x20F5
    "GLX_GPU_FASTEST_TARGET_GPUS_AMD",		# 0x21A2
    "GLX_GPU_RAM_AMD",		# 0x21A3
    "GLX_GPU_CLOCK_AMD",		# 0x21A4
    "GLX_GPU_NUM_PIPES_AMD",		# 0x21A5
    "GLX_GPU_NUM_SIMD_AMD",		# 0x21A6
    "GLX_GPU_NUM_RB_AMD",		# 0x21A7
    "GLX_GPU_NUM_SPI_AMD",		# 0x21A8
    "GLX_NONE",		# 0x8000
    "GLX_SLOW_CONFIG",		# 0x8001
    "GLX_TRUE_COLOR",		# 0x8002
    "GLX_DIRECT_COLOR",		# 0x8003
    "GLX_PSEUDO_COLOR",		# 0x8004
    "GLX_STATIC_COLOR",		# 0x8005
    "GLX_GRAY_SCALE",		# 0x8006
    "GLX_STATIC_GRAY",		# 0x8007
    "GLX_TRANSPARENT_RGB",		# 0x8008
    "GLX_TRANSPARENT_INDEX",		# 0x8009
    "GLX_SHARE_CONTEXT_EXT",		# 0x800A
    "GLX_VISUAL_ID",		# 0x800B
    "GLX_SCREEN",		# 0x800C
    "GLX_NON_CONFORMANT_CONFIG",		# 0x800D
    "GLX_DRAWABLE_TYPE",		# 0x8010
    "GLX_RENDER_TYPE",		# 0x8011
    "GLX_X_RENDERABLE",		# 0x8012
    "GLX_FBCONFIG_ID",		# 0x8013
    "GLX_RGBA_TYPE",		# 0x8014
    "GLX_COLOR_INDEX_TYPE",		# 0x8015
    "GLX_MAX_PBUFFER_WIDTH",		# 0x8016
    "GLX_MAX_PBUFFER_HEIGHT",		# 0x8017
    "GLX_MAX_PBUFFER_PIXELS",		# 0x8018
    "GLX_OPTIMAL_PBUFFER_WIDTH_SGIX",		# 0x8019
    "GLX_OPTIMAL_PBUFFER_HEIGHT_SGIX",		# 0x801A
    "GLX_PRESERVED_CONTENTS",		# 0x801B
    "GLX_LARGEST_PBUFFER",		# 0x801C
    "GLX_WIDTH",		# 0x801D
    "GLX_HEIGHT",		# 0x801E
    "GLX_EVENT_MASK",		# 0x801F
    "GLX_DAMAGED",		# 0x8020
    "GLX_SAVED",		# 0x8021
    "GLX_WINDOW",		# 0x8022
    "GLX_PBUFFER",		# 0x8023
    #"GLX_DIGITAL_MEDIA_PBUFFER_SGIX",		# 0x8024
    "GLX_BLENDED_RGBA_SGIS",		# 0x8025
    "GLX_MULTISAMPLE_SUB_RECT_WIDTH_SGIS",		# 0x8026
    "GLX_MULTISAMPLE_SUB_RECT_HEIGHT_SGIS",		# 0x8027
    "GLX_VISUAL_SELECT_GROUP_SGIX",		# 0x8028
    "GLX_HYPERPIPE_ID_SGIX",		# 0x8030
    "GLX_PBUFFER_HEIGHT",		# 0x8040
    "GLX_PBUFFER_WIDTH",		# 0x8041
    "GLX_SAMPLE_BUFFERS_3DFX",		# 0x8050
    "GLX_SAMPLES_3DFX",		# 0x8051
    "GLX_SWAP_METHOD_OML",		# 0x8060
    "GLX_SWAP_EXCHANGE_OML",		# 0x8061
    "GLX_SWAP_COPY_OML",		# 0x8062
    "GLX_SWAP_UNDEFINED_OML",		# 0x8063
    "GLX_EXCHANGE_COMPLETE_INTEL",		# 0x8180
    "GLX_COPY_COMPLETE_INTEL",		# 0x8181
    "GLX_FLIP_COMPLETE_INTEL",		# 0x8182
    "GLX_RENDERER_VENDOR_ID_MESA",		# 0x8183
    "GLX_RENDERER_DEVICE_ID_MESA",		# 0x8184
    "GLX_RENDERER_VERSION_MESA",		# 0x8185
    "GLX_RENDERER_ACCELERATED_MESA",		# 0x8186
    "GLX_RENDERER_VIDEO_MEMORY_MESA",		# 0x8187
    "GLX_RENDERER_UNIFIED_MEMORY_ARCHITECTURE_MESA",		# 0x8188
    "GLX_RENDERER_PREFERRED_PROFILE_MESA",		# 0x8189
    "GLX_RENDERER_OPENGL_CORE_PROFILE_VERSION_MESA",		# 0x818A
    "GLX_RENDERER_OPENGL_COMPATIBILITY_PROFILE_VERSION_MESA",		# 0x818B
    "GLX_RENDERER_OPENGL_ES_PROFILE_VERSION_MESA",		# 0x818C
    "GLX_RENDERER_OPENGL_ES2_PROFILE_VERSION_MESA",		# 0x818D
    "GLX_RENDERER_ID_MESA",		# 0x818E
    "GLX_LOSE_CONTEXT_ON_RESET_ARB",		# 0x8252
    "GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB",		# 0x8256
    "GLX_NO_RESET_NOTIFICATION_ARB",		# 0x8261
    "GLX_CONTEXT_PROFILE_MASK_ARB",		# 0x9126
    "GLX_SAMPLE_BUFFERS",		# 0x186A0
    "GLX_SAMPLES",		# 0x186A1
    "GLX_DONT_CARE",		# 0xFFFFFFFF
])

GLXError = FakeEnum(Int, [
    "GLX_BAD_SCREEN",
    "GLX_BAD_ATTRIBUTE",
    "GLX_NO_EXTENSION",
    "GLX_BAD_VISUAL",
    "GLX_BAD_CONTEXT",
    "GLX_BAD_VALUE",
    "GLX_BAD_ENUM",
])

GLXname = FakeEnum(Int, [
    "GLX_VENDOR",
    "GLX_VERSION",
    "GLX_EXTENSIONS",
])

GLXbuffer = Flags(Int, [
    "GLX_WINDOW_BIT",
    "GLX_PIXMAP_BIT",
    "GLX_PBUFFER_BIT",
    "GLX_AUX_BUFFERS_BIT",
    "GLX_FRONT_LEFT_BUFFER_BIT",
    "GLX_FRONT_RIGHT_BUFFER_BIT",
    "GLX_BACK_LEFT_BUFFER_BIT",
    "GLX_BACK_RIGHT_BUFFER_BIT",
    "GLX_DEPTH_BUFFER_BIT",
    "GLX_STENCIL_BUFFER_BIT",
    "GLX_ACCUM_BUFFER_BIT",
])

GLXbuffer = Flags(Int, [
    "GLX_RGBA_BIT",
    "GLX_COLOR_INDEX_BIT",
    "GLX_PBUFFER_CLOBBER_MASK",
])

UnusedAttribs = AttribArray(Const(GLXEnum), [])

GLXCommonSizeAttribs = [
    ('GLX_RED_SIZE', UInt),
    ('GLX_GREEN_SIZE', UInt),
    ('GLX_BLUE_SIZE', UInt),
    ('GLX_ALPHA_SIZE', UInt),
    ('GLX_DEPTH_SIZE', UInt),
    ('GLX_STENCIL_SIZE', UInt),
    ('GLX_ACCUM_RED_SIZE', UInt),
    ('GLX_ACCUM_GREEN_SIZE', UInt),
    ('GLX_ACCUM_BLUE_SIZE', UInt),
    ('GLX_ACCUM_ALPHA_SIZE', UInt)
]

GLXVisualAttribs = AttribArray(GLXEnum, GLXCommonSizeAttribs + [
    ('GLX_USE_GL', None),
    ('GLX_BUFFER_SIZE', UInt),
    ('GLX_LEVEL', Int),
    ('GLX_RGBA', None),
    ('GLX_DOUBLEBUFFER', None),
    ('GLX_STEREO', None),
    ('GLX_AUX_BUFFERS', UInt),
    ('GLX_SAMPLE_BUFFERS', UInt),
    ('GLX_SAMPLES', UInt)],
)

GLXFBConfigCommonAttribs = GLXCommonSizeAttribs + [
    ('GLX_BUFFER_SIZE', UInt),
    ('GLX_LEVEL', Int),
    ('GLX_DOUBLEBUFFER', Bool),
    ('GLX_STEREO', Bool),
    ('GLX_AUX_BUFFERS', UInt),
    ('GLX_SAMPLE_BUFFERS', UInt),
    ('GLX_SAMPLES', UInt),
    ('GLX_RENDER_TYPE', Flags(Int, ["GLX_RGBA_BIT", "GLX_COLOR_INDEX_BIT"])),
    ('GLX_DRAWABLE_TYPE', Flags(Int, ["GLX_WINDOW_BIT", "GLX_PIXMAP_BIT", "GLX_PBUFFER_BIT"])),
    ('GLX_X_RENDERABLE', Bool),
    ('GLX_X_VISUAL_TYPE', FakeEnum(Int, ["GLX_TRUE_COLOR", "GLX_DIRECT_COLOR", "GLX_PSEUDO_COLOR", "GLX_STATIC_COLOR"])),
    ('GLX_CONFIG_CAVEAT', FakeEnum(Int, ["GLX_NONE", "GLX_SLOW_CONFIG", "GLX_NON_CONFORMANT_CONFIG"])),
    ('GLX_TRANSPARENT_TYPE', FakeEnum(Int, ["GLX_NONE", "GLX_TRANSPARENT_RGB", "GLX_TRANSPARENT_INDEX"])),
    ('GLX_TRANSPARENT_INDEX_VALUE', Int),
    ('GLX_TRANSPARENT_RED_VALUE', Int),
    ('GLX_TRANSPARENT_GREEN_VALUE', Int),
    ('GLX_TRANSPARENT_BLUE_VALUE', Int),
    ('GLX_TRANSPARENT_ALPHA_VALUE', Int)
]

GLXFBConfigGLXAttribs = GLXFBConfigCommonAttribs + [
    ('GLX_FBCONFIG_ID', Int), # an XID, can we do better than int?
    ('GLX_MAX_PBUFFER_WIDTH', Int),
    ('GLX_MAX_PBUFFER_HEIGHT', Int),
    ('GLX_MAX_PBUFFER_PIXELS', Int),
    ('GLX_VISUAL_ID', Int)  # another XID
]

GLXFBConfigAttribs = AttribArray(Const(GLXEnum), GLXFBConfigGLXAttribs)
GLXFBConfigSGIXAttribs = AttribArray(GLXEnum, GLXFBConfigCommonAttribs)

GLXContextARBAttribs = AttribArray(Const(GLXEnum), [
    ('GLX_RENDER_TYPE', FakeEnum(Int, ["GLX_RGBA_TYPE", "GLX_COLOR_INDEX_TYPE"])),
    ('GLX_CONTEXT_MAJOR_VERSION_ARB', Int),
    ('GLX_CONTEXT_MINOR_VERSION_ARB', Int),
    ('GLX_CONTEXT_FLAGS_ARB', Flags(Int, [
        "GLX_CONTEXT_DEBUG_BIT_ARB",
        "GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB",
        "GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB",
    ])),
    ('GLX_CONTEXT_PROFILE_MASK_ARB', Flags(Int, [
        "GLX_CONTEXT_CORE_PROFILE_BIT_ARB",
        "GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB",
        "GLX_CONTEXT_ES_PROFILE_BIT_EXT",
    ])),
    ('GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB', GLXEnum),
])

GLXPbufferAttribs = AttribArray(Const(GLXEnum), [
    ('GLX_PBUFFER_WIDTH', Int),
    ('GLX_PBUFFER_HEIGHT', Int),
    ('GLX_LARGEST_PBUFFER', Bool),
    ('GLX_PRESERVED_CONTENTS', Bool)
])

GLXPbufferSGIXAttribs = AttribArray(GLXEnum, [
    ('GLX_PRESERVED_CONTENTS_SGIX', Bool),
    ('GLX_LARGEST_PBUFFER', Bool),
    #('GLX_DIGITAL_MEDIA_PBUFFER_SGIX', Bool),
])

glxapi = Module("GLX")

PROC = Opaque("__GLXextFuncPtr")

glxapi.addFunctions([
    # GLX_VERSION_1_0
    Function(Pointer(XVisualInfo), "glXChooseVisual", [(Display, "dpy"), (Int, "screen"), (GLXVisualAttribs, "attribList")]),
    Function(GLXContext, "glXCreateContext", [(Display, "dpy"), (Pointer(XVisualInfo), "vis"), (GLXContext, "shareList"), (Bool, "direct")]),
    Function(Void, "glXDestroyContext",  [(Display, "dpy"), (GLXContext, "ctx")]),
    Function(Bool, "glXMakeCurrent", [(Display, "dpy"), (GLXDrawable, "drawable"), (GLXContext, "ctx")]),
    Function(Void, "glXCopyContext", [(Display, "dpy"), (GLXContext, "src"), (GLXContext, "dst"),
                                      (ULong, "mask")]),
    Function(Void, "glXSwapBuffers", [(Display, "dpy"), (GLXDrawable, "drawable")]),
    Function(GLXPixmap, "glXCreateGLXPixmap", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"),
                                               (Pixmap, "pixmap")]),
    Function(Void, "glXDestroyGLXPixmap", [(Display, "dpy"), (GLXPixmap, "pixmap")]),
    Function(Bool, "glXQueryExtension", [(Display, "dpy"), Out(Pointer(Int), "errorb"), Out(Pointer(Int), "event")]),
    Function(Bool, "glXQueryVersion", [(Display, "dpy"), Out(Pointer(Int), "maj"), Out(Pointer(Int), "min")]),
    Function(Bool, "glXIsDirect", [(Display, "dpy"), (GLXContext, "ctx")]),
    Function(GLXError, "glXGetConfig", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"),
                                    (GLXEnum, "attrib"), Out(Pointer(Int), "value")]),
    Function(GLXContext, "glXGetCurrentContext", [], sideeffects=False),
    Function(GLXDrawable, "glXGetCurrentDrawable", [], sideeffects=False),
    Function(Void, "glXWaitGL", []),
    Function(Void, "glXWaitX", []),
    Function(Void, "glXUseXFont", [(Font, "font"), (Int, "first"), (Int, "count"), (Int, "list")]),

    # GLX_VERSION_1_1
    Function((ConstCString), "glXQueryExtensionsString", [(Display, "dpy"), (Int, "screen")], sideeffects=False),
    Function((ConstCString), "glXQueryServerString",  [(Display, "dpy"), (Int, "screen"), (GLXname, "name")], sideeffects=False),
    Function((ConstCString), "glXGetClientString", [(Display, "dpy"), (GLXname, "name")], sideeffects=False),

    # GLX_VERSION_1_2
    Function(Display, "glXGetCurrentDisplay", [], sideeffects=False),

    # GLX_VERSION_1_3
    Function(Array(GLXFBConfig, "*nitems"), "glXChooseFBConfig", [(Display, "dpy"), (Int, "screen"), (GLXFBConfigAttribs, "attribList"), Out(Pointer(Int), "nitems")]),
    Function(Int, "glXGetFBConfigAttrib", [(Display, "dpy"), (GLXFBConfig, "config"), (GLXEnum, "attribute"), Out(Pointer(Int), "value")]),
    Function(Array(GLXFBConfig, "*nelements"), "glXGetFBConfigs", [(Display, "dpy"), (Int, "screen"),
                                                                   Out(Pointer(Int), "nelements")]),
    Function(Pointer(XVisualInfo), "glXGetVisualFromFBConfig", [(Display, "dpy"),
                                                                (GLXFBConfig, "config")]),
    Function(GLXWindow, "glXCreateWindow", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Window, "win"), (UnusedAttribs, "attribList")]),
    Function(Void, "glXDestroyWindow", [(Display, "dpy"), (GLXWindow, "window")]),
    Function(GLXPixmap, "glXCreatePixmap", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Pixmap, "pixmap"), (UnusedAttribs, "attribList")]),
    Function(Void, "glXDestroyPixmap", [(Display, "dpy"), (GLXPixmap, "pixmap")]),
    Function(GLXPbuffer, "glXCreatePbuffer", [(Display, "dpy"), (GLXFBConfig, "config"),
                                              (GLXPbufferAttribs, "attribList")]),
    Function(Void, "glXDestroyPbuffer", [(Display, "dpy"), (GLXPbuffer, "pbuf")]),
    Function(Void, "glXQueryDrawable", [(Display, "dpy"), (GLXDrawable, "draw"), (GLXEnum, "attribute"),
                                        Out(Pointer(UInt), "value")]),
    Function(GLXContext, "glXCreateNewContext", [(Display, "dpy"), (GLXFBConfig, "config"),
                                                 (GLXEnum, "renderType"), (GLXContext, "shareList"),
                                                 (Bool, "direct")]),
    Function(Bool, "glXMakeContextCurrent", [(Display, "dpy"), (GLXDrawable, "draw"),
                                             (GLXDrawable, "read"), (GLXContext, "ctx")]),
    Function(GLXDrawable, "glXGetCurrentReadDrawable", []),
    Function(Int, "glXQueryContext", [(Display, "dpy"), (GLXContext, "ctx"), (GLXEnum, "attribute"),
                                      Out(Pointer(Int), "value")]),
    Function(Void, "glXSelectEvent", [(Display, "dpy"), (GLXDrawable, "drawable"),
                                      (ULong, "mask")]),
    Function(Void, "glXGetSelectedEvent", [(Display, "dpy"), (GLXDrawable, "drawable"),
                                           Out(Pointer(ULong), "mask")]),

    # GLX_ARB_create_context
    Function(GLXContext, "glXCreateContextAttribsARB", [(Display, "dpy"), (GLXFBConfig, "config"),
                                                        (GLXContext, "share_context"), (Bool, "direct"),
                                                        (GLXContextARBAttribs,  "attrib_list")]),

    # GLX_EXT_import_context
    Function(Display, "glXGetCurrentDisplayEXT", []),
    Function(Int, "glXQueryContextInfoEXT", [(Display, "dpy"), (GLXContext, "context"), (Int, "attribute"), (OpaquePointer(Int), "value")]),
    Function(GLXContextID, "glXGetContextIDEXT", [(Const(GLXContext), "context")]),
    Function(GLXContext, "glXImportContextEXT", [(Display, "dpy"), (GLXContextID, "contextID")]),
    Function(Void, "glXFreeContextEXT", [(Display, "dpy"), (GLXContext, "context")]),

    # GLX_EXT_swap_control
    Function(Void, "glXSwapIntervalEXT", [(Display, "dpy"), (GLXDrawable, "drawable"), (Int, "interval")]),

    # GLX_EXT_texture_from_pixmap
    Function(Void, "glXBindTexImageEXT", [(Display, "display"), (GLXDrawable, "drawable"), (GLXEnum, "buffer"), (GLXFBConfigAttribs, "attrib_list")]),
    Function(Void, "glXReleaseTexImageEXT", [(Display, "display"), (GLXDrawable, "drawable"), (GLXEnum, "buffer")]),

    # GLX_MESA_agp_offset
    Function(UInt, "glXGetAGPOffsetMESA", [(OpaquePointer(Const(Void)), "pointer")]),

    # GLX_MESA_copy_sub_buffer
    Function(Void, "glXCopySubBufferMESA", [(Display, "dpy"), (GLXDrawable, "drawable"), (Int, "x"), (Int, "y"), (Int, "width"), (Int, "height")]),

    # GLX_MESA_pixmap_colormap
    Function(GLXPixmap, "glXCreateGLXPixmapMESA", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"), (Pixmap, "pixmap"), (Colormap, "cmap")]),

    # GLX_MESA_release_buffers
    Function(Bool, "glXReleaseBuffersMESA", [(Display, "dpy"), (GLXDrawable, "drawable")]),

    # GLX_MESA_set_3dfx_mode
    Function(Bool, "glXSet3DfxModeMESA", [(Int, "mode")]),

    # GLX_MESA_swap_control
    Function(Int, "glXSwapIntervalMESA", [(UInt, "interval")]),
    Function(Int, "glXGetSwapIntervalMESA", [], sideeffects=False),

    # GLX_NV_copy_image
    Function(Void, "glXCopyImageSubDataNV", [(Display, "dpy"), (GLXContext, "srcCtx"), (GLuint, "srcName"), (GLenum, "srcTarget"), (GLint, "srcLevel"), (GLint, "srcX"), (GLint, "srcY"), (GLint, "srcZ"), (GLXContext, "dstCtx"), (GLuint, "dstName"), (GLenum, "dstTarget"), (GLint, "dstLevel"), (GLint, "dstX"), (GLint, "dstY"), (GLint, "dstZ"), (GLsizei, "width"), (GLsizei, "height"), (GLsizei, "depth")]),

    # GLX_NV_present_video
    Function(Array(UInt, "(nelements ? *nelements : 0)"), "glXEnumerateVideoDevicesNV", [(Display, "dpy"), (Int, "screen"), Out(Pointer(Int), "nelements")]),
    Function(Int, "glXBindVideoDeviceNV", [(Display, "dpy"), (UInt, "video_slot"), (UInt, "video_device"), (UnusedAttribs, "attrib_list")]),

    # GLX_NV_swap_group
    Function(Bool, "glXJoinSwapGroupNV", [(Display, "dpy"), (GLXDrawable, "drawable"), (GLuint, "group")]),
    Function(Bool, "glXBindSwapBarrierNV", [(Display, "dpy"), (GLuint, "group"), (GLuint, "barrier")]),
    Function(Bool, "glXQuerySwapGroupNV", [(Display, "dpy"), (GLXDrawable, "drawable"), (OpaquePointer(GLuint), "group"), (OpaquePointer(GLuint), "barrier")]),
    Function(Bool, "glXQueryMaxSwapGroupsNV", [(Display, "dpy"), (Int, "screen"), (OpaquePointer(GLuint), "maxGroups"), (OpaquePointer(GLuint), "maxBarriers")]),
    Function(Bool, "glXQueryFrameCountNV", [(Display, "dpy"), (Int, "screen"), (OpaquePointer(GLuint), "count")]),
    Function(Bool, "glXResetFrameCountNV", [(Display, "dpy"), (Int, "screen")]),

    # GLX_NV_vertex_array_range
    Function(OpaquePointer(Void), "glXAllocateMemoryNV", [(GLsizei, "size"), (GLfloat, "readfreq"), (GLfloat, "writefreq"), (GLfloat, "priority")]),
    Function(Void, "glXFreeMemoryNV", [(OpaquePointer(Void), "pointer")]),

    # GLX_NV_video_capture
    Function(Int, "glXBindVideoCaptureDeviceNV", [(Display, "dpy"), (UInt, "video_capture_slot"), (GLXVideoCaptureDeviceNV, "device")]),
    Function(Array(GLXVideoCaptureDeviceNV, "(nelements ? *nelements : 0)"), "glXEnumerateVideoCaptureDevicesNV", [(Display, "dpy"), (Int, "screen"), Out(Pointer(Int), "nelements")]),
    Function(Void, "glXLockVideoCaptureDeviceNV", [(Display, "dpy"), (GLXVideoCaptureDeviceNV, "device")]),
    Function(Int, "glXQueryVideoCaptureDeviceNV", [(Display, "dpy"), (GLXVideoCaptureDeviceNV, "device"), (Int, "attribute"), Out(Pointer(Int), "value")], sideeffects=False),
    Function(Void, "glXReleaseVideoCaptureDeviceNV", [(Display, "dpy"), (GLXVideoCaptureDeviceNV, "device")]),

    # GLX_NV_video_output
    Function(Int, "glXGetVideoDeviceNV", [(Display, "dpy"), (Int, "screen"), (Int, "numVideoDevices"), Out(Array(GLXVideoDeviceNV, "numVideoDevices"), "pVideoDevice")]),
    Function(Int, "glXReleaseVideoDeviceNV", [(Display, "dpy"), (Int, "screen"), (GLXVideoDeviceNV, "VideoDevice")]),
    Function(Int, "glXBindVideoImageNV", [(Display, "dpy"), (GLXVideoDeviceNV, "VideoDevice"), (GLXPbuffer, "pbuf"), (Int, "iVideoBuffer")]),
    Function(Int, "glXReleaseVideoImageNV", [(Display, "dpy"), (GLXPbuffer, "pbuf")]),
    Function(Int, "glXSendPbufferToVideoNV", [(Display, "dpy"), (GLXPbuffer, "pbuf"), (Int, "iBufferType"), Out(Pointer(ULong), "pulCounterPbuffer"), (GLboolean, "bBlock")]),
    Function(Int, "glXGetVideoInfoNV", [(Display, "dpy"), (Int, "screen"), (GLXVideoDeviceNV, "VideoDevice"), Out(Pointer(ULong), "pulCounterOutputPbuffer"), Out(Pointer(ULong), "pulCounterOutputVideo")], sideeffects=False),

    # GLX_OML_sync_control
    Function(Bool, "glXGetSyncValuesOML", [(Display, "dpy"), (GLXDrawable, "drawable"), (OpaquePointer(Int64), "ust"), (OpaquePointer(Int64), "msc"), (OpaquePointer(Int64), "sbc")]),
    Function(Bool, "glXGetMscRateOML", [(Display, "dpy"), (GLXDrawable, "drawable"), (OpaquePointer(Int32), "numerator"), (OpaquePointer(Int32), "denominator")]),
    Function(Int64, "glXSwapBuffersMscOML", [(Display, "dpy"), (GLXDrawable, "drawable"), (Int64, "target_msc"), (Int64, "divisor"), (Int64, "remainder")]),
    Function(Bool, "glXWaitForMscOML", [(Display, "dpy"), (GLXDrawable, "drawable"), (Int64, "target_msc"), (Int64, "divisor"), (Int64, "remainder"), (OpaquePointer(Int64), "ust"), (OpaquePointer(Int64), "msc"), (OpaquePointer(Int64), "sbc")]),
    Function(Bool, "glXWaitForSbcOML", [(Display, "dpy"), (GLXDrawable, "drawable"), (Int64, "target_sbc"), (OpaquePointer(Int64), "ust"), (OpaquePointer(Int64), "msc"), (OpaquePointer(Int64), "sbc")]),

    # GLX_SGI_cushion
    Function(Void, "glXCushionSGI", [(Display, "dpy"), (Window, "window"), (Float, "cushion")]),

    # GLX_SGI_make_current_read
    Function(Bool, "glXMakeCurrentReadSGI", [(Display, "dpy"), (GLXDrawable, "draw"), (GLXDrawable, "read"), (GLXContext, "ctx")]),
    Function(GLXDrawable, "glXGetCurrentReadDrawableSGI", []),

    # GLX_SGI_swap_control
    Function(Int, "glXSwapIntervalSGI", [(Int, "interval")]),

    # GLX_SGI_video_sync
    Function(Int, "glXGetVideoSyncSGI", [(OpaquePointer(UInt), "count")]),
    Function(Int, "glXWaitVideoSyncSGI", [(Int, "divisor"), (Int, "remainder"), (OpaquePointer(UInt), "count")]),

    # GLX_SGIX_dmbuffer
    #Function(Bool, "glXAssociateDMPbufferSGIX", [(Display, "dpy"), (GLXPbufferSGIX, "pbuffer"), (OpaquePointer(DMparams), "params"), (DMbuffer, "dmbuffer")]),

    # GLX_SGIX_fbconfig
    Function(Int, "glXGetFBConfigAttribSGIX", [(Display, "dpy"), (GLXFBConfigSGIX, "config"), (GLXEnum, "attribute"), Out(Pointer(Int), "value")]),
    Function(Array(GLXFBConfigSGIX, "*nelements"), "glXChooseFBConfigSGIX", [(Display, "dpy"), (Int, "screen"), (GLXFBConfigSGIXAttribs, "attrib_list"), Out(Pointer(Int), "nelements")]),
    Function(GLXPixmap, "glXCreateGLXPixmapWithConfigSGIX", [(Display, "dpy"), (GLXFBConfigSGIX, "config"), (Pixmap, "pixmap")]),
    Function(GLXContext, "glXCreateContextWithConfigSGIX", [(Display, "dpy"), (GLXFBConfigSGIX, "config"), (GLXEnum, "render_type"), (GLXContext, "share_list"), (Bool, "direct")]),
    Function(Pointer(XVisualInfo), "glXGetVisualFromFBConfigSGIX", [(Display, "dpy"), (GLXFBConfigSGIX, "config")]),
    Function(GLXFBConfigSGIX, "glXGetFBConfigFromVisualSGIX", [(Display, "dpy"), (Pointer(XVisualInfo), "vis")]),

    # GLX_SGIX_hyperpipe
    #Function(OpaquePointer(GLXHyperpipeNetworkSGIX), "glXQueryHyperpipeNetworkSGIX", [(Display, "dpy"), (OpaquePointer(Int), "npipes")]),
    #Function(Int, "glXHyperpipeConfigSGIX", [(Display, "dpy"), (Int, "networkId"), (Int, "npipes"), (OpaquePointer(GLXHyperpipeConfigSGIX), "cfg"), (OpaquePointer(Int), "hpId")]),
    #Function(OpaquePointer(GLXHyperpipeConfigSGIX), "glXQueryHyperpipeConfigSGIX", [(Display, "dpy"), (Int, "hpId"), (OpaquePointer(Int), "npipes")]),
    #Function(Int, "glXDestroyHyperpipeConfigSGIX", [(Display, "dpy"), (Int, "hpId")]),
    #Function(Int, "glXBindHyperpipeSGIX", [(Display, "dpy"), (Int, "hpId")]),
    #Function(Int, "glXQueryHyperpipeBestAttribSGIX", [(Display, "dpy"), (Int, "timeSlice"), (Int, "attrib"), (Int, "size"), (OpaquePointer(Void), "attribList"), (OpaquePointer(Void), "returnAttribList")]),
    #Function(Int, "glXHyperpipeAttribSGIX", [(Display, "dpy"), (Int, "timeSlice"), (Int, "attrib"), (Int, "size"), (OpaquePointer(Void), "attribList")]),
    #Function(Int, "glXQueryHyperpipeAttribSGIX", [(Display, "dpy"), (Int, "timeSlice"), (Int, "attrib"), (Int, "size"), (OpaquePointer(Void), "returnAttribList")]),

    # GLX_SGIX_pbuffer
    Function(GLXPbufferSGIX, "glXCreateGLXPbufferSGIX", [(Display, "dpy"), (GLXFBConfigSGIX, "config"), (UInt, "width"), (UInt, "height"), (GLXPbufferSGIXAttribs, "attrib_list")]),
    Function(Void, "glXDestroyGLXPbufferSGIX", [(Display, "dpy"), (GLXPbufferSGIX, "pbuf")]),
    Function(Int, "glXQueryGLXPbufferSGIX", [(Display, "dpy"), (GLXPbufferSGIX, "pbuf"), (Int, "attribute"), Out(Pointer(UInt), "value")]),
    Function(Void, "glXSelectEventSGIX", [(Display, "dpy"), (GLXDrawable, "drawable"), (ULong, "mask")]),
    Function(Void, "glXGetSelectedEventSGIX", [(Display, "dpy"), (GLXDrawable, "drawable"), Out(Pointer(ULong), "mask")]),

    # GLX_SGIX_swap_barrier
    Function(Void, "glXBindSwapBarrierSGIX", [(Display, "dpy"), (GLXDrawable, "drawable"), (Int, "barrier")]),
    Function(Bool, "glXQueryMaxSwapBarriersSGIX", [(Display, "dpy"), (Int, "screen"), (OpaquePointer(Int), "max")]),

    # GLX_SGIX_swap_group
    Function(Void, "glXJoinSwapGroupSGIX", [(Display, "dpy"), (GLXDrawable, "drawable"), (GLXDrawable, "member")]),

    # GLX_SGIX_video_resize
    Function(Int, "glXBindChannelToWindowSGIX", [(Display, "display"), (Int, "screen"), (Int, "channel"), (Window, "window")]),
    Function(Int, "glXChannelRectSGIX", [(Display, "display"), (Int, "screen"), (Int, "channel"), (Int, "x"), (Int, "y"), (Int, "w"), (Int, "h")]),
    Function(Int, "glXQueryChannelRectSGIX", [(Display, "display"), (Int, "screen"), (Int, "channel"), (OpaquePointer(Int), "dx"), (OpaquePointer(Int), "dy"), (OpaquePointer(Int), "dw"), (OpaquePointer(Int), "dh")]),
    Function(Int, "glXQueryChannelDeltasSGIX", [(Display, "display"), (Int, "screen"), (Int, "channel"), (OpaquePointer(Int), "x"), (OpaquePointer(Int), "y"), (OpaquePointer(Int), "w"), (OpaquePointer(Int), "h")]),
    Function(Int, "glXChannelRectSyncSGIX", [(Display, "display"), (Int, "screen"), (Int, "channel"), (GLenum, "synctype")]),

    # GLX_SGIX_video_source
    #Function(GLXVideoSourceSGIX, "glXCreateGLXVideoSourceSGIX", [(Display, "display"), (Int, "screen"), (VLServer, "server"), (VLPath, "path"), (Int, "nodeClass"), (VLNode, "drainNode")]),
    #Function(Void, "glXDestroyGLXVideoSourceSGIX", [(Display, "dpy"), (GLXVideoSourceSGIX, "glxvideosource")]),

    # GLX_SUN_get_transparent_index
    #Function(Status, "glXGetTransparentIndexSUN", [(Display, "dpy"), (Window, "overlay"), (Window, "underlay"), (OpaquePointer(Long), "pTransparentIndex")]),

    # Must be last
    Function(PROC, "glXGetProcAddressARB", [(String(Const(GLubyte)), "procName")]),
    Function(PROC, "glXGetProcAddress", [(String(Const(GLubyte)), "procName")]),
])


# To prevent collision with stdapi.Bool
del Bool

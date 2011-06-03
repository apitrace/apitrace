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
from glapi import glapi

VisualID = Alias("VisualID", UInt32)
Display = Opaque("Display *")
Visual = Opaque("Visual *")
Font = Alias("Font", UInt32)
Pixmap = Alias("Pixmap", UInt32)
Window = Alias("Window", UInt32)

GLXContext = Opaque("GLXContext")
GLXPixmap = Alias("GLXPixmap", UInt32)
GLXDrawable = Alias("GLXDrawable", UInt32)
GLXFBConfig = Opaque("GLXFBConfig")
GLXFBConfigID = Alias("GLXFBConfigID", UInt32)
GLXContextID = Alias("GLXContextID", UInt32)
GLXWindow = Alias("GLXWindow", UInt32)
GLXPbuffer = Alias("GLXPbuffer", UInt32)

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

Bool_ = FakeEnum(Int, [
    "False",
    "True",
])

GLXAttrib = FakeEnum(Int, [
    "GLX_USE_GL",
    "GLX_BUFFER_SIZE",
    "GLX_LEVEL",
    "GLX_RGBA",
    "GLX_DOUBLEBUFFER",
    "GLX_STEREO",
    "GLX_AUX_BUFFERS",
    "GLX_RED_SIZE",
    "GLX_GREEN_SIZE",
    "GLX_BLUE_SIZE",
    "GLX_ALPHA_SIZE",
    "GLX_DEPTH_SIZE",
    "GLX_STENCIL_SIZE",
    "GLX_ACCUM_RED_SIZE",
    "GLX_ACCUM_GREEN_SIZE",
    "GLX_ACCUM_BLUE_SIZE",
    "GLX_ACCUM_ALPHA_SIZE",

    "GLX_CONFIG_CAVEAT",		# 0x20
    "GLX_X_VISUAL_TYPE",		# 0x22
    "GLX_TRANSPARENT_TYPE",		# 0x23
    "GLX_TRANSPARENT_INDEX_VALUE",	# 0x24
    "GLX_TRANSPARENT_RED_VALUE",	# 0x25
    "GLX_TRANSPARENT_GREEN_VALUE",	# 0x26
    "GLX_TRANSPARENT_BLUE_VALUE",	# 0x27
    "GLX_TRANSPARENT_ALPHA_VALUE",	# 0x28

    "GLX_BIND_TO_TEXTURE_RGB_EXT",        # 0x20D0
    "GLX_BIND_TO_TEXTURE_RGBA_EXT",       # 0x20D1
    "GLX_BIND_TO_MIPMAP_TEXTURE_EXT",     # 0x20D2
    "GLX_BIND_TO_TEXTURE_TARGETS_EXT",    # 0x20D3
    "GLX_Y_INVERTED_EXT",                 # 0x20D4
    "GLX_TEXTURE_FORMAT_EXT",             # 0x20D5
    "GLX_TEXTURE_TARGET_EXT",             # 0x20D6
    "GLX_MIPMAP_TEXTURE_EXT",             # 0x20D7
    "GLX_TEXTURE_FORMAT_NONE_EXT",        # 0x20D8
    "GLX_TEXTURE_FORMAT_RGB_EXT",         # 0x20D9
    "GLX_TEXTURE_FORMAT_RGBA_EXT",        # 0x20DA
    "GLX_TEXTURE_1D_EXT",                 # 0x20DB
    "GLX_TEXTURE_2D_EXT",                 # 0x20DC
    "GLX_TEXTURE_RECTANGLE_EXT",          # 0x20DD
    "GLX_FRONT_LEFT_EXT",                 # 0x20DE
    "GLX_FRONT_RIGHT_EXT",                # 0x20DF
    "GLX_BACK_LEFT_EXT",                  # 0x20E0
    "GLX_BACK_RIGHT_EXT",                 # 0x20E1
    "GLX_AUX0_EXT",                       # 0x20E2
    "GLX_AUX1_EXT",                       # 0x20E3
    "GLX_AUX2_EXT",                       # 0x20E4
    "GLX_AUX3_EXT",                       # 0x20E5
    "GLX_AUX4_EXT",                       # 0x20E6
    "GLX_AUX5_EXT",                       # 0x20E7
    "GLX_AUX6_EXT",                       # 0x20E8
    "GLX_AUX7_EXT",                       # 0x20E9
    "GLX_AUX8_EXT",                       # 0x20EA
    "GLX_AUX9_EXT",                       # 0x20EB

    "GLX_NONE",			# 0x8000
    "GLX_SLOW_CONFIG",			# 0x8001
    "GLX_TRUE_COLOR",			# 0x8002
    "GLX_DIRECT_COLOR",		# 0x8003
    "GLX_PSEUDO_COLOR",		# 0x8004
    "GLX_STATIC_COLOR",		# 0x8005
    "GLX_GRAY_SCALE",			# 0x8006
    "GLX_STATIC_GRAY",			# 0x8007
    "GLX_TRANSPARENT_RGB",		# 0x8008
    "GLX_TRANSPARENT_INDEX",		# 0x8009
    "GLX_VISUAL_ID",			# 0x800B
    "GLX_SCREEN",			# 0x800C
    "GLX_NON_CONFORMANT_CONFIG",	# 0x800D
    "GLX_DRAWABLE_TYPE",		# 0x8010
    "GLX_RENDER_TYPE",			# 0x8011
    "GLX_X_RENDERABLE",		# 0x8012
    "GLX_FBCONFIG_ID",			# 0x8013
    "GLX_RGBA_TYPE",			# 0x8014
    "GLX_COLOR_INDEX_TYPE",		# 0x8015
    "GLX_MAX_PBUFFER_WIDTH",		# 0x8016
    "GLX_MAX_PBUFFER_HEIGHT",		# 0x8017
    "GLX_MAX_PBUFFER_PIXELS",		# 0x8018
    "GLX_PRESERVED_CONTENTS",		# 0x801B
    "GLX_LARGEST_PBUFFER",		# 0x801C
    "GLX_WIDTH",			# 0x801D
    "GLX_HEIGHT",			# 0x801E
    "GLX_EVENT_MASK",			# 0x801F
    "GLX_DAMAGED",			# 0x8020
    "GLX_SAVED",			# 0x8021
    "GLX_WINDOW",			# 0x8022
    "GLX_PBUFFER",			# 0x8023
    "GLX_PBUFFER_HEIGHT",              # 0x8040
    "GLX_PBUFFER_WIDTH",               # 0x8041

    "GLX_SAMPLE_BUFFERS",              # 100000
    "GLX_SAMPLES",                     # 100001

    "GLX_DONT_CARE", # 0xFFFFFFFF
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

GLXEnum = FakeEnum(Int, [
    "GLX_NONE",
    "GLX_SLOW_CONFIG",
    "GLX_TRUE_COLOR",
    "GLX_DIRECT_COLOR",
    "GLX_PSEUDO_COLOR",
    "GLX_STATIC_COLOR",
    "GLX_GRAY_SCALE",
    "GLX_STATIC_GRAY",
    "GLX_TRANSPARENT_RGB",
    "GLX_TRANSPARENT_INDEX",
    "GLX_VISUAL_ID",
    "GLX_SCREEN",
    "GLX_NON_CONFORMANT_CONFIG",
    "GLX_DRAWABLE_TYPE",
    "GLX_RENDER_TYPE",
    "GLX_X_RENDERABLE",
    "GLX_FBCONFIG_ID",
    "GLX_RGBA_TYPE",
    "GLX_COLOR_INDEX_TYPE",
    "GLX_MAX_PBUFFER_WIDTH",
    "GLX_MAX_PBUFFER_HEIGHT",
    "GLX_MAX_PBUFFER_PIXELS",
    "GLX_PRESERVED_CONTENTS",
    "GLX_LARGEST_PBUFFER",
    "GLX_WIDTH",
    "GLX_HEIGHT",
    "GLX_EVENT_MASK",
    "GLX_DAMAGED",
    "GLX_SAVED",
    "GLX_WINDOW",
    "GLX_PBUFFER",
    "GLX_PBUFFER_HEIGHT",
    "GLX_PBUFFER_WIDTH",
])

GLXbuffer = Flags(Int, [
    "GLX_RGBA_BIT",
    "GLX_COLOR_INDEX_BIT",
    "GLX_PBUFFER_CLOBBER_MASK",
])

glxapi = API("GLX")

PROC = Opaque("__GLXextFuncPtr")

glxapi.add_functions([
    # GLX
    Function(Pointer(XVisualInfo), "glXChooseVisual", [(Display, "dpy"), (Int, "screen"), (Array(GLXAttrib, "__AttribList_size(attribList)"), "attribList")]),
    Function(GLXContext, "glXCreateContext", [(Display, "dpy"), (Pointer(XVisualInfo), "vis"), (GLXContext, "shareList"), (Bool_, "direct")]),
    Function(Void, "glXDestroyContext",  [(Display, "dpy"), (GLXContext, "ctx")]),
    Function(Bool_, "glXMakeCurrent", [(Display, "dpy"), (GLXDrawable, "drawable"), (GLXContext, "ctx")]),
    Function(Void, "glXCopyContext", [(Display, "dpy"), (GLXContext, "src"), (GLXContext, "dst"),
                                      (ULong, "mask")]),
    Function(Void, "glXSwapBuffers", [(Display, "dpy"), (GLXDrawable, "drawable")]),
    Function(GLXPixmap, "glXCreateGLXPixmap", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"),
                                               (Pixmap, "pixmap")]),
    Function(Void, "glXDestroyGLXPixmap", [(Display, "dpy"), (GLXPixmap, "pixmap")]),
    Function(Bool_, "glXQueryExtension", [(Display, "dpy"), Out(Pointer(Int), "errorb"), Out(Pointer(Int), "event")]),
    Function(Bool_, "glXQueryVersion", [(Display, "dpy"), Out(Pointer(Int), "maj"), Out(Pointer(Int), "min")]),
    Function(Bool_, "glXIsDirect", [(Display, "dpy"), (GLXContext, "ctx")]),
    Function(GLXError, "glXGetConfig", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"),
                                    (GLXAttrib, "attrib"), Out(Pointer(Int), "value")]),
    Function(GLXContext, "glXGetCurrentContext", [], sideeffects=False),
    Function(GLXDrawable, "glXGetCurrentDrawable", [], sideeffects=False),
    Function(Void, "glXWaitGL", []),
    Function(Void, "glXWaitX", []),
    Function(Void, "glXUseXFont", [(Font, "font"), (Int, "first"), (Int, "count"), (Int, "list")]),

    # GLX 1.1 and later
    Function((Const(String("char *"))), "glXQueryExtensionsString", [(Display, "dpy"), (Int, "screen")]),
    Function((Const(String("char *"))), "glXQueryServerString",  [(Display, "dpy"), (Int, "screen"), (GLXname, "name")]),
    Function((Const(String("char *"))), "glXGetClientString", [(Display, "dpy"), (GLXname, "name")]),

    # GLX 1.2 and later
    Function(Display, "glXGetCurrentDisplay", [], sideeffects=False),

    # GLX 1.3 and later
    Function(Array(GLXFBConfig, "*nitems"), "glXChooseFBConfig", [(Display, "dpy"), (Int, "screen"), (Const(Array(GLXAttrib, "__AttribList_size(attribList)")), "attribList"), Out(Pointer(Int), "nitems")]),
    Function(Int, "glXGetFBConfigAttrib", [(Display, "dpy"), (GLXFBConfig, "config"), (GLXAttrib, "attribute"), Out(Pointer(Int), "value")]),
    Function(Array(GLXFBConfig, "*nelements"), "glXGetFBConfigs", [(Display, "dpy"), (Int, "screen"), 
                                                                   Out(Pointer(Int), "nelements")]),
    Function(Pointer(XVisualInfo), "glXGetVisualFromFBConfig", [(Display, "dpy"),
                                                                (GLXFBConfig, "config")]),
    Function(GLXWindow, "glXCreateWindow", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Window, "win"), (Const(Array(Int, "__AttribList_size(attribList)")), "attribList")]),
    Function(Void, "glXDestroyWindow", [(Display, "dpy"), (GLXWindow, "window")]),
    Function(GLXPixmap, "glXCreatePixmap", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Pixmap, "pixmap"), (Const(Array(Int, "__AttribList_size(attribList)")), "attribList")]),
    Function(Void, "glXDestroyPixmap", [(Display, "dpy"), (GLXPixmap, "pixmap")]),
    Function(GLXPbuffer, "glXCreatePbuffer", [(Display, "dpy"), (GLXFBConfig, "config"),
                                              (Const(Array(GLXEnum, "__AttribList_size(attribList)")), "attribList")]),
    Function(Void, "glXDestroyPbuffer", [(Display, "dpy"), (GLXPbuffer, "pbuf")]),
    Function(Void, "glXQueryDrawable", [(Display, "dpy"), (GLXDrawable, "draw"), (GLXEnum, "attribute"),
                                        Out(Pointer(UInt), "value")]),
    Function(GLXContext, "glXCreateNewContext", [(Display, "dpy"), (GLXFBConfig, "config"),
                                                 (GLXEnum, "renderType"), (GLXContext, "shareList"),
                                                 (Bool_, "direct")]),
    Function(Bool_, "glXMakeContextCurrent", [(Display, "dpy"), (GLXDrawable, "draw"),
                                             (GLXDrawable, "read"), (GLXContext, "ctx")]),
    Function(GLXDrawable, "glXGetCurrentReadDrawable", []),
    Function(Int, "glXQueryContext", [(Display, "dpy"), (GLXContext, "ctx"), (GLXEnum, "attribute"),
                                      Out(Pointer(Int), "value")]),
    Function(Void, "glXSelectEvent", [(Display, "dpy"), (GLXDrawable, "drawable"),
                                      (ULong, "mask")]),
    Function(Void, "glXGetSelectedEvent", [(Display, "dpy"), (GLXDrawable, "drawable"),
                                           Out(Pointer(ULong), "mask")]),

    # EXT_texture_from_pixmap
    Function(Void, "glXBindTexImageEXT", [(Display, "display"), (GLXDrawable, "drawable"), (Int, "buffer"), (Const(Array(Int, "__AttribList_size(attrib_list)")), "attrib_list")]),
    Function(Void, "glXReleaseTexImageEXT", [(Display, "display"), (GLXDrawable, "drawable"), (Int, "buffer")]),

    # TODO: more extensions

    # Must be last
    Function(PROC, "glXGetProcAddressARB", [(Alias("const GLubyte *", CString), "procName")]),
    Function(PROC, "glXGetProcAddress", [(Alias("const GLubyte *", CString), "procName")]),
])



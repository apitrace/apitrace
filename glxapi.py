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
    "GLX_CONFIG_CAVEAT",
    "GLX_X_VISUAL_TYPE",
    "GLX_TRANSPARENT_TYPE",
    "GLX_TRANSPARENT_INDEX_VALUE",
    "GLX_TRANSPARENT_RED_VALUE",
    "GLX_TRANSPARENT_GREEN_VALUE",
    "GLX_TRANSPARENT_BLUE_VALUE",
    "GLX_TRANSPARENT_ALPHA_VALUE",
    "GLX_DONT_CARE",
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
    Function((Const(String("char *"))), "glXQueryServerString",  [(Display, "dpy"), (Int, "screen"), (Int, "name")]),
    Function((Const(String("char *"))), "glXGetClientString", [(Display, "dpy"), (Int, "name")]),

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
    # TODO: extensions

    # Must be last
    Function(PROC, "glXGetProcAddressARB", [(Alias("const GLubyte *", CString), "procName")]),
    Function(PROC, "glXGetProcAddress", [(Alias("const GLubyte *", CString), "procName")]),
])



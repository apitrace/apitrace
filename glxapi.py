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

Bool_ = Alias("Bool", Int)

glxapi = API("GLX")

PROC = Opaque("__GLXextFuncPtr")

glxapi.add_functions([
    # GLX
    Function(Pointer(XVisualInfo), "glXChooseVisual", [(Display, "dpy"), (Int, "screen"), (Pointer(Int), "attribList")]),
    Function(GLXContext, "glXCreateContext", [(Display, "dpy"), (Pointer(XVisualInfo), "vis"), (GLXContext, "shareList"), (Bool_, "direct")]),
    Function(Void, "glXDestroyContext",  [(Display, "dpy"), (GLXContext, "ctx")]),
    Function(Bool_, "glXMakeCurrent", [(Display, "dpy"), (GLXDrawable, "drawable"), (GLXContext, "ctx")]),
    Function(Void, "glXCopyContext", [(Display, "dpy"), (GLXContext, "src"), (GLXContext, "dst"),
                                      (ULong, "mask")]),
    Function(Void, "glXSwapBuffers", [(Display, "dpy"), (GLXDrawable, "drawable")]),
    Function(GLXPixmap, "glXCreateGLXPixmap", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"),
                                               (Pixmap, "pixmap")]),
    Function(Void, "glXDestroyGLXPixmap", [(Display, "dpy"), (GLXPixmap, "pixmap")]),
    Function(Bool_, "glXQueryExtension", [(Display, "dpy"), (Pointer(Int), "errorb"), (Pointer(Int), "event")]),
    Function(Bool_, "glXQueryVersion", [(Display, "dpy"), (Pointer(Int), "maj"), (Pointer(Int), "min")]),
    Function(Bool_, "glXIsDirect", [(Display, "dpy"), (GLXContext, "ctx")]),
    Function(Int, "glXGetConfig", [(Display, "dpy"), (Pointer(XVisualInfo), "visual"),
                                    (Int, "attrib"), (Pointer(Int), "value")]),
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
    Function(Pointer(GLXFBConfig), "glXChooseFBConfig", [(Display, "dpy"), (Int, "screen"),
                                                         (Pointer(Const(Int)), "attribList"), (Pointer(Int), "nitems")]),
    Function(Int, "glXGetFBConfigAttrib", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Int, "attribute"), (Pointer(Int), "value")]),
    Function(Pointer(GLXFBConfig), "glXGetFBConfigs", [(Display, "dpy"), (Int, "screen"),
                                              (Pointer(Int), "nelements")]),
    Function(Pointer(XVisualInfo), "glXGetVisualFromFBConfig", [(Display, "dpy"),
                                                                (GLXFBConfig, "config")]),
    Function(GLXWindow, "glXCreateWindow", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Window, "win"), (Pointer(Const(Int)), "attribList")]),
    Function(Void, "glXDestroyWindow", [(Display, "dpy"), (GLXWindow, "window")]),
    Function(GLXPixmap, "glXCreatePixmap", [(Display, "dpy"), (GLXFBConfig, "config"),
                                            (Pixmap, "pixmap"), (Pointer(Const(Int)), "attribList")]),
    Function(Void, "glXDestroyPixmap", [(Display, "dpy"), (GLXPixmap, "pixmap")]),
    Function(GLXPbuffer, "glXCreatePbuffer", [(Display, "dpy"), (GLXFBConfig, "config"),
                                              (Pointer(Const(Int)), "attribList")]),
    Function(Void, "glXDestroyPbuffer", [(Display, "dpy"), (GLXPbuffer, "pbuf")]),
    Function(Void, "glXQueryDrawable", [(Display, "dpy"), (GLXDrawable, "draw"), (Int, "attribute"),
                                        (Pointer(UInt), "value")]),
    Function(GLXContext, "glXCreateNewContext", [(Display, "dpy"), (GLXFBConfig, "config"),
                                                 (Int, "renderType"), (GLXContext, "shareList"),
                                                 (Bool_, "direct")]),
    Function(Bool_, "glXMakeContextCurrent", [(Display, "dpy"), (GLXDrawable, "draw"),
                                             (GLXDrawable, "read"), (GLXContext, "ctx")]),
    Function(GLXDrawable, "glXGetCurrentReadDrawable", []),
    Function(Int, "glXQueryContext", [(Display, "dpy"), (GLXContext, "ctx"), (Int, "attribute"),
                                      (Pointer(Int), "value")]),
    Function(Void, "glXSelectEvent", [(Display, "dpy"), (GLXDrawable, "drawable"),
                                      (ULong, "mask")]),
    Function(Void, "glXGetSelectedEvent", [(Display, "dpy"), (GLXDrawable, "drawable"),
                                           (Pointer(ULong), "mask")]),
    # TODO: extensions

    # Must be last
    Function(PROC, "glXGetProcAddressARB", [(Alias("const GLubyte *", CString), "procName")]),
    Function(PROC, "glXGetProcAddress", [(Alias("const GLubyte *", CString), "procName")]),
])



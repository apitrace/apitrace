/**************************************************************************
 *
 * Copyright 2022 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dlfcn.h>

#define GLX_GLXEXT_PROTOTYPES

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>  // for glXGetProcAddressARB


int
main(int argc, char **argv)
{
    void *libGLX = dlopen("libGLX.so.0", RTLD_NOW | RTLD_LOCAL);
    if (libGLX == nullptr) {
        return EXIT_SKIP;
    }

    PFNGLXGETPROCADDRESSPROC pfnglXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)dlsym(libGLX, "glXGetProcAddress");
    assert(pfnglXGetProcAddress != nullptr);
    pfnglXGetProcAddress((const GLubyte *)"glClear");

    void *libOpenGL = dlopen("libOpenGL.so.0", RTLD_NOW | RTLD_LOCAL);
    assert(libOpenGL != nullptr);

    typedef void (APIENTRY *PFNGLCLEARCOLOR)(GLfloat, GLfloat, GLfloat, GLfloat);
    PFNGLCLEARCOLOR pfnClearColor = (PFNGLCLEARCOLOR)dlsym(libOpenGL, "glClearColor");
    assert(pfnClearColor !=0 );

    pfnClearColor(0.25, 0.5, 0.75, 1.0);

    dlclose(libOpenGL);
    dlclose(libGLX);

    return 0;
}

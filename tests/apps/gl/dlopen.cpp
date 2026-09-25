/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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

#ifdef _WIN32
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

#include <GLFW/glfw3.h>


static void
Error(int error, const char *description)
{
    fprintf(stderr, "glfw: error: %s\n", description);
}


int
main(int argc, char **argv)
{
    glfwSetErrorCallback(&Error);

    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
    if (!window) {
       return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    typedef void (APIENTRY *PFNGLCLEARCOLOR)(GLfloat, GLfloat, GLfloat, GLfloat);
    PFNGLCLEARCOLOR pfnClearColor;

#ifdef _WIN32

    HMODULE hModule = LoadLibraryA("OPENGL32");
    assert(hModule);
    pfnClearColor = (PFNGLCLEARCOLOR)GetProcAddress(hModule, "glClearColor");
    assert(pfnClearColor);

#else

#ifdef __APPLE__
    void *handle = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_NOW | RTLD_LOCAL);
#else
    void *handle = dlopen("libGL.so.1", RTLD_NOW | RTLD_LOCAL);
#endif
    assert(handle);

    pfnClearColor = (PFNGLCLEARCOLOR)dlsym(handle, "glClearColor");

#endif

    assert(pfnClearColor);

    pfnClearColor(0.25, 0.5, 0.75, 1.0);

#ifdef _WIN32
    FreeLibrary(hModule);
#else
    dlclose(handle);
#endif

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

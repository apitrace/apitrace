/**************************************************************************
 *
 * Copyright 2016 VMware, Inc
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include <iostream>

#include <GLFW/glfw3.h>


static void
sleep(void)
{
    unsigned msec = 500;
#ifdef _WIN32
    Sleep(msec);
#else
    usleep(msec*1000);
#endif
}


static void
checkWindowSize(GLFWwindow* window, int width, int height, bool fatal)
{
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    if (w == width && h == height) {
        return;
    }
    std::cerr << (fatal ? "error" : "warning") << ": "
              << "expected window size " << width << "x" << height
              << ", but got " << w << "x" << h << std::endl;
    if (fatal) {
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(320, 240, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    checkWindowSize(window, 320, 240, true);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // red
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(window);
    sleep();

    glfwSetWindowSize(window, 640, 480);

    glfwMakeContextCurrent(window);

    glViewport(0, 0, 640, 480);
    glScissor(0, 0, 640, 480);

    // XXX: This does not always succeed immediately
    checkWindowSize(window, 640, 480, false);

    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);  // yellow
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    sleep();

    checkWindowSize(window, 640, 480, true);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);  // green
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(window);
    sleep();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

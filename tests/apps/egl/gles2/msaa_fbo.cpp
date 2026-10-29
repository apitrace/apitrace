/*
 * Copyright 2026 Kun Luo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static void
checkGlError(const char *where)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "error: %s: GL error 0x%x\n", where, error);
        exit(EXIT_FAILURE);
    }
}


static void
checkFramebuffer(void)
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "error: incomplete framebuffer 0x%x\n", status);
        exit(EXIT_FAILURE);
    }
}


static int
runTest(void)
{
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        return EXIT_FAILURE;
    }
    if (!GLAD_GL_ES_VERSION_3_0) {
        return EXIT_SKIP;
    }

    GLint maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    if (maxSamples < 4) {
        return EXIT_SKIP;
    }

    GLuint renderbuffers[2];
    glGenRenderbuffers(2, renderbuffers);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[0]);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, 32, 32);
    checkGlError("multisample renderbuffer storage");

    GLint samples = 0;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
    if (samples != 4) {
        glDeleteRenderbuffers(2, renderbuffers);
        return EXIT_SKIP;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[1]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 32, 32);

    GLuint framebuffers[2];
    glGenFramebuffers(2, framebuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, renderbuffers[0]);
    checkFramebuffer();

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(16, 0, 16, 32);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, renderbuffers[1]);
    checkFramebuffer();
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Dump DRAW's unresolved blue/green image while READ contains solid red.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[0]);
    checkGlError("framebuffer setup and clears");
    glFlush();

    glDeleteFramebuffers(2, framebuffers);
    glDeleteRenderbuffers(2, renderbuffers);
    return 0;
}


int
main(int argc, char *argv[])
{
    if (!glfwInit()) {
        return EXIT_SKIP;
    }

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    GLFWwindow *window = glfwCreateWindow(64, 64, argv[0], NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_SKIP;
    }
    glfwMakeContextCurrent(window);

    int result = runTest();
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}

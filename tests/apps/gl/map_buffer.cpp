/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static GLFWwindow* window = NULL;


static const GLenum target = GL_ARRAY_BUFFER;

enum MapMethod {
   MAP_BUFFER_ARB,
   MAP_BUFFER_1_5,
   MAP_BUFFER_RANGE,
};

enum MapMethod mapMethod = MAP_BUFFER_ARB;


static void
parseArgs(int argc, char** argv)
{
   int i;

   for (i = 1; i < argc; ++i) {
      const char *arg = argv[i];
      if (strcmp(arg, "map_buffer_arb") == 0) {
         mapMethod = MAP_BUFFER_ARB;
      } else if (strcmp(arg, "map_buffer_1_5") == 0) {
         mapMethod = MAP_BUFFER_1_5;
      } else if (strcmp(arg, "map_buffer_range") == 0) {
         mapMethod = MAP_BUFFER_RANGE;
      } else {
         fprintf(stderr, "error: unexpected arg %s\n", arg);
         exit(1);
      }
   }
}


static void
testMapBufferARB(void)
{
    GLuint buffers[2];
    GLvoid *ptr;

    if (!GL_ARB_vertex_buffer_object) {
        fprintf(stderr, "error: GL_ARB_vertex_buffer_object not supported\n");
        exit(EXIT_SKIP);
    }

    glGenBuffersARB(2, buffers);

    glBindBufferARB(target, buffers[0]);
    glBufferDataARB(target, 1000, NULL, GL_STATIC_DRAW);

    ptr = glMapBufferARB(target, GL_WRITE_ONLY_ARB);
    memset(ptr, 1, 1000);
    glUnmapBufferARB(target);

    glBindBufferARB(target, buffers[1]);
    glBufferDataARB(target, 2000, NULL, GL_STATIC_DRAW);
    ptr = glMapBufferARB(target, GL_WRITE_ONLY_ARB);
    memset(ptr, 2, 2000);

    glBindBufferARB(target, buffers[0]);
    ptr = glMapBufferARB(target, GL_WRITE_ONLY_ARB);
    memset(ptr, 3, 1000);

    glBindBufferARB(target, buffers[1]);
    glUnmapBufferARB(target);

    glBindBufferARB(target, buffers[0]);
    glUnmapBufferARB(target);

    glMapBufferARB(target, GL_READ_ONLY_ARB);
    glUnmapBufferARB(target);

    glDeleteBuffersARB(2, buffers);
}


static void
testMapBuffer(void)
{
    GLuint buffers[2];
    GLvoid *ptr;

    if (!GLAD_GL_VERSION_1_5) {
        fprintf(stderr, "error: OpenGL version 1.5 not supported\n");
        exit(EXIT_SKIP);
    }

    glGenBuffers(2, buffers);

    glBindBuffer(target, buffers[0]);
    glBufferData(target, 1000, NULL, GL_STATIC_DRAW);

    ptr = glMapBuffer(target, GL_WRITE_ONLY);
    memset(ptr, 1, 1000);
    glUnmapBuffer(target);

    glBindBuffer(target, buffers[1]);
    glBufferData(target, 2000, NULL, GL_STATIC_DRAW);
    ptr = glMapBuffer(target, GL_WRITE_ONLY);
    memset(ptr, 2, 2000);

    glBindBuffer(target, buffers[0]);
    ptr = glMapBuffer(target, GL_WRITE_ONLY);
    memset(ptr, 3, 1000);

    glBindBuffer(target, buffers[1]);
    glUnmapBuffer(target);

    glBindBuffer(target, buffers[0]);
    glUnmapBuffer(target);

    glMapBuffer(target, GL_READ_ONLY);
    glUnmapBuffer(target);

    glDeleteBuffers(2, buffers);
}


static void
testMapBufferRange(void)
{
    GLuint buffers[2];
    GLvoid *ptr;

    if (!GLAD_GL_VERSION_1_5 ||
        (!GLAD_GL_VERSION_3_2 &&
         !GLAD_GL_ARB_map_buffer_range)) {
        fprintf(stderr, "error: GL_ARB_map_buffer_range not supported\n");
        exit(EXIT_SKIP);
    }

    glGenBuffers(2, buffers);

    glBindBuffer(target, buffers[0]);
    glBufferData(target, 1000, NULL, GL_STATIC_DRAW);

    ptr = glMapBufferRange(target, 100, 200, GL_MAP_WRITE_BIT);
    memset(ptr, 0, 200);
    glUnmapBuffer(target);

    glBindBuffer(target, buffers[1]);
    glBufferData(target, 2000, NULL, GL_STATIC_DRAW);
    ptr = glMapBufferRange(target, 200, 300, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
    memset(ptr, 0, 300);

    glBindBuffer(target, buffers[0]);
    ptr = glMapBufferRange(target, 100, 200, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
    memset(ptr, 0, 200);

    glBindBuffer(target, buffers[1]);
    glFlushMappedBufferRange(target, 20, 30);
    glFlushMappedBufferRange(target, 40, 50);
    glUnmapBuffer(target);

    glBindBuffer(target, buffers[0]);
    glFlushMappedBufferRange(target, 10, 20);
    glFlushMappedBufferRange(target, 30, 40);
    glUnmapBuffer(target);

    glMapBufferRange(target, 100, 200, GL_MAP_READ_BIT);
    glUnmapBuffer(target);

    glDeleteBuffers(2, buffers);
}


int main(int argc, char** argv)
{
    parseArgs(argc, argv);

    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
#ifdef __APPLE__
    if (mapMethod == MAP_BUFFER_RANGE) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
#endif

    window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return EXIT_FAILURE;
    }

    switch (mapMethod) {
    case MAP_BUFFER_ARB:
        testMapBufferARB();
        break;
    case MAP_BUFFER_1_5:
        testMapBuffer();
        break;
    case MAP_BUFFER_RANGE:
        testMapBufferRange();
        break;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

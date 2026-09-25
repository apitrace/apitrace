/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>


static GLFWwindow* window = NULL;


static const GLenum target = GL_ARRAY_BUFFER;

enum MapMethod {
   MAP_BUFFER_OES,
   MAP_BUFFER_RANGE_EXT,
   MAP_BUFFER_RANGE_3_0,
};

enum MapMethod mapMethod = MAP_BUFFER_OES;


static void
parseArgs(int argc, char** argv)
{
   int i;

   for (i = 1; i < argc; ++i) {
      const char *arg = argv[i];
      if (strcmp(arg, "map_buffer_oes") == 0) {
         mapMethod = MAP_BUFFER_OES;
      } else if (strcmp(arg, "map_buffer_range_ext") == 0) {
         mapMethod = MAP_BUFFER_RANGE_EXT;
      } else if (strcmp(arg, "map_buffer_range_3_0") == 0) {
         mapMethod = MAP_BUFFER_RANGE_3_0;
      } else {
         fprintf(stderr, "error: unexpected arg %s\n", arg);
         exit(1);
      }
   }
}


static void
testMapBufferOES(void)
{
    GLuint buffers[2];
    GLvoid *ptr;

    if (!GLAD_GL_OES_mapbuffer) {
        fprintf(stderr, "error: GL_OES_mapbuffer not supported\n");
        exit(EXIT_SKIP);
    }

    glGenBuffers(2, buffers);

    glBindBuffer(target, buffers[0]);
    glBufferData(target, 1000, NULL, GL_STATIC_DRAW);

    ptr = glMapBufferOES(target, GL_WRITE_ONLY_OES);
    memset(ptr, 1, 1000);
    glUnmapBufferOES(target);

    glBindBuffer(target, buffers[1]);
    glBufferData(target, 2000, NULL, GL_STATIC_DRAW);
    ptr = glMapBufferOES(target, GL_WRITE_ONLY_OES);
    memset(ptr, 2, 2000);

    glBindBuffer(target, buffers[0]);
    ptr = glMapBufferOES(target, GL_WRITE_ONLY_OES);
    memset(ptr, 3, 1000);

    glBindBuffer(target, buffers[1]);
    glUnmapBufferOES(target);

    glBindBuffer(target, buffers[0]);
    glUnmapBufferOES(target);

    glDeleteBuffers(2, buffers);
}


static void
testMapBufferRangeEXT(void)
{
    GLuint buffers[2];
    GLvoid *ptr;

    if (!GLAD_GL_EXT_map_buffer_range) {
        fprintf(stderr, "error: GL_EXT_map_buffer_range not supported\n");
        exit(EXIT_SKIP);
    }

    glGenBuffers(2, buffers);

    glBindBuffer(target, buffers[0]);
    glBufferData(target, 1000, NULL, GL_STATIC_DRAW);

    ptr = glMapBufferRangeEXT(target, 100, 200, GL_MAP_WRITE_BIT_EXT);
    memset(ptr, 0, 200);
    glUnmapBufferOES(target);

    glBindBuffer(target, buffers[1]);
    glBufferData(target, 2000, NULL, GL_STATIC_DRAW);
    ptr = glMapBufferRangeEXT(target, 200, 300, GL_MAP_WRITE_BIT_EXT | GL_MAP_FLUSH_EXPLICIT_BIT_EXT);
    memset(ptr, 0, 300);

    glBindBuffer(target, buffers[0]);
    ptr = glMapBufferRangeEXT(target, 100, 200, GL_MAP_WRITE_BIT_EXT | GL_MAP_FLUSH_EXPLICIT_BIT_EXT);
    memset(ptr, 0, 200);

    glBindBuffer(target, buffers[1]);
    glFlushMappedBufferRangeEXT(target, 20, 30);
    glFlushMappedBufferRangeEXT(target, 40, 50);
    glUnmapBufferOES(target);

    glBindBuffer(target, buffers[0]);
    glFlushMappedBufferRangeEXT(target, 10, 20);
    glFlushMappedBufferRangeEXT(target, 30, 40);
    glUnmapBufferOES(target);

    glMapBufferRangeEXT(target, 100, 200, GL_MAP_READ_BIT_EXT);
    glUnmapBufferOES(target);

    glDeleteBuffers(2, buffers);
}


static void
testMapBufferRange30(void)
{
    GLuint buffers[2];
    GLvoid *ptr;

    if (!GLAD_GL_ES_VERSION_3_0) {
        fprintf(stderr, "error: OpenGL ES 3.0 not supported\n");
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
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);

    window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

   if (!gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress)) {
       return EXIT_FAILURE;
   }

    switch (mapMethod) {
    case MAP_BUFFER_OES:
        testMapBufferOES();
        break;
    case MAP_BUFFER_RANGE_EXT:
        testMapBufferRangeEXT();
        break;
    case MAP_BUFFER_RANGE_3_0:
        testMapBufferRange30();
        break;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

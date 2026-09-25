/*
 * Copyright (C) 2008  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>


static GLFWwindow* window = NULL;


static void
draw(void)
{
   if (GLAD_GL_EXT_debug_marker) {
       glPushGroupMarkerEXT(0, __FUNCTION__);
   }

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if (GLAD_GL_EXT_debug_marker) {
       glPopGroupMarkerEXT();
   }

   glfwSwapBuffers(window);
}


/* new window size or exposure */
static void
reshape(void)
{
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   glViewport(0, 0, (GLint) width, (GLint) height);
}


static void
init(void)
{
   if (GLAD_GL_EXT_debug_marker) {
      assert(glInsertEventMarkerEXT);

      glInsertEventMarkerEXT(strlen("Init"), "Init - this should not be included");
   }

   glClearColor(0.4, 0.4, 0.4, 0.0);
}

int
main(int argc, char *argv[])
{
   glfwInit();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

   window = glfwCreateWindow(300, 300, argv[0], NULL, NULL);
   if (!window) {
       return EXIT_SKIP;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress)) {
       return EXIT_FAILURE;
   }

   assert(GLAD_GL_ES_VERSION_2_0);
   assert(GLAD_GL_EXT_debug_marker == glfwExtensionSupported("GL_EXT_debug_marker"));

   init();
   reshape();
   draw();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

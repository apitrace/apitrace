/**************************************************************************
 *
 * Copyright 2011-2014 Jose Fonseca
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

#include <GLFW/glfw3.h>


static GLboolean doubleBuffer = GL_TRUE;
static GLboolean coreProfile = GL_FALSE;
static GLFWwindow* window = NULL;


static void
parseArgs(int argc, char** argv)
{
   int i;

   for (i = 1; i < argc; ++i) {
      const char *arg = argv[i];
      if (strcmp(arg, "-sb") == 0) {
         doubleBuffer = GL_FALSE;
      } else if (strcmp(arg, "-db") == 0) {
         doubleBuffer = GL_TRUE;
      } else if (strcmp(arg, "-core") == 0) {
         coreProfile = GL_TRUE;
      } else {
         fprintf(stderr, "error: unknown arg %s\n", arg);
         exit(1);
      }
   }
}


int
main(int argc, char **argv)
{
   parseArgs(argc, argv);

   glfwInit();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

   if (!doubleBuffer) {
       glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
   }

   if (coreProfile) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   }

   window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
   if (!window) {
       return EXIT_SKIP;
   }

   glfwMakeContextCurrent(window);

   glFlush();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

/*
 * Copyright (c) 1991, 1992, 1993 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the name of
 * Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
 * ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#ifndef GL_SHADING_LANGUAGE_VERSION
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif


static GLFWwindow* window = NULL;


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

   window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
   if (!window) {
      return EXIT_SKIP;
   }

   glfwMakeContextCurrent(window);

   glGetString(GL_VERSION);
   glGetString(GL_VENDOR);
   glGetString(GL_EXTENSIONS);
   glGetString(GL_RENDERER);
   glGetString(GL_SHADING_LANGUAGE_VERSION);
   GLint maxTextureSize = 0;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

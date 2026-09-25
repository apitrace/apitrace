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


static GLFWwindow* window = NULL;


static void
Error(int error, const char *description)
{
   fprintf(stderr, "glfw: error: %s\n", description);
}


static void
Init(void)
{
   glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
}


static void
Reshape(void)
{
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 1000.0);
   glMatrixMode(GL_MODELVIEW);
}


static void
Draw(void)
{
   glClear(GL_COLOR_BUFFER_BIT); 

   glBegin(GL_TRIANGLES);
   glColor3f(.8, 0, 0); 
   glVertex3f(-0.9, -0.9, -30.0);
   glColor3f(0, .9, 0); 
   glVertex3f( 0.9, -0.9, -30.0);
   glColor3f(0, 0, .7); 
   glVertex3f( 0.0,  0.9, -30.0);
   glEnd();

   glFlush();

   glfwSwapBuffers(window);
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

   Init();
   Reshape();
   Draw();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

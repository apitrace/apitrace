/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  varray.c
 *  This program demonstrates vertex arrays.
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static GLFWwindow* window = NULL;


enum SetupMethod {
   POINTER,
   INTERLEAVED,
};

enum DerefMethod {
   DRAWARRAYS,
   ARRAYELEMENT,
   DRAWELEMENTS,
   DRAWARRAYSINDIRECT,
};

static enum SetupMethod setupMethod = POINTER;
static enum DerefMethod derefMethod = DRAWELEMENTS;


static void parseArgs(int argc, char** argv)
{
   int i;

   for (i = 1; i < argc; ++i) {
      const char *arg = argv[i];
      if (strcmp(arg, "pointer") == 0) {
         setupMethod = POINTER;
      } else if (strcmp(arg, "interleaved") == 0) {
         setupMethod = INTERLEAVED;
      } else if (strcmp(arg, "drawarrays") == 0) {
         derefMethod = DRAWARRAYS;
      } else if (strcmp(arg, "arrayelement") == 0) {
         derefMethod = ARRAYELEMENT;
      } else if (strcmp(arg, "drawelements") == 0) {
         derefMethod = DRAWELEMENTS;
      } else if (strcmp(arg, "drawarraysindirect") == 0) {
         derefMethod = DRAWARRAYSINDIRECT;
      } else {
         fprintf(stderr, "error: unknown arg %s\n", arg);
         exit(1);
      }
   }
}

static void setupPointers(void)
{
   static const GLint vertices[] = {
       25,  25,
      100, 325,
      175,  25,
      175, 325,
      250,  25,
      325, 325
   };
   static const GLfloat colors[] = {
      1.00f, 0.20f, 0.20f,
      0.20f, 0.20f, 1.00f,
      0.80f, 1.00f, 0.20f,
      0.75f, 0.75f, 0.75f,
      0.35f, 0.35f, 0.35f,
      0.50f, 0.50f, 0.50f
   };

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   glVertexPointer(2, GL_INT, 2 * sizeof(GLint), vertices);
   glColorPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), colors);
}

static void setupInterleave(void)
{
   static const GLfloat intertwined[] = {
       1.00f, 0.20f, 0.20f,  25.0f,  25.0f, 0.0f,
       0.20f, 0.20f, 1.00f, 100.0f, 325.0f, 0.0f,
       0.80f, 1.00f, 0.20f, 175.0f,  25.0f, 0.0f,
       0.75f, 0.75f, 0.75f, 175.0f, 325.0f, 0.0f,
       0.35f, 0.35f, 0.35f, 250.0f,  25.0f, 0.0f,
       0.50f, 0.50f, 0.50f, 325.0f, 325.0f, 0.0f
   };

   glInterleavedArrays(GL_C3F_V3F, 0, intertwined);
}

static void init(void)
{
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glShadeModel(GL_SMOOTH);
   switch (setupMethod) {
   case POINTER:
       setupPointers();
       break;
   case INTERLEAVED:
       setupInterleave();
       break;
   }
}

static void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);

   if (derefMethod == DRAWARRAYS) {
      glDrawArrays(GL_TRIANGLES, 0, 6);
   } else if (derefMethod == ARRAYELEMENT) {
      glBegin(GL_TRIANGLES);
      glArrayElement(2);
      glArrayElement(3);
      glArrayElement(5);
      glEnd();
   } else if (derefMethod == DRAWELEMENTS) {
      GLuint indices[4] = {0, 1, 3, 4};

      glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
   } else if (derefMethod == DRAWARRAYSINDIRECT) {
      typedef  struct {
         GLuint count;
         GLuint instanceCount;
         GLuint first;
         GLuint baseInstance;
      } DrawArraysIndirectCommand;

      static const DrawArraysIndirectCommand indirect = { 6, 1, 0, 0 };

      glDrawArraysIndirect(GL_TRIANGLE_STRIP, &indirect);
   } else {
      abort();
   }
   glFlush();

   glfwSwapBuffers(window);
}

static void reshape(void)
{
   int w, h;
   glfwGetFramebufferSize(window, &w, &h);

   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, (GLdouble) w, 0.0, (GLdouble) h, -1.0, 1.0);
}

int main(int argc, char** argv)
{
   parseArgs(argc, argv);

   glfwInit();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
   if (derefMethod >= DRAWARRAYSINDIRECT) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
   }

   window = glfwCreateWindow(350, 350, argv[0], NULL, NULL);
   if (!window) {
       return EXIT_SKIP;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
       return EXIT_FAILURE;
   }

   init();
   reshape();
   display();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

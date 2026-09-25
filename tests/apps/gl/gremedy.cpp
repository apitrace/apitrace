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

#include "common.h"


static GLFWwindow* window = NULL;


static void Init(void)
{
   const GLubyte *extensions;
   GLboolean has_GL_GREMEDY_string_marker;
   GLboolean has_GL_GREMEDY_frame_terminator;
      
   extensions = glGetString(GL_EXTENSIONS);
   checkGlError("glGetString(GL_EXTENSIONS)");
   has_GL_GREMEDY_string_marker = checkExtension("GL_GREMEDY_string_marker", extensions);
   has_GL_GREMEDY_frame_terminator = checkExtension("GL_GREMEDY_string_marker", extensions);

   if (GLAD_GL_VERSION_3_0) {
      GLint has_GL3_GREMEDY_string_marker = 0;
      GLint has_GL3_GREMEDY_frame_terminator = 0;
      GLint i;

      GLint num_extensions = 0;
      glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
      checkGlError("glGetIntegerv(GL_NUM_EXTENSIONS)");

      for (i = 0; i < num_extensions; ++i) {
         const char *extension;

         extension = (const char *)glGetStringi(GL_EXTENSIONS, i);
         checkGlError("glGetStringi(GL_EXTENSIONS, i)");

         if (strlen(extension) == 0) {
            fprintf(stderr, "error: glGetStringi returned empty string\n");
            exit(1);
         }

         if (strcmp(extension, "GL_GREMEDY_string_marker") == 0) {
            ++has_GL3_GREMEDY_string_marker;
         }

         if (strcmp(extension, "GL_GREMEDY_frame_terminator") == 0) {
            ++has_GL3_GREMEDY_frame_terminator;
         }
      }

      if (!!has_GL_GREMEDY_string_marker != !!has_GL3_GREMEDY_string_marker) {
         fprintf(stderr, "error: GL_GREMEDY_string_marker not consistently supported by GL3\n");
         exit(1);
      }

      if (!!has_GL_GREMEDY_frame_terminator != !!has_GL3_GREMEDY_frame_terminator) {
         fprintf(stderr, "error: GL_GREMEDY_frame_terminator not consistently supported by GL3\n");
         exit(1);
      }
   }

   if (!!has_GL_GREMEDY_string_marker != !!GLAD_GL_GREMEDY_string_marker) {
      fprintf(stderr, "error: GL_GREMEDY_string_marker not consistently supported by GLAD\n");
      exit(1);
   }

   if (!!has_GL_GREMEDY_frame_terminator != !!GLAD_GL_GREMEDY_frame_terminator) {
      fprintf(stderr, "error: GL_GREMEDY_frame_terminator not consistently supported by GLAD\n");
      exit(1);
   }

   if (GLAD_GL_GREMEDY_string_marker) {
      glStringMarkerGREMEDY(strlen("Init"), "Init - this should not be included");
   }

   glClearColor(0.3, 0.1, 0.3, 1.0);
}

static void Reshape(void)
{
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   if (GLAD_GL_GREMEDY_string_marker) {
      glStringMarkerGREMEDY(0, __FUNCTION__);
   }

   glViewport(0, 0, (GLint)width, (GLint)height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 1000.0);
   glMatrixMode(GL_MODELVIEW);
}

static void Draw(void)
{
   if (GLAD_GL_GREMEDY_string_marker) {
      glStringMarkerGREMEDY(0, __FUNCTION__);
   }

   glClear(GL_COLOR_BUFFER_BIT); 

   glBegin(GL_TRIANGLES);
   glColor3f(.8,0,0); 
   glVertex3f(-0.9, -0.9, -30.0);
   glColor3f(0,.9,0); 
   glVertex3f( 0.9, -0.9, -30.0);
   glColor3f(0,0,.7); 
   glVertex3f( 0.0,  0.9, -30.0);
   glEnd();

   glFlush();

   if (GLAD_GL_GREMEDY_frame_terminator) {
      glFrameTerminatorGREMEDY();
   }

   glfwSwapBuffers(window);
}

int main(int argc, char **argv)
{
   glfwInit();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

   window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
   if (!window) {
       return EXIT_SKIP;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
       return EXIT_FAILURE;
   }

   Init();
   Reshape();
   Draw();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

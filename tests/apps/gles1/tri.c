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

/*
 * Draw a triangle with X/EGL and OpenGL ES 1.x
 * Brian Paul
 * 5 June 2008
 */

#define USE_FIXED_POINT 0

#include <math.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define FLOAT_TO_FIXED(X)   ((X) * 65535.0)


static GLFWwindow* window;


static void
draw(void)
{
#if USE_FIXED_POINT
   static const GLfixed verts[3][2] = {
      { -65536, -65536 },
      {  65536, -65536 },
      {      0,  65536 }
   };
   static const GLfixed colors[3][4] = {
      { 65536,     0,     0,    65536 },
      {     0, 65536,     0 ,   65536},
      {     0,     0, 65536 ,   65536}
   };
#else
   static const GLfloat verts[3][2] = {
      { -1, -1 },
      {  1, -1 },
      {  0,  1 }
   };
   static const GLfloat colors[3][4] = {
      { 1, 0, 0, 1 },
      { 0, 1, 0, 1 },
      { 0, 0, 1, 1 }
   };
#endif

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   {
#if USE_FIXED_POINT
      glVertexPointer(2, GL_FIXED, 0, verts);
      glColorPointer(4, GL_FIXED, 0, colors);
#else
      glVertexPointer(2, GL_FLOAT, 0, verts);
      glColorPointer(4, GL_FLOAT, 0, colors);
#endif
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      /* draw triangle */
      glDrawArrays(GL_TRIANGLES, 0, 3);

      /* draw some points */
      glPointSizex(FLOAT_TO_FIXED(15.5));
      glDrawArrays(GL_POINTS, 0, 3);

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
   }

   glfwSwapBuffers(window);
}


/* new window size or exposure */
static void
reshape(void)
{
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   GLfloat ar = (GLfloat) width / (GLfloat) height;

   glViewport(0, 0, (GLint) width, (GLint) height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustumf(-ar, ar, -1, 1, 5.0, 60.0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -10.0);
}


static void
init(void)
{
   glClearColor(0.4, 0.4, 0.4, 0.0);
}


int
main(int argc, char *argv[])
{
   glfwInit();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

   window = glfwCreateWindow(300, 300, argv[0], NULL, NULL);
   if (!window) {
       return EXIT_SKIP;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLES1Loader((GLADloadproc)glfwGetProcAddress)) {
      return EXIT_FAILURE;
   }

   init();
   reshape();
   draw();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

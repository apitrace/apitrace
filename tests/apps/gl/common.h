/**************************************************************************
 *
 * Copyright 2013 VMware, Inc.
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

#include <glad/glad.h>

#define GLFW_INCLUDE_GLU // for gluErrorString

#include <GLFW/glfw3.h>


/**
 * Identical to gluCheckExtension, which is not part of GLU on Windows.
 */
static GLboolean
checkExtension(const char *extName, const GLubyte *extString)
{
   const char *p = (const char *)extString;
   const char *q = extName;
   char c;
   do {
       c = *p++;
       if (c == '\0' || c == ' ') {
           if (q && *q == '\0') {
               return GL_TRUE;
           } else {
               q = extName;
           }
       } else {
           if (q && *q == c) {
               ++q;
           } else {
               q = 0;
           }
       }
   } while (c);
   return GL_FALSE;
}


static void
checkGlError(const char *call) {
   GLenum error = glGetError();
   if (error != GL_NO_ERROR) {
      fprintf(stderr, "error: %s -> %s\n", call, gluErrorString(error));
      exit(1);
   }
}

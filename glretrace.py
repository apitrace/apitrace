##########################################################################
#
# Copyright 2010 VMware, Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


import stdapi
import glapi
import retrace


class GlRetracer(retrace.Retracer):

    pass


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print
    print '#ifdef WIN32'
    print '#include <windows.h>'
    print '#endif'
    print
    print '#include <GL/glew.h>'
    print '#include <GL/glut.h>'
    print
    api = glapi.glapi
    retracer = GlRetracer()
    retracer.retrace_api(glapi.glapi)
    print '''

Trace::Parser parser;

static bool insideGlBeginEnd;

static void display(void) {
   Trace::Call *call;

   while ((call = parser.parse_call())) {
      if (call->name == "glFlush" ||
          call->name == "glXSwapBuffers" ||
          call->name == "wglSwapBuffers") {
         glFlush();
         return;
      }
      
      retrace_call(*call);

      if (call->name == "glBegin") {
         insideGlBeginEnd = true;
      }
      
      if (call->name == "glEnd") {
         insideGlBeginEnd = false;
      }

      if (!insideGlBeginEnd) {
         GLenum error = glGetError();
         if (error != GL_NO_ERROR) {
            std::cerr << "warning: glGetError() = ";
            switch (error) {
            case GL_INVALID_ENUM:
               std::cerr << "GL_INVALID_ENUM";
               break;
            case GL_INVALID_VALUE:
               std::cerr << "GL_INVALID_VALUE";
               break;
            case GL_INVALID_OPERATION:
               std::cerr << "GL_INVALID_OPERATION";
               break;
            case GL_STACK_OVERFLOW:
               std::cerr << "GL_STACK_OVERFLOW";
               break;
            case GL_STACK_UNDERFLOW:
               std::cerr << "GL_STACK_UNDERFLOW";
               break;
            case GL_OUT_OF_MEMORY:
               std::cerr << "GL_OUT_OF_MEMORY";
               break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
               std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
               break;
            case GL_TABLE_TOO_LARGE:
               std::cerr << "GL_TABLE_TOO_LARGE";
               break;
            default:
               std::cerr << error;
               break;
            }
            std::cerr << "\\n";
         }
      }
   }

   glFlush();
   glutIdleFunc(NULL);
}

static void idle(void) {
   glutPostRedisplay();
}

int main(int argc, char **argv)
{
   glutInit(&argc, argv);
   glutInitWindowPosition(0, 0);
   glutInitWindowSize(800, 600);
   glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE);
   glutCreateWindow(argv[0]);
   glewInit();

   glutDisplayFunc(&display);
   glutIdleFunc(&idle);

    int i;
    for (i = 1; i < argc; ++i) {
      const char *arg = argv[i];

      if (arg[0] != '-') {
         break;
      }

      if (!strcmp(arg, "--")) {
         break;
      }
      else if (!strcmp(arg, "-v")) {
         ++verbosity;
      } else {
         std::cerr << "error: unknown option " << arg << "\\n";
         return 1;
      }
   }

   for ( ; i < argc; ++i) {
      if (parser.open(argv[i])) {
         glutMainLoop();
         parser.close();
      }
   }

   return 0;
}

'''   

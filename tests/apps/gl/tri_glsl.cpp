/* Test fragment shader
 *
 * Brian Pau
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static GLFWwindow* window;

static GLuint fragShader;
static GLuint vertShader;
static GLuint program;


static void
LoadAndCompileShader(GLuint shader, const char *text)
{
   const GLint length = -1;
   GLint stat;

   glShaderSource(shader, 1, (const GLchar **) &text, &length);

   glCompileShader(shader);

   glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
   if (!stat) {
      GLchar log[1000];
      GLsizei len;
      glGetShaderInfoLog(shader, 1000, &len, log);
      fprintf(stderr, "fslight: problem compiling shader:\n%s\n", log);
      exit(1);
   }
}


static void
CheckLink(GLuint prog)
{
   GLint stat;
   glGetProgramiv(prog, GL_LINK_STATUS, &stat);
   if (!stat) {
      GLchar log[1000];
      GLsizei len;
      glGetProgramInfoLog(prog, 1000, &len, log);
      fprintf(stderr, "Linker error:\n%s\n", log);
   }
}


static void
Init(void)
{
   static const char *fragShaderText =
      "void main() {\n"
      "   gl_FragColor = gl_Color;\n"
      "}\n";
   static const char *vertShaderText =
      "void main() {\n"
      "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
      "   gl_FrontColor = gl_Color;\n"
      "}\n";

   if (!GLAD_GL_VERSION_2_0) {
      printf("This program requires OpenGL 2.x\n");
      exit(EXIT_SKIP);
   }

   fragShader = glCreateShader(GL_FRAGMENT_SHADER);
   LoadAndCompileShader(fragShader, fragShaderText);

   vertShader = glCreateShader(GL_VERTEX_SHADER);
   LoadAndCompileShader(vertShader, vertShaderText);

   program = glCreateProgram();
   glAttachShader(program, fragShader);
   glAttachShader(program, vertShader);
   glLinkProgram(program);
   CheckLink(program);

   glDetachShader(program, fragShader);
   glDetachShader(program, vertShader);
   glDeleteShader(fragShader);
   glDeleteShader(vertShader);

   glUseProgram(program);

   assert(glGetError() == 0);

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
   glfwInit();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

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

   glDeleteProgram(program);

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

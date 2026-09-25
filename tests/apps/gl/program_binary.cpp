/**************************************************************************
 *
 * Copyright 2008 VMware, Inc.
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

/*
 * Draw a triangle with OpenGL 3.2 Core profile
 */


#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static GLFWwindow* window;


static bool tracing;
static GLuint fragShader, vertShader;
static GLuint program = 0;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))


static void
draw(void)
{
    GLint u_matrix = -1;
    GLint attr_pos = 0, attr_color = 1;

    glBindFragDataLocation(program, 0, "f_color");

    glUseProgram(program);

    attr_pos = glGetAttribLocation(program, "pos");
    attr_color = glGetAttribLocation(program, "color");

    u_matrix = glGetUniformLocation(program, "modelviewProjection");
    struct Vertex {
        GLfloat pos[2];
        GLfloat color[3];
    };

    const Vertex verts[] = {
        { { -0.9f, -0.9f }, {  0.8f, 0.0f, 0.0f } },
        { {  0.9f, -0.9f }, {  0.0f, 0.9f, 0.0f } },
        { {  0.0f,  0.9f }, {  0.0f, 0.0f, 0.7f } }
    };
    GLuint vbo;

    const GLfloat mat[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &vbo);

    /* Set modelview/projection matrix */
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);

    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof verts, verts, GL_STATIC_DRAW);

    glVertexAttribPointer(attr_pos, ARRAY_SIZE(verts[0].pos), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(attr_pos);

    glVertexAttribPointer(attr_color, ARRAY_SIZE(verts[0].color), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
    glEnableVertexAttribArray(attr_color);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(attr_pos);
    glDisableVertexAttribArray(attr_color);

    glUseProgram(0);

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

    if (!GLAD_GL_VERSION_4_1 &&
        !GLAD_GL_ARB_get_program_binary) {
        exit(EXIT_SKIP);
    }

    GLint numProgramBinaryFormats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numProgramBinaryFormats);
    if ((tracing && numProgramBinaryFormats != 0) ||
        numProgramBinaryFormats < 0) {
        fprintf(stderr, "error: glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS) = %i\n", numProgramBinaryFormats);
        exit(EXIT_FAILURE);
    }

    GLint *programBinaryFormats = new GLint[numProgramBinaryFormats + 1];
    programBinaryFormats[numProgramBinaryFormats] = 0x0BADF00D;
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, programBinaryFormats);
    if (programBinaryFormats[numProgramBinaryFormats] != 0x0BADF00D) {
        fprintf(stderr, "error: glGetIntegerv(GL_PROGRAM_BINARY_FORMATS) overflowed\n");
        exit(EXIT_FAILURE);
    }

    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);

    static const char *fragShaderText =
        "#version 150\n"
        "in vec4 v_color;\n"
        "out vec4 f_color;\n"
        "void main() {\n"
        "    f_color = v_color;\n"
        "}\n";
    static const char *vertShaderText =
        "#version 150\n"
        "uniform mat4 modelviewProjection;\n"
        "in vec4 pos;\n"
        "in vec4 color;\n"
        "out vec4 v_color;\n"
        "void main() {\n"
        "    gl_Position = modelviewProjection * pos;\n"
        "    v_color = color;\n"
        "}\n";

    GLint status;
    char log[1000];
    GLsizei len;

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(fragShader, sizeof log, &len, log);
        fprintf(stderr, "error: compiling fragment shader:\n%s\n", log);
        exit(1);
    }

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(vertShader, sizeof log, &len, log);
        fprintf(stderr, "error: compiling vertex shader:\n%s\n", log);
        exit(1);
    }

    program = glCreateProgram();
    glAttachShader(program, fragShader);
    glAttachShader(program, vertShader);
    glBindAttribLocation(program, 0, "pos");
    glBindAttribLocation(program, 1, "color");
    glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        glGetProgramInfoLog(program, sizeof log, &len, log);
        fprintf(stderr, "error: linking:\n%s\n", log);
        exit(1);
    }
}


static void APIENTRY
debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    fprintf(stderr, "%s\n", message);
}


int
main(int argc, char *argv[])
{
    tracing = !getenv("DRY_RUN");

    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
    if (!window) {
         return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
       return EXIT_FAILURE;
    }

    if (GLAD_GL_KHR_debug) {
        glDebugMessageCallback(&debugMessageCallback, NULL);
    }

    init();
    reshape();
    draw();

    GLint binaryLength = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
    if ((tracing && binaryLength != 0) ||
        binaryLength < 0) {
        fprintf(stderr, "error: glGetProgramiv(GL_PROGRAM_BINARY_LENGTH) = %i\n", binaryLength);
        exit(EXIT_FAILURE);
    }

    while (glGetError() != GL_NO_ERROR) ;

    GLsizei bufSize = binaryLength > 0 ? binaryLength : 32 * 1024 * 1024;
    void *binary = malloc(bufSize);
    GLsizei length = 0;
    GLenum binaryFormat = GL_NONE;
    glGetProgramBinary(program, bufSize, &length, &binaryFormat, binary);
    GLenum error = glGetError();
    if (tracing && error == GL_NO_ERROR) {
        fprintf(stderr, "error: glGetProgramiv(GL_PROGRAM_BINARY_LENGTH) = %i\n", binaryLength);
        exit(EXIT_FAILURE);
    }

    FILE *fp;

    if (!tracing) {
        fp = fopen("program_binary.bin", "wb");
        fwrite(&binaryFormat, sizeof binaryFormat, 1, fp);
        fwrite(&binaryLength, sizeof binaryLength, 1, fp);
        fwrite(binary, 1, binaryLength, fp);
        fclose(fp);
    }

    free(binary);
   
    glDeleteShader(fragShader);
    glDeleteShader(vertShader);
    glDeleteProgram(program);

    program = glCreateProgram();

    fp = fopen("program_binary.bin", "rb");
    fread(&binaryFormat, sizeof binaryFormat, 1, fp);
    fread(&binaryLength, sizeof binaryLength, 1, fp);
    binary = malloc(binaryLength);
    fread(binary, 1, binaryLength, fp);
    fclose(fp);

    glProgramBinary(program, binaryFormat, binary, binaryLength);
    
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLsizei len;
        char log[1000];
        glGetProgramInfoLog(program, sizeof log, &len, log);
        fprintf(stderr, "warning: glProgramBinary failed -- %s\n", log);
    }
    if (tracing && status) {
        exit(EXIT_FAILURE);
    }

    if (status) {
        draw();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

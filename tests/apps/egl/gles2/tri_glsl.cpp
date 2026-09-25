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
 * Draw a triangle with X/EGL and OpenGL ES 2.x
 */


#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static GLFWwindow* window;


static GLint u_matrix = -1;
static GLint attr_pos = 0, attr_color = 1;


static void
draw(void)
{
    static const GLfloat verts[3][2] = {
        { -0.9f, -0.9f },
        {  0.9f, -0.9f },
        {  0.0f,  0.9f }
    };
    static const GLfloat colors[3][3] = {
        { 0.8f, 0.0f, 0.0f },
        { 0.0f, 0.9f, 0.0f },
        { 0.0f, 0.0f, 0.7f }
    };
    const GLfloat mat[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    /* Set modelview/projection matrix */
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(attr_pos);
    glEnableVertexAttribArray(attr_color);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(attr_pos);
    glDisableVertexAttribArray(attr_color);

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
create_shaders(void)
{
    static const char *fragShaderText =
        "precision mediump float;\n"
        "varying vec4 v_color;\n"
        "void main() {\n"
        "    gl_FragColor = v_color;\n"
        "}\n";
    static const char *vertShaderText =
        "uniform mat4 modelviewProjection;\n"
        "attribute vec4 pos;\n"
        "attribute vec4 color;\n"
        "varying vec4 v_color;\n"
        "void main() {\n"
        "    gl_Position = modelviewProjection * pos;\n"
        "    v_color = color;\n"
        "}\n";

    GLuint fragShader, vertShader, program;
    GLint stat;
    char log[1000];
    GLsizei len;

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
    if (!stat) {
        glGetShaderInfoLog(fragShader, sizeof log, &len, log);
        fprintf(stderr, "error: compiling fragment shader:\n%s\n", log);
        exit(1);
    }

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
    if (!stat) {
        glGetShaderInfoLog(vertShader, sizeof log, &len, log);
        fprintf(stderr, "error: compiling vertex shader:\n%s\n", log);
        exit(1);
    }

    program = glCreateProgram();
    glAttachShader(program, fragShader);
    glAttachShader(program, vertShader);
    glBindAttribLocation(program, attr_pos, "pos");
    glBindAttribLocation(program, attr_color, "color");
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &stat);
    if (!stat) {
        glGetProgramInfoLog(program, sizeof log, &len, log);
        fprintf(stderr, "error: linking:\n%s\n", log);
        exit(1);
    }

    glUseProgram(program);

    attr_pos = glGetAttribLocation(program, "pos");
    attr_color = glGetAttribLocation(program, "color");

    u_matrix = glGetUniformLocation(program, "modelviewProjection");
}


static void
init(void)
{
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);

    create_shaders();
}


int
main(int argc, char *argv[])
{
    int version_major = 2;

    int i;

    for (i = 1; i < argc; ++i) {
         version_major = atoi(argv[i]);
    }

    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_major);

    window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        return EXIT_FAILURE;
    }

    init();
    reshape();
    draw();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

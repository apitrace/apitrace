/**************************************************************************
 *
 * Copyright 2016 VMware, Inc
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


#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>


int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    GLFWwindow* window = NULL;
    window = glfwCreateWindow(350, 350, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!glfwExtensionSupported("GL_EXT_compiled_vertex_array")) {
        return EXIT_SKIP;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return EXIT_FAILURE;
    }

    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);

    static const float vertices[6][3] = {
        { -0.9f, -0.9f, -30.0f },
        {  0.9f, -0.9f, -30.0f },
        {  0.0f,  0.9f, -30.0f },
        {  0.0f,  0.0f,   0.0f },
        {  0.0f,  0.0f,   0.0f },
        {  0.0f,  0.0f,   0.0f }
    };

    static const float colors[6][3] = {
        { 0.8f, 0.0f, 0.0f },
        { 0.0f, 0.9f, 0.0f },
        { 0.0f, 0.0f, 0.7f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };

    static const GLuint indices[3] = { 0, 1, 2 };

    glLockArraysEXT(0, 6);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &indices);
    glUnlockArraysEXT();

    glfwSwapBuffers(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

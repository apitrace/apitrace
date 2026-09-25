/**************************************************************************
 *
 * Copyright 2014 VMware, Inc
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>



#define GL_MAP_NOTIFY_EXPLICIT_BIT_VMWX 0x80000000


typedef void (APIENTRY * PFNGLNOTIFYMAPPEDBUFFERRANGEVMWXPROC)(const void * start, GLsizeiptr length);

static void APIENTRY
noopGlNotifyMappedBufferRangeVMWX(const void * start, GLsizeiptr length) {
}

static PFNGLNOTIFYMAPPEDBUFFERRANGEVMWXPROC glNotifyMappedBufferRangeVMWX = &noopGlNotifyMappedBufferRangeVMWX;


static const GLenum target = GL_ARRAY_BUFFER;


static void
testBufferStorage(void)
{

    if (!GLAD_GL_VERSION_4_4 &&
        !glfwExtensionSupported("GL_ARB_buffer_storage")) {
        fprintf(stderr, "error: GL_ARB_buffer_storage not supported\n");
        glfwTerminate();
        exit(EXIT_SKIP);
    }

    GLbitfield map_trace_explicit_bit = 0;
    if (glfwExtensionSupported("GL_VMWX_map_buffer_debug")) {
        glNotifyMappedBufferRangeVMWX = (PFNGLNOTIFYMAPPEDBUFFERRANGEVMWXPROC)glfwGetProcAddress("glNotifyMappedBufferRangeVMWX");
        assert(glNotifyMappedBufferRangeVMWX);
        map_trace_explicit_bit = GL_MAP_NOTIFY_EXPLICIT_BIT_VMWX;
    }

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    glBindBuffer(target, buffer);

    GLsizeiptr size = 1000;

    void *data = malloc(size);
    memset(data, 0, size);

    while ((glGetError() != GL_NO_ERROR))
        ;
    
    glBufferStorage(target, size, data,
                    GL_MAP_WRITE_BIT |
                    GL_MAP_PERSISTENT_BIT |
                    GL_MAP_COHERENT_BIT |
                    map_trace_explicit_bit);

    free(data);

    GLenum error = glGetError();
    switch (error) {
    case GL_NO_ERROR:
        break;
    case GL_OUT_OF_MEMORY:
        exit(EXIT_SKIP);
    default:
        exit(EXIT_FAILURE);
    }

    GLubyte *map;

    // straightforward mapping
    map = (GLubyte *)glMapBufferRange(target, 100, 100, GL_MAP_WRITE_BIT);
    memset(map, 1, 100);
    glUnmapBuffer(target);

    // persistent mapping w/ explicit flush
    map = (GLubyte *)glMapBufferRange(target, 200, 300, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
    memset(map + 20, 2, 30);
    glFlushMappedBufferRange(target, 20, 30);
    memset(map + 50, 3, 50);
    glFlushMappedBufferRange(target, 50, 50);
    glUnmapBuffer(target);

    // persistent & coherent mapping
    map = (GLubyte *)glMapBufferRange(target, 500, 100, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | map_trace_explicit_bit);
    memset(map + 20, 4, 30);
    glNotifyMappedBufferRangeVMWX(map + 20, 30);
    memset(map + 50, 5, 50);
    glNotifyMappedBufferRangeVMWX(map + 50, 50);
    glUnmapBuffer(target);

    glBindBuffer(target, 0);

    glDeleteBuffers(1, &buffer);
}


int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return 1;
    }

    testBufferStorage();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

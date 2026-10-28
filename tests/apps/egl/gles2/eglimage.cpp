/**************************************************************************
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
 * Draws a 4x4 texture that is an EGLImage of another texture, so that the
 * trace has to carry the contents of the image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <glad/glad.h>

#define GLFW_EXPOSE_NATIVE_EGL
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <EGL/eglext.h>


static GLFWwindow* window = NULL;

static const int size = 4;


static void
fail(const char *msg)
{
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}


static GLuint
compile(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    GLint stat = 0;

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
    if (!stat) {
        char log[1000];
        glGetShaderInfoLog(shader, sizeof log, NULL, log);
        fprintf(stderr, "%s\n", log);
        fail("failed to compile shader");
    }
    return shader;
}


int
main(int argc, char **argv)
{
    bool integer = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "rgba8") == 0) {
            integer = false;
        } else if (strcmp(argv[i], "rgba8ui") == 0) {
            integer = true;
        } else {
            fprintf(stderr, "error: unexpected arg %s\n", argv[i]);
            return 1;
        }
    }

    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(64, 64, argv[0], NULL, NULL);
    if (!window)
        fail("failed to create window");
    glfwMakeContextCurrent(window);
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
        fail("failed to load GLES");

    PFNEGLCREATEIMAGEKHRPROC createImage =
        (PFNEGLCREATEIMAGEKHRPROC)glfwGetProcAddress("eglCreateImageKHR");
    PFNEGLDESTROYIMAGEKHRPROC destroyImage =
        (PFNEGLDESTROYIMAGEKHRPROC)glfwGetProcAddress("eglDestroyImageKHR");
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC targetTexture =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)glfwGetProcAddress("glEGLImageTargetTexture2DOES");
    const char *eglExts = eglQueryString(glfwGetEGLDisplay(), EGL_EXTENSIONS);
    const char *glExts = (const char *)glGetString(GL_EXTENSIONS);
    if (!createImage || !destroyImage || !targetTexture ||
        !eglExts || !strstr(eglExts, "EGL_KHR_gl_texture_2D_image") ||
        !glExts || !strstr(glExts, "GL_OES_EGL_image")) {
        fprintf(stderr, "error: EGL_KHR_gl_texture_2D_image or GL_OES_EGL_image not supported\n");
        return EXIT_SKIP;
    }

    /* Texel (x, y) of the image, which the reference snapshot is made from. */
    GLubyte texels[size * size][4];
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            GLubyte *texel = texels[y * size + x];
            texel[0] = x * 80;
            texel[1] = y * 80;
            texel[2] = (x + y) * 30;
            texel[3] = 255;
        }
    }

    GLuint source;
    glGenTextures(1, &source);
    glBindTexture(GL_TEXTURE_2D, source);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, integer ? GL_RGBA8UI : GL_RGBA8, size, size, 0,
                 integer ? GL_RGBA_INTEGER : GL_RGBA, GL_UNSIGNED_BYTE, texels);

    /* the image's pixels are the reference, which creating it leaves undefined unless preserved */
    const EGLint attribs[] = { EGL_GL_TEXTURE_LEVEL_KHR, 0, EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
    EGLImageKHR image = createImage(glfwGetEGLDisplay(), glfwGetEGLContext(window),
                                    EGL_GL_TEXTURE_2D_KHR,
                                    (EGLClientBuffer)(uintptr_t)source, attribs);
    if (image == EGL_NO_IMAGE_KHR)
        fail("failed to create image");

    GLuint target;
    glGenTextures(1, &target);
    glBindTexture(GL_TEXTURE_2D, target);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    targetTexture(GL_TEXTURE_2D, image);
    if (glGetError() != GL_NO_ERROR)
        fail("failed to bind image to texture");

    static const char *vertShaderText =
        "#version 300 es\n"
        "in vec2 pos;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 0.0, 1.0);\n"
        "}\n";
    static const char *fragShaderText[2] = {
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform highp sampler2D image;\n"
        "uniform int cell;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = texelFetch(image, ivec2(gl_FragCoord.xy) / cell, 0);\n"
        "}\n",
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform highp usampler2D image;\n"
        "uniform int cell;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(texelFetch(image, ivec2(gl_FragCoord.xy) / cell, 0)) / 255.0;\n"
        "}\n",
    };

    GLuint program = glCreateProgram();
    glAttachShader(program, compile(GL_VERTEX_SHADER, vertShaderText));
    glAttachShader(program, compile(GL_FRAGMENT_SHADER, fragShaderText[integer]));
    glBindAttribLocation(program, 0, "pos");
    glLinkProgram(program);
    GLint stat = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &stat);
    if (!stat) {
        char log[1000];
        glGetProgramInfoLog(program, sizeof log, NULL, log);
        fprintf(stderr, "%s\n", log);
        fail("failed to link program");
    }
    glUseProgram(program);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glUniform1i(glGetUniformLocation(program, "image"), 0);
    glUniform1i(glGetUniformLocation(program, "cell"), width / size);

    static const GLfloat verts[4][2] = {
        { -1.0f, -1.0f },
        {  1.0f, -1.0f },
        { -1.0f,  1.0f },
        {  1.0f,  1.0f },
    };

    glClear(GL_COLOR_BUFFER_BIT);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);

    destroyImage(glfwGetEGLDisplay(), image);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

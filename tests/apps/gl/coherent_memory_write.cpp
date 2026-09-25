/*
 * Copyright © 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*
 * \file coherent_memory_write.cpp
 *
 * Test verifies correct work of writing into coherent memory
 * while tracing it with Apitrace
 *
 * \author Andrii Kryvytskyi <andrii.o.kryvytskyi@globallogic.com>
 */

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static const GLchar* vert_shader_text =
    "#version 430 core\n"
    "\n"
    "layout (location = 0) in vec3 pos;"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(pos, 1);\n"
    "}\n";

static const GLchar* frag_shader_text =
    "#version 430 core\n"
    "\n"
    "layout(pixel_center_integer) in vec4 gl_FragCoord;\n"
    "layout(std430, binding = 0) buffer ssbo_r { float color_r[65536]; };\n"
    "layout(std430, binding = 1) buffer ssbo_g { float color_g[65536]; };\n"
    "layout(std430, binding = 2) buffer ssbo_b { float color_b[65536]; };\n"
    "layout(location = 0) uniform int width;\n"
    "layout(location = 1) uniform int offset_x;\n"
    "layout(location = 2) uniform int offset_y;\n"
    "\n"
    "out vec4 fragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   int index = int(gl_FragCoord.x - offset_x + width * (gl_FragCoord.y - offset_y));\n"
    "   float c_r = color_r[index];\n"
    "   float c_g = color_g[index];\n"
    "   float c_b = color_b[index];\n"
    "   fragColor = vec4(c_r, c_g, c_b, 1.0f);\n"
    "}\n";

static GLuint prog;
static GLuint fbo;
static GLuint rbo;
static GLuint vbo;
static GLuint vao;
static GLuint ssbo[3];
static void* coherent_colors[3];
static void* coherent_vertices;

static size_t
getSystemPageSize() {
#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

static void
check_error() {
    GLenum error = glGetError();

    switch (error) {
    case GL_NO_ERROR:
        break;
    case GL_OUT_OF_MEMORY:
        exit(EXIT_SKIP);
    default:
        exit(EXIT_FAILURE);
    }
}

static void
build_programm() {
    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vert_shader_text, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
    }

    GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &frag_shader_text, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed:%s\n", infoLog);
    }

    prog = glCreateProgram();
    glAttachShader(prog, vertexShader);
    glAttachShader(prog, fragmentShader);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(prog, 512, NULL, infoLog);
        printf("Program linking failed: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    assert(prog);
}

static bool
setup_buffers(const int width, const int height, const size_t mapping_offset,
    const GLenum mapping_flags) {

    glGenBuffers(3, ssbo);
    for (int i = 0; i < 3; i++) {
        const GLint size = width * height * sizeof(float);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i, ssbo[i], mapping_offset,
            size);

        glBufferStorage(GL_SHADER_STORAGE_BUFFER, size + mapping_offset, NULL,
            mapping_flags);

        coherent_colors[i] = glMapNamedBufferRange(ssbo[i], mapping_offset, size,
            mapping_flags);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        check_error();
        assert(ssbo[i]);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
        rbo);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferStorage(GL_ARRAY_BUFFER, 12 * 4 * sizeof(float), NULL, mapping_flags);

    coherent_vertices = glMapBufferRange(GL_ARRAY_BUFFER, 0,
        12 * 4 * sizeof(float), mapping_flags);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    check_error();

    return true;
}

static void
generate_colors(const int width, const int height, const int start_x,
    const int start_y, const int offset_x, const int offset_y) {
    float* color_r = reinterpret_cast<float*>(coherent_colors[0]);
    float* color_g = reinterpret_cast<float*>(coherent_colors[1]);
    float* color_b = reinterpret_cast<float*>(coherent_colors[2]);

    for (int y = start_y; y < start_y + height / 2; y++) {
        for (int x = start_x; x < start_x + width / 2; x++) {
            int index = x - offset_x + (y - offset_y) * width;

            color_r[index] = x / static_cast<float>(width);
            color_g[index] = y / static_cast<float>(height);
            color_b[index] = (x + y * width) / static_cast<float>(width * height);
        }
    }
}

static void
generate_vertices(const int width, const int height) {
    float* vertices = reinterpret_cast<float*>(coherent_vertices);

    const int quads_positions[4][2] = {
        { width / -2, height / -2 },
        { 0,          height / -2 },
        { width / -2, 0           },
        { 0,          0           }
    };

    int index = 0;
    for (int i = 0; i < 4; i++) {
        int start_x = quads_positions[i][0];
        int start_y = quads_positions[i][1];

        const int coords[4][2] = {
            { start_x,             start_y              },
            { start_x,             start_y + height / 2 },
            { start_x + width / 2, start_y              },
            { start_x + width / 2, start_y + height / 2 }
        };

        for (int i = 0; i < 4; i++) {
            vertices[index++] = coords[i][0] / static_cast<float>(width / 2);
            vertices[index++] = coords[i][1] / static_cast<float>(height / 2);
            vertices[index++] = 0.0f;
        }
    }
}

static void
draw_framebuffer(GLFWwindow* window, const int width, const int height,
    const int quad_offset_x, const int quad_offset_y) {
    GLsync fence;
    glUseProgram(prog);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const int quads_positions[4][2] = {
        { width / -2, height / -2 },
        { 0,          height / -2 },
        { width / -2, 0           },
        { 0,          0           }
    };

    GLint loc = glGetUniformLocation(prog, "width");
    if (loc != -1) {
        glUniform1i(loc, width);
    }

    generate_vertices(width, height);

    int offset_x, offset_y = 0;
    int start_x, start_y;
    for (int i = 0; i < 4; i++) {
        start_x = quads_positions[i][0] + width / 2;
        start_y = quads_positions[i][1] + height / 2;

        //As test operates only with four quads there is no need to
        //create offset for every quad indepentently
        offset_x = start_x == 0 ? 0 : quad_offset_x;
        offset_y = start_y == 0 ? 0 : quad_offset_y;

        GLint offset_x_loc = glGetUniformLocation(prog, "offset_x");
        if (offset_x_loc != -1) {
            glUniform1i(offset_x_loc, offset_x);
        }

        GLint offset_y_loc = glGetUniformLocation(prog, "offset_y");
        if (offset_y_loc != -1) {
            glUniform1i(offset_y_loc, offset_y);
        }

        generate_colors(width, height, start_x, start_y, offset_x, offset_y);

        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);

        if (offset_x == 0 || offset_y == 0) {
            fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT,
                GL_TIMEOUT_IGNORED);
        }
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, 1024, 1024,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

static void
draw(GLFWwindow* window, const int width, const int height,
    const size_t mapping_offset, const int quad_offset_x,
    const int quad_offset_y, const GLenum mapping_flags) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    for (int i = 0; i < 3; i++) {
        const GLint size = width * height * sizeof(float);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i, ssbo[i],
            mapping_offset, size);
    }

    draw_framebuffer(window, width, height, quad_offset_x, quad_offset_y);

    for (int i = 0; i < 3; i++) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

static bool
probe(const int width, const int height) {

    bool pass = true;

    GLfloat* pixels = new GLfloat[width * height * 4];
    GLfloat expected[4];

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            expected[0] = x / static_cast<float>(width);
            expected[1] = y / static_cast<float>(height);
            expected[2] = (x + y * width) / static_cast<float>(width * height);
            expected[3] = 1.0f;

            int index = (x + y * width) * 4;

            if (fabsf(pixels[index + 0] - expected[0]) > 0.01 ||
                fabsf(pixels[index + 1] - expected[1]) > 0.01 ||
                fabsf(pixels[index + 2] - expected[2]) > 0.01 ||
                fabsf(pixels[index + 3] - expected[3]) > 0.01) {
                pass = false && pass;
            }
        }
    }

    delete[] pixels;
    return pass;
}

static void
unmap_buffers(const int width, const int height, const size_t mapping_offset) {
    for (int i = 0; i < 3; i++) {
        const GLint size = width * height * sizeof(float);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i, ssbo[i],
            mapping_offset, size);

        glUnmapNamedBuffer(ssbo[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, 0);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int
main(int argc, char** argv) {
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Coherent memory write", NULL, NULL);

    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return EXIT_FAILURE;
    }

    GLint alignment;
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);
    const size_t page_size = getSystemPageSize();
    const size_t ssbo_alignment = static_cast<const size_t>(alignment);

    bool pass = true;
    const int width = 256;
    const int height = 256;

    const int mapping_offsets_count = 9;
    const size_t mapping_offsets[mapping_offsets_count] = {
        0,
        ssbo_alignment,
        page_size / 2,
        page_size,
        page_size + ssbo_alignment,
        page_size - ssbo_alignment,
        page_size * 2,
        page_size * 10,
        page_size * 4 + 2 * ssbo_alignment
    };

    const int quads_offstes_count = 3;
    const int quads_offsets[quads_offstes_count][2] = {
        { width / 4, height / 4 },
        { width / 2, height / 2 },
        { 0, 0 }
    };

    const int mapping_flags_count = 2;
    const GLenum mapping_flags[mapping_flags_count] = {
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
        GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
    };

    build_programm();

    for (int i = 0; i < mapping_flags_count; i++) {
        for (int j = 0; j < mapping_offsets_count; j++) {
            pass = setup_buffers(width, height, mapping_offsets[j], mapping_flags[i]);

            if (!pass) {
                return EXIT_FAILURE;
            }

            for (int k = 0; k < quads_offstes_count; k++) {
                draw(window, width, height, mapping_offsets[j],
                    quads_offsets[k][0], quads_offsets[k][1],
                    mapping_flags[i]);

                pass = probe(width, height) && pass;
            }
            unmap_buffers(width, height, mapping_offsets[j]);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    if (pass) {
        printf("Passed successfully \n");
    } else {
        printf("Probe failed \n");
    }

    return pass ? 0 : EXIT_FAILURE;
}

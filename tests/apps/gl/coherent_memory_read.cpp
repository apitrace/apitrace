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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*
 * \file coherent_memory_read.cpp
 *
 * The main purpose of this test is to verify the correctnes of reading
 * of the coherent memory after it was written to from GPU.
 *
 * \author Illia Iorin <illia.iorin@globallogic.com>
 */


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

uint32_t getSystemPageSize()
{
#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

struct test_case
{
    uint32_t byte_stride;
    uint32_t byte_offset;
} test_cases[7];

struct coherent_data
{
    GLuint ssbo_id;
    int* data;
};

static const int values_count = 45000;
static const int fill_value_a = 0b01010101010101010101010101010101;
static const int fill_value_b = 0b11010111110101111101011111010111;
static GLuint prog;

static const GLchar* compute_shader_text = "#version 430 core\n"
      "#extension GL_ARB_uniform_buffer_object : require\n"
      "layout(local_size_x = 1000) in; \n"
      "\n"
      "layout(location = 0) uniform uint stride; \n"
      "layout(location = 1) uniform uint start_bound; \n"
      "layout(location = 2) uniform uint end_bound; \n"
      "layout(location = 3) uniform int value; \n"
      "\n"
      "layout(std430, binding=0) buffer Fragments { int data[]; }; \n"
      "\n"
      "void main()\n"
      "{\n"
      "   if (gl_GlobalInvocationID.x >= start_bound &&"
      " gl_GlobalInvocationID.x < end_bound) \n"
      "   {\n"
      "       if (mod(gl_GlobalInvocationID.x, stride) == 0)\n"
      "           data[gl_GlobalInvocationID.x] = value; \n"
      "   }\n"
      "}\n";

void check_error(void)
{
    GLenum error = glGetError();
    switch (error)
    {
        case GL_NO_ERROR:
            break;
        case GL_OUT_OF_MEMORY:
            exit(EXIT_SKIP);
        default:
            exit(EXIT_FAILURE);
    }
}

static void require_extension(const char* extension, int gl_version = GLAD_GL_VERSION_4_4)
{
    if (!gl_version && !glfwExtensionSupported("extension"))
    {
        fprintf(stderr, "error: %s not supported\n", extension);
        glfwTerminate();
        exit(EXIT_SKIP);
    }
}

static void build_programm(void)
{
    prog = glCreateProgram();
    int computeShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(computeShader, 1, &compute_shader_text, NULL);
    glCompileShader(computeShader);

    int success;
    char infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n", infoLog);
    }

    glAttachShader(prog, computeShader);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(prog, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED %s\n", infoLog);
    }

    assert(prog);
}

static GLuint init_ssbo(void)
{
    GLuint data;

    glGenBuffers(1, &data);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, data);

    glBufferStorage(GL_SHADER_STORAGE_BUFFER, values_count * 4, nullptr,
        GL_MAP_READ_BIT |
        GL_MAP_WRITE_BIT |
        GL_MAP_PERSISTENT_BIT |
        GL_MAP_COHERENT_BIT);

    int* coherent_memory = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
        values_count * 4, GL_MAP_READ_BIT |
        GL_MAP_WRITE_BIT |
        GL_MAP_PERSISTENT_BIT |
        GL_MAP_COHERENT_BIT));

    memset(coherent_memory, fill_value_a, values_count * 4);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    check_error();

    return data;
}

void fence()
{
    GLsync fence;
    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
}

void init_test_cases()
{
    const uint32_t page_size = getSystemPageSize();
    test_cases[0] = {2,         0};
    test_cases[1] = {page_size, page_size };
    test_cases[2] = {page_size, page_size - 4 };
    test_cases[3] = {page_size, page_size + 4 };
    test_cases[4] = {page_size, page_size * 2 };
    test_cases[5] = {page_size, page_size * 2 + 4 };
    test_cases[6] = {page_size, page_size * 2 - 4 };
}

bool probe_value_in_range(int* data, const test_case test,
    const uint32_t start_bound, const uint32_t end_bound, const int value)
{
    for (size_t i = 0; i < values_count - test.byte_offset / 4; i++)
    {
        size_t shifted_index = i + test.byte_offset / 4;
        if (shifted_index % test.byte_stride != 0 && data[i] != fill_value_b)
        {
            printf("Probe mismatch in probe_value_in_range data[%zu]: %i\n", i, data[i]);
            return false;
        }
    }
    for (size_t i = start_bound - test.byte_offset / 4; i < end_bound - test.byte_offset / 4; i++)
    {
        size_t shifted_index = i + test.byte_offset / 4;
        if (shifted_index % test.byte_stride == 0 && data[i] != value)
        {
            printf("Probe mismatch in probe_value_in_range data[%zu]: %i\n", i, data[i]);
            return false;
        }
    }

    return true;
}

bool probe_inital_state(int* data, const test_case test)
{
    for (size_t i = 0; i < values_count - test.byte_offset / 4; i++)
    {
        if (data[i] != fill_value_b)
        {
            printf("Probe mismatch in probe_inital_state data[%zu]: %i\n", i, data[i]);
            return false;
        }
    }
    return true;
}

bool probe_offseted_part_of_ssbo(const GLuint ssbo, const test_case test)
{
    if (test.byte_offset == 0)
        return true;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    void* coherent_memory = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
        test.byte_offset,
        GL_MAP_READ_BIT |
        GL_MAP_PERSISTENT_BIT |
        GL_MAP_WRITE_BIT |
        GL_MAP_COHERENT_BIT);
    int* data = reinterpret_cast<int*>(coherent_memory);

    for (size_t i = 0; i < test.byte_offset / 4; i++)
    {
        if (data[i] != fill_value_a)
        {
            printf("Probe mismatch in probe_offseted_part_of_ssbo data[%zu]: %i\n", i, data[i]);
            return false;
        }
    }
    return true;
}

void compute_exec(const GLuint ssbo, const test_case test,
    const uint32_t start_bound, const uint32_t end_bound, const int value)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glUniform1ui(glGetUniformLocation(prog, "stride"), test.byte_stride);
    glUniform1ui(glGetUniformLocation(prog, "start_bound"),
        std::max(test.byte_offset, start_bound));
    glUniform1ui(glGetUniformLocation(prog, "end_bound"), end_bound);
    glUniform1i(glGetUniformLocation(prog, "value"), value);

    glDispatchCompute(45, 1, 1);
}

int* map_ssbo(const GLuint ssbo, const uint32_t byte_offset)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    void* coherent_memory = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, byte_offset,
        values_count * 4 - byte_offset,
        GL_MAP_READ_BIT |
        GL_MAP_WRITE_BIT |
        GL_MAP_PERSISTENT_BIT |
        GL_MAP_COHERENT_BIT);

    memset(coherent_memory, fill_value_b, values_count * 4 - byte_offset);

    return reinterpret_cast<int*>(coherent_memory);
}

/*
How the test works:

"A" = 0b01010101010101010101010101010101
"B" = 0b11010111110101111101011111010111
"X" = 0b00101010101010101010101010101010
"Y" = 0b01100110010001010111010010101010

Part of a buffer before the offset is filled with the pattern "A" after it with the pattern "B".
Buffers are mapped with this offset and written from the CS with the same offset.
We sync after the each stage and check for the values correctness.
We check that the values before the offset aren't corrupted after all computes.

 buff_a    buff_b    buff_c
+------+  +------+  +------+
|AAAAAA|  |AAAAAA|  |AAAAAA|
|BBBBBB|  |BBBBBB|  |BBBBBB|
|BBBBBB|  |BBBBBB|  |BBBBBB|
|BBBBBB|  |BBBBBB|  |BBBBBB|
+------+  +------+  +------+
    v        v         v
+------+  +------+  +------+
|AAAAAA|  |AAAAAA|  |AAAAAA|
|BBBBBB|  |XXXXXX|  |BBBBBB|
|BBBBBB|  |BBBBBB|  |BBBBBB|
|BBBBBB|  |BBBBBB|  |BBBBBB|
+------+  +------+  +------+
    v        v         v
+------+  +------+  +------+
|AAAAAA|  |AAAAAA|  |AAAAAA|
|XXXXXX|  |XXXXXX|  |BBBBBB|
|XXXXXX|  |XXXXXX|  |BBBBBB|
|XXXXXX|  |BBBBBB|  |BBBBBB|
+------+  +------+  +------+
    v        v         v
+------+  +------+  +------+
|AAAAAA|  |AAAAAA|  |AAAAAA|
|XXXXXX|  |XXXXXX|  |BBBBBB|
|XXXXXX|  |YYYYYY|  |XXXXXX|
|XXXXXX|  |BBBBBB|  |BBBBBB|
+------+  +------+  +------+

 */

bool test_script(test_case test)
{
    bool pass = true;
    coherent_data buff_a, buff_b, buff_c;
    const uint32_t one_third = values_count / 3;
    const uint32_t two_third = 2 * one_third;
    const int32_t modified_value =   0b0101010101010101010101010101010;
    const int32_t remodified_value = 0b1100110010001010111010010101010;

    buff_a.ssbo_id = init_ssbo();
    buff_a.data = map_ssbo(buff_a.ssbo_id, test.byte_offset);

    buff_b.ssbo_id = init_ssbo();
    buff_b.data = map_ssbo(buff_b.ssbo_id, test.byte_offset);

    buff_c.ssbo_id = init_ssbo();
    buff_c.data = map_ssbo(buff_c.ssbo_id, test.byte_offset);


    pass &= probe_inital_state(buff_a.data, test);
    pass &= probe_inital_state(buff_b.data, test);
    pass &= probe_inital_state(buff_c.data, test);

    glUseProgram(prog);

    /* Stage 1 */
    {
        compute_exec(buff_b.ssbo_id, test, 0, one_third, modified_value);
        fence();

        pass &= probe_value_in_range(buff_b.data, test, 0, one_third, modified_value);
        pass &= probe_inital_state(buff_a.data, test);
        pass &= probe_inital_state(buff_c.data, test);
    }

    /* Stage 2 */
    {
        compute_exec(buff_a.ssbo_id, test, 0, values_count, modified_value);
        compute_exec(buff_b.ssbo_id, test, one_third, two_third, modified_value);
        fence();

        pass &= probe_value_in_range(buff_a.data, test, 0, values_count, modified_value);
        pass &= probe_value_in_range(buff_b.data, test, 0, two_third, modified_value);
        pass &= probe_inital_state(buff_c.data, test);
    }

    /* Stage 3 */
    {
        compute_exec(buff_a.ssbo_id, test, 0, values_count, modified_value);
        compute_exec(buff_b.ssbo_id, test, one_third, two_third, remodified_value);
        compute_exec(buff_c.ssbo_id, test, one_third, two_third, modified_value);
        fence();

        pass &= probe_value_in_range(buff_a.data, test, 0, values_count, modified_value);
        pass &= probe_value_in_range(buff_b.data, test, 0, one_third, modified_value);
        pass &= probe_value_in_range(buff_b.data, test, one_third, two_third, remodified_value);
        pass &= probe_value_in_range(buff_c.data, test, one_third, two_third, modified_value);
    }

    /* Check that the values outside the maped bounds aren't modified */
    pass &= probe_offseted_part_of_ssbo(buff_a.ssbo_id, test);
    pass &= probe_offseted_part_of_ssbo(buff_b.ssbo_id, test);
    pass &= probe_offseted_part_of_ssbo(buff_c.ssbo_id, test);

    return pass;
}
int main(int argc, char** argv)
{
    bool pass = true;

    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Coherent memory reading", NULL, NULL);
    if (!window)
    {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return EXIT_FAILURE;
    }

    require_extension("GL_ARB_uniform_buffer_object");
    require_extension("GL_ARB_buffer_storage");
    require_extension("GL_ARB_map_buffer_range");
    require_extension("GL_VMWX_map_buffer_debug");

    build_programm();

    init_test_cases();
    for (size_t i = 0; i < 7; i++)
    {
        pass = test_script(test_cases[i]) && pass;
        if (!pass)
        {
            printf("Test failed  with offset: %u stride: %u \n",
                test_cases[i].byte_offset, test_cases[i].byte_stride);
            break;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return pass ? 0 : EXIT_FAILURE;
}
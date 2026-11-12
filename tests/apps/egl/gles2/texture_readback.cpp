/* Deterministic inputs for GLES texture dump-state readback. */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Format {
    const char *name;
    GLenum internal;
    GLenum format;
    GLenum type;
    unsigned channels;
    unsigned bytes;
    bool floating;
    bool integer;
    bool sign;
};

static const Format formats[] = {
    {"rgba8", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, 1, false, false, false},
    {"r8", GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1, 1, false, false, false},
    {"rg8", GL_RG8, GL_RG, GL_UNSIGNED_BYTE, 2, 1, false, false, false},
    {"rgba16f", GL_RGBA16F, GL_RGBA, GL_FLOAT, 4, 4, true, false, false},
    {"r16f", GL_R16F, GL_RED, GL_FLOAT, 1, 4, true, false, false},
    {"rg16f", GL_RG16F, GL_RG, GL_FLOAT, 2, 4, true, false, false},
    {"rgba32f", GL_RGBA32F, GL_RGBA, GL_FLOAT, 4, 4, true, false, false},
    {"r32f", GL_R32F, GL_RED, GL_FLOAT, 1, 4, true, false, false},
    {"rg32f", GL_RG32F, GL_RG, GL_FLOAT, 2, 4, true, false, false},
    {"rgba8ui", GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, 4, 1, false, true, false},
    {"rgba16ui", GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, 4, 2, false, true, false},
    {"rgba32ui", GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, 4, 4, false, true, false},
    {"rgba8i", GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE, 4, 1, false, true, true},
    {"rgba16i", GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT, 4, 2, false, true, true},
    {"rgba32i", GL_RGBA32I, GL_RGBA_INTEGER, GL_INT, 4, 4, false, true, true},
    {"r32ui", GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, 1, 4, false, true, false},
    {"rg32i", GL_RG32I, GL_RG_INTEGER, GL_INT, 2, 4, false, true, true},
    {"rgb32f", GL_RGB32F, GL_RGB, GL_FLOAT, 3, 4, true, false, false},
    {"depth", GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 1, 2, false, false, false},
    {"depth-stencil", GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 1, 4, false, false, false},
};

static void
checkError(const char *where)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "%s: GL error 0x%x\n", where, error);
        exit(EXIT_FAILURE);
    }
}

static std::vector<unsigned char>
pixels(const Format &fmt, int width, int height, int depth, int face, int level)
{
    static const float floats[] = {-2, -0.5f, 0, 0.25f, 1, 2, 16, 64};
    static const uint16_t halves[] = {0xc000, 0xb800, 0, 0x3400, 0x3c00, 0x4000, 0x4c00, 0x5400};
    static const uint32_t uints[] = {0, 1, 127, 254, 255, 256, 65535, UINT32_MAX};
    static const int32_t ints[] = {INT32_MIN, -128, -1, 0, 1, 127, 255, 256, INT32_MAX};
    std::vector<unsigned char> data((size_t)width * height * depth * fmt.channels * fmt.bytes);
    size_t offset = 0;
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (unsigned c = 0; c < fmt.channels; ++c) {
                    unsigned i = x + 3 * y + 11 * z + 17 * face + 23 * level + c;
                    if (fmt.floating) {
                        if (fmt.bytes == 2) {
                            memcpy(&data[offset], &halves[i % 8], sizeof(uint16_t));
                        } else {
                            float value = floats[i % 8];
                            memcpy(&data[offset], &value, sizeof value);
                        }
                    } else if (fmt.integer && fmt.sign) {
                        int64_t limit = INT64_C(1) << (fmt.bytes * 8 - 1);
                        int32_t value = (int32_t)std::max(-limit, std::min(limit - 1, (int64_t)ints[i % 9]));
                        if (fmt.bytes == 1) { int8_t v = (int8_t)value; memcpy(&data[offset], &v, 1); }
                        if (fmt.bytes == 2) { int16_t v = (int16_t)value; memcpy(&data[offset], &v, 2); }
                        if (fmt.bytes == 4) { memcpy(&data[offset], &value, 4); }
                    } else {
                        uint32_t limit = fmt.bytes == 4 ? UINT32_MAX : (1u << (fmt.bytes * 8)) - 1;
                        uint32_t value = fmt.integer ? std::min(limit, uints[i % 8]) : (i * 7 + 31 * c) % 256;
                        if (fmt.format == GL_DEPTH_COMPONENT) {
                            static const uint16_t depths[] = {0, 16384, 32768, 65535};
                            value = depths[i % 4];
                        } else if (fmt.format == GL_DEPTH_STENCIL) {
                            value = (i % 2 ? 0xffffff00u : 0) | (i % 256);
                        }
                        if (fmt.bytes == 1) { uint8_t v = (uint8_t)value; memcpy(&data[offset], &v, 1); }
                        if (fmt.bytes == 2) { uint16_t v = (uint16_t)value; memcpy(&data[offset], &v, 2); }
                        if (fmt.bytes == 4) { memcpy(&data[offset], &value, 4); }
                    }
                    offset += fmt.bytes;
                }
            }
        }
    }
    return data;
}

static int
run(const Format &inputFormat, const char *shape, bool desktop, int version)
{
    Format fmt = inputFormat;
    if (!(desktop ? gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) :
                     gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))) {
        return EXIT_FAILURE;
    }
    fprintf(stderr, "renderer: %s; version: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    bool es2 = !desktop && version == 20;
    if (es2 && fmt.internal == GL_RGBA16F) {
        if (!GLAD_GL_OES_texture_half_float) {
            return EXIT_SKIP;
        }
        fmt.type = GL_HALF_FLOAT_OES;
        fmt.bytes = sizeof(uint16_t);
    }
    if (fmt.floating && !desktop) {
        bool wide = fmt.internal == GL_RGBA32F || fmt.internal == GL_R32F ||
                    fmt.internal == GL_RG32F || fmt.internal == GL_RGB32F;
        if (!glfwExtensionSupported("GL_EXT_color_buffer_float") &&
            (wide || !GLAD_GL_EXT_color_buffer_half_float)) {
            return EXIT_SKIP;
        }
    }
    bool cube = strcmp(shape, "cube") == 0;
    bool volume = strncmp(shape, "3d", 2) == 0;
    bool mipmap = strstr(shape, "mip") != NULL;
    bool state = strcmp(shape, "state") == 0;
    bool mixed = strncmp(shape, "mixed", 5) == 0;
    bool undefined = strcmp(shape, "undefined") == 0;
    if (volume && es2 && !GLAD_GL_OES_texture_3D) {
        return EXIT_SKIP;
    }
    if (mipmap && es2 && !glfwExtensionSupported("GL_OES_fbo_render_mipmap")) {
        return EXIT_SKIP;
    }
    if (fmt.format == GL_DEPTH_COMPONENT || fmt.format == GL_DEPTH_STENCIL) {
        if ((strcmp(shape, "depth-supported") == 0) != bool(GLAD_GL_NV_read_depth_stencil)) {
            return EXIT_SKIP;
        }
    }

    GLenum target = cube ? GL_TEXTURE_CUBE_MAP : volume ? GL_TEXTURE_3D : GL_TEXTURE_2D;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(target, texture);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int levels = cube ? 4 : mipmap ? 3 : 1;
    for (int level = 0; !undefined && level < levels; ++level) {
        int width = cube ? 8 >> level : std::max(1, 7 >> level);
        int height = cube ? width : std::max(1, 5 >> level);
        int depth = volume ? std::max(1, 3 >> level) : 1;
        for (int face = 0; face < (cube ? 6 : 1); ++face) {
            std::vector<unsigned char> data = pixels(fmt, width, height, depth, face, level);
            GLenum internal = es2 ? fmt.format : fmt.internal;
            if (volume) {
                if (es2) {
                    glTexImage3DOES(target, level, internal, width, height, depth, 0, fmt.format, fmt.type, data.data());
                } else {
                    glTexImage3D(target, level, internal, width, height, depth, 0, fmt.format, fmt.type, data.data());
                }
            } else {
                GLenum faceTarget = cube ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + face : target;
                glTexImage2D(faceTarget, level, internal, width, height, 0, fmt.format, fmt.type, data.data());
            }
        }
    }
    checkError("upload");

    if (es2 && fmt.floating) {
        // Validate the reference driver before testing apitrace's ES2 extension path.
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        if (volume) {
            glFramebufferTexture3DOES(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture, 0, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture, 0);
        }
        checkError("native half-float attachment");
        float sample[4] = {};
        glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, sample);
        GLenum error = glGetError();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        if (error != GL_NO_ERROR) {
            fprintf(stderr, "SKIP: native half-float FLOAT readback fails (GL error 0x%x)\n", error);
            return EXIT_SKIP;
        }
    }

    if (state || strcmp(shape, "3d") == 0) {
        // A later binding must still export after an injected failure on unit zero.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(target, texture);
        glActiveTexture(GL_TEXTURE0);
    }

    if (fmt.internal == GL_RGB32F && !desktop) {
        // Only exercise the negative case where this optional format cannot render.
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture, 0);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        if (status == GL_FRAMEBUFFER_COMPLETE) {
            return EXIT_SKIP;
        }
    }

    if (mixed) {
        GLuint empty;
        glGenTextures(1, &empty);
        if (strcmp(shape, "mixed-after") == 0) {
            glBindTexture(GL_TEXTURE_2D, empty);
        }
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, strcmp(shape, "mixed-after") == 0 ? texture : empty);
    }
    if (state) {
        GLuint fbos[2], rbs[2], buffers[2];
        glGenFramebuffers(2, fbos);
        glGenRenderbuffers(2, rbs);
        for (int i = 0; i < 2; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 7, 5);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbs[i]);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]);
        glReadBuffer(GL_NONE);
        glGenBuffers(2, buffers);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, buffers[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 4096, NULL, GL_STATIC_READ);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffers[1]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, 4096, NULL, GL_STATIC_DRAW);
        glPixelStorei(GL_PACK_ALIGNMENT, 8);
        glPixelStorei(GL_PACK_ROW_LENGTH, 13);
        glPixelStorei(GL_PACK_SKIP_ROWS, 2);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 13);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 2);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 1);
        glActiveTexture(GL_TEXTURE2);
    }
    checkError("setup");
    glFlush();
    return EXIT_SUCCESS;
}

int
main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s es20|es30|es31|desktop FORMAT 2d|cube|3d|state|mixed|undefined\n", argv[0]);
        return EXIT_FAILURE;
    }
    const Format *fmt = NULL;
    for (const auto &candidate : formats) {
        if (strcmp(candidate.name, argv[2]) == 0) {
            fmt = &candidate;
        }
    }
    if (!fmt) {
        return EXIT_FAILURE;
    }
    bool desktop = strcmp(argv[1], "desktop") == 0;
    int version = strcmp(argv[1], "es20") == 0 ? 20 : strcmp(argv[1], "es30") == 0 ? 30 : 31;
    if (!glfwInit()) {
        return EXIT_SKIP;
    }
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, desktop ? GLFW_OPENGL_API : GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, desktop ? 3 : version / 10);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, desktop ? 0 : version % 10);
    GLFWwindow *window = glfwCreateWindow(64, 64, argv[0], NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_SKIP;
    }
    glfwMakeContextCurrent(window);
    int result = run(*fmt, argv[3], desktop, version);
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}

/*********************************************************************
 *
 * Copyright 2012 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

/*
 * Auxiliary functions to compute the size of array/blob arguments.
 */

#ifndef _WIN32

#include <string.h>

#include <vector>

#include "os_thread.hpp"
#include "glimports.hpp"
#include "glproc.hpp"
#include "glsize.hpp"
#include "eglsize.hpp"
#include "glpack.hpp"
#include "assert.h"


static int
bisect_val(int min, int max, bool is_valid_val(int val))
{
    bool valid;

    while (1) {
        int try_val = min + (max - min + 1) / 2;

        valid = is_valid_val(try_val);
        if (min == max)
            break;

        if (valid)
            min = try_val;
        else
            max = try_val - 1;
    }

    return valid ? min : -1;
}

static bool
is_valid_width(int val)
{
    _glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, val, 1);
    return _glGetError() == GL_NO_ERROR;
}

static bool
is_valid_height(int val)
{
    _glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 1, val);
    return _glGetError() == GL_NO_ERROR;
}

static int
detect_size(int *width_ret, int *height_ret)
{
    GLint max_tex_size;
    int width;
    int height;

    max_tex_size = 0;
    _glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

    width = bisect_val(1, max_tex_size, is_valid_width);
    if (width < 0)
        return -1;

    height = bisect_val(1, max_tex_size, is_valid_height);
    if (height < 0)
        return -1;

    *width_ret = width;
    *height_ret = height;

    return 0;
}

/*
 * Framebuffer bindings to restore after the helper's own framebuffer.  Binding
 * GL_FRAMEBUFFER replaces both the draw and the read binding, so contexts that
 * have separate ones need both saved.
 */
struct framebuffer_bindings
{
    bool split;
    GLint draw;
    GLint read;
};

static void
bind_framebuffer(const glfeatures::Features &features, GLuint fbo,
                 framebuffer_bindings *saved)
{
    saved->split = features.read_framebuffer_object;
    if (saved->split) {
        _glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &saved->draw);
        _glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &saved->read);
    } else {
        _glGetIntegerv(GL_FRAMEBUFFER_BINDING, &saved->draw);
    }
    _glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

static void
restore_framebuffer(const framebuffer_bindings *saved)
{
    if (saved->split) {
        _glBindFramebuffer(GL_DRAW_FRAMEBUFFER, saved->draw);
        _glBindFramebuffer(GL_READ_FRAMEBUFFER, saved->read);
    } else {
        _glBindFramebuffer(GL_FRAMEBUFFER, saved->draw);
    }
}

static void
_eglCreateImageKHR_get_image_size(EGLImageKHR image, image_info *info,
                                  const glfeatures::Features &features)
{
    GLuint fbo = 0;
    framebuffer_bindings orig_fbo;
    GLuint texture = 0;
    GLuint orig_texture;
    GLenum status;

    _glGenFramebuffers(1, &fbo);
    bind_framebuffer(features, fbo, &orig_fbo);

    _glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&orig_texture);
    _glGenTextures(1, &texture);
    _glBindTexture(GL_TEXTURE_2D, texture);

    _glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);

    info->width = 0;
    info->height = 0;

    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, texture, 0);
    status = _glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE) {
        if (detect_size(&info->width, &info->height) != 0)
            os::log("%s: can't detect image size\n", __func__);
    } else {
        os::log("%s: error: %x\n", __func__, status);
    }

    /* Don't leak errors to the traced application. */
    (void)_glGetError();

    _glBindTexture(GL_TEXTURE_2D, orig_texture);
    _glDeleteTextures(1, &texture);

    restore_framebuffer(&orig_fbo);
    _glDeleteFramebuffers(1, &fbo);
}

/*
 * The size of the image bound to GL_TEXTURE_2D, where the context can tell
 * without the probes, which write to the image and rely on errors.
 */
static bool
query_size(const glfeatures::Profile &profile, image_info *info)
{
    /* glGetTexLevelParameteriv needs GLES 3.1 */
    if (profile.es() && !profile.versionGreaterOrEqual(3, 1))
        return false;

    _glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &info->width);
    _glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &info->height);
    return true;
}

/*
 * Pick a readback format and an internal format for the faked upload that keep
 * the image's precision and encoding, where the context can say what the image
 * holds.  Anything else is read as RGBA8.
 */
static void
choose_image_format(const glfeatures::Profile &profile, image_info *info)
{
    GLint internalformat = GL_NONE;
    GLint red_type = GL_NONE;
    GLint red_size = 0;

    info->internalformat = GL_RGBA;
    info->format = GL_RGBA;
    info->type = GL_UNSIGNED_BYTE;

    /* glGetTexLevelParameteriv needs GLES 3.1 */
    if (profile.es() && !profile.versionGreaterOrEqual(3, 1))
        return;

    _glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
    _glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_TYPE, &red_type);
    _glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &red_size);

    if (internalformat == GL_SRGB8 || internalformat == GL_SRGB8_ALPHA8) {
        info->internalformat = GL_SRGB8_ALPHA8;
    } else if (red_type == GL_FLOAT) {
        info->internalformat = red_size > 16 ? GL_RGBA32F : GL_RGBA16F;
        info->type = GL_FLOAT;
    } else if (red_type == GL_UNSIGNED_NORMALIZED && red_size == 10) {
        info->internalformat = GL_RGB10_A2;
        info->type = GL_UNSIGNED_INT_2_10_10_10_REV;
    } else if (red_type == GL_UNSIGNED_NORMALIZED && red_size == 16) {
        info->internalformat = GL_RGBA16;
        info->type = GL_UNSIGNED_SHORT;
    } else if (red_type == GL_SIGNED_NORMALIZED) {
        bool wide = red_size > 8;

        info->internalformat = wide ? GL_RGBA16_SNORM : GL_RGBA8_SNORM;
        info->type = wide ? GL_SHORT : GL_BYTE;
    } else if (red_type == GL_INT || red_type == GL_UNSIGNED_INT) {
        bool sign = red_type == GL_INT;

        info->format = GL_RGBA_INTEGER;
        if (red_size == 8) {
            info->internalformat = sign ? GL_RGBA8I : GL_RGBA8UI;
            info->type = sign ? GL_BYTE : GL_UNSIGNED_BYTE;
        } else if (red_size == 16) {
            info->internalformat = sign ? GL_RGBA16I : GL_RGBA16UI;
            info->type = sign ? GL_SHORT : GL_UNSIGNED_SHORT;
        } else if (red_size == 10 && !sign) {
            info->internalformat = GL_RGB10_A2UI;
            info->type = GL_UNSIGNED_INT_2_10_10_10_REV;
        } else {
            info->internalformat = sign ? GL_RGBA32I : GL_RGBA32UI;
            info->type = sign ? GL_INT : GL_UNSIGNED_INT;
        }
    } else {
        info->internalformat = GL_RGBA8;
    }
}

/* Narrow components read as 32-bit integers to info->type. */
static void
narrow_integers(image_info *info, const GLuint *wide)
{
    size_t count = (size_t)info->width * info->height * 4;

    switch (info->type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        for (size_t i = 0; i < count; ++i)
            ((GLubyte *)info->pixels)[i] = wide[i];
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        for (size_t i = 0; i < count; ++i)
            ((GLushort *)info->pixels)[i] = wide[i];
        break;
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        for (size_t i = 0; i < count; i += 4)
            ((GLuint *)info->pixels)[i / 4] = (wide[i] & 0x3ff) |
                                              (wide[i + 1] & 0x3ff) << 10 |
                                              (wide[i + 2] & 0x3ff) << 20 |
                                              wide[i + 3] << 30;
        break;
    }
}

static void
read_pixels(const image_info *info, bool framebuffer, GLenum type, void *pixels)
{
    if (framebuffer)
        _glReadPixels(0, 0, info->width, info->height, info->format, type, pixels);
    else
        _glGetTexImage(GL_TEXTURE_2D, 0, info->format, type, pixels);
}

/*
 * Read the bound framebuffer, or else the texture bound to GL_TEXTURE_2D, into
 * a tightly packed allocation, and whether that worked. GLES only guarantees
 * reading integer images as 32-bit integers, so narrower ones are read that
 * way and narrowed.
 */
static bool
read_image(image_info *info, const glfeatures::Features &features,
           bool framebuffer)
{
    GLenum read_type = info->type;
    if (info->format == GL_RGBA_INTEGER) {
        bool sign = info->type == GL_BYTE || info->type == GL_SHORT || info->type == GL_INT;
        read_type = sign ? GL_INT : GL_UNSIGNED_INT;
    }

    free(info->pixels);
    info->size = _gl_pack_image_size(info->format, info->type, info->width,
                                     info->height, features.pack_subimage);
    /* zeroed, as reads that fail in KHR_no_error contexts leave it alone */
    info->pixels = calloc(1, info->size);
    if (!info->pixels)
        return false;

    if (read_type == info->type) {
        read_pixels(info, framebuffer, info->type, info->pixels);
        return _glGetError() == GL_NO_ERROR;
    }

    GLuint *wide = (GLuint *)calloc((size_t)info->width * info->height * 4, sizeof *wide);
    if (!wide)
        return false;
    read_pixels(info, framebuffer, read_type, wide);
    bool read = _glGetError() == GL_NO_ERROR;
    if (read)
        narrow_integers(info, wide);
    free(wide);
    return read;
}

/*
 * Read texture, bound to GL_TEXTURE_2D, through a framebuffer, or with
 * glGetTexImage where the context has it and can't render to the texture.
 * Returns whether it read the image.
 */
static bool
get_texture_2d_image(const glfeatures::Profile &profile, image_info *info,
                     const glfeatures::Features &features, GLuint texture)
{
    GLuint fbo = 0;
    framebuffer_bindings prev_fbo;
    GLenum status;
    bool read = false;

    _glGenFramebuffers(1, &fbo);
    bind_framebuffer(features, fbo, &prev_fbo);

    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texture, 0);
    status = _glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE || profile.desktop()) {
        bool framebuffer = status == GL_FRAMEBUFFER_COMPLETE;
        PixelPackState pack(features);
        /* Only the readback's own error should select the fallback. */
        (void)_glGetError();
        read = read_image(info, features, framebuffer);
        if (!read && info->format == GL_RGBA && info->type != GL_UNSIGNED_BYTE) {
            /* The context can't read this format, fall back to RGBA8. */
            info->internalformat = GL_RGBA8;
            info->type = GL_UNSIGNED_BYTE;
            read = read_image(info, features, framebuffer);
        }
    } else {
        os::log("%s: error: %d\n", __func__, status);
    }

    restore_framebuffer(&prev_fbo);
    _glDeleteFramebuffers(1, &fbo);
    return read;
}

/*
 * GLES state that a draw of viewport 0 to draw buffer 0 depends on, and that
 * the conversion changes. Viewports and scissor tests are per viewport with
 * GL_OES_viewport_array, and blending and color masks per draw buffer with
 * GLES 3.2 or GL_OES_draw_buffers_indexed, so only index 0 is touched there.
 */
struct draw_state
{
    bool viewports;
    bool buffers;
    bool core;
    bool rectangles;
    bool srgb_control;
    GLint program;
    GLint vertex_array;
    GLint array_buffer;
    GLint active_texture;
    GLint external_texture;
    GLint sampler;
    GLfloat viewport[4];
    GLboolean scissor;
    GLboolean blend;
    GLboolean cull;
    GLboolean discard;
    GLboolean srgb;
    GLint mask[4];
    GLint rectangle_mode;
    std::vector<GLint> rectangle_boxes;
};

/* indexed ones by the GLES 3.2 names, or else by the extensions' */
static GLboolean
get_enable(const draw_state *saved, GLenum cap, bool indexed)
{
    if (!indexed)
        return _glIsEnabled(cap);
    return saved->core ? _glIsEnabledi(cap, 0) : _glIsEnablediEXT(cap, 0);
}

static void
set_enable(const draw_state *saved, GLenum cap, bool indexed, GLboolean enable)
{
    if (indexed && saved->core) {
        if (enable)
            _glEnablei(cap, 0);
        else
            _glDisablei(cap, 0);
    } else if (indexed) {
        if (enable)
            _glEnableiEXT(cap, 0);
        else
            _glDisableiEXT(cap, 0);
    } else if (enable) {
        _glEnable(cap);
    } else {
        _glDisable(cap);
    }
}

static void
save_draw_state(const glfeatures::Profile &profile,
                const glfeatures::Features &features, draw_state *saved)
{
    saved->viewports = features.OES_viewport_array;
    saved->buffers = features.draw_buffers_indexed;
    saved->core = profile.versionGreaterOrEqual(3, 2);
    saved->rectangles = features.EXT_window_rectangles;
    saved->srgb_control = features.EXT_sRGB_write_control;

    _glGetIntegerv(GL_CURRENT_PROGRAM, &saved->program);
    _glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &saved->vertex_array);
    _glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &saved->array_buffer);
    _glGetIntegerv(GL_ACTIVE_TEXTURE, &saved->active_texture);
    _glActiveTexture(GL_TEXTURE0);
    _glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &saved->external_texture);
    _glGetIntegerv(GL_SAMPLER_BINDING, &saved->sampler);
    if (saved->viewports)
        _glGetFloati_vOES(GL_VIEWPORT, 0, saved->viewport);
    else
        _glGetFloatv(GL_VIEWPORT, saved->viewport);
    saved->scissor = get_enable(saved, GL_SCISSOR_TEST, saved->viewports);
    saved->blend = get_enable(saved, GL_BLEND, saved->buffers);
    saved->cull = _glIsEnabled(GL_CULL_FACE);
    saved->discard = _glIsEnabled(GL_RASTERIZER_DISCARD);
    if (saved->srgb_control)
        saved->srgb = _glIsEnabled(GL_FRAMEBUFFER_SRGB_EXT);
    /* glGetBooleani_v needs GLES 3.1, unlike GL_OES_draw_buffers_indexed */
    if (saved->buffers)
        _glGetIntegeri_v(GL_COLOR_WRITEMASK, 0, saved->mask);
    else
        _glGetIntegerv(GL_COLOR_WRITEMASK, saved->mask);
    if (saved->rectangles) {
        GLint count = 0;

        _glGetIntegerv(GL_WINDOW_RECTANGLE_MODE_EXT, &saved->rectangle_mode);
        _glGetIntegerv(GL_NUM_WINDOW_RECTANGLES_EXT, &count);
        saved->rectangle_boxes.resize(4 * count);
        for (GLint i = 0; i < count; ++i)
            _glGetIntegeri_v(GL_WINDOW_RECTANGLE_EXT, i, &saved->rectangle_boxes[4 * i]);
    }
}

static void
set_viewport(const draw_state *saved, const GLfloat *viewport)
{
    if (saved->viewports)
        _glViewportIndexedfOES(0, viewport[0], viewport[1], viewport[2], viewport[3]);
    else
        _glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

static void
set_color_mask(const draw_state *saved, const GLint *mask)
{
    if (saved->buffers && saved->core)
        _glColorMaski(0, mask[0], mask[1], mask[2], mask[3]);
    else if (saved->buffers)
        _glColorMaskiEXT(0, mask[0], mask[1], mask[2], mask[3]);
    else
        _glColorMask(mask[0], mask[1], mask[2], mask[3]);
}

static void
restore_draw_state(const draw_state *saved)
{
    _glUseProgram(saved->program);
    _glBindVertexArray(saved->vertex_array);
    _glBindBuffer(GL_ARRAY_BUFFER, saved->array_buffer);
    _glBindTexture(GL_TEXTURE_EXTERNAL_OES, saved->external_texture);
    _glBindSampler(0, saved->sampler);
    _glActiveTexture(saved->active_texture);
    set_viewport(saved, saved->viewport);
    set_enable(saved, GL_SCISSOR_TEST, saved->viewports, saved->scissor);
    set_enable(saved, GL_BLEND, saved->buffers, saved->blend);
    set_enable(saved, GL_CULL_FACE, false, saved->cull);
    set_enable(saved, GL_RASTERIZER_DISCARD, false, saved->discard);
    if (saved->srgb_control)
        set_enable(saved, GL_FRAMEBUFFER_SRGB_EXT, false, saved->srgb);
    set_color_mask(saved, saved->mask);
    if (saved->rectangles)
        _glWindowRectanglesEXT(saved->rectangle_mode,
                               (GLsizei)saved->rectangle_boxes.size() / 4,
                               saved->rectangle_boxes.data());
}

static GLuint
compile_shader(GLenum type, const char *source)
{
    GLuint shader = _glCreateShader(type);
    GLint compiled = GL_FALSE;

    _glShaderSource(shader, 1, &source, NULL);
    _glCompileShader(shader);
    _glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        _glDeleteShader(shader);
        return 0;
    }
    return shader;
}

/* samples the external texture on unit 0 over the whole viewport */
static GLuint
external_program(void)
{
    static const char *vs =
        "attribute vec2 position;\n"
        "varying vec2 coord;\n"
        "void main() {\n"
        "    coord = position * 0.5 + 0.5;\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n";
    static const char *fs =
        "#extension GL_OES_EGL_image_external : require\n"
        "precision highp float;\n"
        "uniform samplerExternalOES image;\n"
        "varying vec2 coord;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(image, coord);\n"
        "}\n";
    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vs);
    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fs);
    GLuint program = 0;
    GLint linked = GL_FALSE;

    if (vertex && fragment) {
        program = _glCreateProgram();
        _glAttachShader(program, vertex);
        _glAttachShader(program, fragment);
        _glBindAttribLocation(program, 0, "position");
        _glLinkProgram(program);
        _glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            _glDeleteProgram(program);
            program = 0;
        }
    }
    _glDeleteShader(vertex);
    _glDeleteShader(fragment);
    return program;
}

/*
 * Whether a helper draw would change what the application sees: its program
 * being flagged for deletion, which switching away from frees, or transform
 * feedback or a query that would count the draw.
 */
static bool
draw_visible(void)
{
    static const GLenum counting[] = {
        GL_SAMPLES_PASSED,
        GL_ANY_SAMPLES_PASSED,
        GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
        GL_PRIMITIVES_GENERATED,
    };
    GLint program = 0;
    GLint deleted = GL_FALSE;
    GLint feedback = GL_FALSE;
    GLint paused = GL_FALSE;
    bool visible = false;

    _glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    if (program)
        _glGetProgramiv(program, GL_DELETE_STATUS, &deleted);
    _glGetIntegerv(GL_TRANSFORM_FEEDBACK_ACTIVE, &feedback);
    _glGetIntegerv(GL_TRANSFORM_FEEDBACK_PAUSED, &paused);
    visible = deleted || (feedback && !paused);
    for (GLenum target : counting) {
        GLint query = 0;

        /* targets the context lacks raise an error and leave it 0 */
        _glGetQueryiv(target, GL_CURRENT_QUERY, &query);
        visible = visible || query;
    }
    (void)_glGetError();
    return visible;
}

/*
 * Draw an image through an external sampler into a new texture of the chosen
 * format, left bound to GL_TEXTURE_2D. Only sampling converts YUV to RGB, as
 * reading the image would give its first plane, and it reads images that the
 * context can't render to. Needs GLES 3.0, a format that isn't integer, and
 * a draw that the application can't see. Returns the texture, or 0 if it
 * can't draw.
 */
static GLuint
convert_image(const glfeatures::Profile &profile,
              const glfeatures::Features &features, EGLImageKHR image,
              const image_info *info)
{
    GLuint texture = 0;

    if (!profile.es() || !profile.versionGreaterOrEqual(3, 0) ||
        info->format == GL_RGBA_INTEGER || draw_visible())
        return 0;

    _glGenTextures(1, &texture);
    _glBindTexture(GL_TEXTURE_2D, texture);
    _glTexStorage2D(GL_TEXTURE_2D, 1, info->internalformat, info->width, info->height);

    draw_state saved;
    save_draw_state(profile, features, &saved);

    static const GLfloat triangle[] = {-1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f};
    static const GLint all[4] = {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
    const GLfloat viewport[4] = {0.0f, 0.0f, (GLfloat)info->width, (GLfloat)info->height};
    GLuint program = external_program();
    GLuint external = 0, vertex_array = 0, buffer = 0, fbo = 0;
    framebuffer_bindings prev_fbo;
    bool drawn = false;

    _glGenTextures(1, &external);
    _glBindTexture(GL_TEXTURE_EXTERNAL_OES, external);
    _glBindSampler(0, 0);
    _glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, image);
    /* one texel per pixel, with the chroma of its own block */
    _glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    _glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    _glGenVertexArrays(1, &vertex_array);
    _glBindVertexArray(vertex_array);
    _glGenBuffers(1, &buffer);
    _glBindBuffer(GL_ARRAY_BUFFER, buffer);
    _glBufferData(GL_ARRAY_BUFFER, sizeof triangle, triangle, GL_STATIC_DRAW);
    _glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    _glEnableVertexAttribArray(0);

    _glGenFramebuffers(1, &fbo);
    bind_framebuffer(features, fbo, &prev_fbo);
    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texture, 0);

    if (program && _glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        _glUseProgram(program);
        set_viewport(&saved, viewport);
        set_enable(&saved, GL_SCISSOR_TEST, saved.viewports, GL_FALSE);
        set_enable(&saved, GL_BLEND, saved.buffers, GL_FALSE);
        set_enable(&saved, GL_CULL_FACE, false, GL_FALSE);
        set_enable(&saved, GL_RASTERIZER_DISCARD, false, GL_FALSE);
        /* sRGB images are decoded by the sampler, so encode them back */
        if (saved.srgb_control)
            set_enable(&saved, GL_FRAMEBUFFER_SRGB_EXT, false, GL_TRUE);
        set_color_mask(&saved, all);
        if (saved.rectangles)
            _glWindowRectanglesEXT(GL_EXCLUSIVE_EXT, 0, NULL);
        (void)_glGetError();
        _glDrawArrays(GL_TRIANGLES, 0, 3);
        drawn = _glGetError() == GL_NO_ERROR;
    }

    restore_framebuffer(&prev_fbo);
    restore_draw_state(&saved);
    _glDeleteFramebuffers(1, &fbo);
    _glDeleteBuffers(1, &buffer);
    _glDeleteVertexArrays(1, &vertex_array);
    _glDeleteTextures(1, &external);
    _glDeleteProgram(program);
    if (!drawn)
        _glDeleteTextures(1, &texture);
    return drawn ? texture : 0;
}

struct image_info *
_EGLImageKHR_get_image_info(GLenum target, EGLImageKHR image,
                            const glfeatures::Profile &profile,
                            const glfeatures::Features &features)
{
    GLuint tex;
    GLint bound_tex = 0;
    struct image_info *info;

    info = new image_info;

    memset(info, 0, sizeof *info);

    _glGenTextures(1, &tex);
    _glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_tex);
    _glBindTexture(GL_TEXTURE_2D, tex);
    _glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);

    if (!query_size(profile, info)) {
        /*
         * Only the probes are left, which need errors that KHR_no_error
         * contexts don't raise. A pending error would fail the first one.
         */
        (void)_glGetError();
        _eglCreateImageKHR_get_image_size(image, info, features);
    }

    if (info->width && info->height) {
        choose_image_format(profile, info);
        bool read = get_texture_2d_image(profile, info, features, tex);
        if (!read) {
            GLuint converted = convert_image(profile, features, image, info);

            /* into RGBA8 where the context can't render to the chosen format */
            if (!converted && info->format != GL_RGBA_INTEGER &&
                info->internalformat != GL_RGBA8 &&
                info->internalformat != GL_SRGB8_ALPHA8) {
                info->internalformat = GL_RGBA8;
                info->format = GL_RGBA;
                info->type = GL_UNSIGNED_BYTE;
                converted = convert_image(profile, features, image, info);
            }
            if (converted) {
                read = get_texture_2d_image(profile, info, features, converted);
                _glDeleteTextures(1, &converted);
            }
        }
        if (!read) {
            /* Rather no upload than one of the first plane alone, or of pixels that weren't read. */
            os::log("%s: can't read the image\n", __func__);
            info->width = 0;
            info->height = 0;
        }
    }
    _glBindTexture(GL_TEXTURE_2D, bound_tex);
    _glDeleteTextures(1, &tex);

    /* Don't leak errors to the traced application. */
    (void)_glGetError();

    if (!info->width || !info->height) {
        _EGLImageKHR_free_image_info(info);
        return NULL;
    }
    return info;
}

void
_EGLImageKHR_free_image_info(struct image_info *info)
{
    free(info->pixels);
    delete info;
}

#endif // !defined(_WIN32)

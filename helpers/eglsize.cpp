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

/* sized for the current pack state */
static void
alloc_image(image_info *info, const glfeatures::Features &features)
{
    free(info->pixels);
    info->size = _gl_pack_image_size(info->format, info->type, info->width,
                                     info->height, features.pack_subimage);
    info->pixels = malloc(info->size);
}

static void
get_texture_2d_image(image_info *info, const glfeatures::Features &features)
{
    GLuint fbo = 0;
    framebuffer_bindings prev_fbo;
    GLint texture;
    GLenum status;

    _glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
    if (!texture)
        return;

    _glGenFramebuffers(1, &fbo);
    bind_framebuffer(features, fbo, &prev_fbo);

    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texture, 0);
    status = _glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        os::log("%s: error: %d\n", __func__, status);
    {
        PixelPackState pack(features);
        alloc_image(info, features);
        _glReadPixels(0, 0, info->width, info->height, info->format, info->type, info->pixels);
    }
    /* Don't leak errors to the traced application. */
    (void)_glGetError();

    restore_framebuffer(&prev_fbo);
    _glDeleteFramebuffers(1, &fbo);
}

struct image_info *
_EGLImageKHR_get_image_info(GLenum target, EGLImageKHR image,
                            const glfeatures::Features &features)
{
    GLuint tex;
    GLint bound_tex = 0;
    struct image_info *info;

    info = new image_info;

    memset(info, 0, sizeof *info);

    info->internalformat = GL_RGBA;
    info->format = GL_RGBA;
    info->type = GL_UNSIGNED_BYTE;

    _eglCreateImageKHR_get_image_size(image, info, features);

    _glGenTextures(1, &tex);
    _glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_tex);
    _glBindTexture(GL_TEXTURE_2D, tex);
    _glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);

    get_texture_2d_image(info, features);
    _glBindTexture(GL_TEXTURE_2D, bound_tex);
    _glDeleteTextures(1, &tex);

    return info;
}

void
_EGLImageKHR_free_image_info(struct image_info *info)
{
    free(info->pixels);
    delete info;
}

#endif // !defined(_WIN32)

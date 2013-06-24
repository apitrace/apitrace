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

/* XXX */
static inline bool
can_unpack_subimage(void) {
    return false;
}

static void
_eglCreateImageKHR_get_image_size(EGLImageKHR image, image_info *info)
{
    GLuint fbo = 0;
    GLuint orig_fbo = 0;
    GLuint texture = 0;
    GLuint orig_texture;
    GLenum status;

    _glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&orig_fbo);
    _glGenFramebuffers(1, &fbo);
    _glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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

    _glBindFramebuffer(GL_FRAMEBUFFER, orig_fbo);
    _glDeleteFramebuffers(1, &fbo);
}

static void
get_texture_2d_image(image_info *info)
{
    GLuint fbo = 0;
    GLint prev_fbo = 0;
    GLint texture;
    GLenum status;

    _glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
    if (!texture)
        return;

    _glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
    _glGenFramebuffers(1, &fbo);
    _glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texture, 0);
    status = _glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        os::log("%s: error: %d\n", __func__, status);
    _glReadPixels(0, 0, info->width, info->height, info->format, info->type, info->pixels);
    /* Don't leak errors to the traced application. */
    (void)_glGetError();

    _glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    _glDeleteFramebuffers(1, &fbo);
}

struct image_info *
_EGLImageKHR_get_image_info(GLenum target, EGLImageKHR image)
{
    GLuint tex;
    GLuint bound_tex;
    struct image_info *info;

    info = new image_info;

    memset(info, 0, sizeof *info);

    info->internalformat = GL_RGBA;
    info->format = GL_RGBA;
    info->type = GL_UNSIGNED_BYTE;

    _eglCreateImageKHR_get_image_size(image, info);

    _glGenTextures(1, &tex);
    _glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&bound_tex);
    _glBindTexture(GL_TEXTURE_2D, tex);
    _glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);

    info->size = _glTexImage2D_size(info->format, info->type, info->width, info->height);
    info->pixels = malloc(info->size);

    get_texture_2d_image(info);
    _glBindTexture(GL_TEXTURE_2D, bound_tex);
    _glDeleteBuffers(1, &tex);

    return info;
}

void
_EGLImageKHR_free_image_info(struct image_info *info)
{
    free(info->pixels);
    delete info;
}

#endif // !defined(_WIN32)


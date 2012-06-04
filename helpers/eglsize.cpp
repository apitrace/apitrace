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
#include <string.h>
#include <map>

#include "os_thread.hpp"
#include "glimports.hpp"
#include "glproc.hpp"
#include "eglsize.hpp"
#include "assert.h"


static os::recursive_mutex image_map_mutex;
static std::map<EGLImageKHR, struct image_info *>image_map;

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
    static GLint max_tex_size;
    int width;
    int height;

    if (!max_tex_size)
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

void
_eglCreateImageKHR_get_image_info(EGLImageKHR image, struct image_info *info)
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

    memset(info, sizeof *info, 0);

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

    return;
}

static struct image_info *
get_image_info(EGLImageKHR image)
{
    struct image_info *info;

    image_map_mutex.lock();
    info = image_map[image];
    image_map_mutex.unlock();

    return info;
}

void
_eglCreateImageKHR_epilog(EGLDisplay dpy, EGLContext ctx, EGLenum target,
                            EGLClientBuffer buffer, const EGLint *attrib_list,
                            EGLImageKHR image)
{
    struct image_info *info;

    info = (struct image_info *)malloc(sizeof *info);
    _eglCreateImageKHR_get_image_info(image, info);

    image_map_mutex.lock();
    assert(image_map.find(image) == image_map.end());
    image_map[image] = info;
    image_map_mutex.unlock();
}

void
_eglDestroyImageKHR_epilog(EGLImageKHR image)
{
    struct image_info *info;

    info = get_image_info(image);

    image_map_mutex.lock();
    image_map.erase(image);
    image_map_mutex.unlock();

    free(info);
}

void
get_texture_2d_image(struct image_blob *blob)
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
    _glReadPixels(0, 0, blob->info.width, blob->info.height, GL_RGBA,
                  GL_UNSIGNED_BYTE, blob->data);
    /* Don't leak errors to the traced application. */
    (void)_glGetError();

    _glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    _glDeleteFramebuffers(1, &fbo);
}

size_t
_glEGLImageTargetTexture2DOES_size(GLint target, EGLImageKHR image)
{
    struct image_info *info;
    size_t size;

    info = get_image_info(image);
    size = sizeof(struct image_blob) - 1;
    /* We always read out the pixels in RGBA format */
    size += info->width * info->height * 4;

    return size;
}

void *
_glEGLImageTargetTexture2DOES_get_ptr(GLenum target, EGLImageKHR image)
{
    GLuint tex;
    GLuint bound_tex;
    size_t image_blob_size = _glEGLImageTargetTexture2DOES_size(target, image);
    struct image_blob *blob;
    struct image_info *info;

    _glGenTextures(1, &tex);
    _glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&bound_tex);
    _glBindTexture(GL_TEXTURE_2D, tex);
    _glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
    blob = (struct image_blob *)malloc(image_blob_size);
    info = get_image_info(image);
    blob->info = *info;
    get_texture_2d_image(blob);
    _glBindTexture(GL_TEXTURE_2D, bound_tex);
    _glDeleteBuffers(1, &tex);

    return (void *)blob;
}

void
_glEGLImageTargetTexture2DOES_put_ptr(const void *buffer)
{
    free((void *)buffer);
}


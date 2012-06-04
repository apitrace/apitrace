
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
#ifndef _EGLSIZE_HPP_
#define _EGLSIZE_HPP_

struct image_info
{
    int width;
    int height;
};

struct image_blob
{
    struct image_info info;
    char data[1];
};

void
_eglDestroyImageKHR_epilog(EGLImageKHR image);

void
_eglCreateImageKHR_epilog(EGLDisplay dpy, EGLContext ctx, EGLenum target,
                            EGLClientBuffer buffer, const EGLint *attrib_list,
                            EGLImageKHR image);

size_t
_glEGLImageTargetTexture2DOES_size(GLint target, EGLImageKHR image);

void *
_glEGLImageTargetTexture2DOES_get_ptr(GLenum target, EGLImageKHR image);

void
_glEGLImageTargetTexture2DOES_put_ptr(const void *buffer);

#endif


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

#ifndef _WIN32

#include "glimports.hpp"


struct image_info
{
    GLint internalformat;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum type;
    GLsizei size;
    GLvoid * pixels;
};

struct image_info *
_EGLImageKHR_get_image_info(GLenum target, EGLImageKHR image);

void
_EGLImageKHR_free_image_info(struct image_info *info);

#endif // !defined(_WIN32)

#endif

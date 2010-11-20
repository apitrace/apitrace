/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
 * All Rights Reserved.
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

#ifndef _GL_HELPERS_HPP_
#define _GL_HELPERS_HPP_


#include <cassert>

static inline size_t
__gl_image_size(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth, GLint border) {
   size_t bits_per_channel;
   switch (type) {
   case GL_UNSIGNED_BYTE:
      bits_per_channel = 8;
      break;
   default:
      assert(0);
      bits_per_channel = 0;
      break;
   }

   size_t bits_per_pixel;
   switch (format) {
   case GL_RGB:
      bits_per_pixel = bits_per_channel * 3;
      break;
   default:
      assert(0);
      bits_per_pixel = 0;
      break;
   }

   size_t row_stride = (width*bits_per_pixel + 7)/8;

   size_t slice_stride = height*row_stride;

   return depth*slice_stride;
}

#endif /* _GL_HELPERS_HPP_ */

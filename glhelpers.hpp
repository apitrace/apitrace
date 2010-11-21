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
__gl_calllists_size(GLsizei n, GLenum type)
{
   size_t bytes;
   switch(type) {
   case GL_BYTE:
   case GL_UNSIGNED_BYTE:
      bytes = 1;
      break;
   case GL_2_BYTES:
   case GL_SHORT:
   case GL_UNSIGNED_SHORT:
      bytes = 2;
      break;
   case GL_3_BYTES:
      bytes = 3;
      break;
   case GL_4_BYTES:
   case GL_INT:
   case GL_UNSIGNED_INT:
   case GL_FLOAT:
      bytes = 4;
      break;
   default:
      assert(0);
      bytes = 1;
   }

   return n*bytes;
}

static inline size_t
__gl_image_size(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth, GLint border) {
   size_t num_channels;
   switch (format) {
   case GL_COLOR_INDEX:
   case GL_RED:
   case GL_GREEN:
   case GL_BLUE:
   case GL_ALPHA:
   case GL_INTENSITY:
   case GL_DEPTH_COMPONENT:
   case GL_STENCIL_INDEX:
      num_channels = 1;
      break;
   case GL_LUMINANCE_ALPHA:
      num_channels = 2;
      break;
   case GL_RGB:
   case GL_BGR:
      num_channels = 3;
      break;
   case GL_RGBA:
   case GL_BGRA:
      num_channels = 4;
      break;
   default:
      assert(0);
      num_channels = 0;
      break;
   }

   size_t bits_per_pixel;
   switch (type) {
   case GL_BITMAP:
      bits_per_pixel = 1;
      break;
   case GL_BYTE:
   case GL_UNSIGNED_BYTE:
      bits_per_pixel = 8 * num_channels;
      break;
   case GL_SHORT:
   case GL_UNSIGNED_SHORT:
      bits_per_pixel = 16 * num_channels;
      break;
   case GL_INT:
   case GL_UNSIGNED_INT:
   case GL_FLOAT:
      bits_per_pixel = 32 * num_channels;
      break;
   case GL_UNSIGNED_BYTE_3_3_2:
   case GL_UNSIGNED_BYTE_2_3_3_REV:
   case GL_UNSIGNED_SHORT_5_6_5:
   case GL_UNSIGNED_SHORT_5_6_5_REV:
      bits_per_pixel = 8;
      break;
   case GL_UNSIGNED_SHORT_4_4_4_4:
   case GL_UNSIGNED_SHORT_4_4_4_4_REV:
   case GL_UNSIGNED_SHORT_5_5_5_1:
   case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      bits_per_pixel = 16;
      break;
   case GL_UNSIGNED_INT_8_8_8_8:
   case GL_UNSIGNED_INT_8_8_8_8_REV:
   case GL_UNSIGNED_INT_10_10_10_2:
   case GL_UNSIGNED_INT_2_10_10_10_REV:
      bits_per_pixel = 32;
      break;
   default:
      assert(0);
      bits_per_pixel = 0;
      break;
   }

   /* FIXME: consider glPixelStore settings */

   size_t row_stride = (width*bits_per_pixel + 7)/8;

   size_t slice_stride = height*row_stride;

   return depth*slice_stride;
}

static inline size_t
__gl_bitmap_size(GLsizei width, GLsizei height) {
   return __gl_image_size(GL_COLOR_INDEX, GL_BITMAP, width, height, 1, 0);
}

#endif /* _GL_HELPERS_HPP_ */

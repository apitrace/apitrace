
/* The code for converting fp11 and fp10 values to fp32 is taken from Mesa:
 *
 * Copyright (C) 2011 Marek Olšák <maraeo@gmail.com>
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef FLOATING_POINT_CONVERSION_H
#define FLOATING_POINT_CONVERSION_H

#include <stdint.h>

#define UF11_EXPONENT_SHIFT  6
#define UF10_EXPONENT_SHIFT  5
#define F32_INFINITY         0x7f800000

inline static float
uint11ToFLoat(unsigned int val)
{
    union {
       float f;
       uint32_t ui;
    } f32;

    int exponent = (val & 0x07c0) >> UF11_EXPONENT_SHIFT;
    int mantissa = (val & 0x003f);

    f32.f = 0.0;

    if (exponent == 0) {
       if (mantissa != 0) {
          const float scale = 1.0 / (1 << 20);
          f32.f = scale * mantissa;
       }
    } else if (exponent == 31) {
       f32.ui = F32_INFINITY | mantissa;
    } else {
       float scale, decimal;
       exponent -= 15;
       if (exponent < 0) {
          scale = 1.0f / (1 << -exponent);
       } else {
          scale = (float) (1 << exponent);
       }
       decimal = 1.0f + (float) mantissa / 64;
       f32.f = scale * decimal;
    }

    return f32.f;
}

inline static float
uint10ToFLoat(unsigned int val)
{
   union {
      float f;
      uint32_t ui;
   } f32;

   int exponent = (val & 0x03e0) >> UF10_EXPONENT_SHIFT;
   int mantissa = (val & 0x001f);

   f32.f = 0.0;

   if (exponent == 0) {
      if (mantissa != 0) {
         const float scale = 1.0 / (1 << 19);
         f32.f = scale * mantissa;
      }
   } else if (exponent == 31) {
      f32.ui = F32_INFINITY | mantissa;
   } else {
      float scale, decimal;
      exponent -= 15;
      if (exponent < 0) {
         scale = 1.0f / (1 << -exponent);
      }
      else {
         scale = (float) (1 << exponent);
      }
      decimal = 1.0f + (float) mantissa / 32;
      f32.f = scale * decimal;
   }

   return f32.f;
}

#endif // FLOATING_POINT_CONVERSION_H

/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright 2008-2010 VMware, Inc.
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


#include <assert.h>
#include <math.h>

#include "image.hpp"


namespace Image {


double Image::compare(Image &ref)
{
    if (width != ref.width ||
        height != ref.height ||
        channels != ref.channels) {
        return 0.0;
    }

    const unsigned char *pSrc = start();
    const unsigned char *pRef = ref.start();

    assert(channels >= 3);

    unsigned long long error = 0;
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned  x = 0; x < width; ++x) {
            // FIXME: Ignore alpha channel until we are able to pick a visual
            // that matches the traces
            for (unsigned  c = 0; c < 3; ++c) {
                int delta = pSrc[x*channels + c] - pRef[x*channels + c];
                error += delta*delta;
            }
        }

        pSrc += stride();
        pRef += ref.stride();
    }

    double numerator = error*2 + 1;
    double denominator = height*width*3ULL*255ULL*255ULL*2;
    double quotient = numerator/denominator;

    // Precision in bits
    double precision = -log(quotient)/log(2.0);

    return precision;
}


} /* namespace Image */

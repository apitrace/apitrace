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
#include <stdint.h>

#include "image.hpp"


namespace Image {

/**
 * http://en.wikipedia.org/wiki/Netpbm_format
 * http://netpbm.sourceforge.net/doc/ppm.html
 */
void
Image::writePNM(std::ostream &os, const char *comment) const {
    assert(channels == 1 || channels >= 3);

    os << (channels == 1 ? "P5" : "P6") << "\n";
    if (comment) {
        os << "#" << comment << "\n";
    }
    os << width << " " << height << "\n";
    os << "255" << "\n";

    const unsigned char *row;

    if (channels == 1 || channels == 3) {
        for (row = start(); row != end(); row += stride()) {
            os.write((const char *)row, width*channels);
        }
    } else {
        unsigned char pixel[3] = {0, 0, 0};
        for (row = start(); row != end(); row += stride()) {
            for (unsigned x = 0; x < width; ++x) {
                for (unsigned channel = 0; channel < channels; ++channel) {
                    pixel[channel] = row[x*channels + channel];
                }
                os.write((const char *)pixel, sizeof pixel);
            }
        }
    }
}


} /* namespace Image */

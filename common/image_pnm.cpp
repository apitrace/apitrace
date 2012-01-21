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
#include <string.h>
#include <stdint.h>

#include "image.hpp"


namespace image {

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
        unsigned char *tmp = new unsigned char[width*3];
        if (channels == 4) {
            for (row = start(); row != end(); row += stride()) {
                const uint32_t *src = (const uint32_t *)row;
                uint32_t *dst = (uint32_t *)tmp;
                unsigned x;
                for (x = 0; x + 4 <= width; x += 4) {
                    /*
                     * It's much faster to access dwords than bytes.
                     *
                     * FIXME: Big-endian version.
                     */

                    uint32_t rgba0 = *src++ & 0xffffff;
                    uint32_t rgba1 = *src++ & 0xffffff;
                    uint32_t rgba2 = *src++ & 0xffffff;
                    uint32_t rgba3 = *src++ & 0xffffff;
                    uint32_t rgb0 = rgba0
                                  | (rgba1 << 24);
                    uint32_t rgb1 = (rgba1 >> 8)
                                  | (rgba2 << 16);
                    uint32_t rgb2 = (rgba2 >> 16)
                                  | (rgba3 << 8);
                    *dst++ = rgb0;
                    *dst++ = rgb1;
                    *dst++ = rgb2;
                }
                for (; x < width; ++x) {
                    tmp[x*3 + 0] = row[x*4 + 0];
                    tmp[x*3 + 1] = row[x*4 + 1];
                    tmp[x*3 + 2] = row[x*4 + 2];
                }
                os.write((const char *)tmp, width*3);
            }
        } else if (channels == 2) {
            for (row = start(); row != end(); row += stride()) {
                const unsigned char *src = row;
                unsigned char *dst = tmp;
                for (unsigned x = 0; x < width; ++x) {
                    *dst++ = *src++;
                    *dst++ = *src++;
                    *dst++ = 0;
                }
                os.write((const char *)tmp, width*3);
            }
        } else {
            assert(0);
        }
        delete [] tmp;
    }
}


} /* namespace image */

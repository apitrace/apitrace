/**************************************************************************
 *
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Meng Mengmeng <mengmeng.meng@intel.com>
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


#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "image.hpp"

extern "C" {
    #include "md5.h"
}


using namespace std;
namespace image {


void
Image::writeMD5(std::ostream &os) const {
    struct MD5Context md5c;
    MD5Init(&md5c);
    const unsigned char *row;
    unsigned len = width*bytesPerPixel;
    for (row = start(); row != end(); row += stride()) {
        MD5Update(&md5c, (unsigned char *)row, len);
    }
    unsigned char signature[16];
    MD5Final(signature, &md5c);

    char csig[33] = {'\0'};
    for(int i = 0; i < sizeof signature; i++){
        snprintf(&csig[2*i], 3, "%02X", signature[i]);
    }

    os << csig;
    os << "\n";
}


} /* namespace image */


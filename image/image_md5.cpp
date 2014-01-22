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


bool
Image::writeMD5(const char *filename) const {
    std::ofstream os(filename, std::ofstream::app);
    if (!os) {
        return false;
    }
    writeMD5(os);
    return true;
}


void
Image::writeMD5(std::ostream &os) const {
    const unsigned char * image_start_buff;
    const unsigned char * image_end_buff;

    const char *hexfmt = "%02X";
    unsigned char signature[16];
    char buf[3] = {'\0'}, csig[33] = {'\0'};
    struct MD5Context md5c;

    image_start_buff = start();
    image_end_buff = end();
    unsigned long lenth =  image_start_buff - image_end_buff;

    unsigned char * buffer = (unsigned char *)malloc(lenth * sizeof(char));
    for(int i = 0; i < height; i += 1){
        memcpy(buffer + (-stride())*i, (unsigned char *)(image_start_buff + stride()*i), -stride());
    }

    MD5Init(&md5c);
    MD5Update(&md5c, buffer, lenth);
    MD5Final(signature, &md5c);
    for(int i = 0; i < sizeof signature; i++){
        sprintf(buf, hexfmt, signature[i]);
        strcat(csig, buf);
    }

    free(buffer);

    os.write((const char *)csig, strlen(csig));
    os.write("\n", strlen("\n"));
}


} /* namespace image */


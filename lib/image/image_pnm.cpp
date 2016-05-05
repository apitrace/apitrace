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
#include <stdio.h>
#include <algorithm>

#include <fstream>

#include "image.hpp"


namespace image {

/**
 * http://en.wikipedia.org/wiki/Netpbm_format
 * http://netpbm.sourceforge.net/doc/ppm.html
 * http://netpbm.sourceforge.net/doc/pfm.html
 */
void
Image::writePNM(std::ostream &os, const char *comment) const
{
    const char *identifier;
    unsigned outChannels;

    switch (channelType) {
    case TYPE_UNORM8:
        if (channels == 1) {
            identifier = "P5";
            outChannels = 1;
        } else {
            identifier = "P6";
            outChannels = 3;
        }
        break;
    case TYPE_FLOAT:
        if (channels == 1) {
            identifier = "Pf";
            outChannels = 1;
        } else if (channels <= 3) {
            identifier = "PF";
            outChannels = 3;
        } else {
            // Non-standard extension for 4 floats
            identifier = "PX";
            outChannels = 4;
        }
        break;
    default:
        assert(0);
        return;
    }

    os << identifier << "\n";

    if (comment) {
        os << "#" << comment << "\n";
    }
    os << width << " " << height << "\n";

    if (channelType == TYPE_UNORM8) {
        os << "255" << "\n";
    } else {
        os << "1" << "\n";
    }

    const unsigned char *row;

    if (channels == outChannels) {
        /*
         * Write whole pixel spans straight from the image buffer.
         */

        for (row = start(); row != end(); row += stride()) {
            os.write((const char *)row, width*bytesPerPixel);
        }
    } else {
        /*
         * Need to add/remove channels, one pixel at a time.
         */

        unsigned char *tmp = new unsigned char[width*outChannels*bytesPerChannel];

        if (channelType == TYPE_UNORM8) {
            /*
             * Optimized path for 8bit unorms.
             */

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
        } else {
            /*
             * General path for float images.
             */

            unsigned copyChannels = std::min(channels, outChannels);

            assert(channelType == TYPE_FLOAT);

            for (row = start(); row != end(); row += stride()) {
                const float *src = (const float *)row;
                float *dst = (float *)tmp;
                for (unsigned x = 0; x < width; ++x) {
                    unsigned channel = 0;
                    for (; channel < copyChannels; ++channel) {
                        *dst++ = *src++;
                    }
                    for (; channel < outChannels; ++channel) {
                        *dst++ = 0;
                    }
                }
                os.write((const char *)tmp, width*outChannels*bytesPerChannel);
            }
        }

        delete [] tmp;
    }
}


bool
Image::writePNM(const char *filename, const char *comment) const
{
    std::ofstream os(filename, std::ofstream::binary);
    if (!os) {
        return false;
    }
    writePNM(os, comment);
    return true;
}


/**
 * Parse PNM header.
 *
 * Returns pointer to data start, or NULL on failure.
 */
const char *
readPNMHeader(const char *buffer, size_t bufferSize, PNMInfo &info)
{
    info.channels = 0;
    info.width = 0;
    info.height = 0;
    info.commentNumber = -1;

    const char *currentBuffer = buffer;
    const char *nextBuffer;

    // parse number of channels
    char c;
    int scannedChannels = sscanf(currentBuffer, "P%c\n", &c);
    if (scannedChannels != 1) { // validate scanning of channels
        // invalid channel line
        return NULL;
    }
    // convert channel token to number of channels
    switch (c) {
    case '5':
        info.channels = 1;
        info.channelType = TYPE_UNORM8;
        break;
    case '6':
        info.channels = 3;
        info.channelType = TYPE_UNORM8;
        break;
    case 'f':
        info.channels = 1;
        info.channelType = TYPE_FLOAT;
        break;
    case 'F':
        info.channels = 3;
        info.channelType = TYPE_FLOAT;
        break;
    case 'X':
        info.channels = 4;
        info.channelType = TYPE_FLOAT;
        break;
    default:
        return NULL;
    }

    // advance past channel line
    nextBuffer = (const char *) memchr((const void *) currentBuffer, '\n', bufferSize) + 1;
    bufferSize -= nextBuffer - currentBuffer;
    currentBuffer = nextBuffer;

    // skip over optional comment
    if (*currentBuffer == '#') {
       // advance past '#'
        currentBuffer += 1;
        bufferSize += 1;

        // actually try to read a number
        sscanf(currentBuffer, "%d", &info.commentNumber);

        // advance past comment line
        nextBuffer = (const char *) memchr((const void *) currentBuffer, '\n', bufferSize) + 1;
        bufferSize -= nextBuffer - currentBuffer;
        currentBuffer = nextBuffer;
    }

    // parse dimensions of image
    int scannedDimensions = sscanf(currentBuffer, "%u %u\n", &info.width, &info.height);
    if (scannedDimensions != 2) { // validate scanning of dimensions
        // invalid dimension line
        return NULL;
    }

    // advance past dimension line
    nextBuffer = (const char *) memchr((const void *) currentBuffer, '\n', bufferSize) + 1;
    bufferSize -= nextBuffer - currentBuffer;
    currentBuffer = nextBuffer;

    // skip scale factor / endianness line
    nextBuffer = (const char *) memchr((const void *) currentBuffer, '\n', bufferSize) + 1;

    // return start of image data
    return nextBuffer;
}


Image *
readPNM(const char *buffer, size_t bufferSize)
{
    PNMInfo info;

    const char *headerEnd = readPNMHeader(buffer, bufferSize, info);

    // if invalid PNM header was encountered, ...
    if (headerEnd == NULL) {
        std::cerr << "error: invalid PNM header";
        return NULL;
    }

    Image *image = new Image(info.width, info.height, info.channels, false, info.channelType);

    size_t rowBytes = info.width * image->bytesPerPixel;
    for (unsigned char *row = image->start(); row != image->end(); row += image->stride()) {
        memcpy(row, headerEnd, rowBytes);
        headerEnd += rowBytes;
    }

    return image;
}



} /* namespace image */

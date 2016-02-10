/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
 * Copyright 2011 Jose Fonseca
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


#include "state_writer.hpp"

#include <assert.h>

#include <sstream>

#include "image.hpp"


StateWriter::~StateWriter()
{
}


void
StateWriter::writeImage(image::Image *image,
                        const ImageDesc & desc)
{
    assert(image);
    if (!image) {
        writeNull();
        return;
    }

    beginObject();

    // Tell the GUI this is no ordinary object, but an image
    writeStringMember("__class__", "image");

    writeIntMember("__width__", image->width);
    writeIntMember("__height__", image->height / desc.depth);
    writeIntMember("__depth__", desc.depth);

    writeStringMember("__format__", desc.format.c_str());

    if (!image->label.empty()) {
        writeStringMember("__label__", image->label.c_str());
    }

    beginMember("__data__");
    std::stringstream ss;

    if (image->channelType == image::TYPE_UNORM8) {
        image->writePNG(ss);
    } else {
        image->writePNM(ss);
    }

    const std::string & s = ss.str();
    writeBlob(s.data(), s.size());
    endMember(); // __data__

    endObject();
}

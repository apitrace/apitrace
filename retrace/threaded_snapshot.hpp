/**************************************************************************
 *
 * Copyright (C) 2015 Collabora Ltd.
 * Author: Emmanuel Gil Peyrot <emmanuel.peyrot@collabora.com>
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

#pragma once

#include <iostream>

#include "image.hpp"
#include "os_string.hpp"
#include "thread_pool.hpp"
#include "retrace.hpp"

namespace image {
    class Image;
}

namespace retrace {
    extern int verbosity;
}


static void
actuallyWritePNG(const os::String& filename, image::Image *image) {
    // Alpha channel often has bogus data, so strip it when writing
    // PNG images to disk to simplify visualization.
    bool strip_alpha = true;

    if (image->writePNG(filename, strip_alpha) &&
        retrace::verbosity >= 0) {
        std::cout << "Wrote " << filename << "\n";
    }

    delete image;
}


/**
 * Write one snapshot at a time, blocking until it is finished.
 */
class Snapshotter
{
public:
    Snapshotter() {}
    virtual ~Snapshotter() {}

    virtual void
    writePNG(const os::String& filename, image::Image *image) {
        actuallyWritePNG(filename, image);
    }
};


/**
 * Write nb_thread snapshots at a time, to better use the available CPU resources.
 */
class ThreadedSnapshotter : public Snapshotter
{
private:
    ThreadPool pool;
    ThreadedSnapshotter() = delete;

public:
    ThreadedSnapshotter(size_t nb_threads) : pool(nb_threads) {}

    virtual void
    writePNG(const os::String& filename, image::Image *image) override {
        pool.enqueue(actuallyWritePNG, filename, image);
    }
};

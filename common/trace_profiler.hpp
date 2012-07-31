/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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

#ifndef TRACE_PROFILER_H
#define TRACE_PROFILER_H

#include <string>
#include <stdint.h>

namespace trace
{
class Profiler
{
public:
    Profiler();
    ~Profiler();

    void setup(bool cpuTimes_, bool gpuTimes_, bool pixelsDrawn_);

    void addFrameStart(unsigned no, uint64_t gpuStart, uint64_t cpuStart);
    void addFrameEnd(uint64_t gpuEnd, uint64_t cpuEnd);

    void addCall(unsigned no,
                 const char* name,
                 unsigned program,
                 uint64_t pixels,
                 uint64_t gpuStart, uint64_t gpuDuration,
                 uint64_t cpuStart, uint64_t cpuDuration);

private:
    uint64_t baseGpuTime;
    uint64_t baseCpuTime;

    bool cpuTimes;
    bool gpuTimes;
    bool pixelsDrawn;

    struct {
        unsigned no;
        uint64_t gpuStart;
        uint64_t cpuStart;
    } lastFrame;
};
}

#endif // TRACE_PROFILER_H

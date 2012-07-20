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
    struct GpuTime
    {
        GpuTime()
            : start(0), duration(0)
        {
        }

        GpuTime(uint64_t start_, uint64_t duration_)
            : start(start_), duration(duration_)
        {
        }

        uint64_t start;
        uint64_t duration;
    };

    struct Call
    {
        Call()
            : no(0)
        {
        }

        Call(unsigned no_, const char* name_, uint64_t gpu_start, uint64_t gpu_duration)
            : no(no_), name(name_), gpu(gpu_start, gpu_duration)
        {
        }

        unsigned no;
        std::string name;

        GpuTime gpu;
    };

    struct Frame
    {
        Frame()
            : no(0)
        {
        }

        Frame(unsigned no_, uint64_t gpu_start, uint64_t gpu_duration)
            : no(no_), gpu(gpu_start, gpu_duration)
        {
        }

        unsigned no;
        GpuTime gpu;
    };

public:
    Profiler();
    ~Profiler();

    void addCall(const Call& call);
    void addFrame(const Frame& frame);

private:
    uint64_t baseTime;
};
}

#endif // TRACE_PROFILER_H

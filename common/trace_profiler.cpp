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

#include "trace_profiler.hpp"
#include <iostream>

namespace trace {
Profiler::Profiler()
    : baseTime(0)
{
}

Profiler::~Profiler()
{
}

void Profiler::addCall(unsigned no, const char *name, uint64_t gpu_start, uint64_t gpu_duration)
{
    if (baseTime == 0)
        baseTime = gpu_start;

    std::cout << "call "
              << no << " "
              << (gpu_start - baseTime) << " "
              << gpu_duration << " "
              << name << std::endl;
}

void Profiler::addFrameStart(unsigned no, uint64_t timestamp)
{
    if (baseTime == 0)
        baseTime = timestamp;

    lastFrame.no = no;
    lastFrame.start = timestamp - baseTime;

    std::cout << "frame begin "
              << lastFrame.no << " "
              << lastFrame.start << std::endl;
}

void Profiler::addFrameEnd(uint64_t timestamp)
{
    if (baseTime == 0)
        baseTime = timestamp;

    lastFrame.end = timestamp - baseTime;

    std::cout << "frame end "
              << lastFrame.no << " "
              << lastFrame.end << " "
              << (lastFrame.end - lastFrame.start) << std::endl;
}
}

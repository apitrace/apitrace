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
#include <string.h>
#include <assert.h>
#include <sstream>
#include "os_time.hpp"

namespace trace {
Profiler::Profiler()
    : baseGpuTime(0),
      baseCpuTime(0),
      cpuTimes(false),
      gpuTimes(true),
      pixelsDrawn(false)
{
}

Profiler::~Profiler()
{
}

void Profiler::setup(bool cpuTimes_, bool gpuTimes_, bool pixelsDrawn_)
{
    cpuTimes = cpuTimes_;
    gpuTimes = gpuTimes_;
    pixelsDrawn = pixelsDrawn_;

    std::cout << "# frame_begin no gpu_start cpu_start" << std::endl;
    std::cout << "# frame_end no gpu_end gpu_dura cpu_end cpu_dura" << std::endl;
    std::cout << "# call no gpu_start gpu_dura cpu_start cpu_dura pixels program name" << std::endl;
}

void Profiler::setBaseTimes(uint64_t gpuStart, uint64_t cpuStart)
{
    baseCpuTime = cpuStart;
    baseGpuTime = gpuStart;
}

void Profiler::addCall(unsigned no,
                       const char *name,
                       unsigned program,
                       uint64_t pixels,
                       uint64_t gpuStart, uint64_t gpuDuration,
                       uint64_t cpuStart, uint64_t cpuDuration)
{
    if (gpuTimes) {
        gpuStart -= baseGpuTime;
    } else {
        gpuStart = 0;
        gpuDuration = 0;
    }

    if (cpuTimes) {
        double cpuTimeScale = 1.0E9 / os::timeFrequency;
        cpuStart = (cpuStart - baseCpuTime) * cpuTimeScale;
        cpuDuration = cpuDuration * cpuTimeScale;
    } else {
        cpuStart = 0;
        cpuDuration = 0;
    }

    if (!pixelsDrawn) {
        pixels = 0;
    }

    std::cout << "call"
              << " " << no
              << " " << gpuStart
              << " " << gpuDuration
              << " " << cpuStart
              << " " << cpuDuration
              << " " << pixels
              << " " << program
              << " " << name
              << std::endl;
}

void Profiler::addFrameStart(unsigned no, uint64_t gpuStart, uint64_t cpuStart)
{
    lastFrame.no = no;
    lastFrame.gpuStart = gpuStart;
    lastFrame.cpuStart = cpuStart;

    if (gpuTimes) {
        gpuStart = gpuStart - baseGpuTime;
    } else {
        gpuStart = 0;
    }

    if (cpuTimes) {
        double cpuTimeScale = 1.0E9 / os::timeFrequency;
        cpuStart = (cpuStart - baseCpuTime) * cpuTimeScale;
    } else {
        cpuStart = 0;
    }

    std::cout << "frame_begin"
              << " " << no
              << " " << gpuStart
              << " " << cpuStart
              << std::endl;
}

void Profiler::addFrameEnd(uint64_t gpuEnd, uint64_t cpuEnd)
{
    uint64_t gpuDuration, cpuDuration;

    if (gpuTimes) {
        gpuDuration = gpuEnd - lastFrame.gpuStart;
        gpuEnd = gpuEnd - baseGpuTime;
    } else {
        gpuEnd = 0;
        gpuDuration = 0;
    }

    if (cpuTimes) {
        double cpuTimeScale = 1.0E9 / os::timeFrequency;
        cpuDuration = (cpuEnd - lastFrame.cpuStart) * cpuTimeScale;
        cpuEnd = (cpuEnd - baseCpuTime) * cpuTimeScale;
    } else {
        cpuEnd = 0;
        cpuDuration = 0;
    }

    std::cout << "frame_end"
              << " " << lastFrame.no
              << " " << gpuEnd
              << " " << gpuDuration
              << " " << cpuEnd
              << " " << cpuDuration
              << std::endl;
}

void Profiler::parseLine(const char* in, Profile* profile)
{
    std::stringstream line(in, std::ios_base::in);
    std::string type;

    if (in[0] == '#' || strlen(in) < 12)
        return;

    line >> type;

    if (type.compare("call") == 0) {
        assert(profile->frames.size());
        Profile::Call call;

        line >> call.no
             >> call.gpuStart
             >> call.gpuDuration
             >> call.cpuStart
             >> call.cpuDuration
             >> call.pixels
             >> call.program
             >> call.name;

        profile->frames.back().calls.push_back(call);
    } else if (type.compare("frame_begin") == 0) {
        Profile::Frame frame;
        frame.gpuDuration = 0;
        frame.cpuDuration = 0;

        line >> frame.no
             >> frame.gpuStart
             >> frame.cpuStart;

        profile->frames.push_back(frame);
    } else if (type.compare("frame_end") == 0) {
        assert(profile->frames.size());
        Profile::Frame& frame = profile->frames.back();
        int64_t skipi64;

        line >> frame.no
             >> skipi64
             >> frame.gpuDuration
             >> skipi64
             >> frame.cpuDuration;
    }
}
}

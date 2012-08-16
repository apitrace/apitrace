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
#include "os_time.hpp"
#include <iostream>
#include <string.h>
#include <sstream>

namespace trace {
Profiler::Profiler()
    : baseGpuTime(0),
      baseCpuTime(0),
      minCpuTime(1000),
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

    std::cout << "# call no gpu_start gpu_dura cpu_start cpu_dura pixels program name" << std::endl;
}

int64_t Profiler::getBaseCpuTime()
{
    return baseCpuTime;
}

int64_t Profiler::getBaseGpuTime()
{
    return baseGpuTime;
}

void Profiler::setBaseCpuTime(int64_t cpuStart)
{
    baseCpuTime = cpuStart;
}

void Profiler::setBaseGpuTime(int64_t gpuStart)
{
    baseGpuTime = gpuStart;
}

bool Profiler::hasBaseTimes()
{
    return baseCpuTime != 0 || baseGpuTime != 0;
}

void Profiler::addCall(unsigned no,
                       const char *name,
                       unsigned program,
                       int64_t pixels,
                       int64_t gpuStart, int64_t gpuDuration,
                       int64_t cpuStart, int64_t cpuDuration)
{
    if (gpuTimes && gpuStart) {
        gpuStart -= baseGpuTime;
    } else {
        gpuStart = 0;
        gpuDuration = 0;
    }

    if (cpuTimes && cpuStart) {
        double cpuTimeScale = 1.0E9 / os::timeFrequency;
        cpuStart = (cpuStart - baseCpuTime) * cpuTimeScale;
        cpuDuration = cpuDuration * cpuTimeScale;

        if (cpuDuration < minCpuTime) {
            return;
        }
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

void Profiler::addFrameEnd()
{
    std::cout << "frame_end" << std::endl;
}

void Profiler::parseLine(const char* in, Profile* profile)
{
    std::stringstream line(in, std::ios_base::in);
    std::string type;
    static int64_t lastGpuTime;
    static int64_t lastCpuTime;

    if (in[0] == '#' || strlen(in) < 4)
        return;

    if (profile->programs.size() == 0 && profile->cpuCalls.size() == 0 && profile->frames.size() == 0) {
        lastGpuTime = 0;
        lastCpuTime = 0;
    }

    line >> type;

    if (type.compare("call") == 0) {
        Profile::DrawCall draw;
        unsigned program;

        line >> draw.no
             >> draw.gpuStart
             >> draw.gpuDuration
             >> draw.cpuStart
             >> draw.cpuDuration
             >> draw.pixels
             >> program
             >> draw.name;

        if (lastGpuTime < draw.gpuStart + draw.gpuDuration) {
            lastGpuTime = draw.gpuStart + draw.gpuDuration;
        }

        if (lastCpuTime < draw.cpuStart + draw.cpuDuration) {
            lastCpuTime = draw.cpuStart + draw.cpuDuration;
        }

        if (draw.pixels >= 0) {
            if (profile->programs.size() <= program) {
                profile->programs.resize(program + 1);
            }

            profile->programs[program].cpuTotal += draw.cpuDuration;
            profile->programs[program].gpuTotal += draw.gpuDuration;
            profile->programs[program].pixelTotal += draw.pixels;
            profile->programs[program].drawCalls.push_back(draw);
        }

        Profile::CpuCall call;
        call.no          = draw.no;
        call.name        = draw.name;
        call.cpuStart    = draw.cpuStart;
        call.cpuDuration = draw.cpuDuration;
        profile->cpuCalls.push_back(call);
    } else if (type.compare("frame_end") == 0) {
        Profile::Frame frame;
        frame.no = profile->frames.size();

        if (frame.no == 0) {
            frame.gpuStart = 0;
            frame.cpuStart = 0;
        } else {
            frame.gpuStart = profile->frames.back().gpuStart + profile->frames.back().gpuDuration;
            frame.cpuStart = profile->frames.back().cpuStart + profile->frames.back().cpuDuration;
        }

        frame.gpuDuration = lastGpuTime - frame.gpuStart;
        frame.cpuDuration = lastCpuTime - frame.cpuStart;
        profile->frames.push_back(frame);
    }
}
}

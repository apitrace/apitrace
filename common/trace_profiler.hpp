/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
 * Copyright 2013 Intel, Inc.
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
#include <vector>
#include <stdint.h>

namespace trace
{

struct Profile {
    struct Call {
        unsigned no;

        unsigned program;

        int64_t gpuStart;
        int64_t gpuDuration;

        int64_t cpuStart;
        int64_t cpuDuration;

        int64_t vsizeStart;
        int64_t vsizeDuration;
        int64_t rssStart;
        int64_t rssDuration;

        int64_t pixels;

        std::string name;
    };

    struct Frame {
        unsigned no;

        int64_t gpuStart;
        int64_t gpuDuration;

        int64_t cpuStart;
        int64_t cpuDuration;

        int64_t vsizeStart;
        int64_t vsizeDuration;
        int64_t rssStart;
        int64_t rssDuration;

        /* Indices to profile->calls array */
        struct {
            unsigned begin;
            unsigned end;
        } calls;
    };

    struct Program {
        Program() : gpuTotal(0), cpuTotal(0), pixelTotal(0) {}

        uint64_t gpuTotal;
        uint64_t cpuTotal;
        uint64_t pixelTotal;
        int64_t vsizeTotal;
        int64_t rssTotal;

        /* Indices to profile->calls array */
        std::vector<unsigned> calls;
    };

    std::vector<Call> calls;
    std::vector<Frame> frames;
    std::vector<Program> programs;
};

class Profiler
{
public:
    Profiler();
    ~Profiler();

    void setup(bool cpuTimes_, bool gpuTimes_, bool pixelsDrawn_, bool memoryUsage_);

    void addCall(unsigned no,
                 const char* name,
                 unsigned program,
                 int64_t pixels,
                 int64_t gpuStart, int64_t gpuDuration,
                 int64_t cpuStart, int64_t cpuDuration,
                 int64_t vsizeStart, int64_t vsizeDuration,
                 int64_t rssStart, int64_t rssDuration);

    void addFrameEnd();

    bool hasBaseTimes();

    void setBaseCpuTime(int64_t cpuStart);
    void setBaseGpuTime(int64_t gpuStart);
    void setBaseVsizeUsage(int64_t vsizeStart);
    void setBaseRssUsage(int64_t rssStart);

    int64_t getBaseCpuTime();
    int64_t getBaseGpuTime();
    int64_t getBaseVsizeUsage();
    int64_t getBaseRssUsage();

    static void parseLine(const char* line, Profile* profile);

private:
    int64_t baseGpuTime;
    int64_t baseCpuTime;
    int64_t minCpuTime;
    int64_t baseVsizeUsage;
    int64_t baseRssUsage;

    bool cpuTimes;
    bool gpuTimes;
    bool pixelsDrawn;
    bool memoryUsage;
};
}

#endif // TRACE_PROFILER_H

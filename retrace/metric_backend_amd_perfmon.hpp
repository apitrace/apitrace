/**************************************************************************
 *
 * Copyright 2015 Alexander Trukhin
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

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>

#include "glproc.hpp"
#include "metric_backend.hpp"
#include "glretrace.hpp"
#include "mmap_allocator.hpp"

#define NUM_MONITORS 1 // number of max used AMD_perfmon monitors

class Metric_AMD_perfmon : public Metric
{
private:
    unsigned m_group, m_id;
    MetricNumType m_nType;
    std::string m_name;
    bool m_precached;

    void precache();

public:
    Metric_AMD_perfmon(unsigned g, unsigned i) : m_group(g), m_id(i),
                                                 m_nType(CNT_NUM_UINT),
                                                 m_precached(false) {}

    GLenum size();


    unsigned id();

    unsigned groupId();

    std::string name();

    std::string description();

    MetricNumType numType();

    MetricType type();
};


class MetricBackend_AMD_perfmon : public MetricBackend
{
private:
    class DataCollector
    {
        private:
            MmapAllocator<unsigned> alloc; // allocator
            // deque with custom allocator
            template <class T>
            using mmapdeque = std::deque<T, MmapAllocator<std::deque<T>>>;
            // data storage
            mmapdeque<mmapdeque<unsigned*>> data;
            unsigned curPass;

        public:
            DataCollector(MmapAllocator<char> &alloc)
                : alloc(alloc), data(1, mmapdeque<unsigned*>(alloc), alloc),
                  curPass(0) {}

            ~DataCollector();

            unsigned* newDataBuffer(unsigned event, size_t size);

            void endPass();

            unsigned* getDataBuffer(unsigned pass, unsigned event);
    };

private:
    bool supported; // extension support (checked initially)
    bool firstRound; // first profiling round (no need to free monitors)
    bool perFrame; // profiling frames?
    unsigned monitors[NUM_MONITORS]; // For cycling
    unsigned curMonitor;
    unsigned monitorEvent[NUM_MONITORS]; // Event saved in monitor
    unsigned numPasses; // all passes
    unsigned numFramePasses; // frame passes
    unsigned curPass;
    unsigned curEvent; // Currently evaluated event
    // metrics selected for profiling boundaries (frames, draw calls)
    std::vector<Metric_AMD_perfmon> metrics[2];
    // metric sets for each pass
    std::vector<std::vector<Metric_AMD_perfmon>> passes;
    // metric offsets in data for each pass
    std::vector<std::map<Metric_AMD_perfmon*, unsigned>> metricOffsets;
    DataCollector collector; // data storage
    // lookup table (metric name -> (gid, id))
    static std::map<std::string, std::pair<unsigned, unsigned>> nameLookup;

    MetricBackend_AMD_perfmon(glretrace::Context* context, MmapAllocator<char> &alloc);

    MetricBackend_AMD_perfmon(MetricBackend_AMD_perfmon const&) = delete;

    void operator=(MetricBackend_AMD_perfmon const&)            = delete;

    // test if given set of metrics can be sampled in one pass
    bool testMetrics(std::vector<Metric_AMD_perfmon>* metrics);

    void freeMonitor(unsigned monitor); // collect metrics data from the monitor

    static void populateLookupGroups(unsigned group, int error, void* userData);

    static void populateLookupMetrics(Metric* metric, int error, void* userData);

    void generatePassesBoundary(QueryBoundary boundary);

public:
    bool isSupported();

    void enumGroups(enumGroupsCallback callback, void* userData = nullptr);

    void enumMetrics(unsigned group, enumMetricsCallback callback,
                     void* userData = nullptr);

    std::unique_ptr<Metric> getMetricById(unsigned groupId, unsigned metricId);

    std::unique_ptr<Metric> getMetricByName(std::string metricName);

    std::string getGroupName(unsigned group);

    int enableMetric(Metric* metric, QueryBoundary pollingRule = QUERY_BOUNDARY_DRAWCALL);

    unsigned generatePasses();

    void beginPass();

    void endPass();

    void beginQuery(QueryBoundary boundary = QUERY_BOUNDARY_DRAWCALL);

    void endQuery(QueryBoundary boundary = QUERY_BOUNDARY_DRAWCALL);

    void enumDataQueryId(unsigned id, enumDataCallback callback,
                         QueryBoundary boundary,
                         void* userData = nullptr);

    unsigned getNumPasses();

    static MetricBackend_AMD_perfmon& getInstance(glretrace::Context* context,
                                                  MmapAllocator<char> &alloc);
};


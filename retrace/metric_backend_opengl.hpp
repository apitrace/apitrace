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

#include <vector>
#include <string>
#include <map>
#include <queue>
#include <array>

#include "glproc.hpp"
#include "metric_backend.hpp"
#include "glretrace.hpp"
#include "mmap_allocator.hpp"

class Metric_opengl : public Metric
{
private:
    unsigned m_gId, m_id;
    std::string m_name, m_desc;
    MetricNumType m_nType;
    MetricType m_type;

public:
    Metric_opengl(unsigned gId, unsigned id, const std::string &name,
                  const std::string &desc, MetricNumType nT, MetricType t);

    unsigned id() override;

    unsigned groupId() override;

    std::string name() override;

    std::string description() override;

    MetricNumType numType() override;

    MetricType type() override;

    // should be set by backend
    bool available;
    bool profiled[QUERY_BOUNDARY_LIST_END]; // profiled in cur pass
    bool enabled[QUERY_BOUNDARY_LIST_END]; // enabled for profiling
};

class MetricBackend_opengl : public MetricBackend
{
private:
    MmapAllocator<char> alloc;
    // storage class
    class Storage
    {
    private:
        std::deque<int64_t, MmapAllocator<int64_t>> data[QUERY_BOUNDARY_LIST_END];

    public:
#ifdef _WIN32
        Storage(MmapAllocator<char> &alloc) {
            for (auto &d : data) {
                d = std::deque<int64_t, MmapAllocator<int64_t>>(alloc);
            }
        }
#else
        Storage(MmapAllocator<char> &alloc)
            : data{ std::deque<int64_t, MmapAllocator<int64_t>>(alloc),
                    std::deque<int64_t, MmapAllocator<int64_t>>(alloc),
                    std::deque<int64_t, MmapAllocator<int64_t>>(alloc) } {};
#endif
        void addData(QueryBoundary boundary, int64_t data);
        int64_t* getData(QueryBoundary boundary, unsigned eventId);
    };

    // indexes into metrics vector
    enum {
        METRIC_CPU_START = 0,
        METRIC_CPU_DURATION,
        METRIC_GPU_START,
        METRIC_GPU_DURATION,
        METRIC_GPU_DISJOINT,
        METRIC_GPU_PIXELS,
        METRIC_CPU_VSIZE_START,
        METRIC_CPU_VSIZE_DURATION,
        METRIC_CPU_RSS_START,
        METRIC_CPU_RSS_DURATION,
        METRIC_LIST_END
    };

    // indexes into queries
    enum {
        QUERY_GPU_START = 0,
        QUERY_GPU_DURATION,
        QUERY_OCCLUSION,
        QUERY_LIST_END,
    };

    // lookup tables
    std::map<std::pair<unsigned,unsigned>, Metric_opengl*> idLookup;
    std::map<std::string, Metric_opengl*> nameLookup;

    // bools
    bool supportsDisjoint, supportsTimestamp, supportsElapsed, supportsOcclusion;
    bool glQueriesNeeded[QUERY_BOUNDARY_LIST_END];
    bool glQueriesNeededAnyBoundary;
    bool cpugpuSync;
    bool twoPasses; // profiling in two passes
    bool queryInProgress[QUERY_BOUNDARY_LIST_END];

    GLint timestamp_bits_precision;

    unsigned curPass;
    int64_t curDisjoint;

    std::vector<Metric_opengl> metrics;
    // storage for metrics
    std::unique_ptr<Storage> data[METRIC_LIST_END][QUERY_BOUNDARY_LIST_END];

    // Helper vars for metrics
    std::queue<std::array<GLuint, QUERY_LIST_END>> queries[QUERY_BOUNDARY_LIST_END];
    GLint64 baseTime;
    double cpuTimeScale;
    int64_t cpuStart[QUERY_BOUNDARY_LIST_END];
    int64_t cpuEnd[QUERY_BOUNDARY_LIST_END];
    int64_t vsizeStart[QUERY_BOUNDARY_LIST_END];
    int64_t vsizeEnd[QUERY_BOUNDARY_LIST_END];
    int64_t rssStart[QUERY_BOUNDARY_LIST_END];
    int64_t rssEnd[QUERY_BOUNDARY_LIST_END];
    int64_t disjointStart[QUERY_BOUNDARY_LIST_END];

    MetricBackend_opengl(glretrace::Context* context, MmapAllocator<char> &alloc);

    MetricBackend_opengl(MetricBackend_opengl const&) = delete;

    void operator=(MetricBackend_opengl const&)       = delete;

public:
    bool isSupported() override;

    void enumGroups(enumGroupsCallback callback, void* userData = nullptr) override;

    void enumMetrics(unsigned group, enumMetricsCallback callback, void* userData = nullptr) override;

    std::unique_ptr<Metric> getMetricById(unsigned groupId, unsigned metricId) override;

    std::unique_ptr<Metric> getMetricByName(std::string metricName) override;

    std::string getGroupName(unsigned group) override;

    int enableMetric(Metric* metric, QueryBoundary pollingRule = QUERY_BOUNDARY_DRAWCALL) override;

    unsigned generatePasses() override;

    void beginPass() override;

    void endPass() override;

    void pausePass() override;

    void continuePass() override;

    void beginQuery(QueryBoundary boundary = QUERY_BOUNDARY_DRAWCALL) override;

    void endQuery(QueryBoundary boundary = QUERY_BOUNDARY_DRAWCALL) override;

    void enumDataQueryId(unsigned id, enumDataCallback callback,
                         QueryBoundary boundary, void* userData = nullptr) override;

    unsigned getNumPasses() override;

    static MetricBackend_opengl& getInstance(glretrace::Context* context,
                                             MmapAllocator<char> &alloc);


private:
    int64_t getCurrentTime(void);

    int64_t getTimeFrequency(void);

    void queryTimestamp(GLuint query);

    int64_t getQueryResult(GLuint query);

    void processQueries();
};


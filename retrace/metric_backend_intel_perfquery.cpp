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

#include "metric_backend_intel_perfquery.hpp"

void Metric_INTEL_perfquery::precache() {
    unsigned offset;
    glGetPerfCounterInfoINTEL(m_group, m_id, 0, nullptr, 0, nullptr, &offset,
                              nullptr, nullptr, nullptr, nullptr);
    this->m_offset = offset;

    GLenum type;
    glGetPerfCounterInfoINTEL(m_group, m_id, 0, nullptr, 0, nullptr, nullptr,
                              nullptr, nullptr, &type, nullptr);
    if (type == GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL) m_nType = CNT_NUM_UINT;
    else if (type == GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL) m_nType = CNT_NUM_FLOAT;
    else if (type == GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL) m_nType = CNT_NUM_DOUBLE;
    else if (type == GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL) m_nType = CNT_NUM_BOOL;
    else if (type == GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL) m_nType = CNT_NUM_UINT64;
    else m_nType = CNT_NUM_UINT;

    char name[INTEL_NAME_LENGTH];
    glGetPerfCounterInfoINTEL(m_group, m_id, INTEL_NAME_LENGTH, name, 0, nullptr,
                              nullptr, nullptr, nullptr, nullptr, nullptr);
    m_name = std::string(name);

    m_precached = true;
}

unsigned Metric_INTEL_perfquery::id() {
    return m_id;
}

unsigned Metric_INTEL_perfquery::groupId() {
    return m_group;
}

std::string Metric_INTEL_perfquery::name() {
    if (!m_precached) precache();
    return m_name;
}

std::string Metric_INTEL_perfquery::description() {
    char desc[INTEL_DESC_LENGTH];
    glGetPerfCounterInfoINTEL(m_group, m_id, 0, nullptr, INTEL_DESC_LENGTH, desc,
                              nullptr, nullptr, nullptr, nullptr, nullptr);
    return std::string(desc);
}

unsigned Metric_INTEL_perfquery::offset() {
    if (!m_precached) precache();
    return m_offset;
}

MetricNumType Metric_INTEL_perfquery::numType() {
    if (!m_precached) precache();
    return m_nType;
}

MetricType Metric_INTEL_perfquery::type() {
    GLenum type;
    glGetPerfCounterInfoINTEL(m_group, m_id, 0, nullptr, 0, nullptr, nullptr,
                              nullptr, &type, nullptr, nullptr);
    if (type == GL_PERFQUERY_COUNTER_TIMESTAMP_INTEL) return CNT_TYPE_TIMESTAMP;
    else if (type == GL_PERFQUERY_COUNTER_EVENT_INTEL) return CNT_TYPE_NUM;
    else if (type == GL_PERFQUERY_COUNTER_DURATION_NORM_INTEL ||
             type == GL_PERFQUERY_COUNTER_DURATION_RAW_INTEL) return CNT_TYPE_DURATION;
    else if (type == GL_PERFQUERY_COUNTER_RAW_INTEL) return CNT_TYPE_GENERIC;
    else if (type == GL_PERFQUERY_COUNTER_THROUGHPUT_INTEL) return CNT_TYPE_GENERIC;
    else return CNT_TYPE_OTHER;
}

MetricBackend_INTEL_perfquery::DataCollector::~DataCollector() {
    for (auto &t1 : data) {
        for (auto &t2 : t1) {
            alloc.deallocate(t2, 1);
        }
    }
}

unsigned char*
MetricBackend_INTEL_perfquery::DataCollector::newDataBuffer(unsigned event,
                                                            size_t size)
{
    // in case there is no data for previous events fill with nullptr
    data[curPass].resize(event, nullptr);
    data[curPass].push_back(alloc.allocate(size));
    return data[curPass][event];
}

void MetricBackend_INTEL_perfquery::DataCollector::endPass() {
    curPass++;
    data.push_back(mmapdeque<unsigned char*>(alloc));
}

unsigned char*
MetricBackend_INTEL_perfquery::DataCollector::getDataBuffer(unsigned pass,
                                                            unsigned event)
{
    if (event < data[pass].size()) {
        return data[pass][event];
    } else return nullptr;
}

MetricBackend_INTEL_perfquery::MetricBackend_INTEL_perfquery(glretrace::Context* context,
                                                             MmapAllocator<char> &alloc)
    : numPasses(1), curPass(0), curEvent(0), collector(alloc) {
    if (context->hasExtension("GL_INTEL_performance_query")) {
        supported = true;
    } else {
        supported = false;
    }
}

bool MetricBackend_INTEL_perfquery::isSupported() {
    return supported;
}

void MetricBackend_INTEL_perfquery::enumGroups(enumGroupsCallback callback,
                                               void* userData)
{
    unsigned gid;
    glGetFirstPerfQueryIdINTEL(&gid);
    while (gid) {
        callback(gid, 0, userData);
        glGetNextPerfQueryIdINTEL(gid, &gid);
    }
}

void
MetricBackend_INTEL_perfquery::enumMetrics(unsigned group,
                                           enumMetricsCallback callback,
                                           void* userData)
{
    unsigned numMetrics;
    glGetPerfQueryInfoINTEL(group, 0, nullptr, nullptr, &numMetrics, nullptr, nullptr);
    for (int i = 1; i <= numMetrics; i++) {
        Metric_INTEL_perfquery metric = Metric_INTEL_perfquery(group, i);
        callback(&metric, 0, userData);
    }
}

std::unique_ptr<Metric>
MetricBackend_INTEL_perfquery::getMetricById(unsigned groupId, unsigned metricId)
{
    std::unique_ptr<Metric> p(new Metric_INTEL_perfquery(groupId, metricId));
    return p;
}

void MetricBackend_INTEL_perfquery::populateLookupGroups(unsigned group,
                                                     int error,
                                                     void* userData)
{
    reinterpret_cast<MetricBackend_INTEL_perfquery*>(userData)->enumMetrics(group, populateLookupMetrics);
}

void MetricBackend_INTEL_perfquery::populateLookupMetrics(Metric* metric,
                                                      int error,
                                                      void* userData)
{
    nameLookup[metric->name()] = std::make_pair(metric->groupId(),
                                                   metric->id());
}

std::unique_ptr<Metric>
MetricBackend_INTEL_perfquery::getMetricByName(std::string metricName)
{
    if (nameLookup.empty()) {
        enumGroups(populateLookupGroups, this);
    }
    if (nameLookup.count(metricName) > 0) {
        std::unique_ptr<Metric> p(new Metric_INTEL_perfquery(nameLookup[metricName].first,
                                                         nameLookup[metricName].second));
        return p;
    }
    else return nullptr;
}

std::string MetricBackend_INTEL_perfquery::getGroupName(unsigned group) {
    char name[INTEL_NAME_LENGTH];
    glGetPerfQueryInfoINTEL(group, INTEL_NAME_LENGTH, name, nullptr,
                            nullptr, nullptr, nullptr);
    return std::string(name);
}

int MetricBackend_INTEL_perfquery::enableMetric(Metric* metric_, QueryBoundary pollingRule) {
    if (pollingRule == QUERY_BOUNDARY_CALL) return 1;
    unsigned id = metric_->id();
    unsigned gid = metric_->groupId();
    unsigned numCounters;

    /* check that counter id is in valid range and group exists */
    glGetError();
    glGetPerfQueryInfoINTEL(gid, 0, nullptr, nullptr, &numCounters, nullptr, nullptr);
    GLenum err = glGetError();
    if (gid >= numCounters || err == GL_INVALID_VALUE) {
        return 1;
    }

    Metric_INTEL_perfquery metric(gid, id);
    passes[pollingRule][gid].push_back(metric);
    return 0;
}

unsigned MetricBackend_INTEL_perfquery::generatePasses() {
    /* begin with passes that profile frames */
    perFrame = true;
    curQueryMetrics = passes[QUERY_BOUNDARY_FRAME].begin();
    numFramePasses = passes[QUERY_BOUNDARY_FRAME].size();
    numPasses = numFramePasses + passes[QUERY_BOUNDARY_DRAWCALL].size();
    nameLookup.clear(); // no need in it after all metrics are set up
    return numPasses;
}

void MetricBackend_INTEL_perfquery::beginPass() {
    if (!numPasses || curQueryMetrics == passes[QUERY_BOUNDARY_DRAWCALL].end()) return;
    /* advance to draw calls after frames */
    if (curQueryMetrics == passes[QUERY_BOUNDARY_FRAME].end()) {
        perFrame = false;
        curQueryMetrics = passes[QUERY_BOUNDARY_DRAWCALL].begin();
    }
    glCreatePerfQueryINTEL(curQueryMetrics->first, &curQuery);
    curEvent = 0;
}

void MetricBackend_INTEL_perfquery::endPass() {
    if (!numPasses) return;
    glDeletePerfQueryINTEL(curQuery);
    curPass++;
    curQueryMetrics++;
    collector.endPass();
}

void MetricBackend_INTEL_perfquery::beginQuery(QueryBoundary boundary) {
    if (!numPasses) return;
    if (boundary == QUERY_BOUNDARY_CALL) return;
    if ((boundary == QUERY_BOUNDARY_FRAME) && !perFrame) return;
    if ((boundary == QUERY_BOUNDARY_DRAWCALL) && perFrame) return;
    glBeginPerfQueryINTEL(curQuery);
}

void MetricBackend_INTEL_perfquery::endQuery(QueryBoundary boundary) {
    if (!numPasses) return;
    if (boundary == QUERY_BOUNDARY_CALL) return;
    if ((boundary == QUERY_BOUNDARY_FRAME) && !perFrame) return;
    if ((boundary == QUERY_BOUNDARY_DRAWCALL) && perFrame) return;
    glEndPerfQueryINTEL(curQuery);
    freeQuery(curEvent++);
}

void MetricBackend_INTEL_perfquery::freeQuery(unsigned event) {
    GLuint size;
    GLuint bWritten;
    glGetPerfQueryInfoINTEL(curQueryMetrics->first, 0, nullptr, &size,
                            nullptr, nullptr, nullptr);
    unsigned char* data = collector.newDataBuffer(event, size);

    glFlush();
    glGetPerfQueryDataINTEL(curQuery, GL_PERFQUERY_WAIT_INTEL, size, data, &bWritten);
    // bWritten != size -> should generate error TODO
}

void MetricBackend_INTEL_perfquery::enumDataQueryId(unsigned id,
                                                    enumDataCallback callback,
                                                    QueryBoundary boundary,
                                                    void* userData)
{
    /* Determine passes to return depending on the boundary */
    if (boundary == QUERY_BOUNDARY_CALL) return;
    auto queryIt = passes[QUERY_BOUNDARY_FRAME].begin();
    unsigned j = 0;
    unsigned nPasses = numFramePasses;
    if (boundary == QUERY_BOUNDARY_DRAWCALL) {
        queryIt = passes[QUERY_BOUNDARY_DRAWCALL].begin();
        j = numFramePasses;
        nPasses = numPasses;
    }
    for (; j < nPasses; j++) {
        unsigned char* buf = collector.getDataBuffer(j, id);
        for (auto &k : queryIt->second) {
            if (buf) {
                callback(&k, id, &buf[k.offset()], 0, userData);
            } else { // No data buffer (in case event #id is not a draw call)
                callback(&k, id, nullptr, 0, userData);
            }
        }
        queryIt++;
    }
}

unsigned MetricBackend_INTEL_perfquery::getNumPasses() {
    return numPasses;
}

MetricBackend_INTEL_perfquery&
MetricBackend_INTEL_perfquery::getInstance(glretrace::Context* context,
                                           MmapAllocator<char> &alloc) {
    static MetricBackend_INTEL_perfquery backend(context, alloc);
    return backend;
}


std::map<std::string, std::pair<unsigned, unsigned>> MetricBackend_INTEL_perfquery::nameLookup;

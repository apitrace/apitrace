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

#include "metric_backend_amd_perfmon.hpp"

void Metric_AMD_perfmon::precache() {
    GLenum type;
    int length;
    std::string name;

    glGetPerfMonitorCounterInfoAMD(m_group, m_id, GL_COUNTER_TYPE_AMD, &type);
    if (type == GL_UNSIGNED_INT) m_nType = CNT_NUM_UINT;
    else if (type == GL_FLOAT || type == GL_PERCENTAGE_AMD) m_nType = CNT_NUM_FLOAT;
    else if (type == GL_UNSIGNED_INT64_AMD) m_nType = CNT_NUM_UINT64;
    else m_nType = CNT_NUM_UINT;

    glGetPerfMonitorCounterStringAMD(m_group, m_id, 0, &length, nullptr);
    name.resize(length);
    glGetPerfMonitorCounterStringAMD(m_group, m_id, length, 0, &name[0]);
    m_name = name;
    m_precached = true;
}

unsigned Metric_AMD_perfmon::id() {
    return m_id;
}

unsigned Metric_AMD_perfmon::groupId() {
    return m_group;
}

std::string Metric_AMD_perfmon::name() {
    if (!m_precached) precache();
    return m_name;
}

std::string Metric_AMD_perfmon::description() {
    return ""; // no description available
}

GLenum Metric_AMD_perfmon::size() {
    GLenum type;
    glGetPerfMonitorCounterInfoAMD(m_group, m_id, GL_COUNTER_TYPE_AMD, &type);
    if (type == GL_UNSIGNED_INT) return sizeof(GLuint);
    else if (type == GL_FLOAT || type == GL_PERCENTAGE_AMD) return sizeof(GLfloat);
    else if (type == GL_UNSIGNED_INT64_AMD) return sizeof(uint64_t);
    else return sizeof(GLuint);
}

MetricNumType Metric_AMD_perfmon::numType() {
    if (!m_precached) precache();
    return m_nType;
}

MetricType Metric_AMD_perfmon::type() {
    GLenum type;
    glGetPerfMonitorCounterInfoAMD(m_group, m_id, GL_COUNTER_TYPE_AMD, &type);
    if ((type == GL_UNSIGNED_INT || type == GL_UNSIGNED_INT64_AMD) ||
        (type == GL_FLOAT)) return CNT_TYPE_GENERIC;
    else if (type == GL_PERCENTAGE_AMD) return CNT_TYPE_PERCENT;
    else return CNT_TYPE_OTHER;
}


MetricBackend_AMD_perfmon::DataCollector::~DataCollector() {
    for (auto &t1 : data) {
        for (auto &t2 : t1) {
            alloc.deallocate(t2, 1);
        }
    }
}

unsigned*
MetricBackend_AMD_perfmon::DataCollector::newDataBuffer(unsigned event,
                                                        size_t size)
{
    // in case there is no data for previous events fill with nullptr
    data[curPass].resize(event, nullptr);
    data[curPass].push_back(alloc.allocate(size));
    return data[curPass][event];
}

void MetricBackend_AMD_perfmon::DataCollector::endPass() {
    curPass++;
    data.push_back(mmapdeque<unsigned*>(alloc));
}

unsigned*
MetricBackend_AMD_perfmon::DataCollector::getDataBuffer(unsigned pass,
                                                        unsigned event)
{
    if (event < data[pass].size()) {
        return data[pass][event];
    } else return nullptr;
}


MetricBackend_AMD_perfmon::MetricBackend_AMD_perfmon(glretrace::Context* context,
                                                     MmapAllocator<char> &alloc)
    : numPasses(1), curPass(0), curEvent(0), collector(alloc) {
    if (context->hasExtension("GL_AMD_performance_monitor")) {
        supported = true;
    } else {
        supported = false;
    }
}

bool MetricBackend_AMD_perfmon::isSupported() {
    return supported;
}

void MetricBackend_AMD_perfmon::enumGroups(enumGroupsCallback callback,
                                           void* userData)
{
    std::vector<unsigned> groups;
    int num_groups;
    glGetPerfMonitorGroupsAMD(&num_groups, 0, nullptr);
    groups.resize(num_groups);
    glGetPerfMonitorGroupsAMD(nullptr, num_groups, &groups[0]);
    for(unsigned &g : groups) {
        callback(g, 0, userData);
    }
}

void MetricBackend_AMD_perfmon::enumMetrics(unsigned group,
                                            enumMetricsCallback callback,
                                            void* userData)
{
    std::vector<unsigned> metrics;
    int num_metrics;
    Metric_AMD_perfmon metric(0,0);
    glGetPerfMonitorCountersAMD(group, &num_metrics, nullptr,  0, nullptr);
    metrics.resize(num_metrics);
    glGetPerfMonitorCountersAMD(group, nullptr, nullptr, num_metrics, &metrics[0]);
    for(unsigned &c : metrics) {
        metric = Metric_AMD_perfmon(group, c);
        callback(&metric, 0, userData);
    }
}

std::unique_ptr<Metric>
MetricBackend_AMD_perfmon::getMetricById(unsigned groupId, unsigned metricId)
{
    std::unique_ptr<Metric> p(new Metric_AMD_perfmon(groupId, metricId));
    return p;
}

void MetricBackend_AMD_perfmon::populateLookupGroups(unsigned group,
                                                     int error,
                                                     void* userData)
{
    reinterpret_cast<MetricBackend_AMD_perfmon*>(userData)->enumMetrics(group,
                                                        populateLookupMetrics);
}

void MetricBackend_AMD_perfmon::populateLookupMetrics(Metric* metric,
                                                      int error,
                                                      void* userData)
{
    nameLookup[metric->name()] = std::make_pair(metric->groupId(),
                                                metric->id());
}

std::unique_ptr<Metric>
MetricBackend_AMD_perfmon::getMetricByName(std::string metricName)
{
    if (nameLookup.empty()) {
        enumGroups(populateLookupGroups, this);
    }
    if (nameLookup.count(metricName) > 0) {
        std::unique_ptr<Metric> p(new Metric_AMD_perfmon(nameLookup[metricName].first,
                                                         nameLookup[metricName].second));
        return p;
    }
    else return nullptr;
}

std::string MetricBackend_AMD_perfmon::getGroupName(unsigned group) {
    int length;
    std::string name;
    glGetPerfMonitorGroupStringAMD(group, 0, &length, nullptr);
    name.resize(length);
    glGetPerfMonitorGroupStringAMD(group, length, 0, &name[0]);
    return name;
}

int MetricBackend_AMD_perfmon::enableMetric(Metric* metric_, QueryBoundary pollingRule) {
    unsigned id = metric_->id();
    unsigned gid = metric_->groupId();
    unsigned monitor;

    // profiling only draw calls
    if (pollingRule == QUERY_BOUNDARY_CALL) return 1;

    // check that Metric is valid metric
    glGenPerfMonitorsAMD(1, &monitor);
    glGetError();
    glSelectPerfMonitorCountersAMD(monitor, 1, gid, 1, &id);
    GLenum err = glGetError();
    glDeletePerfMonitorsAMD(1, &monitor);
    if (err == GL_INVALID_VALUE) {
        return 1;
    }

    Metric_AMD_perfmon metric(gid, id);
    metrics[pollingRule].push_back(metric);
    return 0;
}

bool
MetricBackend_AMD_perfmon::testMetrics(std::vector<Metric_AMD_perfmon>* metrics) {
    unsigned monitor;
    unsigned id;
    glGenPerfMonitorsAMD(1, &monitor);
    for (Metric_AMD_perfmon &c : *metrics) {
        id = c.id();
        glSelectPerfMonitorCountersAMD(monitor, 1, c.groupId(), 1, &id);
    }
    glGetError();
    glBeginPerfMonitorAMD(monitor);
    GLenum err = glGetError();
    glEndPerfMonitorAMD(monitor);
    glDeletePerfMonitorsAMD(1, &monitor);
    if (err == GL_INVALID_OPERATION) {
        return 0;
    }
    return 1;
}

void MetricBackend_AMD_perfmon::generatePassesBoundary(QueryBoundary boundary) {
    std::vector<Metric_AMD_perfmon> copyMetrics(metrics[boundary]);
    std::vector<Metric_AMD_perfmon> newPass;
    while (!copyMetrics.empty()) {
        std::vector<Metric_AMD_perfmon>::iterator it = copyMetrics.begin();
        while (it != copyMetrics.end()) {
            newPass.push_back(*it);
            if (!testMetrics(&newPass)) {
                newPass.pop_back();
                break;
            }
            it = copyMetrics.erase(it);
        }
        passes.push_back(newPass);
        newPass.clear();
    }
}

unsigned MetricBackend_AMD_perfmon::generatePasses() {
    generatePassesBoundary(QUERY_BOUNDARY_FRAME);
    numFramePasses = passes.size();
    generatePassesBoundary(QUERY_BOUNDARY_DRAWCALL);
    nameLookup.clear(); // no need in it after all metrics are set up
    numPasses = passes.size();
    return passes.size();
}

void MetricBackend_AMD_perfmon::beginPass() {
    if (!numPasses) return;
    /* First process per-frame passes, then per-call passes */
    if (curPass < numFramePasses) {
        perFrame = true;
    } else {
        perFrame = false;
    }
    /* Generate monitor */
    glGenPerfMonitorsAMD(NUM_MONITORS, monitors);
    for (Metric_AMD_perfmon &c : passes[curPass]) {
        unsigned id = c.id();
        for (int k = 0; k < NUM_MONITORS; k++) {
            glSelectPerfMonitorCountersAMD(monitors[k], 1, c.groupId(), 1, &id);
        }
    }
    curMonitor = 0;
    firstRound = true;
    curEvent = 0;
}

void MetricBackend_AMD_perfmon::endPass() {
    if (!numPasses) return;
    for (int k = 0; k < NUM_MONITORS; k++) {
        freeMonitor(k);
    }
    glDeletePerfMonitorsAMD(NUM_MONITORS, monitors);
    curPass++;
    collector.endPass();
}

void MetricBackend_AMD_perfmon::beginQuery(QueryBoundary boundary) {
    if (!numPasses) return;
    if (boundary == QUERY_BOUNDARY_CALL) return;
    if ((boundary == QUERY_BOUNDARY_FRAME) && !perFrame) return;
    if ((boundary == QUERY_BOUNDARY_DRAWCALL) && perFrame) return;

    curMonitor %= NUM_MONITORS;
    if (!firstRound) freeMonitor(curMonitor); // get existing data
    monitorEvent[curMonitor] = curEvent; // save monitored event
    glBeginPerfMonitorAMD(monitors[curMonitor]);
}

void MetricBackend_AMD_perfmon::endQuery(QueryBoundary boundary) {
    if (!numPasses) return;
    if (boundary == QUERY_BOUNDARY_CALL) return;
    if ((boundary == QUERY_BOUNDARY_FRAME) && !perFrame) return;
    if ((boundary == QUERY_BOUNDARY_DRAWCALL) && perFrame) return;

    curEvent++;
    glEndPerfMonitorAMD(monitors[curMonitor]);
    curMonitor++;
    if (curMonitor == NUM_MONITORS) firstRound = 0;
    curMonitor %= NUM_MONITORS;
}

void MetricBackend_AMD_perfmon::freeMonitor(unsigned monitorId) {
    unsigned monitor = monitors[monitorId];
    GLuint dataAvail = 0;
    GLuint size;

    glFlush();
    while (!dataAvail) {
        glGetPerfMonitorCounterDataAMD(monitor, GL_PERFMON_RESULT_AVAILABLE_AMD,
                                       sizeof(GLuint), &dataAvail, nullptr);
    }
    glGetPerfMonitorCounterDataAMD(monitor, GL_PERFMON_RESULT_SIZE_AMD,
                                   sizeof(GLuint), &size, nullptr);
    // collect data
    unsigned* buf = collector.newDataBuffer(monitorEvent[monitorId],
                                            size/sizeof(unsigned));
    glGetPerfMonitorCounterDataAMD(monitor, GL_PERFMON_RESULT_AMD, size, buf, nullptr);

    /* populate metricOffsets */
    if (metricOffsets.size() < curPass + 1) {
        std::map<std::pair<unsigned, unsigned>, unsigned> pairOffsets;
        unsigned offset = 0;
        unsigned id, gid;
        for (int k = 0; k < passes[curPass].size(); k++) {
            gid = buf[offset++];
            id = buf[offset++];
            pairOffsets[std::make_pair(gid, id)] = offset;
            Metric_AMD_perfmon metric(gid, id);
            offset += metric.size() / sizeof(unsigned);
        }
        // translate to existing metrics in passes variable
        std::map<Metric_AMD_perfmon*, unsigned> temp;
        for (auto &m : passes[curPass]) {
            id = m.id();
            gid = m.groupId();
            temp[&m] = pairOffsets[std::make_pair(gid, id)];
        }
        metricOffsets.push_back(std::move(temp));
    }
}

void
MetricBackend_AMD_perfmon::enumDataQueryId(unsigned id,
                                           enumDataCallback callback,
                                           QueryBoundary boundary,
                                           void* userData)
{
    /* Determine passes to return depending on the boundary */
    if (boundary == QUERY_BOUNDARY_CALL) return;
    unsigned j = 0;
    unsigned nPasses = numFramePasses;
    if (boundary == QUERY_BOUNDARY_DRAWCALL) {
        j = numFramePasses;
        nPasses = numPasses;
    }
    /* enum passes */
    for (; j < nPasses; j++) {
        unsigned* buf = collector.getDataBuffer(j, id);
        for (auto &m : passes[j]) {
            void* data = (buf) ? &buf[metricOffsets[j][&m]] : nullptr;
            callback(&m, id, data, 0, userData);
        }
    }
}

unsigned MetricBackend_AMD_perfmon::getNumPasses() {
    return numPasses;
}

MetricBackend_AMD_perfmon&
MetricBackend_AMD_perfmon::getInstance(glretrace::Context* context,
                                       MmapAllocator<char> &alloc) {
    static MetricBackend_AMD_perfmon backend(context, alloc);
    return backend;
}


std::map<std::string, std::pair<unsigned, unsigned>> MetricBackend_AMD_perfmon::nameLookup;

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

#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <iostream>

#include "retrace.hpp"
#include "metric_backend.hpp"
#include "metric_writer.hpp"
#include "metric_backend_amd_perfmon.hpp"
#include "metric_backend_intel_perfquery.hpp"
#include "metric_backend_opengl.hpp"
#include "mmap_allocator.hpp"

namespace glretrace {

bool metricBackendsSetup = false;
bool profilingContextAcquired = false;
bool profilingBoundaries[QUERY_BOUNDARY_LIST_END] = {false};
unsigned profilingBoundariesIndex[QUERY_BOUNDARY_LIST_END] = {0};
std::vector<MetricBackend*> metricBackends; // to be populated in initContext()
MetricBackend* curMetricBackend = nullptr; // backend active in the current pass
MetricWriter profiler(metricBackends, MmapAllocator<char>());

MetricBackend* getBackend(std::string backendName) {
    // allocator for metric storage
    MmapAllocator<char> alloc;
    // to be populated with backends
    Context *currentContext = getCurrentContext();
    if (backendName == "GL_AMD_performance_monitor") return &MetricBackend_AMD_perfmon::getInstance(currentContext, alloc);
    else if (backendName == "GL_INTEL_performance_query") return &MetricBackend_INTEL_perfquery::getInstance(currentContext, alloc);
    else if (backendName == "opengl") return &MetricBackend_opengl::getInstance(currentContext, alloc);
    else return nullptr;
}

bool
isLastPass() {
    return ( retrace::curPass + 1 >= retrace::numPasses );
}

/* Callbacks for listing metrics with --list-metrics */
void listMetrics_metricCallback(Metric* c, int error, void* userData) {
    static const std::string metricType[] = {"CNT_TYPE_GENERIC", "CNT_TYPE_NUM",
                                "CNT_TYPE_DURATION", "CNT_TYPE_PERCENT",
                                "CNT_TYPE_TIMESTAMP", "CNT_TYPE_OTHER"};
    static const std::string metricNumType[] = {"CNT_NUM_UINT", "CNT_NUM_FLOAT",
                                   "CNT_NUM_UINT64", "CNT_NUM_DOUBLE",
                                   "CNT_NUM_BOOL", "CNT_NUM_INT64"};

    std::cout << "    Metric #" << c->id() << ": " << c->name()
              << " (type: " << metricType[c->type()] << ", num. type: "
              << metricNumType[c->numType()] << ").\n";
    std::cout << "           Description: " << c->description() << "\n";
}

void listMetrics_groupCallback(unsigned g, int error, void* userData) {
    MetricBackend* b = reinterpret_cast<MetricBackend*>(userData);
    std::cout << "\n  Group #" << g << ": " << b->getGroupName(g) << ".\n";
    b->enumMetrics(g, listMetrics_metricCallback, userData);
}

void listMetricsCLI() {
    // backends is to be populated with backend names
    std::string backends[] = {"GL_AMD_performance_monitor",
                              "GL_INTEL_performance_query",
                              "opengl"};
    std::cout << "Available metrics: \n";
    for (auto s : backends) {
        auto b = getBackend(s);
        if (!b->isSupported()) {
            continue;
        }
        std::cout << "\nBackend " << s << ":\n";
        b->enumGroups(listMetrics_groupCallback, b);
        std::cout << std::endl;
    }
}

void parseMetricsBlock(QueryBoundary pollingRule, const char* str,
                       std::size_t limit, MetricBackend* backend)
{
    const char* end;
    bool lastItem = false;

    while (((end = reinterpret_cast<const char*>(std::memchr(str, ',', limit))) != nullptr)
           || !lastItem)
    {
        std::unique_ptr<Metric> p;
        std::string metricName;

        if (!end) {
            lastItem = true;
            end = str + limit;
        }
        std::size_t span = std::strspn(str, " ");
        limit -= span;
        str += span;
        // parse [group, id]
        if (*str == '[') {
            std::string groupStr = std::string(str, 1, end-str-1);
            limit -= end + 1 - str;
            str = end + 1;
            end = reinterpret_cast<const char*>(std::memchr(str, ']', limit));
            std::string idStr = std::string(str, 0, end-str);
            limit -= end + 1 - str;
            str = end + 1;
            const char* next = reinterpret_cast<const char*>(std::memchr(str, ',', limit));
            if (next) {
                end = next;
                limit -= end + 1 - str;
                str = end + 1;
            }
#if defined(ANDROID)
	    // http://stackoverflow.com/questions/17950814/how-to-use-stdstoul-and-stdstoull-in-android
	    unsigned groupId = strtoul(groupStr.c_str(), nullptr, 10);
	    unsigned metricId = strtoul(idStr.c_str(), nullptr, 10);
#else
	    unsigned groupId = std::stoul(groupStr);
	    unsigned metricId = std::stoul(idStr);
#endif
            p = backend->getMetricById(groupId, metricId);
            metricName = "[" + groupStr + ", " + idStr + "]";
        // parse metricName
        } else {
            if (end - str) {
                metricName = std::string(str, 0, end-str);
                p = backend->getMetricByName(metricName);
            }
            limit -= end + (lastItem ? 0 : 1) - str;
            str = end + (lastItem ? 0 : 1);
            if (metricName.empty()) continue;
        }
        if (!p) {
            std::cerr << "Warning: No metric \"" << metricName
                << "\"." << std::endl;
            continue;
        }
        int error = backend->enableMetric(p.get(), pollingRule);
        if (error) {
            std::cerr << "Warning: Metric " << metricName << " not enabled"
                " (error " << error << ")." << std::endl;
        } else {
            profilingBoundaries[pollingRule] = true;
        }
    }
}

void parseBackendBlock(QueryBoundary pollingRule, const char* str,
                       std::size_t limit, std::set<MetricBackend*> &backendsHash)
{
    const char* delim = reinterpret_cast<const char*>(std::memchr(str, ':', limit));
    if (delim) {
        std::size_t span = std::strspn(str, " ");
        std::string backendName = std::string(str, span, delim-str-span);
        MetricBackend* backend = getBackend(backendName);
        if (!backend) {
            std::cerr << "Warning: No backend \"" << backendName << "\"."
                      << std::endl;
            return;
        }
        if (!backend->isSupported()) {
            std::cerr << "Warning: Backend \"" << backendName
                      << "\" is not supported." << std::endl;
            return;
        }

        /**
         * order in metricBackends is important for output
         * also there should be no duplicates
         */
        if (backendsHash.find(backend) == backendsHash.end()) {
            metricBackends.push_back(backend);
            backendsHash.insert(backend);
        }

        limit -= (delim-str) + 1;
        parseMetricsBlock(pollingRule, delim + 1, limit, backend);
    }
}

void enableMetricsFromCLI(const char* metrics, QueryBoundary pollingRule) {
    static std::set<MetricBackend*> backendsHash; // for not allowing duplicates
    const char* end;

    while ((end = std::strchr(metrics, ';')) != nullptr) {
        parseBackendBlock(pollingRule, metrics, end-metrics, backendsHash);
        metrics = end + 1;
    }
    parseBackendBlock(pollingRule, metrics, std::strlen(metrics), backendsHash);
}

} /* namespace glretrace */

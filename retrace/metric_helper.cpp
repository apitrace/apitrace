#include <string>
#include <vector>
#include <set>
#include <regex>
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

void enableMetricsFromCLI(const char* metrics, QueryBoundary pollingRule) {
    const std::regex rOuter("\\s*([^:]+):\\s*([^;]*);?"); // backend: (...)
    const std::regex rInner("\\s*\\[\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\]\\s*,?"
                            "|\\s*([^;,]+),?"); // [g, i] | metricName
    std::unique_ptr<Metric> p;
    std::string metricName;

    auto rOuter_it = std::cregex_token_iterator(metrics,
                                                metrics+std::strlen(metrics),
                                                rOuter, {1,2});
    auto rOuter_end = std::cregex_token_iterator();
    while (rOuter_it != rOuter_end) {
        static std::set<MetricBackend*> backendsHash; // for not allowing duplicates
        std::string backendName = (rOuter_it++)->str();
        MetricBackend* backend = getBackend(backendName);
        if (!backend) {
            std::cerr << "Warning: No backend \"" << backendName << "\"."
                      << std::endl;
            rOuter_it++;
            continue;
        }
        if (!backend->isSupported()) {
            std::cerr << "Warning: Backend \"" << backendName
                      << "\" is not supported." << std::endl;
            rOuter_it++;
            continue;
        }
        /**
         * order in metricBackends is important for output
         * also there should be no duplicates
         */
        if (backendsHash.find(backend) == backendsHash.end()) {
            metricBackends.push_back(backend);
            backendsHash.insert(backend);
        }

        auto rInner_it = std::cregex_token_iterator(rOuter_it->first, rOuter_it->second,
                                                    rInner, {1,2,3});
        auto rInner_end = std::cregex_token_iterator();
        while (rInner_it != rInner_end) {
            if (rInner_it->matched) {
                std::string groupId = (rInner_it++)->str();
                std::string metricId = (rInner_it++)->str();
                rInner_it++;
                p = backend->getMetricById(std::stoi(groupId), std::stoi(metricId));
                metricName = "[" + groupId + ", " + metricId + "]";
            } else {
                rInner_it++;
                rInner_it++;
                metricName = (rInner_it++)->str();
                p = backend->getMetricByName(metricName);
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
        rOuter_it++;
    }
}

} /* namespace glretrace */

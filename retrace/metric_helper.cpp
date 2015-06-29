#include <string>
#include <vector>
#include <set>
#include <regex>
#include <iostream>

#include "retrace.hpp"
#include "metric_backend.hpp"

namespace glretrace {

bool metricBackendsSetup = false;
bool profilingBoundaries[QUERY_BOUNDARY_LIST_END] = {false};
std::vector<MetricBackend*> metricBackends; // to be populated in initContext()
MetricBackend* curMetricBackend = nullptr; // backend active in the current pass

MetricBackend* getBackend(std::string backendName) {
    return nullptr; // to be populated with backends
}

bool
isLastPass() {
    return ( retrace::curPass + 1 >= retrace::numPasses );
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

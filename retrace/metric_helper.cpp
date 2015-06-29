#include <string>

#include "metric_backend.hpp"

namespace glretrace {

MetricBackend* getBackend(std::string backendName) {
    return nullptr; // to be populated with backends
}

} /* namespace glretrace */

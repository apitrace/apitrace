#ifndef _TRACE_BACKTRACE_HPP_
#define _TRACE_BACKTRACE_HPP_

#include <vector>

#include "trace_model.hpp"

namespace trace {


#if defined(ANDROID) || defined(__ELF__)

std::vector<RawStackFrame> get_backtrace();
bool backtrace_is_needed(const char* fname);

#else

static inline std::vector<RawStackFrame> get_backtrace() {
    return std::vector<RawStackFrame>();
}

static inline bool backtrace_is_needed(const char*) {
    return false;
}

#endif

} /* namespace trace */

#endif

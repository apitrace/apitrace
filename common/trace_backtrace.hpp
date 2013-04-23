#ifndef _TRACE_BACKTRACE_HPP_
#define _TRACE_BACKTRACE_HPP_

#include <vector>

#include "trace_model.hpp"

namespace trace {


struct RawStackFrame {
    char* module;
    char* function;
    char* filename;
    char* linenumber;
    char* offset;
    RawStackFrame() :
      module(0),
      function(0),
      filename(0),
      linenumber(0),
      offset(0)
    {
    }
};

#if defined(ANDROID) or defined(__linux__)

std::vector<RawStackFrame> get_backtrace();
bool backtrace_is_needed(const char* fname);

#if defined(ANDROID)

#define MAX_BT_FUNC 20
#define PREFIX_MAX_FUNC_NAME 100
#define APITRACE_FNAMES_FILE "/data/apitrace.fnames"
#define APITRACE_FNAMES_SOURCE APITRACE_FNAMES_FILE

#elif defined(__linux__)

#define MAX_BT_FUNC 20
#define PREFIX_MAX_FUNC_NAME 100
#define APITRACE_FNAMES_ENV "APITRACE_BT_FUNCTIONS"
#define APITRACE_FNAMES_SOURCE APITRACE_FNAMES_ENV

#endif

#else /* !__linux__ && !ANDROID */

static inline std::vector<StackFrame> get_backtrace() {
    return std::vector<StackFrame>();
}

static inline bool backtrace_is_needed(const char*) {
    return false;
}

#endif

} /* namespace trace */

#endif

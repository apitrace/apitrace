/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Shuang He <shuang.he@intel.com>
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


#include <string.h>

#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"
#include "os_time.hpp"
#include "os_memory.hpp"

/* Synchronous debug output may reduce performance however,
 * without it the callNo in the callback may be inaccurate
 * as the callback may be called at any time.
 */
#define DEBUG_OUTPUT_SYNCHRONOUS 0

namespace glretrace {

bool insideList = false;
bool insideGlBeginEnd = false;
bool supportsARBShaderObjects = false;

enum {
    GPU_START = 0,
    GPU_DURATION,
    OCCLUSION,
    NUM_QUERIES,
};

struct CallQuery
{
    GLuint ids[NUM_QUERIES];
    unsigned call;
    bool isDraw;
    GLuint program;
    const trace::FunctionSig *sig;
    int64_t cpuStart;
    int64_t cpuEnd;
    int64_t vsizeStart;
    int64_t vsizeEnd;
    int64_t rssStart;
    int64_t rssEnd;
};

static bool supportsElapsed = true;
static bool supportsTimestamp = true;
static bool supportsOcclusion = true;
static bool supportsDebugOutput = true;

static std::list<CallQuery> callQueries;

static void APIENTRY
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);

void
checkGlError(trace::Call &call) {
    GLenum error = glGetError();
    while (error != GL_NO_ERROR) {
        std::ostream & os = retrace::warning(call);

        os << "glGetError(";
        os << call.name();
        os << ") = ";

        switch (error) {
        case GL_INVALID_ENUM:
            os << "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            os << "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            os << "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            os << "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            os << "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            os << "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            os << "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_TABLE_TOO_LARGE:
            os << "GL_TABLE_TOO_LARGE";
            break;
        default:
            os << error;
            break;
        }
        os << "\n";
    
        error = glGetError();
    }
}

static inline int64_t
getCurrentTime(void) {
    if (retrace::profilingGpuTimes && supportsTimestamp) {
        /* Get the current GL time without stalling */
        GLint64 timestamp = 0;
        glGetInteger64v(GL_TIMESTAMP, &timestamp);
        return timestamp;
    } else {
        return os::getTime();
    }
}

static inline int64_t
getTimeFrequency(void) {
    if (retrace::profilingGpuTimes && supportsTimestamp) {
        return 1000000000;
    } else {
        return os::timeFrequency;
    }
}

static inline void
getCurrentVsize(int64_t& vsize) {
    vsize = os::getVsize();
}

static inline void
getCurrentRss(int64_t& rss) {
    rss = os::getRss();
}

static void
completeCallQuery(CallQuery& query) {
    /* Get call start and duration */
    int64_t gpuStart = 0, gpuDuration = 0, cpuDuration = 0, pixels = 0, vsizeDuration = 0, rssDuration = 0;

    if (query.isDraw) {
        if (retrace::profilingGpuTimes) {
            if (supportsTimestamp) {
                glGetQueryObjecti64vEXT(query.ids[GPU_START], GL_QUERY_RESULT, &gpuStart);
            }

            glGetQueryObjecti64vEXT(query.ids[GPU_DURATION], GL_QUERY_RESULT, &gpuDuration);
        }

        if (retrace::profilingPixelsDrawn) {
            glGetQueryObjecti64vEXT(query.ids[OCCLUSION], GL_QUERY_RESULT, &pixels);
        }

    } else {
        pixels = -1;
    }

    if (retrace::profilingCpuTimes) {
        double cpuTimeScale = 1.0E9 / getTimeFrequency();
        cpuDuration = (query.cpuEnd - query.cpuStart) * cpuTimeScale;
        query.cpuStart *= cpuTimeScale;
    }

    if (retrace::profilingMemoryUsage) {
        vsizeDuration = query.vsizeEnd - query.vsizeStart;
        rssDuration = query.rssEnd - query.rssStart;
    }

    glDeleteQueries(NUM_QUERIES, query.ids);

    /* Add call to profile */
    retrace::profiler.addCall(query.call, query.sig->name, query.program, pixels, gpuStart, gpuDuration, query.cpuStart, cpuDuration, query.vsizeStart, vsizeDuration, query.rssStart, rssDuration);
}

void
flushQueries() {
    for (std::list<CallQuery>::iterator itr = callQueries.begin(); itr != callQueries.end(); ++itr) {
        completeCallQuery(*itr);
    }

    callQueries.clear();
}

void
beginProfile(trace::Call &call, bool isDraw) {
    glretrace::Context *currentContext = glretrace::getCurrentContext();

    /* Create call query */
    CallQuery query;
    query.isDraw = isDraw;
    query.call = call.no;
    query.sig = call.sig;
    query.program = currentContext ? currentContext->activeProgram : 0;

    glGenQueries(NUM_QUERIES, query.ids);

    /* GPU profiling only for draw calls */
    if (isDraw) {
        if (retrace::profilingGpuTimes) {
            if (supportsTimestamp) {
                glQueryCounter(query.ids[GPU_START], GL_TIMESTAMP);
            }

            glBeginQuery(GL_TIME_ELAPSED, query.ids[GPU_DURATION]);
        }

        if (retrace::profilingPixelsDrawn) {
            glBeginQuery(GL_SAMPLES_PASSED, query.ids[OCCLUSION]);
        }
    }

    callQueries.push_back(query);

    /* CPU profiling for all calls */
    if (retrace::profilingCpuTimes) {
        CallQuery& query = callQueries.back();
        query.cpuStart = getCurrentTime();
    }

    if (retrace::profilingMemoryUsage) {
        CallQuery& query = callQueries.back();
        query.vsizeStart = os::getVsize();
        query.rssStart = os::getRss();
    }
}

void
endProfile(trace::Call &call, bool isDraw) {

    /* CPU profiling for all calls */
    if (retrace::profilingCpuTimes) {
        CallQuery& query = callQueries.back();
        query.cpuEnd = getCurrentTime();
    }

    /* GPU profiling only for draw calls */
    if (isDraw) {
        if (retrace::profilingGpuTimes) {
            glEndQuery(GL_TIME_ELAPSED);
        }

        if (retrace::profilingPixelsDrawn) {
            glEndQuery(GL_SAMPLES_PASSED);
        }
    }

    if (retrace::profilingMemoryUsage) {
        CallQuery& query = callQueries.back();
        query.vsizeEnd = os::getVsize();
        query.rssEnd = os::getRss();
    }
}

void
initContext() {
    glretrace::Context *currentContext = glretrace::getCurrentContext();

    /* Ensure we have adequate extension support */
    assert(currentContext);
    supportsTimestamp   = currentContext->hasExtension("GL_ARB_timer_query");
    supportsElapsed     = currentContext->hasExtension("GL_EXT_timer_query") || supportsTimestamp;
    supportsOcclusion   = currentContext->hasExtension("GL_ARB_occlusion_query");
    supportsDebugOutput = currentContext->hasExtension("GL_ARB_debug_output");
    supportsARBShaderObjects = currentContext->hasExtension("GL_ARB_shader_objects");

    /* Check for timer query support */
    if (retrace::profilingGpuTimes) {
        if (!supportsTimestamp && !supportsElapsed) {
            std::cout << "Error: Cannot run profile, GL_EXT_timer_query extension is not supported." << std::endl;
            exit(-1);
        }

        GLint bits = 0;
        glGetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &bits);

        if (!bits) {
            std::cout << "Error: Cannot run profile, GL_QUERY_COUNTER_BITS == 0." << std::endl;
            exit(-1);
        }
    }

    /* Check for occlusion query support */
    if (retrace::profilingPixelsDrawn && !supportsOcclusion) {
        std::cout << "Error: Cannot run profile, GL_ARB_occlusion_query extension is not supported." << std::endl;
        exit(-1);
    }

    /* Setup debug message call back */
    if (retrace::debug && supportsDebugOutput) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        glDebugMessageCallbackARB(&debugOutputCallback, currentContext);

        if (DEBUG_OUTPUT_SYNCHRONOUS) {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        }
    }

    /* Sync the gpu and cpu start times */
    if (retrace::profilingCpuTimes || retrace::profilingGpuTimes) {
        if (!retrace::profiler.hasBaseTimes()) {
            double cpuTimeScale = 1.0E9 / getTimeFrequency();
            GLint64 currentTime = getCurrentTime() * cpuTimeScale;
            retrace::profiler.setBaseCpuTime(currentTime);
            retrace::profiler.setBaseGpuTime(currentTime);
        }
    }

    if (retrace::profilingMemoryUsage) {
        GLint64 currentVsize, currentRss;
        getCurrentVsize(currentVsize);
        retrace::profiler.setBaseVsizeUsage(currentVsize);
        getCurrentRss(currentRss);
        retrace::profiler.setBaseRssUsage(currentRss);
    }
}

void
frame_complete(trace::Call &call) {
    if (retrace::profiling) {
        /* Complete any remaining queries */
        flushQueries();

        /* Indicate end of current frame */
        retrace::profiler.addFrameEnd();
    }

    retrace::frameComplete(call);

    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (!currentContext) {
        return;
    }

    assert(currentContext->drawable);
    if (retrace::debug && !currentContext->drawable->visible) {
        retrace::warning(call) << "could not infer drawable size (glViewport never called)\n";
    }
}

static const char*
getDebugOutputSource(GLenum source) {
    switch(source) {
    case GL_DEBUG_SOURCE_API_ARB:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
        return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
        return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
        return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION_ARB:
        return "Application";
    case GL_DEBUG_SOURCE_OTHER_ARB:
    default:
        return "";
    }
}

static const char*
getDebugOutputType(GLenum type) {
    switch(type) {
    case GL_DEBUG_TYPE_ERROR_ARB:
        return "error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        return "deprecated behaviour";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
        return "undefined behaviour";
    case GL_DEBUG_TYPE_PORTABILITY_ARB:
        return "portability issue";
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        return "performance issue";
    case GL_DEBUG_TYPE_OTHER_ARB:
    default:
        return "unknown issue";
    }
}

static const char*
getDebugOutputSeverity(GLenum severity) {
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
        return "High";
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        return "Medium";
    case GL_DEBUG_SEVERITY_LOW_ARB:
        return "Low";
    default:
        return "usnknown";
    }
}

static void APIENTRY
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
    std::cerr << retrace::callNo << ": ";
    std::cerr << "glDebugOutputCallback: ";
    std::cerr << getDebugOutputSeverity(severity) << " severity ";
    std::cerr << getDebugOutputSource(source) << " " << getDebugOutputType(type);
    std::cerr << " " << id;
    std::cerr << ", " << message;
    std::cerr << std::endl;
}

} /* namespace glretrace */


class GLDumper : public retrace::Dumper {
public:
    image::Image *
    getSnapshot(void) {
        if (!glretrace::getCurrentContext()) {
            return NULL;
        }
        return glstate::getDrawBufferImage();
    }

    bool
    dumpState(std::ostream &os) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        if (glretrace::insideGlBeginEnd ||
            !currentContext) {
            return false;
        }
        glstate::dumpCurrentContext(os);
        return true;
    }
};

static GLDumper glDumper;


void
retrace::setUp(void) {
    glws::init();
    dumper = &glDumper;
}


void
retrace::addCallbacks(retrace::Retracer &retracer)
{
    retracer.addCallbacks(glretrace::gl_callbacks);
    retracer.addCallbacks(glretrace::glx_callbacks);
    retracer.addCallbacks(glretrace::wgl_callbacks);
    retracer.addCallbacks(glretrace::cgl_callbacks);
    retracer.addCallbacks(glretrace::egl_callbacks);
}


void
retrace::flushRendering(void) {
    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (currentContext) {
        glretrace::flushQueries();
        glFlush();
    }
}

void
retrace::waitForInput(void) {
    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (currentContext) {
        glretrace::flushQueries();
        glFlush();
    }
    while (glws::processEvents()) {
        os::sleep(100*1000);
    }
}

void
retrace::cleanUp(void) {
    glws::cleanup();
}

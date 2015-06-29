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

#include <map>

#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"
#include "os_time.hpp"
#include "os_memory.hpp"
#include "highlight.hpp"


/* Synchronous debug output may reduce performance however,
 * without it the callNo in the callback may be inaccurate
 * as the callback may be called at any time.
 */
#define DEBUG_OUTPUT_SYNCHRONOUS 0

namespace glretrace {

glprofile::Profile defaultProfile(glprofile::API_GL, 1, 0);

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

static std::list<CallQuery> callQueries;

static void APIENTRY
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

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
                /* Use ARB queries in case EXT not present */
                glGetQueryObjecti64v(query.ids[GPU_START], GL_QUERY_RESULT, &gpuStart);
                glGetQueryObjecti64v(query.ids[GPU_DURATION], GL_QUERY_RESULT, &gpuDuration);
            } else {
                glGetQueryObjecti64vEXT(query.ids[GPU_DURATION], GL_QUERY_RESULT, &gpuDuration);
            }
        }

        if (retrace::profilingPixelsDrawn) {
            if (supportsTimestamp) {
                glGetQueryObjecti64v(query.ids[OCCLUSION], GL_QUERY_RESULT, &pixels);
            } else if (supportsElapsed) {
                glGetQueryObjecti64vEXT(query.ids[OCCLUSION], GL_QUERY_RESULT, &pixels);
            } else {
                uint32_t pixels32;
                glGetQueryObjectuiv(query.ids[OCCLUSION], GL_QUERY_RESULT, &pixels32);
                pixels = static_cast<int64_t>(pixels32);
            }
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
    if (retrace::profilingWithBackends) {
        if (profilingBoundaries[QUERY_BOUNDARY_CALL] ||
            profilingBoundaries[QUERY_BOUNDARY_DRAWCALL]) {
            if (curMetricBackend) {
                curMetricBackend->beginQuery(isDraw ? QUERY_BOUNDARY_DRAWCALL : QUERY_BOUNDARY_CALL);
            }
        }
        return;
    }

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
    if (retrace::profilingWithBackends) {
        if (profilingBoundaries[QUERY_BOUNDARY_CALL] ||
            profilingBoundaries[QUERY_BOUNDARY_DRAWCALL]) {
            if (curMetricBackend) {
                curMetricBackend->endQuery(isDraw ? QUERY_BOUNDARY_DRAWCALL : QUERY_BOUNDARY_CALL);
            }
        }
        return;
    }

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


GLenum
blockOnFence(trace::Call &call, GLsync sync, GLbitfield flags) {
    GLenum result;

    do {
        result = glClientWaitSync(sync, flags, 1000);
    } while (result == GL_TIMEOUT_EXPIRED);

    switch (result) {
    case GL_ALREADY_SIGNALED:
    case GL_CONDITION_SATISFIED:
        break;
    default:
        retrace::warning(call) << "got " << glstate::enumToString(result) << "\n";
    }

    return result;
}


/**
 * Helper for properly retrace glClientWaitSync().
 */
GLenum
clientWaitSync(trace::Call &call, GLsync sync, GLbitfield flags, GLuint64 timeout) {
    GLenum result = call.ret->toSInt();
    switch (result) {
    case GL_ALREADY_SIGNALED:
    case GL_CONDITION_SATISFIED:
        // We must block, as following calls might rely on the fence being
        // signaled
        result = blockOnFence(call, sync, flags);
        break;
    case GL_TIMEOUT_EXPIRED:
        result = glClientWaitSync(sync, flags, timeout);
        break;
    case GL_WAIT_FAILED:
        break;
    default:
        retrace::warning(call) << "unexpected return value\n";
        break;
    }
    return result;
}


/*
 * Called the first time a context is made current.
 */
void
initContext() {
    glretrace::Context *currentContext = glretrace::getCurrentContext();
    assert(currentContext);

    /* Ensure we have adequate extension support */
    glprofile::Profile currentProfile = currentContext->actualProfile();
    supportsTimestamp   = currentProfile.versionGreaterOrEqual(glprofile::API_GL, 3, 3) ||
                          currentContext->hasExtension("GL_ARB_timer_query");
    supportsElapsed     = currentContext->hasExtension("GL_EXT_timer_query") || supportsTimestamp;
    supportsOcclusion   = currentProfile.versionGreaterOrEqual(glprofile::API_GL, 1, 5);
    supportsARBShaderObjects = currentContext->hasExtension("GL_ARB_shader_objects");

#ifdef __APPLE__
    // GL_TIMESTAMP doesn't work on Apple.  GL_TIME_ELAPSED still does however.
    // http://lists.apple.com/archives/mac-opengl/2014/Nov/threads.html#00001
    supportsTimestamp   = false;
#endif

    /* Check for timer query support */
    if (retrace::profilingGpuTimes) {
        if (!supportsTimestamp && !supportsElapsed) {
            std::cout << "error: cannot profile, GL_ARB_timer_query or GL_EXT_timer_query extensions are not supported." << std::endl;
            exit(-1);
        }

        GLint bits = 0;
        glGetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &bits);

        if (!bits) {
            std::cout << "error: cannot profile, GL_QUERY_COUNTER_BITS == 0." << std::endl;
            exit(-1);
        }
    }

    /* Check for occlusion query support */
    if (retrace::profilingPixelsDrawn && !supportsOcclusion) {
        std::cout << "error: cannot profile, GL_ARB_occlusion_query extension is not supported (" << currentProfile << ")" << std::endl;
        exit(-1);
    }

    /* Setup debug message call back */
    if (retrace::debug) {
        if (currentContext->hasExtension("GL_KHR_debug")) {
            if (currentProfile.desktop()) {
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
                glDebugMessageCallback(&debugOutputCallback, currentContext);
            } else {
                glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
                glDebugMessageCallbackKHR(&debugOutputCallback, currentContext);
            }

            if (DEBUG_OUTPUT_SYNCHRONOUS) {
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            }
        } else if (currentContext->hasExtension("GL_ARB_debug_output")) {
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
            glDebugMessageCallbackARB(&debugOutputCallback, currentContext);

            if (DEBUG_OUTPUT_SYNCHRONOUS) {
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            }
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

    if (retrace::profilingWithBackends) {
        if (!metricBackendsSetup) {
            if (retrace::profilingCallsMetricsString) {
                enableMetricsFromCLI(retrace::profilingCallsMetricsString,
                                     QUERY_BOUNDARY_CALL);
            }
            if (retrace::profilingFramesMetricsString) {
                enableMetricsFromCLI(retrace::profilingFramesMetricsString,
                                     QUERY_BOUNDARY_FRAME);
            }
            if (retrace::profilingDrawCallsMetricsString) {
                enableMetricsFromCLI(retrace::profilingDrawCallsMetricsString,
                                     QUERY_BOUNDARY_DRAWCALL);
            }
            unsigned numPasses = 0;
            for (auto &b : metricBackends) {
                b->generatePasses();
                numPasses += b->getNumPasses();
            }
            retrace::numPasses = numPasses > 0 ? numPasses : 1;
            metricBackendsSetup = 1;
        }

        unsigned numPasses = 0;
        for (auto &b : metricBackends) {
            numPasses += b->getNumPasses();
            if (retrace::curPass < numPasses) {
                curMetricBackend = b;
                b->beginPass(); // begin pass
                break;
            }
        }

        if (profilingBoundaries[QUERY_BOUNDARY_FRAME]) {
            if (curMetricBackend) {
                curMetricBackend->beginQuery(QUERY_BOUNDARY_FRAME);
            }
        }
    }
}

void
frame_complete(trace::Call &call) {
    if (retrace::profilingWithBackends) {
        if (curMetricBackend) {
            curMetricBackend->endQuery(QUERY_BOUNDARY_FRAME);
        }
    }
    else if (retrace::profiling) {
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

    glws::Drawable *currentDrawable = currentContext->drawable;
    assert(currentDrawable);
    if (retrace::debug &&
        !currentDrawable->pbuffer &&
        !currentDrawable->visible) {
        retrace::warning(call) << "could not infer drawable size (glViewport never called)\n";
    }

    if (curMetricBackend) {
        curMetricBackend->beginQuery(QUERY_BOUNDARY_FRAME);
    }
}


// Limit messages
// TODO: expose this via a command line option.
static const unsigned
maxMessageCount = 100;

static std::map< uint64_t, unsigned > messageCounts;


static void APIENTRY
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const GLchar* message, const void *userParam)
{
    /* Ignore application messages while dumping state. */
    if (retrace::dumpingState &&
        source == GL_DEBUG_SOURCE_APPLICATION) {
        return;
    }

    /* Ignore NVIDIA's "Buffer detailed info:" messages, as they seem to be
     * purely informative, and high frequency. */
    if (source == GL_DEBUG_SOURCE_API &&
        type == GL_DEBUG_TYPE_OTHER &&
        severity == GL_DEBUG_SEVERITY_LOW &&
        id == 131185) {
        return;
    }

    // Keep track of identical messages; and ignore them after a while.
    uint64_t messageHash =  (uint64_t)id
                         + ((uint64_t)severity << 16)
                         + ((uint64_t)type     << 32)
                         + ((uint64_t)source   << 48);
    size_t messageCount = messageCounts[messageHash]++;
    if (messageCount > maxMessageCount) {
        return;
    }

    const highlight::Highlighter & highlighter = highlight::defaultHighlighter(std::cerr);

    const char *severityStr = "";
    const highlight::Attribute * color = &highlighter.normal();

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        color = &highlighter.color(highlight::RED);
        severityStr = " major";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        break;
    case GL_DEBUG_SEVERITY_LOW:
        color = &highlighter.color(highlight::GRAY);
        severityStr = " minor";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        color = &highlighter.color(highlight::GRAY);
        break;
    default:
        assert(0);
    }

    const char *sourceStr = "";
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        sourceStr = " api";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceStr = " window system";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceStr = " shader compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceStr = " third party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceStr = " application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    default:
        assert(0);
    }

    const char *typeStr = "";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeStr = " error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeStr = " deprecated behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeStr = " undefined behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeStr = " portability issue";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeStr = " performance issue";
        break;
    default:
        assert(0);
        /* fall-through */
    case GL_DEBUG_TYPE_OTHER:
        typeStr = " issue";
        break;
    case GL_DEBUG_TYPE_MARKER:
        typeStr = " marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeStr = " push group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeStr = " pop group";
        break;
    }

    std::cerr << *color << retrace::callNo << ": message:" << severityStr << sourceStr << typeStr;

    if (id) {
        std::cerr << " " << id;
    }

    std::cerr << ": ";

    if (messageCount == maxMessageCount) {
        std::cerr << "too many identical messages; ignoring"
                  << highlighter.normal()
                  << std::endl;
        return;
    }

    std::cerr << message;

    std::cerr << highlighter.normal();

    // Write new line if not included in the message already.
    size_t messageLen = strlen(message);
    if (!messageLen ||
        (message[messageLen - 1] != '\n' &&
         message[messageLen - 1] != '\r')) {
       std::cerr << std::endl;
    }
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
    canDump(void) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        if (!currentContext ||
            currentContext->insideBeginEnd) {
            return false;
        }

        return true;
    }

    void
    dumpState(StateWriter &writer) {
        glstate::dumpCurrentContext(writer);
    }
};

static GLDumper glDumper;


void
retrace::setFeatureLevel(const char *featureLevel)
{
    glretrace::defaultProfile = glprofile::Profile(glprofile::API_GL, 3, 2, true);
}


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
    }
}

void
retrace::finishRendering(void) {
    if (profilingWithBackends && glretrace::curMetricBackend) {
            (glretrace::curMetricBackend)->endQuery(QUERY_BOUNDARY_FRAME);
    }

    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (currentContext) {
        glFinish();
    }

    if (retrace::profilingWithBackends) {
        if (glretrace::curMetricBackend) {
            (glretrace::curMetricBackend)->endPass();
        }
    }
}

void
retrace::waitForInput(void) {
    flushRendering();
    while (glws::processEvents()) {
        os::sleep(100*1000);
    }
}

void
retrace::cleanUp(void) {
    glws::cleanup();
}

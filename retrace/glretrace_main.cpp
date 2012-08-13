/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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

/* Synchronous debug output may reduce performance however,
 * without it the callNo in the callback may be inaccurate
 * as the callback may be called at any time.
 */
#define DEBUG_OUTPUT_SYNCHRONOUS 0

namespace glretrace {

bool insideList = false;
bool insideGlBeginEnd = false;

struct CallQuery
{
    GLuint ids[3];
    unsigned call;
    GLuint program;
    const trace::FunctionSig *sig;
    uint64_t cpuStart;
    uint64_t cpuEnd;
};

static bool supportsElapsed = true;
static bool supportsTimestamp = true;
static bool supportsOcclusion = true;
static bool supportsDebugOutput = true;

static bool firstFrame = true;
static std::list<CallQuery> callQueries;

static void APIENTRY
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);

void
checkGlError(trace::Call &call) {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return;
    }

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
}

static inline GLuint64
getGpuTimestamp() {
    GLuint query = 0;
    GLuint64 timestamp = 0;

    if (retrace::profilingGpuTimes && supportsTimestamp) {
        glGenQueries(1, &query);
        glQueryCounter(query, GL_TIMESTAMP);
        glGetQueryObjectui64vEXT(query, GL_QUERY_RESULT, &timestamp);
        glDeleteQueries(1, &query);
    }

    return timestamp;
}

static inline GLuint64
getCpuTimestamp() {
    if (retrace::profilingCpuTimes) {
        return os::getTime();
    } else {
        return 0;
    }
}

static void
completeCallQuery(CallQuery& query) {
    /* Get call start and duration */
    GLuint64 gpuStart = 0, gpuDuration = 0, cpuDuration = 0, samples = 0;

    if (retrace::profilingGpuTimes) {
        if (supportsTimestamp) {
            glGetQueryObjectui64vEXT(query.ids[0], GL_QUERY_RESULT, &gpuStart);
        }

        glGetQueryObjectui64vEXT(query.ids[1], GL_QUERY_RESULT, &gpuDuration);
    }

    if (retrace::profilingCpuTimes) {
        cpuDuration = query.cpuEnd - query.cpuStart;
    }

    if (retrace::profilingPixelsDrawn) {
        glGetQueryObjectui64vEXT(query.ids[2], GL_QUERY_RESULT, &samples);
    }

    glDeleteQueries(3, query.ids);

    /* Add call to profile */
    retrace::profiler.addCall(query.call, query.sig->name, query.program, samples, gpuStart, gpuDuration, query.cpuStart, cpuDuration);
}

void
flushQueries() {
    for (std::list<CallQuery>::iterator itr = callQueries.begin(); itr != callQueries.end(); ++itr) {
        completeCallQuery(*itr);
    }

    callQueries.clear();
}

void
beginProfile(trace::Call &call) {
    if (firstFrame) {
        frame_start();
    }

    /* Create call query */
    CallQuery query;
    query.call = call.no;
    query.sig = call.sig;
    query.program = glretrace::currentContext ? glretrace::currentContext->activeProgram : 0;

    glGenQueries(3, query.ids);

    if (retrace::profilingGpuTimes) {
        if (supportsTimestamp) {
            glQueryCounter(query.ids[0], GL_TIMESTAMP);
        }

        glBeginQuery(GL_TIME_ELAPSED, query.ids[1]);
    }

    if (retrace::profilingPixelsDrawn) {
        glBeginQuery(GL_SAMPLES_PASSED, query.ids[2]);
    }

    if (retrace::profilingCpuTimes) {
        query.cpuStart = getCpuTimestamp();
    }

    callQueries.push_back(query);
}

void
endProfile(trace::Call &call) {
    if (retrace::profilingCpuTimes) {
        CallQuery& query = callQueries.back();
        query.cpuEnd = getCpuTimestamp();
    }

    if (retrace::profilingGpuTimes) {
        glEndQuery(GL_TIME_ELAPSED);
    }

    if (retrace::profilingPixelsDrawn) {
        glEndQuery(GL_SAMPLES_PASSED);
    }
}

void
initContext() {
    GLuint64 gpuTime, cpuTime;
    const char* extensions;

    extensions = (const char*)glGetString(GL_EXTENSIONS);

    /* Ensure we have adequate extension support */
    supportsTimestamp   = glws::checkExtension("GL_ARB_timer_query", extensions);
    supportsElapsed     = glws::checkExtension("GL_EXT_timer_query", extensions) || supportsTimestamp;
    supportsOcclusion   = glws::checkExtension("GL_ARB_occlusion_query", extensions);
    supportsDebugOutput = glws::checkExtension("GL_ARB_debug_output", extensions);

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

    if (retrace::profilingPixelsDrawn && !supportsOcclusion) {
        std::cout << "Error: Cannot run profile, GL_ARB_occlusion_query extension is not supported." << std::endl;
        exit(-1);
    }

    if (retrace::debug && supportsDebugOutput) {
        glDebugMessageCallbackARB(&debugOutputCallback, currentContext);

        if (DEBUG_OUTPUT_SYNCHRONOUS) {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        }
    }

    /* Sync the gpu and cpu start times */
    gpuTime = getGpuTimestamp();
    cpuTime = getCpuTimestamp();
    retrace::profiler.setBaseTimes(gpuTime, cpuTime);
}

void
frame_start() {
    firstFrame = false;

    if (retrace::profiling) {
        retrace::profiler.addFrameStart(retrace::frameNo, getGpuTimestamp(), getCpuTimestamp());
    }
}

void
frame_complete(trace::Call &call) {
    if (retrace::profiling) {
        /* Complete any remaining queries */
        flushQueries();

        /* Indicate end of current frame */
        retrace::profiler.addFrameEnd(getGpuTimestamp(), getCpuTimestamp());
    }

    retrace::frameComplete(call);

    /* Indicate start of next frame */
    frame_start();

    if (!currentDrawable) {
        return;
    }

    if (retrace::debug && !currentDrawable->visible) {
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


void
retrace::setUp(void) {
    glws::init();
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


image::Image *
retrace::getSnapshot(void) {
    if (!glretrace::currentDrawable) {
        return NULL;
    }

    return glstate::getDrawBufferImage();
}


bool
retrace::dumpState(std::ostream &os)
{
    if (glretrace::insideGlBeginEnd ||
        !glretrace::currentDrawable ||
        !glretrace::currentContext) {
        return false;
    }

    glstate::dumpCurrentContext(os);

    return true;
}

void
retrace::flushRendering(void) {
    glretrace::flushQueries();
    glFlush();
}

void
retrace::waitForInput(void) {
    while (glws::processEvents()) {
    }
}

void
retrace::cleanUp(void) {
    glws::cleanup();
}

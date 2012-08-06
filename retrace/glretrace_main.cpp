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


namespace glretrace {

bool insideList = false;
bool insideGlBeginEnd = false;

struct CallQuery
{
    GLuint ids[3];
    unsigned call;
    GLuint program;
    const trace::FunctionSig *sig;
    uint64_t start;
    uint64_t duration;
};

static bool supportsElapsed = true;
static bool supportsTimestamp = true;
static bool supportsOcclusion = true;

static bool firstFrame = true;
static std::list<CallQuery> callQueries;
static std::map<glws::Context*, GLuint> activePrograms;


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

static GLuint64
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

static GLuint64
getCpuTimestamp() {
    if (retrace::profilingCpuTimes) {
        return os::getTime() * (1.0E9 / os::timeFrequency);
    } else {
        return 0;
    }
}

static void
completeCallQuery(CallQuery& query) {
    /* Get call start and duration */
    GLuint64 timestamp = 0, duration = 0, samples = 0;

    if (retrace::profilingGpuTimes) {
        if (supportsTimestamp) {
            glGetQueryObjectui64vEXT(query.ids[0], GL_QUERY_RESULT, &timestamp);
        }

        if (supportsElapsed) {
            glGetQueryObjectui64vEXT(query.ids[1], GL_QUERY_RESULT, &duration);
        }
    }

    if (retrace::profilingPixelsDrawn && supportsOcclusion) {
        glGetQueryObjectui64vEXT(query.ids[2], GL_QUERY_RESULT, &samples);
    }

    glDeleteQueries(3, query.ids);

    /* Add call to profile */
    retrace::profiler.addCall(query.call, query.sig->name, query.program, samples, timestamp, duration, query.start, query.duration);
}

void
flushQueries() {
    for (std::list<CallQuery>::iterator itr = callQueries.begin(); itr != callQueries.end(); ++itr) {
        completeCallQuery(*itr);
    }

    callQueries.clear();
}

void setActiveProgram(GLuint program)
{
    activePrograms[glretrace::currentContext] = program;
}

static GLuint
getActiveProgram()
{
    std::map<glws::Context*, GLuint>::iterator it;
    it = activePrograms.find(glretrace::currentContext);
    if (it == activePrograms.end())
        return 0;

    return it->second;
}

void
beginProfile(trace::Call &call) {
    if (firstFrame) {
        /* Check for extension support */
        const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
        GLint bits;

        supportsTimestamp = glws::checkExtension("GL_ARB_timer_query", extensions);
        supportsElapsed   = glws::checkExtension("GL_EXT_timer_query", extensions) || supportsTimestamp;
        supportsOcclusion = glws::checkExtension("GL_ARB_occlusion_query", extensions);

        if (retrace::profilingGpuTimes) {
            if (!supportsTimestamp && !supportsElapsed) {
                std::cout << "Error: Cannot run profile, GL_EXT_timer_query extension is not supported." << std::endl;
                exit(-1);
            }

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

        frame_start();
    }

    /* Create call query */
    CallQuery query;
    query.call = call.no;
    query.sig = call.sig;
    query.program = getActiveProgram();

    glGenQueries(3, query.ids);

    if (retrace::profilingGpuTimes) {
        if (supportsTimestamp) {
            glQueryCounter(query.ids[0], GL_TIMESTAMP);
        }

        if (supportsElapsed) {
            glBeginQuery(GL_TIME_ELAPSED, query.ids[1]);
        }
    }

    if (retrace::profilingPixelsDrawn && supportsOcclusion) {
        glBeginQuery(GL_SAMPLES_PASSED, query.ids[2]);
    }

    if (retrace::profilingCpuTimes) {
        query.start = os::getTime();
    }

    callQueries.push_back(query);
}

void
endProfile(trace::Call &call) {
    if (retrace::profilingCpuTimes) {
        CallQuery& query = callQueries.back();
        query.duration = (os::getTime() - query.start) * (1.0E9 / os::timeFrequency);
    }

    if (retrace::profilingGpuTimes && supportsElapsed) {
        glEndQuery(GL_TIME_ELAPSED);
    }

    if (retrace::profilingPixelsDrawn && supportsOcclusion) {
        glEndQuery(GL_SAMPLES_PASSED);
    }
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

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


namespace glretrace {

bool insideList = false;
bool insideGlBeginEnd = false;


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

struct CallQuery
{
    GLuint ids[2];
    unsigned call;
    const trace::FunctionSig *sig;
};

static const int maxActiveCallQueries = 256;
static std::list<CallQuery> callQueries;
static bool firstFrame = true;

static GLuint64
getTimestamp() {
    GLuint query;
    GLuint64 timestamp;

    glGenQueries(1, &query);

    glQueryCounter(query, GL_TIMESTAMP);
    glGetQueryObjectui64vEXT(query, GL_QUERY_RESULT, &timestamp);

    glDeleteQueries(1, &query);

    return timestamp;
}

static void
completeCallQuery(CallQuery& query) {
    /* Get call start and duration */
    GLuint64 timestamp, duration;
    glGetQueryObjectui64vEXT(query.ids[0], GL_QUERY_RESULT, &timestamp);
    glGetQueryObjectui64vEXT(query.ids[1], GL_QUERY_RESULT, &duration);
    glDeleteQueries(2, query.ids);

    /* Add call to profile */
    retrace::profiler.addCall(query.call, query.sig->name, timestamp, duration);
}

void
beginProfileGPU(trace::Call &call) {
    if (firstFrame) {
        frame_start();
    }

    /* Ensure we don't have TOO many queries waiting for results */
    if (callQueries.size() >= maxActiveCallQueries) {
        completeCallQuery(callQueries.front());
        callQueries.pop_front();
    }

    /* Create call query */
    CallQuery query;
    query.call = call.no;
    query.sig = call.sig;

    glGenQueries(2, query.ids);
    glQueryCounter(query.ids[0], GL_TIMESTAMP);
    glBeginQuery(GL_TIME_ELAPSED, query.ids[1]);

    callQueries.push_back(query);
}

void
endProfileGPU(trace::Call &call) {
    glEndQuery(GL_TIME_ELAPSED);
}

void
frame_start() {
    firstFrame = false;

    if (retrace::profileGPU) {
        retrace::profiler.addFrameStart(retrace::frameNo, getTimestamp());
    }
}

void
frame_complete(trace::Call &call) {
    if (retrace::profileGPU) {
        /* Complete any remaining queries */
        for (std::list<CallQuery>::iterator itr = callQueries.begin(); itr != callQueries.end(); ++itr) {
            completeCallQuery(*itr);
        }

        callQueries.clear();

        /* Indicate end of current frame */
        retrace::profiler.addFrameEnd(getTimestamp());
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

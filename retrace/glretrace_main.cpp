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

static std::vector<CallQuery> callQueries;
static GLuint frameQueries[2] = { 0, 0 };

void frame_start() {
   if (retrace::profileGPU) {
      glGenQueries(2, frameQueries);

      /* Query frame start time */
      glQueryCounter(frameQueries[0], GL_TIMESTAMP);
   }
}

void frame_complete(trace::Call &call) {
    if (retrace::profileGPU) {
        /* Query frame end time */
       glQueryCounter(frameQueries[1], GL_TIMESTAMP);

       completeQueries();
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

void
completeQueries()
{
   if (callQueries.size() == 0)
      return;

   GLint available;
   GLuint64 frameBegin, frameEnd;

   /* Wait for frame to finish */
   do {
      glGetQueryObjectiv(frameQueries[1], GL_QUERY_RESULT_AVAILABLE, &available);
   } while(!available);

   /* Get frame start and end */
   glGetQueryObjectui64vEXT(frameQueries[0], GL_QUERY_RESULT, &frameBegin);
   glGetQueryObjectui64vEXT(frameQueries[1], GL_QUERY_RESULT, &frameEnd);
   glDeleteQueries(2, frameQueries);

   /* Add frame to profile */
   retrace::profiler.addFrame(trace::Profiler::Frame(retrace::frameNo, frameBegin, frameEnd - frameBegin));

   /* Loop through all active call queries */
   for (std::vector<CallQuery>::iterator itr = callQueries.begin(); itr != callQueries.end(); ++itr) {
      CallQuery& query = *itr;
      GLuint64 timestamp, duration;

      /* Get queue start and duration */
      glGetQueryObjectui64vEXT(query.ids[0], GL_QUERY_RESULT, &timestamp);
      glGetQueryObjectui64vEXT(query.ids[1], GL_QUERY_RESULT, &duration);
      glDeleteQueries(2, query.ids);

      /* Add call to profile */
      retrace::profiler.addCall(trace::Profiler::Call(query.call, query.sig->name, timestamp, duration));
   }

   callQueries.clear();
}

void
beginProfileGPU(trace::Call &call) {
   if (frameQueries[0] == 0) {
      frame_start();
   }

   CallQuery query;
   query.call = call.no;
   query.sig = call.sig;

   /* Create start and duration queries */
   glGenQueries(2, query.ids);
   glQueryCounter(query.ids[0], GL_TIMESTAMP);
   glBeginQuery(GL_TIME_ELAPSED, query.ids[1]);

   callQueries.push_back(query);
}

void
endProfileGPU(trace::Call &call) {
   glEndQuery(GL_TIME_ELAPSED);
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

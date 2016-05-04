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

#pragma once

#include "glws.hpp"
#include "retrace.hpp"
#include "metric_backend.hpp"
#include "metric_writer.hpp"

#include "os_thread.hpp"


namespace glretrace {

class Context
{
public:
    Context(glws::Context* context)
        : wsContext(context)
    {
    }

private:
    unsigned refCount = 1;
    ~Context();

public:
    inline void
    aquire(void) {
        assert(refCount);
        ++refCount;
    }

    inline void
    release(void) {
        assert(refCount);
        if (--refCount == 0) {
            delete this;
        }
    }

    glws::Context* wsContext;

    // Bound drawable
    glws::Drawable *drawable = nullptr;

    // Active program (unswizzled) for profiling
    GLuint currentUserProgram = 0;

    // Current program (swizzled) for uniform swizzling
    GLuint currentProgram = 0;
    GLuint currentPipeline = 0;

    bool insideBeginEnd = false;
    bool insideList = false;
    bool needsFlush = false;

    bool used = false;

    bool KHR_debug = false;
    GLsizei maxDebugMessageLength = 0;

    inline glfeatures::Profile
    profile(void) const {
        return wsContext->profile;
    }

    inline glfeatures::Profile
    actualProfile(void) const {
        return wsContext->actualProfile;
    }

    inline const glfeatures::Features &
    features(void) const {
        return wsContext->actualFeatures;
    }

    inline bool
    hasExtension(const char *extension) const {
        return wsContext->hasExtension(extension);
    }
};

extern bool metricBackendsSetup;
extern bool profilingContextAcquired;
extern bool profilingBoundaries[QUERY_BOUNDARY_LIST_END];
extern unsigned profilingBoundariesIndex[QUERY_BOUNDARY_LIST_END];
extern std::vector<MetricBackend*> metricBackends;
extern MetricBackend* curMetricBackend;
extern MetricWriter profiler;

extern glfeatures::Profile defaultProfile;

extern bool supportsARBShaderObjects;

extern OS_THREAD_LOCAL Context *
currentContextPtr;


static inline Context *
getCurrentContext(void) {
    return currentContextPtr;
}


int
parseAttrib(const trace::Value *attribs, int param, int default_ = 0, int terminator = 0);

glfeatures::Profile
parseContextAttribList(const trace::Value *attribs);


glws::Drawable *
createDrawable(glfeatures::Profile profile);

glws::Drawable *
createDrawable(void);

glws::Drawable *
createPbuffer(int width, int height, const glws::pbuffer_info *info);

Context *
createContext(Context *shareContext, glfeatures::Profile profile);

Context *
createContext(Context *shareContext = 0);

bool
makeCurrent(trace::Call &call, glws::Drawable *drawable, Context *context);


void
checkGlError(trace::Call &call);

void
insertCallMarker(trace::Call &call, Context *currentContext);


extern const retrace::Entry gl_callbacks[];
extern const retrace::Entry cgl_callbacks[];
extern const retrace::Entry glx_callbacks[];
extern const retrace::Entry wgl_callbacks[];
extern const retrace::Entry egl_callbacks[];

void frame_complete(trace::Call &call);
void initContext();
void beforeContextSwitch();
void afterContextSwitch();


void updateDrawable(int width, int height);

void flushQueries();
void beginProfile(trace::Call &call, bool isDraw);
void endProfile(trace::Call &call, bool isDraw);

MetricBackend* getBackend(std::string backendName);

bool isLastPass();

void listMetricsCLI();

void enableMetricsFromCLI(const char* metrics, QueryBoundary pollingRule);

GLenum
blockOnFence(trace::Call &call, GLsync sync, GLbitfield flags);

GLenum
clientWaitSync(trace::Call &call, GLsync sync, GLbitfield flags, GLuint64 timeout);


// WGL_ARB_render_texture
bool
bindTexImage(glws::Drawable *pBuffer, int iBuffer);

// WGL_ARB_render_texture
bool
releaseTexImage(glws::Drawable *pBuffer, int iBuffer);

// WGL_ARB_render_texture
bool
setPbufferAttrib(glws::Drawable *pBuffer, const int *attribList);

} /* namespace glretrace */



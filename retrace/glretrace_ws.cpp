/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
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


/**
 * Window system helpers for glretrace.
 */


#include <string.h>

#include <algorithm>
#include <map>

#include "os_thread.hpp"
#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"


namespace glretrace {


static std::map<glfeatures::Profile, glws::Visual *>
visuals;


inline glws::Visual *
getVisual(glfeatures::Profile profile) {
    std::map<glfeatures::Profile, glws::Visual *>::iterator it = visuals.find(profile);
    if (it == visuals.end()) {
        glws::Visual *visual = NULL;
        unsigned requested_samples = retrace::samples > 1 ? retrace::samples :
                                                            (profile.samples ? profile.samples : 1);
        unsigned samples = requested_samples;
        /* The requested number of samples might not be available, try fewer until we succeed */
        while (!visual && samples > 0) {
            visual = glws::createVisual(retrace::doubleBuffer, samples, profile);
            if (!visual) {
                samples--;
            }
        }
        if (!visual) {
            std::cerr << "error: failed to create OpenGL visual\n";
            exit(1);
        }
        if (samples != requested_samples) {
            std::cerr << "warning: Using " << samples << " samples instead of the requested " << requested_samples << "\n";
        }
        visuals[profile] = visual;
        return visual;
    }
    return it->second;
}


static glws::Drawable *
createDrawableHelper(glfeatures::Profile profile, int width = 32, int height = 32,
                     const glws::pbuffer_info *pbInfo = NULL) {
    glws::Visual *visual = getVisual(profile);
    glws::Drawable *draw = glws::createDrawable(visual, width, height, pbInfo);
    if (!draw) {
        std::cerr << "error: failed to create OpenGL drawable\n";
        exit(1);
    }

    if (pbInfo)
        draw->pbInfo = *pbInfo;

    return draw;
}


glws::Drawable *
createDrawable(glfeatures::Profile profile) {
    return createDrawableHelper(profile);
}


glws::Drawable *
createDrawable(void) {
    return createDrawable(defaultProfile);
}


glws::Drawable *
createPbuffer(int width, int height, const glws::pbuffer_info *pbInfo) {
    // Zero area pbuffers are often accepted, but given we create window
    // drawables instead, they should have non-zero area.
    width  = std::max(width,  1);
    height = std::max(height, 1);

    return createDrawableHelper(defaultProfile, width, height, pbInfo);
}


Context *
createContext(Context *shareContext, glfeatures::Profile profile) {
    glws::Visual *visual = getVisual(profile);
    glws::Context *shareWsContext = shareContext ? shareContext->wsContext : NULL;
    glws::Context *ctx = glws::createContext(visual, shareWsContext, retrace::debug > 0);
    if (!ctx) {
        std::cerr << "error: failed to create " << profile << " context.\n";
        exit(1);
    }

    return new Context(ctx);
}


Context *
createContext(Context *shareContext) {
    return createContext(shareContext, defaultProfile);
}


Context::~Context()
{
    //assert(this != getCurrentContext());
    if (this != getCurrentContext()) {
        delete wsContext;
    }
}


OS_THREAD_LOCAL Context *
currentContextPtr;


bool
makeCurrent(trace::Call &call, glws::Drawable *drawable, Context *context)
{
    return makeCurrent(call, drawable, drawable, context);
}


bool
makeCurrent(trace::Call &call, glws::Drawable *drawable,
            glws::Drawable *readable, Context *context)
{
    Context *currentContext = currentContextPtr;
    glws::Drawable *currentDrawable = currentContext ? currentContext->drawable : NULL;
    glws::Drawable *currentReadable = currentContext ? currentContext->readable : NULL;

    if (drawable == currentDrawable &&
        readable == currentReadable &&
        context == currentContext) {
        return true;
    }

    if (currentContext) {
        glFlush();
        currentContext->needsFlush = false;
        if (!retrace::doubleBuffer) {
            frame_complete(call);
        }
    }

    flushQueries();

    beforeContextSwitch();

    bool success = glws::makeCurrent(drawable, readable, context ? context->wsContext : NULL);

    if (!success) {
        std::cerr << "error: failed to make current OpenGL context and drawable\n";
        exit(1);
    }

    if (context != currentContext) {
        if (context) {
            context->aquire();
        }
        currentContextPtr = context;
        if (currentContext) {
            currentContext->release();
        }
    }

    if (drawable && context) {
        context->drawable = drawable;
        context->readable = readable;

        if (!context->used) {
            initContext();
            context->used = true;
        }
    }

    afterContextSwitch();

    return true;
}


/**
 * Grow the current drawable.
 *
 * We need to infer the drawable size from GL calls because the drawable sizes
 * are specified by OS specific calls which we do not trace.
 */
void
updateDrawable(int width, int height) {
    Context *currentContext = getCurrentContext();
    if (!currentContext) {
        return;
    }

    glws::Drawable *currentDrawable = currentContext->drawable;
    if (!currentDrawable) {
        return;
    }

    if (currentDrawable->pbuffer) {
        return;
    }

    if (currentDrawable->visible &&
        width  <= currentDrawable->width &&
        height <= currentDrawable->height) {
        return;
    }

    // Ignore zero area viewports
    if (width == 0 || height == 0) {
        return;
    }

    width  = std::max(width,  currentDrawable->width);
    height = std::max(height, currentDrawable->height);

    // Check for bound framebuffer last, as this may have a performance impact.
    if (currentContext->features().framebuffer_object) {
        GLint framebuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
        if (framebuffer != 0) {
            return;
        }
    }

    currentDrawable->resize(width, height);
    currentDrawable->show();

    // Ensure the drawable dimensions, as perceived by glstate match.
    if (retrace::debug > 0) {
        GLint newWidth = 0;
        GLint newHeight = 0;
        if (glstate::getDrawableBounds(&newWidth, &newHeight) &&
            (newWidth != width || newHeight != height)) {
            std::cerr
                << "error: drawable failed to resize: "
                << "expected " << width << "x" << height << ", "
                << "got " << newWidth << "x" << newHeight << "\n";
        }
    }

    glScissor(0, 0, width, height);
}


int
parseAttrib(const trace::Value *attribs, int param, int default_, int terminator) {
    const trace::Array *attribs_ = attribs ? attribs->toArray() : NULL;

    if (attribs_) {
        for (size_t i = 0; i + 1 < attribs_->values.size(); i += 2) {
            int param_i = attribs_->values[i]->toSInt();
            if (param_i == terminator) {
                break;
            }

            if (param_i == param) {
                int value = attribs_->values[i + 1]->toSInt();
                return value;
            }
        }
    }

    return default_;
}

void
setSamples(trace::Call& call, int samples)
{
    if (samples > 0)  {
        if (retrace::samples == 1)
            retrace::samples = samples;
        else {
            retrace::warning(call) << "Overriding samples value " << samples
                                   << " found in trace with command line parameter "
                                   << retrace::samples << "\n";
        }
    }
}


/**
 * Parse GLX/WGL_ARB_create_context attribute list.
 */
glfeatures::Profile
parseContextAttribList(const trace::Value *attribs)
{
    // {GLX,WGL}_CONTEXT_MAJOR_VERSION_ARB
    int major_version = parseAttrib(attribs, 0x2091, 1);

    // {GLX,WGL}_CONTEXT_MINOR_VERSION_ARB
    int minor_version = parseAttrib(attribs, 0x2092, 0);

    int profile_mask = parseAttrib(attribs, GL_CONTEXT_PROFILE_MASK, GL_CONTEXT_CORE_PROFILE_BIT);

    bool core_profile = profile_mask & GL_CONTEXT_CORE_PROFILE_BIT;
    if (major_version < 3 ||
        (major_version == 3 && minor_version < 2)) {
        core_profile = false;
    }

    int context_flags = parseAttrib(attribs, GL_CONTEXT_FLAGS, 0);

    bool forward_compatible = context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT;
    if (major_version < 3) {
        forward_compatible = false;
    }

    // {GLX,WGL}_CONTEXT_ES_PROFILE_BIT_EXT
    bool es_profile = profile_mask & 0x0004;

    glfeatures::Profile profile;
    if (es_profile) {
        profile.api = glfeatures::API_GLES;
    } else {
        profile.api = glfeatures::API_GL;
        profile.core = core_profile;
        profile.forwardCompatible = forward_compatible;
    }

    profile.major = major_version;
    profile.minor = minor_version;

    return profile;
}


// WGL_ARB_render_texture / wglBindTexImageARB()
bool
bindTexImage(glws::Drawable *pBuffer, int iBuffer) {
    return glws::bindTexImage(pBuffer, iBuffer);
}

// WGL_ARB_render_texture / wglReleaseTexImageARB()
bool
releaseTexImage(glws::Drawable *pBuffer, int iBuffer) {
    return glws::releaseTexImage(pBuffer, iBuffer);
}

// WGL_ARB_render_texture / wglSetPbufferAttribARB()
bool
setPbufferAttrib(glws::Drawable *pBuffer, const int *attribs) {
    return glws::setPbufferAttrib(pBuffer, attribs);
}

} /* namespace glretrace */

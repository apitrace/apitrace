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


static std::map<glprofile::Profile, glws::Visual *>
visuals;


inline glws::Visual *
getVisual(glprofile::Profile profile) {
    std::map<glprofile::Profile, glws::Visual *>::iterator it = visuals.find(profile);
    if (it == visuals.end()) {
        glws::Visual *visual = NULL;
        unsigned samples = retrace::samples;
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
        if (samples != retrace::samples) {
            std::cerr << "warning: Using " << samples << " samples instead of the requested " << retrace::samples << "\n";
        }
        visuals[profile] = visual;
        return visual;
    }
    return it->second;
}


static glws::Drawable *
createDrawableHelper(glprofile::Profile profile, int width = 32, int height = 32, bool pbuffer = false) {
    glws::Visual *visual = getVisual(profile);
    glws::Drawable *draw = glws::createDrawable(visual, width, height, pbuffer);
    if (!draw) {
        std::cerr << "error: failed to create OpenGL drawable\n";
        exit(1);
    }

    return draw;
}


glws::Drawable *
createDrawable(glprofile::Profile profile) {
    return createDrawableHelper(profile);
}


glws::Drawable *
createDrawable(void) {
    return createDrawable(defaultProfile);
}


glws::Drawable *
createPbuffer(int width, int height) {
    // Zero area pbuffers are often accepted, but given we create window
    // drawables instead, they should have non-zero area.
    width  = std::max(width,  1);
    height = std::max(height, 1);

    return createDrawableHelper(defaultProfile, width, height, true);
}


Context *
createContext(Context *shareContext, glprofile::Profile profile) {
    glws::Visual *visual = getVisual(profile);
    glws::Context *shareWsContext = shareContext ? shareContext->wsContext : NULL;
    glws::Context *ctx = glws::createContext(visual, shareWsContext, retrace::debug);
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


OS_THREAD_SPECIFIC_PTR(Context)
currentContextPtr;


bool
makeCurrent(trace::Call &call, glws::Drawable *drawable, Context *context)
{
    Context *currentContext = currentContextPtr;
    glws::Drawable *currentDrawable = currentContext ? currentContext->drawable : NULL;

    if (drawable == currentDrawable && context == currentContext) {
        return true;
    }

    if (currentContext) {
        glFlush();
        if (!retrace::doubleBuffer) {
            frame_complete(call);
        }
    }

    flushQueries();

    beforeContextSwitch();

    bool success = glws::makeCurrent(drawable, context ? context->wsContext : NULL);

    if (!success) {
        std::cerr << "error: failed to make current OpenGL context and drawable\n";
        exit(1);
    }

    currentContextPtr = context;

    if (drawable && context) {
        context->drawable = drawable;
        
        if (!context->used) {
            initContext();
            context->used = true;
        }
    }

    afterContextSwitch();

    return true;
}


/**
 * Grow the current drawble.
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
    assert(currentDrawable);

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
    GLint draw_framebuffer = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_framebuffer);
    if (draw_framebuffer != 0) {
        return;
    }

    currentDrawable->resize(width, height);
    currentDrawable->show();

    // Ensure the drawable dimensions, as perceived by glstate match.
    if (retrace::debug) {
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


/**
 * Parse GLX/WGL_ARB_create_context attribute list.
 */
glprofile::Profile
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

    glprofile::Profile profile;
    if (es_profile) {
        profile.api = glprofile::API_GLES;
    } else {
        profile.api = glprofile::API_GL;
        profile.core = core_profile;
        profile.forwardCompatible = forward_compatible;
    }

    profile.major = major_version;
    profile.minor = minor_version;

    return profile;
}


} /* namespace glretrace */

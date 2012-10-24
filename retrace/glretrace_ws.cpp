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

#include "os_thread.hpp"
#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"


namespace glretrace {


static glws::Visual *
visuals[glws::PROFILE_MAX];


inline glws::Visual *
getVisual(glws::Profile profile) {
    glws::Visual * & visual = visuals[profile];
    if (!visual) {
        visual = glws::createVisual(retrace::doubleBuffer, profile);
        if (!visual) {
            std::cerr << "error: failed to create OpenGL visual\n";
            exit(1);
        }
    }
    return visual;
}


inline glws::Profile
getDefaultProfile(void)
{
    if (retrace::coreProfile) {
        return glws::PROFILE_CORE;
    } else {
        return glws::PROFILE_COMPAT;
    }
}


static glws::Drawable *
createDrawableHelper(glws::Profile profile, int width = 32, int height = 32, bool pbuffer = false) {
    glws::Visual *visual = getVisual(profile);
    glws::Drawable *draw = glws::createDrawable(visual, width, height, pbuffer);
    if (!draw) {
        std::cerr << "error: failed to create OpenGL drawable\n";
        exit(1);
    }

    return draw;
}


glws::Drawable *
createDrawable(glws::Profile profile) {
    return createDrawableHelper(profile);
}


glws::Drawable *
createDrawable(void) {
    return createDrawable(getDefaultProfile());
}


glws::Drawable *
createPbuffer(int width, int height) {
    return createDrawableHelper(getDefaultProfile(), width, height, true);
}


Context *
createContext(Context *shareContext, glws::Profile profile) {
    glws::Visual *visual = getVisual(profile);
    glws::Context *shareWsContext = shareContext ? shareContext->wsContext : NULL;
    glws::Context *ctx = glws::createContext(visual, shareWsContext, profile, retrace::debug);
    if (!ctx) {
        std::cerr << "error: failed to create OpenGL context\n";
        exit(1);
        return NULL;
    }

    return new Context(ctx);
}


Context *
createContext(Context *shareContext) {
    return createContext(shareContext, getDefaultProfile());
}


Context::~Context()
{
    //assert(this != getCurrentContext());
    if (this != getCurrentContext()) {
        delete wsContext;
    }
}


static thread_specific Context *
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

    bool success = glws::makeCurrent(drawable, context ? context->wsContext : NULL);

    if (!success) {
        std::cerr << "error: failed to make current OpenGL context and drawable\n";
        exit(1);
        return false;
    }

    currentContextPtr = context;

    if (drawable && context) {
        context->drawable = drawable;
        
        if (!context->used) {
            initContext();
            context->used = true;
        }
    }

    return true;
}


Context *
getCurrentContext(void) {
    return currentContextPtr;
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

    // Check for bound framebuffer last, as this may have a performance impact.
    GLint draw_framebuffer = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_framebuffer);
    if (draw_framebuffer != 0) {
        return;
    }

    currentDrawable->resize(width, height);
    currentDrawable->show();

    glScissor(0, 0, width, height);
}


int
parseAttrib(const trace::Value *attribs, int param, int default_ = 0) {
    const trace::Array *attribs_ = dynamic_cast<const trace::Array *>(attribs);

    if (attribs_) {
        for (size_t i = 0; i + 1 < attribs_->values.size(); i += 2) {
            int param_i = attribs_->values[i]->toSInt();
            if (param_i == 0) {
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


} /* namespace glretrace */

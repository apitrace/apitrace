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
#include "os_time.hpp"
#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"


namespace glretrace {


static glws::WindowSystem *winsys = NULL;


static inline void
init(gldispatch::Profile profile) {
    if (!winsys) {
        const char *description;
        switch (profile) {
        case gldispatch::PROFILE_ES1:
        case gldispatch::PROFILE_ES2:
            description = "EGL";
            winsys = glws::createEglWindowSystem();
            break;
        default:
            description = "native";
            winsys = glws::createNativeWindowSystem();
        }
        if (!winsys) {
            std::cerr << "error: failed to initialze " << description << " windowing system\n";
            exit(1);
        }
    }
}


static glws::Visual *
visuals[gldispatch::PROFILE_MAX];


inline glws::Visual *
getVisual(gldispatch::Profile profile) {
    init(profile);

    glws::Visual * & visual = visuals[profile];
    if (!visual) {
        visual = winsys->createVisual(retrace::doubleBuffer, profile);
        if (!visual) {
            std::cerr << "error: failed to create OpenGL visual\n";
            exit(1);
        }
    }
    return visual;
}


inline gldispatch::Profile
getDefaultProfile(void) {
    if (retrace::coreProfile) {
        return gldispatch::PROFILE_CORE;
    } else {
        return gldispatch::PROFILE_COMPAT;
    }
}


static glws::Drawable *
createDrawableHelper(gldispatch::Profile profile, int width = 32, int height = 32, bool pbuffer = false) {
    glws::Visual *visual = getVisual(profile);
    glws::Drawable *draw = winsys->createDrawable(visual, width, height, pbuffer);
    if (!draw) {
        std::cerr << "error: failed to create OpenGL drawable\n";
        exit(1);
    }

    return draw;
}


glws::Drawable *
createDrawable(gldispatch::Profile profile) {
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
createContext(Context *shareContext, gldispatch::Profile profile) {
    init(profile);

    glws::Visual *visual = getVisual(profile);
    glws::Context *shareWsContext = shareContext ? shareContext->wsContext : NULL;
    glws::Context *ctx = winsys->createContext(visual, shareWsContext, profile, retrace::debug);
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


static OS_THREAD_SPECIFIC_PTR(Context)
currentContextPtr;


bool
makeCurrent(trace::Call &call, glws::Drawable *drawable, Context *context)
{
    assert(winsys);
    
    Context *currentContext = currentContextPtr;
    glws::Drawable *currentDrawable = currentContext ? currentContext->drawable : NULL;

    if (drawable == currentDrawable && context == currentContext) {
        return true;
    }

    bool success;
    if (winsys) {
        if (currentDrawable && currentContext) {
            glFlush();
            if (!retrace::doubleBuffer) {
                frame_complete(call);
            }
        }

        flushQueries();

        success = winsys->makeCurrent(drawable, context ? context->wsContext : NULL);
    } else {
        success = false;
    }

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


void
retrace::waitForInput(void) {
    using namespace glretrace;

    glretrace::Context *currentContext = glretrace::getCurrentContext();
    if (currentContext) {
        glretrace::flushQueries();
        glFlush();
    }
    while (winsys->processEvents()) {
        os::sleep(100*1000);
    }
}

void
retrace::cleanUp(void) {
    using namespace glretrace;

    if (winsys) {
        winsys->makeCurrent(NULL, NULL);

        currentContextPtr = NULL;

        delete winsys;

        winsys = NULL;
    }
}

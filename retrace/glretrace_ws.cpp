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

#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"


namespace glretrace {


glws::Drawable *currentDrawable = NULL;
glws::Context *currentContext = NULL;


static glws::Visual *
visuals[glws::PROFILE_MAX];


inline glws::Visual *
getVisual(glws::Profile profile) {
    glws::Visual * & visual = visuals[profile];
    if (!visual) {
        visual = glws::createVisual(retrace::doubleBuffer, profile);
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


glws::Drawable *
createDrawable(glws::Profile profile) {
    glws::Drawable *draw = glws::createDrawable(getVisual(profile));
    if (!draw) {
        std::cerr << "error: failed to create OpenGL drawable\n";
        exit(1);
        return NULL;
    }

    return draw;
}


glws::Drawable *
createDrawable(void) {
    return glretrace::createDrawable(getDefaultProfile());
}


glws::Context *
createContext(glws::Context *shareContext, glws::Profile profile) {
    glws::Context *ctx = glws::createContext(getVisual(profile), shareContext, profile, retrace::debug);
    if (!ctx) {
        std::cerr << "error: failed to create OpenGL context\n";
        exit(1);
        return NULL;
    }

    return ctx;
}


glws::Context *
createContext(glws::Context *shareContext) {
    return createContext(shareContext, getDefaultProfile());
}


bool
makeCurrent(trace::Call &call, glws::Drawable *drawable, glws::Context *context)
{
    if (drawable == currentDrawable && context == currentContext) {
        return true;
    }

    if (currentDrawable && currentContext) {
        glFlush();
        if (!retrace::doubleBuffer) {
            frame_complete(call);
        }
    }

    bool success = glws::makeCurrent(drawable, context);

    if (!success) {
        std::cerr << "error: failed to make current OpenGL context and drawable\n";
        exit(1);
        return false;
    }

    if (drawable && context) {
        currentDrawable = drawable;
        currentContext = context;
    } else {
        currentDrawable = NULL;
        currentContext = NULL;
    }

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
    if (!currentDrawable) {
        return;
    }

    if (!currentDrawable->visible &&
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


} /* namespace glretrace */

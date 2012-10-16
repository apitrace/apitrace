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

#include "glproc.hpp"
#include "retrace.hpp"
#include "glretrace.hpp"


using namespace glretrace;


typedef std::map<unsigned long long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, Context *> ContextMap;
static DrawableMap drawable_map;
static ContextMap context_map;
static Context *sharedContext = NULL;


static glws::Drawable *
getDrawable(unsigned long drawable_id) {
    if (drawable_id == 0) {
        return NULL;
    }

    /* XXX: Support multiple drawables. */
    drawable_id = 1;

    DrawableMap::const_iterator it;
    it = drawable_map.find(drawable_id);
    if (it == drawable_map.end()) {
        return (drawable_map[drawable_id] = glretrace::createDrawable());
    }

    return it->second;
}


static Context *
getContext(unsigned long long ctx) {
    if (ctx == 0) {
        return NULL;
    }

    ContextMap::const_iterator it;
    it = context_map.find(ctx);
    if (it == context_map.end()) {
        Context *context;
        context_map[ctx] = context = glretrace::createContext(sharedContext);
        if (!sharedContext) {
            sharedContext = context;
        }
        return context;
    }

    return it->second;
}

static void retrace_CGLCreateContext(trace::Call &call) {
    unsigned long long share = call.arg(1).toUIntPtr();
    Context *sharedContext = getContext(share);

    const trace::Array *ctx_ptr = dynamic_cast<const trace::Array *>(&call.arg(2));
    unsigned long long ctx = ctx_ptr->values[0]->toUIntPtr();

    Context *context = glretrace::createContext(sharedContext);
    context_map[ctx] = context;
}


static void retrace_CGLDestroyContext(trace::Call &call) {
    unsigned long long ctx = call.arg(0).toUIntPtr();

    ContextMap::iterator it;
    it = context_map.find(ctx);
    if (it == context_map.end()) {
        return;
    }

    delete it->second;

    context_map.erase(it);
}


static void retrace_CGLSetCurrentContext(trace::Call &call) {
    unsigned long long ctx = call.arg(0).toUIntPtr();

    glws::Drawable *new_drawable = getDrawable(ctx);
    Context *new_context = getContext(ctx);

    glretrace::makeCurrent(call, new_drawable, new_context);
}


static void retrace_CGLFlushDrawable(trace::Call &call) {
    if (currentDrawable && currentContext) {
        if (retrace::doubleBuffer) {
            currentDrawable->swapBuffers();
        } else {
            glFlush();
        }

        frame_complete(call);
    }
}


/**
 * We can't fully reimplement CGLTexImageIOSurface2D, as external IOSurface are
 * no longer present.  Simply emit a glTexImage2D to ensure the texture storage
 * is present.
 *
 * See also:
 * - /System/Library/Frameworks/OpenGL.framework/Headers/CGLIOSurface.h
 */
static void retrace_CGLTexImageIOSurface2D(trace::Call &call) {
    if (retrace::debug) {
        retrace::warning(call) << "external IOSurface not supported\n";
    }

    unsigned long long ctx = call.arg(0).toUIntPtr();
    Context *context = getContext(ctx);

    GLenum target;
    target = static_cast<GLenum>((call.arg(1)).toSInt());

    GLint level = 0;

    GLint internalformat;
    internalformat = static_cast<GLenum>((call.arg(2)).toSInt());

    GLsizei width;
    width = (call.arg(3)).toSInt();

    GLsizei height;
    height = (call.arg(4)).toSInt();

    GLint border = 0;

    GLenum format;
    format = static_cast<GLenum>((call.arg(5)).toSInt());

    GLenum type;
    type = static_cast<GLenum>((call.arg(6)).toSInt());

    GLvoid * pixels = NULL;

    if (glretrace::currentContext != context) {
        if (retrace::debug) {
            retrace::warning(call) << "current context mismatch\n";
        }
    }

    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);

    if (retrace::debug && !glretrace::insideGlBeginEnd) {
        glretrace::checkGlError(call);
    }
}


const retrace::Entry glretrace::cgl_callbacks[] = {
    {"CGLCreateContext", &retrace_CGLCreateContext},
    {"CGLDestroyContext", &retrace_CGLDestroyContext},
    {"CGLSetCurrentContext", &retrace_CGLSetCurrentContext},
    {"CGLGetCurrentContext", &retrace::ignore},
    {"CGLEnable", &retrace::ignore},
    {"CGLDisable", &retrace::ignore},
    {"CGLSetParameter", &retrace::ignore},
    {"CGLGetParameter", &retrace::ignore},
    {"CGLFlushDrawable", &retrace_CGLFlushDrawable},
    {"CGLTexImageIOSurface2D", &retrace_CGLTexImageIOSurface2D},
    {NULL, NULL},
};


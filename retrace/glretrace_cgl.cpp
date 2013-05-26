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


#define kCGLPFAAllRenderers            1
#define kCGLPFADoubleBuffer            5
#define kCGLPFAStereo                  6
#define kCGLPFAAuxBuffers              7
#define kCGLPFAColorSize               8
#define kCGLPFAAlphaSize              11
#define kCGLPFADepthSize              12
#define kCGLPFAStencilSize            13
#define kCGLPFAAccumSize              14
#define kCGLPFAMinimumPolicy          51
#define kCGLPFAMaximumPolicy          52
#define kCGLPFAOffScreen              53
#define kCGLPFAFullScreen             54
#define kCGLPFASampleBuffers          55
#define kCGLPFASamples                56
#define kCGLPFAAuxDepthStencil        57
#define kCGLPFAColorFloat             58
#define kCGLPFAMultisample            59
#define kCGLPFASupersample            60
#define kCGLPFASampleAlpha            61
#define kCGLPFARendererID             70
#define kCGLPFASingleRenderer         71
#define kCGLPFANoRecovery             72
#define kCGLPFAAccelerated            73
#define kCGLPFAClosestPolicy          74
#define kCGLPFARobust                 75
#define kCGLPFABackingStore           76
#define kCGLPFAMPSafe                 78
#define kCGLPFAWindow                 80
#define kCGLPFAMultiScreen            81
#define kCGLPFACompliant              83
#define kCGLPFADisplayMask            84
#define kCGLPFAPBuffer                90
#define kCGLPFARemotePBuffer          91
#define kCGLPFAAllowOfflineRenderers  96
#define kCGLPFAAcceleratedCompute     97
#define kCGLPFAOpenGLProfile          99
#define kCGLPFAVirtualScreenCount    128

#define kCGLOGLPVersion_Legacy   0x1000
#define kCGLOGLPVersion_3_2_Core 0x3200


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


static void retrace_CGLChoosePixelFormat(trace::Call &call) {
    int profile = kCGLOGLPVersion_Legacy;

    const trace::Array * attribs = dynamic_cast<const trace::Array *>(&call.arg(0));
    if (attribs) {
        size_t i = 0;
        while (i < attribs->values.size()) {
            int param = attribs->values[i++]->toSInt();
            if (param == 0) {
                break;
            }

            switch (param) {
            case kCGLPFAAllRenderers:
            case kCGLPFADoubleBuffer:
            case kCGLPFAStereo:
            case kCGLPFAAuxBuffers:
            case kCGLPFAMinimumPolicy:
            case kCGLPFAMaximumPolicy:
            case kCGLPFAOffScreen:
            case kCGLPFAFullScreen:
            case kCGLPFAAuxDepthStencil:
            case kCGLPFAColorFloat:
            case kCGLPFAMultisample:
            case kCGLPFASupersample:
            case kCGLPFASampleAlpha:
            case kCGLPFASingleRenderer:
            case kCGLPFANoRecovery:
            case kCGLPFAAccelerated:
            case kCGLPFAClosestPolicy:
            case kCGLPFARobust:
            case kCGLPFABackingStore:
            case kCGLPFAMPSafe:
            case kCGLPFAWindow:
            case kCGLPFAMultiScreen:
            case kCGLPFACompliant:
            case kCGLPFAPBuffer:
            case kCGLPFARemotePBuffer:
            case kCGLPFAAllowOfflineRenderers:
            case kCGLPFAAcceleratedCompute:
                break;

            case kCGLPFAColorSize:
            case kCGLPFAAlphaSize:
            case kCGLPFADepthSize:
            case kCGLPFAStencilSize:
            case kCGLPFAAccumSize:
            case kCGLPFASampleBuffers:
            case kCGLPFASamples:
            case kCGLPFARendererID:
            case kCGLPFADisplayMask:
            case kCGLPFAVirtualScreenCount:
                ++i;
                break;

            case kCGLPFAOpenGLProfile:
                profile = attribs->values[i++]->toSInt();
                break;

            default:
                retrace::warning(call) << "unexpected attribute " << param << "\n";
                break;
            }
        }
    }

    if (profile == kCGLOGLPVersion_3_2_Core) {
        // TODO: Do this on a per visual basis
        retrace::coreProfile = true;
    }
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
    unsigned long long ctx = call.arg(0).toUIntPtr();
    Context *context = getContext(ctx);

    if (context) {
        if (context->drawable) {
            if (retrace::doubleBuffer) {
                context->drawable->swapBuffers();
            } else {
                glFlush();
            }
            frame_complete(call);
        } else {
            if (retrace::debug) {
                retrace::warning(call) << "context has no drawable\n";
            }
        }
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

    if (glretrace::getCurrentContext() != context) {
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
    {"CGLChoosePixelFormat", &retrace_CGLChoosePixelFormat},
    {"CGLCreateContext", &retrace_CGLCreateContext},
    {"CGLDestroyContext", &retrace_CGLDestroyContext},
    {"CGLDestroyPixelFormat", &retrace::ignore},
    {"CGLDisable", &retrace::ignore},
    {"CGLEnable", &retrace::ignore},
    {"CGLErrorString", &retrace::ignore},
    {"CGLFlushDrawable", &retrace_CGLFlushDrawable},
    {"CGLGetCurrentContext", &retrace::ignore},
    {"CGLGetOption", &retrace::ignore},
    {"CGLGetParameter", &retrace::ignore},
    {"CGLGetVersion", &retrace::ignore},
    {"CGLGetVirtualScreen", &retrace::ignore},
    {"CGLIsEnabled", &retrace::ignore},
    {"CGLSetCurrentContext", &retrace_CGLSetCurrentContext},
    {"CGLSetParameter", &retrace::ignore},
    {"CGLTexImageIOSurface2D", &retrace_CGLTexImageIOSurface2D},
    {"CGLUpdateContext", &retrace::ignore},
    {NULL, NULL},
};


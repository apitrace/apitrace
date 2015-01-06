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


#include "glproc.hpp"
#include "retrace.hpp"
#include "glretrace.hpp"


using namespace glretrace;


typedef std::map<unsigned long long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, Context *> ContextMap;
static DrawableMap drawable_map;
static DrawableMap pbuffer_map;
static ContextMap context_map;


static glws::Drawable *
getDrawable(unsigned long long hdc) {
    if (hdc == 0) {
        return NULL;
    }

    DrawableMap::const_iterator it;
    it = drawable_map.find(hdc);
    if (it == drawable_map.end()) {
        return (drawable_map[hdc] = glretrace::createDrawable());
    }

    return it->second;
}

static Context *
getContext(unsigned long long context_ptr) {
    if (context_ptr == 0) {
        return NULL;
    }

    ContextMap::const_iterator it;
    it = context_map.find(context_ptr);
    if (it == context_map.end()) {
        assert(false);
        return NULL;
    }

    return it->second;
}

static void retrace_wglCreateContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *context = glretrace::createContext();
    context_map[orig_context] = context;
}

static void retrace_wglDeleteContext(trace::Call &call) {
    unsigned long long hglrc = call.arg(0).toUIntPtr();

    ContextMap::iterator it;
    it = context_map.find(hglrc);
    if (it == context_map.end()) {
        return;
    }

    delete it->second;
    
    context_map.erase(it);
}

static void retrace_wglMakeCurrent(trace::Call &call) {
    bool ret = call.ret->toBool();

    glws::Drawable *new_drawable = NULL;
    Context *new_context = NULL;
    if (ret) {
        unsigned long long hglrc = call.arg(1).toUIntPtr();
        if (hglrc) {
            new_drawable = getDrawable(call.arg(0).toUIntPtr());
            new_context = getContext(hglrc);
        }
    }

    glretrace::makeCurrent(call, new_drawable, new_context);
}

static void retrace_wglSwapBuffers(trace::Call &call) {
    bool ret = call.ret->toBool();
    if (!ret) {
        return;
    }

    glws::Drawable *drawable = getDrawable(call.arg(0).toUIntPtr());

    frame_complete(call);
    if (retrace::doubleBuffer) {
        if (drawable) {
            drawable->swapBuffers();
        } else {
            glretrace::Context *currentContext = glretrace::getCurrentContext();
            if (currentContext) {
                currentContext->drawable->swapBuffers();
            }
        }
    } else {
        glFlush();
    }
}

static void retrace_wglShareLists(trace::Call &call) {
    bool ret = call.ret->toBool();
    if (!ret) {
        return;
    }

    unsigned long long hglrc1 = call.arg(0).toUIntPtr();
    unsigned long long hglrc2 = call.arg(1).toUIntPtr();

    Context *share_context = getContext(hglrc1);
    Context *old_context = getContext(hglrc2);

    glprofile::Profile profile = old_context->wsContext->profile;
    Context *new_context = glretrace::createContext(share_context, profile);
    if (new_context) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        if (currentContext == old_context) {
            glretrace::makeCurrent(call, currentContext->drawable, new_context);
        }

        context_map[hglrc2] = new_context;
        
        delete old_context;
    }
}

static void retrace_wglCreateLayerContext(trace::Call &call) {
    retrace_wglCreateContext(call);
}

static void retrace_wglSwapLayerBuffers(trace::Call &call) {
    retrace_wglSwapBuffers(call);
}

static void retrace_wglCreatePbufferARB(trace::Call &call) {
    unsigned long long orig_pbuffer = call.ret->toUIntPtr();
    if (!orig_pbuffer) {
        return;
    }

    int iWidth = call.arg(2).toUInt();
    int iHeight = call.arg(3).toUInt();

    glws::Drawable *drawable = glretrace::createPbuffer(iWidth, iHeight);

    pbuffer_map[orig_pbuffer] = drawable;
}

static void retrace_wglGetPbufferDCARB(trace::Call &call) {
    unsigned long long orig_hdc = call.ret->toUIntPtr();
    if (!orig_hdc) {
        return;
    }

    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];

    drawable_map[orig_hdc] = pbuffer;
}

static void retrace_wglCreateContextAttribsARB(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *share_context = getContext(call.arg(1).toUIntPtr());

    const trace::Value * attribList = &call.arg(2);
    glprofile::Profile profile = parseContextAttribList(attribList);

    Context *context = glretrace::createContext(share_context, profile);
    context_map[orig_context] = context;
}

const retrace::Entry glretrace::wgl_callbacks[] = {
    {"glAddSwapHintRectWIN", &retrace::ignore},
    {"wglChoosePixelFormat", &retrace::ignore},
    {"wglChoosePixelFormatARB", &retrace::ignore},
    {"wglChoosePixelFormatEXT", &retrace::ignore},
    {"wglCreateContext", &retrace_wglCreateContext},
    {"wglCreateContextAttribsARB", &retrace_wglCreateContextAttribsARB},
    {"wglCreateLayerContext", &retrace_wglCreateLayerContext},
    {"wglCreatePbufferARB", &retrace_wglCreatePbufferARB},
    {"wglDeleteContext", &retrace_wglDeleteContext},
    {"wglDescribeLayerPlane", &retrace::ignore},
    {"wglDescribePixelFormat", &retrace::ignore},
    {"wglDestroyPbufferARB", &retrace::ignore},
    {"wglGetCurrentContext", &retrace::ignore},
    {"wglGetCurrentDC", &retrace::ignore},
    {"wglGetCurrentReadDCARB", &retrace::ignore},
    {"wglGetCurrentReadDCEXT", &retrace::ignore},
    {"wglGetDefaultProcAddress", &retrace::ignore},
    {"wglGetExtensionsStringARB", &retrace::ignore},
    {"wglGetExtensionsStringEXT", &retrace::ignore},
    {"wglGetLayerPaletteEntries", &retrace::ignore},
    {"wglGetPbufferDCARB", &retrace_wglGetPbufferDCARB},
    {"wglGetPixelFormat", &retrace::ignore},
    {"wglGetPixelFormatAttribfvARB", &retrace::ignore},
    {"wglGetPixelFormatAttribfvEXT", &retrace::ignore},
    {"wglGetPixelFormatAttribivARB", &retrace::ignore},
    {"wglGetPixelFormatAttribivEXT", &retrace::ignore},
    {"wglGetProcAddress", &retrace::ignore},
    {"wglGetSwapIntervalEXT", &retrace::ignore},
    {"wglMakeCurrent", &retrace_wglMakeCurrent},
    {"wglQueryPbufferARB", &retrace::ignore},
    {"wglReleasePbufferDCARB", &retrace::ignore},
    {"wglReleaseTexImageARB", &retrace::ignore},
    {"wglSetPbufferAttribARB", &retrace::ignore},
    {"wglSetPixelFormat", &retrace::ignore},
    {"wglShareLists", &retrace_wglShareLists},
    {"wglSwapBuffers", &retrace_wglSwapBuffers},
    {"wglSwapIntervalEXT", &retrace::ignore},
    {"wglSwapLayerBuffers", &retrace_wglSwapLayerBuffers},
    {NULL, NULL}
};


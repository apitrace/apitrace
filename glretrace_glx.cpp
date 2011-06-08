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


typedef std::map<unsigned long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, glws::Context *> ContextMap;
static DrawableMap drawable_map;
static ContextMap context_map;


static glws::Drawable *
getDrawable(unsigned long drawable_id) {
    if (drawable_id == 0) {
        return NULL;
    }

    DrawableMap::const_iterator it;
    it = drawable_map.find(drawable_id);
    if (it == drawable_map.end()) {
        return (drawable_map[drawable_id] = ws->createDrawable(visual));
    }

    return it->second;
}

static glws::Context *
getContext(unsigned long long context_ptr) {
    if (context_ptr == 0) {
        return NULL;
    }

    ContextMap::const_iterator it;
    it = context_map.find(context_ptr);
    if (it == context_map.end()) {
        return (context_map[context_ptr] = ws->createContext(visual, NULL));
    }

    return it->second;
}

static void retrace_glXCreateContext(Trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    glws::Context *share_context = getContext(call.arg(2).toUIntPtr());

    glws::Context *context = ws->createContext(glretrace::visual, share_context);
    context_map[orig_context] = context;
}

static void retrace_glXCreateContextAttribsARB(Trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    glws::Context *share_context = getContext(call.arg(2).toUIntPtr());

    glws::Context *context = ws->createContext(glretrace::visual, share_context);
    context_map[orig_context] = context;
}

static void retrace_glXMakeCurrent(Trace::Call &call) {
    glws::Drawable *new_drawable = getDrawable(call.arg(1).toUInt());
    glws::Context *new_context = getContext(call.arg(2).toUIntPtr());

    if (new_drawable == drawable && new_context == context) {
        return;
    }

    if (drawable && context) {
        glFlush();
        if (!double_buffer) {
            frame_complete(call.no);
        }
    }

    bool result = ws->makeCurrent(new_drawable, new_context);

    if (new_drawable && new_context && result) {
        drawable = new_drawable;
        context = new_context;
    } else {
        drawable = NULL;
        context = NULL;
    }
}

static void retrace_glXSwapBuffers(Trace::Call &call) {
    frame_complete(call.no);
    if (double_buffer) {
        drawable->swapBuffers();
    } else {
        glFlush();
    }
}

static void retrace_glXCreateNewContext(Trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    glws::Context *share_context = getContext(call.arg(3).toUIntPtr());

    glws::Context *context = ws->createContext(glretrace::visual, share_context);
    context_map[orig_context] = context;
}

static void retrace_glXMakeContextCurrent(Trace::Call &call) {
    glws::Drawable *new_drawable = getDrawable(call.arg(1).toUInt());
    glws::Context *new_context = getContext(call.arg(3).toUIntPtr());

    if (new_drawable == drawable && new_context == context) {
        return;
    }

    if (drawable && context) {
        glFlush();
        if (!double_buffer) {
            frame_complete(call.no);
        }
    }

    bool result = ws->makeCurrent(new_drawable, new_context);

    if (new_drawable && new_context && result) {
        drawable = new_drawable;
        context = new_context;
    } else {
        drawable = NULL;
        context = NULL;
    }
}

static const retrace::Entry callbacks[] = {
    //{"glXBindChannelToWindowSGIX", &retrace_glXBindChannelToWindowSGIX},
    //{"glXBindSwapBarrierNV", &retrace_glXBindSwapBarrierNV},
    //{"glXBindSwapBarrierSGIX", &retrace_glXBindSwapBarrierSGIX},
    //{"glXBindTexImageEXT", &retrace_glXBindTexImageEXT},
    //{"glXChannelRectSGIX", &retrace_glXChannelRectSGIX},
    //{"glXChannelRectSyncSGIX", &retrace_glXChannelRectSyncSGIX},
    {"glXChooseFBConfig", &retrace::ignore},
    {"glXChooseFBConfigSGIX", &retrace::ignore},
    {"glXChooseVisual", &retrace::ignore},
    //{"glXCopyContext", &retrace_glXCopyContext},
    //{"glXCopyImageSubDataNV", &retrace_glXCopyImageSubDataNV},
    //{"glXCopySubBufferMESA", &retrace_glXCopySubBufferMESA},
    {"glXCreateContextAttribsARB", &retrace_glXCreateContextAttribsARB},
    {"glXCreateContext", &retrace_glXCreateContext},
    //{"glXCreateContextWithConfigSGIX", &retrace_glXCreateContextWithConfigSGIX},
    //{"glXCreateGLXPbufferSGIX", &retrace_glXCreateGLXPbufferSGIX},
    //{"glXCreateGLXPixmap", &retrace_glXCreateGLXPixmap},
    //{"glXCreateGLXPixmapWithConfigSGIX", &retrace_glXCreateGLXPixmapWithConfigSGIX},
    {"glXCreateNewContext", &retrace_glXCreateNewContext},
    //{"glXCreatePbuffer", &retrace_glXCreatePbuffer},
    //{"glXCreatePixmap", &retrace_glXCreatePixmap},
    //{"glXCreateWindow", &retrace_glXCreateWindow},
    //{"glXCushionSGI", &retrace_glXCushionSGI},
    //{"glXDestroyContext", &retrace_glXDestroyContext},
    //{"glXDestroyGLXPbufferSGIX", &retrace_glXDestroyGLXPbufferSGIX},
    //{"glXDestroyGLXPixmap", &retrace_glXDestroyGLXPixmap},
    //{"glXDestroyPbuffer", &retrace_glXDestroyPbuffer},
    //{"glXDestroyPixmap", &retrace_glXDestroyPixmap},
    //{"glXDestroyWindow", &retrace_glXDestroyWindow},
    //{"glXFreeContextEXT", &retrace_glXFreeContextEXT},
    {"glXGetAGPOffsetMESA", &retrace::ignore},
    {"glXGetClientString", &retrace::ignore},
    {"glXGetConfig", &retrace::ignore},
    {"glXGetContextIDEXT", &retrace::ignore},
    {"glXGetCurrentContext", &retrace::ignore},
    {"glXGetCurrentDisplayEXT", &retrace::ignore},
    {"glXGetCurrentDisplay", &retrace::ignore},
    {"glXGetCurrentDrawable", &retrace::ignore},
    {"glXGetCurrentReadDrawable", &retrace::ignore},
    {"glXGetCurrentReadDrawableSGI", &retrace::ignore},
    {"glXGetFBConfigAttrib", &retrace::ignore},
    {"glXGetFBConfigAttribSGIX", &retrace::ignore},
    {"glXGetFBConfigFromVisualSGIX", &retrace::ignore},
    {"glXGetFBConfigs", &retrace::ignore},
    {"glXGetMscRateOML", &retrace::ignore},
    {"glXGetProcAddressARB", &retrace::ignore},
    {"glXGetProcAddress", &retrace::ignore},
    {"glXGetSelectedEvent", &retrace::ignore},
    {"glXGetSelectedEventSGIX", &retrace::ignore},
    {"glXGetSyncValuesOML", &retrace::ignore},
    {"glXGetVideoSyncSGI", &retrace::ignore},
    {"glXGetVisualFromFBConfig", &retrace::ignore},
    {"glXGetVisualFromFBConfigSGIX", &retrace::ignore},
    //{"glXImportContextEXT", &retrace_glXImportContextEXT},
    {"glXIsDirect", &retrace::ignore},
    //{"glXJoinSwapGroupNV", &retrace_glXJoinSwapGroupNV},
    //{"glXJoinSwapGroupSGIX", &retrace_glXJoinSwapGroupSGIX},
    {"glXMakeContextCurrent", &retrace_glXMakeContextCurrent},
    //{"glXMakeCurrentReadSGI", &retrace_glXMakeCurrentReadSGI},
    {"glXMakeCurrent", &retrace_glXMakeCurrent},
    {"glXQueryChannelDeltasSGIX", &retrace::ignore},
    {"glXQueryChannelRectSGIX", &retrace::ignore},
    {"glXQueryContextInfoEXT", &retrace::ignore},
    {"glXQueryContext", &retrace::ignore},
    {"glXQueryDrawable", &retrace::ignore},
    {"glXQueryExtension", &retrace::ignore},
    {"glXQueryExtensionsString", &retrace::ignore},
    {"glXQueryFrameCountNV", &retrace::ignore},
    {"glXQueryGLXPbufferSGIX", &retrace::ignore},
    {"glXQueryMaxSwapBarriersSGIX", &retrace::ignore},
    {"glXQueryMaxSwapGroupsNV", &retrace::ignore},
    {"glXQueryServerString", &retrace::ignore},
    {"glXQuerySwapGroupNV", &retrace::ignore},
    {"glXQueryVersion", &retrace::ignore},
    //{"glXReleaseBuffersMESA", &retrace_glXReleaseBuffersMESA},
    //{"glXReleaseTexImageEXT", &retrace_glXReleaseTexImageEXT},
    //{"glXResetFrameCountNV", &retrace_glXResetFrameCountNV},
    //{"glXSelectEvent", &retrace_glXSelectEvent},
    //{"glXSelectEventSGIX", &retrace_glXSelectEventSGIX},
    //{"glXSet3DfxModeMESA", &retrace_glXSet3DfxModeMESA},
    //{"glXSwapBuffersMscOML", &retrace_glXSwapBuffersMscOML},
    {"glXSwapBuffers", &retrace_glXSwapBuffers},
    //{"glXSwapIntervalEXT", &retrace_glXSwapIntervalEXT},
    //{"glXSwapIntervalSGI", &retrace_glXSwapIntervalSGI},
    //{"glXUseXFont", &retrace_glXUseXFont},
    {"glXWaitForMscOML", &retrace::ignore},
    {"glXWaitForSbcOML", &retrace::ignore},
    {"glXWaitGL", &retrace::ignore},
    {"glXWaitVideoSyncSGI", &retrace::ignore},
    {"glXWaitX", &retrace::ignore},
};

void glretrace::retrace_call_glx(Trace::Call &call) {
    retrace::dispatch(call, callbacks, sizeof(callbacks)/sizeof(callbacks[0]));
}


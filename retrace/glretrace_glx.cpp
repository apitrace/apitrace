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

#if !defined(HAVE_X11)

#define GLX_PBUFFER_HEIGHT 0x8040
#define GLX_PBUFFER_WIDTH 0x8041

#endif /* !HAVE_X11 */


using namespace glretrace;


typedef std::map<unsigned long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, Context *> ContextMap;
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
        return (drawable_map[drawable_id] = glretrace::createDrawable());
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
        return (context_map[context_ptr] = glretrace::createContext());
    }

    return it->second;
}

static void retrace_glXCreateContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *share_context = getContext(call.arg(2).toUIntPtr());

    Context *context = glretrace::createContext(share_context);
    context_map[orig_context] = context;
}

static void retrace_glXCreateContextAttribsARB(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *share_context = getContext(call.arg(2).toUIntPtr());

    const trace::Value * attrib_list = &call.arg(4);
    glfeatures::Profile profile = parseContextAttribList(attrib_list);

    Context *context = glretrace::createContext(share_context, profile);
    context_map[orig_context] = context;
}

static void retrace_glXMakeCurrent(trace::Call &call) {
    if (call.ret && !call.ret->toBool() && !retrace::ignoreRetvals) {
        // If false was returned then any previously current rendering context
        // and drawable remain unchanged.
        return;
    }

    glws::Drawable *new_drawable = getDrawable(call.arg(1).toUInt());
    Context *new_context = getContext(call.arg(2).toUIntPtr());

    glretrace::makeCurrent(call, new_drawable, new_context);
}


static void retrace_glXDestroyContext(trace::Call &call) {
    ContextMap::iterator it;
    it = context_map.find(call.arg(1).toUIntPtr());
    if (it == context_map.end()) {
        return;
    }

    it->second->release();

    context_map.erase(it);
}

static void retrace_glXCopySubBufferMESA(trace::Call &call) {
    glws::Drawable *drawable = getDrawable(call.arg(1).toUInt());
    int x = call.arg(2).toSInt();
    int y = call.arg(3).toSInt();
    int width = call.arg(4).toSInt();
    int height = call.arg(5).toSInt();

    drawable->copySubBuffer(x, y, width, height);
}

static void retrace_glXSwapBuffers(trace::Call &call) {
    glws::Drawable *drawable = getDrawable(call.arg(1).toUInt());

    frame_complete(call);
    if (retrace::doubleBuffer) {
        if (drawable) {
            drawable->swapBuffers();
        }
    } else {
        glFlush();
    }
}

static void retrace_glXSwapBuffersMscOML(trace::Call &call) {
    // ignore additional arguments
    retrace_glXSwapBuffers(call);
}

static void retrace_glXCreateNewContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *share_context = getContext(call.arg(3).toUIntPtr());

    Context *context = glretrace::createContext(share_context);
    context_map[orig_context] = context;
}

static void retrace_glXCreatePbuffer(trace::Call &call) {
    unsigned long long orig_drawable = call.ret->toUInt();
    if (!orig_drawable) {
        return;
    }

    const trace::Value *attrib_list = &call.arg(2);
    int width = glretrace::parseAttrib(attrib_list, GLX_PBUFFER_WIDTH, 0);
    int height = glretrace::parseAttrib(attrib_list, GLX_PBUFFER_HEIGHT, 0);
    glws::pbuffer_info pbInfo = {0, 0, false};

    glws::Drawable *drawable = glretrace::createPbuffer(width, height, &pbInfo);
    
    drawable_map[orig_drawable] = drawable;
}

static void retrace_glXDestroyPbuffer(trace::Call &call) {
    glws::Drawable *drawable = getDrawable(call.arg(1).toUInt());

    if (!drawable) {
        return;
    }

    delete drawable;
}

static void retrace_glXMakeContextCurrent(trace::Call &call) {
    if (call.ret && !call.ret->toBool() && !retrace::ignoreRetvals) {
        // If false was returned then any previously current rendering context
        // and drawable remain unchanged.
        return;
    }

    glws::Drawable *new_drawable = getDrawable(call.arg(1).toUInt());
    glws::Drawable *new_readable = getDrawable(call.arg(2).toUInt());
    Context *new_context = getContext(call.arg(3).toUIntPtr());

    glretrace::makeCurrent(call, new_drawable, new_readable, new_context);
}

const retrace::Entry glretrace::glx_callbacks[] = {
    //{"glXBindChannelToWindowSGIX", &retrace_glXBindChannelToWindowSGIX},
    //{"glXBindSwapBarrierNV", &retrace_glXBindSwapBarrierNV},
    //{"glXBindSwapBarrierSGIX", &retrace_glXBindSwapBarrierSGIX},
    {"glXBindTexImageEXT", &retrace::ignore},
    //{"glXChannelRectSGIX", &retrace_glXChannelRectSGIX},
    //{"glXChannelRectSyncSGIX", &retrace_glXChannelRectSyncSGIX},
    {"glXChooseFBConfig", &retrace::ignore},
    {"glXChooseFBConfigSGIX", &retrace::ignore},
    {"glXChooseVisual", &retrace::ignore},
    //{"glXCopyContext", &retrace_glXCopyContext},
    //{"glXCopyImageSubDataNV", &retrace_glXCopyImageSubDataNV},
    {"glXCopySubBufferMESA", &retrace_glXCopySubBufferMESA},
    {"glXCreateContextAttribsARB", &retrace_glXCreateContextAttribsARB},
    {"glXCreateContext", &retrace_glXCreateContext},
    //{"glXCreateContextWithConfigSGIX", &retrace_glXCreateContextWithConfigSGIX},
    //{"glXCreateGLXPbufferSGIX", &retrace_glXCreateGLXPbufferSGIX},
    //{"glXCreateGLXPixmap", &retrace_glXCreateGLXPixmap},
    //{"glXCreateGLXPixmapWithConfigSGIX", &retrace_glXCreateGLXPixmapWithConfigSGIX},
    {"glXCreateNewContext", &retrace_glXCreateNewContext},
    {"glXCreatePbuffer", &retrace_glXCreatePbuffer},
    {"glXCreatePixmap", &retrace::ignore},
    //{"glXCreateWindow", &retrace_glXCreateWindow},
    //{"glXCushionSGI", &retrace_glXCushionSGI},
    {"glXDestroyContext", &retrace_glXDestroyContext},
    //{"glXDestroyGLXPbufferSGIX", &retrace_glXDestroyGLXPbufferSGIX},
    //{"glXDestroyGLXPixmap", &retrace_glXDestroyGLXPixmap},
    {"glXDestroyPbuffer", &retrace_glXDestroyPbuffer},
    {"glXDestroyPixmap", &retrace::ignore},
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
    {"glXGetSwapIntervalMESA", &retrace::ignore},
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
    {"glXQueryContext", &retrace::ignore},
    {"glXQueryContextInfoEXT", &retrace::ignore},
    {"glXQueryCurrentRendererIntegerMESA", &retrace::ignore},
    {"glXQueryCurrentRendererStringMESA", &retrace::ignore},
    {"glXQueryDrawable", &retrace::ignore},
    {"glXQueryExtension", &retrace::ignore},
    {"glXQueryExtensionsString", &retrace::ignore},
    {"glXQueryFrameCountNV", &retrace::ignore},
    {"glXQueryGLXPbufferSGIX", &retrace::ignore},
    {"glXQueryMaxSwapBarriersSGIX", &retrace::ignore},
    {"glXQueryMaxSwapGroupsNV", &retrace::ignore},
    {"glXQueryRendererIntegerMESA", &retrace::ignore},
    {"glXQueryRendererStringMESA", &retrace::ignore},
    {"glXQueryServerString", &retrace::ignore},
    {"glXQuerySwapGroupNV", &retrace::ignore},
    {"glXQueryVersion", &retrace::ignore},
    //{"glXReleaseBuffersMESA", &retrace_glXReleaseBuffersMESA},
    {"glXReleaseTexImageEXT", &retrace::ignore},
    //{"glXResetFrameCountNV", &retrace_glXResetFrameCountNV},
    //{"glXSelectEvent", &retrace_glXSelectEvent},
    //{"glXSelectEventSGIX", &retrace_glXSelectEventSGIX},
    //{"glXSet3DfxModeMESA", &retrace_glXSet3DfxModeMESA},
    {"glXSwapBuffersMscOML", &retrace_glXSwapBuffersMscOML},
    {"glXSwapBuffers", &retrace_glXSwapBuffers},
    {"glXSwapIntervalEXT", &retrace::ignore},
    {"glXSwapIntervalMESA", &retrace::ignore},
    {"glXSwapIntervalSGI", &retrace::ignore},
    //{"glXUseXFont", &retrace_glXUseXFont},
    {"glXWaitForMscOML", &retrace::ignore},
    {"glXWaitForSbcOML", &retrace::ignore},
    {"glXWaitGL", &retrace::ignore},
    {"glXWaitVideoSyncSGI", &retrace::ignore},
    {"glXWaitX", &retrace::ignore},
    {NULL, NULL},
};


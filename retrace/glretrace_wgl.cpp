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

static void retrace_wglCreateContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
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
    glws::Drawable *new_drawable = getDrawable(call.arg(0).toUIntPtr());
    Context *new_context = context_map[call.arg(1).toUIntPtr()];

    glretrace::makeCurrent(call, new_drawable, new_context);
}

static void retrace_wglCopyContext(trace::Call &call) {
}

static void retrace_wglChoosePixelFormat(trace::Call &call) {
}

static void retrace_wglDescribePixelFormat(trace::Call &call) {
}

static void retrace_wglSetPixelFormat(trace::Call &call) {
}

static void retrace_wglSwapBuffers(trace::Call &call) {
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
    unsigned long long hglrc1 = call.arg(0).toUIntPtr();
    unsigned long long hglrc2 = call.arg(1).toUIntPtr();

    Context *share_context = context_map[hglrc1];
    Context *old_context = context_map[hglrc2];

    Context *new_context = glretrace::createContext(share_context);
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

static void retrace_wglDescribeLayerPlane(trace::Call &call) {
}

static void retrace_wglSetLayerPaletteEntries(trace::Call &call) {
}

static void retrace_wglRealizeLayerPalette(trace::Call &call) {
}

static void retrace_wglSwapLayerBuffers(trace::Call &call) {
    retrace_wglSwapBuffers(call);
}

static void retrace_wglUseFontBitmapsA(trace::Call &call) {
}

static void retrace_wglUseFontBitmapsW(trace::Call &call) {
}

static void retrace_wglSwapMultipleBuffers(trace::Call &call) {
}

static void retrace_wglUseFontOutlinesA(trace::Call &call) {
}

static void retrace_wglUseFontOutlinesW(trace::Call &call) {
}

static void retrace_wglCreateBufferRegionARB(trace::Call &call) {
}

static void retrace_wglDeleteBufferRegionARB(trace::Call &call) {
}

static void retrace_wglSaveBufferRegionARB(trace::Call &call) {
}

static void retrace_wglRestoreBufferRegionARB(trace::Call &call) {
}

static void retrace_wglChoosePixelFormatARB(trace::Call &call) {
}

static void retrace_wglMakeContextCurrentARB(trace::Call &call) {
}

static void retrace_wglCreatePbufferARB(trace::Call &call) {
    int iWidth = call.arg(2).toUInt();
    int iHeight = call.arg(3).toUInt();

    unsigned long long orig_pbuffer = call.ret->toUIntPtr();
    glws::Drawable *drawable = glretrace::createPbuffer(iWidth, iHeight);

    pbuffer_map[orig_pbuffer] = drawable;
}

static void retrace_wglGetPbufferDCARB(trace::Call &call) {
    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];

    unsigned long long orig_hdc = call.ret->toUIntPtr();

    drawable_map[orig_hdc] = pbuffer;
}

static void retrace_wglReleasePbufferDCARB(trace::Call &call) {
}

static void retrace_wglDestroyPbufferARB(trace::Call &call) {
}

static void retrace_wglQueryPbufferARB(trace::Call &call) {
}

static void retrace_wglBindTexImageARB(trace::Call &call) {
}

static void retrace_wglReleaseTexImageARB(trace::Call &call) {
}

static void retrace_wglSetPbufferAttribARB(trace::Call &call) {
}

static void retrace_wglCreateContextAttribsARB(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    Context *share_context = NULL;

    if (call.arg(1).toPointer()) {
        share_context = context_map[call.arg(1).toUIntPtr()];
    }

    Context *context = glretrace::createContext(share_context);
    context_map[orig_context] = context;
}

static void retrace_wglMakeContextCurrentEXT(trace::Call &call) {
}

static void retrace_wglChoosePixelFormatEXT(trace::Call &call) {
}

static void retrace_wglSwapIntervalEXT(trace::Call &call) {
}

static void retrace_wglAllocateMemoryNV(trace::Call &call) {
}

static void retrace_wglFreeMemoryNV(trace::Call &call) {
}

static void retrace_glAddSwapHintRectWIN(trace::Call &call) {
}

static void retrace_wglGetProcAddress(trace::Call &call) {
}

const retrace::Entry glretrace::wgl_callbacks[] = {
    {"glAddSwapHintRectWIN", &retrace_glAddSwapHintRectWIN},
    {"wglAllocateMemoryNV", &retrace_wglAllocateMemoryNV},
    {"wglBindTexImageARB", &retrace_wglBindTexImageARB},
    {"wglChoosePixelFormat", &retrace_wglChoosePixelFormat},
    {"wglChoosePixelFormatARB", &retrace_wglChoosePixelFormatARB},
    {"wglChoosePixelFormatEXT", &retrace_wglChoosePixelFormatEXT},
    {"wglCopyContext", &retrace_wglCopyContext},
    {"wglCreateBufferRegionARB", &retrace_wglCreateBufferRegionARB},
    {"wglCreateContext", &retrace_wglCreateContext},
    {"wglCreateContextAttribsARB", &retrace_wglCreateContextAttribsARB},
    {"wglCreateLayerContext", &retrace_wglCreateLayerContext},
    {"wglCreatePbufferARB", &retrace_wglCreatePbufferARB},
    {"wglDeleteBufferRegionARB", &retrace_wglDeleteBufferRegionARB},
    {"wglDeleteContext", &retrace_wglDeleteContext},
    {"wglDescribeLayerPlane", &retrace_wglDescribeLayerPlane},
    {"wglDescribePixelFormat", &retrace_wglDescribePixelFormat},
    {"wglDestroyPbufferARB", &retrace_wglDestroyPbufferARB},
    {"wglFreeMemoryNV", &retrace_wglFreeMemoryNV},
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
    {"wglGetProcAddress", &retrace_wglGetProcAddress},
    {"wglGetSwapIntervalEXT", &retrace::ignore},
    {"wglMakeContextCurrentARB", &retrace_wglMakeContextCurrentARB},
    {"wglMakeContextCurrentEXT", &retrace_wglMakeContextCurrentEXT},
    {"wglMakeCurrent", &retrace_wglMakeCurrent},
    {"wglQueryPbufferARB", &retrace_wglQueryPbufferARB},
    {"wglRealizeLayerPalette", &retrace_wglRealizeLayerPalette},
    {"wglReleasePbufferDCARB", &retrace_wglReleasePbufferDCARB},
    {"wglReleaseTexImageARB", &retrace_wglReleaseTexImageARB},
    {"wglRestoreBufferRegionARB", &retrace_wglRestoreBufferRegionARB},
    {"wglSaveBufferRegionARB", &retrace_wglSaveBufferRegionARB},
    {"wglSetLayerPaletteEntries", &retrace_wglSetLayerPaletteEntries},
    {"wglSetPbufferAttribARB", &retrace_wglSetPbufferAttribARB},
    {"wglSetPixelFormat", &retrace_wglSetPixelFormat},
    {"wglShareLists", &retrace_wglShareLists},
    {"wglSwapBuffers", &retrace_wglSwapBuffers},
    {"wglSwapIntervalEXT", &retrace_wglSwapIntervalEXT},
    {"wglSwapLayerBuffers", &retrace_wglSwapLayerBuffers},
    {"wglSwapMultipleBuffers", &retrace_wglSwapMultipleBuffers},
    {"wglUseFontBitmapsA", &retrace_wglUseFontBitmapsA},
    {"wglUseFontBitmapsW", &retrace_wglUseFontBitmapsW},
    {"wglUseFontOutlinesA", &retrace_wglUseFontOutlinesA},
    {"wglUseFontOutlinesW", &retrace_wglUseFontOutlinesW},
    {NULL, NULL}
};


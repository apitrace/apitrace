/**************************************************************************
 *
 * Copyright 2011 LunarG, Inc.
 * All Rights Reserved.
 *
 * Based on glretrace_glx.cpp, which has
 *
 *   Copyright 2011 Jose Fonseca
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
#include "os.hpp"
#include "eglsize.hpp"

#ifndef EGL_OPENGL_ES_API
#define EGL_OPENGL_ES_API		0x30A0
#define EGL_OPENVG_API			0x30A1
#define EGL_OPENGL_API			0x30A2
#define EGL_CONTEXT_CLIENT_VERSION	0x3098
#endif


using namespace glretrace;


typedef std::map<unsigned long long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, Context *> ContextMap;
typedef std::map<unsigned long long, glws::Profile> ProfileMap;
static DrawableMap drawable_map;
static ContextMap context_map;
static ProfileMap profile_map;

static unsigned int current_api = EGL_OPENGL_ES_API;

/*
 * FIXME: Ideally we would defer the context creation until the profile was
 * clear, as explained in https://github.com/apitrace/apitrace/issues/197 ,
 * instead of guessing.  For now, start with a guess of ES2 profile, which
 * should be the most common case for EGL.
 */
static glws::Profile last_profile = glws::PROFILE_ES2;

static glws::Drawable *null_drawable = NULL;


static void
createDrawable(unsigned long long orig_config, unsigned long long orig_surface);

static glws::Drawable *
getDrawable(unsigned long long surface_ptr) {
    if (surface_ptr == 0) {
        return NULL;
    }

    DrawableMap::const_iterator it;
    it = drawable_map.find(surface_ptr);
    if (it == drawable_map.end()) {
        // In Fennec we get the egl window surface from Java which isn't
        // traced, so just create a drawable if it doesn't exist in here
        createDrawable(0, surface_ptr);
        it = drawable_map.find(surface_ptr);
        assert(it != drawable_map.end());
    }

    return (it != drawable_map.end()) ? it->second : NULL;
}

static Context *
getContext(unsigned long long context_ptr) {
    if (context_ptr == 0) {
        return NULL;
    }

    ContextMap::const_iterator it;
    it = context_map.find(context_ptr);

    return (it != context_map.end()) ? it->second : NULL;
}

static void createDrawable(unsigned long long orig_config, unsigned long long orig_surface)
{
    ProfileMap::iterator it = profile_map.find(orig_config);
    glws::Profile profile;

    // If the requested config is associated with a profile, use that
    // profile. Otherwise, assume that the last used profile is what
    // the user wants.
    if (it != profile_map.end()) {
        profile = it->second;
    } else {
        profile = last_profile;
    }

    glws::Drawable *drawable = glretrace::createDrawable(profile);
    drawable_map[orig_surface] = drawable;
}

static void retrace_eglCreateWindowSurface(trace::Call &call) {
    unsigned long long orig_config = call.arg(1).toUIntPtr();
    unsigned long long orig_surface = call.ret->toUIntPtr();
    createDrawable(orig_config, orig_surface);
}

static void retrace_eglCreatePbufferSurface(trace::Call &call) {
    unsigned long long orig_config = call.arg(1).toUIntPtr();
    unsigned long long orig_surface = call.ret->toUIntPtr();
    createDrawable(orig_config, orig_surface);
    // TODO: Respect the pbuffer dimensions too
}

static void retrace_eglDestroySurface(trace::Call &call) {
    unsigned long long orig_surface = call.arg(1).toUIntPtr();

    DrawableMap::iterator it;
    it = drawable_map.find(orig_surface);

    if (it != drawable_map.end()) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        if (!currentContext || it->second != currentContext->drawable) {
            // TODO: reference count
            delete it->second;
        }
        drawable_map.erase(it);
    }
}

static void retrace_eglBindAPI(trace::Call &call) {
    current_api = call.arg(0).toUInt();

    glws::Api api;
    switch (current_api) {
    case EGL_OPENGL_API:
        api = glws::API_GL;
        break;
    case EGL_OPENGL_ES_API:
        api = glws::API_GLES;
        break;
    default:
        assert(0);
        return;
    }

    glws::bindApi(api);
}

static void retrace_eglCreateContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    unsigned long long orig_config = call.arg(1).toUIntPtr();
    Context *share_context = getContext(call.arg(2).toUIntPtr());
    trace::Array *attrib_array = call.arg(3).toArray();
    glws::Profile profile;

    switch (current_api) {
    case EGL_OPENGL_API:
        profile = glws::PROFILE_COMPAT;
        break;
    case EGL_OPENGL_ES_API:
    default:
        profile = glws::PROFILE_ES1;
        if (attrib_array) {
            for (int i = 0; i < attrib_array->values.size(); i += 2) {
                int v = attrib_array->values[i]->toSInt();
                if (v == EGL_CONTEXT_CLIENT_VERSION) {
                    v = attrib_array->values[i + 1]->toSInt();
                    if (v == 2)
                        profile = glws::PROFILE_ES2;
                    break;
                }
            }
        }
        break;
    }


    Context *context = glretrace::createContext(share_context, profile);
    if (!context) {
        const char *name;
        switch (profile) {
        case glws::PROFILE_COMPAT:
            name = "OpenGL";
            break;
        case glws::PROFILE_ES1:
            name = "OpenGL ES 1.1";
            break;
        case glws::PROFILE_ES2:
            name = "OpenGL ES 2.0";
            break;
        default:
            name = "unknown";
            break;
        }

        retrace::warning(call) << "Failed to create " << name << " context.\n";
        exit(1);
    }

    context_map[orig_context] = context;
    profile_map[orig_config] = profile;
    last_profile = profile;
}

static void retrace_eglDestroyContext(trace::Call &call) {
    unsigned long long orig_context = call.arg(1).toUIntPtr();

    ContextMap::iterator it;
    it = context_map.find(orig_context);

    if (it != context_map.end()) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        if (it->second != currentContext) {
            // TODO: reference count
            delete it->second;
        }
        context_map.erase(it);
    }
}

static void retrace_eglMakeCurrent(trace::Call &call) {
    if (!call.ret->toSInt()) {
        // Previously current rendering context and surfaces (if any) remain
        // unchanged.
        return;
    }

    glws::Drawable *new_drawable = getDrawable(call.arg(1).toUIntPtr());
    Context *new_context = getContext(call.arg(3).toUIntPtr());

    // Try to support GL_OES_surfaceless_context by creating a dummy drawable.
    if (new_context && !new_drawable) {
        if (!null_drawable) {
            null_drawable = glretrace::createDrawable(last_profile);
        }
        new_drawable = null_drawable;
    }

    glretrace::makeCurrent(call, new_drawable, new_context);
}


static void retrace_eglSwapBuffers(trace::Call &call) {
    glws::Drawable *drawable = getDrawable(call.arg(1).toUIntPtr());

    frame_complete(call);

    if (retrace::doubleBuffer) {
        if (drawable) {
            drawable->swapBuffers();
        }
    } else {
        glFlush();
    }
}

const retrace::Entry glretrace::egl_callbacks[] = {
    {"eglGetError", &retrace::ignore},
    {"eglGetDisplay", &retrace::ignore},
    {"eglInitialize", &retrace::ignore},
    {"eglTerminate", &retrace::ignore},
    {"eglQueryString", &retrace::ignore},
    {"eglGetConfigs", &retrace::ignore},
    {"eglChooseConfig", &retrace::ignore},
    {"eglGetConfigAttrib", &retrace::ignore},
    {"eglCreateWindowSurface", &retrace_eglCreateWindowSurface},
    {"eglCreatePbufferSurface", &retrace_eglCreatePbufferSurface},
    //{"eglCreatePixmapSurface", &retrace::ignore},
    {"eglDestroySurface", &retrace_eglDestroySurface},
    {"eglQuerySurface", &retrace::ignore},
    {"eglBindAPI", &retrace_eglBindAPI},
    {"eglQueryAPI", &retrace::ignore},
    //{"eglWaitClient", &retrace::ignore},
    //{"eglReleaseThread", &retrace::ignore},
    //{"eglCreatePbufferFromClientBuffer", &retrace::ignore},
    //{"eglSurfaceAttrib", &retrace::ignore},
    //{"eglBindTexImage", &retrace::ignore},
    //{"eglReleaseTexImage", &retrace::ignore},
    {"eglSwapInterval", &retrace::ignore},
    {"eglCreateContext", &retrace_eglCreateContext},
    {"eglDestroyContext", &retrace_eglDestroyContext},
    {"eglMakeCurrent", &retrace_eglMakeCurrent},
    {"eglGetCurrentContext", &retrace::ignore},
    {"eglGetCurrentSurface", &retrace::ignore},
    {"eglGetCurrentDisplay", &retrace::ignore},
    {"eglQueryContext", &retrace::ignore},
    {"eglWaitGL", &retrace::ignore},
    {"eglWaitNative", &retrace::ignore},
    {"eglSwapBuffers", &retrace_eglSwapBuffers},
    //{"eglCopyBuffers", &retrace::ignore},
    {"eglGetProcAddress", &retrace::ignore},
    {"eglCreateImageKHR", &retrace::ignore},
    {"eglDestroyImageKHR", &retrace::ignore},
    {"glEGLImageTargetTexture2DOES", &retrace::ignore},
    {NULL, NULL},
};

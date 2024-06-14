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

#include <climits>

#ifndef EGL_OPENGL_ES_API
#define EGL_OPENGL_ES_API		0x30A0
#define EGL_OPENVG_API			0x30A1
#define EGL_OPENGL_API			0x30A2
#define EGL_CONTEXT_CLIENT_VERSION	0x3098
#endif


using namespace glretrace;


typedef std::map<unsigned long long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, Context *> ContextMap;
typedef std::map<unsigned long long, glfeatures::Profile> ProfileMap;
static DrawableMap drawable_map;
static ContextMap context_map;
static ProfileMap profile_map;

/* FIXME: This should be tracked per thread. */
static unsigned int current_api = EGL_OPENGL_ES_API;

/*
 * FIXME: Ideally we would defer the context creation until the profile was
 * clear, as explained in https://github.com/apitrace/apitrace/issues/197 ,
 * instead of guessing.  For now, start with a guess of ES2 profile, which
 * should be the most common case for EGL.
 */
static glfeatures::Profile last_profile(glfeatures::API_GLES, 2, 0);

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
    glfeatures::Profile profile;

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

static void retrace_eglSetDamageRegionKHR(trace::Call &call)
{
    glws::Drawable *drawable = getDrawable(call.arg(1).toUIntPtr());
    trace::Array *rects_array = call.arg(2).toArray();
    EGLint nrects = std::min(call.arg(3).toUInt(), (unsigned long long int)PTRDIFF_MAX);

    if (!drawable)
        return;

    int *rects = new int[nrects*4];
    for (size_t i = 0; i < (size_t)nrects*4; i++)
        rects[i] = rects_array->values[i]->toSInt();

    drawable->setDamageRegion(rects, nrects);
    delete [] rects;
}

static void retrace_eglSwapBuffersWithDamage(trace::Call &call) {
    glws::Drawable *drawable = getDrawable(call.arg(1).toUIntPtr());
    trace::Array *rects_array = call.arg(2).toArray();
    EGLint nrects = std::min(call.arg(3).toUInt(), (unsigned long long int)PTRDIFF_MAX);

    frame_complete(call);

    if (retrace::doubleBuffer) {
        if (drawable) {
            int *rects = new int[nrects*4];
            for (size_t i = 0; i < (size_t)nrects*4; i++)
                rects[i] = rects_array->values[i]->toSInt();

            drawable->swapBuffersWithDamage(rects, nrects);
            delete [] rects;
        }
    } else {
        glFlush();
    }

    if (retrace::profilingFrameTimes) {
        // Wait for presentation to finish
        glFinish();
        std::cout << "rendering_finished " << glretrace::getCurrentTime() << std::endl;
    }
}

static void retrace_eglChooseConfig(trace::Call &call) {
    if (!call.ret->toSInt()) {
        return;
    }

    trace::Array *attrib_array = call.arg(1).toArray();
    trace::Array *config_array = call.arg(2).toArray();
    trace::Array *num_config_ptr = call.arg(4).toArray();
    if (!attrib_array || !config_array || !num_config_ptr) {
        return;
    }

    glfeatures::Profile profile;
    unsigned renderableType = parseAttrib(attrib_array, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT);
    std::cerr << "renderableType = " << renderableType << "\n";
    if (renderableType & EGL_OPENGL_BIT) {
        profile = glfeatures::Profile(glfeatures::API_GL, 1, 0);
    } else {
        profile.api = glfeatures::API_GLES;
        if (renderableType & EGL_OPENGL_ES3_BIT) {
            profile.major = 3;
        } else if (renderableType & EGL_OPENGL_ES2_BIT) {
            profile.major = 2;
        } else {
            profile.major = 1;
        }
    }

    if (parseAttrib(attrib_array, EGL_SAMPLE_BUFFERS))
        profile.samples = parseAttrib(attrib_array, EGL_SAMPLES);

    unsigned num_config = num_config_ptr->values[0]->toUInt();
    for (unsigned i = 0; i < num_config; ++i) {
        unsigned long long orig_config = config_array->values[i]->toUIntPtr();
        profile_map[orig_config] = profile;
    }
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
    if (!call.ret->toBool()) {
        return;
    }

    current_api = call.arg(0).toUInt();
}

static void retrace_eglCreateContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (orig_context == 0) {
        return;
    }

    unsigned long long orig_config = call.arg(1).toUIntPtr();
    Context *share_context = getContext(call.arg(2).toUIntPtr());
    trace::Array *attrib_array = call.arg(3).toArray();
    glfeatures::Profile profile;

    switch (current_api) {
    case EGL_OPENGL_API:
        profile.api = glfeatures::API_GL;
        profile.major = parseAttrib(attrib_array, EGL_CONTEXT_MAJOR_VERSION, 1);
        profile.minor = parseAttrib(attrib_array, EGL_CONTEXT_MINOR_VERSION, 0);
        if (profile.versionGreaterOrEqual(3,2)) {
             int profileMask = parseAttrib(attrib_array, EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR);
             if (profileMask & EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR) {
                 profile.core = true;
             }
             int contextFlags = parseAttrib(attrib_array, EGL_CONTEXT_FLAGS_KHR, 0);
             if (contextFlags & EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR) {
                 profile.forwardCompatible = true;
             }
        }
        break;
    case EGL_OPENGL_ES_API:
    default:
        profile.api = glfeatures::API_GLES;
        profile.major = parseAttrib(attrib_array, EGL_CONTEXT_MAJOR_VERSION, 1);
        profile.minor = parseAttrib(attrib_array, EGL_CONTEXT_MINOR_VERSION, 0);
        break;
    }


    Context *context = glretrace::createContext(share_context, profile);
    assert(context);

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
            it->second->release();
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

    if (retrace::profilingFrameTimes) {
        // Wait for presentation to finish
        glFinish();
        std::cout << "rendering_finished " << glretrace::getCurrentTime() << std::endl;
    }
}

const retrace::Entry glretrace::egl_callbacks[] = {
    {"eglGetError", &retrace::ignore},
    {"eglGetDisplay", &retrace::ignore},
    {"eglGetPlatformDisplay", &retrace::ignore},
    {"eglGetPlatformDisplayEXT", &retrace::ignore},
    {"eglInitialize", &retrace::ignore},
    {"eglTerminate", &retrace::ignore},
    {"eglQueryString", &retrace::ignore},
    {"eglGetConfigs", &retrace::ignore},
    {"eglChooseConfig", &retrace_eglChooseConfig},
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
    {"eglReleaseThread", &retrace::ignore},
    {"eglSetDamageRegionKHR", &retrace_eglSetDamageRegionKHR},
    {"eglSwapBuffers", &retrace_eglSwapBuffers},
    {"eglSwapBuffersWithDamageEXT", &retrace_eglSwapBuffersWithDamage},
    {"eglSwapBuffersWithDamageKHR", &retrace_eglSwapBuffersWithDamage},
    //{"eglCopyBuffers", &retrace::ignore},
    {"eglGetProcAddress", &retrace::ignore},
    {"eglCreateImageKHR", &retrace::ignore},
    {"eglDestroyImageKHR", &retrace::ignore},
    {"glEGLImageTargetTexture2DOES", &retrace::ignore},
    {"eglGetSyncValuesCHROMIUM", &retrace::ignore},
    {NULL, NULL},
};

/**************************************************************************
 *
 * Copyright 2011 LunarG, Inc.
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

#include <assert.h>
#include <stdlib.h>

#include <iostream>

#include <dlfcn.h>

#include "glproc.hpp"
#include "glws.hpp"
#include "glws_xlib.hpp"


namespace glws {


static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static char const *eglExtensions = NULL;
static bool has_EGL_KHR_create_context = false;


class EglVisual : public Visual
{
public:
    EGLConfig config;
    XVisualInfo *visinfo;

    EglVisual(Profile prof) :
        Visual(prof),
        config(0),
        visinfo(0)
    {}

    ~EglVisual() {
        XFree(visinfo);
    }
};


class EglDrawable : public Drawable
{
public:
    Window window;
    EGLSurface surface;
    EGLint api;

    EglDrawable(const Visual *vis, int w, int h, bool pbuffer) :
        Drawable(vis, w, h, pbuffer),
        api(EGL_OPENGL_ES_API)
    {
        XVisualInfo *visinfo = static_cast<const EglVisual *>(visual)->visinfo;

        const char *name = "eglretrace";
        window = createWindow(visinfo, name, width, height);

        eglWaitNative(EGL_CORE_NATIVE_ENGINE);

        EGLConfig config = static_cast<const EglVisual *>(visual)->config;
        surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
    }

    ~EglDrawable() {
        eglDestroySurface(eglDisplay, surface);
        eglWaitClient();
        XDestroyWindow(display, window);
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    }

    void
    recreate(void) {
        EGLContext currentContext = eglGetCurrentContext();
        EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
        EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
        bool rebindDrawSurface = currentDrawSurface == surface;
        bool rebindReadSurface = currentReadSurface == surface;

        if (rebindDrawSurface || rebindReadSurface) {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }

        // XXX: Defer destruction to prevent getting the same surface as before, which seems to cause Mesa to crash
        EGLSurface oldSurface = surface;

        EGLConfig config = static_cast<const EglVisual *>(visual)->config;
        surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);

        if (rebindDrawSurface || rebindReadSurface) {
            eglMakeCurrent(eglDisplay, surface, surface, currentContext);
        }

        eglDestroySurface(eglDisplay, oldSurface);
    }

    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }

        eglWaitClient();

        // We need to ensure that pending events are processed here, and XSync
        // with discard = True guarantees that, but it appears the limited
        // event processing we do so far is sufficient
        //XSync(display, True);

        Drawable::resize(w, h);

        resizeWindow(window, w, h);

        eglWaitNative(EGL_CORE_NATIVE_ENGINE);

        /*
         * Some implementations won't update the backbuffer unless we recreate
         * the EGL surface.
         */

        int eglWidth;
        int eglHeight;

        eglQuerySurface(eglDisplay, surface, EGL_WIDTH, &eglWidth);
        eglQuerySurface(eglDisplay, surface, EGL_HEIGHT, &eglHeight);

        if (eglWidth != width || eglHeight != height) {
            recreate();

            eglQuerySurface(eglDisplay, surface, EGL_WIDTH, &eglWidth);
            eglQuerySurface(eglDisplay, surface, EGL_HEIGHT, &eglHeight);
        }

        assert(eglWidth == width);
        assert(eglHeight == height);
    }

    void show(void) {
        if (visible) {
            return;
        }

        eglWaitClient();

        showWindow(window);

        eglWaitNative(EGL_CORE_NATIVE_ENGINE);

        Drawable::show();
    }

    void swapBuffers(void) {
        eglBindAPI(api);
        eglSwapBuffers(eglDisplay, surface);
        processKeys(window);
    }
};


class EglContext : public Context
{
public:
    EGLContext context;

    EglContext(const Visual *vis, EGLContext ctx) :
        Context(vis),
        context(ctx)
    {}

    ~EglContext() {
        eglDestroyContext(eglDisplay, context);
    }
};

/**
 * Load the symbols from the specified shared object into global namespace, so
 * that they can be later found by dlsym(RTLD_NEXT, ...);
 */
static void
load(const char *filename)
{
    if (!dlopen(filename, RTLD_GLOBAL | RTLD_LAZY)) {
        std::cerr << "error: unable to open " << filename << "\n";
        exit(1);
    }
}

void
init(void) {
    load("libEGL.so.1");

    initX();

    eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
    if (eglDisplay == EGL_NO_DISPLAY) {
        std::cerr << "error: unable to get EGL display\n";
        XCloseDisplay(display);
        exit(1);
    }

    EGLint major, minor;
    if (!eglInitialize(eglDisplay, &major, &minor)) {
        std::cerr << "error: unable to initialize EGL display\n";
        XCloseDisplay(display);
        exit(1);
    }

    eglExtensions = eglQueryString(eglDisplay, EGL_EXTENSIONS);
    has_EGL_KHR_create_context = checkExtension("EGL_KHR_create_context", eglExtensions);
}

void
cleanup(void) {
    if (eglDisplay != EGL_NO_DISPLAY) {
        eglTerminate(eglDisplay);
    }

    cleanupX();
}

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {
    EglVisual *visual = new EglVisual(profile);
    // possible combinations
    const EGLint api_bits_gl[7] = {
        EGL_OPENGL_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_BIT | EGL_OPENGL_ES_BIT,
        EGL_OPENGL_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_BIT,
        EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_ES_BIT,
    };
    const EGLint api_bits_gles1[7] = {
        EGL_OPENGL_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_BIT | EGL_OPENGL_ES_BIT,
        EGL_OPENGL_ES_BIT,
        EGL_OPENGL_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_BIT,
        EGL_OPENGL_ES2_BIT,
    };
    const EGLint api_bits_gles2[7] = {
        EGL_OPENGL_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_BIT | EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_ES2_BIT,
        EGL_OPENGL_BIT | EGL_OPENGL_ES_BIT,
        EGL_OPENGL_BIT,
        EGL_OPENGL_ES_BIT,
    };
    const EGLint *api_bits;

    if (profile.api == API_GL) {
        api_bits = api_bits_gl;
        if (profile.core && !has_EGL_KHR_create_context) {
            return NULL;
        }
    } else if (profile.api == API_GLES) {
        switch (profile.major) {
        case 1:
            api_bits = api_bits_gles1;
            break;
        case 2:
        case 3:
            api_bits = api_bits_gles2;
            break;
        default:
            return NULL;
        }
    } else {
        assert(0);
        return NULL;
    }

    for (int i = 0; i < 7; i++) {
        Attributes<EGLint> attribs;

        attribs.add(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);
        attribs.add(EGL_RED_SIZE, 1);
        attribs.add(EGL_GREEN_SIZE, 1);
        attribs.add(EGL_BLUE_SIZE, 1);
        attribs.add(EGL_ALPHA_SIZE, 1);
        attribs.add(EGL_DEPTH_SIZE, 1);
        attribs.add(EGL_STENCIL_SIZE, 1);
        attribs.add(EGL_RENDERABLE_TYPE, api_bits[i]);
        attribs.end(EGL_NONE);

        EGLint num_configs, vid;
        if (eglChooseConfig(eglDisplay, attribs, &visual->config, 1, &num_configs) &&
            num_configs == 1 &&
            eglGetConfigAttrib(eglDisplay, visual->config, EGL_NATIVE_VISUAL_ID, &vid)) {
            XVisualInfo templ;
            int num_visuals;

            templ.visualid = vid;
            visual->visinfo = XGetVisualInfo(display, VisualIDMask, &templ, &num_visuals);
            break;
        }
    }

    assert(visual->visinfo);

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    return new EglDrawable(visual, width, height, pbuffer);
}

bool
bindApi(Api api)
{
    EGLenum eglApi;
    switch (api) {
    case API_GL:
        eglApi = EGL_OPENGL_API;
        break;
    case API_GLES:
        eglApi = EGL_OPENGL_ES_API;
        break;
    default:
        assert(0);
        return false;
    }

    return eglBindAPI(eglApi);
}

Context *
createContext(const Visual *_visual, Context *shareContext, bool debug)
{
    Profile profile = _visual->profile;
    const EglVisual *visual = static_cast<const EglVisual *>(_visual);
    EGLContext share_context = EGL_NO_CONTEXT;
    EGLContext context;
    Attributes<EGLint> attribs;

    if (shareContext) {
        share_context = static_cast<EglContext*>(shareContext)->context;
    }

    EGLint api = eglQueryAPI();

    if (profile.api == API_GL) {
        load("libGL.so.1");
        eglBindAPI(EGL_OPENGL_API);

        if (has_EGL_KHR_create_context) {
            attribs.add(EGL_CONTEXT_MAJOR_VERSION_KHR, profile.major);
            attribs.add(EGL_CONTEXT_MINOR_VERSION_KHR, profile.minor);
            int profileMask = profile.core ? EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR : EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;
            attribs.add(EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, profileMask);
        } else if (profile.versionGreaterOrEqual(3, 2)) {
            std::cerr << "error: EGL_KHR_create_context not supported\n";
            return NULL;
        }
    } else if (profile.api == API_GLES) {
        if (profile.major >= 2) {
            load("libGLESv2.so.2");
        } else {
            load("libGLESv1_CM.so.1");
        }

        eglBindAPI(EGL_OPENGL_ES_API);

        if (has_EGL_KHR_create_context) {
            attribs.add(EGL_CONTEXT_MAJOR_VERSION_KHR, profile.major);
            attribs.add(EGL_CONTEXT_MINOR_VERSION_KHR, profile.minor);
        } else {
            attribs.add(EGL_CONTEXT_CLIENT_VERSION, profile.major);
        }
    } else {
        assert(0);
        return NULL;
    }

    if (debug && has_EGL_KHR_create_context) {
        attribs.add(EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR);
    }

    attribs.end(EGL_NONE);

    context = eglCreateContext(eglDisplay, visual->config, share_context, attribs);
    if (!context) {
        if (debug) {
            // XXX: Mesa has problems with EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR
            // with OpenGL ES contexts, so retry without it
            return createContext(_visual, shareContext, false);
        }
        return NULL;
    }

    eglBindAPI(api);

    return new EglContext(visual, context);
}

bool
makeCurrent(Drawable *drawable, Context *context)
{
    if (!drawable || !context) {
        return eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    } else {
        EglDrawable *eglDrawable = static_cast<EglDrawable *>(drawable);
        EglContext *eglContext = static_cast<EglContext *>(context);
        EGLBoolean ok;

        ok = eglMakeCurrent(eglDisplay, eglDrawable->surface,
                            eglDrawable->surface, eglContext->context);

        if (ok) {
            EGLint api;

            eglQueryContext(eglDisplay, eglContext->context,
                            EGL_CONTEXT_CLIENT_TYPE, &api);

            eglDrawable->api = api;
        }

        return ok;
    }
}



} /* namespace glws */

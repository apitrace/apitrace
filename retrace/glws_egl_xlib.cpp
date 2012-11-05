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


namespace glws {


static Display *display = NULL;
static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static int screen = 0;


class EglVisual : public Visual
{
public:
    EGLConfig config;
    XVisualInfo *visinfo;

    EglVisual() :
        config(0),
        visinfo(0)
    {}

    ~EglVisual() {
        XFree(visinfo);
    }
};


static void describeEvent(const XEvent &event) {
    if (0) {
        switch (event.type) {
        case ConfigureNotify:
            std::cerr << "ConfigureNotify";
            break;
        case Expose:
            std::cerr << "Expose";
            break;
        case KeyPress:
            std::cerr << "KeyPress";
            break;
        case MapNotify:
            std::cerr << "MapNotify";
            break;
        case ReparentNotify:
            std::cerr << "ReparentNotify";
            break;
        default:
            std::cerr << "Event " << event.type;
        }
        std::cerr << " " << event.xany.window << "\n";
    }
}

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

        Window root = RootWindow(display, screen);

        /* window attributes */
        XSetWindowAttributes attr;
        attr.background_pixel = 0;
        attr.border_pixel = 0;
        attr.colormap = XCreateColormap(display, root, visinfo->visual, AllocNone);
        attr.event_mask = StructureNotifyMask;

        unsigned long mask;
        mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

        int x = 0, y = 0;

        window = XCreateWindow(
            display, root,
            x, y, width, height,
            0,
            visinfo->depth,
            InputOutput,
            visinfo->visual,
            mask,
            &attr);

        XSizeHints sizehints;
        sizehints.x = x;
        sizehints.y = y;
        sizehints.width  = width;
        sizehints.height = height;
        sizehints.flags = USSize | USPosition;
        XSetNormalHints(display, window, &sizehints);

        const char *name = "glretrace";
        XSetStandardProperties(
            display, window, name, name,
            None, (char **)NULL, 0, &sizehints);

        eglWaitNative(EGL_CORE_NATIVE_ENGINE);

        EGLConfig config = static_cast<const EglVisual *>(visual)->config;
        surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
    }

    void waitForEvent(int type) {
        XEvent event;
        do {
            XWindowEvent(display, window, StructureNotifyMask, &event);
            describeEvent(event);
        } while (event.type != type);
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
        EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_DRAW);
        bool rebindDrawSurface = currentDrawSurface == surface;
        bool rebindReadSurface = currentReadSurface == surface;

        if (rebindDrawSurface || rebindReadSurface) {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }

        eglDestroySurface(eglDisplay, surface);

        EGLConfig config = static_cast<const EglVisual *>(visual)->config;
        surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);

        if (rebindDrawSurface || rebindReadSurface) {
            eglMakeCurrent(eglDisplay, surface, surface, currentContext);
        }
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

        XResizeWindow(display, window, w, h);

        // Tell the window manager to respect the requested size
        XSizeHints size_hints;
        size_hints.max_width  = size_hints.min_width  = w;
        size_hints.max_height = size_hints.min_height = h;
        size_hints.flags = PMinSize | PMaxSize;
        XSetWMNormalHints(display, window, &size_hints);

        waitForEvent(ConfigureNotify);

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

        XMapWindow(display, window);

        waitForEvent(MapNotify);

        eglWaitNative(EGL_CORE_NATIVE_ENGINE);

        Drawable::show();
    }

    void swapBuffers(void) {
        eglBindAPI(api);
        eglSwapBuffers(eglDisplay, surface);
    }
};


class EglContext : public Context
{
public:
    EGLContext context;

    EglContext(const Visual *vis, Profile prof, EGLContext ctx) :
        Context(vis, prof),
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

    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "error: unable to open display " << XDisplayName(NULL) << "\n";
        exit(1);
    }

    screen = DefaultScreen(display);

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
}

void
cleanup(void) {
    if (display) {
        eglTerminate(eglDisplay);
        XCloseDisplay(display);
        display = NULL;
    }
}

Visual *
createVisual(bool doubleBuffer, Profile profile) {
    EglVisual *visual = new EglVisual();
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

    switch(profile) {
    case PROFILE_COMPAT:
        api_bits = api_bits_gl;
        break;
    case PROFILE_ES1:
        api_bits = api_bits_gles1;
        break;
    case PROFILE_ES2:
        api_bits = api_bits_gles2;
        break;
    default:
        return NULL;
    };

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

Context *
createContext(const Visual *_visual, Context *shareContext, Profile profile, bool debug)
{
    const EglVisual *visual = static_cast<const EglVisual *>(_visual);
    EGLContext share_context = EGL_NO_CONTEXT;
    EGLContext context;
    Attributes<EGLint> attribs;

    if (shareContext) {
        share_context = static_cast<EglContext*>(shareContext)->context;
    }

    EGLint api = eglQueryAPI();

    switch (profile) {
    case PROFILE_COMPAT:
        load("libGL.so.1");
        eglBindAPI(EGL_OPENGL_API);
        break;
    case PROFILE_CORE:
        assert(0);
        return NULL;
    case PROFILE_ES1:
        load("libGLESv1_CM.so.1");
        eglBindAPI(EGL_OPENGL_ES_API);
        break;
    case PROFILE_ES2:
        load("libGLESv2.so.2");
        eglBindAPI(EGL_OPENGL_ES_API);
        attribs.add(EGL_CONTEXT_CLIENT_VERSION, 2);
        break;
    default:
        return NULL;
    }

    attribs.end(EGL_NONE);

    context = eglCreateContext(eglDisplay, visual->config, share_context, attribs);
    if (!context)
        return NULL;

    eglBindAPI(api);

    return new EglContext(visual, profile, context);
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

bool
processEvents(void) {
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);
        describeEvent(event);
    }
    return true;
}


} /* namespace glws */

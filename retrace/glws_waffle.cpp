/*********************************************************************
 *
 * Copyright 2013 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#define WAFFLE_API_EXPERIMENTAL
#define WAFFLE_API_VERSION 0x0103

#include <string.h>

#include <iostream>

#include "waffle.h"
#include "glws.hpp"

namespace glws {

struct waffle_display *dpy;

class WaffleVisual : public Visual
{
public:
    struct waffle_config *config;

    WaffleVisual(Profile prof, waffle_config *cfg) :
        Visual(prof),
        config (cfg)
    {}

    ~WaffleVisual() {
        if (this->config != NULL) {
            waffle_config_destroy(this->config);
            this->config = NULL;
        }
    }
};

class WaffleDrawable : public Drawable
{
public:
    struct waffle_window *window;

    WaffleDrawable(const Visual *vis, int w, int h,
        const pbuffer_info *info,
        struct waffle_window *win) :
        Drawable (vis, w, h, info),
        window (win)
    {}

    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }
        waffle_window_resize(window, w, h);
        Drawable::resize(w, h);
    }

    void
    show(void) {
        if (visible) {
            return;
        }
        waffle_window_show(window);
        Drawable::show();
    }

    void
    swapBuffers(void) {
        waffle_window_swap_buffers(window);
    }
};

class WaffleContext : public Context
{
public:
    struct waffle_context *context;

    WaffleContext(const Visual *vis, struct waffle_context *ctx) :
        Context(vis),
        context(ctx)
    {}

    ~WaffleContext() {
        if( context != NULL ) {
            waffle_context_destroy(context);
            context = NULL;
        }
    }
};

/*
 With waffle there is not too many events to look for..
 */
bool
processEvents(void) {
    return true;
}

void
init(void)
{
    int32_t waffle_platform;;

#if defined(__ANDROID__)
    waffle_platform = WAFFLE_PLATFORM_ANDROID;
#elif defined(__APPLE__)
    waffle_platform = WAFFLE_PLATFORM_CGL
#elif defined(_WIN32)
    waffle_platform = WAFFLE_PLATFORM_WGL
#else
    waffle_platform = WAFFLE_PLATFORM_GLX;

    const char *waffle_platform_name = getenv("WAFFLE_PLATFORM");
    if (waffle_platform_name) {
        if (strcmp(waffle_platform_name, "gbm") == 0) {
            waffle_platform = WAFFLE_PLATFORM_GBM;
        } else if (strcmp(waffle_platform_name, "glx") == 0) {
            waffle_platform = WAFFLE_PLATFORM_GLX;
        } else if (strcmp(waffle_platform_name, "wayland") == 0) {
            waffle_platform = WAFFLE_PLATFORM_WAYLAND;
        } else if (strcmp(waffle_platform_name, "x11_egl") == 0) {
            waffle_platform = WAFFLE_PLATFORM_X11_EGL;
        } else {
            std::cerr << "error: unsupported WAFFLE_PLATFORM \"" << waffle_platform_name << "\"\n";
            exit(1);
        }
    }
#endif

    Attributes<int32_t> waffle_init_attrib_list;
    waffle_init_attrib_list.add(WAFFLE_PLATFORM, waffle_platform);
    waffle_init_attrib_list.end(WAFFLE_NONE);

    waffle_init(waffle_init_attrib_list);

    dpy = waffle_display_connect(NULL);
    if (!dpy) {
        std::cerr << "error: waffle_display_connect failed\n";
        exit(1);
    }
}

void
cleanup(void) {
    waffle_display_disconnect(dpy);
}

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {
    struct waffle_config *cfg;

    int waffle_api;
    if (profile.api == glfeatures::API_GL) {
        waffle_api = WAFFLE_CONTEXT_OPENGL;
    } else if (profile.api == glfeatures::API_GLES) {
        switch (profile.major) {
        case 1:
            waffle_api = WAFFLE_CONTEXT_OPENGL_ES1;
            break;
        case 2:
            waffle_api = WAFFLE_CONTEXT_OPENGL_ES2;
            break;
        case 3:
            waffle_api = WAFFLE_CONTEXT_OPENGL_ES3;
            break;
        default:
            std::cerr << "error: unsupported context profile " << profile << "\n";
            exit(1);
            return NULL;
        }
    } else {
        assert(0);
        return NULL;
    }

    if (!waffle_display_supports_context_api(dpy, waffle_api)) {
        std::cerr << "error: waffle_display_supports_context_api failed \n";
        exit(1);
        return NULL;
    }

    Attributes<int32_t> config_attrib_list;
    config_attrib_list.add(WAFFLE_CONTEXT_API, waffle_api);
    if (profile.api == glfeatures::API_GL) {
        config_attrib_list.add(WAFFLE_CONTEXT_MAJOR_VERSION, profile.major);
        config_attrib_list.add(WAFFLE_CONTEXT_MINOR_VERSION, profile.minor);
        if (profile.versionGreaterOrEqual(3, 2)) {
            int profileMask = profile.core ? WAFFLE_CONTEXT_CORE_PROFILE : WAFFLE_CONTEXT_COMPATIBILITY_PROFILE;
            config_attrib_list.add(WAFFLE_CONTEXT_PROFILE, profileMask);
        }
        if (profile.forwardCompatible) {
            config_attrib_list.add(WAFFLE_CONTEXT_FORWARD_COMPATIBLE, true);
        }
    }
    config_attrib_list.add(WAFFLE_RED_SIZE, 8);
    config_attrib_list.add(WAFFLE_GREEN_SIZE, 8);
    config_attrib_list.add(WAFFLE_BLUE_SIZE, 8);
    config_attrib_list.add(WAFFLE_DEPTH_SIZE, 8);
    config_attrib_list.add(WAFFLE_ALPHA_SIZE, 8);
    config_attrib_list.add(WAFFLE_STENCIL_SIZE, 8);
    config_attrib_list.add(WAFFLE_DOUBLE_BUFFERED, doubleBuffer);
    if (0) {
        config_attrib_list.add(WAFFLE_CONTEXT_DEBUG, true);
    }
    config_attrib_list.end();

    cfg = waffle_config_choose(dpy, config_attrib_list);
    if (!cfg) {
        std::cerr << "error: waffle_config_choose failed\n";
        exit(1);
        return NULL;
    }
    return new WaffleVisual(profile, cfg);
}

Drawable *
createDrawable(const Visual *visual, int width, int height,
               const pbuffer_info *info)
{
    struct waffle_window *window;
    const WaffleVisual *waffleVisual =
        static_cast<const WaffleVisual *>(visual);

    window = waffle_window_create(waffleVisual->config, width, height);
    return new WaffleDrawable(visual, width, height, info, window);
}

Context *
createContext(const Visual *visual, Context *shareContext,
              bool debug)
{
    struct waffle_context *context;
    const WaffleVisual *waffleVisual =
        static_cast<const WaffleVisual *>(visual);

    struct waffle_context * share_context = NULL;
    if (shareContext) {
        share_context = static_cast<WaffleContext*>(shareContext)->context;
    }

    context = waffle_context_create(waffleVisual->config, share_context);
    if (!context) {
        std::cerr << "error: waffle_context_create failed\n";
        exit(1);
        return NULL;
    }
    return new WaffleContext(visual, context);
}

bool
makeCurrentInternal(Drawable *drawable, Context *context)
{
    if (!drawable || !context) {
        return waffle_make_current(dpy, NULL, NULL);
    } else {
        WaffleDrawable *waffleDrawable =
            static_cast<WaffleDrawable *>(drawable);

        WaffleContext *waffleContext =
            static_cast<WaffleContext *>(context);

        return waffle_make_current(dpy, waffleDrawable->window,
                                   waffleContext->context);
    }
}

bool
bindTexImage(Drawable *pBuffer, int iBuffer) {
    std::cerr << "error: Waffle::wglBindTexImageARB not implemented.\n";
    assert(pBuffer->pbuffer);
    return true;
}

bool
releaseTexImage(Drawable *pBuffer, int iBuffer) {
    std::cerr << "error: Waffle::wglReleaseTexImageARB not implemented.\n";
    assert(pBuffer->pbuffer);
    return true;
}

bool
setPbufferAttrib(Drawable *pBuffer, const int *attribList) {
    // nothing to do here.
    assert(pBuffer->pbuffer);
    return true;
}

} /* namespace glws */

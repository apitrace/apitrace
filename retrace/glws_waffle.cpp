#define WAFFLE_API_EXPERIMENTAL
#define WAFFLE_API_VERSION 0x0103

#include "os.hpp"
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

    WaffleDrawable(const Visual *vis, int w, int h, bool pbuffer,
        struct waffle_window *win) :
        Drawable (vis, w, h, pbuffer),
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
init(void) {
    Attributes<int32_t> waffle_init_attrib_list;
    waffle_init_attrib_list.add(
        WAFFLE_PLATFORM,
#if defined(__ANDROID__)
        WAFFLE_PLATFORM_ANDROID
#elif defined(__APPLE__)
        WAFFLE_PLATFORM_CGL
#elif defined(HAVE_X11)
#  if 1
        WAFFLE_PLATFORM_GLX
#  else
        WAFFLE_PLATFORM_X11_EGL
#  endif
#else
#  warning Unsupported platform
        WAFFLE_NONE
#endif
    );
    waffle_init_attrib_list.end(WAFFLE_NONE);

    waffle_init(waffle_init_attrib_list);

    dpy = waffle_display_connect(NULL);
    if (!dpy) {
        os::log("%s: waffle_display_connect(NULL) == NULL\n", __FILE__);
        os::abort();
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
    if (profile.api == glprofile::API_GL) {
        waffle_api = WAFFLE_CONTEXT_OPENGL;
    } else if (profile.api == glprofile::API_GLES) {
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
            os::log("%s: Unsupported context profile\n", __FILE__);
            os::abort();
            return NULL;
        }
    } else {
        assert(0);
        return NULL;
    }

    if (!waffle_display_supports_context_api(dpy, waffle_api)) {
        os::log("%s: !waffle_display_supports_context_api\n",
            __FILE__);

        os::abort();
        return NULL;
    }

    Attributes<int32_t> config_attrib_list;
    config_attrib_list.add(WAFFLE_CONTEXT_API, waffle_api);
    if (profile.api == glprofile::API_GL) {
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
    if (!cfg)
    {
        os::log("Error in %s waffle_config_choose(dpy, " \
            "config_attrib_list)\n", __FILE__);
        os::abort();
        return NULL;
    }
    return new WaffleVisual(profile, cfg);
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    struct waffle_window *window;
    const WaffleVisual *waffleVisual =
        static_cast<const WaffleVisual *>(visual);

    window = waffle_window_create(waffleVisual->config, width, height);
    return new WaffleDrawable(visual, width, height, pbuffer, window);
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
        os::log("Error in %s waffle_context_create()\n",
               __FILE__);

        os::abort();
        return NULL;
    }
    return new WaffleContext(visual, context);
}

bool
makeCurrent(Drawable *drawable, Context *context)
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

} /* namespace glws */

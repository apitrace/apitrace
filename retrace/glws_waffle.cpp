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
    int i;
    int waffle_init_attrib_list[3];

    i = 0;
    waffle_init_attrib_list[i++] = WAFFLE_PLATFORM;
    waffle_init_attrib_list[i++] =
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
    ;
    waffle_init_attrib_list[i++] = WAFFLE_NONE;

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
    int config_attrib_list[64], i(0), waffle_profile;

    switch (profile) {
    case PROFILE_ES1:
        waffle_profile = WAFFLE_CONTEXT_OPENGL_ES1;
        break;
    case PROFILE_ES2:
        waffle_profile = WAFFLE_CONTEXT_OPENGL_ES2;
        break;
    default:
        os::log("%s: Unsupported context profile\n", __FILE__);
        os::abort();
        return NULL;
    }

    if(!waffle_display_supports_context_api(dpy, waffle_profile)) {
        os::log("%s: !waffle_display_supports_context_api\n",
            __FILE__);

        os::abort();
        return NULL;
    }

    config_attrib_list[i++] = WAFFLE_CONTEXT_API;
    config_attrib_list[i++] = waffle_profile;
    config_attrib_list[i++] = WAFFLE_RED_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_GREEN_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_BLUE_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_DEPTH_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_ALPHA_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_STENCIL_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_DOUBLE_BUFFERED;
    config_attrib_list[i++] = doubleBuffer;
    config_attrib_list[i++] = 0;

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

bool
bindApi(Api api)
{
    return true;
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

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

#include <assert.h>
#include <stdlib.h>

#include <iostream>

#include "glproc.hpp"
#include "glws.hpp"


namespace glws {


static Display *display = NULL;
static int screen = 0;

static unsigned glxVersion = 0;
static const char *extensions = 0;
static bool has_GLX_ARB_create_context = false;


class GlxVisual : public Visual
{
public:
    GLXFBConfig fbconfig;
    XVisualInfo *visinfo;

    GlxVisual() :
        fbconfig(0),
        visinfo(0)
    {}

    ~GlxVisual() {
        XFree(visinfo);
    }
};


static void
processEvent(XEvent &event) {
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

    switch (event.type) {
    case KeyPress:
        {
            char buffer[32];
            KeySym keysym;
            XLookupString(&event.xkey, buffer, sizeof buffer - 1, &keysym, NULL);
            if (keysym == XK_Escape) {
                exit(0);
            }
        }
        break;
    }
}

class GlxDrawable : public Drawable
{
public:
    Window window;

    GlxDrawable(const Visual *vis, int w, int h, bool pbuffer) :
        Drawable(vis, w, h, pbuffer)
    {
        XVisualInfo *visinfo = static_cast<const GlxVisual *>(visual)->visinfo;

        Window root = RootWindow(display, screen);

        /* window attributes */
        XSetWindowAttributes attr;
        attr.background_pixel = 0;
        attr.border_pixel = 0;
        attr.colormap = XCreateColormap(display, root, visinfo->visual, AllocNone);
        attr.event_mask = StructureNotifyMask | KeyPressMask;

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

        glXWaitX();
    }

    void processKeys(void) {
        XEvent event;
        while (XCheckWindowEvent(display, window, StructureNotifyMask | KeyPressMask, &event)) {
            processEvent(event);
        }
    }

    void waitForEvent(int type) {
        XEvent event;
        do {
            XWindowEvent(display, window, StructureNotifyMask | KeyPressMask, &event);
            processEvent(event);
        } while (event.type != type);
    }

    ~GlxDrawable() {
        XDestroyWindow(display, window);
    }

    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }

        glXWaitGL();

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

        glXWaitX();
    }

    void show(void) {
        if (visible) {
            return;
        }

        glXWaitGL();

        XMapWindow(display, window);

        waitForEvent(MapNotify);

        glXWaitX();

        Drawable::show();
    }

    void swapBuffers(void) {
        glXSwapBuffers(display, window);

        processKeys();
    }
};


class GlxContext : public Context
{
public:
    GLXContext context;

    GlxContext(const Visual *vis, Profile prof, GLXContext ctx) :
        Context(vis, prof),
        context(ctx)
    {}

    ~GlxContext() {
        glXDestroyContext(display, context);
    }
};

void
init(void) {
    XInitThreads();

    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "error: unable to open display " << XDisplayName(NULL) << "\n";
        exit(1);
    }

    screen = DefaultScreen(display);

    int major = 0, minor = 0;
    glXQueryVersion(display, &major, &minor);
    glxVersion = (major << 8) | minor;

    extensions = glXQueryExtensionsString(display, screen);
    has_GLX_ARB_create_context = checkExtension("GLX_ARB_create_context", extensions);
}

void
cleanup(void) {
    if (display) {
        XCloseDisplay(display);
        display = NULL;
    }
}

Visual *
createVisual(bool doubleBuffer, Profile profile) {
    if (profile != PROFILE_COMPAT &&
        profile != PROFILE_CORE) {
        return NULL;
    }

    GlxVisual *visual = new GlxVisual;

    if (glxVersion >= 0x0103) {
        Attributes<int> attribs;
        attribs.add(GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT);
        attribs.add(GLX_RENDER_TYPE, GLX_RGBA_BIT);
        attribs.add(GLX_RED_SIZE, 1);
        attribs.add(GLX_GREEN_SIZE, 1);
        attribs.add(GLX_BLUE_SIZE, 1);
        attribs.add(GLX_ALPHA_SIZE, 1);
        attribs.add(GLX_DOUBLEBUFFER, doubleBuffer ? GL_TRUE : GL_FALSE);
        attribs.add(GLX_DEPTH_SIZE, 1);
        attribs.add(GLX_STENCIL_SIZE, 1);
        attribs.end();

        int num_configs = 0;
        GLXFBConfig * fbconfigs;
        fbconfigs = glXChooseFBConfig(display, screen, attribs, &num_configs);
        assert(num_configs && fbconfigs);
        visual->fbconfig = fbconfigs[0];
        assert(visual->fbconfig);
        visual->visinfo = glXGetVisualFromFBConfig(display, visual->fbconfig);
        assert(visual->visinfo);
    } else {
        Attributes<int> attribs;
        attribs.add(GLX_RGBA);
        attribs.add(GLX_RED_SIZE, 1);
        attribs.add(GLX_GREEN_SIZE, 1);
        attribs.add(GLX_BLUE_SIZE, 1);
        attribs.add(GLX_ALPHA_SIZE, 1);
        if (doubleBuffer) {
            attribs.add(GLX_DOUBLEBUFFER);
        }
        attribs.add(GLX_DEPTH_SIZE, 1);
        attribs.add(GLX_STENCIL_SIZE, 1);
        attribs.end();

        visual->visinfo = glXChooseVisual(display, screen, attribs);
    }

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    return new GlxDrawable(visual, width, height, pbuffer);
}

Context *
createContext(const Visual *_visual, Context *shareContext, Profile profile, bool debug)
{
    const GlxVisual *visual = static_cast<const GlxVisual *>(_visual);
    GLXContext share_context = NULL;
    GLXContext context;

    if (shareContext) {
        share_context = static_cast<GlxContext*>(shareContext)->context;
    }

    if (glxVersion >= 0x0104 && has_GLX_ARB_create_context) {
        Attributes<int> attribs;
        
        attribs.add(GLX_RENDER_TYPE, GLX_RGBA_TYPE);
        if (debug) {
            attribs.add(GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB);
        }

        switch (profile) {
        case PROFILE_COMPAT:
            break;
        case PROFILE_CORE:
            // XXX: This will invariable return a 3.2 context, when supported.
            // We probably should have a PROFILE_CORE_XX per version.
            attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, 3);
            attribs.add(GLX_CONTEXT_MINOR_VERSION_ARB, 2);
            attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB);
            break;
        default:
            return NULL;
        }
        
        attribs.end();

        context = glXCreateContextAttribsARB(display, visual->fbconfig, share_context, True, attribs);
    } else {
        if (profile != PROFILE_COMPAT) {
            return NULL;
        }

        if (glxVersion >= 0x103) {
            context = glXCreateNewContext(display, visual->fbconfig, GLX_RGBA_TYPE, share_context, True);
        } else {
            context = glXCreateContext(display, visual->visinfo, share_context, True);
        }
    }

    if (!context) {
        return NULL;
    }

    return new GlxContext(visual, profile, context);
}

bool
makeCurrent(Drawable *drawable, Context *context)
{
    if (!drawable || !context) {
        return glXMakeCurrent(display, None, NULL);
    } else {
        GlxDrawable *glxDrawable = static_cast<GlxDrawable *>(drawable);
        GlxContext *glxContext = static_cast<GlxContext *>(context);

        return glXMakeCurrent(display, glxDrawable->window, glxContext->context);
    }
}

bool
processEvents(void) {
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);
        processEvent(event);
    }
    return true;
}


} /* namespace glws */

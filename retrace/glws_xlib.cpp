/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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
#include "ws.hpp"


namespace glws {


Display *display = NULL;
int screen = 0;


static void
describeEvent(const XEvent &event) {
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


static void
processEvent(XEvent &event) {
    describeEvent(event);

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


static int
errorHandler(Display *dpy, XErrorEvent *error)
{
    char buffer[512];
    XGetErrorText(dpy, error->error_code, buffer, sizeof buffer);
    std::cerr << "error: xlib: " << buffer;

    if (error->request_code < 128) {
        std::string request_code = std::to_string(error->request_code);
        XGetErrorDatabaseText(dpy, "XRequest", request_code.c_str(), "", buffer, sizeof buffer);
        std::cerr << " in " << buffer;
    }

    std::cerr << "\n";
    return 0;
}

static int (*oldErrorHandler)(Display *, XErrorEvent *) = NULL;


void
initX(void)
{
#ifndef NDEBUG
    _Xdebug = 1;
#endif

    XInitThreads();

    oldErrorHandler = XSetErrorHandler(errorHandler);

    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "error: unable to open display " << XDisplayName(NULL) << "\n";
        exit(1);
    }

    screen = DefaultScreen(display);
}

void
cleanupX(void)
{
    if (display) {
        XCloseDisplay(display);
        display = NULL;
    }

    XSetErrorHandler(oldErrorHandler);
    oldErrorHandler = NULL;
}


bool
processEvents(void)
{
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);
        processEvent(event);
    }
    return true;
}


static XEvent
waitForEvent(Window window, int type, long request = 0)
{
    XEvent event;
    do {
        XWindowEvent(display, window, StructureNotifyMask | KeyPressMask, &event);
        processEvent(event);
    } while (event.type != type || event.xany.serial < request);
    return event;
}


void
processKeys(Window window)
{
    XEvent event;
    while (XCheckWindowEvent(display, window, StructureNotifyMask | KeyPressMask, &event)) {
        processEvent(event);
    }
}


Window
createWindow(XVisualInfo *visinfo,
             const char *name,
             int width, int height)
{
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

    Window window = XCreateWindow(
        display, root,
        x, y, width, height,
        0,
        visinfo->depth,
        InputOutput,
        visinfo->visual,
        mask,
        &attr);

    XSizeHints *sizeHints = XAllocSizeHints();
    sizeHints->x = x;
    sizeHints->y = y;
    sizeHints->width  = width;
    sizeHints->height = height;
    sizeHints->flags = USSize | USPosition;
    XSetWMNormalHints(display, window, sizeHints);

    XSetStandardProperties(
        display, window, name, name,
        None, (char **)NULL, 0, sizeHints);

    XFree(sizeHints);

    XSync(display, False);

    return window;
}


void
resizeWindow(Window window, int w, int h)
{
    // We need to ensure that pending events are processed here, and XSync
    // with discard = True guarantees that, but it appears the limited
    // event processing we do so far is sufficient
    if (0) {
        XSync(display, True);
    } else {
        processEvents();
    }

    // Tell the window manager to respect the requested size
    XSizeHints *sizeHints = XAllocSizeHints();
    sizeHints->max_width  = sizeHints->min_width  = w;
    sizeHints->max_height = sizeHints->min_height = h;
    sizeHints->flags = PMinSize | PMaxSize;
    XSetWMNormalHints(display, window, sizeHints);
    XFree(sizeHints);

    // We need to try multiple times because we can't distinguish the configure
    // event due to our resize, from the configure event from other actions
    // (e.g, user moving our window around.)  It's not safe to wait
    // indefinitely for the event with the desired size neither, because it's
    // possible the window manager does not allow a window that size.
    XEvent event;
    for (unsigned attempt = 0; attempt < 4; ++attempt) {
        long request = NextRequest(display);
        XResizeWindow(display, window, w, h);

        event = waitForEvent(window, ConfigureNotify, request);
        assert(event.type == ConfigureNotify);
        assert(event.xany.window == window);
        assert(event.xany.serial >= request);
        if (event.xconfigure.width == w &&
            event.xconfigure.height == h) {
            return;
        }
    }

    std::cerr
        << "warning: xlib: expected " << w << "x" << h << " configure event, "
        << "got " << event.xconfigure.width << "x" << event.xconfigure.height << "\n";
}


void
showWindow(Window window)
{
    // FIXME: This works for DRI drivers, but not NVIDIA proprietary drivers,
    // for which the only solution seems to be to use Pbuffers.
    if (true || !ws::headless) {
        long request = NextRequest(display);
        XMapWindow(display, window);
        waitForEvent(window, MapNotify, request);
    }
}


void
setWindowName(Window window, const char *name)
{
    XStoreName(display, window, const_cast<char *>(name));
}


} /* namespace glws */

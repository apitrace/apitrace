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
#include <limits.h>

#include <iostream>

#include "glproc.hpp"
#include "glws.hpp"
#include "glws_xlib.hpp"


namespace glws {


static const char *extensions = 0;
static bool has_GLX_ARB_create_context = false;
static bool has_GLX_ARB_create_context_profile = false;
static bool has_GLX_EXT_create_context_es_profile = false;
static bool has_GLX_EXT_create_context_es2_profile = false;


class GlxVisual : public Visual
{
public:
    GLXFBConfig fbconfig;
    XVisualInfo *visinfo;

    GlxVisual(Profile prof) :
        Visual(prof),
        fbconfig(0),
        visinfo(0)
    {}

    ~GlxVisual() {
        XFree(visinfo);
    }
};


class GlxDrawable : public Drawable
{
public:
    Window window;
    bool ever_current;

    GlxDrawable(const Visual *vis, int w, int h, bool pbuffer) :
        Drawable(vis, w, h, pbuffer),
        ever_current(false)
    {
        XVisualInfo *visinfo = static_cast<const GlxVisual *>(visual)->visinfo;

        const char *name = "glretrace";
        window = createWindow(visinfo, name, width, height);

        glXWaitX();
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

        resizeWindow(window, w, h);

        glXWaitX();
    }

    void show(void) {
        if (visible) {
            return;
        }

        glXWaitGL();

        showWindow(window);

        glXWaitX();

        Drawable::show();
    }

    void copySubBuffer(int x, int y, int width, int height) {
        glXCopySubBufferMESA(display, window, x, y, width, height);

        processKeys(window);
    }

    void swapBuffers(void) {
        if (ever_current) {
            // The window has been bound to a context at least once
            glXSwapBuffers(display, window);
        } else {
            // Don't call glXSwapBuffers on this window to avoid an
            // (untrappable) X protocol error with NVIDIA's driver.
            std::cerr << "warning: attempt to issue SwapBuffers on unbound window "
                         " - skipping.\n";
        }
        processKeys(window);
    }
};


class GlxContext : public Context
{
public:
    GLXContext context;

    GlxContext(const Visual *vis, GLXContext ctx) :
        Context(vis),
        context(ctx)
    {}

    ~GlxContext() {
        glXDestroyContext(display, context);
    }
};


#ifndef GLXBadFBConfig
#define GLXBadFBConfig 9
#endif

static int errorBase = INT_MIN;
static int eventBase = INT_MIN;

static int (*oldErrorHandler)(Display *, XErrorEvent *) = NULL;

static int
errorHandler(Display *dpy, XErrorEvent *error)
{
    if (error->error_code == errorBase + GLXBadFBConfig) {
        // Ignore, as we handle these.
        return 0;
    }

    return oldErrorHandler(dpy, error);
}


void
init(void) {
    initX();

    int major = 0, minor = 0;
    if (!glXQueryVersion(display, &major, &minor)) {
        std::cerr << "error: failed to obtain GLX version\n";
        exit(1);
    }
    const int requiredMajor = 1, requiredMinor = 3;
    if (major < requiredMajor ||
        (major == requiredMajor && minor < requiredMinor)) {
        std::cerr << "error: GLX version " << requiredMajor << "." << requiredMinor << " required, but got version " << major << "." << minor << "\n";
        exit(1);
    }

    glXQueryExtension(display, &errorBase, &eventBase);
    oldErrorHandler = XSetErrorHandler(errorHandler);

    extensions = glXQueryExtensionsString(display, screen);

#define CHECK_EXTENSION(name) \
    has_##name = checkExtension(#name, extensions)

    CHECK_EXTENSION(GLX_ARB_create_context);
    CHECK_EXTENSION(GLX_ARB_create_context_profile);
    CHECK_EXTENSION(GLX_EXT_create_context_es_profile);
    CHECK_EXTENSION(GLX_EXT_create_context_es2_profile);

#undef CHECK_EXTENSION
}

void
cleanup(void) {
    XSetErrorHandler(oldErrorHandler);
    oldErrorHandler = NULL;

    cleanupX();
}

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {
    GlxVisual *visual = new GlxVisual(profile);

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
    if (samples > 1) {
        attribs.add(GLX_SAMPLE_BUFFERS, 1);
        attribs.add(GLX_SAMPLES_ARB, samples);
    }
    attribs.end();

    int num_configs = 0;
    GLXFBConfig * fbconfigs;
    fbconfigs = glXChooseFBConfig(display, screen, attribs, &num_configs);
    if (!num_configs || !fbconfigs) {
        return NULL;
    }
    visual->fbconfig = fbconfigs[0];
    assert(visual->fbconfig);
    visual->visinfo = glXGetVisualFromFBConfig(display, visual->fbconfig);
    assert(visual->visinfo);

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    return new GlxDrawable(visual, width, height, pbuffer);
}

Context *
createContext(const Visual *_visual, Context *shareContext, bool debug)
{
    const GlxVisual *visual = static_cast<const GlxVisual *>(_visual);
    Profile profile = visual->profile;
    GLXContext share_context = NULL;
    GLXContext context;

    if (shareContext) {
        share_context = static_cast<GlxContext*>(shareContext)->context;
    }

    if (has_GLX_ARB_create_context) {
        Attributes<int> attribs;
        attribs.add(GLX_RENDER_TYPE, GLX_RGBA_TYPE);
        int contextFlags = 0;
        if (profile.api == glprofile::API_GL) {
            attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, profile.major);
            attribs.add(GLX_CONTEXT_MINOR_VERSION_ARB, profile.minor);
            if (profile.versionGreaterOrEqual(3, 2)) {
                if (!has_GLX_ARB_create_context_profile) {
                    std::cerr << "error: GLX_ARB_create_context_profile not supported\n";
                    return NULL;
                }
                int profileMask = profile.core ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB : GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, profileMask);
                if (profile.forwardCompatible) {
                    contextFlags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
                }
            }
        } else if (profile.api == glprofile::API_GLES) {
            if (has_GLX_EXT_create_context_es_profile) {
                attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES_PROFILE_BIT_EXT);
                attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, profile.major);
                attribs.add(GLX_CONTEXT_MINOR_VERSION_ARB, profile.minor);
            } else if (profile.major < 2) {
                std::cerr << "error: " << profile << " requested but GLX_EXT_create_context_es_profile not supported\n";
                return NULL;
            } else if (has_GLX_EXT_create_context_es2_profile) {
                assert(profile.major >= 2);
                if (profile.major != 2 || profile.minor != 0) {
                    // We might still get a ES 3.0 context later (in particular Mesa does this)
                    std::cerr << "warning: " << profile << " requested but GLX_EXT_create_context_es_profile not supported\n";
                }
                attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT);
                attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, 2);
                attribs.add(GLX_CONTEXT_MINOR_VERSION_ARB, 0);
            } else {
                std::cerr << "warning: " << profile << " requested but GLX_EXT_create_context_es_profile or GLX_EXT_create_context_es2_profile not supported\n";
            }
        } else {
            assert(0);
        }
        if (debug) {
            contextFlags |= GLX_CONTEXT_DEBUG_BIT_ARB;
        }
        if (contextFlags) {
            attribs.add(GLX_CONTEXT_FLAGS_ARB, contextFlags);
        }
        attribs.end();

        context = glXCreateContextAttribsARB(display, visual->fbconfig, share_context, True, attribs);
        if (!context && debug) {
            // XXX: Mesa has problems with GLX_CONTEXT_DEBUG_BIT_ARB with
            // OpenGL ES contexts, so retry without it
            return createContext(_visual, shareContext, false);
        }
    } else {
        if (profile.api != glprofile::API_GL ||
            profile.core) {
            return NULL;
        }

        context = glXCreateNewContext(display, visual->fbconfig, GLX_RGBA_TYPE, share_context, True);
    }

    if (!context) {
        return NULL;
    }

    return new GlxContext(visual, context);
}

bool
makeCurrent(Drawable *drawable, Context *context)
{
    if (!drawable || !context) {
        return glXMakeCurrent(display, None, NULL);
    } else {
        GlxDrawable *glxDrawable = static_cast<GlxDrawable *>(drawable);
        GlxContext *glxContext = static_cast<GlxContext *>(context);

        glxDrawable->ever_current = true;

        return glXMakeCurrent(display, glxDrawable->window, glxContext->context);
    }
}


} /* namespace glws */

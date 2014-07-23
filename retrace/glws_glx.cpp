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
#include "glws_xlib.hpp"


namespace glws {


static unsigned glxVersion = 0;
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

void
init(void) {
    initX();

    int major = 0, minor = 0;
    glXQueryVersion(display, &major, &minor);
    glxVersion = (major << 8) | minor;

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
    cleanupX();
}

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {
    GlxVisual *visual = new GlxVisual(profile);

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
        if (samples > 1) {
            attribs.add(GLX_SAMPLE_BUFFERS, 1);
            attribs.add(GLX_SAMPLES_ARB, samples);
        }
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

bool
bindApi(Api api)
{
    return true;
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

    if (glxVersion >= 0x0104 && has_GLX_ARB_create_context) {
        Attributes<int> attribs;
        
        attribs.add(GLX_RENDER_TYPE, GLX_RGBA_TYPE);
        if (debug) {
            attribs.add(GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB);
        }

        ProfileDesc desc;
        getProfileDesc(profile, desc);

        switch (profile) {
        case PROFILE_COMPAT:
            break;
        case PROFILE_ES1:
            if (!has_GLX_EXT_create_context_es_profile) {
                return NULL;
            }
            attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES_PROFILE_BIT_EXT);
            attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, 1);
            break;
        case PROFILE_ES2:
            if (!has_GLX_EXT_create_context_es2_profile) {
                return NULL;
            }
            attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT);
            attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, 2);
            break;
        default:
            {
                attribs.add(GLX_CONTEXT_MAJOR_VERSION_ARB, desc.major);
                attribs.add(GLX_CONTEXT_MINOR_VERSION_ARB, desc.minor);
                if (desc.core) {
                    if (!has_GLX_ARB_create_context_profile) {
                        return NULL;
                    }
                    attribs.add(GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB);
                }
                break;
            }
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

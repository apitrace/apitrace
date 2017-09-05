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
static bool has_GLX_EXT_swap_control = false;
static bool has_GLX_MESA_swap_control = false;
static bool has_GLX_OML_swap_method = false;


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
    Window window = 0;
    GLXDrawable drawable = 0;

    GlxDrawable(const Visual *vis, int w, int h,
                const glws::pbuffer_info *pbInfo) :
        Drawable(vis, w, h, pbInfo ? true : false)
    {
        const GlxVisual *glxvisual = static_cast<const GlxVisual *>(visual);
        XVisualInfo *visinfo = glxvisual->visinfo;

        const char *name = "glretrace";
        if (pbInfo) {
            drawable = createPbuffer(display, glxvisual, pbInfo, w, h);
        }
        else {
            window = createWindow(visinfo, name, width, height);
            drawable = glXCreateWindow(display, glxvisual->fbconfig, window, NULL);
            if (has_GLX_EXT_swap_control) {
                glXSwapIntervalEXT(display, drawable, 0);
            }
        }

        glXWaitX();
    }

    ~GlxDrawable() {
        if (pbuffer) {
            glXDestroyPbuffer(display, drawable);
        } else {
            glXDestroyWindow(display, drawable);
            XDestroyWindow(display, window);
        }
    }

    void
    resize(int w, int h) override {
        if (!window) {
            return;
        }

        if (w == width && h == height) {
            return;
        }

        glXWaitGL();

        Drawable::resize(w, h);

        resizeWindow(window, w, h);

        glXWaitX();
    }

    void show(void) override {
        if (!window ||
            visible) {
            return;
        }

        glXWaitGL();

        showWindow(window);

        glXWaitX();

        Drawable::show();
    }

    void copySubBuffer(int x, int y, int width, int height) override {
        glXCopySubBufferMESA(display, drawable, x, y, width, height);

        if (window) {
            processKeys(window);
        }
    }

    void swapBuffers(void) override {
        assert(!pbuffer);
        if (window &&
            !has_GLX_EXT_swap_control &&
            has_GLX_MESA_swap_control) {
            glXSwapIntervalMESA(0);
        }
        glXSwapBuffers(display, drawable);
        if (window) {
            processKeys(window);
        }
    }

private:
    Window createPbuffer(Display *dpy, const GlxVisual *visinfo,
                         const glws::pbuffer_info *pbInfo, int w, int h);
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
    CHECK_EXTENSION(GLX_EXT_swap_control);
    CHECK_EXTENSION(GLX_MESA_swap_control);
    CHECK_EXTENSION(GLX_OML_swap_method);

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
    attribs.add(GLX_RED_SIZE, 8);
    attribs.add(GLX_GREEN_SIZE, 8);
    attribs.add(GLX_BLUE_SIZE, 8);
    attribs.add(GLX_ALPHA_SIZE, 8);
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
    fbconfigs = glXChooseFBConfig(display, screen,
                                  attribs,
                                  &num_configs);
    if (!num_configs || !fbconfigs) {
        if (fbconfigs) {
            XFree(fbconfigs);
        }
        return NULL;
    }

    visual->fbconfig = fbconfigs[0];
    visual->visinfo = glXGetVisualFromFBConfig(display, visual->fbconfig);

    if (has_GLX_OML_swap_method) {
        XVisualInfo *visinfo;
        int ret;

        for (int i = 0; i < num_configs; ++i) {
            if (glXGetFBConfigAttrib(display, fbconfigs[i], GLX_SWAP_METHOD_OML,
                                     &ret) ||
                ret != GLX_SWAP_COPY_OML) {
                continue;
            }

            visinfo = glXGetVisualFromFBConfig(display, fbconfigs[i]);

            /* We don't want the X 32-bit composite visual. */
            if (visinfo->depth != 32) {
                visual->fbconfig = fbconfigs[i];
                visual->visinfo = visinfo;
                break;
            }
            XFree(visinfo);
        }
    }

    assert(visual->fbconfig);
    assert(visual->visinfo);

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height,
               const glws::pbuffer_info *pbInfo)
{
    return new GlxDrawable(visual, width, height, pbInfo);
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
        if (profile.api == glfeatures::API_GL) {
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
        } else if (profile.api == glfeatures::API_GLES) {
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
        if (profile.api != glfeatures::API_GL ||
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
makeCurrentInternal(Drawable *drawable, Drawable *readable, Context *context)
{
    Window draw = None;
    if (drawable) {
        GlxDrawable *glxDrawable = static_cast<GlxDrawable *>(drawable);
        draw = glxDrawable->drawable;
    }

    Window read = None;
    if (readable) {
        GlxDrawable *glxReadable = static_cast<GlxDrawable *>(readable);
        read = glxReadable->drawable;
    }

    GLXContext ctx = nullptr;
    if (context) {
        GlxContext *glxContext = static_cast<GlxContext *>(context);
        ctx = glxContext->context;
    }

    // We assume all GLX implementations support GLX 1.3 and the
    // glXMakeContextCurrent() function.  But only call it when needed.
    if (read != draw)
        return glXMakeContextCurrent(display, draw, read, ctx);
    else
        return glXMakeCurrent(display, draw, ctx);
}

Window
GlxDrawable::createPbuffer(Display *dpy, const GlxVisual *visinfo,
                           const glws::pbuffer_info *pbInfo, int w, int h)
{
    int samples = 0;

    // XXX ideally, we'd populate these attributes according to the Visual info
    Attributes<int> attribs;
    attribs.add(GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT);
    attribs.add(GLX_RENDER_TYPE, GLX_RGBA_BIT);
    attribs.add(GLX_RED_SIZE, 1);
    attribs.add(GLX_GREEN_SIZE, 1);
    attribs.add(GLX_BLUE_SIZE, 1);
    attribs.add(GLX_ALPHA_SIZE, 1);
    //attribs.add(GLX_DOUBLEBUFFER, doubleBuffer ? GL_TRUE : GL_FALSE);
    attribs.add(GLX_DEPTH_SIZE, 1);
    attribs.add(GLX_STENCIL_SIZE, 1);
    if (samples > 1) {
        attribs.add(GLX_SAMPLE_BUFFERS, 1);
        attribs.add(GLX_SAMPLES_ARB, samples);
    }
    attribs.end();

    int num_configs = 0;
    GLXFBConfig *fbconfigs;
    fbconfigs = glXChooseFBConfig(dpy, screen, attribs, &num_configs);
    if (!num_configs || !fbconfigs) {
        std::cerr << "error: glXChooseFBConfig for pbuffer failed.\n";
        exit(1);
    }

    Attributes<int> pbAttribs;
    pbAttribs.add(GLX_PBUFFER_WIDTH, w);
    pbAttribs.add(GLX_PBUFFER_HEIGHT, h);
    pbAttribs.add(GLX_PRESERVED_CONTENTS, True);
    pbAttribs.end();

    GLXDrawable pbuffer = glXCreatePbuffer(dpy, fbconfigs[0], pbAttribs);
    if (!pbuffer) {
        std::cerr << "error: glXCreatePbuffer() failed\n";
        exit(1);
    }

    return pbuffer;
}


// For GLX, we implement wglBindTexARB() as a copy operation.
// We copy the pbuffer image to the currently bound texture.
// If there's any rendering to the pbuffer before the wglReleaseTexImage()
// call, the results are undefined (and permitted by the extension spec).
//
// The spec says that glTexImage and glCopyTexImage calls which effect
// the pbuffer/texture should not be allowed, but we do not enforce that.
//
// The spec says that when a pbuffer is released from the texture that
// the contents do not have to be preserved.  But that's what will happen
// since we're copying here.
bool
bindTexImage(Drawable *pBuffer, int iBuffer) {
    GLint readBufSave;
    GLint width, height;

    assert(pBuffer->pbuffer);

    // Save the current drawing surface and bind the pbuffer surface
    GLXDrawable prevDrawable = glXGetCurrentDrawable();
    GLXContext prevContext = glXGetCurrentContext();
    GlxDrawable *glxPBuffer = static_cast<GlxDrawable *>(pBuffer);
    glXMakeCurrent(display, glxPBuffer->drawable, prevContext);

    glGetIntegerv(GL_READ_BUFFER, &readBufSave);

    assert(iBuffer == GL_FRONT_LEFT ||
           iBuffer == GL_BACK_LEFT ||
           iBuffer == GL_FRONT_RIGHT ||
           iBuffer == GL_BACK_RIGHT ||
           iBuffer == GL_AUX0);

    // set src buffer
    glReadBuffer(iBuffer);

    // Just copy image from pbuffer to texture
    switch (pBuffer->pbInfo.texTarget) {
    case GL_TEXTURE_1D:
        glGetTexLevelParameteriv(GL_TEXTURE_1D, pBuffer->mipmapLevel,
                                 GL_TEXTURE_WIDTH, &width);
        if (width == pBuffer->width) {
            // replace existing texture
            glCopyTexSubImage1D(GL_TEXTURE_1D,
                                pBuffer->mipmapLevel,
                                0,    // xoffset
                                0, 0, // x, y
                                pBuffer->width);
        } else {
            // define initial texture
            glCopyTexImage1D(GL_TEXTURE_1D,
                             pBuffer->mipmapLevel,
                             pBuffer->pbInfo.texFormat,
                             0, 0, // x, y
                             pBuffer->width, 0);
        }
        break;
    case GL_TEXTURE_2D:
        glGetTexLevelParameteriv(GL_TEXTURE_2D, pBuffer->mipmapLevel,
                                 GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, pBuffer->mipmapLevel,
                                 GL_TEXTURE_HEIGHT, &height);
        if (width == pBuffer->width && height == pBuffer->height) {
            // replace existing texture
            glCopyTexSubImage2D(GL_TEXTURE_2D,
                                pBuffer->mipmapLevel,
                                0, 0, // xoffset, yoffset
                                0, 0, // x, y
                                pBuffer->width, pBuffer->height);
        } else {
            // define initial texture
            glCopyTexImage2D(GL_TEXTURE_2D,
                             pBuffer->mipmapLevel,
                             pBuffer->pbInfo.texFormat,
                             0, 0, // x, y
                             pBuffer->width, pBuffer->height, 0);
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        {
            const GLenum target =
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + pBuffer->cubeFace;
            glGetTexLevelParameteriv(target, pBuffer->mipmapLevel,
                                     GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(target, pBuffer->mipmapLevel,
                                     GL_TEXTURE_HEIGHT, &height);
            if (width == pBuffer->width && height == pBuffer->height) {
                // replace existing texture
                glCopyTexSubImage2D(target,
                                    pBuffer->mipmapLevel,
                                    0, 0, // xoffset, yoffset
                                    0, 0, // x, y
                                    pBuffer->width, pBuffer->height);
            } else {
                // define new texture
                glCopyTexImage2D(target,
                                 pBuffer->mipmapLevel,
                                 pBuffer->pbInfo.texFormat,
                                 0, 0, // x, y
                                 pBuffer->width, pBuffer->height, 0);
            }
        }
        break;
    default:
        ; // no op
    }

    // restore
    glReadBuffer(readBufSave);

    // rebind previous drawing surface
    glXMakeCurrent(display, prevDrawable, prevContext);

    return true;
}

bool
releaseTexImage(Drawable *pBuffer, int iBuffer) {
    assert(pBuffer->pbuffer);
    // nothing to do here.
    return true;
}

bool
setPbufferAttrib(Drawable *pBuffer, const int *attribList) {
    assert(pBuffer->pbuffer);
    // Nothing to do here.  retrace_wglSetPbufferAttribARB() will have parsed
    // and saved the mipmap/cubeface info in the Drawable.
    std::cout << "Calling GLX setPbufferAttrib\n";
    return true;
}

} /* namespace glws */

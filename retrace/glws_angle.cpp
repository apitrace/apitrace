#include <iostream>

#include "glproc.hpp"
#include "glws.hpp"
#include "ws_win32.hpp"

#include <EGL/eglext.h>

namespace glws {
    static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    static char const* eglExtensions = NULL;
    static bool has_EGL_KHR_create_context = false;

    static EGLenum translateAPI(glfeatures::Profile profile)
    {
        switch (profile.api) {
        case glfeatures::API_GL:
            return EGL_OPENGL_API;
        case glfeatures::API_GLES:
            return EGL_OPENGL_ES_API;
        default:
            assert(0);
            return EGL_NONE;
        }
    }

    /* Must be called before
     *
     * - eglCreateContext
     * - eglGetCurrentContext
     * - eglGetCurrentDisplay
     * - eglGetCurrentSurface
     * - eglMakeCurrent (when its ctx parameter is EGL_NO_CONTEXT ),
     * - eglWaitClient
     * - eglWaitNative
     */
    static void bindAPI(EGLenum api)
    {
        if (eglBindAPI(api) != EGL_TRUE) {
            std::cerr << "error: eglBindAPI failed\n";
            exit(1);
        }
    }

    class EglVisual : public Visual
    {
    public:
        EGLConfig config;

        EglVisual(Profile prof) :
            Visual(prof),
            config(0)
        {}

        ~EglVisual() = default;
    };

    class EglDrawable : public Drawable
    {
    public:
        EGLSurface surface;
        EGLenum api;
        EGLNativeWindowType window;

        EglDrawable(const Visual* vis, int w, int h,
            const glws::pbuffer_info* pbInfo) :
            Drawable(vis, w, h, pbInfo),
            api(EGL_OPENGL_ES_API)
        {
            window = ws::createWindow("eglretrace", w, h);

            eglWaitNative(EGL_CORE_NATIVE_ENGINE);

            EGLConfig config = static_cast<const EglVisual*>(visual)->config;
            surface = eglCreateWindowSurface(eglDisplay, config, window, NULL);
        }

        ~EglDrawable() {
            eglDestroySurface(eglDisplay, surface);
            eglWaitClient();
            DestroyWindow(window);
            eglWaitNative(EGL_CORE_NATIVE_ENGINE);
        }

        void recreate(void) {
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

            EGLConfig config = static_cast<const EglVisual*>(visual)->config;
            surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
            if (surface == EGL_NO_SURFACE) {
                // XXX: But don't defer destruction if eglCreateWindowSurface fails, which is the case of SwiftShader
                eglDestroySurface(eglDisplay, oldSurface);
                oldSurface = EGL_NO_SURFACE;
                surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
            }
            assert(surface != EGL_NO_SURFACE);

            if (rebindDrawSurface || rebindReadSurface) {
                eglMakeCurrent(eglDisplay, surface, surface, currentContext);
            }

            if (oldSurface != EGL_NO_SURFACE) {
                eglDestroySurface(eglDisplay, oldSurface);
            }
        }

        void resize(int w, int h) override {
            if (w == width && h == height) {
                return;
            }

            eglWaitClient();

            Drawable::resize(w, h);

            ws::resizeWindow(window, w, h);

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

        void show(void) override {
            if (visible) {
                return;
            }

            eglWaitClient();

            ws::showWindow(window);

            eglWaitNative(EGL_CORE_NATIVE_ENGINE);

            Drawable::show();
        }

        void swapBuffers(void) override {
            bindAPI(api);
            eglSwapBuffers(eglDisplay, surface);
        }
    };

    class EglContext : public Context
    {
    public:
        EGLContext context;

        EglContext(const Visual* vis, EGLContext ctx) :
            Context(vis),
            context(ctx)
        {}

        ~EglContext() {
            eglDestroyContext(eglDisplay, context);
        }
    };

    void init(void) {

        eglExtensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        if (eglDisplay == EGL_NO_DISPLAY) {
            std::cerr << "error: unable to get EGL display\n";
            exit(1);
        }

        EGLint major, minor;
        if (!eglInitialize(eglDisplay, &major, &minor)) {
            std::cerr << "error: unable to initialize EGL display\n";
            exit(1);
        }

        eglExtensions = eglQueryString(eglDisplay, EGL_EXTENSIONS);
        has_EGL_KHR_create_context = checkExtension("EGL_KHR_create_context", eglExtensions);
    }

    void cleanup(void) {
        if (eglDisplay != EGL_NO_DISPLAY) {
            eglTerminate(eglDisplay);
        }
    }

    bool processEvents(void) {
        throw std::runtime_error("processEvents not implemented");
    }

    Visual* createVisual(bool doubleBuffer, unsigned int samples, Profile profile) {
        EGLint api_bits;
        if (profile.api == glfeatures::API_GL) {
            api_bits = EGL_OPENGL_BIT;
            if (profile.core && !has_EGL_KHR_create_context) {
                return NULL;
            }
        }
        else if (profile.api == glfeatures::API_GLES) {
            switch (profile.major) {
            case 1:
                api_bits = EGL_OPENGL_ES_BIT;
                break;
            case 3:
                if (has_EGL_KHR_create_context) {
                    api_bits = EGL_OPENGL_ES3_BIT;
                    break;
                }
                /* fall-through */
            case 2:
                api_bits = EGL_OPENGL_ES2_BIT;
                break;
            default:
                return NULL;
            }
        }
        else {
            assert(0);
            return NULL;
        }

        Attributes<EGLint> attribs;
        attribs.add(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);
        attribs.add(EGL_RED_SIZE, 8);
        attribs.add(EGL_GREEN_SIZE, 8);
        attribs.add(EGL_BLUE_SIZE, 8);
        attribs.add(EGL_ALPHA_SIZE, 8);
        attribs.add(EGL_DEPTH_SIZE, 24);
        attribs.add(EGL_STENCIL_SIZE, 8);
        attribs.add(EGL_RENDERABLE_TYPE, api_bits);
        attribs.end(EGL_NONE);

        EGLint num_configs = 0;
        if (!eglGetConfigs(eglDisplay, NULL, 0, &num_configs) ||
            num_configs <= 0) {
            return NULL;
        }

        std::vector<EGLConfig> configs(num_configs);
        if (!eglChooseConfig(eglDisplay, attribs, &configs[0], num_configs, &num_configs) ||
            num_configs <= 0) {
            return NULL;
        }

        // We can't tell what other APIs the trace will use afterwards, therefore
        // try to pick a config which supports the widest set of APIs.
        int bestScore = -1;
        EGLConfig config = configs[0];
        for (EGLint i = 0; i < num_configs; ++i) {
            EGLint renderable_type = EGL_NONE;
            eglGetConfigAttrib(eglDisplay, configs[i], EGL_RENDERABLE_TYPE, &renderable_type);
            int score = 0;
            assert(renderable_type & api_bits);
            renderable_type &= ~api_bits;
            if (renderable_type & EGL_OPENGL_ES2_BIT) {
                score += 1 << 4;
            }
            if (renderable_type & EGL_OPENGL_ES3_BIT) {
                score += 1 << 3;
            }
            if (renderable_type & EGL_OPENGL_ES_BIT) {
                score += 1 << 2;
            }
            if (renderable_type & EGL_OPENGL_BIT) {
                score += 1 << 1;
            }
            if (score > bestScore) {
                config = configs[i];
                bestScore = score;
            }
        }
        assert(bestScore >= 0);

        EGLint visual_id;
        if (!eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &visual_id)) {
            assert(0);
            return NULL;
        }

        EglVisual* visual = new EglVisual(profile);
        visual->config = config;

        return visual;
    }

    Drawable* createDrawable(const Visual* visual, int width, int height, const glws::pbuffer_info* pbInfo) {
        return new EglDrawable(visual, width, height, pbInfo);
    }

    Context* createContext(const Visual* _visual, Context* shareContext, bool debug) {
        Profile profile = _visual->profile;
        const EglVisual* visual = static_cast<const EglVisual*>(_visual);
        EGLContext share_context = EGL_NO_CONTEXT;
        EGLContext context;
        Attributes<EGLint> attribs;

        if (shareContext) {
            share_context = static_cast<EglContext*>(shareContext)->context;
        }

        int contextFlags = 0;
        if (profile.api == glfeatures::API_GL) {
            if (has_EGL_KHR_create_context) {
                attribs.add(EGL_CONTEXT_MAJOR_VERSION_KHR, profile.major);
                attribs.add(EGL_CONTEXT_MINOR_VERSION_KHR, profile.minor);
                int profileMask = profile.core ? EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR : EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;
                attribs.add(EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, profileMask);
                if (profile.forwardCompatible) {
                    contextFlags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
                }
            }
            else if (profile.versionGreaterOrEqual(3, 2)) {
                std::cerr << "error: EGL_KHR_create_context not supported\n";
                return NULL;
            }
        }
        else if (profile.api == glfeatures::API_GLES) {
            if (has_EGL_KHR_create_context) {
                attribs.add(EGL_CONTEXT_MAJOR_VERSION_KHR, profile.major);
                attribs.add(EGL_CONTEXT_MINOR_VERSION_KHR, profile.minor);
            }
            else {
                attribs.add(EGL_CONTEXT_CLIENT_VERSION, profile.major);
            }
        }
        else {
            assert(0);
            return NULL;
        }

        if (debug) {
            contextFlags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
        }
        if (contextFlags && has_EGL_KHR_create_context) {
            attribs.add(EGL_CONTEXT_FLAGS_KHR, contextFlags);
        }
        attribs.end(EGL_NONE);

        EGLenum api = translateAPI(profile);
        bindAPI(api);

        context = eglCreateContext(eglDisplay, visual->config, share_context, attribs);
        if (!context) {
            if (debug) {
                // XXX: Mesa has problems with EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR
                // with OpenGL ES contexts, so retry without it
                return createContext(_visual, shareContext, false);
            }
            return NULL;
        }

        return new EglContext(visual, context);
    }

    bool makeCurrentInternal(Drawable* drawable, Drawable* readable, Context* context) {
        if (!drawable || !context) {
            return eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        } else {
            EglDrawable* eglDrawable = static_cast<EglDrawable*>(drawable);
            EglDrawable* eglReadable = static_cast<EglDrawable*>(readable);
            EglContext* eglContext = static_cast<EglContext*>(context);
            EGLBoolean ok;

            EGLenum api = translateAPI(eglContext->profile);
            bindAPI(api);

            ok = eglMakeCurrent(eglDisplay, eglDrawable->surface,
                eglReadable->surface, eglContext->context);

            if (ok) {
                eglDrawable->api = api;
                eglReadable->api = api;
            }

            return ok;
        }
    }

    bool bindTexImage(Drawable* pBuffer, int iBuffer) {
        throw std::runtime_error("bindTexImage not implemented");
    }

    bool releaseTexImage(Drawable* pBuffer, int iBuffer) {
        throw std::runtime_error("releaseTexImage not implemented");
    }

    bool setPbufferAttrib(Drawable* pBuffer, const int* attribList) {
        throw std::runtime_error("setPbufferAttrib not implemented");
    }
}
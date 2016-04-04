/**************************************************************************
 *
 * Copyright 2011 LunarG, Inc.
 * Copyright 2011 Jose Fonseca
 * Copyright 2015 BogDan Vatra
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

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include <regex>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

#include <android/native_window_jni.h>
#include <android/log.h>

#include "glproc.hpp"
#include "glws.hpp"


extern "C" int main(int, char **);

namespace glws {
static char TAG[]="apitrace";

static JavaVM *s_javaVM = nullptr;
static jobject s_activityObject = nullptr;
static std::mutex s_activityObjectMutex;

static jmethodID s_setSurfaceSizeMethodID = nullptr;

static jfieldID s_descriptor = nullptr;

typedef std::shared_ptr<ANativeWindow> AndroidWindow;
static AndroidWindow s_currentNativeWindow;
static std::atomic_int s_currentNativeWindowId(0);
static std::mutex s_currentNativeWindowMutex;

static int s_stdout_fd = -1;
static int s_stderr_fd = -1;
static std::mutex s_stateMutex;
static std::condition_variable s_stateWait;

static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static char const *eglExtensions = NULL;
static bool has_EGL_KHR_create_context = false;
struct ResourceTracker;

static std::mutex s_resourcesMutex;
static std::vector<ResourceTracker*> s_resources;

class FdWriter : public std::streambuf
{
public:
    FdWriter(int fd)
    {
        m_fd = fd;
        int opts = fcntl(m_fd, F_GETFL);
        opts = opts & (~O_NONBLOCK);
        fcntl(m_fd, F_SETFL, opts);
    }

    // basic_streambuf interface
protected:
    std::streamsize xsputn(const char_type *__s, std::streamsize __n)
    {
        std::streamsize ret = 0;
        while( __n ) {
            ssize_t written = write(m_fd, __s, __n);
            if (written > 0) {
                __n -= written;
                __s += written;
                ret += written;
            } else {
                switch (errno) {
                case EBADF:
                case EINVAL:
                case EPIPE:
                    std::exit(1);
                    break;
                }
            }
        }
        return ret;
    }

    int_type overflow(int_type __c)
    {
        return xsputn(reinterpret_cast<const char_type *>(&__c), 1) == 1 ? __c : traits_type::eof();
    }

private:
    int m_fd = -1;
};

static EGLenum
translateAPI(glfeatures::Profile profile)
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
static void
bindAPI(EGLenum api)
{
    if (eglBindAPI(api) != EGL_TRUE) {
        std::cerr << "error: eglBindAPI failed\n";
        exit(1);
    }
}

struct EglVisual : public Visual
{
    EglVisual(Profile prof) : Visual(prof) {}
    EGLConfig config = 0;
    EGLint format = -1;
};

struct ResourceTracker
{
    ResourceTracker()
    {
        s_resourcesMutex.lock();
        s_resources.push_back(this);
        s_resourcesMutex.unlock();
    }

    virtual ~ResourceTracker()
    {
        s_resourcesMutex.lock();
        s_resources.erase(std::find(s_resources.begin(), s_resources.end(), this));
        s_resourcesMutex.unlock();
    }
};

class EglDrawable : public Drawable, private ResourceTracker
{
public:
    EglDrawable(const Visual *vis, int w, int h, const pbuffer_info *info) :
        Drawable(vis, w, h, info),
        api(EGL_OPENGL_ES_API),
        windowId(0)
    {
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
        const EglVisual * eglVisual = static_cast<const EglVisual *>(visual);
        update();
        ANativeWindow_setBuffersGeometry(window.get(), 0, 0, eglVisual->format);
        surface = eglCreateWindowSurface(eglDisplay, eglVisual->config, window.get(), NULL);
    }

    ~EglDrawable()
    {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(eglDisplay, surface);
        eglWaitClient();
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    }

    void recreate(void)
    {
        EGLContext currentContext = eglGetCurrentContext();
        EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
        EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
        bool rebindDrawSurface = currentDrawSurface == surface;
        bool rebindReadSurface = currentReadSurface == surface;

        if (rebindDrawSurface || rebindReadSurface) {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroySurface(eglDisplay, surface);
        }

        const EglVisual * eglVisual = static_cast<const EglVisual *>(visual);
        ANativeWindow_setBuffersGeometry(window.get(), 0, 0, eglVisual->format);
        surface = eglCreateWindowSurface(eglDisplay, eglVisual->config, (EGLNativeWindowType)window.get(), NULL);

        if (rebindDrawSurface || rebindReadSurface)
            eglMakeCurrent(eglDisplay, surface, surface, currentContext);
    }

    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }
        eglWaitClient();
        Drawable::resize(w, h);
        resizeSurface(w, h);
    }

    void show(void) {
        if (visible) {
            return;
        }
        eglWaitClient();
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
        Drawable::show();
    }

    void swapBuffers(void) {
        if (update())
            recreate();
        bindAPI(api);
        eglSwapBuffers(eglDisplay, surface);
    }

private:
    inline bool update()
    {
        if (windowId.load() == s_currentNativeWindowId.load())
            return false;

        s_currentNativeWindowMutex.lock();
        window = s_currentNativeWindow;
        windowId.store(s_currentNativeWindowId.load());
        s_currentNativeWindowMutex.unlock();
        return true;
    }

    void resizeSurface(int w, int h)
    {
        JNIEnv *env = nullptr;
        if (JNI_OK != s_javaVM->AttachCurrentThread(&env, nullptr)) {
            __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed");
            std::exit(1);
        }
        s_activityObjectMutex.lock();
        env->CallVoidMethod(s_activityObject, s_setSurfaceSizeMethodID, w, h);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        s_activityObjectMutex.unlock();
        s_javaVM->DetachCurrentThread();
    }

public:
    EGLSurface surface;
    EGLenum api;

private:
    AndroidWindow window;
    std::atomic_int windowId;
};


class EglContext : public Context, private ResourceTracker
{
public:
    EGLContext context;

    EglContext(const Visual *vis, EGLContext ctx) :
        Context(vis),
        context(ctx)
    {}

    ~EglContext() {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(eglDisplay, context);
    }
};

void
init(void) {
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

void
cleanup(void) {
    while (!s_resources.empty()) {
        delete *s_resources.rbegin();
    }

    if (eglDisplay != EGL_NO_DISPLAY) {
        eglTerminate(eglDisplay);
    }
    eglDisplay = EGL_NO_DISPLAY;
    eglExtensions = nullptr;
    has_EGL_KHR_create_context = false;
    s_stateMutex.lock();
    close(s_stdout_fd);
    close(s_stderr_fd);
    s_stdout_fd = -1;
    s_stderr_fd = -1;
    s_stateMutex.unlock();
}

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {
    EGLint api_bits;
    if (profile.api == glfeatures::API_GL) {
        api_bits = EGL_OPENGL_BIT;
        if (profile.core && !has_EGL_KHR_create_context) {
            return NULL;
        }
    } else if (profile.api == glfeatures::API_GLES) {
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
    } else {
        assert(0);
        return NULL;
    }

    Attributes<EGLint> attribs;
    attribs.add(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);
    attribs.add(EGL_RED_SIZE, 1);
    attribs.add(EGL_GREEN_SIZE, 1);
    attribs.add(EGL_BLUE_SIZE, 1);
    attribs.add(EGL_ALPHA_SIZE, 1);
    attribs.add(EGL_DEPTH_SIZE, 1);
    attribs.add(EGL_STENCIL_SIZE, 1);
    attribs.add(EGL_RENDERABLE_TYPE, api_bits);
    attribs.end(EGL_NONE);

    EGLint num_configs = 0;
    if (!eglGetConfigs(eglDisplay, NULL, 0, &num_configs) ||
        num_configs <= 0) {
        return NULL;
    }

    std::vector<EGLConfig> configs(num_configs);
    if (!eglChooseConfig(eglDisplay, attribs, &configs[0], num_configs,  &num_configs) ||
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

    EGLint visual_id = -1;
    if (!eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &visual_id)) {
        assert(0);
        return NULL;
    }
    assert(visual_id != -1);

    EglVisual *visual = new EglVisual(profile);
    visual->config = config;
    visual->format = visual_id;

    return visual;
}

Drawable *
createDrawable(const Visual *visual, int width, int height,
               const pbuffer_info *info)
{
    return new EglDrawable(visual, width, height, info);
}


Context *
createContext(const Visual *_visual, Context *shareContext, bool debug)
{
    Profile profile = _visual->profile;
    const EglVisual *visual = static_cast<const EglVisual *>(_visual);
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
        } else if (profile.versionGreaterOrEqual(3, 2)) {
            std::cerr << "error: EGL_KHR_create_context not supported\n";
            return NULL;
        }
    } else if (profile.api == glfeatures::API_GLES) {
        if (has_EGL_KHR_create_context) {
            attribs.add(EGL_CONTEXT_MAJOR_VERSION_KHR, profile.major);
            attribs.add(EGL_CONTEXT_MINOR_VERSION_KHR, profile.minor);
        } else {
            attribs.add(EGL_CONTEXT_CLIENT_VERSION, profile.major);
        }
    } else {
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

bool
makeCurrentInternal(Drawable *drawable, Context *context)
{
    if (!drawable || !context) {
        return eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    } else {
        EglDrawable *eglDrawable = static_cast<EglDrawable *>(drawable);
        EglContext *eglContext = static_cast<EglContext *>(context);
        EGLBoolean ok;

        EGLenum api = translateAPI(eglContext->profile);
        bindAPI(api);

        ok = eglMakeCurrent(eglDisplay, eglDrawable->surface,
                            eglDrawable->surface, eglContext->context);

        if (ok) {
            eglDrawable->api = api;
        }

        return ok;
    }
}

bool processEvents(void)
{
    return false;
}

bool
bindTexImage(Drawable *pBuffer, int iBuffer) {
    std::cerr << "error: EGL/Android::wglBindTexImageARB not implemented.\n";
    assert(pBuffer->pbuffer);
    return true;
}

bool
releaseTexImage(Drawable *pBuffer, int iBuffer) {
    std::cerr << "error: EGL/Android::wglReleaseTexImageARB not implemented.\n";
    assert(pBuffer->pbuffer);
    return true;
}

bool
setPbufferAttrib(Drawable *pBuffer, const int *attribList) {
    // nothing to do here.
    assert(pBuffer->pbuffer);
    return true;
}

static void readParamsAndStartTrace()
{
    std::string paramsLine;
    char buff[4096];
    while (true) {
        ssize_t sz = read(s_stdout_fd, buff, 4095);
        if (sz < 0) {
            cleanup();
            return ;
        }
        buff[sz] = '\0';
        paramsLine += buff;
        if (paramsLine.find('\n') > 0)
            break;
    }
    std::regex word_regex("(\"[^\"]*\")|([^\\s]+)");
    auto words_begin =
        std::sregex_iterator(paramsLine.begin(), paramsLine.end(), word_regex);
    auto words_end = std::sregex_iterator();
    std::vector<std::string> paramsVector;
    std::vector<char *> params;
    params.push_back(TAG);
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        paramsVector.push_back(i->str());
        params.push_back(const_cast<char*>(paramsVector.back().c_str()));
    }

    {
        std::unique_lock<std::mutex> lock(s_stateMutex);
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Wait for stderr socket");
        s_stateWait.wait(lock, []{return s_stderr_fd != -1; });
    }
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Start retrace");
    optind = 1;
    optreset = 1;
    main(params.size(), &params[0]);
    cleanup();

    // Remove "std::exit" after apitrace will cleanup its static variables.
    std::exit(0); // goodbye cruel world !
}

static inline int getFd(JNIEnv* env, jobject obj)
{
    return env->GetIntField(obj, s_descriptor);
}

static bool setFd(std::ostream &stream, int *state_fd, int fd)
{
    std::lock_guard<std::mutex> lock(s_stateMutex);
    if (*state_fd != -1)
        return false;
    *state_fd = fd;
    delete dynamic_cast<FdWriter*>(stream.rdbuf(new FdWriter(fd)));
    return true;
}

static jboolean setStdoutFileDescriptor(JNIEnv *env, jobject /*object*/, jobject fd)
{
    bool res = setFd(std::cout, &s_stdout_fd, getFd(env, fd));
    if (res)
        std::async(std::launch::async, glws::readParamsAndStartTrace);
    return res;
}

static jboolean setStderrFileDescriptor(JNIEnv *env, jobject /*object*/, jobject fd)
{
    bool res = setFd(std::cerr, &s_stderr_fd, getFd(env, fd));
    if (res)
        s_stateWait.notify_one();
    return res;
}

static void onActivityCreated(JNIEnv *env, jobject /*object*/, jobject activity)
{
    s_activityObjectMutex.lock();
    s_activityObject = env->NewGlobalRef(activity);
    s_activityObjectMutex.unlock();
}

static void onSurfaceCreated(JNIEnv *env, jobject /*object*/, jobject surface)
{
    s_currentNativeWindowMutex.lock();
    s_currentNativeWindow = AndroidWindow(ANativeWindow_fromSurface(env, surface), [](ANativeWindow *w) { ANativeWindow_release(w); });
    ++s_currentNativeWindowId;
    s_currentNativeWindowMutex.unlock();
}

static void onSurfaceChanged(JNIEnv *env, jobject /*object*/, jobject surface, int /*format*/, int /*width*/, int /*height*/)
{
    s_currentNativeWindowMutex.lock();
    s_currentNativeWindow = AndroidWindow(ANativeWindow_fromSurface(env, surface), [](ANativeWindow *w) { ANativeWindow_release(w); });
    ++s_currentNativeWindowId;
    s_currentNativeWindowMutex.unlock();
}

static void onSurfaceRedrawNeeded(JNIEnv */*env*/, jobject /*object*/, jobject /*surface*/)
{
}

static void onSurfaceDestroyed(JNIEnv */*env*/, jobject /*object*/)
{
    s_currentNativeWindowMutex.lock();
    s_currentNativeWindow.reset();
    s_currentNativeWindowId.store(0);
    s_currentNativeWindowMutex.unlock();
}

static void onActivityDestroyed(JNIEnv *env, jobject /*object*/)
{
    s_activityObjectMutex.lock();
    env->DeleteGlobalRef(s_activityObject);
    s_activityObject = nullptr;
    s_activityObjectMutex.unlock();
}

static JNINativeMethod methods[] = {
    {"setStdoutFileDescriptor", "(Ljava/io/FileDescriptor;)Z", (void *)setStdoutFileDescriptor},
    {"setStderrFileDescriptor", "(Ljava/io/FileDescriptor;)Z", (void *)setStderrFileDescriptor},
    {"onActivityCreatedNative", "(Lapitrace/github/io/eglretrace/RetraceActivity;)V", (void *)onActivityCreated},
    {"onSurfaceCreatedNative", "(Landroid/view/Surface;)V", (void *)onSurfaceCreated},
    {"onSurfaceChangedNative", "(Landroid/view/Surface;III)V", (void *)onSurfaceChanged},
    {"onSurfaceRedrawNeededNative", "(Landroid/view/Surface;)V", (void *)onSurfaceRedrawNeeded},
    {"onSurfaceDestroyedNative", "()V", (void *)onSurfaceDestroyed},
    {"onActivityDestroyedNative", "()V", (void *)onActivityDestroyed}
};

} /* namespace glws */


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void */*reserved*/)
{
    typedef union {
        JNIEnv *nativeEnvironment;
        void *venv;
    } UnionJNIEnvToVoid;

    __android_log_print(ANDROID_LOG_INFO, glws::TAG, "apitrace start");
    UnionJNIEnvToVoid uenv;
    uenv.venv = nullptr;
    glws::s_javaVM = nullptr;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        __android_log_print(ANDROID_LOG_FATAL, glws::TAG, "GetEnv failed");
        return -1;
    }

    JNIEnv *env = uenv.nativeEnvironment;

    jclass clazz;
    clazz = env->FindClass("apitrace/github/io/eglretrace/RetraceActivity");
    if (!clazz) {
        __android_log_print(ANDROID_LOG_FATAL, glws::TAG, "Can't find RetraceActivity");
        return JNI_FALSE;
    }

    glws::s_setSurfaceSizeMethodID = env->GetMethodID(clazz, "setSurfaceSize", "(II)V");
    if (!glws::s_setSurfaceSizeMethodID) {
        __android_log_print(ANDROID_LOG_FATAL, glws::TAG, "Can't find setSurfaceSize method");
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, glws::methods, sizeof(glws::methods) / sizeof(glws::methods[0])) < 0) {
        __android_log_print(ANDROID_LOG_FATAL, glws::TAG, "RegisterNatives failed");
        return JNI_FALSE;
    }

    clazz = env->FindClass("java/io/FileDescriptor");
    if (!clazz) {
        __android_log_print(ANDROID_LOG_FATAL, glws::TAG, "Can't find FileDescriptor class");
        return JNI_FALSE;
    }
    glws::s_descriptor = env->GetFieldID(clazz, "descriptor", "I");
    if (!glws::s_descriptor) {
        __android_log_print(ANDROID_LOG_FATAL, glws::TAG, "Can't find descriptor field");
        return JNI_FALSE;
    }

    glws::s_javaVM = vm;
    return JNI_VERSION_1_4;
}

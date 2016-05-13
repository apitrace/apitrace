/*********************************************************************
 *
 * Copyright 2012 Intel Corporation
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

#include <assert.h>

#include <map>
#include <memory>

#include <os_thread.hpp>
#include <glproc.hpp>
#include <gltrace.hpp>

namespace gltrace {

typedef std::shared_ptr<Context> context_ptr_t;
static std::map<uintptr_t, context_ptr_t> context_map;
static os::recursive_mutex context_map_mutex;

class ThreadState {
public:
    context_ptr_t current_context;
    context_ptr_t dummy_context;     /*
                                      * For cases when there is no current
                                      * context, but the app still calls some
                                      * GL function that expects one.
                                      */
    ThreadState() : dummy_context(new Context)
    {
        current_context = dummy_context;
    }
};

static OS_THREAD_SPECIFIC_PTR(ThreadState) thread_state;

static ThreadState *get_ts(void)
{
    ThreadState *ts = thread_state;
    if (!ts) {
        thread_state = ts = new ThreadState;
    }

    return ts;
}

static void _retainContext(context_ptr_t ctx)
{
    ctx->retain_count++;
}

void retainContext(uintptr_t context_id)
{
    context_map_mutex.lock();
    if (context_map.find(context_id) != context_map.end())
        _retainContext(context_map[context_id]);
    context_map_mutex.unlock();
}

static bool _releaseContext(context_ptr_t ctx)
{
    return !(--ctx->retain_count);
}

/*
 * return true if the context was destroyed, false if only its refcount
 * got decreased. Note that even if the context was destroyed it may
 * still live, if it's the currently selected context (by setContext).
 */
bool releaseContext(uintptr_t context_id)
{
    bool res = false;

    context_map_mutex.lock();
    /*
     * This can potentially called (from glX) with an invalid context_id,
     * so don't assert on it being valid.
     */
    if (context_map.find(context_id) != context_map.end()) {
        res = _releaseContext(context_map[context_id]);
        if (res)
            context_map.erase(context_id);
    }
    context_map_mutex.unlock();

    return res;
}

void createContext(uintptr_t context_id)
{
    // wglCreateContextAttribsARB causes internal calls to wglCreateContext to be
    // traced, causing context to be defined twice.
    if (context_map.find(context_id) != context_map.end()) {
        return;
    }

    context_ptr_t ctx(new Context);

    context_map_mutex.lock();

    _retainContext(ctx);
    context_map[context_id] = ctx;

    context_map_mutex.unlock();
}

void setContext(uintptr_t context_id)
{
    ThreadState *ts = get_ts();
    context_ptr_t ctx;

    context_map_mutex.lock();

    assert(context_map.find(context_id) != context_map.end());
    ctx = context_map[context_id];

    context_map_mutex.unlock();

    ts->current_context = ctx;

    if (!ctx->bound) {
        ctx->profile = glfeatures::getCurrentContextProfile();
        ctx->extensions.getCurrentContextExtensions(ctx->profile);
        ctx->features.load(ctx->profile, ctx->extensions);
        ctx->bound = true;
    }

    if (!ctx->boundDrawable) {
        /*
         * The default viewport and scissor state is set when a context is
         * first made current, with values matching the bound drawable.  Many
         * applications never thouch the default state ever again.
         *
         * Since we currently don't trace window sizes, and rely on viewport
         * calls to deduct, emit fake calls here so that viewport/scissor state
         * can be deducated.
         *
         * FIXME: don't call the real functions here -- just emit the fake
         * calls.
         */
        GLint viewport[4] = {0, 0, 0, 0};
        GLint scissor[4] = {0, 0, 0, 0};
        _glGetIntegerv(GL_VIEWPORT, viewport);
        _glGetIntegerv(GL_SCISSOR_BOX, scissor);

        /*
         * On MacOSX the current context and surface are set independently, and
         * we might be called before both are set, so ignore empty boxes.
         */
        if (viewport[2] && viewport[3] && scissor[2] && scissor[3]) {
            _fake_glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            _fake_glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
            ctx->boundDrawable = true;
        }
    }
}

void clearContext(void)
{
    ThreadState *ts = get_ts();

    ts->current_context = ts->dummy_context;
}

Context *getContext(void)
{
    return get_ts()->current_context.get();
}

}

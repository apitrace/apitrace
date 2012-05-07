#include <gltrace.hpp>
#include <os_thread.hpp>
#include <assert.h>
#include <tr1/memory>

namespace gltrace {

typedef std::tr1::shared_ptr<Context> context_ptr_t;
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

static os::thread_specific_ptr<struct ThreadState> thread_state;

static ThreadState *get_ts(void)
{
    ThreadState *ts = thread_state.get();

    if (!ts) {
        ts = new ThreadState;
        thread_state.reset(ts);
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
    bool res;

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
    context_ptr_t ctx(new Context);

    context_map_mutex.lock();

    _retainContext(ctx);
    assert(context_map.find(context_id) == context_map.end());
    context_map[context_id] = ctx;

    context_map_mutex.unlock();
}

/*
 * return true if the context has been destroyed, false otherwise. See
 * the note at releaseContext about the actual ccontext lifetime.
 */
bool destroyContext(uintptr_t context_id)
{
    return releaseContext(context_id);
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

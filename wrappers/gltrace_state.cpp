#include <gltrace.hpp>

gltrace::Context *
gltrace::getContext(void)
{
    // TODO return the context set by other APIs (GLX, EGL, and etc.)
    static gltrace::Context _ctx = { gltrace::PROFILE_COMPAT, false, false, false };
    return &_ctx;
}


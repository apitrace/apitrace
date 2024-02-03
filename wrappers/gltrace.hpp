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

#pragma once


#include "glimports.hpp"

#include "glfeatures.hpp"

#include "glmemshadow.hpp"

#include <map>
#include <vector>
#include <memory>

void APIENTRY _fake_glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY _fake_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

namespace gltrace {

class ShareableContextResources {
public:
    std::map<GLint, std::unique_ptr<GLMemoryShadow>> bufferToShadowMemory;

    std::vector<GLMemoryShadow*> dirtyShadows;
};

class Context {
public:
    glfeatures::Profile profile;
    glfeatures::Extensions extensions;
    glfeatures::Features features;

    bool user_arrays = false;
    bool userArraysOnBegin = false;
    unsigned retain_count = 0;

    // Whether it has been bound before
    bool bound = false;
    // Whether it has been bound to a drawable
    bool boundDrawable = false;

    // whether glLockArraysEXT() has ever been called
    GLuint lockedArrayCount = 0;

    // the data which can be shared between shared contexts
    std::shared_ptr<ShareableContextResources> sharedRes;

    Context(void) :
        profile(glfeatures::API_GL, 1, 0),
        sharedRes(std::make_shared<ShareableContextResources>())
    { }
};

void
createContext(uintptr_t context_id, uintptr_t shared_context_id);

void
retainContext(uintptr_t context_id);

bool
releaseContext(uintptr_t context_id);

void
setContext(uintptr_t context_id);

void
clearContext(void);

gltrace::Context *
getContext(void);

const GLubyte *
_glGetString_override(GLenum name);

void
_glGetIntegerv_override(GLenum pname, GLint *params);

const GLubyte *
_glGetStringi_override(GLenum name, GLuint index);


static inline bool
is_coherent_write_map(GLbitfield access)
{
    if ((access & GL_MAP_WRITE_BIT) == 0) {
        return false;
    }

    if (access & GL_MAP_COHERENT_BIT) {
        return true;
    }

    if (access & GL_MAP_FLUSH_EXPLICIT_BIT) {
        return false;
    }

    // Uncomment to workaround https://github.com/apitrace/apitrace/issues/900
    //
    // This is not enabled by default to avoid wasteful creation of shadow
    // memory on glBufferStorage, as GL_MAP_FLUSH_EXPLICIT_BIT is not allowed
    // there, therefore there's no way to now whether a persistent buffer is
    // meant to be used with explicit flushes or not
#if 0
    if (access & GL_MAP_PERSISTENT_BIT) {
        return true;
    }
#endif

    return false;
}


} /* namespace gltrace */

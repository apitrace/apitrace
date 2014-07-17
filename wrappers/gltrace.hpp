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

#ifndef _GLTRACE_HPP_
#define _GLTRACE_HPP_


#include <string.h>
#include <stdlib.h>
#include <map>

#include "glimports.hpp"


namespace gltrace {


enum Profile {
    PROFILE_COMPAT,
    PROFILE_ES1,
    PROFILE_ES2,
};


/**
 * OpenGL ES buffers cannot be read. This class is used to track index buffer
 * contents.
 */
class Buffer {
public:
    GLsizeiptr size;
    GLvoid *data;

    Buffer() :
        size(0),
        data(0)
    {}

    ~Buffer() {
        free(data);
    }

    void
    bufferData(GLsizeiptr new_size, const void *new_data) {
        if (new_size < 0) {
            new_size = 0;
        }
        size = new_size;
        data = realloc(data, new_size);
        if (new_size && new_data) {
            memcpy(data, new_data, size);
        }
    }

    void
    bufferSubData(GLsizeiptr offset, GLsizeiptr length, const void *new_data) {
        if (offset >= 0 && offset < size && length > 0 && offset + length <= size && new_data) {
            memcpy((GLubyte *)data + offset, new_data, length);
        }
    }

    void
    getSubData(GLsizeiptr offset, GLsizeiptr length, void *out_data) {
        if (offset >= 0 && offset < size && length > 0 && offset + length <= size && out_data) {
            memcpy(out_data, (GLubyte *)data + offset, length);
        }
    }
};

class Context {
public:
    enum Profile profile;
    bool user_arrays;
    bool user_arrays_nv;
    unsigned retain_count;

    // Whether it has been bound before
    bool bound;

    // TODO: This will fail for buffers shared by multiple contexts.
    std::map <GLuint, Buffer> buffers;

    Context(void) :
        profile(PROFILE_COMPAT),
        user_arrays(false),
        user_arrays_nv(false),
        retain_count(0),
        bound(false)
    { }

    inline bool
    needsShadowBuffers(void)
    {
        return profile == PROFILE_ES1 || profile == PROFILE_ES2;
    }
};

void
createContext(uintptr_t context_id);

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


} /* namespace gltrace */


#endif /* _GLRETRACE_HPP_ */

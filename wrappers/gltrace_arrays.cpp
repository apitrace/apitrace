/**************************************************************************
 *
 * Copyright 2010-2016 VMware, Inc.
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


#include "gltrace_arrays.hpp"
#include "gltrace.hpp"


/* FIXME take in consideration instancing */


GLuint
_glDraw_count(gltrace::Context *ctx, const DrawArraysParams &params)
{
    if (!params.count) {
        return 0;
    }
    return params.first + params.count;
}


GLuint
_glDraw_count(gltrace::Context *ctx, const DrawElementsParams &params)
{
    if (params.end < params.start ||
        params.count <= 0) {
        return 0;
    }

    if (params.end != ~0U) {
        return params.end + params.basevertex + 1;
    }

    GLuint count = params.count;
    GLenum type = params.type;
    const void *indices = params.indices;

    GLvoid *temp = 0;

    if (!count) {
        return 0;
    }

    GLint element_array_buffer = _element_array_buffer_binding();
    if (element_array_buffer) {
        // Read indices from index buffer object
        if (ctx->profile.es()) {
            // We could try to implement this on top of GL_OES_mapbuffer but should seldom be needed
            os::log("apitrace: warning: %s: element array buffer with memory vertex arrays no longer supported on ES\n", __FUNCTION__);
            return 0;
        }

        GLintptr offset = (GLintptr)indices;
        GLsizeiptr size = count*_gl_type_size(type);
        temp = malloc(size);
        if (!temp) {
            return 0;
        }
        memset(temp, 0, size);
        _glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, temp);
        indices = temp;
    } else {
        if (!indices) {
            return 0;
        }
    }

    GLuint maxindex = 0;

    GLboolean restart_enabled = GL_FALSE;
    GLuint restart_index = 0;
    if (ctx->features.primitive_restart) {
        _glIsEnabled(GL_PRIMITIVE_RESTART);
        if (restart_enabled) {
            restart_index = (GLuint)_glGetInteger(GL_PRIMITIVE_RESTART_INDEX);
        }
    }

    GLsizei i;
    if (type == GL_UNSIGNED_BYTE) {
        const GLubyte *p = (const GLubyte *)indices;
        for (i = 0; i < count; ++i) {
            GLuint index = p[i];
            if (restart_enabled && index == restart_index) {
                continue;
            }
            if (index > maxindex) {
                maxindex = index;
            }
        }
    } else if (type == GL_UNSIGNED_SHORT) {
        const GLushort *p = (const GLushort *)indices;
        for (i = 0; i < count; ++i) {
            GLuint index = p[i];
            if (restart_enabled && index == restart_index) {
                continue;
            }
            if (index > maxindex) {
                maxindex = index;
            }
        }
    } else if (type == GL_UNSIGNED_INT) {
        const GLuint *p = (const GLuint *)indices;
        for (i = 0; i < count; ++i) {
            GLuint index = p[i];
            if (restart_enabled && index == restart_index) {
                continue;
            }
            if (index > maxindex) {
                maxindex = index;
            }
        }
    } else {
        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, type);
    }

    if (element_array_buffer) {
        free(temp);
    }

    maxindex += params.basevertex;

    return maxindex + 1;
}


GLuint
_glDraw_count(gltrace::Context *ctx, const MultiDrawArraysParams &params)
{
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < params.drawcount; ++draw) {
        DrawArraysParams params_draw;
        if (params.first) {
            params_draw.first = params.first[draw];
        }
        if (params.count) {
            params_draw.count = params.count[draw];
        }
        params_draw.instancecount = params.instancecount;
        params_draw.baseinstance = params.baseinstance;
        GLuint _count_draw = _glDraw_count(ctx, params_draw);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}


GLuint
_glDraw_count(gltrace::Context *ctx, const MultiDrawElementsParams &params)
{
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < params.drawcount; ++draw) {
        DrawElementsParams params_draw;
        if (params.count) {
            params_draw.count = params.count[draw];
        }
        params_draw.type = params.type;
        if (params.indices) {
            params_draw.indices = params.indices[draw];
        }
        if (params.basevertex) {
            params_draw.basevertex = params.basevertex[draw];
        }
        params_draw.instancecount = params.instancecount;
        params_draw.baseinstance = params.baseinstance;
        GLuint _count_draw = _glDraw_count(ctx, params_draw);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}


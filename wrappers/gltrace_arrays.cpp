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


GLuint
_glDrawArrays_count(GLint first, GLsizei count)
{
    if (!count) {
        return 0;
    }
    return first + count;
}


GLuint
_glDrawElementsBaseVertex_count(GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
{
    GLvoid *temp = 0;

    if (!count) {
        return 0;
    }

    GLint element_array_buffer = _element_array_buffer_binding();
    if (element_array_buffer) {
        // Read indices from index buffer object
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

    GLboolean restart_enabled = _glIsEnabled(GL_PRIMITIVE_RESTART);
    while ((_glGetError() == GL_INVALID_ENUM))
        ;

    GLuint restart_index = 0;
    if (restart_enabled) {
        restart_index = (GLuint)_glGetInteger(GL_PRIMITIVE_RESTART_INDEX);
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

    maxindex += basevertex;

    return maxindex + 1;
}


GLuint
_glDrawRangeElementsBaseVertex_count(GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex)
{
    if (end < start ||
        count < 0) {
        return 0;
    }

    return end + basevertex + 1;
}


GLuint
_glMultiDrawArrays_count(const GLint *first, const GLsizei *count, GLsizei drawcount)
{
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < drawcount; ++draw) {
        GLuint _count_draw = _glDrawArrays_count(first[draw], count[draw]);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}


GLuint
_glMultiDrawElements_count(const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount)
{
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < drawcount; ++draw) {
        GLuint _count_draw = _glDrawElements_count(count[draw], type, indices[draw]);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}


GLuint
_glMultiDrawElementsBaseVertex_count(const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount, const GLint * basevertex)
{
    GLuint _count = 0;
    for (GLsizei draw = 0; draw < drawcount; ++draw) {
        GLuint _count_draw = _glDrawElementsBaseVertex_count(count[draw], type, indices[draw], basevertex[draw]);
        _count = std::max(_count, _count_draw);
    }
    return _count;
}

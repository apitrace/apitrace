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

#pragma once


#include "glproc.hpp"
#include "glsize.hpp"


namespace gltrace {
    class Context;
};


/*
 * TODO: Unify all draw params structure into a single structure.
 */


struct DrawArraysParams
{
    GLuint first = 0;
    GLuint count = 0;
    GLsizei instancecount = 1;
    GLuint baseinstance = 0;
};


struct DrawElementsParams
{
    GLuint start = 0;
    GLuint end = ~0U;
    GLuint count = 0;
    GLenum type = GL_NONE;
    const void *indices = nullptr;
    GLint basevertex = 0;
    GLsizei instancecount = 1;
    GLuint baseinstance = 0;
};


struct MultiDrawArraysParams
{
    const GLint *first = nullptr;
    const GLsizei *count = nullptr;
    GLsizei drawcount = 0;
    GLsizei instancecount = 1;
    GLuint baseinstance = 0;
};


struct MultiDrawElementsParams
{
    const GLsizei *count = nullptr;
    GLenum type = GL_NONE;
    const void * const *indices = nullptr;
    const GLint *basevertex = nullptr;
    GLsizei drawcount = 0;
    GLsizei instancecount = 1;
    GLuint baseinstance = 0;
};


GLuint
_glDraw_count(gltrace::Context *ctx, const DrawArraysParams &params);

GLuint
_glDraw_count(gltrace::Context *ctx, const DrawElementsParams &params);

GLuint
_glDraw_count(gltrace::Context *ctx, const MultiDrawArraysParams &params);

GLuint
_glDraw_count(gltrace::Context *ctx, const MultiDrawElementsParams &params);

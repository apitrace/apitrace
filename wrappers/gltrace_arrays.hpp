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


GLuint
_glDrawArrays_count(GLint first, GLsizei count);

#define _glDrawArraysEXT_count _glDrawArrays_count

GLuint
_glDrawElementsBaseVertex_count(GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex);

GLuint
_glDrawRangeElementsBaseVertex_count(GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);


#define _glDrawElements_count(count, type, indices) _glDrawElementsBaseVertex_count(count, type, indices, 0)
#define _glDrawRangeElements_count(start, end, count, type, indices) _glDrawRangeElementsBaseVertex_count(start, end, count, type, indices, 0)
#define _glDrawRangeElementsEXT_count _glDrawRangeElements_count

/* FIXME take in consideration instancing */
#define _glDrawArraysInstanced_count(first, count, primcount) _glDrawArrays_count(first, count)
#define _glDrawElementsInstanced_count(count, type, indices, primcount) _glDrawElements_count(count, type, indices)
#define _glDrawElementsInstancedBaseVertex_count(count, type, indices, primcount, basevertex) _glDrawElementsBaseVertex_count(count, type, indices, basevertex)
#define _glDrawRangeElementsInstanced_count(start, end, count, type, indices, primcount) _glDrawRangeElements_count(start, end, count, type, indices)
#define _glDrawRangeElementsInstancedBaseVertex_count(start, end, count, type, indices, primcount, basevertex) _glDrawRangeElementsBaseVertex_count(start, end, count, type, indices, basevertex)

#define _glDrawArraysInstancedBaseInstance_count(first, count, primcount, baseinstance) _glDrawArrays_count(first, count)
#define _glDrawElementsInstancedBaseInstance_count(count, type, indices, primcount, baseinstance) _glDrawElements_count(count, type, indices)
#define _glDrawElementsInstancedBaseVertexBaseInstance_count(count, type, indices, primcount, basevertex, baseinstance) _glDrawElementsBaseVertex_count(count, type, indices, basevertex)

#define _glDrawArraysInstancedARB_count _glDrawArraysInstanced_count
#define _glDrawArraysInstancedEXT_count _glDrawArraysInstanced_count
#define _glDrawArraysInstancedANGLE_count _glDrawArraysInstanced_count
#define _glDrawArraysInstancedBaseInstanceEXT_count _glDrawArraysInstancedBaseInstance_count
#define _glDrawElementsBaseVertexEXT_count _glDrawElementsBaseVertex_count
#define _glDrawElementsInstancedARB_count _glDrawElementsInstanced_count
#define _glDrawElementsInstancedEXT_count _glDrawElementsInstanced_count
#define _glDrawElementsInstancedANGLE_count _glDrawElementsInstanced_count
#define _glDrawElementsInstancedBaseInstanceEXT_count _glDrawElementsInstancedBaseInstance_count
#define _glDrawElementsInstancedBaseVertexEXT_count _glDrawElementsInstancedBaseVertex_count
#define _glDrawElementsInstancedBaseVertexBaseInstanceEXT_count _glDrawElementsInstancedBaseVertexBaseInstance_count
#define _glDrawRangeElementsBaseVertexEXT_count _glDrawRangeElementsBaseVertex_count


GLuint
_glMultiDrawArrays_count(const GLint *first, const GLsizei *count, GLsizei drawcount);

GLuint
_glMultiDrawElements_count(const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount);

GLuint
_glMultiDrawElementsBaseVertex_count(const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount, const GLint * basevertex);

#define _glMultiDrawArraysEXT_count _glMultiDrawArrays_count
#define _glMultiDrawElementsEXT_count _glMultiDrawElements_count
#define _glMultiDrawElementsBaseVertexEXT_count _glMultiDrawElementsBaseVertex_count

#define _glMultiModeDrawArraysIBM_count(first, count, drawcount, modestride) _glMultiDrawArrays_count(first, count, drawcount)
#define _glMultiModeDrawElementsIBM_count(count, type, indices, drawcount, modestride) _glMultiDrawElements_count(count, type, (const GLvoid **)indices, drawcount)



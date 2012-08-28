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

#ifndef _GLRETRACE_HPP_
#define _GLRETRACE_HPP_

#include "glws.hpp"
#include "retrace.hpp"


namespace glretrace {

struct Context {
    Context(glws::Context* context)
        : wsContext(context),
          drawable(0),
          activeProgram(0),
          used(false)
    {
    }

    ~Context();

    glws::Context* wsContext;

    // Bound drawable
    glws::Drawable *drawable;

    GLuint activeProgram;
    bool used;
    
    // Context must be current
    inline bool
    hasExtension(const char *extension) const {
        return wsContext->hasExtension(extension);
    }
};

extern bool insideList;
extern bool insideGlBeginEnd;
extern bool supportsARBShaderObjects;

Context *
getCurrentContext(void);


int
parseAttrib(const trace::Value *attribs, int param, int default_);


glws::Drawable *
createDrawable(glws::Profile profile);

glws::Drawable *
createDrawable(void);

glws::Drawable *
createPbuffer(int width, int height);

Context *
createContext(Context *shareContext, glws::Profile profile);

Context *
createContext(Context *shareContext = 0);

bool
makeCurrent(trace::Call &call, glws::Drawable *drawable, Context *context);


void
checkGlError(trace::Call &call);

extern const retrace::Entry gl_callbacks[];
extern const retrace::Entry cgl_callbacks[];
extern const retrace::Entry glx_callbacks[];
extern const retrace::Entry wgl_callbacks[];
extern const retrace::Entry egl_callbacks[];

void frame_complete(trace::Call &call);
void initContext();


void updateDrawable(int width, int height);

void flushQueries();
void beginProfile(trace::Call &call, bool isDraw);
void endProfile(trace::Call &call, bool isDraw);

} /* namespace glretrace */


#endif /* _GLRETRACE_HPP_ */

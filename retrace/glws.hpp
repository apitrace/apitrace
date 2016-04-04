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

/*
 * Abstraction for GL window system specific APIs (GLX, WGL).
 */

#pragma once


#include <assert.h>

#include <vector>
#include <string>

#include "glfeatures.hpp"


namespace glws {


using glfeatures::Profile;

class Drawable;


bool
checkExtension(const char *extName, const char *extString);

// Extra info for creating PBuffers
struct pbuffer_info
{
    int texFormat;  // GL_RGB, GL_RGBA, or GL_NONE
    int texTarget;  // GL_TEXTURE_1D/2D/CUBE_MAP or GL_NONE
    bool texMipmap; // 0 or 1 (false/true)

    Drawable *hdc_drawable;  // Needed for WGL Pbuffers
};


template< class T >
class Attributes {
protected:
    std::vector<T> attribs;

public:
    void add(T param) {
        attribs.push_back(param);
    }

    void add(T pname, T pvalue) {
        add(pname);
        add(pvalue);
    }

    void end(T terminator = 0) {
        add(terminator);
    }

    operator T * (void) {
        return &attribs[0];
    }

    operator const T * (void) const {
        return &attribs[0];
    }
};


class Visual
{
public:
    Profile profile;

    /* TODO */
#if 0
    unsigned long redMask;
    unsigned long greenMask;
    unsigned long blueMask;
    unsigned long alphaMask;
#endif
    bool doubleBuffer;

    Visual(Profile prof) :
        profile(prof)
    {}

    virtual ~Visual() {}
};


class Drawable
{
public:
    const Visual *visual;
    int width;
    int height;
    bool pbuffer;
    bool visible;

    // For WGL_ARB_render_texture
    glws::pbuffer_info pbInfo;
    int mipmapLevel, cubeFace;

    Drawable(const Visual *vis, int w, int h, bool pb) :
        visual(vis),
        width(w),
        height(h),
        pbuffer(pb),
        visible(false),
        mipmapLevel(0),
        cubeFace(0)
    {}

    virtual ~Drawable() {}
    
    virtual void
    resize(int w, int h) {
        width = w;
        height = h;
    }

    virtual void
    show(void) {
        assert(!pbuffer);
        visible = true;
    }

    virtual void copySubBuffer(int x, int y, int width, int height);

    virtual void swapBuffers(void) = 0;
};


class Context
{
public:
    const Visual *visual;

    // Requested profile
    Profile profile;

    // Created profile
    Profile actualProfile;
    glfeatures::Extensions actualExtensions;
    glfeatures::Features actualFeatures;

    Context(const Visual *vis) :
        visual(vis),
        profile(vis->profile),
        actualProfile(profile),
        initialized(false)
    {}

    virtual ~Context() {}

    // Context must have been made current once
    inline bool
    hasExtension(const char *extension) const {
        assert(initialized);
        return actualExtensions.has(extension);
    }

private:
    bool initialized;
    void initialize(void);

    friend bool makeCurrent(Drawable *, Context *);
};


void
init(void);

void
cleanup(void);

Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile);

Drawable *
createDrawable(const Visual *visual, int width, int height,
               const glws::pbuffer_info *pbInfo = NULL);

Context *
createContext(const Visual *visual, Context *shareContext = 0, bool debug = false);

bool
makeCurrentInternal(Drawable *drawable, Context *context);

inline bool
makeCurrent(Drawable *drawable, Context *context)
{
    bool success = makeCurrentInternal(drawable, context);
    if (success && context && !context->initialized) {
        context->initialize();
    }
    return success;
}

bool
processEvents(void);

// iBuffer is one of GL_FRONT/BACK_LEFT/RIGHT, GL_AUX0...
bool
bindTexImage(Drawable *pBuffer, int iBuffer);

// iBuffer is one of GL_FRONT/BACK_LEFT/RIGHT, GL_AUX0...
bool
releaseTexImage(Drawable *pBuffer, int iBuffer);

bool
setPbufferAttrib(Drawable *pBuffer, const int *attribList);


} /* namespace glws */

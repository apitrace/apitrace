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

#ifndef _GLWS_HPP_
#define _GLWS_HPP_


#include <assert.h>

#include <vector>
#include <set>
#include <string>


namespace glws {


enum Profile {
    PROFILE_COMPAT      = 0x000,
    PROFILE_3_0         = 0x030,
    PROFILE_3_1         = 0x031,
    PROFILE_3_2_CORE    = 0x132,
    PROFILE_3_3_CORE    = 0x133,
    PROFILE_4_0_CORE    = 0x140,
    PROFILE_4_1_CORE    = 0x141,
    PROFILE_4_2_CORE    = 0x142,
    PROFILE_4_3_CORE    = 0x143,
    PROFILE_4_4_CORE    = 0x144,
    PROFILE_ES1         = 0x210,
    PROFILE_ES2         = 0x220,
    PROFILE_MAX
};


enum Api {
    API_GL = 0,
    API_GLES
};


struct ProfileDesc {
    Api api;
    unsigned major;
    unsigned minor;
    bool core;
};


void
getProfileDesc(Profile profile, ProfileDesc &desc);


bool
checkExtension(const char *extName, const char *extString);


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

    Drawable(const Visual *vis, int w, int h, bool pb) :
        visual(vis),
        width(w),
        height(h),
        pbuffer(pb),
        visible(false)
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
    Profile profile;
    
    std::set<std::string> extensions;

    Context(const Visual *vis) :
        visual(vis),
        profile(vis->profile)
    {}

    virtual ~Context() {}

    // Context must be current
    bool
    hasExtension(const char *extension);
};


void
init(void);

void
cleanup(void);

Visual *
createVisual(bool doubleBuffer = false, unsigned samples = 1, Profile profile = PROFILE_COMPAT);

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer = false);

bool
bindApi(Api api);

Context *
createContext(const Visual *visual, Context *shareContext = 0, bool debug = false);

bool
makeCurrent(Drawable *drawable, Context *context);

bool
processEvents(void);


} /* namespace glws */


#endif /* _GLWS_HPP_ */

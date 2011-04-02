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

#ifndef _GLRETRACE_HPP_
#define _GLRETRACE_HPP_


namespace glretrace {


class Visual
{
public:
    unsigned long redMask;
    unsigned long greenMask;
    unsigned long blueMask;
    unsigned long alphaMask;
    bool doubleBuffer;

    virtual ~Visual() {}
};


class Drawable
{
public:
    const Visual *visual;
    unsigned width;
    unsigned height;

    Drawable(const Visual *vis) :
        visual(vis)
    {}

    virtual ~Drawable() {}
    
    virtual void
    resize(unsigned w, unsigned h) {
        width = w;
        height = h;
    }

    virtual void swapBuffers(void) = 0;
};


class Context
{
public:
    const Visual *visual;
    
    Context(const Visual *vis) :
        visual(vis)
    {}

    virtual ~Context() {}
};


class WindowSystem
{
public:
    Drawable *currentDrawable;
    Context *currentContext;


    inline WindowSystem() :
        currentDrawable(NULL),
        currentContext(NULL)
    {}

    virtual ~WindowSystem() {}

    virtual Visual *
    createVisual(bool doubleBuffer=false) = 0;
    
    virtual Drawable *
    createDrawable(const Visual *visual) = 0;

    virtual Context *
    createContext(const Visual *visual) = 0;
    
    virtual bool
    makeCurrent(Drawable *drawable, Context *context) = 0;

    virtual bool
    processEvents(void) = 0;
};


WindowSystem *createNativeWindowSystem(void);


} /* namespace glretrace */


#endif /* _GLRETRACE_HPP_ */

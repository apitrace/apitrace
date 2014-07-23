/**************************************************************************
 *
 * Copyright 2011 VMware, Inc.
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


/**
 * Minimal Cocoa integration.
 *
 * See also:
 * - http://developer.apple.com/library/mac/#samplecode/CocoaGL/Introduction/Intro.html
 * - http://developer.apple.com/library/mac/#samplecode/Cocoa_With_Carbon_or_CPP/Introduction/Intro.html
 * - http://developer.apple.com/library/mac/#samplecode/glut/Introduction/Intro.html
 * - http://developer.apple.com/library/mac/#samplecode/GLEssentials/Introduction/Intro.html
 * - http://www.glfw.org/
 * - http://cocoasamurai.blogspot.co.uk/2008/04/guide-to-threading-on-leopard.html
 * - http://developer.apple.com/library/ios/#documentation/Cocoa/Conceptual/Multithreading/Introduction/Introduction.html
 */


#include "glproc.hpp"

#include <stdlib.h>
#include <iostream>

#include <dlfcn.h>

#include <Cocoa/Cocoa.h>

#include "os_thread.hpp"
#include "glws.hpp"


/**
 * Dummy thread to force Cocoa to enter multithreading mode.
 */
@interface DummyThread : NSObject
    + (void)enterMultiThreaded;
    + (void)dummyThreadMethod:(id)unused;
@end

@implementation DummyThread
    + (void)dummyThreadMethod:(id)unused {
        (void)unused;
    }

    + (void)enterMultiThreaded {
        [NSThread detachNewThreadSelector:@selector(dummyThreadMethod:)
                  toTarget:self
                  withObject:nil];
    }
@end


namespace glws {


static OS_THREAD_SPECIFIC_PTR(NSAutoreleasePool)
autoreleasePool;


class CocoaVisual : public Visual
{
public:
    NSOpenGLPixelFormat *pixelFormat;

    CocoaVisual(Profile prof, NSOpenGLPixelFormat *pf) :
        Visual(prof),
        pixelFormat(pf)
    {}

    ~CocoaVisual() {
        [pixelFormat release];
    }
};
 

class CocoaDrawable : public Drawable
{
public:
    NSWindow *window;
    NSOpenGLView *view;
    NSOpenGLContext *currentContext;

    CocoaDrawable(const Visual *vis, int w, int h, bool pbuffer) :
        Drawable(vis, w, h, pbuffer),
        currentContext(nil)
    {
        NSOpenGLPixelFormat *pixelFormat = static_cast<const CocoaVisual *>(visual)->pixelFormat;

        NSRect winRect = NSMakeRect(0, 0, w, h);

        window = [[NSWindow alloc]
                         initWithContentRect:winRect
                                   styleMask:NSTitledWindowMask |
                                             NSClosableWindowMask |
                                             NSMiniaturizableWindowMask
                                     backing:NSBackingStoreRetained
                                       defer:NO];
        assert(window != nil);

        view = [[NSOpenGLView alloc]
                initWithFrame:winRect
                  pixelFormat:pixelFormat];
        assert(view != nil);

        [window setContentView:view];
        [window setTitle:@"glretrace"];

    }

    ~CocoaDrawable() {
        [window release];
    }

    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }

        [window setContentSize:NSMakeSize(w, h)];

        if (currentContext != nil) {
            [currentContext update];
            [window makeKeyAndOrderFront:nil];
            [currentContext setView:view];
            [currentContext makeCurrentContext];
        }

        Drawable::resize(w, h);
    }

    void show(void) {
        if (visible) {
            return;
        }

        // TODO

        Drawable::show();
    }

    void swapBuffers(void) {
        if (currentContext != nil) {
            [currentContext flushBuffer];
        }
    }
};


class CocoaContext : public Context
{
public:
    NSOpenGLContext *context;

    CocoaContext(const Visual *vis, NSOpenGLContext *ctx) :
        Context(vis),
        context(ctx)
    {}

    ~CocoaContext() {
        [context release];
    }
};


static inline void
initThread(void) {
    if (autoreleasePool == nil) {
        autoreleasePool = [[NSAutoreleasePool alloc] init];
    }
}

void
init(void) {
    // Prevent glproc to load system's OpenGL, so that we can trace glretrace.
    _libGlHandle = dlopen("OpenGL", RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);

    initThread();

    [DummyThread enterMultiThreaded];

    bool isMultiThreaded = [NSThread isMultiThreaded];
    if (!isMultiThreaded) {
        std::cerr << "error: failed to enable Cocoa multi-threading\n";
        exit(1);
    }

    [NSApplication sharedApplication];

    [NSApp finishLaunching];
}


void
cleanup(void) {
    [autoreleasePool release];
}


Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {

    initThread();

    Attributes<NSOpenGLPixelFormatAttribute> attribs;

    attribs.add(NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)1);
    attribs.add(NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)24);
    if (doubleBuffer) {
        attribs.add(NSOpenGLPFADoubleBuffer);
    }
    attribs.add(NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)1);
    attribs.add(NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute)1);
    switch (profile) {
    case PROFILE_COMPAT:
        break;
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1090
    case PROFILE_3_3_CORE:
    case PROFILE_4_0_CORE:
    case PROFILE_4_1_CORE:
        /*
         * Fall-through.
         *
         * kCGLOGLPVersion_GL4_Core doesn't seem to work as expected.  The
         * recommended approach is to use NSOpenGLProfileVersion3_2Core and
         * then check the OpenGL version.
         *
         * TODO: Actually check the OpenGL version (the first time the OpenGL
         * context is bound).
         */
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
    case PROFILE_3_0:
    case PROFILE_3_1:
    case PROFILE_3_2_CORE:
        attribs.add(NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core);
        break;
#endif
    default:
        return NULL;
    }
    
    // Use Apple software rendering for debugging purposes.
    if (0) {
        attribs.add(NSOpenGLPFARendererID, 0x00020200); // kCGLRendererGenericID
    }

    attribs.end();

    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc]
                                     initWithAttributes:attribs];

    return new CocoaVisual(profile, pixelFormat);
}

Drawable *
createDrawable(const Visual *visual, int width, int height, bool pbuffer)
{
    initThread();

    return new CocoaDrawable(visual, width, height, pbuffer);
}

bool
bindApi(Api api)
{
    return true;
}

Context *
createContext(const Visual *visual, Context *shareContext, bool debug)
{
    initThread();

    NSOpenGLPixelFormat *pixelFormat = static_cast<const CocoaVisual *>(visual)->pixelFormat;
    NSOpenGLContext *share_context = nil;
    NSOpenGLContext *context;

    if (shareContext) {
        share_context = static_cast<CocoaContext*>(shareContext)->context;
    }

    context = [[NSOpenGLContext alloc]
               initWithFormat:pixelFormat
               shareContext:share_context];
    assert(context != nil);

    return new CocoaContext(visual, context);
}

bool
makeCurrent(Drawable *drawable, Context *context)
{
    initThread();

    if (!drawable || !context) {
        [NSOpenGLContext clearCurrentContext];
    } else {
        CocoaDrawable *cocoaDrawable = static_cast<CocoaDrawable *>(drawable);
        CocoaContext *cocoaContext = static_cast<CocoaContext *>(context);

        [cocoaDrawable->window makeKeyAndOrderFront:nil];
        [cocoaContext->context setView:[cocoaDrawable->window contentView]];
        [cocoaContext->context makeCurrentContext];

        cocoaDrawable->currentContext = cocoaContext->context;
    }

    return TRUE;
}

bool
processEvents(void) {
    initThread();

    NSEvent* event;
    do {
        event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                   untilDate:[NSDate distantPast]
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES];
        if (event)
            [NSApp sendEvent:event];
    } while (event);

    return true;
}


} /* namespace glws */

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
#include <string.h>
#include <iostream>

#include <dlfcn.h>

#include <Cocoa/Cocoa.h>

#include "glws.hpp"


/**
 * Dummy thread to force Cocoa to enter multithreading mode.
 *
 * See also:
 * - https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html
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


static __thread NSAutoreleasePool *
autoreleasePool = nil;


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

    CocoaDrawable(const Visual *vis, int w, int h, const pbuffer_info *info) :
        Drawable(vis, w, h, info),
        currentContext(nil)
    {
        NSOpenGLPixelFormat *pixelFormat = static_cast<const CocoaVisual *>(visual)->pixelFormat;

        NSRect winRect = NSMakeRect(0, 0, w, h);

        window = [[NSWindow alloc]
                         initWithContentRect:winRect
                                   styleMask:NSWindowStyleMaskTitled |
                                             NSWindowStyleMaskClosable |
                                             NSWindowStyleMaskMiniaturizable
                                     backing:NSBackingStoreBuffered
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
        [view release];
        [window close];
    }

    void
    resize(int w, int h) {
        if (w == width && h == height) {
            return;
        }

        [window setContentSize:NSMakeSize(w, h)];

        processEvents();

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


static GLint gRendererID = 0;


void
init(void) {
    // Prevent glproc to load system's OpenGL, so that we can trace glretrace.
    _libGlHandle = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);
    assert(_libGlHandle);

    initThread();

    [DummyThread enterMultiThreaded];

    bool isMultiThreaded = [NSThread isMultiThreaded];
    if (!isMultiThreaded) {
        std::cerr << "error: failed to enable Cocoa multi-threading\n";
        exit(1);
    }

    [NSApplication sharedApplication];

    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    [NSApp finishLaunching];
    [NSApp activateIgnoringOtherApps:YES];

    // Allow to pick a specific renderer via a command line option
    const char *vendor = getenv("VENDOR");
    if (vendor != nullptr) {
        CGLRendererInfoObj renderer_info = NULL;
        GLint num_renderers = 0;
        if (CGLQueryRendererInfo(~GLuint(0),
                    &renderer_info,
                    &num_renderers) == kCGLNoError) {
            for (GLint i = 0; i < num_renderers; ++i) {
                GLint rendererID = 0;
                if (CGLDescribeRenderer(renderer_info,
                            i,
                            kCGLRPRendererID,
                            &rendererID) != kCGLNoError) {
                    continue;
                }

                GLint vendorID = rendererID & kCGLRendererIDMatchingMask & ~0xfff;

                const char *name;
                switch (vendorID) {
                case 0x00020000:
                    name = "Software";
                    break;
                case 0x00021000:
                    name = "AMD";
                    break;
                case 0x00022000:
                    name = "NVIDIA";
                    break;
                case 0x00024000:
                    name = "Intel";
                    break;
                default:
                    name = "Unknown";
                    break;
                }

                if (gRendererID == 0 &&
                    strcasecmp(vendor, name) == 0) {
                    gRendererID = rendererID;
                }

                GLint accelerated = 0;
                if (CGLDescribeRenderer(renderer_info,
                            i,
                            kCGLRPAccelerated,
                            &accelerated) == kCGLNoError) {
                }

                std::cerr << "info: found " << name
                          << " renderer (0x" << std::hex << rendererID << std::dec << ") "
                          << (accelerated ? "accelerated" : "unacellerated")
                          << std::endl;
            }
        }
    }
}


void
cleanup(void) {
    [autoreleasePool release];
}


Visual *
createVisual(bool doubleBuffer, unsigned samples, Profile profile) {

    initThread();

    Attributes<NSOpenGLPixelFormatAttribute> attribs;

    attribs.add(NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)8);
    attribs.add(NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)24);
    if (doubleBuffer) {
        attribs.add(NSOpenGLPFADoubleBuffer);
    }
    attribs.add(NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24);
    attribs.add(NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute)8);

    if (profile.api != glfeatures::API_GL) {
        return NULL;
    }

    // We snap 3.1 to 3.2 core, as allowed by GLX/WGL_ARB_create_context
    if (profile.versionGreaterOrEqual(3, 1) || profile.core) {
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
        /*
         * kCGLOGLPVersion_GL4_Core doesn't seem to work as expected.  The
         * recommended approach is to use NSOpenGLProfileVersion3_2Core and
         * then check the OpenGL version.
         */
        attribs.add(NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core);
#else
        return NULL;
#endif
    } else if (profile.versionGreaterOrEqual(3, 0)) {
        // Compatibility profile is not supported
        return NULL;
    }
    
    if (gRendererID) {
        attribs.add(NSOpenGLPFARendererID, gRendererID); // kCGLRendererGenericID
    }

    // Force hardware acceleration
    // https://developer.apple.com/library/mac/qa/qa1502/_index.html
    if (0) {
        attribs.add(NSOpenGLPFAAccelerated);
        attribs.add(NSOpenGLPFANoRecovery);
    }

    // Tell this context is offline renderer aware
    // https://developer.apple.com/library/content/technotes/tn2229/_index.html
    // TODO: override NSOpenGLView::update to receive notifications
    attribs.add(NSOpenGLPFAAllowOfflineRenderers);

    attribs.end();

    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc]
                                     initWithAttributes:attribs];

    return new CocoaVisual(profile, pixelFormat);
}

Drawable *
createDrawable(const Visual *visual, int width, int height,
               const pbuffer_info *info)
{
    initThread();

    return new CocoaDrawable(visual, width, height, info);
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
makeCurrentInternal(Drawable *drawable, Drawable *readable, Context *context)
{
    // NOTE: the 'readable' argument is ignored.
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

    if (![NSThread isMainThread]) {
        return true;
    }

    NSEvent* event;
    do {
        event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                   untilDate:[NSDate distantPast]
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES];
        if (event)
            [NSApp sendEvent:event];
    } while (event);

    return true;
}

bool
bindTexImage(Drawable *pBuffer, int iBuffer) {
    std::cerr << "error: Cocoa::wglBindTexImageARB not implemented.\n";
    assert(pBuffer->pbuffer);
    return true;
}

bool
releaseTexImage(Drawable *pBuffer, int iBuffer) {
    std::cerr << "error: Cocoa::wglReleaseTexImageARB not implemented.\n";
    assert(pBuffer->pbuffer);
    return true;
}

bool
setPbufferAttrib(Drawable *pBuffer, const int *attribList) {
    // nothing to do here.
    assert(pBuffer->pbuffer);
    return true;
}


} /* namespace glws */

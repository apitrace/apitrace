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


#include <assert.h>
#include <stdint.h>

#include "image.hpp"
#include "glproc.hpp"
#include "glsize.hpp"


#if !defined(_WIN32) && !defined(__APPLE__)


#include <X11/Xproto.h>


static int
errorHandler(Display *display, XErrorEvent *event)
{
    if (event->error_code == BadDrawable &&
        event->request_code == X_GetGeometry) {
        return 0;
    }

    char error_text[512];
    XGetErrorText(display, event->error_code, error_text, sizeof error_text);
    fprintf(stderr, "X Error of failed request:  %s\n", error_text);

    return 0;
}


#endif /* !_WIN32 && !__APPLE__ */


namespace glsnapshot {


/**
 * Get the contents of the current drawable into an image.
 */
static Image::Image *
getDrawableImage(void) {
#if defined(_WIN32)

    HDC hDC = __wglGetCurrentDC();
    if (!hDC) {
        return false;
    }

    HWND hWnd = WindowFromDC(hDC);
    RECT rect;

    if (!GetClientRect(hWnd, &rect)) {
       return false;
    }

    int width  = rect.right  - rect.left;
    int height = rect.bottom - rect.top;

    // TODO: http://msdn.microsoft.com/en-us/library/dd183402

    return NULL;

#elif defined(__APPLE__)

    // TODO
    return NULL;

#else

    Display *display;
    Drawable drawable;
    Window root;
    int x, y;
    unsigned int w, h, bw, depth;

    __glFinish();
    __glXWaitGL();

    display = __glXGetCurrentDisplay();
    if (!display) {
        return false;
    }

    drawable = __glXGetCurrentDrawable();
    if (drawable == None) {
        return false;
    }

    /*
     * XXX: This does not work for drawables created with glXCreateWindow
     */

    int (*oldErrorHandler)(Display *, XErrorEvent *);
    Status status;

    oldErrorHandler = XSetErrorHandler(errorHandler);
    status = XGetGeometry(display, drawable, &root, &x, &y, &w, &h, &bw, &depth);
    XSetErrorHandler(oldErrorHandler);
    if (!status) {
        return false;
    }

    XImage *ximage;

    ximage = XGetImage(display, drawable, 0, 0, w, h, AllPlanes, ZPixmap);
    if (!ximage) {
        return NULL;
    }

    Image::Image *image = NULL;

    if (ximage->depth          == 24 &&
        ximage->bits_per_pixel == 32 &&
        ximage->red_mask       == 0x00ff0000 &&
        ximage->green_mask     == 0x0000ff00 &&
        ximage->blue_mask      == 0x000000ff) {

        image = new Image::Image(w, h, 4);

        if (image) {
            const uint32_t *src = (const uint32_t *)ximage->data;
            uint32_t *dst = (uint32_t*) image->start();
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    uint32_t bgra = src[x];
                    uint32_t rgba = (bgra & 0xff00ff00)
                                  | ((bgra >> 16) & 0xff)
                                  | ((bgra & 0xff) << 16);
                    dst[x] = rgba;
                }

                src += ximage->bytes_per_line / sizeof *src;
                dst += image->stride() / sizeof *src;
            }
        }
    } else {
        OS::DebugMessage("apitrace: unexpected XImage: "
                         "bits_per_pixel = %i, "
                         "depth = %i, "
                         "red_mask = 0x%08lx, "
                         "green_mask = 0x%08lx, "
                         "blue_mask = 0x%08lx\n",
                         ximage->bits_per_pixel,
                         ximage->depth,
                         ximage->red_mask,
                         ximage->green_mask,
                         ximage->blue_mask);
    }

    XDestroyImage(ximage);

    return image;
#endif
}


// Prefix of the snapshot images to take, if not NULL.
static const char *snapshot_prefix = NULL;

// Maximum number of frames to trace.
static unsigned max_frames = ~0;

// Current frame number.
static unsigned frame_no = 0;


void snapshot(unsigned call_no) {

    if (frame_no == 0) {
        const char *max_frames_str = getenv("TRACE_FRAMES");
        if (max_frames_str) {
            max_frames = atoi(max_frames_str);
        }
        snapshot_prefix = getenv("TRACE_SNAPSHOT");
    }

    ++frame_no;

    if (snapshot_prefix) {
        Image::Image *src = getDrawableImage();
        if (src) {
            char filename[PATH_MAX];
            snprintf(filename, sizeof filename, "%s%010u.png", snapshot_prefix, call_no);
            if (src->writePNG(filename)) {
                OS::DebugMessage("apitrace: wrote %s\n", filename);
            }

            delete src;
        }
    }

    if (frame_no >= max_frames) {
        exit(0);
    }
}


} /* namespace glsnapshot */

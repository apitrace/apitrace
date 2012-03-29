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


#include <string.h>

#include <algorithm>
#include <iostream>

#include "image.hpp"
#include "json.hpp"
#include "glproc.hpp"
#include "glsize.hpp"
#include "glstate.hpp"
#include "glstate_internal.hpp"


#ifdef __APPLE__

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C" {
#endif

OSStatus CGSGetSurfaceBounds(CGSConnectionID, CGWindowID, CGSSurfaceID, CGRect *);

#ifdef __cplusplus
}
#endif

#endif /* __APPLE__ */


/* Change thi to one to force interpreting depth buffers as RGBA, which enables
 * visualizing full dynamic range, until we can transmit HDR images to the GUI */
#define DEPTH_AS_RGBA 0


namespace glstate {


static inline void
dumpTextureImage(JSONWriter &json, Context &context, GLenum target, GLint level)
{
    GLint width, height = 1, depth = 1;
    GLint format;

    glGetTexLevelParameteriv(target, level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    width = 0;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);

    if (target != GL_TEXTURE_1D) {
        height = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);
        if (target == GL_TEXTURE_3D) {
            depth = 0;
            glGetTexLevelParameteriv(target, level, GL_TEXTURE_DEPTH, &depth);
        }
    }

    if (width <= 0 || height <= 0 || depth <= 0) {
        return;
    } else {
        char label[512];

        GLint active_texture = GL_TEXTURE0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);
        snprintf(label, sizeof label, "%s, %s, level = %d",
                 enumToString(active_texture), enumToString(target), level);

        json.beginMember(label);

        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", depth);

        json.writeStringMember("__format__", enumToString(format));

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", 4);

        GLubyte *pixels = new GLubyte[depth*width*height*4];

        context.resetPixelPackState();

        glGetTexImage(target, level, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        context.restorePixelPackState();

        json.beginMember("__data__");
        char *pngBuffer;
        int pngBufferSize;
        image::writePixelsToBuffer(pixels, width, height, 4, true, &pngBuffer, &pngBufferSize);
        json.writeBase64(pngBuffer, pngBufferSize);
        free(pngBuffer);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
}


static inline void
dumpTexture(JSONWriter &json, Context &context, GLenum target, GLenum binding)
{
    GLint texture_binding = 0;
    glGetIntegerv(binding, &texture_binding);
    if (!glIsEnabled(target) && !texture_binding) {
        return;
    }

    GLint level = 0;
    do {
        GLint width = 0, height = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);
        if (!width || !height) {
            break;
        }

        if (target == GL_TEXTURE_CUBE_MAP) {
            for (int face = 0; face < 6; ++face) {
                dumpTextureImage(json, context, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level);
            }
        } else {
            dumpTextureImage(json, context, target, level);
        }

        ++level;
    } while(true);
}


void
dumpTextures(JSONWriter &json, Context &context)
{
    json.beginMember("textures");
    json.beginObject();
    GLint active_texture = GL_TEXTURE0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);
    GLint max_texture_coords = 0;
    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);
    GLint max_combined_texture_image_units = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);
    GLint max_units = std::max(max_combined_texture_image_units, max_texture_coords);
    for (GLint unit = 0; unit < max_units; ++unit) {
        GLenum texture = GL_TEXTURE0 + unit;
        glActiveTexture(texture);
        dumpTexture(json, context, GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D);
        dumpTexture(json, context, GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D);
        dumpTexture(json, context, GL_TEXTURE_3D, GL_TEXTURE_BINDING_3D);
        dumpTexture(json, context, GL_TEXTURE_RECTANGLE, GL_TEXTURE_BINDING_RECTANGLE);
        dumpTexture(json, context, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP);
    }
    glActiveTexture(active_texture);
    json.endObject();
    json.endMember(); // textures
}


static bool
getDrawableBounds(GLint *width, GLint *height) {
#if defined(TRACE_EGL)

    EGLContext currentContext = eglGetCurrentContext();
    if (currentContext == EGL_NO_CONTEXT) {
        return false;
    }

    EGLSurface currentSurface = eglGetCurrentSurface(EGL_DRAW);
    if (currentSurface == EGL_NO_SURFACE) {
        return false;
    }

    EGLDisplay currentDisplay = eglGetCurrentDisplay();
    if (currentDisplay == EGL_NO_DISPLAY) {
        return false;
    }

    if (!eglQuerySurface(currentDisplay, currentSurface, EGL_WIDTH, width) ||
        !eglQuerySurface(currentDisplay, currentSurface, EGL_HEIGHT, height)) {
        return false;
    }

    return true;

#elif defined(_WIN32)

    HDC hDC = wglGetCurrentDC();
    if (!hDC) {
        return false;
    }

    HWND hWnd = WindowFromDC(hDC);
    RECT rect;

    if (!GetClientRect(hWnd, &rect)) {
       return false;
    }

    *width  = rect.right  - rect.left;
    *height = rect.bottom - rect.top;
    return true;

#elif defined(__APPLE__)

    CGLContextObj ctx = CGLGetCurrentContext();
    if (ctx == NULL) {
        return false;
    }

    CGSConnectionID cid;
    CGSWindowID wid;
    CGSSurfaceID sid;

    if (CGLGetSurface(ctx, &cid, &wid, &sid) != kCGLNoError) {
        return false;
    }

    CGRect rect;

    if (CGSGetSurfaceBounds(cid, wid, sid, &rect) != 0) {
        return false;
    }

    *width = rect.size.width;
    *height = rect.size.height;
    return true;

#elif defined(HAVE_X11)

    Display *display;
    Drawable drawable;
    Window root;
    int x, y;
    unsigned int w, h, bw, depth;

    display = glXGetCurrentDisplay();
    if (!display) {
        return false;
    }

    drawable = glXGetCurrentDrawable();
    if (drawable == None) {
        return false;
    }

    if (!XGetGeometry(display, drawable, &root, &x, &y, &w, &h, &bw, &depth)) {
        return false;
    }

    *width = w;
    *height = h;
    return true;

#else

    return false;

#endif
}


static const GLenum texture_bindings[][2] = {
    {GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D},
    {GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D},
    {GL_TEXTURE_3D, GL_TEXTURE_BINDING_3D},
    {GL_TEXTURE_RECTANGLE, GL_TEXTURE_BINDING_RECTANGLE},
    {GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP}
};


static bool
bindTexture(GLint texture, GLenum &target, GLint &bound_texture)
{

    for (unsigned i = 0; i < sizeof(texture_bindings)/sizeof(texture_bindings[0]); ++i) {
        target  = texture_bindings[i][0];

        GLenum binding = texture_bindings[i][1];

        while (glGetError() != GL_NO_ERROR)
            ;

        glGetIntegerv(binding, &bound_texture);
        glBindTexture(target, texture);

        if (glGetError() == GL_NO_ERROR) {
            return true;
        }

        glBindTexture(target, bound_texture);
    }

    target = GL_NONE;

    return false;
}


static bool
getTextureLevelSize(GLint texture, GLint level, GLint *width, GLint *height)
{
    *width = 0;
    *height = 0;

    GLenum target;
    GLint bound_texture = 0;
    if (!bindTexture(texture, target, bound_texture)) {
        return false;
    }

    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, width);
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, height);

    glBindTexture(target, bound_texture);

    return *width > 0 && *height > 0;
}


static GLenum
getTextureLevelFormat(GLint texture, GLint level)
{
    GLenum target;
    GLint bound_texture = 0;
    if (!bindTexture(texture, target, bound_texture)) {
        return GL_NONE;
    }

    GLint format = GL_NONE;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    glBindTexture(target, bound_texture);

    return format;
}



static bool
getRenderbufferSize(GLint renderbuffer, GLint *width, GLint *height)
{
    GLint bound_renderbuffer = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &bound_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

    *width = 0;
    *height = 0;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, height);

    glBindRenderbuffer(GL_RENDERBUFFER, bound_renderbuffer);
    
    return *width > 0 && *height > 0;
}


static GLenum
getRenderbufferFormat(GLint renderbuffer)
{
    GLint bound_renderbuffer = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &bound_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

    GLint format = GL_NONE;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

    glBindRenderbuffer(GL_RENDERBUFFER, bound_renderbuffer);
    
    return format;
}


static bool
getFramebufferAttachmentSize(GLenum target, GLenum attachment, GLint *width, GLint *height)
{
    GLint object_type = GL_NONE;
    glGetFramebufferAttachmentParameteriv(target, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                          &object_type);
    if (object_type == GL_NONE) {
        return false;
    }

    GLint object_name = 0;
    glGetFramebufferAttachmentParameteriv(target, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                          &object_name);
    if (object_name == 0) {
        return false;
    }

    if (object_type == GL_RENDERBUFFER) {
        return getRenderbufferSize(object_name, width, height);
    } else if (object_type == GL_TEXTURE) {
        GLint texture_level = 0;
        glGetFramebufferAttachmentParameteriv(target, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
                                              &texture_level);
        return getTextureLevelSize(object_name, texture_level, width, height);
    } else {
        std::cerr << "warning: unexpected GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = " << object_type << "\n";
        return false;
    }
}



static GLint
getFramebufferAttachmentFormat(GLenum target, GLenum attachment)
{
    GLint object_type = GL_NONE;
    glGetFramebufferAttachmentParameteriv(target, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                          &object_type);
    if (object_type == GL_NONE) {
        return GL_NONE;
    }

    GLint object_name = 0;
    glGetFramebufferAttachmentParameteriv(target, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                          &object_name);
    if (object_name == 0) {
        return GL_NONE;
    }

    if (object_type == GL_RENDERBUFFER) {
        return getRenderbufferFormat(object_name);
    } else if (object_type == GL_TEXTURE) {
        GLint texture_level = 0;
        glGetFramebufferAttachmentParameteriv(target, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
                                              &texture_level);
        return getTextureLevelFormat(object_name, texture_level);
    } else {
        std::cerr << "warning: unexpected GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = " << object_type << "\n";
        return GL_NONE;
    }
}



image::Image *
getDrawBufferImage() {
    GLenum format = GL_RGB;
    GLint channels = __gl_format_channels(format);
    if (channels > 4) {
        return NULL;
    }

    Context context;

    GLenum framebuffer_binding;
    GLenum framebuffer_target;
    if (context.ES) {
        framebuffer_binding = GL_FRAMEBUFFER_BINDING;
        framebuffer_target = GL_FRAMEBUFFER;
    } else {
        framebuffer_binding = GL_DRAW_FRAMEBUFFER_BINDING;
        framebuffer_target = GL_DRAW_FRAMEBUFFER;
    }

    GLint draw_framebuffer = 0;
    glGetIntegerv(framebuffer_binding, &draw_framebuffer);

    GLint draw_buffer = GL_NONE;
    GLint width, height;
    if (draw_framebuffer) {
        if (context.ARB_draw_buffers) {
            glGetIntegerv(GL_DRAW_BUFFER0, &draw_buffer);
            if (draw_buffer == GL_NONE) {
                return NULL;
            }
        }

        if (!getFramebufferAttachmentSize(framebuffer_target, draw_buffer, &width, &height)) {
            return NULL;
        }
    } else {
        if (!context.ES) {
            glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
            if (draw_buffer == GL_NONE) {
                return NULL;
            }
        }

        if (!getDrawableBounds(&width, &height)) {
            return NULL;
        }
    }

    GLenum type = GL_UNSIGNED_BYTE;

#if DEPTH_AS_RGBA
    if (format == GL_DEPTH_COMPONENT) {
        type = GL_UNSIGNED_INT;
        channels = 4;
    }
#endif

    image::Image *image = new image::Image(width, height, channels, true);
    if (!image) {
        return NULL;
    }

    while (glGetError() != GL_NO_ERROR) {}

    GLint read_framebuffer = 0;
    GLint read_buffer = GL_NONE;
    if (!context.ES) {
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, draw_framebuffer);

        glGetIntegerv(GL_READ_BUFFER, &read_buffer);
        glReadBuffer(draw_buffer);
    }

    // TODO: reset imaging state too
    context.resetPixelPackState();

    glReadPixels(0, 0, width, height, format, type, image->pixels);

    context.restorePixelPackState();

    if (!context.ES) {
        glReadBuffer(read_buffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        do {
            std::cerr << "warning: " << enumToString(error) << " while getting snapshot\n";
            error = glGetError();
        } while(error != GL_NO_ERROR);
        delete image;
        return NULL;
    }
     
    return image;
}


/**
 * Dump the image of the currently bound read buffer.
 */
static inline void
dumpReadBufferImage(JSONWriter &json, GLint width, GLint height, GLenum format,
                    GLint internalFormat = GL_NONE)
{
    GLint channels = __gl_format_channels(format);

    Context context;

    json.beginObject();

    // Tell the GUI this is no ordinary object, but an image
    json.writeStringMember("__class__", "image");

    json.writeNumberMember("__width__", width);
    json.writeNumberMember("__height__", height);
    json.writeNumberMember("__depth__", 1);

    json.writeStringMember("__format__", enumToString(internalFormat));

    // Hardcoded for now, but we could chose types more adequate to the
    // texture internal format
    json.writeStringMember("__type__", "uint8");
    json.writeBoolMember("__normalized__", true);
    json.writeNumberMember("__channels__", channels);

    GLenum type = GL_UNSIGNED_BYTE;

#if DEPTH_AS_RGBA
    if (format == GL_DEPTH_COMPONENT) {
        type = GL_UNSIGNED_INT;
        channels = 4;
    }
#endif

    GLubyte *pixels = new GLubyte[width*height*channels];

    // TODO: reset imaging state too
    context.resetPixelPackState();

    glReadPixels(0, 0, width, height, format, type, pixels);

    context.restorePixelPackState();

    json.beginMember("__data__");
    char *pngBuffer;
    int pngBufferSize;
    image::writePixelsToBuffer(pixels, width, height, channels, true, &pngBuffer, &pngBufferSize);
    //std::cerr <<" Before = "<<(width * height * channels * sizeof *pixels)
    //          <<", after = "<<pngBufferSize << ", ratio = " << double(width * height * channels * sizeof *pixels)/pngBufferSize;
    json.writeBase64(pngBuffer, pngBufferSize);
    free(pngBuffer);
    json.endMember(); // __data__

    delete [] pixels;
    json.endObject();
}


static inline GLuint
downsampledFramebuffer(GLuint oldFbo, GLint drawbuffer,
                       GLint colorRb, GLint depthRb, GLint stencilRb,
                       GLuint *rbs, GLint *numRbs)
{
    GLuint fbo;
    GLint format;
    GLint w, h;

    *numRbs = 0;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glBindRenderbuffer(GL_RENDERBUFFER, colorRb);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_WIDTH, &w);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_HEIGHT, &h);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

    glGenRenderbuffers(1, &rbs[*numRbs]);
    glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
    glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, drawbuffer,
                              GL_RENDERBUFFER, rbs[*numRbs]);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(drawbuffer);
    glReadBuffer(drawbuffer);
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    ++*numRbs;

    if (stencilRb == depthRb && stencilRb) {
        //combined depth and stencil buffer
        glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_WIDTH, &w);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_HEIGHT, &h);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

        glGenRenderbuffers(1, &rbs[*numRbs]);
        glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
        glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbs[*numRbs]);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                          GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        ++*numRbs;
    } else {
        if (depthRb) {
            glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_WIDTH, &w);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_HEIGHT, &h);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

            glGenRenderbuffers(1, &rbs[*numRbs]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
            glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER, rbs[*numRbs]);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glDrawBuffer(GL_DEPTH_ATTACHMENT);
            glReadBuffer(GL_DEPTH_ATTACHMENT);
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            ++*numRbs;
        }
        if (stencilRb) {
            glBindRenderbuffer(GL_RENDERBUFFER, stencilRb);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_WIDTH, &w);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_HEIGHT, &h);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

            glGenRenderbuffers(1, &rbs[*numRbs]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
            glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, rbs[*numRbs]);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glDrawBuffer(GL_STENCIL_ATTACHMENT);
            glReadBuffer(GL_STENCIL_ATTACHMENT);
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                              GL_STENCIL_BUFFER_BIT, GL_NEAREST);
            ++*numRbs;
        }
    }

    return fbo;
}


/**
 * Dump images of current draw drawable/window.
 */
static void
dumpDrawableImages(JSONWriter &json, Context &context)
{
    GLint width, height;

    if (!getDrawableBounds(&width, &height)) {
        return;
    }

    GLint draw_buffer = GL_NONE;
    if (context.ES) {
        draw_buffer = GL_BACK;
    } else {
        glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
        glReadBuffer(draw_buffer);
    }

    if (draw_buffer != GL_NONE) {
        GLint read_buffer = GL_NONE;
        if (!context.ES) {
            glGetIntegerv(GL_READ_BUFFER, &read_buffer);
        }

        GLint alpha_bits = 0;
#if 0
        // XXX: Ignore alpha until we are able to match the traced visual
        glGetIntegerv(GL_ALPHA_BITS, &alpha_bits);
#endif
        GLenum format = alpha_bits ? GL_RGBA : GL_RGB;
        json.beginMember(enumToString(draw_buffer));
        dumpReadBufferImage(json, width, height, format);
        json.endMember();

        if (!context.ES) {
            glReadBuffer(read_buffer);
        }
    }

    if (!context.ES) {
        GLint depth_bits = 0;
        glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
        if (depth_bits) {
            json.beginMember("GL_DEPTH_COMPONENT");
            dumpReadBufferImage(json, width, height, GL_DEPTH_COMPONENT);
            json.endMember();
        }

        GLint stencil_bits = 0;
        glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
        if (stencil_bits) {
            json.beginMember("GL_STENCIL_INDEX");
            dumpReadBufferImage(json, width, height, GL_STENCIL_INDEX);
            json.endMember();
        }
    }
}


/**
 * Dump the specified framebuffer attachment.
 *
 * In the case of a color attachment, it assumes it is already bound for read.
 */
static void
dumpFramebufferAttachment(JSONWriter &json, GLenum target, GLenum attachment, GLenum format)
{
    GLint width = 0, height = 0;
    if (!getFramebufferAttachmentSize(target, attachment, &width, &height)) {
        return;
    }

    GLint internalFormat = getFramebufferAttachmentFormat(target, attachment);

    json.beginMember(enumToString(attachment));
    dumpReadBufferImage(json, width, height, format, internalFormat);
    json.endMember();
}


static void
dumpFramebufferAttachments(JSONWriter &json, Context &context, GLenum target)
{
    GLint read_framebuffer = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);

    GLint read_buffer = GL_NONE;
    glGetIntegerv(GL_READ_BUFFER, &read_buffer);

    GLint max_draw_buffers = 1;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
    GLint max_color_attachments = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);

    for (GLint i = 0; i < max_draw_buffers; ++i) {
        GLint draw_buffer = GL_NONE;
        glGetIntegerv(GL_DRAW_BUFFER0 + i, &draw_buffer);
        if (draw_buffer != GL_NONE) {
            glReadBuffer(draw_buffer);
            GLint attachment;
            if (draw_buffer >= GL_COLOR_ATTACHMENT0 && draw_buffer < GL_COLOR_ATTACHMENT0 + max_color_attachments) {
                attachment = draw_buffer;
            } else {
                std::cerr << "warning: unexpected GL_DRAW_BUFFER" << i << " = " << draw_buffer << "\n";
                attachment = GL_COLOR_ATTACHMENT0;
            }
            GLint alpha_size = 0;
            glGetFramebufferAttachmentParameteriv(target, attachment,
                                                  GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
                                                  &alpha_size);
            GLenum format = alpha_size ? GL_RGBA : GL_RGB;
            dumpFramebufferAttachment(json, target, attachment, format);
        }
    }

    glReadBuffer(read_buffer);

    if (!context.ES) {
        dumpFramebufferAttachment(json, target, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
        dumpFramebufferAttachment(json, target, GL_STENCIL_ATTACHMENT, GL_STENCIL_INDEX);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
}


void
dumpFramebuffer(JSONWriter &json, Context &context)
{
    json.beginMember("framebuffer");
    json.beginObject();

    GLint boundDrawFbo = 0, boundReadFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundDrawFbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &boundReadFbo);
    if (!boundDrawFbo) {
        dumpDrawableImages(json, context);
    } else if (context.ES) {
        dumpFramebufferAttachments(json, context, GL_FRAMEBUFFER);
    } else {
        GLint colorRb = 0, stencilRb = 0, depthRb = 0;
        GLint draw_buffer0 = GL_NONE;
        glGetIntegerv(GL_DRAW_BUFFER0, &draw_buffer0);
        bool multisample = false;

        GLint boundRb = 0;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &boundRb);

        GLint object_type;
        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, draw_buffer0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &object_type);
        if (object_type == GL_RENDERBUFFER) {
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, draw_buffer0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &colorRb);
            glBindRenderbuffer(GL_RENDERBUFFER, colorRb);
            GLint samples = 0;
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
            if (samples) {
                multisample = true;
            }
        }

        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &object_type);
        if (object_type == GL_RENDERBUFFER) {
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthRb);
            glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
            GLint samples = 0;
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
            if (samples) {
                multisample = true;
            }
        }

        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &object_type);
        if (object_type == GL_RENDERBUFFER) {
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &stencilRb);
            glBindRenderbuffer(GL_RENDERBUFFER, stencilRb);
            GLint samples = 0;
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
            if (samples) {
                multisample = true;
            }
        }

        glBindRenderbuffer(GL_RENDERBUFFER, boundRb);

        GLuint rbs[3];
        GLint numRbs = 0;
        GLuint fboCopy = 0;

        if (multisample) {
            // glReadPixels doesnt support multisampled buffers so we need
            // to blit the fbo to a temporary one
            fboCopy = downsampledFramebuffer(boundDrawFbo, draw_buffer0,
                                             colorRb, depthRb, stencilRb,
                                             rbs, &numRbs);
        }

        dumpFramebufferAttachments(json, context, GL_DRAW_FRAMEBUFFER);

        if (multisample) {
            glBindRenderbuffer(GL_RENDERBUFFER_BINDING, boundRb);
            glDeleteRenderbuffers(numRbs, rbs);
            glDeleteFramebuffers(1, &fboCopy);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, boundReadFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundDrawFbo);
    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace glstate */

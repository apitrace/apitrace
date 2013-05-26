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
#include <string.h>

#include <algorithm>
#include <iostream>

#include "image.hpp"
#include "json.hpp"
#include "glproc.hpp"
#include "glsize.hpp"
#include "glstate.hpp"
#include "glstate_internal.hpp"


#ifdef __linux__
#include <dlfcn.h>
#endif

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


struct ImageDesc
{
    GLint width;
    GLint height;
    GLint depth;
    GLint samples;
    GLint internalFormat;

    inline
    ImageDesc() :
        width(0),
        height(0),
        depth(0),
        samples(0),
        internalFormat(GL_NONE)
    {}

    inline bool
    operator == (const ImageDesc &other) const {
        return width == other.width &&
               height == other.height &&
               depth == other.depth &&
               samples == other.samples &&
               internalFormat == other.internalFormat;
    }

    inline bool
    valid(void) const {
        return width > 0 && height > 0 && depth > 0;
    }
};


/**
 * Sames as enumToString, but with special provision to handle formatsLUMINANCE_ALPHA.
 *
 * OpenGL 2.1 specification states that "internalFormat may (for backwards
 * compatibility with the 1.0 version of the GL) also take on the integer
 * values 1, 2, 3, and 4, which are equivalent to symbolic constants LUMINANCE,
 * LUMINANCE ALPHA, RGB, and RGBA respectively". 
 */
const char *
formatToString(GLenum internalFormat) {
    switch (internalFormat) {
    case 1:
        return "GL_LUMINANCE";
    case 2:
        return "GL_LUMINANCE_ALPHA";
    case 3:
        return "GL_RGB";
    case 4:
        return "GL_RGBA";
    default:
        return enumToString(internalFormat);
    }
}


/**
 * OpenGL ES does not support glGetTexLevelParameteriv, but it is possible to
 * probe whether a texture has a given size by crafting a dummy glTexSubImage()
 * call.
 */
static bool
probeTextureLevelSizeOES(GLenum target, GLint level, const GLint size[3]) {
    while (glGetError() != GL_NO_ERROR)
        ;

    GLenum internalFormat = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLint dummy = 0;

    switch (target) {
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        glTexSubImage2D(target, level, size[0], size[1], 0, 0, internalFormat, type, &dummy);
        break;
    case GL_TEXTURE_3D_OES:
        glTexSubImage3DOES(target, level, size[0], size[1], size[2], 0, 0, 0, internalFormat, type, &dummy);
    default:
        assert(0);
        return false;
    }

    GLenum error = glGetError();

    if (0) {
        std::cerr << "(" << size[0] << ", " << size[1] << ", " << size[2] << ") = " << enumToString(error) << "\n";
    }

    if (error == GL_NO_ERROR) {
        return true;
    }

    while (glGetError() != GL_NO_ERROR)
        ;

    return false;
}


/**
 * Bisect the texture size along an axis.
 *
 * It is assumed that the texture exists.
 */
static GLint
bisectTextureLevelSizeOES(GLenum target, GLint level, GLint axis, GLint max) {
    GLint size[3] = {0, 0, 0};

    assert(axis < 3);
    assert(max >= 0);

    GLint min = 0;
    while (true) {
        GLint test = (min + max) / 2;
        if (test == min) {
            return min;
        }

        size[axis] = test;

        if (probeTextureLevelSizeOES(target, level, size)) {
            min = test;
        } else {
            max = test;
        }
    }
}


/**
 * Special path to obtain texture size on OpenGL ES, that does not rely on
 * glGetTexLevelParameteriv
 */
static bool
getActiveTextureLevelDescOES(Context &context, GLenum target, GLint level, ImageDesc &desc)
{
    if (target == GL_TEXTURE_1D) {
        // OpenGL ES does not support 1D textures
        return false;
    }

    const GLint size[3] = {1, 1, 1}; 
    if (!probeTextureLevelSizeOES(target, level, size)) {
        return false;
    }

    // XXX: mere guess
    desc.internalFormat = GL_RGBA;

    GLint maxSize = 0;
    switch (target) {
    case GL_TEXTURE_2D:
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
        desc.width = bisectTextureLevelSizeOES(target, level, 0, maxSize);
        desc.height = bisectTextureLevelSizeOES(target, level, 1, maxSize);
        desc.depth = 1;
        break;
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxSize);
        desc.width = bisectTextureLevelSizeOES(target, level, 0, maxSize);
        desc.height = desc.width;
        desc.depth = 1;
        break;
    case GL_TEXTURE_3D_OES:
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE_OES, &maxSize);
        desc.width = bisectTextureLevelSizeOES(target, level, 0, maxSize);
        desc.width = bisectTextureLevelSizeOES(target, level, 1, maxSize);
        desc.depth = bisectTextureLevelSizeOES(target, level, 2, maxSize);
        break;
    default:
        return false;
    }

    if (0) {
        std::cerr
            << enumToString(target) << " "
            << level << " "
            << desc.width << "x" << desc.height << "x" << desc.depth
            << "\n";
    }

    return desc.valid();
}


static inline bool
getActiveTextureLevelDesc(Context &context, GLenum target, GLint level, ImageDesc &desc)
{
    assert(target != GL_TEXTURE_CUBE_MAP);

    if (context.ES) {
        return getActiveTextureLevelDescOES(context, target, level, desc);
    }

    glGetTexLevelParameteriv(target, level, GL_TEXTURE_INTERNAL_FORMAT, &desc.internalFormat);

    desc.width = 0;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &desc.width);

    if (target == GL_TEXTURE_1D) {
        desc.height = 1;
        desc.depth = 1;
    } else {
        desc.height = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &desc.height);
        if (target != GL_TEXTURE_3D) {
            desc.depth = 1;
        } else {
            desc.depth = 0;
            glGetTexLevelParameteriv(target, level, GL_TEXTURE_DEPTH, &desc.depth);
        }
    }

    glGetTexLevelParameteriv(target, level, GL_TEXTURE_SAMPLES, &desc.samples);

    return desc.valid();
}


static GLenum
getTextureBinding(GLenum target)
{
    switch (target) {
    case GL_TEXTURE_1D:
        return GL_TEXTURE_BINDING_1D;
    case GL_TEXTURE_1D_ARRAY:
        return GL_TEXTURE_BINDING_1D_ARRAY;
    case GL_TEXTURE_2D:
        return GL_TEXTURE_BINDING_2D;
    case GL_TEXTURE_2D_MULTISAMPLE:
        return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
    case GL_TEXTURE_2D_ARRAY:
        return GL_TEXTURE_BINDING_2D_ARRAY;
    case GL_TEXTURE_RECTANGLE:
        return GL_TEXTURE_BINDING_RECTANGLE;
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        return GL_TEXTURE_BINDING_CUBE_MAP;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
    case GL_TEXTURE_3D:
        return GL_TEXTURE_BINDING_3D;
    default:
        assert(false);
        return GL_NONE;
    }
}


/**
 * OpenGL ES does not support glGetTexImage. Obtain the pixels by attaching the
 * texture to a framebuffer.
 */
static inline void
getTexImageOES(GLenum target, GLint level, ImageDesc &desc, GLubyte *pixels)
{
    memset(pixels, 0x80, desc.height * desc.width * 4);

    GLenum texture_binding = getTextureBinding(target);
    if (texture_binding == GL_NONE) {
        return;
    }

    GLint texture = 0;
    glGetIntegerv(texture_binding, &texture);
    if (!texture) {
        return;
    }

    GLint prev_fbo = 0;
    GLuint fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLenum status;

    switch (target) {
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, level);
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << __FUNCTION__ << ": " << enumToString(status) << "\n";
        }
        glReadPixels(0, 0, desc.width, desc.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        break;
    case GL_TEXTURE_3D_OES:
        for (int i = 0; i < desc.depth; i++) {
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texture, level, i);
            glReadPixels(0, 0, desc.width, desc.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels + 4 * i * desc.width * desc.height);
        }
        break;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);

    glDeleteFramebuffers(1, &fbo);
}


static inline GLboolean
isDepthFormat(GLenum internalFormat)
{
   switch (internalFormat) {
   case GL_DEPTH_COMPONENT:
   case GL_DEPTH_COMPONENT16:
   case GL_DEPTH_COMPONENT24:
   case GL_DEPTH_COMPONENT32:
   case GL_DEPTH_COMPONENT32F:
   case GL_DEPTH_COMPONENT32F_NV:
   case GL_DEPTH_STENCIL:
   case GL_DEPTH24_STENCIL8:
   case GL_DEPTH32F_STENCIL8:
   case GL_DEPTH32F_STENCIL8_NV:
      return GL_TRUE;
   }
   return GL_FALSE;
}


static inline void
dumpActiveTextureLevel(JSONWriter &json, Context &context, GLenum target, GLint level)
{
    ImageDesc desc;
    if (!getActiveTextureLevelDesc(context, target, level, desc)) {
        return;
    }

    char label[512];
    GLint active_texture = GL_TEXTURE0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);
    snprintf(label, sizeof label, "%s, %s, level = %d",
             enumToString(active_texture), enumToString(target), level);

    json.beginMember(label);

    GLuint channels;
    GLenum format;
    if (!context.ES && isDepthFormat(desc.internalFormat)) {
       format = GL_DEPTH_COMPONENT;
       channels = 1;
    } else {
       format = GL_RGBA;
       channels = 4;
    }

    image::Image *image = new image::Image(desc.width, desc.height*desc.depth, channels, true);

    context.resetPixelPackState();

    if (context.ES) {
        getTexImageOES(target, level, desc, image->pixels);
    } else {
        glGetTexImage(target, level, format, GL_UNSIGNED_BYTE, image->pixels);
    }

    context.restorePixelPackState();

    json.writeImage(image, formatToString(desc.internalFormat), desc.depth);

    delete image;

    json.endMember(); // label
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
        ImageDesc desc;

        if (target == GL_TEXTURE_CUBE_MAP) {
            for (int face = 0; face < 6; ++face) {
                if (!getActiveTextureLevelDesc(context, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, desc)) {
                    return;
                }
                dumpActiveTextureLevel(json, context, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level);
            }
        } else {
            if (!getActiveTextureLevelDesc(context, target, level, desc)) {
                return;
            }
            dumpActiveTextureLevel(json, context, target, level);
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

    /*
     * At least the Android software GL implementation doesn't return the
     * proper value for this, but rather returns 0. The GL(ES) specification
     * mandates a minimum value of 2, so use this as a fall-back value.
     */
    max_units = std::max(max_units, 2);

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
#if defined(__linux__)
    if (dlsym(RTLD_DEFAULT, "eglGetCurrentContext")) {
        EGLContext currentContext = eglGetCurrentContext();
        if (currentContext != EGL_NO_CONTEXT) {
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
        }
    }
#endif

#if defined(_WIN32)

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


struct TextureTargetBinding
{
    GLenum target;
    GLenum binding;
};


static const GLenum
textureTargets[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_RECTANGLE,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_3D,
    GL_TEXTURE_2D_MULTISAMPLE,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_CUBE_MAP_ARRAY,
};


static GLenum
getTextureTarget(GLint texture)
{
    if (!glIsTexture(texture)) {
        return GL_NONE;
    }

    for (unsigned i = 0; i < sizeof(textureTargets)/sizeof(textureTargets[0]); ++i) {
        GLenum target = textureTargets[i];
        GLenum binding = getTextureBinding(target);

        while (glGetError() != GL_NO_ERROR)
            ;

        GLint bound_texture = 0;
        glGetIntegerv(binding, &bound_texture);
        glBindTexture(target, texture);

        bool succeeded = glGetError() == GL_NO_ERROR;

        glBindTexture(target, bound_texture);

        if (succeeded) {
            return target;
        }
    }

    return GL_NONE;
}


static bool
getBoundRenderbufferDesc(Context &context, ImageDesc &desc)
{
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &desc.width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &desc.height);
    desc.depth = 1;
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &desc.samples);

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &desc.internalFormat);
    
    return desc.valid();
}


static bool
getRenderbufferDesc(Context &context, GLint renderbuffer, ImageDesc &desc)
{
    GLint bound_renderbuffer = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &bound_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

    getBoundRenderbufferDesc(context, desc);

    glBindRenderbuffer(GL_RENDERBUFFER, bound_renderbuffer);
    
    return desc.valid();
}


static bool
getFramebufferAttachmentDesc(Context &context, GLenum target, GLenum attachment, ImageDesc &desc)
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
        return getRenderbufferDesc(context, object_name, desc);
    } else if (object_type == GL_TEXTURE) {
        GLint texture_face = 0;
        glGetFramebufferAttachmentParameteriv(target, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
                                              &texture_face);

        GLint texture_level = 0;
        glGetFramebufferAttachmentParameteriv(target, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
                                              &texture_level);

        GLint bound_texture = 0;
        if (texture_face != 0) {
            glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound_texture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, object_name);
            getActiveTextureLevelDesc(context, texture_face, texture_level, desc);
            glBindTexture(GL_TEXTURE_CUBE_MAP, bound_texture);
        } else {
            GLenum texture_target = getTextureTarget(object_name);
            GLenum texture_binding = getTextureBinding(texture_target);
            glGetIntegerv(texture_binding, &bound_texture);
            glBindTexture(texture_target, object_name);
            getActiveTextureLevelDesc(context, texture_target, texture_level, desc);
            glBindTexture(texture_target, bound_texture);
        }

        return desc.valid();
    } else {
        std::cerr << "warning: unexpected GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = " << object_type << "\n";
        return false;
    }
}



image::Image *
getDrawBufferImage() {
    GLenum format = GL_RGB;
    GLint channels = _gl_format_channels(format);
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
    ImageDesc desc;
    if (draw_framebuffer) {
        if (context.ARB_draw_buffers) {
            glGetIntegerv(GL_DRAW_BUFFER0, &draw_buffer);
            if (draw_buffer == GL_NONE) {
                return NULL;
            }
        } else {
            // GL_COLOR_ATTACHMENT0 is implied
            draw_buffer = GL_COLOR_ATTACHMENT0;
        }

        if (!getFramebufferAttachmentDesc(context, framebuffer_target, draw_buffer, desc)) {
            return NULL;
        }
    } else {
        if (context.ES) {
            // XXX: Draw buffer is always FRONT for single buffer context, BACK
            // for double buffered contexts. There is no way to know which (as
            // GL_DOUBLEBUFFER state is also unavailable), so always assume
            // double-buffering.
            draw_buffer = GL_BACK;
        } else {
            glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
            if (draw_buffer == GL_NONE) {
                return NULL;
            }
        }

        if (!getDrawableBounds(&desc.width, &desc.height)) {
            return NULL;
        }

        desc.depth = 1;
    }

    GLenum type = GL_UNSIGNED_BYTE;

#if DEPTH_AS_RGBA
    if (format == GL_DEPTH_COMPONENT) {
        type = GL_UNSIGNED_INT;
        channels = 4;
    }
#endif

    image::Image *image = new image::Image(desc.width, desc.height, channels, true);
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

    glReadPixels(0, 0, desc.width, desc.height, format, type, image->pixels);

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
    GLint channels = _gl_format_channels(format);

    if (internalFormat == GL_NONE) {
        internalFormat = format;
    }

    Context context;

    GLenum type = GL_UNSIGNED_BYTE;

#if DEPTH_AS_RGBA
    if (format == GL_DEPTH_COMPONENT) {
        type = GL_UNSIGNED_INT;
        channels = 4;
    }
#endif

    image::Image *image = new image::Image(width, height, channels, true);

    while (glGetError() != GL_NO_ERROR) {}

    // TODO: reset imaging state too
    context.resetPixelPackState();

    glReadPixels(0, 0, width, height, format, type, image->pixels);

    context.restorePixelPackState();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        do {
            std::cerr << "warning: " << enumToString(error) << " while reading framebuffer\n";
            error = glGetError();
        } while(error != GL_NO_ERROR);
        json.writeNull();
    } else {
        json.writeImage(image, formatToString(internalFormat));
    }

    delete image;
}


static inline GLuint
downsampledFramebuffer(Context &context,
                       GLuint oldFbo, GLint drawbuffer,
                       const ImageDesc &colorDesc,
                       const ImageDesc &depthDesc,
                       const ImageDesc &stencilDesc,
                       GLuint *rbs, GLint *numRbs)
{
    GLuint fbo;


    *numRbs = 0;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    {
        // color buffer
        glGenRenderbuffers(1, &rbs[*numRbs]);
        glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
        glRenderbufferStorage(GL_RENDERBUFFER, colorDesc.internalFormat, colorDesc.width, colorDesc.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, drawbuffer,
                                  GL_RENDERBUFFER, rbs[*numRbs]);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glDrawBuffer(drawbuffer);
        glReadBuffer(drawbuffer);
        glBlitFramebuffer(0, 0, colorDesc.width, colorDesc.height, 0, 0, colorDesc.width, colorDesc.height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        ++*numRbs;
    }

    if (stencilDesc == depthDesc &&
        depthDesc.valid()) {
        //combined depth and stencil buffer
        glGenRenderbuffers(1, &rbs[*numRbs]);
        glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
        glRenderbufferStorage(GL_RENDERBUFFER, depthDesc.internalFormat, depthDesc.width, depthDesc.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbs[*numRbs]);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, depthDesc.width, depthDesc.height, 0, 0, depthDesc.width, depthDesc.height,
                          GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        ++*numRbs;
    } else {
        if (depthDesc.valid()) {
            glGenRenderbuffers(1, &rbs[*numRbs]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
            glRenderbufferStorage(GL_RENDERBUFFER, depthDesc.internalFormat, depthDesc.width, depthDesc.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER, rbs[*numRbs]);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glDrawBuffer(GL_DEPTH_ATTACHMENT);
            glReadBuffer(GL_DEPTH_ATTACHMENT);
            glBlitFramebuffer(0, 0, depthDesc.width, depthDesc.height, 0, 0, depthDesc.width, depthDesc.height,
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            ++*numRbs;
        }
        if (stencilDesc.valid()) {
            glGenRenderbuffers(1, &rbs[*numRbs]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
            glRenderbufferStorage(GL_RENDERBUFFER, stencilDesc.internalFormat, stencilDesc.width, stencilDesc.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, rbs[*numRbs]);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glDrawBuffer(GL_STENCIL_ATTACHMENT);
            glReadBuffer(GL_STENCIL_ATTACHMENT);
            glBlitFramebuffer(0, 0, stencilDesc.width, stencilDesc.height, 0, 0, stencilDesc.width, stencilDesc.height,
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
        // XXX: Draw buffer is always FRONT for single buffer context, BACK for
        // double buffered contexts. There is no way to know which (as
        // GL_DOUBLEBUFFER state is also unavailable), so always assume
        // double-buffering.
        draw_buffer = GL_BACK;
    } else {
        glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
    }

    if (draw_buffer != GL_NONE) {
        // Read from current draw buffer
        GLint read_buffer = GL_NONE;
        if (!context.ES) {
            glGetIntegerv(GL_READ_BUFFER, &read_buffer);
            glReadBuffer(draw_buffer);
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

        // Restore original read buffer
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
dumpFramebufferAttachment(JSONWriter &json, Context &context, GLenum target, GLenum attachment, GLenum format)
{
    ImageDesc desc;
    if (!getFramebufferAttachmentDesc(context, target, attachment, desc)) {
        return;
    }

    assert(desc.samples == 0);

    json.beginMember(enumToString(attachment));
    dumpReadBufferImage(json, desc.width, desc.height, format, desc.internalFormat);
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
            dumpFramebufferAttachment(json, context, target, attachment, format);
        }
    }

    glReadBuffer(read_buffer);

    if (!context.ES) {
        dumpFramebufferAttachment(json, context, target, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
        dumpFramebufferAttachment(json, context, target, GL_STENCIL_ATTACHMENT, GL_STENCIL_INDEX);
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
        GLint draw_buffer0 = GL_NONE;
        glGetIntegerv(GL_DRAW_BUFFER0, &draw_buffer0);
        bool multisample = false;

        GLint boundRb = 0;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &boundRb);

        ImageDesc colorDesc;
        if (getFramebufferAttachmentDesc(context, GL_DRAW_FRAMEBUFFER, draw_buffer0, colorDesc)) {
            if (colorDesc.samples) {
                multisample = true;
            }
        }

        ImageDesc depthDesc;
        if (getFramebufferAttachmentDesc(context, GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthDesc)) {
            if (depthDesc.samples) {
                multisample = true;
            }
        }

        ImageDesc stencilDesc;
        if (getFramebufferAttachmentDesc(context, GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, stencilDesc)) {
            if (stencilDesc.samples) {
                multisample = true;
            }
        }

        GLuint rbs[3];
        GLint numRbs = 0;
        GLuint fboCopy = 0;

        if (multisample) {
            // glReadPixels doesnt support multisampled buffers so we need
            // to blit the fbo to a temporary one
            fboCopy = downsampledFramebuffer(context,
                                             boundDrawFbo, draw_buffer0,
                                             colorDesc, depthDesc, stencilDesc,
                                             rbs, &numRbs);
        } else {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, boundDrawFbo);
        }

        dumpFramebufferAttachments(json, context, GL_READ_FRAMEBUFFER);

        if (multisample) {
            glBindRenderbuffer(GL_RENDERBUFFER, boundRb);
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

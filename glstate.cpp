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
#include <iostream>
#include <algorithm>

#include "image.hpp"
#include "json.hpp"
#include "glproc.hpp"
#include "glsize.hpp"
#include "glstate.hpp"


#ifdef __APPLE__

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int CGSConnectionID;
typedef int CGSWindowID;
typedef int CGSSurfaceID;

CGLError CGLGetSurface(CGLContextObj, CGSConnectionID*, CGSWindowID*, CGSSurfaceID*);
OSStatus CGSGetSurfaceBounds(CGSConnectionID, CGWindowID, CGSSurfaceID, CGRect *);

#ifdef __cplusplus
}
#endif

#endif /* __APPLE__ */


namespace glstate {


static void
dumpShader(JSONWriter &json, GLuint shader)
{
    if (!shader) {
        return;
    }

    GLint shader_type = 0;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);
    if (!shader_type) {
        return;
    }

    GLint source_length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);
    if (!source_length) {
        return;
    }

    GLchar *source = new GLchar[source_length];
    GLsizei length = 0;
    source[0] = 0;
    glGetShaderSource(shader, source_length, &length, source);

    json.beginMember(enumToString(shader_type));
    json.writeString(source);
    json.endMember();

    delete [] source;
}


static void
dumpShaderObj(JSONWriter &json, GLhandleARB shaderObj)
{
    if (!shaderObj) {
        return;
    }

    GLint shader_type = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_TYPE_ARB, &shader_type);
    if (!shader_type) {
        return;
    }

    GLint source_length = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &source_length);
    if (!source_length) {
        return;
    }

    GLcharARB *source = new GLcharARB[source_length];
    GLsizei length = 0;
    source[0] = 0;
    glGetShaderSource(shaderObj, source_length, &length, source);

    json.beginMember(enumToString(shader_type));
    json.writeString(source);
    json.endMember();

    delete [] source;
}


static inline void
dumpCurrentProgram(JSONWriter &json)
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    if (!program) {
        return;
    }

    GLint attached_shaders = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    GLuint *shaders = new GLuint[attached_shaders];
    GLsizei count = 0;
    glGetAttachedShaders(program, attached_shaders, &count, shaders);
    for (GLsizei i = 0; i < count; ++ i) {
       dumpShader(json, shaders[i]);
    }
    delete [] shaders;
}


static inline void
dumpCurrentProgramObj(JSONWriter &json)
{
    GLhandleARB programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
    if (!programObj) {
        return;
    }

    GLint attached_shaders = 0;
    glGetProgramivARB(programObj, GL_OBJECT_ATTACHED_OBJECTS_ARB, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    GLhandleARB *shaderObjs = new GLhandleARB[attached_shaders];
    GLsizei count = 0;
    glGetAttachedObjectsARB(programObj, attached_shaders, &count, shaderObjs);
    for (GLsizei i = 0; i < count; ++ i) {
       dumpShaderObj(json, shaderObjs[i]);
    }
    delete [] shaderObjs;
}


static inline void
dumpArbProgram(JSONWriter &json, GLenum target)
{
    if (!glIsEnabled(target)) {
        return;
    }

    GLint program_length = 0;
    glGetProgramivARB(target, GL_PROGRAM_LENGTH_ARB, &program_length);
    if (!program_length) {
        return;
    }

    GLchar *source = new GLchar[program_length + 1];
    source[0] = 0;
    glGetProgramStringARB(target, GL_PROGRAM_STRING_ARB, source);
    source[program_length] = 0;

    json.beginMember(enumToString(target));
    json.writeString(source);
    json.endMember();

    delete [] source;
}


static inline void
dumpShaders(JSONWriter &json)
{
    json.beginMember("shaders");
    json.beginObject();
    dumpCurrentProgram(json);
    dumpArbProgram(json, GL_FRAGMENT_PROGRAM_ARB);
    dumpArbProgram(json, GL_VERTEX_PROGRAM_ARB);
    json.endObject();
    json.endMember(); //shaders
}


static inline void
dumpTextureImage(JSONWriter &json, GLenum target, GLint level)
{
    GLint width, height = 1, depth = 1;

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
        snprintf(label, sizeof label, "%s, %s, level = %i", enumToString(active_texture), enumToString(target), level);

        json.beginMember(label);

        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", depth);

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", 4);

        GLubyte *pixels = new GLubyte[depth*width*height*4];

        glGetTexImage(target, level, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        json.beginMember("__data__");
        char *pngBuffer;
        int pngBufferSize;
        Image::writePixelsToBuffer(pixels, width, height, 4, false, &pngBuffer, &pngBufferSize);
        json.writeBase64(pngBuffer, pngBufferSize);
        free(pngBuffer);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
}


static inline void
dumpTexture(JSONWriter &json, GLenum target, GLenum binding)
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
                dumpTextureImage(json, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level);
            }
        } else {
            dumpTextureImage(json, target, level);
        }

        ++level;
    } while(true);
}


static inline void
dumpTextures(JSONWriter &json)
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
        dumpTexture(json, GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D);
        dumpTexture(json, GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D);
        dumpTexture(json, GL_TEXTURE_3D, GL_TEXTURE_BINDING_3D);
        dumpTexture(json, GL_TEXTURE_RECTANGLE, GL_TEXTURE_BINDING_RECTANGLE);
        dumpTexture(json, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP);
    }
    glActiveTexture(active_texture);
    json.endObject();
    json.endMember(); // textures
}


static bool
getDrawableBounds(GLint *width, GLint *height) {
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

#else

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

#endif

    return true;
}


static inline void
dumpDrawBufferImage(JSONWriter &json, GLenum format)
{
    GLint channels = __gl_format_channels(format);

    GLint width, height;

    if (!getDrawableBounds(&width, &height)) {
        json.writeNull();
    } else {
        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", 1);

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", channels);

        GLubyte *pixels = new GLubyte[width*height*channels];

        GLint drawbuffer = GL_NONE;
        GLint readbuffer = GL_NONE;
        glGetIntegerv(GL_DRAW_BUFFER, &drawbuffer);
        glGetIntegerv(GL_READ_BUFFER, &readbuffer);
        glReadBuffer(drawbuffer);

        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
        glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_SKIP_IMAGES, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);

        glPopClientAttrib();
        glReadBuffer(readbuffer);

        json.beginMember("__data__");
        char *pngBuffer;
        int pngBufferSize;
        Image::writePixelsToBuffer(pixels, width, height, channels, false, &pngBuffer, &pngBufferSize);
        //std::cerr <<" Before = "<<(width * height * channels * sizeof *pixels)
        //          <<", after = "<<pngBufferSize << ", ratio = " << double(width * height * channels * sizeof *pixels)/pngBufferSize;
        json.writeBase64(pngBuffer, pngBufferSize);
        free(pngBuffer);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
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


static void
dumpDrawBuffers(JSONWriter &json, GLboolean dumpDepth, GLboolean dumpStencil)
{
    json.beginMember("GL_RGBA");
    dumpDrawBufferImage(json, GL_RGBA);
    json.endMember();

    if (dumpDepth) {
        json.beginMember("GL_DEPTH_COMPONENT");
        dumpDrawBufferImage(json, GL_DEPTH_COMPONENT);
        json.endMember();
    }

    if (dumpStencil) {
        json.beginMember("GL_STENCIL_INDEX");
        dumpDrawBufferImage(json, GL_STENCIL_INDEX);
        json.endMember();
    }
}


static void
dumpFramebuffer(JSONWriter &json)
{
    json.beginMember("framebuffer");
    json.beginObject();

    GLint boundDrawFbo = 0, boundReadFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundDrawFbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &boundReadFbo);
    if (!boundDrawFbo) {
        GLint depth_bits = 0;
        glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
        GLint stencil_bits = 0;
        glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
        dumpDrawBuffers(json, depth_bits, stencil_bits);
    } else {
        GLint colorRb, stencilRb, depthRb;
        GLint boundRb;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &boundRb);
        GLint drawbuffer = GL_NONE;
        glGetIntegerv(GL_DRAW_BUFFER, &drawbuffer);

        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                              drawbuffer,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &colorRb);
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                              GL_DEPTH_ATTACHMENT,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &depthRb);
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                              GL_STENCIL_ATTACHMENT,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &stencilRb);

        GLint colorSamples, depthSamples, stencilSamples;
        GLuint rbs[3];
        GLint numRbs = 0;
        GLuint fboCopy = 0;
        glBindRenderbuffer(GL_RENDERBUFFER, colorRb);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_SAMPLES, &colorSamples);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_SAMPLES, &depthSamples);
        glBindRenderbuffer(GL_RENDERBUFFER, stencilRb);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_SAMPLES, &stencilSamples);
        glBindRenderbuffer(GL_RENDERBUFFER, boundRb);

        if (colorSamples || depthSamples || stencilSamples) {
            //glReadPixels doesnt support multisampled buffers so we need
            // to blit the fbo to a temporary one
            fboCopy = downsampledFramebuffer(boundDrawFbo, drawbuffer,
                                             colorRb, depthRb, stencilRb,
                                             rbs, &numRbs);
        }
        glDrawBuffer(drawbuffer);
        glReadBuffer(drawbuffer);

        dumpDrawBuffers(json, depthRb, stencilRb);

        if (fboCopy) {
            glBindFramebuffer(GL_FRAMEBUFFER, boundDrawFbo);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, boundReadFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundDrawFbo);
            glBindRenderbuffer(GL_RENDERBUFFER_BINDING, boundRb);
            glDeleteRenderbuffers(numRbs, rbs);
            glDeleteFramebuffers(1, &fboCopy);
        }
    }

    json.endObject();
    json.endMember(); // framebuffer
}


void dumpCurrentContext(std::ostream &os)
{
    JSONWriter json(os);
    dumpParameters(json);
    dumpShaders(json);
    dumpTextures(json);
    dumpFramebuffer(json);
}


} /* namespace glstate */

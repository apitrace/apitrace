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


#include "glretrace_wgl.hpp"

#include "glproc.hpp"
#include "retrace.hpp"
#include "glretrace.hpp"


using namespace glretrace;


typedef std::map<unsigned long long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, Context *> ContextMap;
static DrawableMap drawable_map;
static DrawableMap pbuffer_map;
static ContextMap context_map;


static glws::Drawable *
getDrawable(unsigned long long hdc) {
    if (hdc == 0) {
        return NULL;
    }

    DrawableMap::const_iterator it;
    it = drawable_map.find(hdc);
    if (it == drawable_map.end()) {
        return (drawable_map[hdc] = glretrace::createDrawable());
    }

    return it->second;
}

static Context *
getContext(unsigned long long context_ptr) {
    if (context_ptr == 0) {
        return NULL;
    }

    ContextMap::const_iterator it;
    it = context_map.find(context_ptr);
    if (it == context_map.end()) {
        assert(false);
        return NULL;
    }

    return it->second;
}

static void retrace_wglCreateContext(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *context = glretrace::createContext();
    context_map[orig_context] = context;
}

static void retrace_wglDeleteContext(trace::Call &call) {
    unsigned long long hglrc = call.arg(0).toUIntPtr();

    ContextMap::iterator it;
    it = context_map.find(hglrc);
    if (it == context_map.end()) {
        return;
    }

    it->second->release();
    
    context_map.erase(it);
}

static void retrace_wglMakeCurrent(trace::Call &call) {
    bool ret = call.ret->toBool();

    glws::Drawable *new_drawable = NULL;
    Context *new_context = NULL;
    if (ret) {
        unsigned long long hglrc = call.arg(1).toUIntPtr();
        if (hglrc) {
            new_drawable = getDrawable(call.arg(0).toUIntPtr());
            new_context = getContext(hglrc);
        }
    }

    glretrace::makeCurrent(call, new_drawable, new_context);
}

static void retrace_wglSwapBuffers(trace::Call &call) {
    bool ret = call.ret->toBool();
    if (!ret) {
        return;
    }

    glws::Drawable *drawable = getDrawable(call.arg(0).toUIntPtr());

    frame_complete(call);
    if (retrace::doubleBuffer) {
        if (drawable) {
            drawable->swapBuffers();
        } else {
            glretrace::Context *currentContext = glretrace::getCurrentContext();
            if (currentContext) {
                currentContext->drawable->swapBuffers();
            }
        }
    } else {
        glFlush();
    }
}

static void retrace_wglShareLists(trace::Call &call) {
    bool ret = call.ret->toBool();
    if (!ret) {
        return;
    }

    unsigned long long hglrc1 = call.arg(0).toUIntPtr();
    unsigned long long hglrc2 = call.arg(1).toUIntPtr();

    Context *share_context = getContext(hglrc1);
    Context *old_context = getContext(hglrc2);

    glfeatures::Profile profile = old_context->profile();
    Context *new_context = glretrace::createContext(share_context, profile);
    if (new_context) {
        glretrace::Context *currentContext = glretrace::getCurrentContext();
        if (currentContext == old_context) {
            glretrace::makeCurrent(call, currentContext->drawable, new_context);
        }

        context_map[hglrc2] = new_context;
        
        old_context->release();
    }
}

static void retrace_wglCreateLayerContext(trace::Call &call) {
    retrace_wglCreateContext(call);
}

static void retrace_wglSwapLayerBuffers(trace::Call &call) {
    retrace_wglSwapBuffers(call);
}

#define WGL_BIND_TO_TEXTURE_RGB_ARB         0x2070
#define WGL_BIND_TO_TEXTURE_RGBA_ARB        0x2071
#define WGL_TEXTURE_FORMAT_ARB              0x2072
#define WGL_TEXTURE_TARGET_ARB              0x2073
#define WGL_MIPMAP_TEXTURE_ARB              0x2074
#define WGL_TEXTURE_RGB_ARB                 0x2075
#define WGL_TEXTURE_RGBA_ARB                0x2076
#define WGL_NO_TEXTURE_ARB                  0x2077
#define WGL_TEXTURE_CUBE_MAP_ARB            0x2078
#define WGL_TEXTURE_1D_ARB                  0x2079
#define WGL_TEXTURE_2D_ARB                  0x207A
#define WGL_NO_TEXTURE_ARB                  0x2077
#define WGL_MIPMAP_LEVEL_ARB                0x207B
#define WGL_CUBE_MAP_FACE_ARB               0x207C
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x207D
#define WGL_FRONT_LEFT_ARB                  0x2083
#define WGL_FRONT_RIGHT_ARB                 0x2084
#define WGL_BACK_LEFT_ARB                   0x2085
#define WGL_BACK_RIGHT_ARB                  0x2086
#define WGL_AUX0_ARB                        0x2087
#define WGL_AUX1_ARB                        0x2088
#define WGL_AUX2_ARB                        0x2089
#define WGL_AUX3_ARB                        0x208A
#define WGL_AUX4_ARB                        0x208B
#define WGL_AUX5_ARB                        0x208C
#define WGL_AUX6_ARB                        0x208D
#define WGL_AUX7_ARB                        0x208E
#define WGL_AUX8_ARB                        0x208F
#define WGL_AUX9_ARB                        0x2090

static void retrace_wglCreatePbufferARB(trace::Call &call) {
    unsigned long long orig_pbuffer = call.ret->toUIntPtr();
    if (!orig_pbuffer) {
        return;
    }

    int iWidth = call.arg(2).toUInt();
    int iHeight = call.arg(3).toUInt();
    const trace::Value *attribs = &call.arg(4);
    glws::pbuffer_info pbInfo = {0, 0, false};

    // XXX parse attrib list to populate pbInfo
    int k;

    k = parseAttrib(attribs, WGL_TEXTURE_FORMAT_ARB, WGL_NO_TEXTURE_ARB);
    switch (k) {
    case WGL_TEXTURE_RGB_ARB:
        pbInfo.texFormat = GL_RGB;
        break;
    case WGL_TEXTURE_RGBA_ARB:
        pbInfo.texFormat = GL_RGBA;
        break;
    case WGL_NO_TEXTURE_ARB:
        pbInfo.texFormat = GL_NONE;
        break;
    default:
        std::cerr << "error: invalid value for WGL_TEXTURE_FORMAT_ARB\n";
        pbInfo.texFormat = GL_NONE;
    }

    k = parseAttrib(attribs, WGL_TEXTURE_TARGET_ARB, WGL_NO_TEXTURE_ARB);
    switch (k) {
    case WGL_TEXTURE_CUBE_MAP_ARB:
        pbInfo.texTarget = GL_TEXTURE_CUBE_MAP;
        break;
    case WGL_TEXTURE_1D_ARB:
        pbInfo.texTarget = GL_TEXTURE_1D;
        break;
    case WGL_TEXTURE_2D_ARB:
        pbInfo.texTarget = GL_TEXTURE_2D;
        break;
    case WGL_NO_TEXTURE_ARB:
        pbInfo.texTarget = GL_NONE;
        break;
    default:
        std::cerr << "error: invalid value for WGL_TEXTURE_TARGET_ARB\n";
        pbInfo.texTarget = GL_NONE;
    }

    pbInfo.texMipmap = !!parseAttrib(attribs, WGL_MIPMAP_TEXTURE_ARB, 0);

    // WGL interface needs the HDC
    pbInfo.hdc_drawable = getDrawable(call.arg(0).toUInt());

    glws::Drawable *drawable = glretrace::createPbuffer(iWidth, iHeight,
                                                        &pbInfo);

    pbuffer_map[orig_pbuffer] = drawable;
}

static void retrace_wglGetPbufferDCARB(trace::Call &call) {
    unsigned long long orig_hdc = call.ret->toUIntPtr();
    if (!orig_hdc) {
        return;
    }

    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];

    drawable_map[orig_hdc] = pbuffer;
}

static void retrace_wglCreateContextAttribsARB(trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    if (!orig_context) {
        return;
    }

    Context *share_context = getContext(call.arg(1).toUIntPtr());

    const trace::Value * attribList = &call.arg(2);
    glfeatures::Profile profile = parseContextAttribList(attribList);

    Context *context = glretrace::createContext(share_context, profile);
    context_map[orig_context] = context;
}


static GLenum
wgl_buffer_to_enum(int iBuffer)
{
    switch (iBuffer) {
    case WGL_FRONT_LEFT_ARB:
        return GL_FRONT_LEFT;
    case WGL_BACK_LEFT_ARB:
        return GL_BACK_LEFT;
    case WGL_FRONT_RIGHT_ARB:
        return GL_FRONT_RIGHT;
    case WGL_BACK_RIGHT_ARB:
        return GL_BACK_RIGHT;
    case WGL_AUX0_ARB:
        return GL_AUX0;
    default:
        std::cerr << "error: invalid iBuffer in wgl_buffer_to_enum()\n";
        return GL_FRONT_LEFT;
    }
}

static void retrace_wglBindTexImageARB(trace::Call &call) {
    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];
    signed long long iBuffer = call.arg(1).toSInt();

    glretrace::bindTexImage(pbuffer, wgl_buffer_to_enum(iBuffer));
}

static void retrace_wglReleaseTexImageARB(trace::Call &call) {
    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];
    signed long long iBuffer = call.arg(1).toSInt();

    glretrace::releaseTexImage(pbuffer, wgl_buffer_to_enum(iBuffer));
}

static void retrace_wglSetPbufferAttribARB(trace::Call &call) {
    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];
    const trace::Value * attribList = &call.arg(1);

    // call the window system's setPbufferAttrib function.
    {
        int attribs[100], j = 0;
        const trace::Array *attribs_ = attribList ? attribList->toArray() : NULL;

        for (size_t i = 0; i + 1 < attribs_->values.size(); i += 2) {
            int param_i = attribs_->values[i]->toSInt();
            if (param_i == 0) {
                attribs[j] = 0;
            }

            attribs[j] = param_i;
            attribs[j+1] = attribs_->values[i+1]->toSInt();
        }

        glretrace::setPbufferAttrib(pbuffer, attribs);
    }

    if (!pbuffer || !attribList)
        return;

    // Update the glws::Drawable's fields
    const int undefined = -99999;
    int val;

    val = parseAttrib(attribList, WGL_MIPMAP_LEVEL_ARB, undefined);
    if (val != undefined) {
        pbuffer->mipmapLevel = val;
    }

    val = parseAttrib(attribList, WGL_CUBE_MAP_FACE_ARB, undefined);
    if (val != undefined) {
        // Drawable::cubeFace is integer in [0..5]
        val -= WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
        if (val < 0 || val > 5) {
            fprintf(stderr, "Invalid WGL_CUBE_MAP_FACE_ARB value!\n");
        }
        else {
            pbuffer->cubeFace = val;
        }
    }
}


static void retrace_wglUseFontBitmapsAW(trace::Call &call)
{
    bool ret = call.ret->toBool();
    if (!ret) {
        return;
    }

    uint32_t first = call.arg(1).toUInt();
    uint32_t count = call.arg(2).toUInt();
    uint32_t listBase = call.arg(3).toUInt();

    GLint row_length = 0;
    GLint alignment = 4;
    _glGetIntegerv(GL_UNPACK_ROW_LENGTH, &row_length);
    _glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

    for (uint32_t i = 0; i < count; ++i) {
        uint32_t dwChar = (first + i) % 256;
        const Bitmap *bm = &wglSystemFontBitmaps[dwChar];

        glPixelStorei(GL_UNPACK_ROW_LENGTH, bm->width);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        glNewList(listBase + i, GL_COMPILE);

        glBitmap(bm->width, bm->height,
                 bm->xorig, bm->yorig, bm->xmove, bm->ymove,
                 (const GLubyte *)bm->pixels);

        glEndList();
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, row_length);
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}



const retrace::Entry glretrace::wgl_callbacks[] = {
    {"glAddSwapHintRectWIN", &retrace::ignore},
    {"wglBindTexImageARB", &retrace_wglBindTexImageARB},
    {"wglChoosePixelFormat", &retrace::ignore},
    {"wglChoosePixelFormatARB", &retrace::ignore},
    {"wglChoosePixelFormatEXT", &retrace::ignore},
    {"wglCreateContext", &retrace_wglCreateContext},
    {"wglCreateContextAttribsARB", &retrace_wglCreateContextAttribsARB},
    {"wglCreateLayerContext", &retrace_wglCreateLayerContext},
    {"wglCreatePbufferARB", &retrace_wglCreatePbufferARB},
    {"wglDeleteContext", &retrace_wglDeleteContext},
    {"wglDescribeLayerPlane", &retrace::ignore},
    {"wglDescribePixelFormat", &retrace::ignore},
    {"wglDestroyPbufferARB", &retrace::ignore},
    {"wglGetCurrentContext", &retrace::ignore},
    {"wglGetCurrentDC", &retrace::ignore},
    {"wglGetCurrentReadDCARB", &retrace::ignore},
    {"wglGetCurrentReadDCEXT", &retrace::ignore},
    {"wglGetDefaultProcAddress", &retrace::ignore},
    {"wglGetExtensionsStringARB", &retrace::ignore},
    {"wglGetExtensionsStringEXT", &retrace::ignore},
    {"wglGetLayerPaletteEntries", &retrace::ignore},
    {"wglGetPbufferDCARB", &retrace_wglGetPbufferDCARB},
    {"wglGetPixelFormat", &retrace::ignore},
    {"wglGetPixelFormatAttribfvARB", &retrace::ignore},
    {"wglGetPixelFormatAttribfvEXT", &retrace::ignore},
    {"wglGetPixelFormatAttribivARB", &retrace::ignore},
    {"wglGetPixelFormatAttribivEXT", &retrace::ignore},
    {"wglGetProcAddress", &retrace::ignore},
    {"wglGetSwapIntervalEXT", &retrace::ignore},
    {"wglMakeCurrent", &retrace_wglMakeCurrent},
    {"wglQueryPbufferARB", &retrace::ignore},
    {"wglReleasePbufferDCARB", &retrace::ignore},
    {"wglReleaseTexImageARB", &retrace_wglReleaseTexImageARB},
    {"wglSetPbufferAttribARB", &retrace_wglSetPbufferAttribARB},
    {"wglSetPixelFormat", &retrace::ignore},
    {"wglShareLists", &retrace_wglShareLists},
    {"wglSwapBuffers", &retrace_wglSwapBuffers},
    {"wglSwapIntervalEXT", &retrace::ignore},
    {"wglSwapLayerBuffers", &retrace_wglSwapLayerBuffers},
    {"wglUseFontBitmapsA", &retrace_wglUseFontBitmapsAW},
    {"wglUseFontBitmapsW", &retrace_wglUseFontBitmapsAW},
    {NULL, NULL}
};


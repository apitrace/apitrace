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
#include <map>
#include <sstream>

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


namespace glstate {


Context::Context(void) {
    memset(this, 0, sizeof *this);

    const char *version = (const char *)glGetString(GL_VERSION);
    if (version) {
        if (version[0] == 'O' &&
            version[1] == 'p' &&
            version[2] == 'e' &&
            version[3] == 'n' &&
            version[4] == 'G' &&
            version[5] == 'L' &&
            version[6] == ' ' &&
            version[7] == 'E' &&
            version[8] == 'S' &&
            version[9] == ' ') {
            ES = true;
        }
    }

    ARB_draw_buffers = !ES;

    // TODO: Check extensions we use below
}

void
Context::resetPixelPackState(void) {
    if (!ES) {
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
        glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_SKIP_IMAGES, 0);
    } else {
        packAlignment = 4;
        glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);
    }
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

void
Context::restorePixelPackState(void) {
    if (!ES) {
        glPopClientAttrib();
    } else {
        glPixelStorei(GL_PACK_ALIGNMENT, packAlignment);
    }
}


// Mapping from shader type to shader source, used to accumulated the sources
// of different shaders with same type.
typedef std::map<std::string, std::string> ShaderMap;


static void
getShaderSource(ShaderMap &shaderMap, GLuint shader)
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

    shaderMap[enumToString(shader_type)] += source;

    delete [] source;
}


static void
getShaderObjSource(ShaderMap &shaderMap, GLhandleARB shaderObj)
{
    if (!shaderObj) {
        return;
    }

    GLint object_type = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_TYPE_ARB, &object_type);
    if (object_type != GL_SHADER_OBJECT_ARB) {
        return;
    }

    GLint shader_type = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_SUBTYPE_ARB, &shader_type);
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

    shaderMap[enumToString(shader_type)] += source;

    delete [] source;
}


static inline void
dumpProgram(JSONWriter &json, GLint program)
{
    GLint attached_shaders = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    ShaderMap shaderMap;

    GLuint *shaders = new GLuint[attached_shaders];
    GLsizei count = 0;
    glGetAttachedShaders(program, attached_shaders, &count, shaders);
    std::sort(shaders, shaders + count);
    for (GLsizei i = 0; i < count; ++ i) {
       getShaderSource(shaderMap, shaders[i]);
    }
    delete [] shaders;

    for (ShaderMap::const_iterator it = shaderMap.begin(); it != shaderMap.end(); ++it) {
        json.beginMember(it->first);
        json.writeString(it->second);
        json.endMember();
    }
}


static inline void
dumpProgramObj(JSONWriter &json, GLhandleARB programObj)
{
    GLint attached_shaders = 0;
    glGetObjectParameterivARB(programObj, GL_OBJECT_ATTACHED_OBJECTS_ARB, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    ShaderMap shaderMap;

    GLhandleARB *shaderObjs = new GLhandleARB[attached_shaders];
    GLsizei count = 0;
    glGetAttachedObjectsARB(programObj, attached_shaders, &count, shaderObjs);
    std::sort(shaderObjs, shaderObjs + count);
    for (GLsizei i = 0; i < count; ++ i) {
       getShaderObjSource(shaderMap, shaderObjs[i]);
    }
    delete [] shaderObjs;

    for (ShaderMap::const_iterator it = shaderMap.begin(); it != shaderMap.end(); ++it) {
        json.beginMember(it->first);
        json.writeString(it->second);
        json.endMember();
    }
}

/*
 * When fetching the uniform name of an array we usually get name[0]
 * so we need to cut the trailing "[0]" in order to properly construct
 * array names later. Otherwise we endup with stuff like
 * uniformArray[0][0],
 * uniformArray[0][1],
 * instead of
 * uniformArray[0],
 * uniformArray[1].
 */
static std::string
resolveUniformName(const GLchar *name,  GLint size)
{
    std::string qualifiedName(name);
    if (size > 1) {
        std::string::size_type nameLength = qualifiedName.length();
        static const char * const arrayStart = "[0]";
        static const int arrayStartLen = 3;
        if (qualifiedName.rfind(arrayStart) == (nameLength - arrayStartLen)) {
            qualifiedName = qualifiedName.substr(0, nameLength - 3);
        }
    }
    return qualifiedName;
}

static void
dumpUniform(JSONWriter &json, GLint program, GLint size, GLenum type, const GLchar *name) {
    GLenum elemType;
    GLint numElems;
    __gl_uniform_size(type, elemType, numElems);
    if (elemType == GL_NONE) {
        return;
    }

    GLfloat fvalues[4*4];
    GLdouble dvalues[4*4];
    GLint ivalues[4*4];
    GLuint uivalues[4*4];

    GLint i, j;

    std::string qualifiedName = resolveUniformName(name, size);

    for (i = 0; i < size; ++i) {
        std::stringstream ss;
        ss << qualifiedName;

        if (size > 1) {
            ss << '[' << i << ']';
        }

        std::string elemName = ss.str();

        json.beginMember(elemName);

        GLint location = glGetUniformLocation(program, elemName.c_str());

        if (numElems > 1) {
            json.beginArray();
        }

        switch (elemType) {
        case GL_FLOAT:
            glGetUniformfv(program, location, fvalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(fvalues[j]);
            }
            break;
        case GL_DOUBLE:
            glGetUniformdv(program, location, dvalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(dvalues[j]);
            }
            break;
        case GL_INT:
            glGetUniformiv(program, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(ivalues[j]);
            }
            break;
        case GL_UNSIGNED_INT:
            glGetUniformuiv(program, location, uivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(uivalues[j]);
            }
            break;
        case GL_BOOL:
            glGetUniformiv(program, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeBool(ivalues[j]);
            }
            break;
        default:
            assert(0);
            break;
        }

        if (numElems > 1) {
            json.endArray();
        }

        json.endMember();
    }
}


static void
dumpUniformARB(JSONWriter &json, GLhandleARB programObj, GLint size, GLenum type, const GLchar *name) {

    GLenum elemType;
    GLint numElems;
    __gl_uniform_size(type, elemType, numElems);
    if (elemType == GL_NONE) {
        return;
    }

    GLfloat fvalues[4*4];
    GLint ivalues[4*4];

    GLint i, j;

    std::string qualifiedName = resolveUniformName(name, size);

    for (i = 0; i < size; ++i) {
        std::stringstream ss;
        ss << qualifiedName;

        if (size > 1) {
            ss << '[' << i << ']';
        }

        std::string elemName = ss.str();

        json.beginMember(elemName);

        GLint location = glGetUniformLocationARB(programObj, elemName.c_str());

        if (numElems > 1) {
            json.beginArray();
        }

        switch (elemType) {
        case GL_DOUBLE:
            // glGetUniformdvARB does not exists
        case GL_FLOAT:
            glGetUniformfvARB(programObj, location, fvalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(fvalues[j]);
            }
            break;
        case GL_UNSIGNED_INT:
            // glGetUniformuivARB does not exists
        case GL_INT:
            glGetUniformivARB(programObj, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(ivalues[j]);
            }
            break;
        case GL_BOOL:
            glGetUniformivARB(programObj, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeBool(ivalues[j]);
            }
            break;
        default:
            assert(0);
            break;
        }

        if (numElems > 1) {
            json.endArray();
        }

        json.endMember();
    }
}


static inline void
dumpProgramUniforms(JSONWriter &json, GLint program)
{
    GLint active_uniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &active_uniforms);
    if (!active_uniforms) {
        return;
    }

    GLint active_uniform_max_length = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length);
    GLchar *name = new GLchar[active_uniform_max_length];
    if (!name) {
        return;
    }

    for (GLint index = 0; index < active_uniforms; ++index) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_NONE;
        glGetActiveUniform(program, index, active_uniform_max_length, &length, &size, &type, name);

        dumpUniform(json, program, size, type, name);
    }

    delete [] name;
}


static inline void
dumpProgramObjUniforms(JSONWriter &json, GLhandleARB programObj)
{
    GLint active_uniforms = 0;
    glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &active_uniforms);
    if (!active_uniforms) {
        return;
    }

    GLint active_uniform_max_length = 0;
    glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &active_uniform_max_length);
    GLchar *name = new GLchar[active_uniform_max_length];
    if (!name) {
        return;
    }

    for (GLint index = 0; index < active_uniforms; ++index) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_NONE;
        glGetActiveUniformARB(programObj, index, active_uniform_max_length, &length, &size, &type, name);

        dumpUniformARB(json, programObj, size, type, name);
    }

    delete [] name;
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
dumpArbProgramUniforms(JSONWriter &json, GLenum target, const char *prefix)
{
    if (!glIsEnabled(target)) {
        return;
    }

    GLint program_parameters = 0;
    glGetProgramivARB(target, GL_PROGRAM_PARAMETERS_ARB, &program_parameters);
    if (!program_parameters) {
        return;
    }

    GLint max_program_local_parameters = 0;
    glGetProgramivARB(target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &max_program_local_parameters);
    for (GLint index = 0; index < max_program_local_parameters; ++index) {
        GLdouble params[4] = {0, 0, 0, 0};
        glGetProgramLocalParameterdvARB(target, index, params);

        if (!params[0] && !params[1] && !params[2] && !params[3]) {
            continue;
        }

        char name[256];
        snprintf(name, sizeof name, "%sprogram.local[%i]", prefix, index);

        json.beginMember(name);
        json.beginArray();
        json.writeNumber(params[0]);
        json.writeNumber(params[1]);
        json.writeNumber(params[2]);
        json.writeNumber(params[3]);
        json.endArray();
        json.endMember();
    }

    GLint max_program_env_parameters = 0;
    glGetProgramivARB(target, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &max_program_env_parameters);
    for (GLint index = 0; index < max_program_env_parameters; ++index) {
        GLdouble params[4] = {0, 0, 0, 0};
        glGetProgramEnvParameterdvARB(target, index, params);

        if (!params[0] && !params[1] && !params[2] && !params[3]) {
            continue;
        }

        char name[256];
        snprintf(name, sizeof name, "%sprogram.env[%i]", prefix, index);

        json.beginMember(name);
        json.beginArray();
        json.writeNumber(params[0]);
        json.writeNumber(params[1]);
        json.writeNumber(params[2]);
        json.writeNumber(params[3]);
        json.endArray();
        json.endMember();
    }
}


static inline void
dumpShadersUniforms(JSONWriter &json)
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    GLhandleARB programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);

    json.beginMember("shaders");
    json.beginObject();
    if (program) {
        dumpProgram(json, program);
    } else if (programObj) {
        dumpProgramObj(json, programObj);
    } else {
        dumpArbProgram(json, GL_FRAGMENT_PROGRAM_ARB);
        dumpArbProgram(json, GL_VERTEX_PROGRAM_ARB);
    }
    json.endObject();
    json.endMember(); // shaders

    json.beginMember("uniforms");
    json.beginObject();
    if (program) {
        dumpProgramUniforms(json, program);
    } else if (programObj) {
        dumpProgramObjUniforms(json, programObj);
    } else {
        dumpArbProgramUniforms(json, GL_FRAGMENT_PROGRAM_ARB, "fp.");
        dumpArbProgramUniforms(json, GL_VERTEX_PROGRAM_ARB, "vp.");
    }
    json.endObject();
    json.endMember(); // uniforms
}


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


static inline void
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

    image::Image *image = new image::Image(width, height, channels, true);
    if (!image) {
        return NULL;
    }

    while (glGetError() != GL_NO_ERROR) {}

    GLint read_framebuffer = 0;
    GLint read_buffer = 0;
    if (!context.ES) {
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, draw_framebuffer);

        glGetIntegerv(GL_READ_BUFFER, &read_buffer);
        glReadBuffer(draw_buffer);
    }

    // TODO: reset imaging state too
    context.resetPixelPackState();

    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, image->pixels);

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

#if 0
    /* XXX: Hack to force interpreting depth buffers as RGBA to visualize full
     * dynamic range, until we can transmit HDR images to the GUI */
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


static void
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


static const GLenum bindings[] = {
    GL_DRAW_BUFFER,
    GL_READ_BUFFER,
    GL_PIXEL_PACK_BUFFER_BINDING,
    GL_PIXEL_UNPACK_BUFFER_BINDING,
    GL_TEXTURE_BINDING_1D,
    GL_TEXTURE_BINDING_2D,
    GL_TEXTURE_BINDING_3D,
    GL_TEXTURE_BINDING_RECTANGLE,
    GL_TEXTURE_BINDING_CUBE_MAP,
    GL_DRAW_FRAMEBUFFER_BINDING,
    GL_READ_FRAMEBUFFER_BINDING,
    GL_RENDERBUFFER_BINDING,
    GL_DRAW_BUFFER0,
    GL_DRAW_BUFFER1,
    GL_DRAW_BUFFER2,
    GL_DRAW_BUFFER3,
    GL_DRAW_BUFFER4,
    GL_DRAW_BUFFER5,
    GL_DRAW_BUFFER6,
    GL_DRAW_BUFFER7,
};


#define NUM_BINDINGS sizeof(bindings)/sizeof(bindings[0])


void dumpCurrentContext(std::ostream &os)
{
    JSONWriter json(os);

#ifndef NDEBUG
    GLint old_bindings[NUM_BINDINGS];
    for (unsigned i = 0; i < NUM_BINDINGS; ++i) {
        old_bindings[i] = 0;
        glGetIntegerv(bindings[i], &old_bindings[i]);
    }
#endif

    Context context;

    dumpParameters(json, context);
    dumpShadersUniforms(json);
    dumpTextures(json, context);
    dumpFramebuffer(json, context);

#ifndef NDEBUG
    for (unsigned i = 0; i < NUM_BINDINGS; ++i) {
        GLint new_binding = 0;
        glGetIntegerv(bindings[i], &new_binding);
        if (new_binding != old_bindings[i]) {
            std::cerr << "warning: " << enumToString(bindings[i]) << " was clobbered\n";
        }
    }
#endif

}


} /* namespace glstate */

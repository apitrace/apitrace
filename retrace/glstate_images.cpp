/**************************************************************************
 *
 * Copyright 2011-2014 Jose Fonseca
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
#include <sstream>
#include <vector>

#include "image.hpp"
#include "state_writer.hpp"
#include "retrace.hpp"
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


namespace retrace {
    extern bool resolveMSAA;
}

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


static GLenum
getTextureTarget(Context &context, GLuint texture);


/**
 * OpenGL ES does not support glGetTexLevelParameteriv, but it is possible to
 * probe whether a texture has a given size by crafting a dummy glTexSubImage()
 * call.
 */
static bool
probeTextureLevelSizeOES(GLenum target, GLint level, const GLint size[3],
                         GLenum internalFormat, GLenum type)
{
    flushErrors();

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
        break;
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

    flushErrors();

    return false;
}


static bool
probeTextureFormatOES(GLenum target, GLint level,
                      GLenum *internalFormat, GLenum *type)
{
    static const struct {
        GLenum internalFormat;
        GLenum type;
    } info[] = {
        /* internalFormat */        /* type */
        { GL_RGBA,                  GL_UNSIGNED_BYTE },
        { GL_DEPTH_STENCIL,         GL_FLOAT_32_UNSIGNED_INT_24_8_REV },
        { GL_DEPTH_STENCIL,         GL_UNSIGNED_INT_24_8 },
        { GL_DEPTH_COMPONENT,       GL_FLOAT },
        { GL_DEPTH_COMPONENT,       GL_UNSIGNED_SHORT },
        { GL_STENCIL_INDEX,         GL_UNSIGNED_BYTE },
        /* others? */
        { 0, 0 },
    };
    static const GLint size[3] = {1, 1, 1};

    for (int i = 0; info[i].internalFormat; i++) {
        if (probeTextureLevelSizeOES(target, level, size,
                                     info[i].internalFormat,
                                     info[i].type)) {
            *internalFormat = info[i].internalFormat;
            *type = info[i].type;
            return true;
        }
    }

    return false;
}


/**
 * Bisect the texture size along an axis.
 *
 * It is assumed that the texture exists.
 */
static GLint
bisectTextureLevelSizeOES(GLenum target, GLint level, GLint axis, GLint max,
                          GLenum internalFormat, GLenum type)
{
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

        if (probeTextureLevelSizeOES(target, level, size, internalFormat, type)) {
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
    GLenum internalFormat, type;

    if (!probeTextureFormatOES(target, level, &internalFormat, &type)) {
        return false;
    }

    desc.internalFormat = internalFormat;

    GLint maxSize = 0;
    switch (target) {
    case GL_TEXTURE_2D:
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
        desc.width = bisectTextureLevelSizeOES(target, level, 0, maxSize, internalFormat, type);
        desc.height = bisectTextureLevelSizeOES(target, level, 1, maxSize, internalFormat, type);
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
        desc.width = bisectTextureLevelSizeOES(target, level, 0, maxSize, internalFormat, type);
        desc.height = desc.width;
        desc.depth = 1;
        break;
    case GL_TEXTURE_3D_OES:
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE_OES, &maxSize);
        desc.width = bisectTextureLevelSizeOES(target, level, 0, maxSize, internalFormat, type);
        desc.width = bisectTextureLevelSizeOES(target, level, 1, maxSize, internalFormat, type);
        desc.depth = bisectTextureLevelSizeOES(target, level, 2, maxSize, internalFormat, type);
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

    if (target == GL_TEXTURE_BUFFER) {
        assert(level == 0);

        GLint buffer = 0;
        glGetIntegerv(GL_TEXTURE_BUFFER_DATA_STORE_BINDING, &buffer);
        if (!buffer) {
            return false;
        }

        // This is the general binding point, not the texture's
        GLint active_buffer = 0;
        glGetIntegerv(GL_TEXTURE_BUFFER, &active_buffer);
        glBindBuffer(GL_TEXTURE_BUFFER, buffer);

        GLint buffer_size = 0;
        glGetBufferParameteriv(GL_TEXTURE_BUFFER, GL_BUFFER_SIZE, &buffer_size);

        glBindBuffer(GL_TEXTURE_BUFFER, active_buffer);

        glGetIntegerv(GL_TEXTURE_BUFFER_FORMAT_ARB, &desc.internalFormat);

        const InternalFormatDesc &formatDesc = getInternalFormatDesc(desc.internalFormat);
        if (formatDesc.type == GL_NONE) {
            std::cerr << "error: unexpected GL_TEXTURE_BUFFER internal format "
                      << enumToString(desc.internalFormat)
                      << " (https://git.io/JOMRy)\n";
            return false;
        }

        unsigned bits_per_pixel = _gl_format_size(formatDesc.format, formatDesc.type);

        desc.width = buffer_size * 8 / bits_per_pixel;
        desc.height = 1;
        desc.depth = 1;

        return desc.valid();
    }

    glGetTexLevelParameteriv(target, level, GL_TEXTURE_INTERNAL_FORMAT, &desc.internalFormat);

    desc.width = 0;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &desc.width);

    if (target == GL_TEXTURE_BUFFER ||
        target == GL_TEXTURE_1D) {
        desc.height = 1;
        desc.depth = 1;
    } else {
        desc.height = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &desc.height);
        if (target != GL_TEXTURE_3D &&
            target != GL_TEXTURE_2D_ARRAY &&
            target != GL_TEXTURE_2D_MULTISAMPLE_ARRAY &&
            target != GL_TEXTURE_CUBE_MAP_ARRAY) {
            desc.depth = 1;
        } else {
            desc.depth = 0;
            glGetTexLevelParameteriv(target, level, GL_TEXTURE_DEPTH, &desc.depth);
        }
    }

    glGetTexLevelParameteriv(target, level, GL_TEXTURE_SAMPLES, &desc.samples);

    return desc.valid();
}


const GLenum
textureTargets[] = {
    GL_TEXTURE_2D,
    GL_TEXTURE_1D,
    GL_TEXTURE_RECTANGLE,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_3D,
    GL_TEXTURE_2D_MULTISAMPLE,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_CUBE_MAP_ARRAY,
    GL_TEXTURE_BUFFER,
};

const unsigned
numTextureTargets = ARRAYSIZE(textureTargets);


GLenum
getTextureBinding(GLenum target)
{
    switch (target) {
    case GL_TEXTURE_1D:
        return GL_TEXTURE_BINDING_1D;
    case GL_TEXTURE_1D_ARRAY:
        return GL_TEXTURE_BINDING_1D_ARRAY;
    case GL_TEXTURE_2D:
        return GL_TEXTURE_BINDING_2D;
    case GL_TEXTURE_2D_ARRAY:
        return GL_TEXTURE_BINDING_2D_ARRAY;
    case GL_TEXTURE_2D_MULTISAMPLE:
        return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
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
    case GL_TEXTURE_BUFFER:
        return GL_TEXTURE_BINDING_BUFFER;
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
getTexImageOES(GLenum target, GLint level, GLenum format, GLenum type,
               ImageDesc &desc, GLubyte *pixels)
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
        glReadPixels(0, 0, desc.width, desc.height, format, type, pixels);
        break;
    case GL_TEXTURE_3D_OES:
        for (int i = 0; i < desc.depth; i++) {
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texture, level, i);
            glReadPixels(0, 0, desc.width, desc.height, format, type, pixels + 4 * i * desc.width * desc.height);
        }
        break;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);

    glDeleteFramebuffers(1, &fbo);
}

/**
 * Simple Shader compile
 */
static inline int
compileShader(const GLchar* shaderSource, GLenum shaderType )
{

    GLuint id;
    GLint result;
    int logLength;
    char logInfo[1000];

    id = glCreateShader(shaderType);
    glShaderSource(id, 1, (const GLchar**)&shaderSource, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        glGetShaderInfoLog(id, logLength, NULL, &logInfo[0]);
        std::cerr << std::endl << logInfo << std::endl;
        return -1;
    }

    return id;
}

/**
 * Program Creation / Linking.
 */
static inline void
createProgram(GLuint program_id, const GLchar* vShaderSource, const GLchar* pShaderSource)
{
    GLint result;
    int logLength;
    char logInfo[1000];

    GLint vshaderID = compileShader(vShaderSource, GL_VERTEX_SHADER);
    GLint pshaderID = compileShader(pShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(program_id, vshaderID);
    glAttachShader(program_id, pshaderID);
    glLinkProgram(program_id);
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);

    if (result == GL_FALSE) {
        glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &logLength);
        glGetShaderInfoLog(program_id, logLength, NULL, &logInfo[0]);
        std::cerr << std::endl << logInfo << std::endl;
    }

    glDeleteShader(vshaderID);
    glDeleteShader(pshaderID);
    return;
}

/**
 * Obtain unresolved/resolved MSAA surface by attaching the
 * texture to a framebuffer and using a multisample texel fetch inside custom shader.
 */
static inline void
getTexImageMSAA(GLenum target, GLenum format, GLenum type,
               ImageDesc &desc, GLubyte *pixels, bool resolve)
{
    TempId prog = TempId(GL_PROGRAM);
    TempId vao = TempId(GL_VERTEX_ARRAY);
    TempId vbo = TempId(GL_ARRAY_BUFFER);

    /*
     * Vertices for 2 tri strip
     */
    const GLint channels = 4;
    const GLint vertices = 4;
    static const float vertArray[vertices][channels] = {
        {  1.0f, -1.0f, 0.0f, 1.0f },
        {  1.0f,  1.0f, 0.0f, 1.0f },
        { -1.0f, -1.0f, 0.0f, 1.0f },
        { -1.0f,  1.0f, 0.0f, 1.0f },
    };

    /*
     * Create an empty texture + fbo of correct size.
     */
    TempId tex = TempId(GL_TEXTURE);
    TextureBinding bt = TextureBinding(GL_TEXTURE_2D, tex.ID());

    TempId fbo = TempId(GL_FRAMEBUFFER);
    BufferBinding fb = BufferBinding(GL_FRAMEBUFFER, fbo.ID());
    BufferBinding Db = BufferBinding(GL_DRAW_BUFFER, GL_COLOR_ATTACHMENT0);
    BufferBinding Rb = BufferBinding(GL_READ_BUFFER, GL_COLOR_ATTACHMENT0);

    GLint savedProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgram);

    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, savedViewport);
    GLuint viewport_height = desc.height;

    if (resolve){

        /*
         * Create blitting program to perform resolve.
         */

        const GLchar* const vShaderSource =
            "in vec4 Position;\n            "
            "void main() {\n                "
            "     gl_Position = Position;\n "
            "}\n                            ";

        const GLchar* const pShaderSource =
            "#version 430\n;                                          "
            "uniform ivec3 texDim; // Width, Height, samples\n        "
            "uniform sampler2DMS Sampler;\n                           "
            "layout(location = 0) out vec4 FragColor;\n               "
            "void main() {\n                                          "
            "   ivec2 coord = ivec2(gl_FragCoord.xy);\n               "
            "   coord.x = texDim.x - coord.x - 1;\n                   "
            "   vec4 outColor = { 0, 0, 0, 0 }; \n                    "
            "   for (int i=0; i<int(texDim.z); i++) {                 "
            "       outColor += texelFetch(Sampler, coord, i);\n      "
            "   } \n                                                  "
            "   FragColor = outColor / float(texDim.z); \n            "
            "}\n                                                      ";

        createProgram(prog.ID(), vShaderSource, pShaderSource);
        glUseProgram(prog.ID());

        /*
         * Pass uniform for texture dimensions and number of samples
         */
        GLint myLoc = glGetUniformLocation(prog.ID(), "texDim");
        glProgramUniform3i(prog.ID(), myLoc, desc.width, desc.height, desc.samples);

    } else { /* no resolve */

        /*
         * Read out each individual sample surface with border for MSAA surface
         * Create blitting program to copy unresolved samples into tall texture.
         */
        const GLchar* const vShaderSource =
            "in vec4 Position;\n            "
            "void main() {\n                "
            "     gl_Position = Position;\n "
            "}\n                            ";

        const GLchar* const pShaderSource =
            "#version 430\n;                                          "
            "uniform ivec2 texDim; // Width, Height\n                 "
            "uniform sampler2DMS Sampler;\n                           "
            "layout(location = 0) out vec4 FragColor;\n               "
            "void main() {\n                                          "
            "   ivec2 coord = ivec2(gl_FragCoord.xy);\n               "
            "   int height = int(gl_FragCoord.y / texDim.y);\n        "
            "   coord.y = (coord.y % texDim.y);\n                     "
            "   coord.x = texDim.x - coord.x - 1;\n                   "
            "   FragColor = texelFetch(Sampler, coord, height);\n     "
            "}\n                                                      ";


        createProgram(prog.ID(), vShaderSource, pShaderSource);
        glUseProgram(prog.ID());

        /*
         * Pass uniform for texture dimensions and viewport height
         * Add bottom border to each sample window only (in-between samples)
         */
        viewport_height = desc.height * desc.samples;
        GLint myLoc = glGetUniformLocation(prog.ID(), "texDim");
        glProgramUniform2i(prog.ID(), myLoc, desc.width, desc.height);
    }

    glViewport(0, 0, desc.width, viewport_height);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertArray), vertArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glTexImage2D(GL_TEXTURE_2D, 0, desc.internalFormat, desc.width, viewport_height, 0, format, type, NULL);

    GLuint clearBufferBit;
    switch(format)
    {
        case GL_DEPTH_COMPONENT:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, bt.ID(), 0);
            clearBufferBit = GL_DEPTH_BUFFER_BIT;
            break;
        case GL_STENCIL_INDEX:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, bt.ID(), 0);
            clearBufferBit = GL_STENCIL_BUFFER_BIT;
            break;
        default:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, bt.ID(), 0);
            clearBufferBit = GL_COLOR_BUFFER_BIT;
            break;
    }

    GLuint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not done..." << std::endl;
        goto exit_clean;
    }

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear( clearBufferBit );

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glReadPixels(0, 0, desc.width, viewport_height, format, type, pixels);

exit_clean:
    // Put back state
    glUseProgram(savedProgram);
    glDisableVertexAttribArray(0);
    glViewport(savedViewport[0],savedViewport[1],savedViewport[2],savedViewport[3]);

    return;
}

static inline void
dumpActiveTextureLevel(StateWriter &writer, Context &context,
                       GLenum target, GLint level,
                       GLint layer, GLint num_layers,
                       const std::string & label,
                       const char *userLabel)
{

    bool multiSample = (target == GL_TEXTURE_2D_MULTISAMPLE
                        || target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) ? true : false;

    ImageDesc desc;
    if (!getActiveTextureLevelDesc(context, target, level, desc)) {
        return;
    }
    if (layer < 0 || num_layers < 0 || layer + num_layers > desc.depth) {
        std::cerr << "warning: invalid layer range\n";
        return;
    }

    const InternalFormatDesc &formatDesc = getInternalFormatDesc(desc.internalFormat);

    GLenum format;
    GLenum type;
    const PixelFormat *pixelFormat = nullptr;

    if (target == GL_TEXTURE_BUFFER) {
        pixelFormat = getPixelFormat(desc.internalFormat);
        if (!pixelFormat) {
            std::cerr << "warning: unsupported texture buffer internal format " << formatToString(desc.internalFormat) << "\n";
            return;
        }
        format = GL_RGBA;
        type = GL_FLOAT;
    } else {
        chooseReadBackFormat(formatDesc, format, type);
    }

    writer.beginMember(label);

    if (context.ES && format == GL_DEPTH_COMPONENT) {
        format = GL_RED;
    }

    GLuint channels;
    image::ChannelType channelType;
    getImageFormat(format, type, channels, channelType);

    if (0) {
        std::cerr << std::endl << std::endl;
        std::cerr << label << ", " << userLabel << std::endl;
        std::cerr << enumToString(desc.internalFormat) << ", " << enumToString(format) << ", " << enumToString(type) << std::endl;
        std::cerr << "desc (w,h,d) (" << desc.width << ", " << desc.height << ", " << desc.depth << "), "
                  << "channels: " << channels << ", channelType: " << channelType << std::endl;
    }

    image::Image *image;
    PixelPackState pps(context);

    if (target == GL_TEXTURE_BUFFER) {
        assert(desc.height == 1);
        assert(desc.depth == 1);
        assert(pixelFormat);
        assert(format == GL_RGBA);
        assert(type == GL_FLOAT);

        image = new image::Image(desc.width, desc.height * desc.depth, channels, true, channelType);
        assert(image->bytesPerPixel == sizeof(float[4]));

        GLint buffer = 0;
        glGetIntegerv(GL_TEXTURE_BUFFER_DATA_STORE_BINDING, &buffer);
        assert(buffer);

        BufferMapping bm;

        const GLvoid *map = bm.map(GL_TEXTURE_BUFFER, buffer);
        if (map) {
            pixelFormat->unpackSpan(static_cast<const uint8_t *>(map),
                                    reinterpret_cast<float *>(image->pixels), image->width);
        }
    } else if (multiSample) {
        // Perform multisample retrieval here.

        if (retrace::resolveMSAA){
            // For resolved MSAA...
            image = new image::Image(desc.width, desc.height, channels, true, channelType);
            memset(image->pixels, 0x0, image->sizeInBytes());
            getTexImageMSAA(target, format, type, desc, image->pixels, true);
        }
        else {
            // For unresolved MSAA...
            GLuint samples = std::max(desc.samples, 1);
            GLuint total_height = desc.height * samples;
            image = new image::Image(desc.width, total_height, channels, true, channelType);
            memset(image->pixels, 0x0, image->sizeInBytes());
            getTexImageMSAA(target, format, type, desc, image->pixels, false);
        }
    }
    else {
        // Create a simple image single sample size.
        image = new image::Image(desc.width, desc.height * desc.depth, channels, true, channelType);

        if (context.ES) {
            getTexImageOES(target, level, format, type, desc, image->pixels);
        } else {
            glGetTexImage(target, level, format, type, image->pixels);
        }

        // If a strict subset of the texture's layers is requested, those layers
        // need extracting from image->pixels into a new replacement
        // image::Image. This is redundant when all layers are requested.
        if (num_layers > 0 && num_layers < desc.depth) {
            image::Image *image2 = new image::Image(desc.width, desc.height * num_layers, channels, true, channelType);
            memcpy(image2->pixels,
                   image->pixels + layer * image->bytesPerPixel * desc.width * desc.height,
                   num_layers * image->bytesPerPixel * desc.width * desc.height);
            delete image;
            image = image2;
            desc.depth = num_layers;
        }
    }

    if (userLabel) {
        image->label = userLabel;
    }

    StateWriter::ImageDesc imageDesc;
    imageDesc.depth = desc.depth;
    imageDesc.format = formatToString(desc.internalFormat);
    writer.writeImage(image, imageDesc);

    delete image;

    writer.endMember(); // label
}


static inline void
dumpActiveTexture(StateWriter &writer, Context &context, GLenum target, GLuint texture)
{
    char *object_label = getObjectLabel(context, GL_TEXTURE, texture);

    GLint active_texture = GL_TEXTURE0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);
    assert(active_texture >= GL_TEXTURE0);

    GLenum start_subtarget;
    GLenum stop_subtarget;
    if (target == GL_TEXTURE_CUBE_MAP) {
        start_subtarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        stop_subtarget = start_subtarget + 6;
    } else {
        start_subtarget = target;
        stop_subtarget = start_subtarget + 1;
    }

    GLboolean allowMipmaps = target != GL_TEXTURE_RECTANGLE &&
                             target != GL_TEXTURE_BUFFER &&
                             target != GL_TEXTURE_2D_MULTISAMPLE &&
                             target != GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

    GLint level = 0;
    do {
        ImageDesc desc;

        for (GLenum subtarget = start_subtarget; subtarget < stop_subtarget; ++subtarget) {
            std::stringstream label;
            label << "GL_TEXTURE" << (active_texture - GL_TEXTURE0) << ", "
                  << enumToString(subtarget);
            if (allowMipmaps) {
                label << ", level = " << level;
            }

            if (!getActiveTextureLevelDesc(context, subtarget, level, desc)) {
                goto finished;
            }
            dumpActiveTextureLevel(writer, context, subtarget, level, 0, 0, label.str(), object_label);
        }

        if (!allowMipmaps) {
            // no mipmaps
            break;
        }

        ++level;
    } while(true);

finished:
    free(object_label);
}

static void
dumpTextureImages(StateWriter &writer, Context &context)
{
    if (!context.ARB_shader_image_load_store) {
        return;
    }

    GLint maxImageUnits = 0;
    glGetIntegerv(GL_MAX_IMAGE_UNITS, &maxImageUnits);
    glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

    for (GLint imageUnit = 0; imageUnit < maxImageUnits; ++imageUnit) {
        GLint texture = 0;
        glGetIntegeri_v(GL_IMAGE_BINDING_NAME, imageUnit, &texture);
        if (texture) {
            GLint level = 0;
            glGetIntegeri_v(GL_IMAGE_BINDING_LEVEL, imageUnit, &level);
            GLint isLayered = 0;
            glGetIntegeri_v(GL_IMAGE_BINDING_LAYERED, imageUnit, &isLayered);
            GLint layer = 0;
            glGetIntegeri_v(GL_IMAGE_BINDING_LAYER, imageUnit, &layer);
            std::stringstream label;
            label << "Image Unit " << imageUnit;
            if (level) {
                label << ", level = " << level;
            }
            if (isLayered) {
                label << ", layer = " << layer;
            }
            GLenum target = getTextureTarget(context, texture);
            GLint previousTexture = 0;
            glGetIntegerv(getTextureBinding(target), &previousTexture);

            glBindTexture(target, texture);
            char *object_label = getObjectLabel(context, GL_TEXTURE, texture);
            dumpActiveTextureLevel(writer, context, target, level, 0, 0, label.str(),
                                   object_label);
            free(object_label);
            glBindTexture(target, previousTexture);
        }
    }
}


void
dumpTextures(StateWriter &writer, Context &context)
{
    writer.beginMember("textures");
    writer.beginObject();

    GLint max_texture_coords = 0;
    if (!context.core) {
        glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);
    }

    GLint max_combined_texture_image_units = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);

    /*
     * At least the Android software GL implementation doesn't return the
     * proper value for this, but rather returns 0. The GL(ES) specification
     * mandates a minimum value of 2, so use this as a fall-back value.
     */
    max_combined_texture_image_units = std::max(max_combined_texture_image_units, 2);

    GLint max_units = std::max(max_combined_texture_image_units, max_texture_coords);

    GLint active_texture = GL_TEXTURE0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);

    for (GLint unit = 0; unit < max_units; ++unit) {
        GLenum texture = GL_TEXTURE0 + unit;
        glActiveTexture(texture);

        for (auto target : textureTargets) {

            // Whether this fixed-function stage is enabled
            GLboolean enabled = GL_FALSE;
            if (unit < max_texture_coords &&
                (target == GL_TEXTURE_1D ||
                 target == GL_TEXTURE_2D ||
                 target == GL_TEXTURE_3D ||
                 target == GL_TEXTURE_CUBE_MAP ||
                 target == GL_TEXTURE_RECTANGLE)) {
                glGetBooleanv(target, &enabled);
            }

            // Whether a texture object is bound
            GLint texture = 0;
            if (unit < max_combined_texture_image_units) {
                GLenum binding = getTextureBinding(target);
                glGetIntegerv(binding, &texture);
            }

            if (enabled || texture) {
                dumpActiveTexture(writer, context, target, texture);
            }
        }
    }

    glActiveTexture(active_texture);

    dumpTextureImages(writer, context);

    writer.endObject();
    writer.endMember(); // textures
}


bool
getDrawableBounds(GLint *width, GLint *height) {
#if defined(__linux__)
    if (_getPublicProcAddress("eglGetCurrentContext")) {
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

    if (!_getPublicProcAddress("glXGetCurrentDisplay")) {
        return false;
    }

    Display *display;
    GLXDrawable drawable;

    display = glXGetCurrentDisplay();
    if (!display) {
        return false;
    }

    drawable = glXGetCurrentDrawable();
    if (drawable == None) {
        return false;
    }

    int major = 0, minor = 0;
    if (!glXQueryVersion(display, &major, &minor)) {
        return false;
    }

    // XGetGeometry will fail for PBuffers, whereas glXQueryDrawable should not.
    unsigned w = 0;
    unsigned h = 0;
    if (major > 1 || (major == 1 && minor >= 3)) {
        glXQueryDrawable(display, drawable, GLX_WIDTH, &w);
        glXQueryDrawable(display, drawable, GLX_HEIGHT, &h);
    } else {
        Window root;
        int x, y;
        unsigned bw, depth;
        XGetGeometry(display, drawable,  &root, &x, &y, &w, &h, &bw, &depth);
    }

    *width = w;
    *height = h;
    return true;

#else

    return false;

#endif
}


static GLenum
getTextureTarget(Context &context, GLuint texture)
{
    if (!glIsTexture(texture)) {
        return GL_NONE;
    }

    /*
     * GL_ARB_direct_state_access's glGetTextureParameteriv(GL_TEXTURE_TARGET)
     * would be perfect, except the fact it's not supported for
     * TEXTURE_BUFFERS...
     */

    GLint result = GL_NONE;

    flushErrors();

    // Temporarily disable debug messages
    GLDEBUGPROC prevDebugCallbackFunction = 0;
    void *prevDebugCallbackUserParam = 0;
    if (context.KHR_debug) {
        glGetPointerv(GL_DEBUG_CALLBACK_FUNCTION, (GLvoid **) &prevDebugCallbackFunction);
        glGetPointerv(GL_DEBUG_CALLBACK_USER_PARAM, &prevDebugCallbackUserParam);
        glDebugMessageCallback(NULL, NULL);
    }

    for (auto target : textureTargets) {
        GLenum binding = getTextureBinding(target);

        GLint bound_texture = 0;
        glGetIntegerv(binding, &bound_texture);
        glBindTexture(target, texture);

        bool succeeded = glGetError() == GL_NO_ERROR;

        glBindTexture(target, bound_texture);

        if (succeeded) {
            result = target;
            break;
        }

        flushErrors();
    }

    if (context.KHR_debug) {
        glDebugMessageCallback(prevDebugCallbackFunction, prevDebugCallbackUserParam);
    }

    return result;
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
            GLenum texture_target = getTextureTarget(context, object_name);
            GLenum texture_binding = getTextureBinding(texture_target);
            glGetIntegerv(texture_binding, &bound_texture);
            glBindTexture(texture_target, object_name);
            getActiveTextureLevelDesc(context, texture_target, texture_level, desc);
            glBindTexture(texture_target, bound_texture);
        }

        return desc.valid();
    } else {
        std::cerr << "warning: unexpected GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = " << enumToString(object_type) << "\n";
        return false;
    }
}


int
getDrawBufferImageCount()
{
    Context context;
    GLint count;

    if (context.framebuffer_object) {
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &count);
        flushErrors();
    } else {
        return 0;
    }

    return count;
}


image::Image *
getDrawBufferImage(int n, bool backBuffer)
{
    Context context;

    GLenum format = GL_RGB;
    GLenum type = GL_UNSIGNED_BYTE;
    if (context.ES) {
        format = GL_RGBA;
        if (n < 0 && !context.NV_read_depth_stencil) {
            return nullptr;
        }
    }

    GLenum framebuffer_binding;
    GLenum framebuffer_target;
    if (context.read_framebuffer_object) {
        framebuffer_binding = GL_DRAW_FRAMEBUFFER_BINDING;
        framebuffer_target = GL_DRAW_FRAMEBUFFER;
    } else {
        framebuffer_binding = GL_FRAMEBUFFER_BINDING;
        framebuffer_target = GL_FRAMEBUFFER;
    }
    GLint draw_framebuffer = 0;
    if (context.framebuffer_object && !backBuffer) {
        glGetIntegerv(framebuffer_binding, &draw_framebuffer);
    }

    /*
     * TODO: Use alpha for non-FBOs once we are able to match the traced
     * visuals.
     */
    if (draw_framebuffer || retrace::snapshotAlpha) {
        format = GL_RGBA;
    }

    if (n == -2) {
        /* read stencil */
        format = GL_STENCIL_INDEX;
        n = 0;
    } else if (n == -1) {
        /* read depth */
        format = GL_DEPTH_COMPONENT;
        n = 0;
    }

    GLint draw_buffer = GL_NONE;
    ImageDesc desc;
    if (draw_framebuffer && !backBuffer) {
        if (context.ARB_draw_buffers) {
            glGetIntegerv(GL_DRAW_BUFFER0 + n, &draw_buffer);
            if (draw_buffer == GL_NONE) {
                return NULL;
            }
        } else {
            // GL_COLOR_ATTACHMENT0 is implied
            draw_buffer = GL_COLOR_ATTACHMENT0 + n;
        }

        if (!getFramebufferAttachmentDesc(context, framebuffer_target, draw_buffer, desc)) {
            return NULL;
        }
    } else if (n == 0) {
        if (context.ES || backBuffer) {
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
    } else {
        return NULL;
    }

    GLint channels = _gl_format_channels(format);
    if (channels > 4) {
        return NULL;
    }

    image::ChannelType channelType = image::TYPE_UNORM8;

    if (format == GL_DEPTH_COMPONENT) {
        type = GL_FLOAT;
        channels = 1;
        channelType = image::TYPE_FLOAT;
    }

    image::Image *image = new image::Image(desc.width, desc.height, channels, true, channelType);
    if (!image) {
        return NULL;
    }

    flushErrors();

    GLint read_framebuffer = 0;
    GLint read_buffer = GL_NONE;
    if (context.read_framebuffer_object) {
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, draw_framebuffer);
    }

    if (context.read_buffer) {
        glGetIntegerv(GL_READ_BUFFER, &read_buffer);
        glReadBuffer(draw_buffer);
    }

    {
        // TODO: reset imaging state too
        PixelPackState pps(context);
        glReadPixels(0, 0, desc.width, desc.height, format, type, image->pixels);
    }


    if (context.read_buffer) {
        glReadBuffer(read_buffer);
    }
    if (context.read_framebuffer_object) {
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
dumpReadBufferImage(StateWriter &writer,
                    Context & context,
                    const char *label,
                    const char *userLabel,
                    GLint width, GLint height,
                    GLenum format, GLenum type,
                    GLenum internalFormat = GL_NONE)
{
    if (internalFormat == GL_NONE) {
        internalFormat = format;
    }

    if (context.ES) {
        switch (format) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_STENCIL:
            if (!context.NV_read_depth_stencil) {
                return;
            }
            /* FIXME: NV_read_depth_stencil states that type must match the
             * internal format, whereas we always request GL_FLOAT, as that's
             * what we want.  To fix this we should probe the adequate type
             * here, and then manually convert the pixels to float after
             * glReadPixels */
            break;
        case GL_STENCIL_INDEX:
            if (!context.NV_read_depth_stencil) {
                return;
            }
            type = GL_UNSIGNED_BYTE;
            break;
        default:
            // Color formats -- GLES glReadPixels only supports the following combinations:
            // - GL_RGBA and GL_UNSIGNED_BYTE
            // - values of IMPLEMENTATION_COLOR_READ_FORMAT and IMPLEMENTATION_COLOR_READ_TYPE
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        }
    }

    GLuint channels;
    image::ChannelType channelType;
    getImageFormat(format, type, channels, channelType);

    if (0) {
        std::cerr << enumToString(internalFormat) << " "
                  << enumToString(format) << " "
                  << enumToString(type) << "\n";
    }

    image::Image *image = new image::Image(width, height, channels, true, channelType);

    flushErrors();

    {
        // TODO: reset imaging state too
        PixelPackState pps(context);

        glReadPixels(0, 0, width, height, format, type, image->pixels);
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        do {
            std::cerr << "warning: " << enumToString(error) << " while reading framebuffer\n";
            error = glGetError();
        } while(error != GL_NO_ERROR);
    } else {
        if (userLabel) {
            image->label = userLabel;
        }

        StateWriter::ImageDesc imageDesc;
        imageDesc.format = formatToString(internalFormat);
        writer.beginMember(label);
        writer.writeImage(image, imageDesc);
        writer.endMember();
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

    GLboolean scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    if (scissor_test) {
        glDisable(GL_SCISSOR_TEST);
    }

    {
        // color buffer
        GLint maxColorAtt = 0;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAtt);
        std::vector<GLenum> drawbufs(maxColorAtt);
        for (int iColorAtt = 0; iColorAtt < maxColorAtt; ++iColorAtt) {
            GLenum colorAtt = GL_COLOR_ATTACHMENT0 + iColorAtt;
            drawbufs[iColorAtt] = colorAtt;

            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            GLint objType = GL_NONE;
            glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, colorAtt,
                                                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
            if (objType != GL_NONE) {
               glGenRenderbuffers(1, &rbs[*numRbs]);

               glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
               glRenderbufferStorage(GL_RENDERBUFFER, colorDesc.internalFormat, colorDesc.width, colorDesc.height);
               glFramebufferRenderbuffer(GL_FRAMEBUFFER, colorAtt,
                                         GL_RENDERBUFFER, rbs[*numRbs]);

               glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
               glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
               glReadBuffer(colorAtt);
               glDrawBuffer(colorAtt);

               glBlitFramebuffer(0, 0, colorDesc.width, colorDesc.height, 0, 0, colorDesc.width, colorDesc.height,
                                 GL_COLOR_BUFFER_BIT, GL_NEAREST);
               glBindFramebuffer(GL_FRAMEBUFFER, fbo);
               ++*numRbs;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffers(maxColorAtt, &drawbufs[0]);
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
        glDrawBuffer(drawbuffer);
        glReadBuffer(drawbuffer);
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
            glDrawBuffer(drawbuffer);
            glReadBuffer(drawbuffer);
            glBlitFramebuffer(0, 0, depthDesc.width, depthDesc.height, 0, 0, depthDesc.width, depthDesc.height,
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
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
            glDrawBuffer(drawbuffer);
            glReadBuffer(drawbuffer);
            glBlitFramebuffer(0, 0, stencilDesc.width, stencilDesc.height, 0, 0, stencilDesc.width, stencilDesc.height,
                              GL_STENCIL_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            ++*numRbs;
        }
    }

    if (scissor_test) {
        glEnable(GL_SCISSOR_TEST);
    }

    return fbo;
}


/**
 * Dump images of current draw drawable/window.
 */
static void
dumpDrawableImages(StateWriter &writer, Context &context)
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

    // Reset read framebuffer
    GLint read_framebuffer = 0;
    if (context.read_framebuffer_object) {
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    } else if (context.framebuffer_object) {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &read_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if (draw_buffer != GL_NONE) {
        // Read from current draw buffer
        GLint read_buffer = GL_NONE;
        if (context.read_buffer) {
            glGetIntegerv(GL_READ_BUFFER, &read_buffer);
            glReadBuffer(draw_buffer);
        }

        GLint alpha_bits = 0;
#if 0
        // XXX: Ignore alpha until we are able to match the traced visual
        glGetIntegerv(GL_ALPHA_BITS, &alpha_bits);
#endif
        GLenum format = alpha_bits ? GL_RGBA : GL_RGB;
        GLenum type = GL_UNSIGNED_BYTE;

        dumpReadBufferImage(writer, context, enumToString(draw_buffer), NULL, width, height, format, type);

        // Restore original read buffer
        if (context.read_buffer) {
            glReadBuffer(read_buffer);
        }
    }

    if (!context.ES || context.NV_read_depth_stencil) {
        GLint depth_bits = 0;
        if (context.core) {
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
        } else {
            glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
        }
        if (depth_bits) {
            dumpReadBufferImage(writer, context, "GL_DEPTH_COMPONENT", NULL, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
        }

        GLint stencil_bits = 0;
        if (context.core) {
            glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
        } else {
            glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
        }
        if (stencil_bits) {
            assert(stencil_bits <= 8);
            dumpReadBufferImage(writer, context, "GL_STENCIL_INDEX", NULL, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE);
        }
    }

    // Restore original read framebuffer
    if (context.read_framebuffer_object) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    } else if (context.framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, read_framebuffer);
    }
}


/**
 * Dump the specified framebuffer attachment.
 *
 * In the case of a color attachment, it assumes it is already bound for read.
 */
static void
dumpFramebufferAttachment(StateWriter &writer, Context &context, GLenum target, GLenum attachment)
{
    ImageDesc desc;
    if (!getFramebufferAttachmentDesc(context, target, attachment, desc)) {
        return;
    }

    assert(desc.samples == 0);

    GLenum format;
    GLenum type;
    switch (attachment) {
    case GL_DEPTH_ATTACHMENT:
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
        break;
    case GL_STENCIL_ATTACHMENT:
        format = GL_STENCIL_INDEX;
        type = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(desc.internalFormat != GL_NONE);
        const InternalFormatDesc &formatDesc = getInternalFormatDesc(desc.internalFormat);
        chooseReadBackFormat(formatDesc, format, type);
    }

    GLint object_type = GL_NONE;
    glGetFramebufferAttachmentParameteriv(target, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                          &object_type);
    GLint object_name = 0;
    glGetFramebufferAttachmentParameteriv(target, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                          &object_name);
    char *object_label = getObjectLabel(context, object_type, object_name);

    const char *label = enumToString(attachment);

    if (object_type == GL_TEXTURE) {
        GLint layered = GL_FALSE;
        glGetFramebufferAttachmentParameteriv(target, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_LAYERED,
                                              &layered);

        // OVR_multiview
        GLint first_view = 0;
        GLint num_views = 0;
        if (context.OVR_multiview) {
            glGetFramebufferAttachmentParameteriv(target, attachment,
                                                  GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR,
                                                  &first_view);
            glGetFramebufferAttachmentParameteriv(target, attachment,
                                                  GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR,
                                                  &num_views);
        }
        if ((layered && isGeometryShaderBound(context)) ||
            num_views > 0) {
            /*
             * Dump multiple layers in the texture array.
             *
             * Unfortunately we can't tell whether the bound GS writes or not gl_Layer.
             */

            GLint level = 0;
            glGetFramebufferAttachmentParameteriv(target, attachment,
                                                  GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
                                                  &level);

            GLenum texture_target = getTextureTarget(context, object_name);
            GLenum texture_binding = getTextureBinding(texture_target);

            GLint bound_texture = 0;
            glGetIntegerv(texture_binding, &bound_texture);
            glBindTexture(texture_target, object_name);

            dumpActiveTextureLevel(writer, context, texture_target, level, first_view, num_views, label, object_label);

            glBindTexture(texture_target, bound_texture);

            free(object_label);
            return;
        }
    }


    dumpReadBufferImage(writer, context,
                        label, object_label,
                        desc.width, desc.height,
                        format, type, desc.internalFormat);
    free(object_label);
}


static void
dumpFramebufferAttachments(StateWriter &writer, Context &context, GLenum target)
{
    GLenum status = glCheckFramebufferStatus(target);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr
            << "warning: incomplete " << enumToString(target)
            << " (" << enumToString(status) << ")\n";
        return;
    }

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
            dumpFramebufferAttachment(writer, context, target, attachment);
        }
    }

    glReadBuffer(read_buffer);

    if (!context.ES || context.NV_read_depth_stencil) {
        dumpFramebufferAttachment(writer, context, target, GL_DEPTH_ATTACHMENT);
        dumpFramebufferAttachment(writer, context, target, GL_STENCIL_ATTACHMENT);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
}


void
dumpFramebuffer(StateWriter &writer, Context &context)
{
    writer.beginMember("framebuffer");
    writer.beginObject();

    GLint boundDrawFbo = 0, boundReadFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundDrawFbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &boundReadFbo);
    if (!boundDrawFbo) {
        dumpDrawableImages(writer, context);
    } else if (context.ES) {
        dumpFramebufferAttachments(writer, context, GL_FRAMEBUFFER);
    } else {
        GLint draw_buffer0 = GL_NONE;
        glGetIntegerv(GL_DRAW_BUFFER0, &draw_buffer0);
        bool multisample = false;

        GLint boundRb = 0;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &boundRb);

        ImageDesc colorDesc;
        /* If a draw buffer is not bound then we shouldn't query the attachment. */
        if (draw_buffer0 != 0) {
            if (getFramebufferAttachmentDesc(context, GL_DRAW_FRAMEBUFFER, draw_buffer0, colorDesc)) {
                if (colorDesc.samples) {
                    multisample = true;
                }
            }
        }

        ImageDesc depthDesc;
        if (getFramebufferAttachmentDesc(context, GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthDesc)) {
            if (depthDesc.samples) {
                multisample = true;
            }
        }

        ImageDesc stencilDesc;
        if (getFramebufferAttachmentDesc(context, GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, stencilDesc)) {
            if (stencilDesc.samples) {
                multisample = true;
            }
        }

        GLuint rbs[2 + 8];
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

        dumpFramebufferAttachments(writer, context, GL_READ_FRAMEBUFFER);

        if (multisample) {
            glBindRenderbuffer(GL_RENDERBUFFER, boundRb);
            assert(numRbs < sizeof rbs / sizeof rbs[0]);
            glDeleteRenderbuffers(numRbs, rbs);
            glDeleteFramebuffers(1, &fboCopy);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, boundReadFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundDrawFbo);
    }

    writer.endObject();
    writer.endMember(); // framebuffer
}


} /* namespace glstate */

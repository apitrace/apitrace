/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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


#include "glstate_internal.hpp"
#include "glstate.hpp"


namespace glstate {


/**
 * Sames as enumToString, but with special provision to handle luminance/alpha
 * formats.
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


struct InternalFormatDesc
{
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};


static const InternalFormatDesc
internalFormatDescs[] = {

    {1,	GL_RED,	GL_NONE},
    {2,	GL_RG,	GL_NONE},
    {3,	GL_RGB,	GL_NONE},
    {4,	GL_RGBA,	GL_NONE},

    {GL_RED,	GL_RED,	GL_NONE},
    {GL_GREEN,	GL_GREEN,	GL_NONE},
    {GL_BLUE,	GL_BLUE,	GL_NONE},
    {GL_ALPHA,	GL_ALPHA,	GL_NONE},
    {GL_RG,	GL_RG,	GL_NONE},
    {GL_RGB,	GL_RGB,	GL_NONE},
    {GL_BGR,	GL_RGB,	GL_NONE},
    {GL_RGBA,	GL_RGBA,	GL_NONE},
    {GL_BGRA,	GL_RGBA,	GL_NONE},
    {GL_LUMINANCE,	GL_LUMINANCE,	GL_NONE},
    {GL_LUMINANCE_ALPHA,	GL_LUMINANCE_ALPHA,	GL_NONE},
    {GL_INTENSITY,	GL_INTENSITY,	GL_NONE},

    {GL_RG8,	GL_RG,	GL_UNSIGNED_BYTE},
    {GL_RG16,	GL_RG,	GL_UNSIGNED_SHORT},
    {GL_RGB8,	GL_RGB,	GL_UNSIGNED_BYTE},
    {GL_RGB16,	GL_RGB,	GL_UNSIGNED_SHORT},
    {GL_RGBA8,	GL_RGBA,	GL_UNSIGNED_BYTE},
    {GL_RGBA16,	GL_RGBA,	GL_UNSIGNED_SHORT},
    {GL_RGB10_A2,	GL_RGBA,	GL_NONE},
    {GL_LUMINANCE8,	GL_LUMINANCE,	GL_UNSIGNED_BYTE},
    {GL_LUMINANCE16,	GL_LUMINANCE,	GL_UNSIGNED_SHORT},
    {GL_ALPHA8,	GL_ALPHA,	GL_UNSIGNED_BYTE},
    {GL_ALPHA16,	GL_ALPHA,	GL_UNSIGNED_SHORT},
    {GL_LUMINANCE8_ALPHA8,	GL_LUMINANCE_ALPHA,	GL_UNSIGNED_BYTE},
    {GL_LUMINANCE16_ALPHA16,	GL_LUMINANCE_ALPHA,	GL_UNSIGNED_SHORT},
    {GL_INTENSITY8,	GL_INTENSITY,	GL_UNSIGNED_BYTE},
    {GL_INTENSITY16,	GL_INTENSITY,	GL_UNSIGNED_SHORT},

    {GL_RG16F,	GL_RG,	GL_HALF_FLOAT},
    {GL_RG32F,	GL_RG,	GL_FLOAT},
    {GL_RGB16F,	GL_RGB,	GL_HALF_FLOAT},
    {GL_RGB32F,	GL_RGB,	GL_FLOAT},
    {GL_RGBA16F,	GL_RGBA,	GL_HALF_FLOAT},
    {GL_RGBA32F,	GL_RGBA,	GL_FLOAT},
    {GL_RGB10_A2,	GL_RGBA,	GL_NONE},
    {GL_LUMINANCE16F_ARB,	GL_LUMINANCE,	GL_HALF_FLOAT},
    {GL_LUMINANCE32F_ARB,	GL_LUMINANCE,	GL_FLOAT},
    {GL_ALPHA16F_ARB,	GL_ALPHA,	GL_HALF_FLOAT},
    {GL_ALPHA32F_ARB,	GL_ALPHA,	GL_FLOAT},
    {GL_LUMINANCE_ALPHA16F_ARB,	GL_LUMINANCE_ALPHA,	GL_HALF_FLOAT},
    {GL_LUMINANCE_ALPHA32F_ARB,	GL_LUMINANCE_ALPHA,	GL_FLOAT},
    {GL_INTENSITY16F_ARB,	GL_INTENSITY,	GL_HALF_FLOAT},
    {GL_INTENSITY32F_ARB,	GL_INTENSITY,	GL_FLOAT},

    {GL_RED_INTEGER,	GL_RED_INTEGER,	GL_NONE},
    {GL_GREEN_INTEGER,	GL_GREEN_INTEGER,	GL_NONE},
    {GL_BLUE_INTEGER,	GL_BLUE_INTEGER,	GL_NONE},
    {GL_ALPHA_INTEGER,	GL_ALPHA_INTEGER,	GL_NONE},
    {GL_RG_INTEGER,	GL_RG_INTEGER,	GL_NONE},
    {GL_RGB_INTEGER,	GL_RGB_INTEGER,	GL_NONE},
    {GL_BGR_INTEGER,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGBA_INTEGER,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_BGRA_INTEGER,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_LUMINANCE_INTEGER_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},

    {GL_R8I,	GL_RED_INTEGER,	GL_NONE},
    {GL_R8UI,	GL_RED_INTEGER,	GL_NONE},
    {GL_R16I,	GL_RED_INTEGER,	GL_NONE},
    {GL_R16UI,	GL_RED_INTEGER,	GL_NONE},
    {GL_R32I,	GL_RED_INTEGER,	GL_NONE},
    {GL_R32UI,	GL_RED_INTEGER,	GL_NONE},
    {GL_RG8I,	GL_RG_INTEGER,	GL_NONE},
    {GL_RG8UI,	GL_RG_INTEGER,	GL_NONE},
    {GL_RG16I,	GL_RG_INTEGER,	GL_NONE},
    {GL_RG16UI,	GL_RG_INTEGER,	GL_NONE},
    {GL_RG32I,	GL_RG_INTEGER,	GL_NONE},
    {GL_RG32UI,	GL_RG_INTEGER,	GL_NONE},
    {GL_RGB8I,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGB8UI,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGB16I,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGB16UI,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGB32I,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGB32UI,	GL_RGB_INTEGER,	GL_NONE},
    {GL_RGBA8I,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_RGBA8UI,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_RGBA16I,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_RGBA16UI,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_RGBA32I,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_RGBA32UI,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_RGB10_A2UI,	GL_RGBA_INTEGER,	GL_NONE},
    {GL_LUMINANCE8I_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE8UI_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE16I_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE16UI_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE32I_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE32UI_EXT,	GL_LUMINANCE_INTEGER_EXT,	GL_NONE},
    {GL_ALPHA8I_EXT,	GL_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_ALPHA8UI_EXT,	GL_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_ALPHA16I_EXT,	GL_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_ALPHA16UI_EXT,	GL_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_ALPHA32I_EXT,	GL_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_ALPHA32UI_EXT,	GL_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA8I_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA8UI_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA16I_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA16UI_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA32I_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_LUMINANCE_ALPHA32UI_EXT,	GL_LUMINANCE_ALPHA_INTEGER_EXT,	GL_NONE},
    {GL_INTENSITY8I_EXT,	GL_RED_INTEGER,	GL_NONE},
    {GL_INTENSITY8UI_EXT,	GL_RED_INTEGER,	GL_NONE},
    {GL_INTENSITY16I_EXT,	GL_RED_INTEGER,	GL_NONE},
    {GL_INTENSITY16UI_EXT,	GL_RED_INTEGER,	GL_NONE},
    {GL_INTENSITY32I_EXT,	GL_RED_INTEGER,	GL_NONE},
    {GL_INTENSITY32UI_EXT,	GL_RED_INTEGER,	GL_NONE},

    {GL_DEPTH_COMPONENT,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH_COMPONENT16,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH_COMPONENT32,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH_COMPONENT32F,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH_COMPONENT32F_NV,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH_STENCIL,	        GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH24_STENCIL8,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH32F_STENCIL8,	GL_DEPTH_COMPONENT,	GL_NONE},
    {GL_DEPTH32F_STENCIL8_NV,	GL_DEPTH_COMPONENT,	GL_NONE},
};


bool
getInternalFormatType(GLenum internalFormat, GLenum &format, GLenum &type)
{
    for (unsigned i = 0; i < sizeof internalFormatDescs / sizeof internalFormatDescs[0]; ++i) {
        if (internalFormatDescs[i].internalFormat == internalFormat) {
            format = internalFormatDescs[i].format;
            type = internalFormatDescs[i].type;
            return true;
        }
    }

    return false;
}


/**
 * Choose the glReadPixels/glGetTexImage format appropriate for the given
 * internalFormat.
 */
GLenum
getFormat(GLenum internalFormat)
{
    GLenum format;
    GLenum type;
    if (getInternalFormatType(internalFormat, format, type)) {
        return format;
    }
    return GL_RGBA;
}


} /* namespace glstate */

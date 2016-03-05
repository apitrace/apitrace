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

#include <assert.h>

#include <iostream>
#include <limits>
#include <type_traits>

#include "glsize.hpp"
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


static const InternalFormatDesc
internalFormatDescs[] = {
    // GLenum internalFormat,                  GLenum format,                   GLenum type,                        GLenum readType

    // Unsized UNORM
    { 1,                                       GL_RED,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { 2,                                       GL_RG,                           GL_NONE,                            GL_UNSIGNED_BYTE },
    { 3,                                       GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { 4,                                       GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RED,                                  GL_RED,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_GREEN,                                GL_GREEN,                        GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_BLUE,                                 GL_BLUE,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RG,                                   GL_RG,                           GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGB,                                  GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_BGR,                                  GL_BGR,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGBA,                                 GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_BGRA,                                 GL_BGRA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE,                            GL_LUMINANCE,                    GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_ALPHA,                                GL_ALPHA,                        GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA,                      GL_LUMINANCE_ALPHA,              GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_INTENSITY,                            GL_INTENSITY,                    GL_NONE,                            GL_UNSIGNED_BYTE },

    // Unsized SNORM
    { GL_RED_SNORM,                            GL_RED,                          GL_NONE,                            GL_FLOAT },
    { GL_RG_SNORM,                             GL_RG,                           GL_NONE,                            GL_FLOAT },
    { GL_RGB_SNORM,                            GL_RGB,                          GL_NONE,                            GL_FLOAT },
    { GL_RGBA_SNORM,                           GL_RGBA,                         GL_NONE,                            GL_FLOAT },
    { GL_LUMINANCE_SNORM,                      GL_LUMINANCE,                    GL_NONE,                            GL_FLOAT },
    { GL_ALPHA_SNORM,                          GL_ALPHA,                        GL_NONE,                            GL_FLOAT },
    { GL_LUMINANCE_ALPHA_SNORM,                GL_LUMINANCE_ALPHA,              GL_NONE,                            GL_FLOAT },
    { GL_INTENSITY_SNORM,                      GL_INTENSITY,                    GL_NONE,                            GL_FLOAT },

    // 4-bit UNORM
    { GL_RGB4,                                 GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGBA4,                                GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE4,                           GL_LUMINANCE,                    GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_ALPHA4,                               GL_ALPHA,                        GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE4_ALPHA4,                    GL_LUMINANCE_ALPHA,              GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_INTENSITY4,                           GL_INTENSITY,                    GL_NONE,                            GL_UNSIGNED_BYTE },

    // 8-bit UNORM
    { GL_R8,                                   GL_RED,                          GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_RG8,                                  GL_RG,                           GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_RGB8,                                 GL_RGB,                          GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_RGBA8,                                GL_RGBA,                         GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_BGRA8_EXT,                            GL_BGRA,                         GL_UNSIGNED_INT_8_8_8_8_REV,        GL_UNSIGNED_BYTE },
    { GL_LUMINANCE8,                           GL_LUMINANCE,                    GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_ALPHA8,                               GL_ALPHA,                        GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_LUMINANCE8_ALPHA8,                    GL_LUMINANCE_ALPHA,              GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_INTENSITY8,                           GL_INTENSITY,                    GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },

    // 8-bit SNORM
    { GL_R8_SNORM,                             GL_RED,                          GL_BYTE,                            GL_FLOAT },
    { GL_RG8_SNORM,                            GL_RG,                           GL_BYTE,                            GL_FLOAT },
    { GL_RGB8_SNORM,                           GL_RGB,                          GL_BYTE,                            GL_FLOAT },
    { GL_RGBA8_SNORM,                          GL_RGBA,                         GL_BYTE,                            GL_FLOAT },
    { GL_LUMINANCE8_SNORM,                     GL_LUMINANCE,                    GL_BYTE,                            GL_FLOAT },
    { GL_ALPHA8_SNORM,                         GL_ALPHA,                        GL_BYTE,                            GL_FLOAT },
    { GL_LUMINANCE8_ALPHA8_SNORM,              GL_LUMINANCE_ALPHA,              GL_BYTE,                            GL_FLOAT },
    { GL_INTENSITY8_SNORM,                     GL_INTENSITY,                    GL_BYTE,                            GL_FLOAT },

    // Other UNORM
    { GL_R3_G3_B2,                             GL_RGB,                          GL_UNSIGNED_BYTE_2_3_3_REV,         GL_UNSIGNED_BYTE },
    { GL_RGB4,                                 GL_RGB,                          GL_UNSIGNED_SHORT_4_4_4_4_REV,      GL_UNSIGNED_BYTE },
    { GL_RGB5,                                 GL_RGB,                          GL_UNSIGNED_SHORT_1_5_5_5_REV,      GL_UNSIGNED_BYTE },
    { GL_RGB10,                                GL_RGB,                          GL_UNSIGNED_INT_2_10_10_10_REV,     GL_UNSIGNED_BYTE },
    { GL_RGB12,                                GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGBA2,                                GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGBA4,                                GL_RGBA,                         GL_UNSIGNED_SHORT_4_4_4_4_REV,      GL_UNSIGNED_BYTE },
    { GL_RGB5_A1,                              GL_RGBA,                         GL_UNSIGNED_SHORT_1_5_5_5_REV,      GL_UNSIGNED_BYTE },
    { GL_RGB10_A2,                             GL_RGBA,                         GL_UNSIGNED_INT_2_10_10_10_REV,     GL_UNSIGNED_BYTE },
    { GL_RGBA12,                               GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },

    // 16-bit UNORM
    { GL_R16,                                  GL_RED,                          GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_RG16,                                 GL_RG,                           GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_RGB16,                                GL_RGB,                          GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_RGBA16,                               GL_RGBA,                         GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_LUMINANCE16,                          GL_LUMINANCE,                    GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_ALPHA16,                              GL_ALPHA,                        GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_LUMINANCE16_ALPHA16,                  GL_LUMINANCE_ALPHA,              GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_INTENSITY16,                          GL_INTENSITY,                    GL_UNSIGNED_SHORT,                  GL_FLOAT },

    // 16-bit SNORM
    { GL_R16_SNORM,                            GL_RED,                          GL_SHORT,                           GL_FLOAT },
    { GL_RG16_SNORM,                           GL_RG,                           GL_SHORT,                           GL_FLOAT },
    { GL_RGB16_SNORM,                          GL_RGB,                          GL_SHORT,                           GL_FLOAT },
    { GL_RGBA16_SNORM,                         GL_RGBA,                         GL_SHORT,                           GL_FLOAT },
    { GL_LUMINANCE16_SNORM,                    GL_LUMINANCE,                    GL_SHORT,                           GL_FLOAT },
    { GL_ALPHA16_SNORM,                        GL_ALPHA,                        GL_SHORT,                           GL_FLOAT },
    { GL_LUMINANCE16_ALPHA16_SNORM,            GL_LUMINANCE_ALPHA,              GL_SHORT,                           GL_FLOAT },
    { GL_INTENSITY16_SNORM,                    GL_INTENSITY,                    GL_SHORT,                           GL_FLOAT },

    // 16-bit float
    { GL_R16F,                                 GL_RED,                          GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_RG16F,                                GL_RG,                           GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_RGB16F,                               GL_RGB,                          GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_RGBA16F,                              GL_RGBA,                         GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_LUMINANCE16F_ARB,                     GL_LUMINANCE,                    GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_ALPHA16F_ARB,                         GL_ALPHA,                        GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_LUMINANCE_ALPHA16F_ARB,               GL_LUMINANCE_ALPHA,              GL_HALF_FLOAT,                      GL_FLOAT },
    { GL_INTENSITY16F_ARB,                     GL_INTENSITY,                    GL_HALF_FLOAT,                      GL_FLOAT },

    // 32-bit float
    { GL_R32F,                                 GL_RED,                          GL_FLOAT,                           GL_FLOAT },
    { GL_RG32F,                                GL_RG,                           GL_FLOAT,                           GL_FLOAT },
    { GL_RGB32F,                               GL_RGB,                          GL_FLOAT,                           GL_FLOAT },
    { GL_RGBA32F,                              GL_RGBA,                         GL_FLOAT,                           GL_FLOAT },
    { GL_LUMINANCE32F_ARB,                     GL_LUMINANCE,                    GL_FLOAT,                           GL_FLOAT },
    { GL_ALPHA32F_ARB,                         GL_ALPHA,                        GL_FLOAT,                           GL_FLOAT },
    { GL_LUMINANCE_ALPHA32F_ARB,               GL_LUMINANCE_ALPHA,              GL_FLOAT,                           GL_FLOAT },
    { GL_INTENSITY32F_ARB,                     GL_INTENSITY,                    GL_FLOAT,                           GL_FLOAT },

    // Other float
    { GL_R11F_G11F_B10F,                       GL_RGB,                          GL_UNSIGNED_INT_10F_11F_11F_REV,    GL_FLOAT },
    { GL_RGB9_E5,                              GL_RGB,                          GL_UNSIGNED_INT_5_9_9_9_REV,        GL_FLOAT },

    // Unsized integer
    // FIXME: currently we can only dump integers as GL_UNSIGNED_BYTE
    { GL_RED_INTEGER,                          GL_RED_INTEGER,                  GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_GREEN_INTEGER,                        GL_GREEN_INTEGER,                GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_BLUE_INTEGER,                         GL_BLUE_INTEGER,                 GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_ALPHA_INTEGER,                        GL_ALPHA_INTEGER,                GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RG_INTEGER,                           GL_RG_INTEGER,                   GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGB_INTEGER,                          GL_RGB_INTEGER,                  GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_BGR_INTEGER,                          GL_BGR_INTEGER,                  GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_RGBA_INTEGER,                         GL_RGBA_INTEGER,                 GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_BGRA_INTEGER,                         GL_BGRA_INTEGER,                 GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_INTEGER_EXT,                GL_LUMINANCE_INTEGER_EXT,        GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA_INTEGER_EXT,          GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_NONE,                            GL_UNSIGNED_BYTE },

    // 8-bit unsigned integer
    { GL_R8UI,                                 GL_RED_INTEGER,                  GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_RG8UI,                                GL_RG_INTEGER,                   GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_RGB8UI,                               GL_RGB_INTEGER,                  GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_RGBA8UI,                              GL_RGBA_INTEGER,                 GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_LUMINANCE8UI_EXT,                     GL_LUMINANCE_INTEGER_EXT,        GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_ALPHA8UI_EXT,                         GL_ALPHA_INTEGER_EXT,            GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA8UI_EXT,               GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_INTENSITY8UI_EXT,                     GL_RED_INTEGER,                  GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },

    // 8-bit signed integer
    { GL_R8I,                                  GL_RED_INTEGER,                  GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_RG8I,                                 GL_RG_INTEGER,                   GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_RGB8I,                                GL_RGB_INTEGER,                  GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_RGBA8I,                               GL_RGBA_INTEGER,                 GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE8I_EXT,                      GL_LUMINANCE_INTEGER_EXT,        GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_ALPHA8I_EXT,                          GL_ALPHA_INTEGER_EXT,            GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA8I_EXT,                GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_BYTE,                            GL_UNSIGNED_BYTE },
    { GL_INTENSITY8I_EXT,                      GL_RED_INTEGER,                  GL_BYTE,                            GL_UNSIGNED_BYTE },

    // 10-bit unsigned integer
    { GL_RGB10_A2UI,                           GL_RGBA_INTEGER,                 GL_UNSIGNED_INT_2_10_10_10_REV,     GL_UNSIGNED_BYTE },

    // 16-bit unsigned integer
    { GL_R16UI,                                GL_RED_INTEGER,                  GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_RG16UI,                               GL_RG_INTEGER,                   GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_RGB16UI,                              GL_RGB_INTEGER,                  GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_RGBA16UI,                             GL_RGBA_INTEGER,                 GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_LUMINANCE16UI_EXT,                    GL_LUMINANCE_INTEGER_EXT,        GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_ALPHA16UI_EXT,                        GL_ALPHA_INTEGER_EXT,            GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA16UI_EXT,              GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
    { GL_INTENSITY16UI_EXT,                    GL_RED_INTEGER,                  GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },

    // 16-bit signed integer
    { GL_R16I,                                 GL_RED_INTEGER,                  GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_RG16I,                                GL_RG_INTEGER,                   GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_RGB16I,                               GL_RGB_INTEGER,                  GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_RGBA16I,                              GL_RGBA_INTEGER,                 GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_LUMINANCE16I_EXT,                     GL_LUMINANCE_INTEGER_EXT,        GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_ALPHA16I_EXT,                         GL_ALPHA_INTEGER_EXT,            GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA16I_EXT,               GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_SHORT,                           GL_UNSIGNED_BYTE },
    { GL_INTENSITY16I_EXT,                     GL_RED_INTEGER,                  GL_SHORT,                           GL_UNSIGNED_BYTE },

    // 32-bit unsigned integer
    { GL_R32UI,                                GL_RED_INTEGER,                  GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_RG32UI,                               GL_RG_INTEGER,                   GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_RGB32UI,                              GL_RGB_INTEGER,                  GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_RGBA32UI,                             GL_RGBA_INTEGER,                 GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_LUMINANCE32UI_EXT,                    GL_LUMINANCE_INTEGER_EXT,        GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_ALPHA32UI_EXT,                        GL_ALPHA_INTEGER_EXT,            GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA32UI_EXT,              GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },
    { GL_INTENSITY32UI_EXT,                    GL_RED_INTEGER,                  GL_UNSIGNED_INT,                    GL_UNSIGNED_BYTE },

    // 32-bit signed integer
    { GL_R32I,                                 GL_RED_INTEGER,                  GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_RG32I,                                GL_RG_INTEGER,                   GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_RGB32I,                               GL_RGB_INTEGER,                  GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_RGBA32I,                              GL_RGBA_INTEGER,                 GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_LUMINANCE32I_EXT,                     GL_LUMINANCE_INTEGER_EXT,        GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_ALPHA32I_EXT,                         GL_ALPHA_INTEGER_EXT,            GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_LUMINANCE_ALPHA32I_EXT,               GL_LUMINANCE_ALPHA_INTEGER_EXT,  GL_INT,                             GL_UNSIGNED_BYTE },
    { GL_INTENSITY32I_EXT,                     GL_RED_INTEGER,                  GL_INT,                             GL_UNSIGNED_BYTE },

    // Compressed
    { GL_COMPRESSED_RED,                       GL_RED,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RED_RGTC1,                 GL_RED,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_R11_EAC,                   GL_RED,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RG,                        GL_RG,                           GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RG_RGTC2,                  GL_RG,                           GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RG11_EAC,                  GL_RG,                           GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGB,                       GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGB_S3TC_DXT1_EXT,         GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGB8_ETC2,                 GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGBA,                      GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,        GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,        GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,        GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGBA8_ETC2_EAC,            GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA,                     GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_LUMINANCE,                 GL_LUMINANCE,                    GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_LUMINANCE_ALPHA,           GL_LUMINANCE_ALPHA,              GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_LUMINANCE_LATC1_EXT,       GL_LUMINANCE,                    GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, GL_LUMINANCE_ALPHA,              GL_NONE,                            GL_UNSIGNED_BYTE },

    // Compressed signed
    { GL_COMPRESSED_SIGNED_RED_RGTC1,          GL_RED,                          GL_NONE,                            GL_FLOAT },
    { GL_COMPRESSED_SIGNED_R11_EAC,            GL_RED,                          GL_NONE,                            GL_FLOAT },
    { GL_COMPRESSED_SIGNED_RG_RGTC2,           GL_RG,                           GL_NONE,                            GL_FLOAT },
    { GL_COMPRESSED_SIGNED_RG11_EAC,           GL_RG,                           GL_NONE,                            GL_FLOAT },
    { GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT, GL_LUMINANCE,                   GL_NONE,                            GL_FLOAT },
    { GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT, GL_LUMINANCE_ALPHA,       GL_NONE,                            GL_FLOAT },

    // SRGB
    { GL_SRGB,                                 GL_RGB,                          GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SRGB8,                                GL_RGB,                          GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SRGB_ALPHA,                           GL_RGBA,                         GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SRGB8_ALPHA8,                         GL_RGBA,                         GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SLUMINANCE_ALPHA,                     GL_LUMINANCE_ALPHA,              GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SLUMINANCE8_ALPHA8,                   GL_LUMINANCE_ALPHA,              GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SLUMINANCE,                           GL_LUMINANCE,                    GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_SLUMINANCE8,                          GL_LUMINANCE,                    GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SRGB,                      GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SRGB_ALPHA,                GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SLUMINANCE,                GL_LUMINANCE,                    GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SLUMINANCE_ALPHA,          GL_LUMINANCE_ALPHA,              GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,        GL_RGB,                          GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,  GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,  GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,  GL_RGBA,                         GL_NONE,                            GL_UNSIGNED_BYTE },

    // depth-stencil
    { GL_DEPTH_COMPONENT,                      GL_DEPTH_COMPONENT,              GL_NONE,                            GL_FLOAT },
    { GL_DEPTH_COMPONENT16,                    GL_DEPTH_COMPONENT,              GL_UNSIGNED_SHORT,                  GL_FLOAT },
    { GL_DEPTH_COMPONENT24,                    GL_DEPTH_COMPONENT,              GL_NONE,                            GL_FLOAT },
    { GL_DEPTH_COMPONENT32,                    GL_DEPTH_COMPONENT,              GL_UNSIGNED_INT,                    GL_FLOAT },
    { GL_DEPTH_COMPONENT32F,                   GL_DEPTH_COMPONENT,              GL_FLOAT,                           GL_FLOAT },
    { GL_DEPTH_COMPONENT32F_NV,                GL_DEPTH_COMPONENT,              GL_FLOAT,                           GL_FLOAT },
    { GL_DEPTH_STENCIL,                        GL_DEPTH_STENCIL,                GL_NONE,                            GL_FLOAT },
    { GL_DEPTH24_STENCIL8,                     GL_DEPTH_STENCIL,                GL_UNSIGNED_INT_24_8_EXT,           GL_FLOAT },
    { GL_DEPTH32F_STENCIL8,                    GL_DEPTH_STENCIL,                GL_FLOAT_32_UNSIGNED_INT_24_8_REV,  GL_FLOAT },
    { GL_DEPTH32F_STENCIL8_NV,                 GL_DEPTH_STENCIL,                GL_FLOAT_32_UNSIGNED_INT_24_8_REV,  GL_FLOAT },
    { GL_STENCIL_INDEX,                        GL_STENCIL_INDEX,                GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_STENCIL_INDEX1,                       GL_STENCIL_INDEX,                GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_STENCIL_INDEX4,                       GL_STENCIL_INDEX,                GL_NONE,                            GL_UNSIGNED_BYTE },
    { GL_STENCIL_INDEX8,                       GL_STENCIL_INDEX,                GL_UNSIGNED_BYTE,                   GL_UNSIGNED_BYTE },
    { GL_STENCIL_INDEX16,                      GL_STENCIL_INDEX,                GL_UNSIGNED_SHORT,                  GL_UNSIGNED_BYTE },
};


static const InternalFormatDesc
defaultInternalFormatDesc = {
    GL_RGBA, GL_RGBA, GL_NONE, GL_FLOAT
};


const InternalFormatDesc &
getInternalFormatDesc(GLenum internalFormat)
{
    for (auto & internalFormatDesc : internalFormatDescs) {
        if (internalFormatDesc.internalFormat == internalFormat) {
            return internalFormatDesc;
        }
    }

    std::cerr << "warning: unexpected internalFormat " << enumToString(internalFormat) << "\n";

    return defaultInternalFormatDesc;
}


/**
 * Choose appropriate format/type for glReadPixels()/glGetTexImage()
 */
void
chooseReadBackFormat(const InternalFormatDesc &formatDesc, GLenum &format, GLenum &type)
{
    /* TODO: leverage glGetInternalformat where available */

    type = formatDesc.readType;
    assert(type == GL_UNSIGNED_BYTE || type == GL_FLOAT);

    switch (formatDesc.format) {
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_RG:
    case GL_RGB:
    case GL_BGR:
        format = GL_RGB;
        break;
    case GL_ALPHA:
    case GL_RGBA:
    case GL_BGRA:
    case GL_INTENSITY:
        format = GL_RGBA;
        break;
    case GL_LUMINANCE:
        format = GL_LUMINANCE;
        break;
    case GL_LUMINANCE_ALPHA:
        format = GL_LUMINANCE_ALPHA;
        break;

    case GL_RED_INTEGER:
    case GL_GREEN_INTEGER:
    case GL_BLUE_INTEGER:
    case GL_RG_INTEGER:
    case GL_RGB_INTEGER:
    case GL_BGR_INTEGER:
    case GL_ALPHA_INTEGER:
    case GL_RGBA_INTEGER:
    case GL_BGRA_INTEGER:
        format = GL_RGBA_INTEGER;
        break;
    case GL_LUMINANCE_INTEGER_EXT:
        format = GL_LUMINANCE_INTEGER_EXT;
        break;
    case GL_LUMINANCE_ALPHA_INTEGER_EXT:
        format = GL_LUMINANCE_ALPHA_INTEGER_EXT;
        break;

    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_STENCIL:
        /* TODO: allow pick stencil component too*/
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
        break;
    case GL_STENCIL_INDEX:
        format = GL_STENCIL_INDEX;
        type = GL_UNSIGNED_BYTE;
        break;

    default:
        assert(0);
        format = GL_RGBA;
        type = GL_FLOAT;
        break;
    }
}


void
getImageFormat(GLenum format, GLenum type,
               GLuint & channels, image::ChannelType & channelType)
{
    channels = _gl_format_channels(format);

    switch (type) {
    case GL_UNSIGNED_BYTE:
        channelType = image::TYPE_UNORM8;
        break;
    case GL_FLOAT:
        channelType = image::TYPE_FLOAT;
        break;
    default:
        assert(0);
    }
}


// Macros for describing arbitrary swizzles

#define SWIZZLE_X 0
#define SWIZZLE_Y 1
#define SWIZZLE_Z 2
#define SWIZZLE_W 3
#define SWIZZLE_0 4
#define SWIZZLE_1 5
#define SWIZZLE_COUNT 6

#define SWIZZLE_BITS 4
#define SWIZZLE_MASK ((1 << SWIZZLE_BITS) - 1)

#define SWIZZLE(x, y, z, w) \
    (((SWIZZLE_##x) << (SWIZZLE_BITS*0)) | \
     ((SWIZZLE_##y) << (SWIZZLE_BITS*1)) | \
     ((SWIZZLE_##z) << (SWIZZLE_BITS*2)) | \
     ((SWIZZLE_##w) << (SWIZZLE_BITS*3)))

#define SWIZZLE_RGBA SWIZZLE(X,Y,Z,W)
#define SWIZZLE_RGB  SWIZZLE(X,Y,Z,1)
#define SWIZZLE_RG   SWIZZLE(X,Y,0,1)
#define SWIZZLE_R    SWIZZLE(X,0,0,1)
#define SWIZZLE_LA   SWIZZLE(X,X,X,Y)
#define SWIZZLE_L    SWIZZLE(X,X,X,1)
#define SWIZZLE_A    SWIZZLE(0,0,0,X)
#define SWIZZLE_I    SWIZZLE(X,X,X,X)

#define SWIZZLE_EXTRACT(swizzle, channel) \
    ((swizzle) >> (SWIZZLE_BITS * (channel)) & SWIZZLE_MASK)


// Template that can describe all regular array-based pixel formats
template< typename Type, unsigned components, bool normalized, uint16_t swizzle >
class PixelTemplate : public PixelFormat
{
protected:

    static_assert( 1 <= components && components <= 4, "invalid number of components" );

    static const unsigned swizzle_r = SWIZZLE_EXTRACT(swizzle, 0);
    static const unsigned swizzle_g = SWIZZLE_EXTRACT(swizzle, 1);
    static const unsigned swizzle_b = SWIZZLE_EXTRACT(swizzle, 2);
    static const unsigned swizzle_a = SWIZZLE_EXTRACT(swizzle, 3);

    static_assert( swizzle_r < components || swizzle_r == SWIZZLE_0 || swizzle_r == SWIZZLE_1, "invalid R swizzle" );
    static_assert( swizzle_g < components || swizzle_g == SWIZZLE_0 || swizzle_g == SWIZZLE_1, "invalid G swizzle" );
    static_assert( swizzle_b < components || swizzle_b == SWIZZLE_0 || swizzle_b == SWIZZLE_1, "invalid B swizzle" );
    static_assert( swizzle_a < components || swizzle_a == SWIZZLE_0 || swizzle_a == SWIZZLE_1, "invalid A swizzle" );

    // We must use double precision intermediate values when normalizing 32bits integers.
    typedef typename std::conditional< normalized && sizeof(Type) >= 4 , double , float >::type Scale;

    // Scale normalized types
    template<typename T = void>
    static inline Scale
    scale(Scale value, typename std::enable_if<normalized, T>::type* = 0)
    {
        static_assert( normalized, "should only be instantiated for normalized types" );
#ifndef _MSC_VER
        static constexpr Type typeMax = std::numeric_limits<Type>::max();
        static_assert( static_cast<Type>(static_cast<Scale>(typeMax)) == typeMax,
                       "intermediate type cannot represent maximum value without loss of precission" );
        static constexpr Scale scaleFactor = Scale(1) / Scale(typeMax);
        static_assert( Scale(typeMax) * scaleFactor == Scale(1), "cannot represent unity" );
        static_assert( Scale(0) * scaleFactor == Scale(0), "cannot represent zero" );
#else
        // XXX: MSCV doesn't support constexpr yet
        static const Type typeMax = std::numeric_limits<Type>::max();
        assert( static_cast<Type>(static_cast<Scale>(typeMax)) == typeMax );
        static const Scale scaleFactor = Scale(1) / Scale(typeMax);
        assert( Scale(typeMax) * scaleFactor == Scale(1) );
        assert( Scale(0) * scaleFactor == Scale(0) );
#endif
        return value * scaleFactor;
    }

    // No-op for unormalized types
    template<typename T = void>
    static inline Scale
    scale(Scale value, typename std::enable_if<!normalized, T>::type* = 0)
    {
        static_assert( !normalized, "should only be instantiated for non-normalized types" );
        return value;
    }

    // Unpack a single pixel
    static inline void
    unpackPixel(const Type *inPixel, float outPixel[4])
    {
        float scaledComponents[SWIZZLE_COUNT];
        for (unsigned component = 0; component < components; ++component) {
            Scale scaledComponent = scale(static_cast<Scale>(inPixel[component]));
            scaledComponents[component] = static_cast<float>(scaledComponent);
        }
        scaledComponents[SWIZZLE_0] = 0.0f;
        scaledComponents[SWIZZLE_1] = 1.0f;

        outPixel[0] = scaledComponents[swizzle_r];
        outPixel[1] = scaledComponents[swizzle_g];
        outPixel[2] = scaledComponents[swizzle_b];
        outPixel[3] = scaledComponents[swizzle_a];
    }

public:

    inline
    PixelTemplate(void) {
    }

    size_t
    size(void) const override {
        return sizeof(Type) * components;
    }

    void
    unpackSpan(const uint8_t *inSpan, float *outSpan, unsigned width) const override
    {
        const Type *inPixel = reinterpret_cast<const Type *>(inSpan);

        for (unsigned x = 0; x < width; ++x) {
            unpackPixel(inPixel, outSpan);
            inPixel += components;
            outSpan += 4;
        }
    }
};


const PixelFormat *
getPixelFormat(GLenum internalFormat)
{
    static const bool Y = true;
    static const bool N = false;

#define CASE(internalFormat, type, components, norm, swizzle) \
    case GL_##internalFormat: \
        { \
            static const PixelTemplate< GL##type, components, norm, SWIZZLE_##swizzle > pixel; \
            return &pixel; \
        }

    switch (internalFormat) {

    CASE(ALPHA8,                  ubyte,  1, Y, A);
    CASE(ALPHA16,                 ushort, 1, Y, A);
    CASE(ALPHA16F_ARB,            half,   1, N, A);
    CASE(ALPHA32F_ARB,            float,  1, N, A);
    CASE(ALPHA8I_EXT,             byte,   1, N, A);
    CASE(ALPHA16I_EXT,            short,  1, N, A);
    CASE(ALPHA32I_EXT,            int,    1, N, A);
    CASE(ALPHA8UI_EXT,            ubyte,  1, N, A);
    CASE(ALPHA16UI_EXT,           ushort, 1, N, A);
    CASE(ALPHA32UI_EXT,           uint,   1, N, A);

    CASE(LUMINANCE8,              ubyte,  1, Y, L);
    CASE(LUMINANCE16,             ushort, 1, Y, L);
    CASE(LUMINANCE16F_ARB,        half,   1, N, L);
    CASE(LUMINANCE32F_ARB,        float,  1, N, L);
    CASE(LUMINANCE8I_EXT,         byte,   1, N, L);
    CASE(LUMINANCE16I_EXT,        short,  1, N, L);
    CASE(LUMINANCE32I_EXT,        int,    1, N, L);
    CASE(LUMINANCE8UI_EXT,        ubyte,  1, N, L);
    CASE(LUMINANCE16UI_EXT,       ushort, 1, N, L);
    CASE(LUMINANCE32UI_EXT,       uint,   1, N, L);

    CASE(LUMINANCE8_ALPHA8,       ubyte,  2, Y, LA);
    CASE(LUMINANCE16_ALPHA16,     ushort, 2, Y, LA);
    CASE(LUMINANCE_ALPHA16F_ARB,  half,   2, N, LA);
    CASE(LUMINANCE_ALPHA32F_ARB,  float,  2, N, LA);
    CASE(LUMINANCE_ALPHA8I_EXT,   byte,   2, N, LA);
    CASE(LUMINANCE_ALPHA16I_EXT,  short,  2, N, LA);
    CASE(LUMINANCE_ALPHA32I_EXT,  int,    2, N, LA);
    CASE(LUMINANCE_ALPHA8UI_EXT,  ubyte,  2, N, LA);
    CASE(LUMINANCE_ALPHA16UI_EXT, ushort, 2, N, LA);
    CASE(LUMINANCE_ALPHA32UI_EXT, uint,   2, N, LA);

    CASE(INTENSITY8,              ubyte,  1, Y, I);
    CASE(INTENSITY16,             ushort, 1, Y, I);
    CASE(INTENSITY16F_ARB,        half,   1, N, I);
    CASE(INTENSITY32F_ARB,        float,  1, N, I);
    CASE(INTENSITY8I_EXT,         byte,   1, N, I);
    CASE(INTENSITY16I_EXT,        short,  1, N, I);
    CASE(INTENSITY32I_EXT,        int,    1, N, I);
    CASE(INTENSITY8UI_EXT,        ubyte,  1, N, I);
    CASE(INTENSITY16UI_EXT,       ushort, 1, N, I);
    CASE(INTENSITY32UI_EXT,       uint,   1, N, I);

    CASE(RGBA8,                   ubyte,  4, Y, RGBA);
    CASE(RGBA16,                  ushort, 4, Y, RGBA);
    CASE(RGBA16F,                 half,   4, N, RGBA);
    CASE(RGBA32F,                 float,  4, N, RGBA);
    CASE(RGBA8I,                  byte,   4, N, RGBA);
    CASE(RGBA16I,                 short,  4, N, RGBA);
    CASE(RGBA32I,                 int,    4, N, RGBA);
    CASE(RGBA8UI,                 ubyte,  4, N, RGBA);
    CASE(RGBA16UI,                ushort, 4, N, RGBA);
    CASE(RGBA32UI,                uint,   4, N, RGBA);

    CASE(RGB8,                    ubyte,  3, Y, RGB);
    CASE(RGB16,                   ushort, 3, Y, RGB);
    CASE(RGB16F,                  half,   3, N, RGB);
    CASE(RGB32F,                  float,  3, N, RGB);
    CASE(RGB8I,                   byte,   3, N, RGB);
    CASE(RGB16I,                  short,  3, N, RGB);
    CASE(RGB32I,                  int,    3, N, RGB);
    CASE(RGB8UI,                  ubyte,  3, N, RGB);
    CASE(RGB16UI,                 ushort, 3, N, RGB);
    CASE(RGB32UI,                 uint,   3, N, RGB);

    CASE(RG8,                     ubyte,  2, Y, RG);
    CASE(RG16,                    ushort, 2, Y, RG);
    CASE(RG16F,                   half,   2, N, RG);
    CASE(RG32F,                   float,  2, N, RG);
    CASE(RG8I,                    byte,   2, N, RG);
    CASE(RG16I,                   short,  2, N, RG);
    CASE(RG32I,                   int,    2, N, RG);
    CASE(RG8UI,                   ubyte,  2, N, RG);
    CASE(RG16UI,                  ushort, 2, N, RG);
    CASE(RG32UI,                  uint,   2, N, RG);

    CASE(R8,                      ubyte,  1, Y, R);
    CASE(R16,                     ushort, 1, Y, R);
    CASE(R16F,                    half,   1, N, R);
    CASE(R32F,                    float,  1, N, R);
    CASE(R8I,                     byte,   1, N, R);
    CASE(R16I,                    short,  1, N, R);
    CASE(R32I,                    int,    1, N, R);
    CASE(R8UI,                    ubyte,  1, N, R);
    CASE(R16UI,                   ushort, 1, N, R);
    CASE(R32UI,                   uint,   1, N, R);

    default:
        return nullptr;
    }

#undef CASE
}


} /* namespace glstate */

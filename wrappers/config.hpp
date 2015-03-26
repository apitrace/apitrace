/**************************************************************************
 *
 * Copyright 2015 Brian Paul
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


#include "glimports.hpp"

#include <string>


namespace gltrace {


struct configuration
{
    int versionMajor;
    int versionMinor;
    std::string version;
    std::string vendor;
    std::string extensions;
    std::string renderer;
    std::string glslVersion;
    GLbitfield profileMask;
    int maxTextureSize;  // 2D texture size
    int numExtensions;
    char **extensionsList;

    inline
    configuration() :
        versionMajor(0),
        versionMinor(0),
        profileMask(0),
        maxTextureSize(0),
        numExtensions(0),
        extensionsList(0)
    {}
};


const configuration *
getConfig(void);

const GLubyte *
getConfigString(const configuration *config, GLenum pname);

const GLubyte *
getConfigStringi(const configuration *config, GLenum pname, GLuint index);

GLint
getConfigInteger(const configuration *config, GLenum pname);


} /* namespace gltrace */

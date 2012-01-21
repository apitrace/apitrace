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


/*
 * Manipulation of GL extensions.
 *
 * So far we insert GREMEDY extensions, but in the future we could also clamp
 * the GL extensions to core GL versions here.
 */


#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <map>

#include "glproc.hpp"
#include "gltrace.hpp"


namespace gltrace {


typedef std::map<std::string, const char *> ExtensionsMap;

// Cache of the translated extensions strings
static ExtensionsMap extensionsMap;


// Additional extensions to be advertised
static const char *extra_extensions[] = {
    "GL_GREMEDY_string_marker",
    "GL_GREMEDY_frame_terminator",
};
#define NUM_EXTRA_EXTENSIONS (sizeof(extra_extensions)/sizeof(extra_extensions[0]))


/**
 * Translate the GL extensions string, adding new extensions.
 */
static const char *
overrideExtensionsString(const char *extensions)
{
    size_t i;

    ExtensionsMap::const_iterator it = extensionsMap.find(extensions);
    if (it != extensionsMap.end()) {
        return it->second;
    }

    size_t extensions_len = strlen(extensions);

    size_t extra_extensions_len = 0;
    for (i = 0; i < NUM_EXTRA_EXTENSIONS; ++i) {
        const char * extra_extension = extra_extensions[i];
        size_t extra_extension_len = strlen(extra_extension);
        extra_extensions_len += extra_extension_len + 1;
    }

    // We use malloc memory instead of a std::string because we need to ensure
    // that extensions strings will not move in memory as the extensionsMap is
    // updated.
    size_t new_extensions_len = extensions_len + 1 + extra_extensions_len + 1;
    char *new_extensions = (char *)malloc(new_extensions_len);
    if (!new_extensions) {
        return extensions;
    }

    if (extensions_len) {
        memcpy(new_extensions, extensions, extensions_len);

        // Add space separator if necessary
        if (new_extensions[extensions_len - 1] != ' ') {
            new_extensions[extensions_len++] = ' ';
        }
    }

    for (i = 0; i < NUM_EXTRA_EXTENSIONS; ++i) {
        const char * extra_extension = extra_extensions[i];
        size_t extra_extension_len = strlen(extra_extension);
        memcpy(new_extensions + extensions_len, extra_extension, extra_extension_len);
        extensions_len += extra_extension_len;
        new_extensions[extensions_len++] = ' ';
    }
    new_extensions[extensions_len++] = '\0';
    assert(extensions_len <= new_extensions_len);

    extensionsMap[extensions] = new_extensions;

    return new_extensions;
}


const GLubyte *
__glGetString_override(GLenum name)
{
    const GLubyte *result = __glGetString(name);

    if (result) {
        switch (name) {
        case GL_EXTENSIONS:
            result = (const GLubyte *)overrideExtensionsString((const char *)result);
            break;
        default:
            break;
        }
    }

    return result;
}


void
__glGetIntegerv_override(GLenum pname, GLint *params)
{
    __glGetIntegerv(pname, params);

    if (params) {
        switch (pname) {
        case GL_NUM_EXTENSIONS:
            *params += NUM_EXTRA_EXTENSIONS;
            break;
        default:
            break;
        }
    }
}


const GLubyte *
__glGetStringi_override(GLenum name, GLuint index)
{
    switch (name) {
    case GL_EXTENSIONS:
        {
            GLint num_extensions = 0;
            __glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
            if ((GLuint)num_extensions <= index && index < (GLuint)num_extensions + NUM_EXTRA_EXTENSIONS) {
                return (const GLubyte *)extra_extensions[index - (GLuint)num_extensions];
            }
        }
        break;
    default:
        break;
    }

    return __glGetStringi(name, index);
}


} /* namespace gltrace */


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
static const char *
extraExtension_stringsFull[] = {
    "GL_GREMEDY_string_marker",
    "GL_GREMEDY_frame_terminator",
};

static const char *
extraExtension_stringsES[] = {
    "GL_EXT_debug_marker",
};

// Description of additional extensions we want to advertise
struct ExtensionsDesc
{
    unsigned numStrings;
    const char **strings;
};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

const struct ExtensionsDesc
extraExtensionsFull = {
    ARRAY_SIZE(extraExtension_stringsFull),
    extraExtension_stringsFull
};

const struct ExtensionsDesc
extraExtensionsES = {
    ARRAY_SIZE(extraExtension_stringsES),
    extraExtension_stringsES
};


const struct ExtensionsDesc *
getExtraExtensions(void)
{
    Context *ctx = getContext();

    switch (ctx->profile) {
    case PROFILE_COMPAT:
        return &extraExtensionsFull;
    case PROFILE_ES1:
    case PROFILE_ES2:
        return &extraExtensionsES;
    default:
        assert(0);
        return &extraExtensionsFull;
    }
}


/**
 * Translate the GL extensions string, adding new extensions.
 */
static const char *
overrideExtensionsString(const char *extensions)
{
    const ExtensionsDesc *desc = getExtraExtensions();
    size_t i;

    ExtensionsMap::const_iterator it = extensionsMap.find(extensions);
    if (it != extensionsMap.end()) {
        return it->second;
    }

    size_t extensionsLen = strlen(extensions);

    size_t extraExtensionsLen = 0;
    for (i = 0; i < desc->numStrings; ++i) {
        const char * extraExtension = desc->strings[i];
        size_t extraExtensionLen = strlen(extraExtension);
        extraExtensionsLen += extraExtensionLen + 1;
    }

    // We use malloc memory instead of a std::string because we need to ensure
    // that extensions strings will not move in memory as the extensionsMap is
    // updated.
    size_t newExtensionsLen = extensionsLen + 1 + extraExtensionsLen + 1;
    char *newExtensions = (char *)malloc(newExtensionsLen);
    if (!newExtensions) {
        return extensions;
    }

    if (extensionsLen) {
        memcpy(newExtensions, extensions, extensionsLen);

        // Add space separator if necessary
        if (newExtensions[extensionsLen - 1] != ' ') {
            newExtensions[extensionsLen++] = ' ';
        }
    }

    for (i = 0; i < desc->numStrings; ++i) {
        const char * extraExtension = desc->strings[i];
        size_t extraExtensionLen = strlen(extraExtension);
        memcpy(newExtensions + extensionsLen, extraExtension, extraExtensionLen);
        extensionsLen += extraExtensionLen;
        newExtensions[extensionsLen++] = ' ';
    }
    newExtensions[extensionsLen++] = '\0';
    assert(extensionsLen <= newExtensionsLen);

    extensionsMap[extensions] = newExtensions;

    return newExtensions;
}


const GLubyte *
_glGetString_override(GLenum name)
{
    const GLubyte *result = _glGetString(name);

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
_glGetIntegerv_override(GLenum pname, GLint *params)
{
    _glGetIntegerv(pname, params);

    if (params) {
        switch (pname) {
        case GL_NUM_EXTENSIONS:
            {
                const ExtensionsDesc *desc = getExtraExtensions();
                *params += desc->numStrings;
            }
            break;
        default:
            break;
        }
    }
}


const GLubyte *
_glGetStringi_override(GLenum name, GLuint index)
{
    switch (name) {
    case GL_EXTENSIONS:
        {
            const ExtensionsDesc *desc = getExtraExtensions();
            GLint numExtensions = 0;
            _glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
            if ((GLuint)numExtensions <= index && index < (GLuint)numExtensions + desc->numStrings) {
                return (const GLubyte *)desc->strings[index - (GLuint)numExtensions];
            }
        }
        break;
    default:
        break;
    }

    return _glGetStringi(name, index);
}


} /* namespace gltrace */


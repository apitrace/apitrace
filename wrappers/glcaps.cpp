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
#include <stdio.h>

#include <string>
#include <map>

#include "glproc.hpp"
#include "gltrace.hpp"
#include "os.hpp"
#include "config.hpp"


namespace gltrace {


typedef std::map<std::string, const char *> ExtensionsMap;

// Cache of the translated extensions strings
static ExtensionsMap extensionsMap;


// Additional extensions to be advertised
static const char *
extraExtension_stringsFull[] = {
    "GL_GREMEDY_string_marker",
    "GL_GREMEDY_frame_terminator",
    "GL_ARB_debug_output",
    "GL_AMD_debug_output",
    "GL_KHR_debug",
    "GL_EXT_debug_marker",
    "GL_EXT_debug_label",
    "GL_VMWX_map_buffer_debug",
};

static const char *
extraExtension_stringsES[] = {
    "GL_KHR_debug",
    "GL_EXT_debug_marker",
    "GL_EXT_debug_label",
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
getExtraExtensions(const Context *ctx)
{
    switch (ctx->profile.api) {
    case glfeatures::API_GL:
        return &extraExtensionsFull;
    case glfeatures::API_GLES:
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
    const Context *ctx = getContext();
    const ExtensionsDesc *desc = getExtraExtensions(ctx);
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
    const configuration *config = getConfig();
    const GLubyte *result;

    // Try getting the override string value first
    result = getConfigString(config, name);
    if (!result) {
        // Ask the real GL library
        result = _glGetString(name);
    }

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


static void
getInteger(const configuration *config,
           GLenum pname, GLint *params)
{
    // Disable ARB_get_program_binary
    switch (pname) {
    case GL_NUM_PROGRAM_BINARY_FORMATS:
        if (params) {
            GLint numProgramBinaryFormats = 0;
            _glGetIntegerv(pname, &numProgramBinaryFormats);
            if (numProgramBinaryFormats > 0) {
                os::log("apitrace: warning: hiding program binary formats (https://github.com/apitrace/apitrace/issues/316)\n");
            }
            params[0] = 0;
        }
        return;
    case GL_PROGRAM_BINARY_FORMATS:
        // params might be NULL here, as we returned 0 for
        // GL_NUM_PROGRAM_BINARY_FORMATS.
        return;
    }

    if (params) {
        *params = getConfigInteger(config, pname);
        if (*params != 0) {
            return;
        }
    }

    // Ask the real GL library
    _glGetIntegerv(pname, params);
}


/**
 * TODO: To be thorough, we should override all glGet*v.
 */
void
_glGetIntegerv_override(GLenum pname, GLint *params)
{
    const configuration *config = getConfig();

    /*
     * It's important to handle params==NULL correctly here, which can and does
     * happen, particularly when pname is GL_COMPRESSED_TEXTURE_FORMATS or
     * GL_PROGRAM_BINARY_FORMATS and the implementation returns 0 for
     * GL_NUM_COMPRESSED_TEXTURE_FORMATS or GL_NUM_PROGRAM_BINARY_FORMATS, as
     * the application ends up calling `params = malloc(0)` or `param = new
     * GLint[0]` which can yield NULL.
     */

    getInteger(config, pname, params);

    if (params) {
        const Context *ctx;
        switch (pname) {
        case GL_NUM_EXTENSIONS:
            ctx = getContext();
            if (ctx->profile.major >= 3) {
                const ExtensionsDesc *desc = getExtraExtensions(ctx);
                *params += desc->numStrings;
            }
            break;
        case GL_MAX_LABEL_LENGTH:
            /* We provide our default implementation of KHR_debug when the
             * driver does not.  So return something sensible here.
             */
            if (params[0] == 0) {
                params[0] = 256;
            }
            break;
        case GL_MAX_DEBUG_MESSAGE_LENGTH:
            if (params[0] == 0) {
                params[0] = 4096;
            }
            break;
        }
    }
}


const GLubyte *
_glGetStringi_override(GLenum name, GLuint index)
{
    const configuration *config = getConfig();
    const Context *ctx = getContext();
    const GLubyte *retVal;

    if (ctx->profile.major >= 3) {
        switch (name) {
        case GL_EXTENSIONS:
            {
                const ExtensionsDesc *desc = getExtraExtensions(ctx);
                GLint numExtensions = 0;
                getInteger(config, GL_NUM_EXTENSIONS, &numExtensions);
                if ((GLuint)numExtensions <= index && index < (GLuint)numExtensions + desc->numStrings) {
                    return (const GLubyte *)desc->strings[index - (GLuint)numExtensions];
                }
            }
            break;
        default:
            break;
        }
    }

    retVal = getConfigStringi(config, name, index);
    if (retVal)
        return retVal;

    return _glGetStringi(name, index);
}


} /* namespace gltrace */


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
#include <stdlib.h>

#include <string>
#include <map>

#include "gltrace.hpp"


namespace gltrace {


typedef std::map<std::string, const char *> ExtensionsMap;

// Cache of the translated extensions strings
static ExtensionsMap extensionsMap;


// Additional extensions to be advertised
const char extra_extensions[] =
    "GL_GREMEDY_string_marker "
    "GL_GREMEDY_frame_terminator "
;


/**
 * Translate the GL extensions string, adding new extensions.
 */
const char *
translateExtensionsString(const char *extensions)
{
    ExtensionsMap::const_iterator it = extensionsMap.find(extensions);
    if (it != extensionsMap.end()) {
        return it->second;
    }

    size_t extensions_len = strlen(extensions);

    char *new_extensions = (char *)malloc(extensions_len + 1 + sizeof extra_extensions);
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

    memcpy(new_extensions + extensions_len, extra_extensions, sizeof extra_extensions);

    extensionsMap[extensions] = new_extensions;

    return new_extensions;
}


} /* namespace gltrace */


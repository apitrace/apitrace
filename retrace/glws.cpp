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


#include <assert.h>
#include <stdlib.h>

#include <iostream>

#include "glproc.hpp"
#include "glws.hpp"


namespace glws {


bool
checkExtension(const char *extName, const char *extString)
{
    assert(extName);

    if (!extString) {
        return false;
    }

    const char *p = extString;
    const char *q = extName;
    char c;
    do {
        c = *p++;
        if (c == '\0' || c == ' ') {
            if (q && *q == '\0') {
                return true;
            } else {
                q = extName;
            }
        } else {
            if (q && *q == c) {
                ++q;
            } else {
                q = 0;
            }
        }
    } while (c);
    return false;
}


void
Drawable::copySubBuffer(int x, int y, int width, int height) {
    std::cerr << "warning: copySubBuffer not yet implemented\n";
}


void
Context::initialize(void)
{
    assert(!initialized);

    actualProfile = glfeatures::getCurrentContextProfile();
    actualExtensions.getCurrentContextExtensions(actualProfile);
    actualFeatures.load(actualProfile, actualExtensions);

    /* Ensure we got a matching profile.
     *
     * In particular on MacOSX, there is no way to specify specific versions, so this is all we can do.
     *
     * Also, see if OpenGL ES can be handled through ARB_ES*_compatibility.
     */
    glfeatures::Profile expectedProfile = profile;
    if (!actualProfile.matches(expectedProfile)) {
        if (expectedProfile.api == glfeatures::API_GLES &&
            actualProfile.api == glfeatures::API_GL &&
            ((expectedProfile.major == 2 && actualExtensions.has("GL_ARB_ES2_compatibility")) ||
             (expectedProfile.major == 3 && actualExtensions.has("GL_ARB_ES3_compatibility")))) {
            std::cerr << "warning: context mismatch:"
                      << " expected " << expectedProfile << ","
                      << " but got " << actualProfile << " with GL_ARB_ES" << expectedProfile.major << "_compatibility\n";
        } else {
            std::cerr << "error: context mismatch: expected " << expectedProfile << ", but got " << actualProfile << "\n";
            exit(1);
        }
    }

    initialized = true;
}


} /* namespace glws */

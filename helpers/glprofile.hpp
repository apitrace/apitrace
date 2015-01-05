/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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
 * Representation and manipulation of OpenGL context profiles.
 */


#include <ostream>


#ifndef GLPROFILE_HPP
#define GLPROFILE_HPP


namespace glprofile {


enum Api {
    API_GL = 0,
    API_GLES
};


struct Profile {
    unsigned major:8;
    unsigned minor:8;
    unsigned api:1;
    unsigned core:1;

    inline
    Profile(Api _api = API_GL, unsigned _major = 1, unsigned _minor = 0, bool _core = false) {
        api = _api;
        major = _major;
        minor = _minor;
        core = _core;
    }

    inline bool
    versionGreaterOrEqual(unsigned refMajor, unsigned refMinor) const {
        return major > refMajor ||
               (major == refMajor && minor >= refMinor);
    }

    inline bool
    matches(const Profile expected) const {
        return api == expected.api &&
               versionGreaterOrEqual(expected.major, expected.minor) &&
               core == expected.core;
    }

    // Comparison operator, mainly for use in std::map
    inline bool
    operator == (const Profile & other) const {
        return major == other.major &&
               minor == other.minor &&
               api == other.api &&
               core == other.core;
    }

    // Comparison operator, mainly for use in std::map
    inline bool
    operator < (const Profile & other) const {
        return major < other.major ||
               minor < other.minor ||
               api < other.api ||
               core < other.core;
    }
};


std::ostream &
operator << (std::ostream &os, const Profile & profile);


Profile
getCurrentContextProfile(void);


} /* namespace glprofile */


#endif // GLPROFILE_HPP

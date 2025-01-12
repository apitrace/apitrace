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


#include "glfeatures.hpp"

#include <assert.h>

#include <sstream>

#include "os.hpp"
#include "glproc.hpp"


namespace glfeatures {


bool
Profile::matches(const Profile expected) const
{
    if (api != expected.api) {
        return false;
    }

    if (!versionGreaterOrEqual(expected.major, expected.minor)) {
        return false;
    }

    /*
     * GLX_ARB_create_context/WGL_ARB_create_context specs state that
     *
     *   "If version 3.1 is requested, the context returned may implement
     *   any of the following versions:
     *
     *     * Version 3.1. The GL_ARB_compatibility extension may or may not
     *       be implemented, as determined by the implementation.
     *
     *     * The core profile of version 3.2 or greater."
     */
    if (core != expected.core &&
        (expected.major != 3 || expected.minor != 1)) {
        return false;
    }

    /*
     * Only check forward-compatible flag prior to 3.2 contexts.
     *
     * Note that on MacOSX all 3.2+ context must be forward-compatible.
     */
#ifndef __APPLE__
    if (forwardCompatible > expected.forwardCompatible) {
        return false;
    }
#endif

    return true;
}


std::string
Profile::str(void) const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}


std::ostream &
operator << (std::ostream &os, const Profile & profile) {
    os << "OpenGL";
    if (profile.api == API_GLES) {
        os << " ES";
    }
    os << " " << profile.major << "." << profile.minor;
    if (profile.api == API_GL) {
        if (profile.versionGreaterOrEqual(3, 2)) {
            os << " " << (profile.core ? "core" : "compat");
        }
        if (profile.forwardCompatible) {
            os << " forward-compatible";
        }
    }
    return os;
}


static inline bool
isDigit(char c) {
    return c >= '0' && c <= '9';
}


static unsigned
parseNumber(const char * & p)
{
    unsigned n = 0;
    char c = *p;
    while (isDigit(c)) {
        unsigned digit = c - '0';
        n *= 10;
        n += digit;
        ++p;
        c = *p;
    }
    return n;
}


/*
 * Parse API and version numbers from a GL_VERSION string.
 *
 * OpenGL 2.1 specification states that GL_VERSION string is laid out as
 *
 *     <version number><space><vendor-specific information>
 *
 *   Where <version number> is either of the form <major number>.<minor number>
 *   or <major number>.<minor number>.<release number>, where the numbers all
 *   have one or more digits.  The release number and vendor specific
 *   information are optional.
 *
 * OpenGL ES 1.x specification states that GL_VERSION is laid out as
 *
 *   "OpenGL ES-XX 1.x" XX={CM,CL}
 *
 * OpenGL ES 2 and 3 specifications state that GL_VERSION is laid out as
 *
 *   "OpenGL ES N.M vendor-specific information"
 */
static Profile
parseVersion(const char *version)
{
    Profile profile(API_GL, 0, 0, false);

    const char *p = version;

    if (p[0] == 'O' &&
        p[1] == 'p' &&
        p[2] == 'e' &&
        p[3] == 'n' &&
        p[4] == 'G' &&
        p[5] == 'L' &&
        p[6] == ' ' &&
        p[7] == 'E' &&
        p[8] == 'S') {
        p += 9;

        profile.api = API_GLES;

        // skip `-{CM,CL}`
        if (*p == '-') {
            ++p;
            while (*p != ' ') {
                if (*p == '\0') {
                    goto malformed;
                }
                ++p;
            }
        }

        // skip white-space
        while (*p == ' ') {
            if (*p == '\0') {
                goto malformed;
            }
            ++p;
        }
    }

    if (!isDigit(*p)) {
        goto malformed;
    }

    profile.major = parseNumber(p);
    if (*p++ == '.' &&
        isDigit(*p)) {
        profile.minor = parseNumber(p);
    } else {
        goto malformed;
    }

    return profile;

malformed:
    os::log("warning: malformed GL_VERSION (\"%s\")\n", version);
    return profile;
}


/*
 * Get the profile of the current context.
 */
Profile
getCurrentContextProfile(void)
{
    Profile profile(API_GL, 0, 0, false);

    assert(parseVersion("3.0 Mesa 10.3.2") == Profile(API_GL, 3, 0));
    assert(parseVersion("3.3 (Core Profile) Mesa 10.3.2") == Profile(API_GL, 3, 3));
    assert(parseVersion("4.4.0 NVIDIA 331.89") == Profile(API_GL, 4, 4));
    assert(parseVersion("OpenGL ES 3.0 Mesa 10.3.2") == Profile(API_GLES, 3, 0));

    const char *version = (const char *)_glGetString(GL_VERSION);
    if (!version) {
        os::log("apitrace: warning: got null GL_VERSION\n");
        return profile;
    }

    // Parse the version string.
    profile = parseVersion(version);

    if (profile.major >= 3) {
        /*
         * From OpenGL and OpenGL ES version 3 onwards, the GL version may also
         * be queried via GL_MAJOR VERSION and GL_MINOR_VERSION, which should
         * match the major number and minor number in GL_VERSION string, so use
         * it to check we parsed the versions correcly.
         */

        GLint majorVersion = 0;
        _glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
        GLint minorVersion = 0;
        _glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

        if (majorVersion != profile.major ||
            minorVersion != profile.minor) {
            os::log("apitrace: warning: OpenGL context version mismatch (GL_VERSION=\"%s\", but GL_MAJOR/MINOR_VERSION=%u.%u)\n",
                    version, majorVersion, minorVersion);
        }
    }

    if (profile.api == API_GL) {
        if (profile.versionGreaterOrEqual(3, 0)) {
            GLint contextFlags = 0;
            _glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
            if (contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) {
                profile.forwardCompatible = true;
            }
        }

        if (profile.versionGreaterOrEqual(3, 2)) {
            GLint profileMask = 0;
            _glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
            if (profileMask & GL_CONTEXT_CORE_PROFILE_BIT) {
                profile.core = true;
            }
        }
    }

    return profile;

}


void
Extensions::getCurrentContextExtensions(const Profile & profile)
{
    assert(strings.empty());
    if (profile.major >= 3) {
        // Use glGetStringi
        GLint num_strings = 0;
        _glGetIntegerv(GL_NUM_EXTENSIONS, &num_strings);
        assert(num_strings);
        for (int i = 0; i < num_strings; ++i) {
            const char *extension = reinterpret_cast<const char *>(_glGetStringi(GL_EXTENSIONS, i));
            assert(extension);
            if (extension) {
                strings.insert(extension);
            }
        }
    } else {
        // Use glGetString
        const char *begin = reinterpret_cast<const char *>(_glGetString(GL_EXTENSIONS));
        assert(begin);
        if (begin) {
            do {
                const char *end = begin;
                char c = *end;
                while (c != '\0' && c != ' ') {
                    ++end;
                    c = *end;
                }
                if (end != begin) {
                    strings.insert(std::string(begin, end));
                }
                if (c == '\0') {
                    break;
                }
                begin = end + 1;
            } while(true);
        }
    }
}


bool
Extensions::has(const char *string) const
{
    return strings.find(string) != strings.end();
}


Features::Features(void)
{
    memset(this, 0, sizeof *this);
}


void
Features::load(const Profile & profile, const Extensions & ext)
{
    ES = profile.es();
    core = profile.core;

    ARB_draw_buffers = !ES;

    // Check extensions we use.
    ARB_sampler_objects = ext.has("GL_ARB_sampler_objects");
    ARB_get_program_binary = ext.has("GL_ARB_get_program_binary");
    KHR_debug = !ES && ext.has("GL_KHR_debug");
    EXT_debug_label = ext.has("GL_EXT_debug_label");
    ARB_direct_state_access = ext.has("GL_ARB_direct_state_access");
    ARB_shader_image_load_store = ext.has("GL_ARB_shader_image_load_store");
    ARB_shader_storage_buffer_object = ext.has("GL_ARB_shader_storage_buffer_object");
    ARB_program_interface_query = ext.has("GL_ARB_program_interface_query");
    OVR_multiview = ext.has("GL_OVR_multiview");

    NV_read_depth_stencil = ES && ext.has("GL_NV_read_depth_stencil");

    if (profile.desktop()) {
        ARB_color_buffer_float = profile.versionGreaterOrEqual(3, 0) ||
                                 ext.has("GL_ARB_color_buffer_float");

        // GL_EXT_texture3D uses different entrypoints
        texture_3d = profile.versionGreaterOrEqual(1, 2);

        pixel_buffer_object = profile.versionGreaterOrEqual(2, 1) ||
                              ext.has("GL_ARB_pixel_buffer_object") ||
                              ext.has("GL_EXT_pixel_buffer_object");

        read_buffer = 1;

        // GL_EXT_framebuffer_object requires different entry points
        framebuffer_object = profile.versionGreaterOrEqual(3, 0) ||
                             ext.has("GL_ARB_framebuffer_object");

        read_framebuffer_object = framebuffer_object;

        query_buffer_object = profile.versionGreaterOrEqual(4, 4) ||
                              ext.has("GL_ARB_query_buffer_object") ||
                              ext.has("GL_AMD_query_buffer_object");

        primitive_restart = profile.versionGreaterOrEqual(3, 1) ||
                            ext.has("GL_NV_primitive_restart");

        unpack_subimage = 1;
        instanced_arrays = profile.versionGreaterOrEqual(3, 3) || ext.has("GL_ARB_instanced_arrays");
    } else {
        texture_3d = 1;

        pixel_buffer_object = profile.versionGreaterOrEqual(3, 0) ||
                              ext.has("GL_NV_pixel_buffer_object");

        // GL_EXT_multiview_draw_buffers requires different entry points
        // GL_NV_read_buffer requires different entry points
        read_buffer = 0;

        // GL_OES_framebuffer_object requires different entry points
        framebuffer_object = profile.versionGreaterOrEqual(2, 0);

        // GL_ANGLE_framebuffer_blit requires different entry points
        // GL_APPLE_framebuffer_multisample requires different entry points
        // GL_NV_framebuffer_blit requires different entry points
        read_framebuffer_object = 0;

        query_buffer_object = 0;

        primitive_restart = 0;

        unpack_subimage = ext.has("GL_EXT_unpack_subimage");
        instanced_arrays = profile.versionGreaterOrEqual(3, 0) || ext.has("GL_EXT_instanced_arrays");
    }
}


void
Features::load(void)
{
    glfeatures::Profile profile = glfeatures::getCurrentContextProfile();
    glfeatures::Extensions exts;
    exts.getCurrentContextExtensions(profile);
    load(profile, exts);
}


} /* namespace glfeatures */

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


#pragma once


#include <ostream>
#include <set>
#include <string>


namespace glfeatures {


enum Api {
    API_GL = 0,
    API_GLES
};


struct Profile {
    unsigned major:8;
    unsigned minor:8;
    unsigned samples:8;
    unsigned api:1;
    unsigned core:1;
    unsigned forwardCompatible:1;

    inline
    Profile(
        Api _api = API_GL,
        unsigned _major = 1,
        unsigned _minor = 0,
        bool _core = false,
        bool _forwardCompatible = false
    ) {
        api = _api;
        major = _major;
        minor = _minor;
        samples = 1;
        core = _core;
        forwardCompatible = _forwardCompatible;
    }

    inline bool
    desktop(void) const {
        return api == API_GL;
    }

    inline bool
    es(void) const {
        return api == API_GLES;
    }

    inline bool
    versionGreaterOrEqual(unsigned refMajor, unsigned refMinor) const {
        return major > refMajor ||
               (major == refMajor && minor >= refMinor);
    }

    inline bool
    versionGreaterOrEqual(Api refApi, unsigned refMajor, unsigned refMinor) const {
        return api == refApi && versionGreaterOrEqual(refMajor, refMinor);
    }

    bool
    matches(const Profile expected) const;

    // Comparison operator, mainly for use in std::map
    inline bool
    operator == (const Profile & other) const {
        return major == other.major &&
               minor == other.minor &&
               api   == other.api   &&
               core  == other.core  &&
               forwardCompatible == other.forwardCompatible;
    }

    // Comparison operator, mainly for use in std::map
    inline bool
    operator < (const Profile & other) const {
        if (major != other.major) {
            return major < other.major;
        }
        if (minor != other.minor) {
            return minor < other.minor;
        }
        if (api != other.api) {
            return api < other.api;
        }
        if (core != other.core) {
            return core < other.core;
        }
        if (forwardCompatible != other.forwardCompatible) {
            return forwardCompatible < other.forwardCompatible;
        }
        
        return false;
    }

    std::string
    str(void) const;
};


std::ostream &
operator << (std::ostream &os, const Profile & profile);


Profile
getCurrentContextProfile(void);


class Extensions
{
private:
    std::set<std::string> strings;

public:
    void
    getCurrentContextExtensions(const Profile & profile);

    bool
    has(const char *string) const;
};


struct Features
{
    unsigned ES:1;
    unsigned core:1;

    unsigned ARB_draw_buffers:1;
    unsigned ARB_sampler_objects:1;
    unsigned ARB_get_program_binary:1;
    unsigned KHR_debug:1;
    unsigned EXT_debug_label:1;
    unsigned NV_read_depth_stencil:1;  /* ES only */
    unsigned ARB_shader_image_load_store:1;
    unsigned ARB_direct_state_access:1;
    unsigned ARB_shader_storage_buffer_object:1;
    unsigned ARB_program_interface_query:1;
    unsigned OVR_multiview:1;
    unsigned ARB_color_buffer_float:1;
    unsigned instanced_arrays:1;

    unsigned texture_3d:1;
    unsigned pixel_buffer_object:1;
    unsigned read_buffer:1;
    unsigned framebuffer_object:1;
    unsigned read_framebuffer_object:1;
    unsigned query_buffer_object:1;
    unsigned primitive_restart:1;
    unsigned unpack_subimage:1;

    Features(void);

    void
    load(const Profile & profile, const Extensions & exts);

    // Load from current context
    void
    load(void);
};


} /* namespace glfeatures */

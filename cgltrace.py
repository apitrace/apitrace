##########################################################################
#
# Copyright 2011 VMware, Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


"""Cgl tracing generator."""


from stdapi import API
from glapi import glapi
from cglapi import cglapi
from gltrace import GlTracer


class CglTracer(GlTracer):

    def is_public_function(self, function):
        # The symbols visible in libGL.dylib can vary, so expose them all
        return True


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print
    print '#ifndef _GNU_SOURCE'
    print '#define _GNU_SOURCE // for dladdr'
    print '#endif'
    print '#include <dlfcn.h>'
    print
    print '#include "trace_write.hpp"'
    print
    print '// To validate our prototypes'
    print '#define GL_GLEXT_PROTOTYPES'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print

    api = API()
    api.add_api(cglapi)
    api.add_api(glapi)
    tracer = CglTracer()
    tracer.trace_api(api)

    print r'''


/*
 * Handle to the true libGL.so
 */
static void *libgl_handle = NULL;


/*
 * Lookup a libGL symbol
 */
static void * __dlsym(const char *symbol)
{
    void *result;
    if (!libgl_handle) {
        const char * libgl_filename;

        /* 
         * Unfortunately we can't just dlopen
         * /System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib
         * because DYLD_LIBRARY_PATH takes precedence, even for absolute paths.
         */
        libgl_filename = "libGL.system.dylib";

        libgl_handle = dlopen(libgl_filename, RTLD_LOCAL | RTLD_NOW | RTLD_FIRST);
        if (!libgl_handle) {
            OS::DebugMessage("error: couldn't load %s\n", libgl_filename);
            return NULL;
        }
    }

    result = dlsym(libgl_handle, symbol);

    if (result == dlsym(RTLD_SELF, symbol)) {
        OS::DebugMessage("error: symbol lookup recursion\n");
        OS::Abort();
        return NULL;
    }

    return result;
}


PUBLIC
void * gll_noop = 0;

'''

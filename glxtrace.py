##########################################################################
#
# Copyright 2008-2010 VMware, Inc.
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


"""GLX tracing generator."""


from stdapi import API
from glapi import glapi
from glxapi import glxapi
from gltrace import GlTracer
from dispatch import function_pointer_type, function_pointer_value


class GlxTracer(GlTracer):

    def get_function_address(self, function):
        return '__%s' % (function.name,)

    def wrap_ret(self, function, instance):
        if function.name in ("glXGetProcAddress", "glXGetProcAddressARB"):
            print '    %s = __unwrap_proc_addr(procName, %s);' % (instance, instance)


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print '#include <dlfcn.h>'
    print
    print '#include "trace_write.hpp"'
    print
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print
    print 'extern "C" {'
    print
    print 'static __GLXextFuncPtr __unwrap_proc_addr(const GLubyte * procName, __GLXextFuncPtr procPtr);'
    print

    api = API()
    api.add_api(glxapi)
    api.add_api(glapi)
    tracer = GlxTracer()
    tracer.trace_api(api)

    print 'static __GLXextFuncPtr __unwrap_proc_addr(const GLubyte * procName, __GLXextFuncPtr procPtr) {'
    print '    if (!procPtr) {'
    print '        return procPtr;'
    print '    }'
    for f in api.functions:
        ptype = function_pointer_type(f)
        pvalue = function_pointer_value(f)
        print '    if(!strcmp("%s", (const char *)procName)) {' % f.name
        print '        %s = (%s)procPtr;' % (pvalue, ptype)
        print '        return (__GLXextFuncPtr)&%s;' % (f.name,)
        print '    }'
    print '    return procPtr;'
    print '}'
    print
    print '} /* extern "C" */'

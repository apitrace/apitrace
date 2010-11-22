##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
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

from gl import *
from dl import *

libgl = Dll("GL")
libgl.functions = basic_functions(DllFunction)


class GlxGetProcAddressFunction(DllFunction):

    def __init__(self, type, name, args):
        DllFunction.__init__(self, type, name, args)
        self.functions = []

    def wrap_decl(self):
        for function in self.functions:
            function.wrap_decl()
        DllFunction.wrap_decl(self)

    def wrap_impl(self):
        for function in self.functions:
            function.wrap_impl()
        DllFunction.wrap_impl(self)

    def post_call_impl(self):
        print '    if(result) {'
        for function in self.functions:
            ptype = function.pointer_type()
            pvalue = function.pointer_value()
            print '        if(!strcmp("%s", (const char *)procName)) {' % function.name
            print '            %s = (%s)result;' % (pvalue, ptype)
            print '            result = (void(*)())&%s;' % function.name;
            print '        }'
        print '    }'


PROC = Opaque("__GLXextFuncPtr")

glXgetprocaddress = GlxGetProcAddressFunction(PROC, "glXGetProcAddress", [(Alias("const GLubyte *", String), "procName")])
libgl.functions.append(glXgetprocaddress)

class GlxFunction(Function):

    def __init__(self, type, name, args, call = '', fail = None):
        Function.__init__(self, type, name, args, call=call, fail=fail)
        
    def get_true_pointer(self):
        ptype = self.pointer_type()
        pvalue = self.pointer_value()
        print '    if(!%s)' % (pvalue,)
        self.fail_impl()

glXgetprocaddress.functions += extended_functions(GlxFunction)


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print '#include <dlfcn.h>'
    print '#include <X11/Xlib.h>'
    print '#include <GL/gl.h>'
    print '#include <GL/glext.h>'
    print '#include <GL/glx.h>'
    print '#include <GL/glxext.h>'
    print
    print '#include "log.hpp"'
    print '#include "glhelpers.hpp"'
    print
    print 'extern "C" {'
    print
    wrap()
    print
    print '}'

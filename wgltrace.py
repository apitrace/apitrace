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


"""WGL tracing code generator."""


from stdapi import API
from glapi import glapi
from wglapi import wglapi
from dispatch import function_pointer_type, function_pointer_value
from gltrace import GlTracer
from codegen import *


class WglTracer(GlTracer):

    def get_function_address(self, function):
        return '__%s' % (function.name,)

    def wrap_ret(self, function, instance):
        GlTracer.wrap_ret(self, function, instance)

        if function.name == "wglGetProcAddress":
            print '    if (%s) {' % instance
        
            func_dict = {}
            for f in glapi.functions + wglapi.functions:
                func_dict[f.name] = f
                for alias in f.aliases:
                    func_dict[alias] = f

            def handle_case(function_name):
                f = func_dict[function_name]
                ptype = function_pointer_type(f)
                pvalue = function_pointer_value(f)
                print '    %s = (%s)%s;' % (pvalue, ptype, instance)
                print '    %s = (%s)&%s;' % (instance, function.type, f.name);
        
            def handle_default():
                print '    OS::DebugMessage("apitrace: unknown function \\"%s\\"\\n", lpszProc);'

            string_switch('lpszProc', func_dict.keys(), handle_case, handle_default)
            print '    }'


if __name__ == '__main__':
    print
    print '#define _GDI32_'
    print
    print '#include <string.h>'
    print '#include <windows.h>'
    print
    print '#include "trace_write.hpp"'
    print '#include "os.hpp"'
    print
    print '''
static HINSTANCE g_hDll = NULL;

static PROC
__getPublicProcAddress(LPCSTR lpProcName)
{
    if (!g_hDll) {
        char szDll[MAX_PATH] = {0};
        
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        
        strcat(szDll, "\\\\opengl32.dll");
        
        g_hDll = LoadLibraryA(szDll);
        if (!g_hDll) {
            return NULL;
        }
    }
        
    return GetProcAddress(g_hDll, lpProcName);
}

    '''
    print '#include "glproc.hpp"'
    print '#include "glsize.hpp"'
    print
    api = API()
    api.add_api(glapi)
    api.add_api(wglapi)
    tracer = WglTracer()
    tracer.trace_api(api)

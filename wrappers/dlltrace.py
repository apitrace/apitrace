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

"""Trace code generation for Windows DLLs."""


from trace import Tracer
from dispatch import Dispatcher
from specs.stdapi import API


class DllTracer(Tracer):

    def __init__(self, dllname):
        self.dllname = dllname
    
    def header(self, api):
        print '''
static HMODULE g_hDll = NULL;

static PROC
_getPublicProcAddress(LPCSTR lpProcName)
{
    if (!g_hDll) {
        char szDll[MAX_PATH] = {0};
        
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        
        strcat(szDll, "\\\\%s");
        
        g_hDll = LoadLibraryA(szDll);
        if (!g_hDll) {
            return NULL;
        }
    }
        
    return GetProcAddress(g_hDll, lpProcName);
}

''' % self.dllname

        for module in api.modules:
            dispatcher = Dispatcher()
            dispatcher.dispatchModule(module)

        Tracer.header(self, api)

    def traceModule(self, module):
        api = API()
        api.addModule(module)
        self.traceApi(api)

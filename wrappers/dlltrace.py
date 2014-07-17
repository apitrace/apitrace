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


import ntpath

from trace import Tracer
from dispatch import Dispatcher
from specs.stdapi import API


class DllDispatcher(Dispatcher):

    def dispatchModule(self, module):
        tag = module.name.upper()
        print r'HMODULE g_h%sModule = NULL;' % (tag,)
        print r''
        print r'static PROC'
        print r'_get%sProcAddress(LPCSTR lpProcName) {' % tag
        print r'    if (!g_h%sModule) {' % tag
        print r'        char szDll[MAX_PATH] = {0};'
        print r'        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {'
        print r'            return NULL;'
        print r'        }'
        print r'        strcat(szDll, "\\\\%s.dll");' % module.name
        print r'        g_h%sModule = LoadLibraryA(szDll);' % tag
        print r'        if (!g_h%sModule) {' % tag
        print r'            return NULL;'
        print r'        }'
        print r'    }'
        print r'    return GetProcAddress(g_h%sModule, lpProcName);' % tag
        print r'}'
        print r''

        Dispatcher.dispatchModule(self, module)

    def getProcAddressName(self, module, function):
        return '_get%sProcAddress' % (module.name.upper())


class DllTracer(Tracer):

    def header(self, api):

        for module in api.modules:
            dispatcher = DllDispatcher()
            dispatcher.dispatchModule(module)

        Tracer.header(self, api)

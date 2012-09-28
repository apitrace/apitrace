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


from retrace import Retracer
from dispatch import Dispatcher


class DllDispatcher(Dispatcher):

    def dispatchApi(self, api):
        tag = api.name.upper()
        print r'const char *g_sz%sDllName = NULL;' % (tag,)
        print r'HMODULE g_h%sModule = NULL;' % (tag,)
        print r''
        print r'static PROC'
        print r'_getPublicProcAddress(LPCSTR lpProcName) {'
        print r'    if (!g_h%sModule) {' % tag
        print r'        if (g_sz%sDllName) {' % tag
        print r'            g_h%sModule = LoadLibraryA(g_sz%sDllName);' % (tag, tag)
        print r'            if (!g_h%sModule) {' % tag
        print r'                os::log("warning: failed to load %%s\n", g_sz%sDllName);' % tag 
        print r'            }'
        print r'        }'
        print r'        if (!g_h%sModule) {' % tag
        print r'            g_h%sModule = LoadLibraryA("%s.dll");' % (tag, api.name)
        print r'        }'
        print r'        if (!g_h%sModule) {' % tag
        print r'            os::log("error: failed to load %s.dll\n");' % api.name
        print r'            exit(1);'
        print r'            return NULL;'
        print r'        }'
        print r'    }'
        print r'    return GetProcAddress(g_h%sModule, lpProcName);' % tag
        print r'}'
        print r''

        Dispatcher.dispatchApi(self, api)


class DllRetracer(Retracer):

    def retraceApi(self, api):
        dispatcher = DllDispatcher()
        dispatcher.dispatchApi(api)

        Retracer.retraceApi(self, api)


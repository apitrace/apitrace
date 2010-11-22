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

"""dl"""


from base import *


class DllFunction(Function):

    def __init__(self, type, name, args, call = '', **kwargs):
        Function.__init__(self, type, name, args, call=call, **kwargs)
        
    def get_true_pointer(self):
        ptype = self.pointer_type()
        pvalue = self.pointer_value()
        print '    if(!%s) {' % (pvalue,)
        print '        %s = (%s)dlsym(RTLD_NEXT, "%s");' % (pvalue, ptype, self.name)
        print '        if(!%s)' % (pvalue,)
        self.fail_impl()
        print '    }'


class Dll:

    def __init__(self, name):
        self.name = name
        self.functions = []
        if self not in towrap:
            towrap.append(self)

    def wrap_name(self):
        return "Wrap" + self.name

    def wrap_pre_decl(self):
        pass

    def wrap_decl(self):
        for function in self.functions:
            function.wrap_decl()
        print

    def wrap_impl(self):
        for function in self.functions:
            function.wrap_impl()
        print
        print '''
static void _init(void) __attribute__((constructor));
static void _init(void) {'''
        print r'        Log::Open("%s");' % self.name
        print '''}

static void _uninit(void) __attribute__((destructor));
static void _uninit(void) {
    Log::Close();
}
'''


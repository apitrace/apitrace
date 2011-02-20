##########################################################################
#
# Copyright 2010 VMware, Inc.
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


"""Generate DLL/SO dispatching functions.
""" 


import stdapi


def function_pointer_type(function):
    return '__PFN' + function.name.upper()


def function_pointer_value(function):
    return '__' + function.name + '_ptr'


class Dispatcher:

    def header(self):
        # Must be implemented by derived classes, which should define, declare,
        # or implement something like:
        #
        #  typedef void (*__PROC)(void);
        #
        #  static __PROC __getPublicProcAddress(const char *name);
        #  static __PROC __getPrivateProcAddress(const char *name);
        #  static void __abort(void);
        #
        raise NotImplementedError

    def dispatch_api(self, api):
        for function in api.functions:
            self.dispatch_function(function)
        
        # define standard name aliases for convenience, but only when not
        # tracing, as that would cause symbol clashing with the tracing
        # functions
        print '#ifdef RETRACE'
        for function in api.functions:
            if not self.is_public_function(function):
                print '#define %s __%s' % (function.name, function.name)
        print '#endif /* RETRACE */'
        print

    def dispatch_function(self, function):
        if self.is_public_function(function):
            print '#ifndef RETRACE'
            print
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)
        print 'typedef ' + function.prototype('* %s' % ptype) + ';'
        print 'static %s %s = NULL;' % (ptype, pvalue)
        print
        print 'static inline ' + function.prototype('__' + function.name) + ' {'
        if function.type is stdapi.Void:
            ret = ''
        else:
            ret = 'return '
        self.get_true_pointer(function)
        print '    %s%s(%s);' % (ret, pvalue, ', '.join([str(arg.name) for arg in function.args]))
        print '}'
        print
        if self.is_public_function(function):
            print '#endif /* !RETRACE */'
            print

    def is_public_function(self, function):
        return True

    def get_true_pointer(self, function):
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)
        if self.is_public_function(function):
            get_proc_address = '__getPublicProcAddress'
        else:
            get_proc_address = '__getPrivateProcAddress'
        print '    if (!%s) {' % (pvalue,)
        print '        %s = (%s)%s("%s");' % (pvalue, ptype, get_proc_address, function.name)
        print '        if (!%s) {' % (pvalue,)
        self.fail_function(function)
        print '        }'
        print '    }'

    def fail_function(self, function):
        print '            OS::DebugMessage("error: unavailable function \\"%s\\"\\n");' % function.name
        if function.fail is not None:
            if function.type is stdapi.Void:
                assert function.fail == ''
                print '            return;' 
            else:
                assert function.fail != ''
                print '            return %s;' % function.fail
        else:
            print '            __abort();'



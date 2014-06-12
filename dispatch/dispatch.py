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


# Adjust path
import os.path
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))


import specs.stdapi as stdapi


def function_pointer_type(function):
    return 'PFN_' + function.name.upper()


def function_pointer_value(function):
    return '_' + function.name + '_ptr'


class Dispatcher:

    def header(self):
        # Must be implemented by derived classes, which should define, declare,
        # or implement something like:
        #
        #  typedef void (*_PROC)(void);
        #
        #  static _PROC _getPublicProcAddress(const char *name);
        #  static _PROC _getPrivateProcAddress(const char *name);
        #
        raise NotImplementedError

    def dispatchModule(self, module):
        for function in module.functions:
            self.dispatchFunction(module, function)
        
        # define standard name aliases for convenience, but only when not
        # tracing, as that would cause symbol clashing with the tracing
        # functions
        print '#ifdef RETRACE'
        for function in module.functions:
            print '#define %s _%s' % (function.name, function.name)
        print '#endif /* RETRACE */'
        print

    def dispatchFunction(self, module, function):
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)
        print 'typedef ' + function.prototype('* %s' % ptype) + ';'
        print 'static %s %s = NULL;' % (ptype, pvalue)
        print
        print 'static inline ' + function.prototype('_' + function.name) + ' {'
        print '    const char *_name = "%s";' % function.name
        if function.type is stdapi.Void:
            ret = ''
        else:
            ret = 'return '
        self.invokeGetProcAddress(module, function)
        print '    %s%s(%s);' % (ret, pvalue, ', '.join([str(arg.name) for arg in function.args]))
        print '}'
        print

    def isFunctionPublic(self, module, function):
        return True

    def getProcAddressName(self, module, function):
        if self.isFunctionPublic(module, function):
            return '_getPublicProcAddress'
        else:
            return '_getPrivateProcAddress'

    def invokeGetProcAddress(self, module, function):
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)
        getProcAddressName = self.getProcAddressName(module, function)
        print '    if (!%s) {' % (pvalue,)
        print '        %s = (%s)%s(_name);' % (pvalue, ptype, getProcAddressName)
        print '        if (!%s) {' % (pvalue,)
        self.failFunction(function)
        print '        }'
        print '    }'

    def failFunction(self, function):
        if function.type is stdapi.Void or function.fail is not None:
            print r'            os::log("warning: ignoring call to unavailable function %s\n", _name);'
            if function.type is stdapi.Void:
                assert function.fail is None
                print '            return;' 
            else:
                assert function.fail is not None
                print '            return %s;' % function.fail
        else:
            print r'            os::log("error: unavailable function %s\n", _name);'
            print r'            os::abort();'



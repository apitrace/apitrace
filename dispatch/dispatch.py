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
    return '_' + function.name


class Dispatcher:

    def dispatchModule(self, module):
        self.dispatchModuleDecl(module)
        self.dispatchModuleImpl(module)
    
    def dispatchModuleDecl(self, module):
        for function in module.functions:
            self.dispatchFunctionDecl(module, function)
        
        # define standard name aliases for convenience, but only when not
        # tracing, as that would cause symbol clashing with the tracing
        # functions
        print '#ifdef RETRACE'
        for function in module.functions:
            print '#define %s _%s' % (function.name, function.name)
        print '#endif /* RETRACE */'
        print
    
    def dispatchFunctionDecl(self, module, function):
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)
        print 'typedef ' + function.prototype('* %s' % ptype) + ';'
        print 'extern %s %s;' % (ptype, pvalue)
        print

    def dispatchModuleImpl(self, module):
        for function in module.functions:
            self.dispatchFunctionImpl(module, function)

    def dispatchFunctionImpl(self, module, function):
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)

        if function.type is stdapi.Void:
            ret = ''
        else:
            ret = 'return '

        print 'static ' + function.prototype('_fail_' + function.name) + ' {'
        self.failFunction(function)
        print '}'
        print

        print 'static ' + function.prototype('_get_' + function.name) + ' {'
        self.invokeGetProcAddress(module, function)
        print '    %s%s(%s);' % (ret, pvalue, ', '.join([str(arg.name) for arg in function.args]))
        print '}'
        print

        print '%s %s = &%s;' % (ptype, pvalue, '_get_' + function.name)
        print

    def getProcAddressName(self, module, function):
        raise NotImplementedError

    def invokeGetProcAddress(self, module, function):
        ptype = function_pointer_type(function)
        pvalue = function_pointer_value(function)
        getProcAddressName = self.getProcAddressName(module, function)
        print '    %s _ptr;' % (ptype,)
        print '    _ptr = (%s)%s("%s");' % (ptype, getProcAddressName, function.name)
        print '    if (!_ptr) {'
        print '        _ptr = &%s;' % ('_fail_' + function.name)
        print '    }'
        print '    %s = _ptr;' % (pvalue,)

    def failFunction(self, function):
        print r'    const char *_name = "%s";' % function.name
        if function.type is stdapi.Void or function.fail is not None:
            print r'    os::log("warning: ignoring call to unavailable function %s\n", _name);'
            if function.type is stdapi.Void:
                assert function.fail is None
                print '    return;' 
            else:
                assert function.fail is not None
                print '    return %s;' % function.fail
        else:
            print r'    os::log("error: unavailable function %s\n", _name);'
            print r'    os::abort();'



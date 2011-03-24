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


"""Generic retracing code generator."""

import stdapi
import glapi
from codegen import *


class ConstRemover(stdapi.Rebuilder):

    def visit_const(self, const):
        return const.type

    def visit_opaque(self, opaque):
        expr = opaque.expr
        if expr.startswith('const '):
            expr = expr[6:]
        return stdapi.Opaque(expr)


def handle_entry(handle, value):
    if handle.key is None:
        return "__%s_map[%s]" % (handle.name, value)
    else:
        key_name, key_type = handle.key
        return "__%s_map[%s][%s]" % (handle.name, key_name, value)


class ValueExtractor(stdapi.Visitor):

    def visit_literal(self, literal, lvalue, rvalue):
        if literal.format == 'Bool':
            print '    %s = static_cast<bool>(%s);' % (lvalue, rvalue)
        else:
            print '    %s = %s;' % (lvalue, rvalue)

    def visit_const(self, const, lvalue, rvalue):
        self.visit(const.type, lvalue, rvalue)

    def visit_alias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)
    
    def visit_enum(self, enum, lvalue, rvalue):
        print '    %s = %s;' % (lvalue, rvalue)

    def visit_bitmask(self, bitmask, lvalue, rvalue):
        self.visit(bitmask.type, lvalue, rvalue)

    def visit_array(self, array, lvalue, rvalue):
        print '    const Trace::Array *__a%s = dynamic_cast<const Trace::Array *>(&%s);' % (array.id, rvalue)
        print '    if (__a%s) {' % (array.id)
        length = '__a%s->values.size()' % array.id
        print '        %s = new %s[%s];' % (lvalue, array.type, length)
        index = '__j' + array.id
        print '        for(size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = length)
        try:
            self.visit(array.type, '%s[%s]' % (lvalue, index), '*__a%s->values[%s]' % (array.id, index))
        finally:
            print '        }'
            print '    } else {'
            print '        %s = NULL;' % lvalue
            print '    }'
    
    def visit_pointer(self, pointer, lvalue, rvalue):
        print '    const Trace::Array *__a%s = dynamic_cast<const Trace::Array *>(&%s);' % (pointer.id, rvalue)
        print '    if (__a%s) {' % (pointer.id)
        print '        %s = new %s;' % (lvalue, pointer.type)
        try:
            self.visit(pointer.type, '%s[0]' % (lvalue,), '*__a%s->values[0]' % (pointer.id,))
        finally:
            print '    } else {'
            print '        %s = NULL;' % lvalue
            print '    }'

    def visit_handle(self, handle, lvalue, rvalue):
        self.visit(handle.type, lvalue, handle_entry(handle, rvalue));
        print '    if (verbosity >= 2)'
        print '        std::cout << "%s " << static_cast<%s>(%s) << " <- " << %s << "\\n";' % (handle.name, handle.type, rvalue, lvalue)
    
    def visit_blob(self, blob, lvalue, rvalue):
        print '    %s = static_cast<%s>((%s).blob());' % (lvalue, blob, rvalue)
    
    def visit_string(self, string, lvalue, rvalue):
        print '    %s = (%s)((%s).string());' % (lvalue, string.expr, rvalue)



class ValueWrapper(stdapi.Visitor):

    def visit_literal(self, literal, lvalue, rvalue):
        pass

    def visit_alias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)
    
    def visit_enum(self, enum, lvalue, rvalue):
        pass

    def visit_bitmask(self, bitmask, lvalue, rvalue):
        pass

    def visit_array(self, array, lvalue, rvalue):
        print '    const Trace::Array *__a%s = dynamic_cast<const Trace::Array *>(&%s);' % (array.id, rvalue)
        print '    if (__a%s) {' % (array.id)
        length = '__a%s->values.size()' % array.id
        index = '__j' + array.id
        print '        for(size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = length)
        try:
            self.visit(array.type, '%s[%s]' % (lvalue, index), '*__a%s->values[%s]' % (array.id, index))
        finally:
            print '        }'
            print '    }'
    
    def visit_pointer(self, pointer, lvalue, rvalue):
        print '    const Trace::Array *__a%s = dynamic_cast<const Trace::Array *>(&%s);' % (pointer.id, rvalue)
        print '    if (__a%s) {' % (pointer.id)
        try:
            self.visit(pointer.type, '%s[0]' % (lvalue,), '*__a%s->values[0]' % (pointer.id,))
        finally:
            print '    }'
    

    def visit_handle(self, handle, lvalue, rvalue):
        if handle.range is None:
            rvalue = "static_cast<%s>(%s)" % (handle.type, rvalue)
            entry = handle_entry(handle, rvalue) 
            print "    %s = %s;" % (entry, lvalue)
            print '    if (verbosity >= 2)'
            print '        std::cout << "{handle.name} " << {rvalue} << " -> " << {lvalue} << "\\n";'.format(**locals())
        else:
            i = '__h' + handle.id
            lvalue = "%s + %s" % (lvalue, i)
            rvalue = "static_cast<%s>(%s) + %s" % (handle.type, rvalue, i)
            entry = handle_entry(handle, rvalue) 
            print '    for({handle.type} {i} = 0; {i} < {handle.range}; ++{i}) {{'.format(**locals())
            print '        {entry} = {lvalue};'.format(**locals())
            print '        if (verbosity >= 2)'
            print '            std::cout << "{handle.name} " << ({rvalue}) << " -> " << ({lvalue}) << "\\n";'.format(**locals())
            print '    }'
    
    def visit_blob(self, blob, lvalue, rvalue):
        pass
    
    def visit_string(self, string, lvalue, rvalue):
        pass


class Retracer:

    def retrace_function(self, function):
        print 'static void retrace_%s(Trace::Call &call) {' % function.name
        self.retrace_function_body(function)
        print '}'
        print

    def retrace_function_body(self, function):
        success = True
        for arg in function.args:
            arg_type = ConstRemover().visit(arg.type)
            #print '    // %s ->  %s' % (arg.type, arg_type)
            print '    %s %s;' % (arg_type, arg.name)
            rvalue = 'call.arg(%u)' % (arg.index,)
            lvalue = arg.name
            try:
                self.extract_arg(function, arg, arg_type, lvalue, rvalue)
            except NotImplementedError:
                success = False
                print '    %s = 0; // FIXME' % arg.name
        if not success:
            self.fail_function(function)
        self.call_function(function)
        for arg in function.args:
            if arg.output:
                arg_type = ConstRemover().visit(arg.type)
                rvalue = 'call.arg(%u)' % (arg.index,)
                lvalue = arg.name
                try:
                    ValueWrapper().visit(arg_type, lvalue, rvalue)
                except NotImplementedError:
                    print '   // FIXME: %s' % arg.name
        if function.type is not stdapi.Void:
            rvalue = '*call.ret'
            lvalue = '__result'
            try:
                ValueWrapper().visit(function.type, lvalue, rvalue)
            except NotImplementedError:
                print '   // FIXME: result'

    def fail_function(self, function):
        print '    if (verbosity >= 0)'
        print '        std::cerr << "warning: unsupported call %s\\n";' % function.name
        print '    return;'

    def extract_arg(self, function, arg, arg_type, lvalue, rvalue):
        ValueExtractor().visit(arg_type, lvalue, rvalue)

    def call_function(self, function):
        arg_names = ", ".join([arg.name for arg in function.args])
        if function.type is not stdapi.Void:
            print '    %s __result;' % (function.type)
            print '    __result = %s(%s);' % (function.name, arg_names)
        else:
            print '    %s(%s);' % (function.name, arg_names)

    def filter_function(self, function):
        return True

    def retrace_functions(self, functions):
        functions = filter(self.filter_function, functions)

        for function in functions:
            if function.sideeffects:
                self.retrace_function(function)

        print 'static bool retrace_call(Trace::Call &call) {'
        print '    const char *name = call.name().c_str();'
        print
        print '    if (verbosity >= 1) {'
        print '        std::cout << call;'
        print '        std::cout.flush();'
        print '    };'
        print

        func_dict = dict([(function.name, function) for function in functions])

        def handle_case(function_name):
            function = func_dict[function_name]
            if function.sideeffects:
                print '        retrace_%s(call);' % function.name
            print '        return true;'
    
        string_switch('name', func_dict.keys(), handle_case)

        print '    if (verbosity >= 0)'
        print '        std::cerr << "warning: unknown call " << call.name() << "\\n";'
        print '    return false;'
        print '}'
        print


    def retrace_api(self, api):

        print '#include "trace_parser.hpp"'
        print

        types = api.all_types()
        handles = [type for type in types if isinstance(type, stdapi.Handle)]
        handle_names = set()
        for handle in handles:
            if handle.name not in handle_names:
                if handle.key is None:
                    print 'static std::map<%s, %s> __%s_map;' % (handle.type, handle.type, handle.name)
                else:
                    key_name, key_type = handle.key
                    print 'static std::map<%s, std::map<%s, %s> > __%s_map;' % (key_type, handle.type, handle.type, handle.name)
                handle_names.add(handle.name)
        print

        print 'int verbosity = 0;'
        print

        self.retrace_functions(api.functions)


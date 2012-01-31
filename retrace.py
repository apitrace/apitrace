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


import sys

import specs.stdapi as stdapi
import specs.glapi as glapi


class ConstRemover(stdapi.Rebuilder):
    '''Type visitor which strips out const qualifiers from types.'''

    def visitConst(self, const):
        return const.type

    def visitOpaque(self, opaque):
        return opaque


def lookupHandle(handle, value):
    if handle.key is None:
        return "__%s_map[%s]" % (handle.name, value)
    else:
        key_name, key_type = handle.key
        return "__%s_map[%s][%s]" % (handle.name, key_name, value)


class ValueDeserializer(stdapi.Visitor):

    def visitLiteral(self, literal, lvalue, rvalue):
        print '    %s = (%s).to%s();' % (lvalue, rvalue, literal.kind)

    def visitConst(self, const, lvalue, rvalue):
        self.visit(const.type, lvalue, rvalue)

    def visitAlias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)
    
    def visitEnum(self, enum, lvalue, rvalue):
        print '    %s = (%s).toSInt();' % (lvalue, rvalue)

    def visitBitmask(self, bitmask, lvalue, rvalue):
        self.visit(bitmask.type, lvalue, rvalue)

    def visitArray(self, array, lvalue, rvalue):
        print '    const trace::Array *__a%s = dynamic_cast<const trace::Array *>(&%s);' % (array.tag, rvalue)
        print '    if (__a%s) {' % (array.tag)
        length = '__a%s->values.size()' % array.tag
        print '        %s = new %s[%s];' % (lvalue, array.type, length)
        index = '__j' + array.tag
        print '        for (size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = length)
        try:
            self.visit(array.type, '%s[%s]' % (lvalue, index), '*__a%s->values[%s]' % (array.tag, index))
        finally:
            print '        }'
            print '    } else {'
            print '        %s = NULL;' % lvalue
            print '    }'
    
    def visitPointer(self, pointer, lvalue, rvalue):
        print '    const trace::Array *__a%s = dynamic_cast<const trace::Array *>(&%s);' % (pointer.tag, rvalue)
        print '    if (__a%s) {' % (pointer.tag)
        print '        %s = new %s;' % (lvalue, pointer.type)
        try:
            self.visit(pointer.type, '%s[0]' % (lvalue,), '*__a%s->values[0]' % (pointer.tag,))
        finally:
            print '    } else {'
            print '        %s = NULL;' % lvalue
            print '    }'

    def visitIntPointer(self, pointer, lvalue, rvalue):
        print '    %s = static_cast<%s>((%s).toPointer());' % (lvalue, pointer, rvalue)

    def visitLinearPointer(self, pointer, lvalue, rvalue):
        print '    %s = static_cast<%s>(retrace::toPointer(%s));' % (lvalue, pointer, rvalue)

    def visitHandle(self, handle, lvalue, rvalue):
        #OpaqueValueDeserializer().visit(handle.type, lvalue, rvalue);
        self.visit(handle.type, lvalue, rvalue);
        new_lvalue = lookupHandle(handle, lvalue)
        print '    if (retrace::verbosity >= 2) {'
        print '        std::cout << "%s " << size_t(%s) << " <- " << size_t(%s) << "\\n";' % (handle.name, lvalue, new_lvalue)
        print '    }'
        print '    %s = %s;' % (lvalue, new_lvalue)
    
    def visitBlob(self, blob, lvalue, rvalue):
        print '    %s = static_cast<%s>((%s).toPointer());' % (lvalue, blob, rvalue)
    
    def visitString(self, string, lvalue, rvalue):
        print '    %s = (%s)((%s).toString());' % (lvalue, string.expr, rvalue)


class OpaqueValueDeserializer(ValueDeserializer):
    '''Value extractor that also understands opaque values.

    Normally opaque values can't be retraced, unless they are being extracted
    in the context of handles.'''

    def visitOpaque(self, opaque, lvalue, rvalue):
        print '    %s = static_cast<%s>(retrace::toPointer(%s));' % (lvalue, opaque, rvalue)


class SwizzledValueRegistrator(stdapi.Visitor):
    '''Type visitor which will register (un)swizzled value pairs, to later be
    swizzled.'''

    def visitLiteral(self, literal, lvalue, rvalue):
        pass

    def visitAlias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)
    
    def visitEnum(self, enum, lvalue, rvalue):
        pass

    def visitBitmask(self, bitmask, lvalue, rvalue):
        pass

    def visitArray(self, array, lvalue, rvalue):
        print '    const trace::Array *__a%s = dynamic_cast<const trace::Array *>(&%s);' % (array.tag, rvalue)
        print '    if (__a%s) {' % (array.tag)
        length = '__a%s->values.size()' % array.tag
        index = '__j' + array.tag
        print '        for (size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = length)
        try:
            self.visit(array.type, '%s[%s]' % (lvalue, index), '*__a%s->values[%s]' % (array.tag, index))
        finally:
            print '        }'
            print '    }'
    
    def visitPointer(self, pointer, lvalue, rvalue):
        print '    const trace::Array *__a%s = dynamic_cast<const trace::Array *>(&%s);' % (pointer.tag, rvalue)
        print '    if (__a%s) {' % (pointer.tag)
        try:
            self.visit(pointer.type, '%s[0]' % (lvalue,), '*__a%s->values[0]' % (pointer.tag,))
        finally:
            print '    }'
    
    def visitIntPointer(self, pointer, lvalue, rvalue):
        pass
    
    def visitLinearPointer(self, pointer, lvalue, rvalue):
        assert pointer.size is not None
        if pointer.size is not None:
            print r'    retrace::addRegion((%s).toUIntPtr(), %s, %s);' % (rvalue, lvalue, pointer.size)

    def visitHandle(self, handle, lvalue, rvalue):
        print '    %s __orig_result;' % handle.type
        OpaqueValueDeserializer().visit(handle.type, '__orig_result', rvalue);
        if handle.range is None:
            rvalue = "__orig_result"
            entry = lookupHandle(handle, rvalue) 
            print "    %s = %s;" % (entry, lvalue)
            print '    if (retrace::verbosity >= 2) {'
            print '        std::cout << "{handle.name} " << {rvalue} << " -> " << {lvalue} << "\\n";'.format(**locals())
            print '    }'
        else:
            i = '__h' + handle.tag
            lvalue = "%s + %s" % (lvalue, i)
            rvalue = "__orig_result + %s" % (i,)
            entry = lookupHandle(handle, rvalue) 
            print '    for ({handle.type} {i} = 0; {i} < {handle.range}; ++{i}) {{'.format(**locals())
            print '        {entry} = {lvalue};'.format(**locals())
            print '        if (retrace::verbosity >= 2) {'
            print '            std::cout << "{handle.name} " << ({rvalue}) << " -> " << ({lvalue}) << "\\n";'.format(**locals())
            print '        }'
            print '    }'
    
    def visitBlob(self, blob, lvalue, rvalue):
        pass
    
    def visitString(self, string, lvalue, rvalue):
        pass


class Retracer:

    def retraceFunction(self, function):
        print 'static void retrace_%s(trace::Call &call) {' % function.name
        self.retraceFunctionBody(function)
        print '}'
        print

    def retraceFunctionBody(self, function):
        if not function.sideeffects:
            print '    (void)call;'
            return

        success = True
        for arg in function.args:
            arg_type = ConstRemover().visit(arg.type)
            #print '    // %s ->  %s' % (arg.type, arg_type)
            print '    %s %s;' % (arg_type, arg.name)
            rvalue = 'call.arg(%u)' % (arg.index,)
            lvalue = arg.name
            try:
                self.extractArg(function, arg, arg_type, lvalue, rvalue)
            except NotImplementedError:
                success = False
                print '    %s = 0; // FIXME' % arg.name
        if not success:
            print '    if (1) {'
            self.failFunction(function)
            print '    }'
        self.invokeFunction(function)
        for arg in function.args:
            if arg.output:
                arg_type = ConstRemover().visit(arg.type)
                rvalue = 'call.arg(%u)' % (arg.index,)
                lvalue = arg.name
                try:
                    self.regiterSwizzledValue(arg_type, lvalue, rvalue)
                except NotImplementedError:
                    print '    // XXX: %s' % arg.name
        if function.type is not stdapi.Void:
            rvalue = '*call.ret'
            lvalue = '__result'
            try:
                self.regiterSwizzledValue(function.type, lvalue, rvalue)
            except NotImplementedError:
                print '    // XXX: result'
        if not success:
            if function.name[-1].islower():
                sys.stderr.write('warning: unsupported %s call\n' % function.name)

    def failFunction(self, function):
        print '    if (retrace::verbosity >= 0) {'
        print '        retrace::unsupported(call);'
        print '    }'
        print '    return;'

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        ValueDeserializer().visit(arg_type, lvalue, rvalue)
    
    def extractOpaqueArg(self, function, arg, arg_type, lvalue, rvalue):
        OpaqueValueDeserializer().visit(arg_type, lvalue, rvalue)

    def regiterSwizzledValue(self, type, lvalue, rvalue):
        visitor = SwizzledValueRegistrator()
        visitor.visit(type, lvalue, rvalue)

    def invokeFunction(self, function):
        arg_names = ", ".join(function.argNames())
        if function.type is not stdapi.Void:
            print '    %s __result;' % (function.type)
            print '    __result = %s(%s);' % (function.name, arg_names)
            print '    (void)__result;'
        else:
            print '    %s(%s);' % (function.name, arg_names)

    def filterFunction(self, function):
        return True

    table_name = 'retrace::callbacks'

    def retraceFunctions(self, functions):
        functions = filter(self.filterFunction, functions)

        for function in functions:
            self.retraceFunction(function)

        print 'const retrace::Entry %s[] = {' % self.table_name
        for function in functions:
            print '    {"%s", &retrace_%s},' % (function.name, function.name)
        print '    {NULL, NULL}'
        print '};'
        print


    def retraceApi(self, api):

        print '#include "trace_parser.hpp"'
        print '#include "retrace.hpp"'
        print

        types = api.getAllTypes()
        handles = [type for type in types if isinstance(type, stdapi.Handle)]
        handle_names = set()
        for handle in handles:
            if handle.name not in handle_names:
                if handle.key is None:
                    print 'static retrace::map<%s> __%s_map;' % (handle.type, handle.name)
                else:
                    key_name, key_type = handle.key
                    print 'static std::map<%s, retrace::map<%s> > __%s_map;' % (key_type, handle.type, handle.name)
                handle_names.add(handle.name)
        print

        self.retraceFunctions(api.functions)


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


# Adjust path
import os.path
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))


import specs.stdapi as stdapi
import specs.glapi as glapi


class UnsupportedType(Exception):
    pass


def lookupHandle(handle, value):
    if handle.key is None:
        return "__%s_map[%s]" % (handle.name, value)
    else:
        key_name, key_type = handle.key
        return "__%s_map[%s][%s]" % (handle.name, key_name, value)


class ValueAllocator(stdapi.Visitor):

    def visitLiteral(self, literal, lvalue, rvalue):
        pass

    def visitConst(self, const, lvalue, rvalue):
        self.visit(const.type, lvalue, rvalue)

    def visitAlias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)

    def visitEnum(self, enum, lvalue, rvalue):
        pass

    def visitBitmask(self, bitmask, lvalue, rvalue):
        pass

    def visitArray(self, array, lvalue, rvalue):
        print '    %s = _allocator.alloc<%s>(&%s);' % (lvalue, array.type, rvalue)

    def visitPointer(self, pointer, lvalue, rvalue):
        print '    %s = _allocator.alloc<%s>(&%s);' % (lvalue, pointer.type, rvalue)

    def visitIntPointer(self, pointer, lvalue, rvalue):
        pass

    def visitObjPointer(self, pointer, lvalue, rvalue):
        pass

    def visitLinearPointer(self, pointer, lvalue, rvalue):
        pass

    def visitReference(self, reference, lvalue, rvalue):
        self.visit(reference.type, lvalue, rvalue);

    def visitHandle(self, handle, lvalue, rvalue):
        pass

    def visitBlob(self, blob, lvalue, rvalue):
        pass

    def visitString(self, string, lvalue, rvalue):
        pass

    def visitStruct(self, struct, lvalue, rvalue):
        pass

    def visitPolymorphic(self, polymorphic, lvalue, rvalue):
        self.visit(polymorphic.defaultType, lvalue, rvalue)

    def visitOpaque(self, opaque, lvalue, rvalue):
        pass


class ValueDeserializer(stdapi.Visitor):

    def visitLiteral(self, literal, lvalue, rvalue):
        print '    %s = (%s).to%s();' % (lvalue, rvalue, literal.kind)

    def visitConst(self, const, lvalue, rvalue):
        self.visit(const.type, lvalue, rvalue)

    def visitAlias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)
    
    def visitEnum(self, enum, lvalue, rvalue):
        print '    %s = static_cast<%s>((%s).toSInt());' % (lvalue, enum, rvalue)

    def visitBitmask(self, bitmask, lvalue, rvalue):
        self.visit(bitmask.type, lvalue, rvalue)

    def visitArray(self, array, lvalue, rvalue):

        tmp = '__a_' + array.tag + '_' + str(self.seq)
        self.seq += 1

        print '    if (%s) {' % (lvalue,)
        print '        const trace::Array *%s = dynamic_cast<const trace::Array *>(&%s);' % (tmp, rvalue)
        length = '%s->values.size()' % (tmp,)
        index = '__j' + array.tag
        print '        for (size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = length)
        try:
            self.visit(array.type, '%s[%s]' % (lvalue, index), '*%s->values[%s]' % (tmp, index))
        finally:
            print '        }'
            print '    }'
    
    def visitPointer(self, pointer, lvalue, rvalue):
        tmp = '__a_' + pointer.tag + '_' + str(self.seq)
        self.seq += 1

        print '    if (%s) {' % (lvalue,)
        print '        const trace::Array *%s = dynamic_cast<const trace::Array *>(&%s);' % (tmp, rvalue)
        try:
            self.visit(pointer.type, '%s[0]' % (lvalue,), '*%s->values[0]' % (tmp,))
        finally:
            print '    }'

    def visitIntPointer(self, pointer, lvalue, rvalue):
        print '    %s = static_cast<%s>((%s).toPointer());' % (lvalue, pointer, rvalue)

    def visitObjPointer(self, pointer, lvalue, rvalue):
        old_lvalue = '(%s).toUIntPtr()' % (rvalue,)
        new_lvalue = '_obj_map[%s]' % (old_lvalue,)
        print '    if (retrace::verbosity >= 2) {'
        print '        std::cout << std::hex << "obj 0x" << size_t(%s) << " <- 0x" << size_t(%s) << std::dec <<"\\n";' % (old_lvalue, new_lvalue)
        print '    }'
        print '    %s = static_cast<%s>(%s);' % (lvalue, pointer, new_lvalue)

    def visitLinearPointer(self, pointer, lvalue, rvalue):
        print '    %s = static_cast<%s>(retrace::toPointer(%s));' % (lvalue, pointer, rvalue)

    def visitReference(self, reference, lvalue, rvalue):
        self.visit(reference.type, lvalue, rvalue);

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

    seq = 0

    def visitStruct(self, struct, lvalue, rvalue):
        tmp = '__s_' + struct.tag + '_' + str(self.seq)
        self.seq += 1

        print '    const trace::Struct *%s = dynamic_cast<const trace::Struct *>(&%s);' % (tmp, rvalue)
        print '    assert(%s);' % (tmp)
        for i in range(len(struct.members)):
            member_type, member_name = struct.members[i]
            self.visit(member_type, '%s.%s' % (lvalue, member_name), '*%s->members[%s]' % (tmp, i))

    def visitPolymorphic(self, polymorphic, lvalue, rvalue):
        self.visit(polymorphic.defaultType, lvalue, rvalue)
    
    def visitOpaque(self, opaque, lvalue, rvalue):
        raise UnsupportedType


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
    
    def visitObjPointer(self, pointer, lvalue, rvalue):
        print r'    _obj_map[(%s).toUIntPtr()] = %s;' % (rvalue, lvalue)
    
    def visitLinearPointer(self, pointer, lvalue, rvalue):
        assert pointer.size is not None
        if pointer.size is not None:
            print r'    retrace::addRegion((%s).toUIntPtr(), %s, %s);' % (rvalue, lvalue, pointer.size)

    def visitReference(self, reference, lvalue, rvalue):
        pass
    
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

    seq = 0

    def visitStruct(self, struct, lvalue, rvalue):
        tmp = '__s_' + struct.tag + '_' + str(self.seq)
        self.seq += 1

        print '    const trace::Struct *%s = dynamic_cast<const trace::Struct *>(&%s);' % (tmp, rvalue)
        print '    assert(%s);' % (tmp,)
        print '    (void)%s;' % (tmp,)
        for i in range(len(struct.members)):
            member_type, member_name = struct.members[i]
            self.visit(member_type, '%s.%s' % (lvalue, member_name), '*%s->members[%s]' % (tmp, i))
    
    def visitPolymorphic(self, polymorphic, lvalue, rvalue):
        self.visit(polymorphic.defaultType, lvalue, rvalue)
    
    def visitOpaque(self, opaque, lvalue, rvalue):
        pass


class Retracer:

    def retraceFunction(self, function):
        print 'static void retrace_%s(trace::Call &call) {' % function.name
        self.retraceFunctionBody(function)
        print '}'
        print

    def retraceInterfaceMethod(self, interface, method):
        print 'static void retrace_%s__%s(trace::Call &call) {' % (interface.name, method.name)
        self.retraceInterfaceMethodBody(interface, method)
        print '}'
        print

    def retraceFunctionBody(self, function):
        assert function.sideeffects

        self.deserializeArgs(function)
        
        self.invokeFunction(function)

        self.swizzleValues(function)

    def retraceInterfaceMethodBody(self, interface, method):
        assert method.sideeffects

        self.deserializeThisPointer(interface)

        self.deserializeArgs(method)
        
        self.invokeInterfaceMethod(interface, method)

        self.swizzleValues(method)

    def deserializeThisPointer(self, interface):
        print r'    %s *_this;' % (interface.name,)
        print r'    _this = static_cast<%s *>(_obj_map[call.arg(0).toUIntPtr()]);' % (interface.name,)
        print r'    if (!_this) {'
        print r'        retrace::warning(call) << "NULL this pointer\n";'
        print r'        return;'
        print r'    }'

    def deserializeArgs(self, function):
        print '    retrace::ScopedAllocator _allocator;'
        print '    (void)_allocator;'
        success = True
        for arg in function.args:
            arg_type = arg.type.mutable()
            print '    %s %s;' % (arg_type, arg.name)
            rvalue = 'call.arg(%u)' % (arg.index,)
            lvalue = arg.name
            try:
                self.extractArg(function, arg, arg_type, lvalue, rvalue)
            except UnsupportedType:
                success =  False
                print '    memset(&%s, 0, sizeof %s); // FIXME' % (arg.name, arg.name)
            print

        if not success:
            print '    if (1) {'
            self.failFunction(function)
            if function.name[-1].islower():
                sys.stderr.write('warning: unsupported %s call\n' % function.name)
            print '    }'

    def swizzleValues(self, function):
        for arg in function.args:
            if arg.output:
                arg_type = arg.type.mutable()
                rvalue = 'call.arg(%u)' % (arg.index,)
                lvalue = arg.name
                try:
                    self.regiterSwizzledValue(arg_type, lvalue, rvalue)
                except UnsupportedType:
                    print '    // XXX: %s' % arg.name
        if function.type is not stdapi.Void:
            rvalue = '*call.ret'
            lvalue = '__result'
            try:
                self.regiterSwizzledValue(function.type, lvalue, rvalue)
            except UnsupportedType:
                raise
                print '    // XXX: result'

    def failFunction(self, function):
        print '    if (retrace::verbosity >= 0) {'
        print '        retrace::unsupported(call);'
        print '    }'
        print '    return;'

    def extractArg(self, function, arg, arg_type, lvalue, rvalue):
        ValueAllocator().visit(arg_type, lvalue, rvalue)
        if arg.input:
            ValueDeserializer().visit(arg_type, lvalue, rvalue)
    
    def extractOpaqueArg(self, function, arg, arg_type, lvalue, rvalue):
        try:
            ValueAllocator().visit(arg_type, lvalue, rvalue)
        except UnsupportedType:
            pass
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

    def invokeInterfaceMethod(self, interface, method):
        arg_names = ", ".join(method.argNames())
        if method.type is not stdapi.Void:
            print '    %s __result;' % (method.type)
            print '    __result = _this->%s(%s);' % (method.name, arg_names)
            print '    (void)__result;'
        else:
            print '    _this->%s(%s);' % (method.name, arg_names)

    def filterFunction(self, function):
        return True

    table_name = 'retrace::callbacks'

    def retraceApi(self, api):

        print '#include "os_time.hpp"'
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

        print 'static std::map<unsigned long long, void *> _obj_map;'
        print

        functions = filter(self.filterFunction, api.functions)
        for function in functions:
            if function.sideeffects:
                self.retraceFunction(function)
        interfaces = api.getAllInterfaces()
        for interface in interfaces:
            for method in interface.iterMethods():
                if method.sideeffects:
                    self.retraceInterfaceMethod(interface, method)

        print 'const retrace::Entry %s[] = {' % self.table_name
        for function in functions:
            if function.sideeffects:
                print '    {"%s", &retrace_%s},' % (function.name, function.name)
            else:
                print '    {"%s", &retrace::ignore},' % (function.name,)
        for interface in interfaces:
            for method in interface.iterMethods():                
                if method.sideeffects:
                    print '    {"%s::%s", &retrace_%s__%s},' % (interface.name, method.name, interface.name, method.name)
                else:
                    print '    {"%s::%s", &retrace::ignore},' % (interface.name, method.name)
        print '    {NULL, NULL}'
        print '};'
        print


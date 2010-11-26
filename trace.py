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

"""C basic types"""


import stdapi


all_types = {}


class DumpDeclarator(stdapi.OnceVisitor):
    '''Declare helper functions to dump complex types.'''

    def visit_void(self, literal):
        pass

    def visit_literal(self, literal):
        pass

    def visit_string(self, string):
        pass

    def visit_const(self, const):
        self.visit(const.type)

    def visit_struct(self, struct):
        for type, name in struct.members:
            self.visit(type)
        print 'static void __traceStruct%s(const %s &value) {' % (struct.id, struct.expr)
        print '    Trace::BeginStruct(%u);' % len(struct.members)
        for type, name in struct.members:
            print '    Trace::BeginMember("%s");' % (name,)
            dump_instance(type, 'value.%s' % (name,))
            print '    Trace::EndMember();'
        print '    Trace::EndStruct();'
        print '}'
        print

    def visit_array(self, array):
        self.visit(array.type)

    def visit_blob(self, array):
        pass

    def visit_enum(self, enum):
        print 'static void __traceEnum%s(const %s value) {' % (enum.id, enum.expr)
        print '    switch(value) {'
        for value in enum.values:
            print '    case %s:' % value
            print '        Trace::LiteralNamedConstant("%s", %s);' % (value, value)
            print '        break;'
        print '    default:'
        print '        Trace::LiteralSInt(value);'
        print '        break;'
        print '    }'
        print '}'
        print

    def visit_bitmask(self, bitmask):
        print 'static void __traceBitmask%s(%s value) {' % (bitmask.id, bitmask.type)
        print '    Trace::BeginBitmask();'
        for value in bitmask.values:
            print '    if((value & %s) == %s) {' % (value, value)
            print '        Trace::LiteralNamedConstant("%s", %s);' % (value, value)
            print '        value &= ~%s;' % value
            print '    }'
        print '    if(value) {'
        dump_instance(bitmask.type, "value");
        print '    }'
        print '    Trace::EndBitmask();'
        print '}'
        print

    def visit_pointer(self, pointer):
        self.visit(pointer.type)

    def visit_handle(self, handle):
        self.visit(handle.type)

    def visit_alias(self, alias):
        self.visit(alias.type)

    def visit_opaque(self, opaque):
        pass

    def visit_interface(self, interface):
        pass


class DumpImplementer(stdapi.Visitor):
    '''Dump an instance.'''

    def visit_literal(self, literal, instance):
        print '    Trace::Literal%s(%s);' % (literal.format, instance)

    def visit_string(self, string, instance):
        if string.length is not None:
            print '    Trace::LiteralString((const char *)%s, %s);' % (instance, string.length)
        else:
            print '    Trace::LiteralString((const char *)%s);' % instance

    def visit_const(self, const, instance):
        self.visit(const.type, instance)

    def visit_struct(self, struct, instance):
        print '    __traceStruct%s(%s);' % (struct.id, instance)

    def visit_array(self, array, instance):
        print '    if(%s) {' % instance
        index = '__i' + array.type.id
        print '        Trace::BeginArray(%s);' % (array.length,)
        print '        for (int %s = 0; %s < %s; ++%s) {' % (index, index, array.length, index)
        print '            Trace::BeginElement();'
        self.visit(array.type, '(%s)[%s]' % (instance, index))
        print '            Trace::EndElement();'
        print '        }'
        print '        Trace::EndArray();'
        print '    }'
        print '    else'
        print '        Trace::LiteralNull();'

    def visit_blob(self, blob, instance):
        print '    Trace::LiteralBlob(%s, %s);' % (instance, blob.size)

    def visit_enum(self, enum, instance):
        print '    __traceEnum%s(%s);' % (enum.id, instance)

    def visit_bitmask(self, bitmask, instance):
        print '    __traceBitmask%s(%s);' % (bitmask.id, instance)

    def visit_pointer(self, pointer, instance):
        print '    if(%s) {' % instance
        print '        Trace::BeginArray(1);'
        print '        Trace::BeginElement();'
        dump_instance(pointer.type, "*" + instance)
        print '        Trace::EndElement();'
        print '        Trace::EndArray();'
        print '    }'
        print '    else'
        print '        Trace::LiteralNull();'

    def visit_handle(self, handle, instance):
        self.visit(handle.type, instance)

    def visit_alias(self, alias, instance):
        self.visit(alias.type, instance)

    def visit_opaque(self, opaque, instance):
        print '    Trace::LiteralOpaque((const void *)%s);' % instance

    def visit_interface(self, interface, instance):
        print '    Trace::LiteralOpaque((const void *)%s);' % instance


dump_instance = DumpImplementer().visit



class Wrapper(stdapi.Visitor):
    '''Wrap an instance.'''

    def visit_void(self, type, instance):
        raise NotImplementedError

    def visit_literal(self, type, instance):
        pass

    def visit_string(self, type, instance):
        pass

    def visit_const(self, type, instance):
        pass

    def visit_struct(self, struct, instance):
        for type, name in struct.members:
            self.visit(type, "(%s).%s" % (instance, name))

    def visit_array(self, array, instance):
        # XXX: actually it is possible to return an array of pointers
        pass

    def visit_blob(self, blob, instance):
        pass

    def visit_enum(self, enum, instance):
        pass

    def visit_bitmask(self, bitmask, instance):
        pass

    def visit_pointer(self, pointer, instance):
        self.visit(pointer.type, "*" + instance)

    def visit_handle(self, handle, instance):
        self.visit(handle.type, instance)

    def visit_alias(self, alias, instance):
        self.visit(alias.type, instance)

    def visit_opaque(self, opaque, instance):
        pass
    
    def visit_interface(self, interface, instance):
        print "    if(%s)" % instance
        print "        %s = new %s(%s);" % (instance, interface.type.wrap_name(), instance)


class Unwrapper(Wrapper):

    def visit_interface(self, interface, instance):
        print "    if(%s)" % instance
        print "        %s = static_cast<%s *>(%s)->m_pInstance;" % (instance, interface.type.wrap_name(), instance)

wrap_instance = Wrapper().visit
unwrap_instance = Unwrapper().visit


class Tracer:

    def trace_api(self, api):
        self.header(api)

        # Includes
        for header in api.headers:
            print header
        print

        # Type dumpers
        types = api.all_types()
        visitor = DumpDeclarator()
        map(visitor.visit, types)
        print

        # Interfaces wrapers
        map(self.interface_wrap_name, api.interfaces)
        map(self.interface_pre_decl, api.interfaces)
        map(self.interface_decl, api.interfaces)
        map(self.interface_wrap_impl, api.interfaces)
        print

        # Function wrappers
        map(self.trace_function_decl, api.functions)
        map(self.trace_function_impl, api.functions)
        print

        self.footer(api)

    def header(self, api):
        pass

    def footer(self, api):
        pass

    def function_pointer_type(self, function):
        return 'P' + function.name

    def function_pointer_value(self, function):
        return 'p' + function.name

    def trace_function_decl(self, function):
        ptype = self.function_pointer_type(function)
        pvalue = self.function_pointer_value(function)
        print 'typedef ' + function.prototype('* %s' % ptype) + ';'
        print 'static %s %s = NULL;' % (ptype, pvalue)
        print

    def trace_function_fail(self, function):
        if function.fail is not None:
            if function.type is stdapi.Void:
                assert function.fail == ''
                print '            return;' 
            else:
                assert function.fail != ''
                print '            return %s;' % function.fail
        else:
            print '            Trace::Abort();'

    def get_function_address(self, function):
        raise NotImplementedError

    def _get_true_pointer(self, function):
        ptype = self.function_pointer_type(function)
        pvalue = self.function_pointer_value(function)
        print '    if(!%s) {' % (pvalue,)
        print '        %s = (%s)%s;' % (pvalue, ptype, self.get_function_address(function))
        print '        if(!%s)' % (pvalue,)
        self.trace_function_fail(function)
        print '    }'

    def trace_function_impl(self, function):
        pvalue = self.function_pointer_value(function)
        print function.prototype() + ' {'
        if function.type is stdapi.Void:
            result = ''
        else:
            print '    %s __result;' % function.type
            result = '__result = '
        self._get_true_pointer(function)
        print '    unsigned __call = Trace::BeginEnter("%s");' % (function.name)
        for arg in function.args:
            if not arg.output:
                self.unwrap_arg(function, arg)
                self.dump_arg(function, arg)
        print '    Trace::EndEnter();'
        print '    %s%s(%s);' % (result, pvalue, ', '.join([str(arg.name) for arg in function.args]))
        print '    Trace::BeginLeave(__call);'
        for arg in function.args:
            if arg.output:
                self.dump_arg(function, arg)
                self.wrap_arg(function, arg)
        if function.type is not stdapi.Void:
            self.dump_ret(function, "__result")
        print '    Trace::EndLeave();'
        if function.type is not stdapi.Void:
            self.wrap_ret(function, "__result")
            print '    return __result;'
        print '}'
        print

    def dump_arg(self, function, arg):
        print '    Trace::BeginArg(%u, "%s");' % (arg.index, arg.name,)
        dump_instance(arg.type, arg.name)
        print '    Trace::EndArg();'

    def wrap_arg(self, function, arg):
        wrap_instance(arg.type, arg.name)

    def unwrap_arg(self, function, arg):
        unwrap_instance(arg.type, arg.name)

    def dump_ret(self, function, instance):
        print '    Trace::BeginReturn();'
        dump_instance(function.type, instance)
        print '    Trace::EndReturn();'

    def wrap_ret(self, function, instance):
        wrap_instance(function.type, instance)

    def unwrap_ret(self, function, instance):
        unwrap_instance(function.type, instance)

    def interface_wrap_name(self, interface):
        return "Wrap" + interface.expr

    def interface_pre_decl(self, interface):
        print "class %s;" % interface.wrap_name()

    def interface_decl(self, interface):
        print "class %s : public %s " % (interface.wrap_name(), interface.name)
        print "{"
        print "public:"
        print "    %s(%s * pInstance);" % (interface.wrap_name(), interface.name)
        print "    virtual ~%s();" % interface.wrap_name()
        print
        for method in interface.itermethods():
            print "    " + method.prototype() + ";"
        print
        #print "private:"
        print "    %s * m_pInstance;" % (interface.name,)
        print "};"
        print

    def interface_wrap_impl(self, interface):
        print '%s::%s(%s * pInstance) {' % (interface.wrap_name(), interface.wrap_name(), interface.name)
        print '    m_pInstance = pInstance;'
        print '}'
        print
        print '%s::~%s() {' % (interface.wrap_name(), interface.wrap_name())
        print '}'
        print
        for method in interface.itermethods():
            self.trace_method(interface, method)
        print

    def trace_method(self, interface, method):
        print method.prototype(interface.wrap_name() + '::' + method.name) + ' {'
        if method.type is Void:
            result = ''
        else:
            print '    %s __result;' % method.type
            result = '__result = '
        print '    Trace::BeginCall("%s");' % (interface.name + '::' + method.name)
        print '    Trace::BeginArg(0, "this");'
        print '    Trace::LiteralOpaque((const void *)m_pInstance);'
        print '    Trace::EndArg();'
        for arg in method.args:
            if not arg.output:
                self.unwrap_arg(method, arg)
                self.dump_arg(method, arg)
        print '    %sm_pInstance->%s(%s);' % (result, method.name, ', '.join([str(arg.name) for arg in method.args]))
        for arg in method.args:
            if arg.output:
                self.dump_arg(method, arg)
                self.wrap_arg(method, arg)
        if method.type is not Void:
            print '    Trace::BeginReturn("%s");' % method.type
            dump_instance(method.type, "__result")
            print '    Trace::EndReturn();'
            wrap_instance(method.type, '__result')
        print '    Trace::EndCall();'
        if method.name == 'QueryInterface':
            print '    if (*ppvObj == m_pInstance)'
            print '        *ppvObj = this;'
        if method.name == 'Release':
            assert method.type is not Void
            print '    if (!__result)'
            print '        delete this;'
        if method.type is not Void:
            print '    return __result;'
        print '}'
        print



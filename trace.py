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

"""Common trace code generation."""


import stdapi


all_types = {}


def interface_wrap_name(interface):
    return "Wrap" + interface.expr

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
        print '    static const char * members[%u] = {' % (len(struct.members),)
        for type, name,  in struct.members:
            print '        "%s",' % (name,)
        print '    };'
        print '    static const Trace::StructSig sig = {'
        print '       %u, "%s", %u, members' % (int(struct.id), struct.name, len(struct.members))
        print '    };'
        print '    Trace::BeginStruct(&sig);'
        for type, name in struct.members:
            dump_instance(type, 'value.%s' % (name,))
        print '    Trace::EndStruct();'
        print '}'
        print

    def visit_array(self, array):
        self.visit(array.type)

    def visit_blob(self, array):
        pass

    def visit_enum(self, enum):
        print 'static void __traceEnum%s(const %s value) {' % (enum.id, enum.expr)
        n = len(enum.values)
        for i in range(n):
            value = enum.values[i]
            print '    static const Trace::EnumSig sig%u = {%u, "%s", %s};' % (i, enum.vid + i, value, value)
        print '    const Trace::EnumSig *sig;'
        print '    switch(value) {'
        for i in range(n):
            value = enum.values[i]
            print '    case %s:' % value
            print '        sig = &sig%u;' % i
            print '        break;'
        print '    default:'
        print '        Trace::LiteralSInt(value);'
        print '        return;'
        print '    }'
        print '        Trace::LiteralEnum(sig);'
        print '}'
        print

    def visit_bitmask(self, bitmask):
        print 'static const Trace::BitmaskVal __bitmask%s_vals[] = {' % (bitmask.id)
        for value in bitmask.values:
            print '   {"%s", %s},' % (value, value)
        print '};'
        print
        print 'static const Trace::BitmaskSig __bitmask%s_sig = {' % (bitmask.id)
        print '   %u, %u, __bitmask%s_vals' % (int(bitmask.id), len(bitmask.values), bitmask.id)
        print '};'
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
        print "class %s : public %s " % (interface_wrap_name(interface), interface.name)
        print "{"
        print "public:"
        print "    %s(%s * pInstance);" % (interface_wrap_name(interface), interface.name)
        print "    virtual ~%s();" % interface_wrap_name(interface)
        print
        for method in interface.itermethods():
            print "    " + method.prototype() + ";"
        print
        #print "private:"
        print "    %s * m_pInstance;" % (interface.name,)
        print "};"
        print


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
        print '    Trace::LiteralBitmask(__bitmask%s_sig, %s);' % (bitmask.id, instance)

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
        print '    Trace::LiteralOpaque((const void *)&%s);' % instance


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
        assert instance.startswith('*')
        instance = instance[1:]
        print "    if(%s)" % instance
        print "        %s = new %s(%s);" % (instance, interface_wrap_name(interface), instance)


class Unwrapper(Wrapper):

    def visit_interface(self, interface, instance):
        assert instance.startswith('*')
        instance = instance[1:]
        print "    if(%s)" % instance
        print "        %s = static_cast<%s *>(%s)->m_pInstance;" % (instance, interface_wrap_name(interface), instance)


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
        interfaces = [type for type in types if isinstance(type, stdapi.Interface)]
        map(self.interface_wrap_impl, interfaces)
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
        print 'extern "C" ' + function.prototype() + ' {'
        if function.args:
            print '    static const char * __args[%u] = {%s};' % (len(function.args), ', '.join(['"%s"' % arg.name for arg in function.args]))
        else:
            print '    static const char ** __args = NULL;'
        print '    static const Trace::FunctionSig __sig = {%u, "%s", %u, __args};' % (int(function.id), function.name, len(function.args))
        if function.type is stdapi.Void:
            result = ''
        else:
            print '    %s __result;' % function.type
            result = '__result = '
        self._get_true_pointer(function)
        print '    unsigned __call = Trace::BeginEnter(__sig);'
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
        print '    Trace::BeginArg(%u);' % (arg.index,)
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

    def interface_wrap_impl(self, interface):
        print '%s::%s(%s * pInstance) {' % (interface_wrap_name(interface), interface_wrap_name(interface), interface.name)
        print '    m_pInstance = pInstance;'
        print '}'
        print
        print '%s::~%s() {' % (interface_wrap_name(interface), interface_wrap_name(interface))
        print '}'
        print
        for method in interface.itermethods():
            self.trace_method(interface, method)
        print

    def trace_method(self, interface, method):
        print method.prototype(interface_wrap_name(interface) + '::' + method.name) + ' {'
        print '    static const char * __args[%u] = {%s};' % (len(method.args) + 1, ', '.join(['"this"'] + ['"%s"' % arg.name for arg in method.args]))
        print '    static const Trace::FunctionSig __sig = {%u, "%s", %u, __args};' % (int(method.id), interface.name + '::' + method.name, len(method.args) + 1)
        print '    unsigned __call = Trace::BeginEnter(__sig);'
        print '    Trace::BeginArg(0);'
        print '    Trace::LiteralOpaque((const void *)m_pInstance);'
        print '    Trace::EndArg();'
        for arg in method.args:
            if not arg.output:
                self.unwrap_arg(method, arg)
                self.dump_arg(method, arg)
        if method.type is stdapi.Void:
            result = ''
        else:
            print '    %s __result;' % method.type
            result = '__result = '
        print '    Trace::EndEnter();'
        print '    %sm_pInstance->%s(%s);' % (result, method.name, ', '.join([str(arg.name) for arg in method.args]))
        print '    Trace::BeginLeave(__call);'
        for arg in method.args:
            if arg.output:
                self.dump_arg(method, arg)
                self.wrap_arg(method, arg)
        if method.type is not stdapi.Void:
            print '    Trace::BeginReturn();'
            dump_instance(method.type, "__result")
            print '    Trace::EndReturn();'
            wrap_instance(method.type, '__result')
        print '    Trace::EndLeave();'
        if method.name == 'QueryInterface':
            print '    if (*ppvObj == m_pInstance)'
            print '        *ppvObj = this;'
        if method.name == 'Release':
            assert method.type is not stdapi.Void
            print '    if (!__result)'
            print '        delete this;'
        if method.type is not stdapi.Void:
            print '    return __result;'
        print '}'
        print


class DllTracer(Tracer):

    def __init__(self, dllname):
        self.dllname = dllname
    
    def get_function_address(self, function):
        return '__GetProcAddress("%s")' % (function.name,)


    def header(self, api):
        Tracer.header(self, api)

        print '''
static HINSTANCE g_hDll = NULL;

static PROC
__GetProcAddress(LPCSTR lpProcName)
{
    if (!g_hDll) {
        char szDll[MAX_PATH] = {0};
        
        if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
            return NULL;
        }
        
        strcat(szDll, "\\\\%s");
        
        g_hDll = LoadLibraryA(szDll);
        if (!g_hDll) {
            return NULL;
        }
    }
        
    return GetProcAddress(g_hDll, lpProcName);
}

    ''' % self.dllname


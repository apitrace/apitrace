##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
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


import debug


all_types = {}


class Visitor:

    def visit(self, type, *args, **kwargs):
        return type.visit(self, *args, **kwargs)

    __call__ = visit

    def visit_void(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_literal(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_string(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_const(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_struct(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_array(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_blob(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_enum(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_bitmask(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_pointer(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_alias(self, type, *args, **kwargs):
        raise NotImplementedError

    def visit_opaque(self, type, *args, **kwargs):
        raise NotImplementedError


class Rebuilder(Visitor):

    def visit_void(self, void):
        return void

    def visit_literal(self, literal):
        return literal

    def visit_string(self, string):
        return string

    def visit_const(self, const):
        return Const(const.type)

    def visit_struct(self, struct):
        members = [self.visit(member) for member in struct.members]
        return Struct(struct.name, members)

    def visit_array(self, array):
        type = self.visit(array.type)
        return Array(type, array.length)

    def visit_blob(self, blob):
        type = self.visit(blob.type)
        return Blob(type, blob.size)

    def visit_enum(self, enum):
        return enum

    def visit_bitmask(self, bitmask):
        type = self.visit(bitmask.type)
        return Bitmask(type, bitmask.values)

    def visit_pointer(self, pointer):
        type = self.visit(pointer.type)
        return Pointer(type)

    def visit_alias(self, alias):
        type = self.visit(alias.type)
        return Alias(alias.expr, type)

    def visit_opaque(self, opaque):
        return opaque


class Type:

    __seq = 0

    def __init__(self, expr, id = ''):
        self.expr = expr
        
        for char in id:
            assert char.isalnum() or char in '_ '

        id = id.replace(' ', '_')
        
        if id in all_types:
            Type.__seq += 1
            id += str(Type.__seq)
        
        assert id not in all_types
        all_types[id] = self

        self.id = id

    def __str__(self):
        return self.expr

    def visit(self, visitor, *args, **kwargs):
        raise NotImplementedError

    def decl(self):
        pass

    def impl(self):
        pass

    def dump(self, instance):
        raise NotImplementedError
    
    def wrap_instance(self, instance):
        pass 

    def unwrap_instance(self, instance):
        pass


class _Void(Type):

    def __init__(self):
        Type.__init__(self, "void")

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_void(self, *args, **kwargs)

Void = _Void()


class Concrete(Type):

    def decl(self):
        print 'static void Dump%s(const %s &value);' % (self.id, self.expr)
    
    def impl(self):
        print 'static void Dump%s(const %s &value) {' % (self.id, self.expr)
        self._dump("value");
        print '}'
        print
    
    def _dump(self, instance):
        raise NotImplementedError
    
    def dump(self, instance):
        print '    Dump%s(%s);' % (self.id, instance)
    

class Literal(Concrete):

    def __init__(self, expr, format, base=10):
        Concrete.__init__(self, expr)
        self.format = format

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_literal(self, *args, **kwargs)

    def _dump(self, instance):
        print '    Log::Literal%s(%s);' % (self.format, instance)


class Const(Type):

    def __init__(self, type):

        if type.expr.startswith("const "):
            expr = type.expr + " const"
        else:
            expr = "const " + type.expr

        Type.__init__(self, expr, 'C' + type.id)

        self.type = type

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_const(self, *args, **kwargs)

    def dump(self, instance):
        self.type.dump(instance)


class Pointer(Type):

    def __init__(self, type):
        Type.__init__(self, type.expr + " *", 'P' + type.id)
        self.type = type

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_pointer(self, *args, **kwargs)

    def dump(self, instance):
        print '    if(%s) {' % instance
        print '        Log::BeginPointer("%s", (const void *)%s);' % (self.type, instance)
        self.type.dump("*" + instance)
        print '        Log::EndPointer();'
        print '    }'
        print '    else'
        print '        Log::LiteralNull();'

    def wrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)

    def unwrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)


def ConstPointer(type):
    return Pointer(Const(type))


class Enum(Concrete):

    def __init__(self, name, values):
        Concrete.__init__(self, name)
        self.values = values
    
    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_enum(self, *args, **kwargs)

    def _dump(self, instance):
        print '    switch(%s) {' % instance
        for value in self.values:
            print '    case %s:' % value
            print '        Log::LiteralNamedConstant("%s", %s);' % (value, value)
            print '        break;'
        print '    default:'
        print '        Log::LiteralSInt(%s);' % instance
        print '        break;'
        print '    }'


def FakeEnum(type, values):
    return Enum(type.expr, values)


class Bitmask(Concrete):

    def __init__(self, type, values):
        Concrete.__init__(self, type.expr)
        self.type = type
        self.values = values

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_bitmask(self, *args, **kwargs)

    def _dump(self, instance):
        print '    %s l_Value = %s;' % (self.type, instance)
        print '    Log::BeginBitmask("%s");' % (self.type,)
        for value in self.values:
            print '    if((l_Value & %s) == %s) {' % (value, value)
            print '        Log::LiteralNamedConstant("%s", %s);' % (value, value)
            print '        l_Value &= ~%s;' % value
            print '    }'
        print '    if(l_Value) {'
        self.type.dump("l_Value");
        print '    }'
        print '    Log::EndBitmask();'

Flags = Bitmask


class Array(Type):

    def __init__(self, type, length):
        Type.__init__(self, type.expr + " *")
        self.type = type
        self.length = length

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_array(self, *args, **kwargs)

    def dump(self, instance):
        print '    if(%s) {' % instance
        index = '__i' + self.type.id
        print '        Log::BeginArray("%s", %s);' % (self.type, self.length)
        print '        for (int %s = 0; %s < %s; ++%s) {' % (index, index, self.length, index)
        print '            Log::BeginElement("%s");' % (self.type,)
        self.type.dump('(%s)[%s]' % (instance, index))
        print '            Log::EndElement();'
        print '        }'
        print '        Log::EndArray();'
        print '    }'
        print '    else'
        print '        Log::LiteralNull();'

    def wrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)

    def unwrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)


class Blob(Type):

    def __init__(self, type, size):
        Type.__init__(self, type.expr + ' *')
        self.type = type
        self.size = size

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_blob(self, *args, **kwargs)

    def dump(self, instance):
        print '    Log::LiteralBlob(%s, %s);' % (instance, self.size)


class Struct(Concrete):

    def __init__(self, name, members):
        Concrete.__init__(self, name)
        self.name = name
        self.members = members

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_struct(self, *args, **kwargs)

    def _dump(self, instance):
        print '    Log::BeginStruct("%s");' % (self.name,)
        for type, name in self.members:
            print '    Log::BeginMember("%s", "%s");' % (type, name)
            type.dump('(%s).%s' % (instance, name))
            print '    Log::EndMember();'
        print '    Log::EndStruct();'


class Alias(Type):

    def __init__(self, expr, type):
        Type.__init__(self, expr)
        self.type = type

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_alias(self, *args, **kwargs)

    def dump(self, instance):
        self.type.dump(instance)


def Out(type, name):
    arg = Arg(type, name, output=True)
    return arg


class Arg:

    def __init__(self, type, name, output=False):
        self.type = type
        self.name = name
        self.output = output

    def __str__(self):
        return '%s %s' % (self.type, self.name)


class Function:

    def __init__(self, type, name, args, call = '__stdcall', fail = None, sideeffects=True):
        self.type = type
        self.name = name

        self.args = []
        for arg in args:
            if isinstance(arg, tuple):
                arg_type, arg_name = arg
                arg = Arg(arg_type, arg_name)
            self.args.append(arg)

        self.call = call
        self.fail = fail
        self.sideeffects = sideeffects

    def prototype(self, name=None):
        if name is not None:
            name = name.strip()
        else:
            name = self.name
        s = name
        if self.call:
            s = self.call + ' ' + s
        if name.startswith('*'):
            s = '(' + s + ')'
        s = self.type.expr + ' ' + s
        s += "("
        if self.args:
            s += ", ".join(["%s %s" % (arg.type, arg.name) for arg in self.args])
        else:
            s += "void"
        s += ")"
        return s

    def pointer_type(self):
        return 'P' + self.name

    def pointer_value(self):
        return 'p' + self.name

    def wrap_decl(self):
        ptype = self.pointer_type()
        pvalue = self.pointer_value()
        print 'typedef ' + self.prototype('* %s' % ptype) + ';'
        print 'static %s %s = NULL;' % (ptype, pvalue)
        print

    def get_true_pointer(self):
        raise NotImplementedError

    def exit_impl(self):
        print '            Log::Abort();'

    def fail_impl(self):
        if self.fail is not None:
            if self.type is Void:
                assert self.fail == ''
                print '            return;' 
            else:
                assert self.fail != ''
                print '            return %s;' % self.fail
        else:
            self.exit_impl()

    def wrap_impl(self):
        pvalue = self.pointer_value()
        print self.prototype() + ' {'
        if self.type is Void:
            result = ''
        else:
            print '    %s result;' % self.type
            result = 'result = '
        self.get_true_pointer()
        print '    Log::BeginCall("%s");' % (self.name)
        for arg in self.args:
            if not arg.output:
                arg.type.unwrap_instance(arg.name)
                print '    Log::BeginArg("%s", "%s");' % (arg.type, arg.name)
                arg.type.dump(arg.name)
                print '    Log::EndArg();'
        print '    %s%s(%s);' % (result, pvalue, ', '.join([str(arg.name) for arg in self.args]))
        for arg in self.args:
            if arg.output:
                print '    Log::BeginArg("%s", "%s");' % (arg.type, arg.name)
                arg.type.dump(arg.name)
                print '    Log::EndArg();'
                arg.type.wrap_instance(arg.name)
        if self.type is not Void:
            print '    Log::BeginReturn("%s");' % self.type
            self.type.dump("result")
            print '    Log::EndReturn();'
            self.type.wrap_instance('result')
        print '    Log::EndCall();'
        self.post_call_impl()
        if self.type is not Void:
            print '    return result;'
        print '}'
        print

    def post_call_impl(self):
        pass


def FunctionPointer(type, name, args, **kwargs):
    # XXX
    return Opaque(name)


class Interface(Type):

    def __init__(self, name, base=None):
        Type.__init__(self, name)
        self.name = name
        self.base = base
        self.methods = []

    def dump(self, instance):
        print '    Log::LiteralOpaque((const void *)%s);' % instance

    def itermethods(self):
        if self.base is not None:
            for method in self.base.itermethods():
                yield method
        for method in self.methods:
            yield method
        raise StopIteration

    def wrap_name(self):
        return "Wrap" + self.expr

    def wrap_pre_decl(self):
        print "class %s;" % self.wrap_name()

    def wrap_decl(self):
        print "class %s : public %s " % (self.wrap_name(), self.name)
        print "{"
        print "public:"
        print "    %s(%s * pInstance);" % (self.wrap_name(), self.name)
        print "    virtual ~%s();" % self.wrap_name()
        print
        for method in self.itermethods():
            print "    " + method.prototype() + ";"
        print
        #print "private:"
        print "    %s * m_pInstance;" % (self.name,)
        print "};"
        print

    def wrap_impl(self):
        print '%s::%s(%s * pInstance) {' % (self.wrap_name(), self.wrap_name(), self.name)
        print '    m_pInstance = pInstance;'
        print '}'
        print
        print '%s::~%s() {' % (self.wrap_name(), self.wrap_name())
        print '}'
        print
        for method in self.itermethods():
            print method.prototype(self.wrap_name() + '::' + method.name) + ' {'
            if method.type is Void:
                result = ''
            else:
                print '    %s result;' % method.type
                result = 'result = '
            print '    Log::BeginCall("%s");' % (self.name + '::' + method.name)
            print '    Log::BeginArg("%s *", "this");' % self.name
            print '    Log::BeginPointer("%s", (const void *)m_pInstance);' % self.name
            print '    Log::EndPointer();'
            print '    Log::EndArg();'
            for arg in method.args:
                if not arg.output:
                    arg.type.unwrap_instance(arg.name)
                    print '    Log::BeginArg("%s", "%s");' % (arg.type, arg.name)
                    arg.type.dump(arg.name)
                    print '    Log::EndArg();'
            print '    %sm_pInstance->%s(%s);' % (result, method.name, ', '.join([str(arg.name) for arg in method.args]))
            for arg in method.args:
                if arg.output:
                    print '    Log::BeginArg("%s", "%s");' % (arg.type, arg.name)
                    arg.type.dump(arg.name)
                    print '    Log::EndArg();'
                    arg.type.wrap_instance(arg.name)
            if method.type is not Void:
                print '    Log::BeginReturn("%s");' % method.type
                method.type.dump("result")
                print '    Log::EndReturn();'
                method.type.wrap_instance('result')
            print '    Log::EndCall();'
            if method.name == 'QueryInterface':
                print '    if(*ppvObj == m_pInstance)'
                print '        *ppvObj = this;'
            if method.name == 'Release':
                assert method.type is not Void
                print '    if(!result)'
                print '        delete this;'
            if method.type is not Void:
                print '    return result;'
            print '}'
            print
        print


class Method(Function):

    def __init__(self, type, name, args):
        Function.__init__(self, type, name, args, call = '__stdcall')


towrap = []

class WrapPointer(Pointer):

    def __init__(self, type):
        Pointer.__init__(self, type)
        if type not in towrap:
            towrap.append(type)

    def wrap_instance(self, instance):
        print "    if(%s)" % instance
        print "        %s = new %s(%s);" % (instance, self.type.wrap_name(), instance)

    def unwrap_instance(self, instance):
        print "    if(%s)" % instance
        print "        %s = static_cast<%s *>(%s)->m_pInstance;" % (instance, self.type.wrap_name(), instance)


class _String(Type):

    def __init__(self):
        Type.__init__(self, "char *")

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_string(self, *args, **kwargs)

    def dump(self, instance):
        print '    Log::LiteralString((const char *)%s);' % instance

String = _String()


class Opaque(Type):
    '''Opaque pointer.'''

    def __init__(self, expr):
        Type.__init__(self, expr)

    def visit(self, visitor, *args, **kwargs):
        return visitor.visit_opaque(self, *args, **kwargs)

    def dump(self, instance):
        print '    Log::LiteralOpaque((const void *)%s);' % instance


def OpaquePointer(type):
    return Opaque(type.expr + ' *')


Bool = Literal("bool", "Bool")
SChar = Literal("signed char", "SInt")
UChar = Literal("unsigned char", "UInt")
Short = Literal("short", "SInt")
Int = Literal("int", "SInt")
Long = Literal("long", "SInt")
LongLong = Literal("long long", "SInt")
UShort = Literal("unsigned short", "UInt")
UInt = Literal("unsigned int", "UInt")
ULong = Literal("unsigned long", "UInt")
ULongLong = Literal("unsigned long long", "UInt")
Float = Literal("float", "Float")
Double = Literal("double", "Float")
SizeT = Literal("size_t", "UInt")
WString = Literal("wchar_t *", "WString")


def wrap():
    for type in all_types.itervalues():
        type.decl()
    print
    for type in all_types.itervalues():
        type.impl()
    print
    for type in towrap:
        type.wrap_pre_decl()
    print
    for type in towrap:
        type.wrap_decl()
    print
    for type in towrap:
        type.wrap_impl()
    print

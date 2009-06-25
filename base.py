#############################################################################
#
# Copyright 2008 Tungsten Graphics, Inc.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################

"""C basic types"""


import debug


all_types = {}

class Type:

    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

    def identifier(self):
        return self.name.replace(' ', '_')

    def isoutput(self):
        return False

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

Void = _Void()


class Concrete(Type):

    def __init__(self, name):
        for char in name:
            assert char.isalnum() or char in '_ '

        Type.__init__(self, name)
        
        assert self.name not in all_types
        if self.name not in all_types:
            all_types[self.name] = self

    def decl(self):
        print 'void Dump%s(const %s &value);' % (self.identifier(), str(self))
    
    def impl(self):
        print 'void Dump%s(const %s &value) {' % (self.identifier(), str(self))
        self._dump("value");
        print '}'
        print
    
    def _dump(self, instance):
        raise NotImplementedError
    
    def dump(self, instance):
        print '    Dump%s(%s);' % (self.identifier(), instance)
    

class Intrinsic(Concrete):

    def __init__(self, expr, format, name = None):
        if name is None:
            name = expr
        Concrete.__init__(self, name)
        self.expr = expr
        self.format = format

    def _dump(self, instance):
        print '    Log::TextF("%s", %s);' % (self.format, instance)
        
    def __str__(self):
        return self.expr


class Const(Type):

    def __init__(self, type):
        Type.__init__(self, 'C' + type.name)
        self.type = type

    def dump(self, instance):
        self.type.dump(instance)

    def __str__(self):
        return "const " + str(self.type)


class Pointer(Type):

    def __init__(self, type):
        Type.__init__(self, 'P' + type.name)
        self.type = type

    def __str__(self):
        return str(self.type) + " *"
    
    def dump(self, instance):
        print '    if(%s) {' % instance
        print '        Log::BeginReference("%s", %s);' % (self.type, instance)
        try:
            self.type.dump("*" + instance)
        except NotImplementedError:
            pass
        print '        Log::EndReference();'
        print '    }'
        print '    else'
        print '        Log::Text("NULL");'

    def wrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)

    def unwrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)


def ConstPointer(type):
    return Pointer(Const(type))


class OutPointer(Pointer):

    def isoutput(self):
        return True


class Enum(Concrete):

    def __init__(self, name, values):
        Concrete.__init__(self, name)
        self.values = values
    
    def _dump(self, instance):
        print '    switch(%s) {' % instance
        for value in self.values:
            print '    case %s:' % value
            print '        Log::Text("%s");' % value
            print '        break;'
        print '    default:'
        print '        Log::TextF("%%i", %s);' % instance
        print '        break;'
        print '    }'


class FakeEnum(Enum):

    __seq = 0
    
    def __init__(self, type, values):
        FakeEnum.__seq += 1
        Enum.__init__(self, type.name + str(FakeEnum.__seq), values)
        self.type = type
    
    def __str__(self):
        return str(self.type)


class Flags(Concrete):

    __seq = 0
    
    def __init__(self, type, values):
        Flags.__seq += 1
        Concrete.__init__(self, type.name + str(Flags.__seq))
        self.type = type
        self.values = values

    def __str__(self):
        return str(self.type)
    
    def _dump(self, instance):
        print '    bool l_First = TRUE;'
        print '    %s l_Value = %s;' % (self.type, instance)
        for value in self.values:
            print '    if((l_Value & %s) == %s) {' % (value, value)
            print '        if(!l_First)'
            print '            Log::Text(" | ");'
            print '        Log::Text("%s");' % value
            print '        l_Value &= ~%s;' % value
            print '        l_First = FALSE;'
            print '    }'
        print '    if(l_Value || l_First) {'
        print '        if(!l_First)'
        print '            Log::Text(" | ");'
        self.type.dump("l_Value");
        print '    }'


class Struct(Concrete):

    def __init__(self, name, members):
        Concrete.__init__(self, name)
        self.members = members

    def _dump(self, instance):
        for type, name in self.members:
            print '    Log::BeginElement("%s", "%s");' % (type, name)
            type.dump('(%s).%s' % (instance, name))
            print '    Log::EndElement();'


class Alias(Type):

    def __init__(self, name, type):
        Type.__init__(self, name)
        self.type = type

    def dump(self, instance):
        self.type.dump(instance)


class Function:

    def __init__(self, type, name, args, call = '__stdcall', fail = None):
        self.type = type
        self.name = name
        self.args = args
        self.call = call
        self.fail = fail

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
        s = str(self.type) + ' ' + s
        s += "("
        if self.args:
            s += ", ".join(["%s %s" % (type, name) for type, name in self.args])
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

    def fail_impl(self):
        if self.fail is not None:
            assert self.type is not Void
            print '            return %s;' % self.fail
        else:
            print '            ExitProcess(0);'

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
        for type, name in self.args:
            if not type.isoutput():
                type.unwrap_instance(name)
                print '    Log::BeginArg("%s", "%s");' % (type, name)
                type.dump(name)
                print '    Log::EndArg();'
        print '    %s%s(%s);' % (result, pvalue, ', '.join([str(name) for type, name in self.args]))
        for type, name in self.args:
            if type.isoutput():
                print '    Log::BeginArg("%s", "%s");' % (type, name)
                type.dump(name)
                print '    Log::EndArg();'
                type.wrap_instance(name)
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


class Interface(Type):

    def __init__(self, name, base=None):
        Type.__init__(self, name)
        self.base = base
        self.methods = []

    def itermethods(self):
        if self.base is not None:
            for method in self.base.itermethods():
                yield method
        for method in self.methods:
            yield method
        raise StopIteration

    def wrap_name(self):
        return "Wrap" + self.name

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
            print '    Log::BeginReference("%s", m_pInstance);' % self.name
            print '    Log::EndReference();'
            print '    Log::EndArg();'
            for type, name in method.args:
                if not type.isoutput():
                    type.unwrap_instance(name)
                    print '    Log::BeginArg("%s", "%s");' % (type, name)
                    type.dump(name)
                    print '    Log::EndArg();'
            print '    %sm_pInstance->%s(%s);' % (result, method.name, ', '.join([str(name) for type, name in method.args]))
            for type, name in method.args:
                if type.isoutput():
                    print '    Log::BeginArg("%s", "%s");' % (type, name)
                    type.dump(name)
                    print '    Log::EndArg();'
                    type.wrap_instance(name)
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
        Function.__init__(self, type, name, args)


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
        Type.__init__(self, "String")

    def __str__(self):
        return "const char *"

    def dump(self, instance):
        print '    Log::DumpString((const char *)%s);' % instance

String = _String()

class _WString(Type):

    def __init__(self):
        Type.__init__(self, "WString")

    def __str__(self):
        return "const wchar_t *"

    def dump(self, instance):
        print '    Log::DumpWString(%s);' % instance

WString = _WString()


SChar = Intrinsic("signed char", "%i")
UChar = Intrinsic("unsigned char", "%u")
Short = Intrinsic("short", "%i")
Int = Intrinsic("int", "%i")
Long = Intrinsic("long", "%li")
UShort = Intrinsic("unsigned short", "%u")
UInt = Intrinsic("unsigned int", "%u")
ULong = Intrinsic("unsigned long", "%lu")
Float = Intrinsic("float", "%f")
Double = Intrinsic("double", "%f")


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

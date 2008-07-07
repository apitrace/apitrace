"""C basic types"""

class Type:

    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

    def isoutput(self):
        return False

    def dump(self, instance):
        raise NotImplementedError
    
    def wrap_instance(self, instance):
        pass

    def unwrap_instance(self, instance):
        pass


class Void(Type):

    def __init__(self):
        Type.__init__(self, "void")

Void = Void()


class Intrinsic(Type):

    def __init__(self, name, format):
        Type.__init__(self, name)
        self.format = format

    def dump(self, instance):
        print '    g_pLog->TextF("%s", %s);' % (self.format, instance)


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
        try:
            self.type.dump("*" + instance)
        except NotImplementedError:
            print '        g_pLog->TextF("%%p", %s);' % instance
        print '    }'
        print '    else'
        print '        g_pLog->Text("NULL");'

    def wrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)

    def unwrap_instance(self, instance):
        self.type.wrap_instance("*" + instance)


class OutPointer(Pointer):

    def isoutput(self):
        return True


class Enum(Type):

    def __init__(self, name, values):
        Type.__init__(self, name)
        self.values = values
    
    def dump(self, instance):
        print '    switch(%s) {' % instance
        for value in self.values:
            print '    case %s:' % value
            print '        g_pLog->Text("%s");' % value
            print '        break;'
        print '    default:'
        print '        g_pLog->TextF("%%i", %s);' % instance
        print '        break;'
        print '    }'


class Flags(Type):

    def __init__(self, type, values):
        Type.__init__(self, type.name)
        self.values = values


class Struct(Type):

    def __init__(self, name, members):
        Type.__init__(self, name)
        self.members = members

    def dump(self, instance):
        print '    g_pLog->Text("{");'
        first = True
        for type, name in self.members:
            if first:
                first = False
            else:
                print '    g_pLog->Text(", ");'
            type.dump('(%s).%s' % (instance, name))
        print '    g_pLog->Text("}");'


class Alias(Type):

    def __init__(self, name, type):
        Type.__init__(self, name)
        self.type = type

    def dump(self, instance):
        self.type.dump(instance)


class Function:

    def __init__(self, type, name, args, call = '__stdcall'):
        self.type = type
        self.name = name
        self.args = args
        self.call = call

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
            print '    g_pLog->BeginCall("%s");' % (self.name + '::' + method.name)
            print '    g_pLog->BeginParam("this", "%s *");' % self.name
            print '    g_pLog->TextF("%p", m_pInstance);'
            print '    g_pLog->EndParam();'
            for type, name in method.args:
                if not type.isoutput():
                    type.unwrap_instance(name)
                    print '    g_pLog->BeginParam("%s", "%s");' % (name, type)
                    type.dump(name)
                    print '    g_pLog->EndParam();'
            print '    %sm_pInstance->%s(%s);' % (result, method.name, ', '.join([str(name) for type, name in method.args]))
            for type, name in method.args:
                if type.isoutput():
                    print '    g_pLog->BeginParam("%s", "%s");' % (name, type)
                    type.dump(name)
                    print '    g_pLog->EndParam();'
                    type.wrap_instance(name)
            if method.type is not Void:
                print '    g_pLog->BeginReturn("%s");' % method.type
                method.type.dump("result")
                print '    g_pLog->EndReturn();'
                method.type.wrap_instance('result')
            print '    g_pLog->EndCall();'
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

String = Intrinsic("char *", "%s")
Int = Intrinsic("int", "%i")
Long = Intrinsic("long", "%li")
Float = Intrinsic("float", "%f")


def wrap():
    for type in towrap:
        type.wrap_pre_decl()
    print
    for type in towrap:
        type.wrap_decl()
    print
    for type in towrap:
        type.wrap_impl()
    print

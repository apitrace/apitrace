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


# Adjust path
import os.path
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))


import specs.stdapi as stdapi


def getWrapperInterfaceName(interface):
    return "Wrap" + interface.expr



class ComplexValueSerializer(stdapi.OnceVisitor):
    '''Type visitors which generates serialization functions for
    complex types.
    
    Simple types are serialized inline.
    '''

    def __init__(self, serializer):
        stdapi.OnceVisitor.__init__(self)
        self.serializer = serializer

    def visitVoid(self, literal):
        pass

    def visitLiteral(self, literal):
        pass

    def visitString(self, string):
        pass

    def visitConst(self, const):
        self.visit(const.type)

    def visitStruct(self, struct):
        print 'static const char * _struct%s_members[%u] = {' % (struct.tag, len(struct.members))
        for type, name,  in struct.members:
            if name is None:
                print '    "",'
            else:
                print '    "%s",' % (name,)
        print '};'
        print 'static const trace::StructSig _struct%s_sig = {' % (struct.tag,)
        if struct.name is None:
            structName = '""'
        else:
            structName = '"%s"' % struct.name
        print '    %u, %s, %u, _struct%s_members' % (struct.id, structName, len(struct.members), struct.tag)
        print '};'
        print

    def visitArray(self, array):
        self.visit(array.type)

    def visitBlob(self, array):
        pass

    def visitEnum(self, enum):
        print 'static const trace::EnumValue _enum%s_values[] = {' % (enum.tag)
        for value in enum.values:
            print '    {"%s", %s},' % (value, value)
        print '};'
        print
        print 'static const trace::EnumSig _enum%s_sig = {' % (enum.tag)
        print '    %u, %u, _enum%s_values' % (enum.id, len(enum.values), enum.tag)
        print '};'
        print

    def visitBitmask(self, bitmask):
        print 'static const trace::BitmaskFlag _bitmask%s_flags[] = {' % (bitmask.tag)
        for value in bitmask.values:
            print '    {"%s", %s},' % (value, value)
        print '};'
        print
        print 'static const trace::BitmaskSig _bitmask%s_sig = {' % (bitmask.tag)
        print '    %u, %u, _bitmask%s_flags' % (bitmask.id, len(bitmask.values), bitmask.tag)
        print '};'
        print

    def visitPointer(self, pointer):
        self.visit(pointer.type)

    def visitIntPointer(self, pointer):
        pass

    def visitObjPointer(self, pointer):
        self.visit(pointer.type)

    def visitLinearPointer(self, pointer):
        self.visit(pointer.type)

    def visitHandle(self, handle):
        self.visit(handle.type)

    def visitReference(self, reference):
        self.visit(reference.type)

    def visitAlias(self, alias):
        self.visit(alias.type)

    def visitOpaque(self, opaque):
        pass

    def visitInterface(self, interface):
        pass

    def visitPolymorphic(self, polymorphic):
        if not polymorphic.contextLess:
            return
        print 'static void _write__%s(int selector, %s const & value) {' % (polymorphic.tag, polymorphic.expr)
        print '    switch (selector) {'
        for cases, type in polymorphic.iterSwitch():
            for case in cases:
                print '    %s:' % case
            self.serializer.visit(type, 'static_cast<%s>(value)' % (type,))
            print '        break;'
        print '    }'
        print '}'
        print


class ValueSerializer(stdapi.Visitor, stdapi.ExpanderMixin):
    '''Visitor which generates code to serialize any type.
    
    Simple types are serialized inline here, whereas the serialization of
    complex types is dispatched to the serialization functions generated by
    ComplexValueSerializer visitor above.
    '''

    def visitLiteral(self, literal, instance):
        print '    trace::localWriter.write%s(%s);' % (literal.kind, instance)

    def visitString(self, string, instance):
        if not string.wide:
            cast = 'const char *'
            suffix = 'String'
        else:
            cast = 'const wchar_t *'
            suffix = 'WString'
        if cast != string.expr:
            # reinterpret_cast is necessary for GLubyte * <=> char *
            instance = 'reinterpret_cast<%s>(%s)' % (cast, instance)
        if string.length is not None:
            length = ', %s' % self.expand(string.length)
        else:
            length = ''
        print '    trace::localWriter.write%s(%s%s);' % (suffix, instance, length)

    def visitConst(self, const, instance):
        self.visit(const.type, instance)

    def visitStruct(self, struct, instance):
        print '    trace::localWriter.beginStruct(&_struct%s_sig);' % (struct.tag,)
        for member in struct.members:
            self.visitMember(member, instance)
        print '    trace::localWriter.endStruct();'

    def visitArray(self, array, instance):
        length = '_c' + array.type.tag
        index = '_i' + array.type.tag
        array_length = self.expand(array.length)
        print '    if (%s) {' % instance
        print '        size_t %s = %s > 0 ? %s : 0;' % (length, array_length, array_length)
        print '        trace::localWriter.beginArray(%s);' % length
        print '        for (size_t %s = 0; %s < %s; ++%s) {' % (index, index, length, index)
        print '            trace::localWriter.beginElement();'
        self.visitElement(index, array.type, '(%s)[%s]' % (instance, index))
        print '            trace::localWriter.endElement();'
        print '        }'
        print '        trace::localWriter.endArray();'
        print '    } else {'
        print '        trace::localWriter.writeNull();'
        print '    }'

    def visitBlob(self, blob, instance):
        print '    trace::localWriter.writeBlob(%s, %s);' % (instance, self.expand(blob.size))

    def visitEnum(self, enum, instance):
        print '    trace::localWriter.writeEnum(&_enum%s_sig, %s);' % (enum.tag, instance)

    def visitBitmask(self, bitmask, instance):
        print '    trace::localWriter.writeBitmask(&_bitmask%s_sig, %s);' % (bitmask.tag, instance)

    def visitPointer(self, pointer, instance):
        print '    if (%s) {' % instance
        print '        trace::localWriter.beginArray(1);'
        print '        trace::localWriter.beginElement();'
        self.visit(pointer.type, "*" + instance)
        print '        trace::localWriter.endElement();'
        print '        trace::localWriter.endArray();'
        print '    } else {'
        print '        trace::localWriter.writeNull();'
        print '    }'

    def visitIntPointer(self, pointer, instance):
        print '    trace::localWriter.writePointer((uintptr_t)%s);' % instance

    def visitObjPointer(self, pointer, instance):
        print '    trace::localWriter.writePointer((uintptr_t)%s);' % instance

    def visitLinearPointer(self, pointer, instance):
        print '    trace::localWriter.writePointer((uintptr_t)%s);' % instance

    def visitReference(self, reference, instance):
        self.visit(reference.type, instance)

    def visitHandle(self, handle, instance):
        self.visit(handle.type, instance)

    def visitAlias(self, alias, instance):
        self.visit(alias.type, instance)

    def visitOpaque(self, opaque, instance):
        print '    trace::localWriter.writePointer((uintptr_t)%s);' % instance

    def visitInterface(self, interface, instance):
        assert False

    def visitPolymorphic(self, polymorphic, instance):
        if polymorphic.contextLess:
            print '    _write__%s(%s, %s);' % (polymorphic.tag, polymorphic.switchExpr, instance)
        else:
            switchExpr = self.expand(polymorphic.switchExpr)
            print '    switch (%s) {' % switchExpr
            for cases, type in polymorphic.iterSwitch():
                for case in cases:
                    print '    %s:' % case
                caseInstance = instance
                if type.expr is not None:
                    caseInstance = 'static_cast<%s>(%s)' % (type, caseInstance)
                self.visit(type, caseInstance)
                print '        break;'
            if polymorphic.defaultType is None:
                print r'    default:'
                print r'        os::log("apitrace: warning: %%s: unexpected polymorphic case %%i\n", __FUNCTION__, (int)%s);' % (switchExpr,)
                print r'        trace::localWriter.writeNull();'
                print r'        break;'
            print '    }'


class WrapDecider(stdapi.Traverser):
    '''Type visitor which will decide wheter this type will need wrapping or not.
    
    For complex types (arrays, structures), we need to know this before hand.
    '''

    def __init__(self):
        self.needsWrapping = False

    def visitLinearPointer(self, void):
        pass

    def visitInterface(self, interface):
        self.needsWrapping = True


class ValueWrapper(stdapi.Traverser, stdapi.ExpanderMixin):
    '''Type visitor which will generate the code to wrap an instance.
    
    Wrapping is necessary mostly for interfaces, however interface pointers can
    appear anywhere inside complex types.
    '''

    def visitStruct(self, struct, instance):
        for member in struct.members:
            self.visitMember(member, instance)

    def visitArray(self, array, instance):
        array_length = self.expand(array.length)
        print "    if (%s) {" % instance
        print "        for (size_t _i = 0, _s = %s; _i < _s; ++_i) {" % array_length
        self.visitElement('_i', array.type, instance + "[_i]")
        print "        }"
        print "    }"

    def visitPointer(self, pointer, instance):
        print "    if (%s) {" % instance
        self.visit(pointer.type, "*" + instance)
        print "    }"
    
    def visitObjPointer(self, pointer, instance):
        elem_type = pointer.type.mutable()
        if isinstance(elem_type, stdapi.Interface):
            self.visitInterfacePointer(elem_type, instance)
        elif isinstance(elem_type, stdapi.Alias) and isinstance(elem_type.type, stdapi.Interface):
            self.visitInterfacePointer(elem_type.type, instance)
        else:
            self.visitPointer(pointer, instance)
    
    def visitInterface(self, interface, instance):
        raise NotImplementedError

    def visitInterfacePointer(self, interface, instance):
        print "    if (%s) {" % instance
        print "        %s = %s::_Create(__FUNCTION__, %s);" % (instance, getWrapperInterfaceName(interface), instance)
        print "    }"
    
    def visitPolymorphic(self, type, instance):
        # XXX: There might be polymorphic values that need wrapping in the future
        raise NotImplementedError


class ValueUnwrapper(ValueWrapper):
    '''Reverse of ValueWrapper.'''

    allocated = False

    def visitStruct(self, struct, instance):
        if not self.allocated:
            # Argument is constant. We need to create a non const
            print '    {'
            print "        %s * _t = static_cast<%s *>(alloca(sizeof *_t));" % (struct, struct)
            print '        *_t = %s;' % (instance,)
            assert instance.startswith('*')
            print '        %s = _t;' % (instance[1:],)
            instance = '*_t'
            self.allocated = True
            try:
                return ValueWrapper.visitStruct(self, struct, instance)
            finally:
                print '    }'
        else:
            return ValueWrapper.visitStruct(self, struct, instance)

    def visitArray(self, array, instance):
        if self.allocated or isinstance(instance, stdapi.Interface):
            return ValueWrapper.visitArray(self, array, instance)
        array_length = self.expand(array.length)
        elem_type = array.type.mutable()
        print "    if (%s && %s) {" % (instance, array_length)
        print "        %s * _t = static_cast<%s *>(alloca(%s * sizeof *_t));" % (elem_type, elem_type, array_length)
        print "        for (size_t _i = 0, _s = %s; _i < _s; ++_i) {" % array_length
        print "            _t[_i] = %s[_i];" % instance 
        self.allocated = True
        self.visit(array.type, "_t[_i]")
        print "        }"
        print "        %s = _t;" % instance
        print "    }"

    def visitInterfacePointer(self, interface, instance):
        print r'    if (%s) {' % instance
        print r'        const %s *pWrapper = static_cast<const %s*>(%s);' % (getWrapperInterfaceName(interface), getWrapperInterfaceName(interface), instance)
        print r'        if (pWrapper && pWrapper->m_dwMagic == 0xd8365d6c) {'
        print r'            %s = pWrapper->m_pInstance;' % (instance,)
        print r'        } else {'
        print r'            os::log("apitrace: warning: %%s: unexpected %%s pointer\n", __FUNCTION__, "%s");' % interface.name
        print r'        }'
        print r'    }'


class Tracer:
    '''Base class to orchestrate the code generation of API tracing.'''

    # 0-3 are reserved to memcpy, malloc, free, and realloc
    __id = 4

    def __init__(self):
        self.api = None

    def serializerFactory(self):
        '''Create a serializer.
        
        Can be overriden by derived classes to inject their own serialzer.
        '''

        return ValueSerializer()

    def traceApi(self, api):
        self.api = api

        self.header(api)

        # Includes
        for module in api.modules:
            for header in module.headers:
                print header
        print

        # Generate the serializer functions
        types = api.getAllTypes()
        visitor = ComplexValueSerializer(self.serializerFactory())
        map(visitor.visit, types)
        print

        # Interfaces wrapers
        self.traceInterfaces(api)

        # Function wrappers
        self.interface = None
        self.base = None
        for function in api.getAllFunctions():
            self.traceFunctionDecl(function)
        for function in api.getAllFunctions():
            self.traceFunctionImpl(function)
        print

        self.footer(api)

    def header(self, api):
        print '#ifdef _WIN32'
        print '#  include <malloc.h> // alloca'
        print '#  ifndef alloca'
        print '#    define alloca _alloca'
        print '#  endif'
        print '#else'
        print '#  include <alloca.h> // alloca'
        print '#endif'
        print
        print '#include "trace.hpp"'
        print
        print 'static std::map<void *, void *> g_WrappedObjects;'

    def footer(self, api):
        pass

    def traceFunctionDecl(self, function):
        # Per-function declarations

        if not function.internal:
            if function.args:
                print 'static const char * _%s_args[%u] = {%s};' % (function.name, len(function.args), ', '.join(['"%s"' % arg.name for arg in function.args]))
            else:
                print 'static const char ** _%s_args = NULL;' % (function.name,)
            print 'static const trace::FunctionSig _%s_sig = {%u, "%s", %u, _%s_args};' % (function.name, self.getFunctionSigId(), function.name, len(function.args), function.name)
            print

    def getFunctionSigId(self):
        id = Tracer.__id
        Tracer.__id += 1
        return id

    def isFunctionPublic(self, function):
        return True

    def traceFunctionImpl(self, function):
        if self.isFunctionPublic(function):
            print 'extern "C" PUBLIC'
        else:
            print 'extern "C" PRIVATE'
        print function.prototype() + ' {'
        if function.type is not stdapi.Void:
            print '    %s _result;' % function.type

        # No-op if tracing is disabled
        print '    if (!trace::isTracingEnabled()) {'
        self.doInvokeFunction(function)
        if function.type is not stdapi.Void:
            print '        return _result;'
        else:
            print '        return;'
        print '    }'

        self.traceFunctionImplBody(function)
        if function.type is not stdapi.Void:
            print '    return _result;'
        print '}'
        print

    def traceFunctionImplBody(self, function):
        if not function.internal:
            print '    unsigned _call = trace::localWriter.beginEnter(&_%s_sig);' % (function.name,)
            for arg in function.args:
                if not arg.output:
                    self.unwrapArg(function, arg)
            for arg in function.args:
                if not arg.output:
                    self.serializeArg(function, arg)
            print '    trace::localWriter.endEnter();'
        self.invokeFunction(function)
        if not function.internal:
            print '    trace::localWriter.beginLeave(_call);'
            print '    if (%s) {' % self.wasFunctionSuccessful(function)
            for arg in function.args:
                if arg.output:
                    self.serializeArg(function, arg)
                    self.wrapArg(function, arg)
            print '    }'
            if function.type is not stdapi.Void:
                self.serializeRet(function, "_result")
            if function.type is not stdapi.Void:
                self.wrapRet(function, "_result")
            print '    trace::localWriter.endLeave();'

    def invokeFunction(self, function):
        self.doInvokeFunction(function)

    def doInvokeFunction(self, function, prefix='_', suffix=''):
        # Same as invokeFunction() but called both when trace is enabled or disabled.
        if function.type is stdapi.Void:
            result = ''
        else:
            result = '_result = '
        dispatch = prefix + function.name + suffix
        print '    %s%s(%s);' % (result, dispatch, ', '.join([str(arg.name) for arg in function.args]))

    def wasFunctionSuccessful(self, function):
        if function.type is stdapi.Void:
            return 'true'
        if str(function.type) == 'HRESULT':
            return 'SUCCEEDED(_result)'
        return 'true'

    def serializeArg(self, function, arg):
        print '    trace::localWriter.beginArg(%u);' % (arg.index,)
        self.serializeArgValue(function, arg)
        print '    trace::localWriter.endArg();'

    def serializeArgValue(self, function, arg):
        self.serializeValue(arg.type, arg.name)

    def wrapArg(self, function, arg):
        assert not isinstance(arg.type, stdapi.ObjPointer)

        from specs.winapi import REFIID
        riid = None
        for other_arg in function.args:
            if not other_arg.output and other_arg.type is REFIID:
                riid = other_arg
        if riid is not None \
           and isinstance(arg.type, stdapi.Pointer) \
           and isinstance(arg.type.type, stdapi.ObjPointer):
            self.wrapIid(function, riid, arg)
            return

        self.wrapValue(arg.type, arg.name)

    def unwrapArg(self, function, arg):
        self.unwrapValue(arg.type, arg.name)

    def serializeRet(self, function, instance):
        print '    trace::localWriter.beginReturn();'
        self.serializeValue(function.type, instance)
        print '    trace::localWriter.endReturn();'

    def serializeValue(self, type, instance):
        serializer = self.serializerFactory()
        serializer.visit(type, instance)

    def wrapRet(self, function, instance):
        self.wrapValue(function.type, instance)

    def needsWrapping(self, type):
        visitor = WrapDecider()
        visitor.visit(type)
        return visitor.needsWrapping

    def wrapValue(self, type, instance):
        if self.needsWrapping(type):
            visitor = ValueWrapper()
            visitor.visit(type, instance)

    def unwrapValue(self, type, instance):
        if self.needsWrapping(type):
            visitor = ValueUnwrapper()
            visitor.visit(type, instance)

    def traceInterfaces(self, api):
        interfaces = api.getAllInterfaces()
        if not interfaces:
            return
        map(self.declareWrapperInterface, interfaces)
        self.implementIidWrapper(api)
        map(self.implementWrapperInterface, interfaces)
        print

    def declareWrapperInterface(self, interface):
        print "class %s : public %s " % (getWrapperInterfaceName(interface), interface.name)
        print "{"
        print "private:"
        print "    %s(%s * pInstance);" % (getWrapperInterfaceName(interface), interface.name)
        print "    virtual ~%s();" % getWrapperInterfaceName(interface)
        print "public:"
        print "    static %s* _Create(const char *functionName, %s * pInstance);" % (getWrapperInterfaceName(interface), interface.name)
        print
        for method in interface.iterMethods():
            print "    " + method.prototype() + ";"
        print
        #print "private:"
        for type, name, value in self.enumWrapperInterfaceVariables(interface):
            print '    %s %s;' % (type, name)
        for i in range(64):
            print r'    virtual void _dummy%i(void) const {' % i
            print r'        os::log("error: %s: unexpected virtual method\n");' % interface.name
            print r'        os::abort();'
            print r'    }'
        print "};"
        print

    def enumWrapperInterfaceVariables(self, interface):
        return [
            ("DWORD", "m_dwMagic", "0xd8365d6c"),
            ("%s *" % interface.name, "m_pInstance", "pInstance"),
            ("void *", "m_pVtbl", "*(void **)pInstance"),
            ("UINT", "m_NumMethods", len(list(interface.iterBaseMethods()))),
        ] 

    def implementWrapperInterface(self, interface):
        self.interface = interface

        # Private constructor
        print '%s::%s(%s * pInstance) {' % (getWrapperInterfaceName(interface), getWrapperInterfaceName(interface), interface.name)
        for type, name, value in self.enumWrapperInterfaceVariables(interface):
            if value is not None:
                print '    %s = %s;' % (name, value)
        print '}'
        print

        # Public constructor
        print '%s *%s::_Create(const char *functionName, %s * pInstance) {' % (getWrapperInterfaceName(interface), getWrapperInterfaceName(interface), interface.name)
        print r'    std::map<void *, void *>::const_iterator it = g_WrappedObjects.find(pInstance);'
        print r'    if (it != g_WrappedObjects.end()) {'
        print r'        Wrap%s *pWrapper = (Wrap%s *)it->second;' % (interface.name, interface.name)
        print r'        assert(pWrapper);'
        print r'        assert(pWrapper->m_dwMagic == 0xd8365d6c);'
        print r'        assert(pWrapper->m_pInstance == pInstance);'
        print r'        if (pWrapper->m_pVtbl == *(void **)pInstance &&'
        print r'            pWrapper->m_NumMethods >= %s) {' % len(list(interface.iterBaseMethods()))
        #print r'            os::log("%s: fetched pvObj=%p pWrapper=%p pVtbl=%p\n", functionName, pInstance, pWrapper, pWrapper->m_pVtbl);'
        print r'            return pWrapper;'
        print r'        }'
        print r'    }'
        print r'    Wrap%s *pWrapper = new Wrap%s(pInstance);' % (interface.name, interface.name)
        #print r'    os::log("%%s: created %s pvObj=%%p pWrapper=%%p pVtbl=%%p\n", functionName, pInstance, pWrapper, pWrapper->m_pVtbl);' % interface.name
        print r'    g_WrappedObjects[pInstance] = pWrapper;'
        print r'    return pWrapper;'
        print '}'
        print

        # Destructor
        print '%s::~%s() {' % (getWrapperInterfaceName(interface), getWrapperInterfaceName(interface))
        #print r'        os::log("%s::Release: deleted pvObj=%%p pWrapper=%%p pVtbl=%%p\n", m_pInstance, this, m_pVtbl);' % interface.name
        print r'        g_WrappedObjects.erase(m_pInstance);'
        print '}'
        print
        
        for base, method in interface.iterBaseMethods():
            self.base = base
            self.implementWrapperInterfaceMethod(interface, base, method)

        print

    def implementWrapperInterfaceMethod(self, interface, base, method):
        print method.prototype(getWrapperInterfaceName(interface) + '::' + method.name) + ' {'

        if False:
            print r'    os::log("%%s(%%p -> %%p)\n", "%s", this, m_pInstance);' % (getWrapperInterfaceName(interface) + '::' + method.name)

        if method.type is not stdapi.Void:
            print '    %s _result;' % method.type
    
        self.implementWrapperInterfaceMethodBody(interface, base, method)
    
        if method.type is not stdapi.Void:
            print '    return _result;'
        print '}'
        print

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        assert not method.internal

        print '    static const char * _args[%u] = {%s};' % (len(method.args) + 1, ', '.join(['"this"'] + ['"%s"' % arg.name for arg in method.args]))
        print '    static const trace::FunctionSig _sig = {%u, "%s", %u, _args};' % (self.getFunctionSigId(), interface.name + '::' + method.name, len(method.args) + 1)

        print '    %s *_this = static_cast<%s *>(m_pInstance);' % (base, base)

        print '    unsigned _call = trace::localWriter.beginEnter(&_sig);'
        print '    trace::localWriter.beginArg(0);'
        print '    trace::localWriter.writePointer((uintptr_t)m_pInstance);'
        print '    trace::localWriter.endArg();'
        for arg in method.args:
            if not arg.output:
                self.unwrapArg(method, arg)
        for arg in method.args:
            if not arg.output:
                self.serializeArg(method, arg)
        print '    trace::localWriter.endEnter();'
        
        self.invokeMethod(interface, base, method)

        print '    trace::localWriter.beginLeave(_call);'

        print '    if (%s) {' % self.wasFunctionSuccessful(method)
        for arg in method.args:
            if arg.output:
                self.serializeArg(method, arg)
                self.wrapArg(method, arg)
        print '    }'

        if method.type is not stdapi.Void:
            self.serializeRet(method, '_result')
        if method.type is not stdapi.Void:
            self.wrapRet(method, '_result')

        if method.name == 'Release':
            assert method.type is not stdapi.Void
            print r'    if (!_result) {'
            print r'        delete this;'
            print r'    }'
        
        print '    trace::localWriter.endLeave();'

    def implementIidWrapper(self, api):
        print r'static void'
        print r'warnIID(const char *functionName, REFIID riid, const char *reason) {'
        print r'    os::log("apitrace: warning: %s: %s IID {0x%08lX,0x%04X,0x%04X,{0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X}}\n",'
        print r'            functionName, reason,'
        print r'            riid.Data1, riid.Data2, riid.Data3,'
        print r'            riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);'
        print r'}'
        print 
        print r'static void'
        print r'wrapIID(const char *functionName, REFIID riid, void * * ppvObj) {'
        print r'    if (!ppvObj || !*ppvObj) {'
        print r'        return;'
        print r'    }'
        else_ = ''
        for iface in api.getAllInterfaces():
            print r'    %sif (riid == IID_%s) {' % (else_, iface.name)
            print r'        *ppvObj = Wrap%s::_Create(functionName, (%s *) *ppvObj);' % (iface.name, iface.name)
            print r'    }'
            else_ = 'else '
        print r'    %s{' % else_
        print r'        warnIID(functionName, riid, "unknown");'
        print r'    }'
        print r'}'
        print

    def wrapIid(self, function, riid, out):
        # Cast output arg to `void **` if necessary
        out_name = out.name
        obj_type = out.type.type.type
        if not obj_type is stdapi.Void:
            assert isinstance(obj_type, stdapi.Interface)
            out_name = 'reinterpret_cast<void * *>(%s)' % out_name

        print r'    if (%s && *%s) {' % (out.name, out.name)
        functionName = function.name
        else_ = ''
        if self.interface is not None:
            functionName = self.interface.name + '::' + functionName
            print r'        if (*%s == m_pInstance &&' % (out_name,)
            print r'            (%s)) {' % ' || '.join('%s == IID_%s' % (riid.name, iface.name) for iface in self.interface.iterBases())
            print r'            *%s = this;' % (out_name,)
            print r'        }'
            else_ = 'else '
        print r'        %s{' % else_
        print r'             wrapIID("%s", %s, %s);' % (functionName, riid.name, out_name)
        print r'        }'
        print r'    }'

    def invokeMethod(self, interface, base, method):
        if method.type is stdapi.Void:
            result = ''
        else:
            result = '_result = '
        print '    %s_this->%s(%s);' % (result, method.name, ', '.join([str(arg.name) for arg in method.args]))
    
    def emit_memcpy(self, dest, src, length):
        print '        unsigned _call = trace::localWriter.beginEnter(&trace::memcpy_sig, true);'
        print '        trace::localWriter.beginArg(0);'
        print '        trace::localWriter.writePointer((uintptr_t)%s);' % dest
        print '        trace::localWriter.endArg();'
        print '        trace::localWriter.beginArg(1);'
        print '        trace::localWriter.writeBlob(%s, %s);' % (src, length)
        print '        trace::localWriter.endArg();'
        print '        trace::localWriter.beginArg(2);'
        print '        trace::localWriter.writeUInt(%s);' % length
        print '        trace::localWriter.endArg();'
        print '        trace::localWriter.endEnter();'
        print '        trace::localWriter.beginLeave(_call);'
        print '        trace::localWriter.endLeave();'
    
    def fake_call(self, function, args):
        print '            unsigned _fake_call = trace::localWriter.beginEnter(&_%s_sig);' % (function.name,)
        for arg, instance in zip(function.args, args):
            assert not arg.output
            print '            trace::localWriter.beginArg(%u);' % (arg.index,)
            self.serializeValue(arg.type, instance)
            print '            trace::localWriter.endArg();'
        print '            trace::localWriter.endEnter();'
        print '            trace::localWriter.beginLeave(_fake_call);'
        print '            trace::localWriter.endLeave();'
       

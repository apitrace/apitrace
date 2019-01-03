#!/usr/bin/env python



copyright = '''
##########################################################################
#
# Copyright 2009-2016 VMware, Inc.
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
'''


#
# Generates API specs from headers using castxml/pygccxml.
#
# Usage:
#
#   sudo apt-get install castxml mingw-w64-i686-dev
#   pip install 'pygccxml==1.9.1'
#   python specs/scripts/cxx2api.py -Idxsdk/Include -DD2D_USE_C_DEFINITIONS dcomp.h dcomptypes.h dcompanimation.h
#
# See also:
# - http://pygccxml.readthedocs.org/en/develop/index.html
# - https://github.com/CastXML/CastXML/blob/master/doc/manual/castxml.1.rst
#

import os.path
import sys
import io as StringIO
import subprocess

from pygccxml import utils
from pygccxml import parser
from pygccxml import declarations

from pygccxml.declarations import algorithm
from pygccxml.declarations import decl_visitor
from pygccxml.declarations import type_traits
from pygccxml.declarations import type_visitor


class decl_dumper_t(decl_visitor.decl_visitor_t):

    def __init__(self, decl = None):
        decl_visitor.decl_visitor_t.__init__(self)
        self.decl = decl
        self.result = None

    def clone(self):
        return decl_dumper_t(self.decl)

    def visit_class(self):
        class_ = self.decl
        assert class_.class_type in ('struct', 'union')
        self.result = class_.name

    def visit_class_declaration(self):
        class_ = self.decl
        self.result = class_.name

    def visit_typedef(self):
        typedef = self.decl
        self.result = typedef.name

    def visit_enumeration(self):
        self.result = self.decl.name


def dump_decl(decl):
    visitor = decl_dumper_t(decl)
    algorithm.apply_visitor(visitor, decl)
    return visitor.result


class type_dumper_t(type_visitor.type_visitor_t):

    def __init__(self, type):
        type_visitor.type_visitor_t.__init__(self)
        self.type = type
        self.result = None

    def clone(self):
        return type_dumper_t(self.type)

    def visit_void(self):
        self.result = 'Void'

    def visit_char(self):
        self.result = 'Char'

    def visit_unsigned_char(self):
        self.result = 'UChar'

    def visit_signed_char(self):
        self.result = 'SChar'

    def visit_wchar(self):
        raise NotImplementedError

    def visit_short_int(self):
        self.result = 'Short'

    def visit_short_unsigned_int(self):
        self.result = 'UShort'

    def visit_bool(self):
        raise NotImplementedError

    def visit_int(self):
        self.result = 'Int'

    def visit_unsigned_int(self):
        self.result = 'UInt'

    def visit_long_int(self):
        self.result = 'Long'

    def visit_long_unsigned_int(self):
        self.result = 'ULong'

    def visit_long_long_int(self):
        self.result = 'LongLong'

    def visit_long_long_unsigned_int(self):
        self.result = 'ULongLong'

    def visit_float(self):
        self.result = "Float"

    def visit_double(self):
        self.result = "Double"

    def visit_array(self):
        base_type = dump_type(self.type.base)
        length = self.type.size
        try:
            int(length)
        except ValueError:
            length = '"%s"' % length
        self.result = 'Array(%s, %s)' % (base_type, length)

    def visit_pointer(self):
        base_type = dump_type(self.type.base)
        # TODO: Use ObjPointer where appropriate
        #if isinstance(self.type.base, declarations.cpptypes.declarated_t):
        #    decl = self.type.base.declaration
        #    if isinstance(decl, declarations.typedef.typedef_t):
        #        print(decl.type, type(decl.type))
        #    if isinstance(decl, declarations.class_declaration.class_t):
        #        if decl.public_members:
        #            self.result = 'ObjPointer(%s)' % decl.name
        #            return
        #    if isinstance(decl, declarations.class_declaration.class_declaration_t):
        #        if decl.public_members:
        #            self.result = 'ObjPointer(%s)' % decl.name
        #            return
        if base_type.startswith('IDComposition') or \
           base_type.startswith('IDXGI') or \
           base_type == 'IUnknown':
            self.result = 'ObjPointer(%s)' % base_type
            return
        self.result = 'Pointer(%s)' % base_type

    def visit_reference(self):
        base_type = dump_type(self.type.base)
        if base_type == 'Const(IID)':
            self.result = 'REFIID'
        elif base_type == 'Const(GUID)':
            self.result = 'REFGUID'
        else:
            self.result = 'Reference(%s)' % base_type

    def visit_const(self):
        self.result = 'Const(%s)' % dump_type(self.type.base)

    def visit_declarated(self):
        decl = self.type.declaration
        self.result = dump_decl(decl)

    def visit_free_function_type(self):
        self.result = 'Opaque("%s")' % self.type


def dump_type(type):
    visitor = type_dumper_t(type)
    algorithm.apply_visitor(visitor, type)
    # XXX: RECT becomes tagRECT somehow
    if visitor.result == 'tagRECT':
        return 'RECT'
    return visitor.result


def is_interface(class_):
    if not class_.name.startswith('I'):
        return
    if len(class_.bases) != 1:
        return False
    # TODO: Ensure interface derives from IUnknown
    return True


class decl2_dumper_t(decl_visitor.decl_visitor_t):

    def __init__(self, name):
        decl_visitor.decl_visitor_t.__init__(self)

        self.name = name

        # The current declaration
        self.decl = None

        self.interfaces = StringIO.StringIO()
        self.methods = StringIO.StringIO()
        self.functions = StringIO.StringIO()

    def start(self):
        print(copyright.strip())
        print()
        print()
        print(r'from winapi import *')
        print()

    def finish(self):
        sys.stdout.write(self.interfaces.getvalue())
        sys.stdout.write('\n')
        sys.stdout.write(self.methods.getvalue())

        name = self.name
        sys.stdout.write('%s = Module(%r)\n' % (name, name))
        sys.stdout.write('%s.addFunctions([\n' % (name,))
        sys.stdout.write(self.functions.getvalue())
        sys.stdout.write('])\n\n')

    def clone(self):
        return decl_dumper_t(self.decl)

    def visit_class(self):
        class_ = self.decl
        assert class_.class_type in ('struct', 'union')

        if is_interface(class_):
            self.visit_interface()
        elif class_.name != '':
            self.visit_struct(class_.name, class_)

    def visit_struct(self, decl_name, decl):
        struct = decl
        print(r'%s = Struct(%r, [' % (decl_name, decl_name))
        for variable in struct.variables(allow_empty=True):
            var_type = dump_type(variable.decl_type)
            print(r'    (%s, %r),' % (var_type, variable.name))
        print(r'])')
        print()

    def visit_interface(self):
        class_ = self.decl
        assert len(class_.bases) == 1
        base = class_.bases[0]

        s = self.interfaces
        s.write('%s = Interface(%r, %s)\n' % (class_.name, class_.name, base.related_class.name))

        s = self.methods
        s.write('%s.methods += [\n' % (class_.name,))
        for member in class_.public_members:
            if member.virtuality != 'pure virtual':
                continue
            ret_type = dump_type(member.return_type)
            arg_types = self.convert_args(member.arguments)
            s.write('    StdMethod(%s, %r, [%s]),\n' % (ret_type, member.name, arg_types))
        s.write(']\n\n')

    def convert_args(self, args):
        # TODO: use __attribute__ ((annotate ("out")))
        # See also:
        # - https://github.com/CastXML/CastXML/issues/25
        # XXX: Requires a castxml version newer than the one in Ubuntu 15.10
        arg_types = []
        for arg in args:
            if arg.attributes is not None:
                sys.stderr.write('warning: found %s attribute %r\n' % (arg.name, arg.attributes))
            res_arg_type = dump_type(arg.decl_type)
            res_arg = '(%s, %r)' % (res_arg_type, arg.name)

            # Infer output arguments
            if res_arg_type.startswith('Pointer(') and \
               not res_arg_type.startswith('Pointer(Const('):
                   res_arg = 'Out' + res_arg

            arg_types.append(res_arg)

        arg_types = ', '.join(arg_types)
        return arg_types

    def visit_class_declaration(self):
        pass

    def visit_typedef(self):
        typedef = self.decl
        base_type = dump_type(typedef.decl_type)
        if base_type == typedef.name:
            # Ignore `typedef struct Foo Foo;`
            return
        if base_type == '':
            if isinstance(typedef.decl_type, declarations.cpptypes.declarated_t):
                base_decl = typedef.decl_type.declaration
                self.visit_struct(typedef.name, base_decl)
                return
        print(r'%s = Alias(%r, %s)' % (typedef.name, typedef.name, base_type))
        print()

    def visit_enumeration(self):
        enum = self.decl
        print(r'%s = Enum(%r, [' % (enum.name, enum.name))
        for name, value in enum.values:
            print(r'    %r,' % (name,))
        print(r'])')
        print()

    def visit_variable(self):
        pass

    def visit_free_function(self):
        function = self.decl
        if function.has_inline:
            return

        s = self.functions
        ret_type = dump_type(function.return_type)
        arg_types = self.convert_args(function.arguments)
        s.write('    StdFunction(%s, %r, [%s]),\n' % (ret_type, function.name, arg_types))

    def visit_free_operator(self):
        pass


def main():
    defines = []
    includes = []
    cxxflags = [
        '-Wno-unknown-attributes',
        '-Wno-unused-value',
        '-Wno-macro-redefined',
    ]
    compiler = 'g++'

    args = sys.argv[1:]
    while args and args[0].startswith('-'):
        arg = args.pop(0)
        if arg.startswith('-I'):
            include = arg[2:]
            includes.append(include)
        elif arg.startswith('-D'):
            define = arg[2:]
            defines.append(define)
        else:
            sys.stderr.write('error: unknown option %r\n' % arg)
            sys.exit(1)

    winsdk = True
    if winsdk:
        # Set up Clang compiler flags to use MinGW runtime
        # http://stackoverflow.com/a/19839946
        p = subprocess.Popen(
            ["x86_64-w64-mingw32-g++", "-x", "c++", "-E", "-Wp,-v", '-', '-fsyntax-only'],
            stdin=open(os.devnull, 'rt'),
            stdout=open(os.devnull, 'wt'),
            stderr=subprocess.PIPE)
        includes.append('/usr/share/castxml/clang/include')
        for line in p.stderr:
            if line.startswith(' '):
                include = line.strip()
                if os.path.isdir(include):
                    if os.path.exists(os.path.join(include, 'ia32intrin.h')):
                        # XXX: We must use Clang's intrinsic headers
                        continue
                    includes.append(os.path.normpath(include))

        winver = 0x0602

        defines += [
            # emulate MinGW
            '__MINGW32__',
            '_WIN32',
            '_WIN64',
            '__declspec(x)=',
            # Avoid namespace pollution when including windows.h
            # http://support.microsoft.com/kb/166474
            'WIN32_LEAN_AND_MEAN',
            # Set Windows version to 8.1
            '_WIN32_WINNT=0x%04X' % winver,
            'WINVER=0x%04X' % winver,
            'NTDDI_VERSION=0x%04X0000' % winver,
            # Prevent headers from requiring a rpcndr.h version beyond MinGW's
            '__REQUIRED_RPCNDR_H_VERSION__=475',
            # Avoid C++ helper classes
            'D3D10_NO_HELPERS',
            'D3D11_NO_HELPERS',
            'D3D11_VIDEO_NO_HELPERS',
        ]

        # XXX: Change compiler?
        #compiler = 'cl'

        # XXX: This doesn't seem to work well
        cxxflags += [
            #'-m32',
            #'-target', 'x86_64-pc-mingw32',
        ]

    sys.stderr.write('Include path:\n')
    for include in includes:
        sys.stderr.write('  %s\n' % include)
    sys.stderr.write('Definitions:\n')
    for define in defines:
        sys.stderr.write('  %s\n' % define)

    import logging
    utils.loggers.set_level(logging.DEBUG)

    # Find the location of the xml generator (castxml or gccxml)
    generator_path, generator_name = utils.find_xml_generator("castxml")

    # Configure the xml generator
    config = parser.xml_generator_configuration_t(
        xml_generator_path=generator_path,
        xml_generator=generator_name,
        define_symbols = defines,
        include_paths = includes,
        cflags = ' '.join(cxxflags),
        compiler = compiler,
        #keep_xml = True,
    )

    script_dir = os.path.dirname(__file__)
    headers = [
        os.path.join(script_dir, '..', '..', 'compat', 'winsdk_compat.h'),
        os.path.join(script_dir, 'cxx2api.h'),
    ]
    main_header = args[0]
    headers.append(main_header)

    decls = parser.parse(headers, config, parser.COMPILATION_MODE.ALL_AT_ONCE)
    global_ns = declarations.get_global_namespace(decls)

    def decl_filter(decl):
        location = decl.location
        if location is None:
            return False
        return os.path.basename(location.file_name) in list(map(os.path.basename, args))

    module, _ = os.path.splitext(main_header)
    visitor = decl2_dumper_t(module)
    visitor.start()
    for decl in global_ns.declarations:
        if not decl_filter(decl):
            continue

        if sys.stdout.isatty():
            print('# ' + str(decl))

        visitor.decl = decl
        algorithm.apply_visitor(visitor, decl)
    visitor.finish()


if __name__ == '__main__':
    main()

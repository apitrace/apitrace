#!/usr/bin/env python

# Copyright 2010 VMware, Inc.
# Copyright 2004, 2005 IBM Corporation
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# on the rights to use, copy, modify, merge, publish, distribute, sub
# license, and/or sell copies of the Software, and to permit persons to whom
# the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
# Authors:
#    Jose Fonseca <jfonseca@vmware.com>
#    Ian Romanick <idr@us.ibm.com>

import os.path
import sys

glapi_path =  os.path.join(os.environ['MESA'], 'src/mapi/glapi/gen')
sys.path.insert(0, glapi_path)

import gl_XML, glX_XML
import license
import getopt, copy, string


class glx_enum_function:
    def __init__(self, func_name, enum_dict):
        self.name = func_name
        self.mode = 1
        self.sig = None

        # "enums" is a set of lists.  The element in the set is the
        # value of the enum.  The list is the list of names for that
        # value.  For example, [0x8126] = {"POINT_SIZE_MIN",
        # "POINT_SIZE_MIN_ARB", "POINT_SIZE_MIN_EXT",
        # "POINT_SIZE_MIN_SGIS"}.

        self.enums = {}

        # "count" is indexed by count values.  Each element of count
        # is a list of index to "enums" that have that number of
        # associated data elements.  For example, [4] = 
        # {GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION,
        # GL_AMBIENT_AND_DIFFUSE} (the enum names are used here,
        # but the actual hexadecimal values would be in the array).

        self.count = {}


        # Fill self.count and self.enums using the dictionary of enums
        # that was passed in.  The generic Get functions (e.g.,
        # GetBooleanv and friends) are handled specially here.  In
        # the data the generic Get functions are refered to as "Get".

        if func_name in ["GetIntegerv", "GetBooleanv", "GetFloatv", "GetDoublev"]:
            match_name = "Get"
        else:
            match_name = func_name

        mode_set = 0
        for enum_name in enum_dict:
            e = enum_dict[ enum_name ]

            if e.functions.has_key( match_name ):
                [count, mode] = e.functions[ match_name ]

                if mode_set and mode != self.mode:
                    raise RuntimeError("Not all enums for %s have the same mode." % (func_name))

                self.mode = mode

                if self.enums.has_key( e.value ):
                    if e.name not in self.enums[ e.value ]:
                        self.enums[ e.value ].append( e )
                else:
                    if not self.count.has_key( count ):
                        self.count[ count ] = []

                    self.enums[ e.value ] = [ e ]
                    self.count[ count ].append( e.value )


        return


    def signature( self ):
        if self.sig == None:
            self.sig = ""
            for i in self.count:
                if i == None:
                    raise RuntimeError("i is None.  WTF?")

                self.count[i].sort()
                for e in self.count[i]:
                    self.sig += "%04x,%d," % (e, i)

        return self.sig


    def PrintUsingSwitch(self, name):
        """Emit the body of the __gl*_size function using a 
        switch-statement."""

        print '    switch (pname) {'

        for c in self.count:
            for e in self.count[c]:
                first = 1

                # There may be multiple enums with the same
                # value.  This happens has extensions are
                # promoted from vendor-specific or EXT to
                # ARB and to the core.  Emit the first one as
                # a case label, and emit the others as
                # commented-out case labels.

                list = {}
                for enum_obj in self.enums[e]:
                    list[ enum_obj.priority() ] = enum_obj.name

                keys = list.keys()
                keys.sort()
                for k in keys:
                    j = list[k]
                    if first:
                        print '    case GL_%s:' % (j)
                        first = 0
                    else:
                        print '/*  case GL_%s:*/' % (j)
                    
            if c == -1:
                print '        return __gl%s_variable_size(pname);' % (name)
            else:
                print '        return %u;' % (c)
                    
        print '    default:' 
        print '        OS::DebugMessage("warning: %s: unknown GLenum 0x%04X\\n", __FUNCTION__, pname);'
        print '        return 1;'
        print '    }'


    def Print(self, name):
        print 'static inline size_t'
        print '__gl%s_size(GLenum pname)' % (name)
        print '{'

        self.PrintUsingSwitch(name)

        print '}'
        print ''


class glx_server_enum_function(glx_enum_function):
    def __init__(self, func, enum_dict):
        glx_enum_function.__init__(self, func.name, enum_dict)
        
        self.function = func
        return


    def signature( self ):
        if self.sig == None:
            sig = glx_enum_function.signature(self)

            p = self.function.variable_length_parameter()
            if p:
                sig += "%u" % (p.size())

            self.sig = sig

        return self.sig;


    def Print(self, name, printer):
        f = self.function
        printer.common_func_print_just_header( f )

        fixup = []
        
        foo = {}
        for param_name in f.count_parameter_list:
            o = f.offset_of( param_name )
            foo[o] = param_name

        for param_name in f.counter_list:
            o = f.offset_of( param_name )
            foo[o] = param_name

        keys = foo.keys()
        keys.sort()
        for o in keys:
            p = f.parameters_by_name[ foo[o] ]

            printer.common_emit_one_arg(p, "pc", 0)
            fixup.append( p.name )


        print '    GLsizei compsize;'
        print ''

        printer.common_emit_fixups(fixup)

        print ''
        print '    compsize = __gl%s_size(%s);' % (f.name, string.join(f.count_parameter_list, ","))
        p = f.variable_length_parameter()
        print '    return __GLX_PAD(%s);' % (p.size_string())

        print '}'
        print ''


class PrintGlxSizeStubs_c(gl_XML.gl_print_base):

    def __init__(self):
        gl_XML.gl_print_base.__init__(self)

        self.name = "glX_proto_size.py (from Mesa)"
        self.license = license.bsd_license_template % ( "Copyright 2010 VMware, Inc.\nCopyright 2004 IBM Corporation", "AUTHORS")

    def printRealHeader(self):
        print
        print
        print '#include <cassert>'
        print
        print '#include "glimports.hpp"'
        print
        print

    def printBody(self, api):
        enum_sigs = {}

        for func in api.functionIterateByOffset():
            ef = glx_enum_function( func.name, api.enums_by_name )
            if len(ef.enums) == 0:
                continue

            if True:
                sig = ef.signature()
                if enum_sigs.has_key( sig ):
                    alias_name, real_name = func.name, enum_sigs[ sig ]
                    print '#define __gl%s_size __gl%s_size' % (alias_name, real_name)
                    print
                else:
                    enum_sigs[ sig ] = func.name
                    ef.Print( func.name )


def show_usage():
    print "Usage: %s [-f input_file_name] [--only-get | --only-set] [--get-alias-set]" % sys.argv[0]
    sys.exit(1)


if __name__ == '__main__':
    file_name = os.path.join(glapi_path, "gl_API.xml")

    try:
        (args, trail) = getopt.getopt(sys.argv[1:], "f:h:", ["header-tag"])
    except Exception,e:
        show_usage()

    header_tag = None

    for (arg,val) in args:
        if arg == "-f":
            file_name = val
        elif (arg == '-h') or (arg == "--header-tag"):
            header_tag = val

    printer = PrintGlxSizeStubs_c()

    api = gl_XML.parse_GL_API( file_name, glX_XML.glx_item_factory() )


    printer.Print( api )

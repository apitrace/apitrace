#!/usr/bin/python

# Copyright 2008 VMware, Inc.
# Copyright 2004 IBM Corporation
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

import gl_XML
import license
import sys, getopt

type_map = {
   'void': 'Void',
   'int': 'Int',
   'float': 'Float',
}

def parse_type(tokens, count = 0):
    if count:
        if tokens[-1] == '**':
            return 'Array(Pointer(%s), "%s")' % (parse_type(tokens[:-1]), count)
        if tokens[-1] == '*':
            return 'Array(%s, "%s")' % (parse_type(tokens[:-1]), count)
    else:
        if tokens[-1] == '**':
            return 'Pointer(Pointer(%s))' % parse_type(tokens[:-1])
        if tokens[-1] == '*':
            return 'Pointer(%s)' % parse_type(tokens[:-1])
    if tokens[-1] == 'const':
        return 'Const(%s)' % parse_type(tokens[:-1])
    if tokens[0] == 'const':
        return 'Const(%s)' % parse_type(tokens[1:])
    assert len(tokens) == 1
    base = tokens[0]
    return type_map.get(base, base)

def get_type(t, count = 0):
    tokens = t.split()

    return parse_type(tokens, count)


class PrintGlTable(gl_XML.gl_print_base):
    def __init__(self):
        gl_XML.gl_print_base.__init__(self)

        #self.header_tag = '_GLAPI_TABLE_H_'
        self.name = "gl_trace.py (from Mesa)"
        self.license = license.bsd_license_template % ( \
"""Copyright (C) 1999-2003  Brian Paul   All Rights Reserved.
(C) Copyright IBM Corporation 2004""", "BRIAN PAUL, IBM")
        return

    def printBody(self, api):
        abi = [ "1.0", "1.1", "1.2", "1.5", "GL_ARB_multitexture" ]
        for pass_ in range(2):
            for f in api.functionIterateByOffset():
                for name in f.entry_points:
                    [category, num] = api.get_category_for_name( name )
                    args = []
                    for p in f.parameters:
                        type = get_type(p.type_string(), p.count)
                        args.append('(%s, "%s")' % (type, p.name))
                    arg_string = '[' + ', '.join(args) + ']'
                    if category in abi:
                        if pass_ == 0:
                            print '    DllFunction(%s, "gl%s", %s),' % (get_type(f.return_type), name, arg_string)
                    else:
                        if pass_ == 1:
                            print '    WglFunction(%s, "gl%s", %s),' % (get_type(f.return_type), name, arg_string)

    def printRealHeader(self):
        return

    def printRealFooter(self):
        print ']'


def show_usage():
    print "Usage: %s [-f input_file_name]" % sys.argv[0]
    sys.exit(1)

if __name__ == '__main__':
    file_name = "gl_API.xml"
    
    try:
        (args, trail) = getopt.getopt(sys.argv[1:], "f:")
    except Exception,e:
        show_usage()

    for (arg,val) in args:
        if arg == "-f":
            file_name = val

    printer = PrintGlTable()

    api = gl_XML.parse_GL_API( file_name )

    printer.Print( api )

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


from dlltrace import DllTracer
from specs.dwrite import dwrite
from specs.d2d1 import d2d1 # cyclic dependency


class DWriteTracer(DllTracer):

    def wrapArg(self, function, arg):
        if function.name == 'DWriteCreateFactory' and arg.output:
            print '    if (*%s) {' % arg.name
            for iface in dwrite.interfaces:
                print '        if (iid == IID_%s) {' % iface.name
                print '            *%s = new Wrap%s(static_cast<%s *>(*%s));' % (arg.name, iface.name, iface.name, arg.name)
                print '        }'
            print '    }'
        else:
            DllTracer.wrapArg(self, function, arg)


if __name__ == '__main__':
    print '#define INITGUID'
    print
    print '#include "trace_writer_local.hpp"'
    print '#include "os.hpp"'
    print
    print '#include <windows.h>'
    print
    print '#include "compat.h"'
    print
    print '#define DWRITE_EXPORT WINAPI'
    print
    print '#include <d2d1.h>'
    print '#include <dwrite.h>'
    print
    print 'DEFINE_GUID(IID_IDWriteFactory,0xb859ee5a,0xd838,0x4b5b,0xa2,0xe8,0x1a,0xdc,0x7d,0x93,0xdb,0x48);'
    print

    tracer = DWriteTracer('dwrite.dll')
    tracer.trace_api(dwrite)

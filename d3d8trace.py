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


from specs.d3d8 import d3d8
from dlltrace import DllTracer


class D3D8Tracer(DllTracer):

    def serializeArgValue(self, function, arg):
        # Dump shaders as strings
        if function.name in ('CreateVertexShader', 'CreatePixelShader') and arg.name == 'pFunction':
            print '    DumpShader(trace::localWriter, %s);' % (arg.name)
            return

        DllTracer.serializeArgValue(self, function, arg)


if __name__ == '__main__':
    print '#define INITGUID'
    print
    print '#include <windows.h>'
    print '#include <d3d8.h>'
    print '#include "d3dshader.hpp"'
    print
    print '#include "trace_writer_local.hpp"'
    print '#include "os.hpp"'
    print
    tracer = D3D8Tracer('d3d8.dll')
    tracer.trace_api(d3d8)


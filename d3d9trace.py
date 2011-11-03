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


from trace import DllTracer
from specs.d3d9 import d3d9


class D3D9Tracer(DllTracer):

    def dump_arg_instance(self, function, arg):
        # Dump shaders as strings
        if function.name in ('CreateVertexShader', 'CreatePixelShader') and arg.name == 'pFunction':
            print '    DumpShader(trace::localWriter, %s);' % (arg.name)
            return

        DllTracer.dump_arg_instance(self, function, arg)


if __name__ == '__main__':
    print '#include "trace_writer.hpp"'
    print '#include "os.hpp"'
    print
    print '#include "d3d9imports.hpp"'
    print '#include "d3dshader.hpp"'
    print
    print '''
static inline size_t
_declCount(const D3DVERTEXELEMENT9 *pVertexElements) {
    size_t count = 0;
    if (pVertexElements) {
        while (pVertexElements[count++].Stream != 0xff)
            ;
    }
    return count;
}
'''
    tracer = D3D9Tracer('d3d9.dll')
    tracer.trace_api(d3d9)


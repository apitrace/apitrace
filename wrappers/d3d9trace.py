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
from specs.d3d9 import d3d9


class D3D9Tracer(DllTracer):

    def serializeArgValue(self, function, arg):
        # Dump shaders as strings
        if function.name in ('CreateVertexShader', 'CreatePixelShader') and arg.name == 'pFunction':
            print '    DumpShader(trace::localWriter, %s);' % (arg.name)
            return

        DllTracer.serializeArgValue(self, function, arg)

    def declareWrapperInterfaceVariables(self, interface):
        DllTracer.declareWrapperInterfaceVariables(self, interface)
        
        if interface.name == 'IDirect3DVertexBuffer9':
            print '    UINT m_SizeToLock;'
            print '    VOID *m_pbData;'

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        if interface.name == 'IDirect3DVertexBuffer9' and method.name == 'Unlock':
            print '    if (m_pbData) {'
            self.emit_memcpy('(LPBYTE)m_pbData', '(LPBYTE)m_pbData', 'm_SizeToLock')
            print '    }'

        DllTracer.implementWrapperInterfaceMethodBody(self, interface, base, method)

        if interface.name == 'IDirect3DVertexBuffer9' and method.name == 'Lock':
            # FIXME: handle recursive locks
            print '    if (__result == D3D_OK && !(Flags & D3DLOCK_READONLY)) {'
            print '        if (SizeToLock) {'
            print '            m_SizeToLock = SizeToLock;'
            print '        } else {'
            print '            D3DVERTEXBUFFER_DESC Desc;'
            print '            m_pInstance->GetDesc(&Desc);'
            print '            m_SizeToLock = Desc.Size;'
            print '        }'
            print '        m_pbData = *ppbData;'
            print '    } else {'
            print '        m_pbData = NULL;'
            print '    }'


if __name__ == '__main__':
    print '#define INITGUID'
    print
    print '#include "trace_writer_local.hpp"'
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


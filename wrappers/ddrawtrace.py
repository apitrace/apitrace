##########################################################################
#
# Copyright 2008-2015 VMware, Inc.
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
from specs.stdapi import API
from specs.d3d import ddraw, interfaces


class DDrawTracer(DllTracer):
    # FIXME: emit fake memcpy calls for IDirectDrawSurface7::EnumAttachedSurfaces

    # FIXME: wrap objects passed to IDirectDrawSurface7::EnumAttachedSurfaces
    # callback -- we don't really care for tracing these calls, but we do want
    # to trace everything done inside the callback.
    def enumWrapperInterfaceVariables(self, interface):
        variables = DllTracer.enumWrapperInterfaceVariables(self, interface)

        # Add additional members to track locks
        if interface.getMethodByName('Lock') is not None:
            variables += [
                ('size_t', '_MappedSize', '0'),
                ('VOID *', 'm_pbData', '0'),
            ]

        return variables

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        if method.name == 'Unlock':
            print('    if (_MappedSize && m_pbData) {')
            self.emit_memcpy('(LPBYTE)m_pbData', '_MappedSize')
            print('    }')

        DllTracer.implementWrapperInterfaceMethodBody(self, interface, base, method)

        if method.name == 'Lock':
            # FIXME: handle recursive locks, directdraw allows simultaneous surface locks with different rects.
            if interface.name.startswith("IDirectDrawSurface"):
                print('    if (SUCCEEDED(_result) && !(dwFlags & DDLOCK_READONLY)) {')
            else:
                print('    if (SUCCEEDED(_result)) {')
            if interface.name.startswith("IDirectDrawSurface"):
                print('        _getMapInfo(_this, %s, m_pbData, _MappedSize);' % ', '.join(method.argNames()[:-2]))
            elif interface.name.startswith("IDirect3DVertexBuffer"):
                print('        _getMapInfo(_this, %s, m_pbData, _MappedSize);' % ', '.join(method.argNames()[1:-1]))
            else:
                print('        _getMapInfo(_this, %s, m_pbData, _MappedSize);' % ', '.join(method.argNames()))
            print('    } else {')
            print('        m_pbData = nullptr;')
            print('        _MappedSize = 0;')
            print('    }')

if __name__ == '__main__':
    print('#define INITGUID')
    print('#include "d3dimports.hpp"')
    print('#include "trace_writer_local.hpp"')
    print('#include "d3d7size.hpp"')
    print('#include "os.hpp"')
    print()

    api = API()
    api.addModule(ddraw)
    tracer = DDrawTracer()
    tracer.traceApi(api)

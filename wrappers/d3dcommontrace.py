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
from specs import stdapi


class D3DCommonTracer(DllTracer):

    def serializeArgValue(self, function, arg):
        # Dump shaders as strings
        if isinstance(arg.type, stdapi.Blob) and arg.name.startswith('pShaderBytecode'):
            print '    DumpShader(trace::localWriter, %s, %s);' % (arg.name, arg.type.size)
            return

        DllTracer.serializeArgValue(self, function, arg)
    
    def enumWrapperInterfaceVariables(self, interface):
        variables = DllTracer.enumWrapperInterfaceVariables(self, interface)
        
        # Add additional members to track maps
        if interface.getMethodByName('Map') is not None:
            variables += [
                ('VOID *', '_pMappedData', '0'),
                ('size_t', '_MappedSize', '0'),
            ]

        return variables

    def implementWrapperInterfaceMethodBody(self, interface, base, method):
        if method.name == 'Unmap':
            print '    if (_MappedSize && _pMappedData) {'
            self.emit_memcpy('_pMappedData', '_pMappedData', '_MappedSize')
            print '    }'

        DllTracer.implementWrapperInterfaceMethodBody(self, interface, base, method)

        if method.name == 'Map':
            # NOTE: recursive locks are explicitely forbidden
            print '    if (SUCCEEDED(_result)) {'
            print '        _getMapInfo(_this, %s, _pMappedData, _MappedSize);' % ', '.join(method.argNames())
            print '    } else {'
            print '        _pMappedData = NULL;'
            print '        _MappedSize = 0;'
            print '    }'



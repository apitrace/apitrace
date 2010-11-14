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


from stdapi import Type


class D3DShader(Type):

    def __init__(self, version):
        self.version = version
        Type.__init__(self, "const DWORD *")

    def decl(self):
        print '#include <d3dx9.h>'
        print '#include <stdio.h>'
        print 
        print 'void Dump%s(const DWORD *tokens);' % (self.id)
    
    def impl(self):
        print '''
typedef HRESULT 
(WINAPI *PD3DXDISASSEMBLESHADER)(
    CONST DWORD *pShader,
    BOOL EnableColorCode,
    LPCSTR pComments,
    LPD3DXBUFFER *ppDisassembly
);
        '''
        print 'void Dump%s(const DWORD *tokens)' % (self.id)
        print '''{
    static BOOL firsttime = TRUE;
    static HMODULE hD3DXModule = NULL; 
    static PD3DXDISASSEMBLESHADER pfnD3DXDisassembleShader = NULL;

    if(firsttime) {
        if(!hD3DXModule) {
            unsigned release;
            int version;
            for(release = 0; release <= 1; ++release) {
                /* Version 41 corresponds to Mar 2009 version of DirectX Runtime / SDK */ 
                for(version = 41; version >= 0; --version) {
                    char filename[256];
                    _snprintf(filename, sizeof(filename), 
                              "d3dx9%s%s%u.dll", release ? "" : "d", version ? "_" : "", version);
                    hD3DXModule = LoadLibraryA(filename);
                    if(hD3DXModule)
                        goto found;
                }
            }
found:
            ;
        }

        if (hD3DXModule)
            if(!pfnD3DXDisassembleShader)
                pfnD3DXDisassembleShader = (PD3DXDISASSEMBLESHADER)GetProcAddress(hD3DXModule, "D3DXDisassembleShader");

        firsttime = FALSE;
    }
   
    if(pfnD3DXDisassembleShader) {
        LPD3DXBUFFER pDisassembly = NULL;
   
        if (pfnD3DXDisassembleShader( (DWORD *)tokens, FALSE, NULL, &pDisassembly) == D3D_OK)
            Trace::LiteralString((char *)pDisassembly->GetBufferPointer());

        if(pDisassembly)
            pDisassembly->Release();
    }
}
'''
    
    def dump(self, instance):
        print '    Dump%s(%s);' % (self.id, instance)

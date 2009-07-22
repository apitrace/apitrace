#############################################################################
#
# Copyright 2009 VMware, Inc.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################


from base import Type


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
            unsigned version;
            for(release = 0; release <= 1; ++release) {
                /* Version 41 corresponds to Mar 2009 version of DirectX Runtime / SDK */ 
                for(version = 41; version >= 24; --version) {
                    char filename[256];
                    _snprintf(filename, sizeof(filename), 
                              "d3dx9%s_%u.dll", release ? "" : "d", version);
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
            Log::DumpString((char *)pDisassembly->GetBufferPointer());

        if(pDisassembly)
            pDisassembly->Release();
    }
}
'''
    
    def dump(self, instance):
        print '    Dump%s(%s);' % (self.id, instance)

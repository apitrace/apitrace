#############################################################################
#
# Copyright 2008 Tungsten Graphics, Inc.
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

"""windows.h"""

from base import *

SHORT = Intrinsic("SHORT", "%i")
USHORT = Intrinsic("USHORT", "%u")
INT = Intrinsic("INT", "%i")
UINT = Intrinsic("UINT", "%u")
LONG = Intrinsic("LONG", "%li")
ULONG = Intrinsic("ULONG", "%lu")
FLOAT = Intrinsic("FLOAT", "%f")

INT32 = Intrinsic("INT32", "%i")
UINT32 = Intrinsic("UINT32", "%i")

BYTE = Intrinsic("BYTE", "0x%02lx")
WORD = Intrinsic("WORD", "0x%04lx")
DWORD = Intrinsic("DWORD", "0x%08lx")

BOOL = Intrinsic("BOOL", "%i")

LPLONG = Pointer(LONG)
LPWORD = Pointer(WORD)
LPDWORD = Pointer(DWORD)
LPBOOL = Pointer(BOOL)
LPSIZE = LPDWORD

LPSTR = String
LPCSTR = Const(String)
LPWSTR = String

LARGE_INTEGER = Intrinsic("LARGE_INTEGER", "0x%llx")

HRESULT = Alias("HRESULT", Int)

PVOID = Intrinsic("PVOID", "%p")
LPVOID = PVOID
HANDLE = Intrinsic("HANDLE", "%p")
HWND = Intrinsic("HWND", "%p")
HDC = Intrinsic("HDC", "%p")
HMONITOR = Intrinsic("HMONITOR", "%p")

GUID = Struct("GUID", [
    (DWORD, "Data1"),
    (WORD, "Data2"),
    (WORD, "Data3"),
    (BYTE, "Data4[0]"),
    (BYTE, "Data4[1]"),
    (BYTE, "Data4[2]"),
    (BYTE, "Data4[3]"),
    (BYTE, "Data4[4]"),
    (BYTE, "Data4[5]"),
    (BYTE, "Data4[6]"),
    (BYTE, "Data4[7]"),
])
LPGUID = Pointer(GUID)

#REFGUID = Alias("REFGUID", Pointer(GUID))
REFGUID = Alias("REFGUID", GUID)

IID = Alias("IID", GUID)
#REFIID = Alias("REFIID", Pointer(IID))
REFIID = Alias("REFIID", IID)

CLSID = Alias("CLSID", GUID)
#REFCLSID = Alias("REFCLSID", Pointer(CLSID))
REFCLSID = Alias("REFCLSID", CLSID)

LUID = Struct("LUID", [
    (DWORD, "LowPart"),
    (LONG, "HighPart"),
])

POINT = Struct("POINT", (
  (LONG, "x"),
  (LONG, "y"),
)) 
LPPOINT = Pointer(POINT)

RECT = Struct("RECT", (
  (LONG, "left"),
  (LONG, "top"),
  (LONG, "right"), 
  (LONG, "bottom"), 
)) 
LPRECT = Pointer(RECT)

PALETTEENTRY = Struct("PALETTEENTRY", (
  (BYTE, "peRed"),
  (BYTE, "peGreen"),
  (BYTE, "peBlue"), 
  (BYTE, "peFlags"), 
)) 
LPPALETTEENTRY = Pointer(PALETTEENTRY)


RGNDATAHEADER = Struct("RGNDATAHEADER", [
    (DWORD, "dwSize"),
    (DWORD, "iType"),
    (DWORD, "nCount"),
    (DWORD, "nRgnSize"),
    (RECT, "rcBound"),
])

RGNDATA = Struct("RGNDATA", [
    (RGNDATAHEADER, "rdh"),
    #(Char, "Buffer[1]"),
])
LPRGNDATA = Pointer(RGNDATA)


IUnknown = Interface("IUnknown")

IUnknown.methods = (
	Method(HRESULT, "QueryInterface", ((REFIID, "riid"), (Pointer(Pointer(Void)), "ppvObj"))),
	Method(ULONG, "AddRef", ()),
	Method(ULONG, "Release", ()),
)


class DllFunction(Function):

    def get_true_pointer(self):
        ptype = self.pointer_type()
        pvalue = self.pointer_value()
        print '    if(!g_hDll) {'
        print '        g_hDll = LoadLibrary(g_szDll);'
        print '        if(!g_hDll)'
        self.fail_impl()
        print '    }'
        print '    if(!%s) {' % (pvalue,)
        print '        %s = (%s)GetProcAddress( g_hDll, "%s");' % (pvalue, ptype, self.name)
        print '        if(!%s)' % (pvalue,)
        self.fail_impl()
        print '    }'


class Dll:

    def __init__(self, name):
        self.name = name
        self.functions = []
        if self not in towrap:
            towrap.append(self)

    def wrap_name(self):
        return "Wrap" + self.name

    def wrap_pre_decl(self):
        pass

    def wrap_decl(self):
        print 'static HINSTANCE g_hDll = NULL;'
        print 'static TCHAR g_szDll[MAX_PATH] = {0};'
        print
        print 'BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);'
        print
        for function in self.functions:
            function.wrap_decl()
        print

    def wrap_impl(self):
        print r'BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {'
        print r'    switch(fdwReason) {'
        print r'    case DLL_PROCESS_ATTACH:'
        print r'        if(!GetSystemDirectory(g_szDll, MAX_PATH))'
        print r'            return FALSE;'
        print r'        _tcscat(g_szDll, TEXT("\\%s.dll"));' % self.name
        print r'        Log::Open("%s");' % self.name
        print r'    case DLL_THREAD_ATTACH:'
        print r'        return TRUE;'
        print r'    case DLL_THREAD_DETACH:'
        print r'        return TRUE;'
        print r'    case DLL_PROCESS_DETACH:'
        print r'        Log::Close();'
        print r'        if(g_hDll) {'
        print r'            FreeLibrary(g_hDll);'
        print r'            g_hDll = NULL;'
        print r'        }'
        print r'        return TRUE;'
        print r'    }'
        print r'    (void)hinstDLL;'
        print r'    (void)lpvReserved;'
        print r'    return TRUE;'
        print r'}'
        print
        for function in self.functions:
            function.wrap_impl()
        print


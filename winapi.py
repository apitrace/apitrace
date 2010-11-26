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

"""Win32 API type description."""

from stdapi import *

SHORT = Alias("SHORT", Short)
USHORT = Alias("USHORT", UShort)
INT = Alias("INT", Int)
UINT = Alias("UINT", UInt)
LONG = Alias("LONG", Long)
ULONG = Alias("ULONG", ULong)
LONGLONG = Alias("LONGLONG", LongLong)
FLOAT = Alias("FLOAT", Float)

INT32 = Literal("INT32", "UInt")
UINT32 = Literal("UINT32", "UInt")

BYTE = Literal("BYTE", "UInt", base=16)
WORD = Literal("WORD", "UInt", base=16)
DWORD = Literal("DWORD", "UInt", base=16)

BOOL = Alias("BOOL", Bool)

LPLONG = Pointer(LONG)
LPWORD = Pointer(WORD)
LPDWORD = Pointer(DWORD)
LPBOOL = Pointer(BOOL)
LPSIZE = LPDWORD

LPSTR = CString
LPCSTR = Const(CString)
LPWSTR = WString
LPCWSTR = Const(WString)

LARGE_INTEGER = Struct("LARGE_INTEGER", [
    (LONGLONG, 'QuadPart'),
])

SIZE_T = Alias("SIZE_T", SizeT)

HRESULT = Alias("HRESULT", Int)

VOID = Void
PVOID = Opaque("PVOID")
LPVOID = PVOID
HANDLE = Opaque("HANDLE")
HWND = Opaque("HWND")
HDC = Opaque("HDC")
HMONITOR = Opaque("HMONITOR")

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

HMODULE = Opaque("HMODULE")

IUnknown = Interface("IUnknown")

IUnknown.methods = (
	Method(HRESULT, "QueryInterface", ((REFIID, "riid"), (Pointer(OpaquePointer(Void)), "ppvObj"))),
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
        print '        %s = (%s)GetProcAddress(g_hDll, "%s");' % (pvalue, ptype, self.name)
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
        print r'        Trace::Open("%s");' % self.name
        print r'    case DLL_THREAD_ATTACH:'
        print r'        return TRUE;'
        print r'    case DLL_THREAD_DETACH:'
        print r'        return TRUE;'
        print r'    case DLL_PROCESS_DETACH:'
        print r'        Trace::Close();'
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


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

INT8 = Alias("INT8", Int8)
UINT8 = Alias("UINT8", UInt8)
INT16 = Alias("INT16", Int16)
UINT16 = Alias("UINT16", UInt16)
INT32 = Alias("INT32", Int32)
UINT32 = Alias("UINT32", UInt32)
INT64 = Alias("INT64", Int64)
UINT64 = Alias("UINT64", UInt64)

BYTE = Alias("BYTE", UInt8)
WORD = Alias("WORD", UInt16)
DWORD = Alias("DWORD", UInt32)

UCHAR = Alias("UCHAR", UChar)
WCHAR = Alias("WCHAR", Short)

BOOL = Enum("BOOL", [
    "FALSE",
    "TRUE",
])

LPLONG = Pointer(LONG)
LPWORD = Pointer(WORD)
LPDWORD = Pointer(DWORD)
LPBOOL = Pointer(BOOL)

LPSTR = CString
LPCSTR = ConstCString
LPWSTR = WString
LPCWSTR = ConstWString

LARGE_INTEGER = Struct("LARGE_INTEGER", [
    (LONGLONG, 'QuadPart'),
])

SIZE_T = Alias("SIZE_T", SizeT)

VOID = Void
PVOID = OpaquePointer(VOID)
LPVOID = PVOID
LPCVOID = OpaquePointer(Const(VOID))

def DECLARE_HANDLE(expr):
    return Handle(expr, IntPointer(expr))

HANDLE = DECLARE_HANDLE("HANDLE")
HWND = DECLARE_HANDLE("HWND")
HDC = DECLARE_HANDLE("HDC")
HMONITOR = DECLARE_HANDLE("HMONITOR")

GUID = Struct("GUID", [
    (DWORD, "Data1"),
    (WORD, "Data2"),
    (WORD, "Data3"),
    (Array(BYTE, 8), "Data4"),
])
LPGUID = Pointer(GUID)

REFGUID = Alias("REFGUID", Reference(GUID))

IID = Alias("IID", GUID)
REFIID = Alias("REFIID", Reference(IID))

CLSID = Alias("CLSID", GUID)
REFCLSID = Alias("REFCLSID", Reference(CLSID))

LUID = Struct("LUID", [
    (DWORD, "LowPart"),
    (LONG, "HighPart"),
])

POINT = Struct("POINT", (
  (LONG, "x"),
  (LONG, "y"),
)) 
LPPOINT = Pointer(POINT)

SIZE = Struct("SIZE", (
  (LONG, "cx"),
  (LONG, "cy"),
)) 
LPSIZE = Pointer(SIZE)

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

HMODULE = IntPointer("HMODULE")

FILETIME = Struct("FILETIME", [
    (DWORD, "dwLowDateTime"),
    (DWORD, "dwHighDateTime"),
])

COLORREF = Alias("COLORREF", DWORD)

LOGFONTW = Struct("LOGFONTW", [
    (LONG, "lfHeight"),
    (LONG, "lfWidth"),
    (LONG, "lfEscapement"),
    (LONG, "lfOrientation"),
    (LONG, "lfWeight"),
    (BYTE, "lfItalic"),
    (BYTE, "lfUnderline"),
    (BYTE, "lfStrikeOut"),
    (BYTE, "lfCharSet"),
    (BYTE, "lfOutPrecision"),
    (BYTE, "lfClipPrecision"),
    (BYTE, "lfQuality"),
    (BYTE, "lfPitchAndFamily"),
    (WString, "lfFaceName"),
])

SECURITY_ATTRIBUTES = Struct("SECURITY_ATTRIBUTES", [
    (DWORD, "nLength"),
    (LPVOID, "lpSecurityDescriptor"),
    (BOOL, "bInheritHandle"),
])

# http://msdn.microsoft.com/en-us/library/ff485842.aspx
# http://msdn.microsoft.com/en-us/library/windows/desktop/ms681381.aspx
def MAKE_HRESULT(errors, ok = "S_OK", false = "S_FALSE"):
    values = [ok, false]
    values.extend(errors)
    values.extend([
        "E_PENDING", # 0x8000000A
        "E_NOTIMPL", # 0x80004001
        "E_NOINTERFACE", # 0x80004002
        "E_POINTER", # 0x80004003
        "E_ABORT", # 0x80004004
        "E_FAIL", # 0x80004005
        "E_UNEXPECTED", # 0x8000FFFF
        "E_ACCESSDENIED", # 0x80070005
        "E_HANDLE", # 0x80070006
        "E_OUTOFMEMORY", # 0x8007000E
        "E_INVALIDARG", # 0x80070057
    ])
    return Enum("HRESULT", values)

HRESULT = MAKE_HRESULT([])


IUnknown = Interface("IUnknown")

IUnknown.methods = (
	StdMethod(HRESULT, "QueryInterface", ((REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppvObj"))),
	StdMethod(ULONG, "AddRef", ()),
	StdMethod(ULONG, "Release", ()),
)



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

WCHAR = Alias("WCHAR", Short)

BOOL = Alias("BOOL", Bool)

LPLONG = Pointer(LONG)
LPWORD = Pointer(WORD)
LPDWORD = Pointer(DWORD)
LPBOOL = Pointer(BOOL)

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
    (Array(BYTE, 8), "Data4"),
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

HMODULE = Opaque("HMODULE")

IUnknown = Interface("IUnknown")

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

HRESULT_com = FakeEnum(HRESULT, [
    "S_OK",
    "E_NOINTERFACE",
    "E_POINTER",
])

IUnknown.methods = (
	Method(HRESULT_com, "QueryInterface", ((REFIID, "riid"), Out(Pointer(OpaquePointer(Void)), "ppvObj"))),
	Method(ULONG, "AddRef", ()),
	Method(ULONG, "Release", ()),
)



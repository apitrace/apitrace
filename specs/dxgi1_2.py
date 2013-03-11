##########################################################################
#
# Copyright 2013 Jeff Muizelaar
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


from dxgi import *


IDXGIResource1 = Interface("IDXGIResource1", IDXGIResource)
IDXGISurface2 = Interface("IDXGISurface2", IDXGISurface1)


DXGI_SHARED_RESOURCE_FLAG = Flags(DWORD, [
    "DXGI_SHARED_RESOURCE_READ",
    "DXGI_SHARED_RESOURCE_WRITE",
])


IDXGIResource1.methods += [
    StdMethod(HRESULT, "CreateSharedHandle", [(Pointer(Const(SECURITY_ATTRIBUTES)), "pAttributes"), (DXGI_SHARED_RESOURCE_FLAG, "dwAccess"), (LPCWSTR, "lpName"), Out(Pointer(HANDLE), "pHandle")]),
    StdMethod(HRESULT, "CreateSubresourceSurface", [(UINT, "index"), Out(Pointer(ObjPointer(IDXGISurface2)), "ppSurface")]),
]


IDXGISurface2.methods += [
    StdMethod(HRESULT, "GetResource", [(REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppParentResource"), Out(Pointer(UINT), "pSubresourceIndex")]),
]

dxgi.addInterfaces([
    IDXGIResource1,
])

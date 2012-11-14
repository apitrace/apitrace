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
from specs.stdapi import API
from specs.d3d import ddraw, interfaces


if __name__ == '__main__':
    print '#define INITGUID'
    print '#include <windows.h>'
    print '#include <ddraw.h>'
    print '#include <d3d.h>'
    print
    print '''

#ifndef DDBLT_EXTENDED_FLAGS
#define DDBLT_EXTENDED_FLAGS 0x40000000l
#endif

#ifndef DDBLT_EXTENDED_LINEAR_CONTENT
#define DDBLT_EXTENDED_LINEAR_CONTENT 0x00000004l
#endif

#ifndef D3DLIGHT_PARALLELPOINT
#define D3DLIGHT_PARALLELPOINT (D3DLIGHTTYPE)4
#endif

#ifndef D3DLIGHT_GLSPOT
#define D3DLIGHT_GLSPOT (D3DLIGHTTYPE)5
#endif

'''
    print '#include "trace_writer_local.hpp"'
    print '#include "os.hpp"'
    print

    api = API()
    api.addModule(ddraw)
    tracer = DllTracer()
    tracer.traceApi(api)

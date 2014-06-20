##########################################################################
#
# Copyright 2014 VMware, Inc.
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


from stdapi import API, stdapi

from glapi import glapi
from glxapi import glxapi
from wglapi import wglapi
from cglapi import cglapi

from d3d9 import d3d9
from dxgi import dxgi
from d3d10 import d3d10
from d3d10_1 import d3d10_1
from d3d11 import d3d11


superapi = API()
superapi.addModule(stdapi)
superapi.addModule(glapi)
superapi.addModule(glxapi)
superapi.addModule(wglapi)
superapi.addModule(cglapi)
superapi.addModule(d3d9)
superapi.addModule(dxgi)
superapi.addModule(d3d10)
superapi.addModule(d3d10_1)
superapi.addModule(d3d11)

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

"""CGL API description."""


from stdapi import *
from glapi import glapi


CGLError = Enum("CGLError", [
    "kCGLNoError",	# 0	
    "kCGLBadAttribute",	# 10000	
    "kCGLBadProperty",	# 10001	
    "kCGLBadPixelFormat",	# 10002	
    "kCGLBadRendererInfo",	# 10003	
    "kCGLBadContext",	# 10004	
    "kCGLBadDrawable",	# 10005	
    "kCGLBadDisplay",	# 10006	
    "kCGLBadState",	# 10007	
    "kCGLBadValue",	# 10008	
    "kCGLBadMatch",	# 10009	
    "kCGLBadEnumeration",	# 10010	
    "kCGLBadOffScreen",	# 10011	
    "kCGLBadFullScreen",	# 10012	
    "kCGLBadWindow",	# 10013	
    "kCGLBadAddress",	# 10014	
    "kCGLBadCodeModule",	# 10015	
    "kCGLBadAlloc",	# 10016	
    "kCGLBadConnection",	# 10017	
])

CGLContextObj = Opaque("CGLContextObj")

cglapi = API("CGL")

cglapi.add_functions([
    Function(CGLError, "CGLSetCurrentContext", [(CGLContextObj, "ctx")]),
    Function(CGLContextObj, "CGLGetCurrentContext", []),
])


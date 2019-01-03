##########################################################################
#
# Copyright 2008-2010 VMware, Inc.
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

"""Trace code generation for Windows DLLs."""


import ntpath

from trace import Tracer
from dispatch import Dispatcher
from specs.stdapi import API


class DllDispatcher(Dispatcher):

    def dispatchModule(self, module):
        print(r'Module g_mod%s("%s");' % (module.name.upper(), module.name))

        Dispatcher.dispatchModule(self, module)

    def getProcAddressName(self, module, function):
        return r'g_mod%s.getProcAddress' % (module.name.upper(),)


class DllTracer(Tracer):

    def header(self, api):
        print(r'#include "dlltrace.hpp"')
        print()

        for module in api.modules:
            dispatcher = DllDispatcher()
            dispatcher.dispatchModule(module)

        Tracer.header(self, api)

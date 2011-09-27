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

'''Debugging utilities.'''


import sys
import traceback
import inspect


def excepthook(type, value, tb):
	"""
	Automatically start the debugger on an exception.

	See also:
	- http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/65287
	"""

	if hasattr(sys, 'ps1') \
	or not (sys.stdin.isatty() and sys.stdout.isatty() and sys.stderr.isatty()) \
	or type == SyntaxError or type == KeyboardInterrupt:
		# we are in interactive mode or we don't have a tty-like
		# device, so we call the default hook
		oldexcepthook(type, value, tb)
	else:
		import traceback, pdb
		# we are NOT in interactive mode, print the exception...
		traceback.print_exception(type, value, tb)
		print
		# ...then start the debugger in post-mortem mode.
		pdb.pm()

oldexcepthook, sys.excepthook = sys.excepthook, excepthook


def dump(var):
	sys.stderr.write(repr(var) + '\n')
	

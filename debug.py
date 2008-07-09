#############################################################################
#
# Copyright 2007 Jose Fonseca
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
	

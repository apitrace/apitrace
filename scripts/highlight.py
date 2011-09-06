##########################################################################
#
# Copyright 2011 Jose Fonseca
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


import sys
import platform


class PlainHighlighter:
    '''Plain formatter'''

    black = None
    red = None
    green = None
    yellow = None
    blue = None
    magenta = None
    cyan = None
    white = None

    def __init__(self, stream):
        self.stream = stream

    def write(self, text):
        self.stream.write(text)

    def flush(self):
        self.stream.flush()

    def normal(self):
        pass

    def color(self, color):
        pass

    def bold(self):
        pass

    def italic(self):
        pass


class AnsiHighlighter(PlainHighlighter):
    '''Highlighter for plain-text files which outputs ANSI escape codes. See
    http://en.wikipedia.org/wiki/ANSI_escape_code for more information
    concerning ANSI escape codes.
    '''

    _csi = '\33['

    _normal = '0m'
    _bold = '1m'
    _italic = '3m'

    black = 0
    red = 1
    green = 2
    yellow = 3
    blue = 4
    magenta = 5
    cyan = 6
    white = 7

    def __init__(self, stream):
        PlainHighlighter.__init__(self, stream)
        self.isatty = stream.isatty()

    def _escape(self, code):
        if self.isatty:
            self.stream.write(self._csi + code)

    def normal(self):
        self._escape(self._normal)

    def color(self, color):
        self._escape(str(30 + color) + 'm')

    def bold(self):
        self._escape(self._bold)

    def italic(self):
        self._escape(self._italic)


class WindowsConsoleHighlighter(PlainHighlighter):
    '''Highlighter for the Windows Console. See 
    http://code.activestate.com/recipes/496901/ for more information.
    '''

    INVALID_HANDLE_VALUE = -1
    STD_INPUT_HANDLE  = -10
    STD_OUTPUT_HANDLE = -11
    STD_ERROR_HANDLE  = -12

    FOREGROUND_BLUE      = 0x01
    FOREGROUND_GREEN     = 0x02
    FOREGROUND_RED       = 0x04
    FOREGROUND_INTENSITY = 0x08
    BACKGROUND_BLUE      = 0x10
    BACKGROUND_GREEN     = 0x20
    BACKGROUND_RED       = 0x40
    BACKGROUND_INTENSITY = 0x80

    COMMON_LVB_LEADING_BYTE = 0x0100
    COMMON_LVB_TRAILING_BYTE = 0x0200
    COMMON_LVB_GRID_HORIZONTAL = 0x0400
    COMMON_LVB_GRID_LVERTICAL = 0x0800
    COMMON_LVB_GRID_RVERTICAL = 0x1000
    COMMON_LVB_REVERSE_VIDEO = 0x4000
    COMMON_LVB_UNDERSCORE = 0x8000

    _normal = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
    _bold   = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
    _italic = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED

    black   = 0
    red     =                                      FOREGROUND_RED
    green   =                   FOREGROUND_GREEN                 
    blue    = FOREGROUND_BLUE                                    
    white   = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED

    def __init__(self, stream):
        PlainHighlighter.__init__(self, stream)

        if stream is sys.stdin:
            nStdHandle = self.STD_INPUT_HANDLE
        elif stream is sys.stdout:
            nStdHandle = self.STD_OUTPUT_HANDLE
        elif stream is sys.stderr:
            nStdHandle = self.STD_ERROR_HANDLE
        else:
            nStdHandle = None

        if nStdHandle is not None:
            import ctypes
            self._handle = ctypes.windll.kernel32.GetStdHandle(nStdHandle)
        else:
            self._handle = INVALID_HANDLE_VALUE

        self._attribute = self.white

    def _setAttribute(self, attr):
        if self._handle != INVALID_HANDLE_VALUE:
            import ctypes
            ctypes.windll.kernel32.SetConsoleTextAttribute(self._handle, attr)
        self._attribute = attr

    def normal(self):
        self._setAttribute(self._normal)

    def color(self, color):
        intensity = self._attribute & FOREGROUND_INTENSITY
        self._setAttribute(color | intensity)

    def bold(self):
        self._setAttribute(self._attribute | FOREGROUND_INTENSITY)

    def italic(self):
        pass


def Highlighter(stream = sys.stdout):
    if platform.system() == 'Windows':
        return WindowsConsoleHighlighter(stream)
    else:
        return AnsiHighlighter(stream)


__all__ = [
    'Highlighter',
]

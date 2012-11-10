##########################################################################
#
# Copyright 2011 Jose Fonseca
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


from winapi import *


HRESULT = Enum("HRESULT", [
    "D2DERR_UNSUPPORTED_PIXEL_FORMAT",
    "D2DERR_INSUFFICIENT_BUFFER",
    "D2DERR_WRONG_STATE",
    "D2DERR_NOT_INITIALIZED",
    "D2DERR_UNSUPPORTED_OPERATION",
    "D2DERR_SCANNER_FAILED",
    "D2DERR_SCREEN_ACCESS_DENIED",
    "D2DERR_DISPLAY_STATE_INVALID",
    "D2DERR_ZERO_VECTOR",
    "D2DERR_INTERNAL_ERROR",
    "D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED",
    "D2DERR_INVALID_CALL",
    "D2DERR_NO_HARDWARE_DEVICE",
    "D2DERR_RECREATE_TARGET",
    "D2DERR_TOO_MANY_SHADER_ELEMENTS",
    "D2DERR_SHADER_COMPILE_FAILED",
    "D2DERR_MAX_TEXTURE_SIZE_EXCEEDED",
    "D2DERR_UNSUPPORTED_VERSION",
    "D2DERR_BAD_NUMBER",
    "D2DERR_WRONG_FACTORY",
    "D2DERR_LAYER_ALREADY_IN_USE",
    "D2DERR_POP_CALL_DID_NOT_MATCH_PUSH",
    "D2DERR_WRONG_RENDER_TARGET",
    "D2DERR_PUSH_POP_UNBALANCED",
    "D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT",
    "D2DERR_INCOMPATIBLE_BRUSH_TYPES",
    "D2DERR_WIN32_ERROR",
    "D2DERR_TARGET_NOT_GDI_COMPATIBLE",
    "D2DERR_TEXT_EFFECT_IS_WRONG_TYPE",
    "D2DERR_TEXT_RENDERER_NOT_RELEASED",
    "D2DERR_EXCEEDS_MAX_BITMAP_SIZE",
])

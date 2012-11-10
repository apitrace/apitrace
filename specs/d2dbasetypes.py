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


D3DCOLORVALUE = Struct("D3DCOLORVALUE", [
    (FLOAT, "r"),
    (FLOAT, "g"),
    (FLOAT, "b"),
    (FLOAT, "a"),
])

D2D_POINT_2U = Struct("D2D_POINT_2U", [
    (UINT32, "x"),
    (UINT32, "y"),
])

D2D_POINT_2F = Struct("D2D_POINT_2F", [
    (FLOAT, "x"),
    (FLOAT, "y"),
])

D2D_RECT_F = Struct("D2D_RECT_F", [
    (FLOAT, "left"),
    (FLOAT, "top"),
    (FLOAT, "right"),
    (FLOAT, "bottom"),
])

D2D_RECT_U = Struct("D2D_RECT_U", [
    (UINT32, "left"),
    (UINT32, "top"),
    (UINT32, "right"),
    (UINT32, "bottom"),
])

D2D_SIZE_F = Struct("D2D_SIZE_F", [
    (FLOAT, "width"),
    (FLOAT, "height"),
])

D2D_SIZE_U = Struct("D2D_SIZE_U", [
    (UINT32, "width"),
    (UINT32, "height"),
])

D2D_COLOR_F = Alias("D2D_COLOR_F", D3DCOLORVALUE)
D2D_MATRIX_3X2_F = Struct("D2D_MATRIX_3X2_F", [
    (FLOAT, "_11"),
    (FLOAT, "_12"),
    (FLOAT, "_21"),
    (FLOAT, "_22"),
    (FLOAT, "_31"),
    (FLOAT, "_32"),
])


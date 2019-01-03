#!/usr/bin/env python
##########################################################################
#
# Copyright 2014 VMware, Inc
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


#
# Script to half-generate glparams.py based on Khronos' gl.xml
#


import sys
import xml.etree.ElementTree as ET


def canonical(x, y):
    assert x != y

    # Prefer the shortest
    if x.startswith(y):
        return y
    if y.startswith(x):
        return x

    # then _KHR
    if x.endswith('_KHR'):
        return x
    if y.endswith('_KHR'):
        return y

    # then _ARB
    if x.endswith('_ARB'):
        return x
    if y.endswith('_ARB'):
        return y

    # then _EXT
    if x.endswith('_EXT'):
        return x
    if y.endswith('_EXT'):
        return y

    # Return the first
    return x


for arg in sys.argv[1:]:
    tree = ET.parse(arg)
    root = tree.getroot()


    params = {}
    for enums in root.findall('enums'):
        if enums.attrib.get('type') == 'bitmask':
            continue

        for enum in enums.findall('enum'):
            name = enum.attrib['name']
            value = enum.attrib['value']

            if value.isdigit():
                value = int(value)
            elif value.startswith('0x'):
                value = int(value, 16)
            else:
                continue

            try:
                origName = params[value]
            except KeyError:
                params[value] = name
            else:
                params[value] = canonical(origName, name)


    values = list(params.keys())
    values.sort()
    for value in values:
        name = params[value]
        print('    ("",\tX,\t1,\t"%s"),\t# 0x%04X' % (name, value))

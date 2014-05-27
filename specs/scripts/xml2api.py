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
# Script to half-generate *api.py based on Khronos' *.xml
#


import sys
import xml.etree.ElementTree as ET

import c2api



def getType(node):
    ptype = node.find('ptype')
    if ptype is None:
        typeText = node.text.strip()
    else:
        typeText = ptype.text

    parser = c2api.DeclParser()
    parser.tokenize(typeText + ';')

    return parser.parse_type()


for arg in sys.argv[1:]:
    tree = ET.parse(arg)
    root = tree.getroot()

    prototypes = {}

    for commands in root.findall('commands'):
        namespace = commands.get('namespace')
        for command in commands.findall('command'):
            proto = command.find('proto')
            retType = getType(proto)
            functionName = proto.find('name').text

            args = []
            for param in command.findall('param'):
                argType = getType(param)
                argName = param.find('name').text
                if argName.lower() == 'hdc':
                    argName = 'hDC'
                arg = '(%s, "%s")' % (argType, argName)
                args.append(arg)

            if namespace == 'WGL':
                constructor = 'StdFunction'
            else:
                constructor = 'GlFunction'

            prototype = '%s(%s, "%s", [%s])' % (constructor, retType, functionName, ', '.join(args))
            prototypes[functionName] = prototype

    for feature in root.findall('feature'):
        print '    # %s' % feature.get('name')
        require = feature.find('require')
        for command in require.findall('command'):
            functionName = command.get('name')
            prototype = prototypes[functionName]
            print '    %s,' % prototype
        print

    extensions = root.find('extensions')
    for extension in extensions.findall('extension'):
        print '    # %s' % extension.get('name')
        require = extension.find('require')
        for command in require.findall('command'):
            functionName = command.get('name')
            prototype = prototypes[functionName]
            print '    %s,' % prototype
        print


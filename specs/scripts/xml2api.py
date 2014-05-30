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


def appendToken(tokens, text):
    for token in text.split():
        if token.startswith('*'):
            for c in token:
                tokens.append(c)
        else:
            tokens.append(token)


def getType(node):
    tokens = []

    if node.text is not None:
        appendToken(tokens, node.text)

    ptype = node.find('ptype')
    if ptype is not None:
        appendToken(tokens, ptype.text)
        appendToken(tokens, ptype.tail)

    # Array
    lenExpr = node.get('len')
    if lenExpr is not None:
        assert tokens[-1] == '*'
        tokens = tokens[:-1]
        if lenExpr == "COMPSIZE(pname)":
            lenExpr = "_gl_param_size(pname)"
    
    typeText = ' '.join(tokens)
    parser = c2api.DeclParser()
    parser.tokenize(typeText + ';')
    typeExpr = parser.parse_type()

    if lenExpr is not None:
        typeExpr = 'Array(%s, "%s")' % (typeExpr, lenExpr)

    return typeExpr


def processCommand(prototypes, command):
    proto = command.find('proto')

    functionName = proto.find('name').text

    retType = getType(proto)

    args = []
    for param in command.findall('param'):
        argName = param.find('name').text
        #print argName, param.text
        argType = getType(param)
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


def processRequire(prototypes, node):
    requireNode = node.find('require')
    if requireNode is None:
        return
    commands = requireNode.findall('command')
    if not commands:
        return
    print '    # %s' % node.get('name')
    for command in commands:
        functionName = command.get('name')
        prototype = prototypes[functionName]
        print '    %s,' % prototype
    print


for arg in sys.argv[1:]:
    tree = ET.parse(arg)
    root = tree.getroot()

    prototypes = {}

    for commands in root.findall('commands'):
        namespace = commands.get('namespace')
        for command in commands.findall('command'):
            processCommand(prototypes, command)

    for feature in root.findall('feature'):
        processRequire(prototypes, feature)

    extensions = root.find('extensions')
    for extension in extensions.findall('extension'):
        processRequire(prototypes, extension)


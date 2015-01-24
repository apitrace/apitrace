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


import optparse
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
        if lenExpr == "1":
            typeExpr = 'Pointer(%s)' % (typeExpr)
        else:
            if not lenExpr.isdigit():
                lenExpr = '"' + lenExpr + '"'
            typeExpr = 'Array(%s, %s)' % (typeExpr, lenExpr)

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


def processRequire(node, filterName):
    nodeName = node.get('name')
    if filterName is not None and nodeName != filterName:
        return

    commands = []
    for requireNode in node.findall('require'):
        commands.extend(requireNode.findall('command'))
    if not commands:
        return

    functionNames = [command.get('name') for command in commands]

    return nodeName, functionNames



def printPrototypes(prototypes, extensionName, functionNames, skip=set()):
    print '    # %s' % extensionName

    if extensionName == 'GL_EXT_direct_state_access':
        functionNames.sort()

    for functionName in functionNames:
        if functionName not in skip:
            prototype = prototypes[functionName]
            print '    %s,' % prototype

    print


def main():
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] <xml> ...',
        version='%%prog')
    optparser.add_option(
        '--filter', metavar='NAME',
        type='string', dest='filter',
        help='filter feature/extension')
    (options, args) = optparser.parse_args(sys.argv[1:])

    global prototypes
    global namespace

    for arg in args:
        tree = ET.parse(arg)
        root = tree.getroot()

        prototypes = {}

        for commands in root.findall('commands'):
            namespace = commands.get('namespace')
            for command in commands.findall('command'):
                processCommand(prototypes, command)

        # Extract features
        features = []
        for feature in root.findall('feature'):
            ret = processRequire(feature, options.filter)
            if ret is not None:
                features.append(ret)

        # Extract extensions
        extensions = []
        for extension in root.find('extensions').findall('extension'):
            ret = processRequire(extension, options.filter)
            if ret is not None:
                extensions.append(ret)

        # Eliminate the functions from features that are in extensions
        for extensionName, extensionFunctionNames in extensions:
            for featureName, featureFunctionNames in features:
                for functionName in extensionFunctionNames:
                    try:
                        featureFunctionNames.remove(functionName)
                    except ValueError:
                        pass

        # Print all
        skip = set()
        for extensionName, functionNames in features + extensions:
            printPrototypes(prototypes, extensionName, functionNames, skip)
            skip.update(functionNames)


if __name__ == '__main__':
    main()

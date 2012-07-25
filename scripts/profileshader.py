#!/usr/bin/env python
##########################################################################
#
# Copyright 2012 VMware, Inc.
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

if len(sys.argv) <= 1:
	print 'Please specify a file to read'
	sys.exit()

shaderTimes = {}
activeShader = 0

for line in open(sys.argv[1], 'r'):
    words = line.split(' ')

    if line.startswith('#'):
        continue

    if words[0:3] == ['use','shader','program']:
        activeShader = long(words[3])
    elif words[0] == 'call':
        id = long(words[1])
        func = words[-1]
        duration = long(words[3])

        if shaderTimes.has_key(activeShader):
            shaderTimes[activeShader]['draws'] += 1
            shaderTimes[activeShader]['duration'] += duration
            if duration > shaderTimes[activeShader]['longestDuration']:
                shaderTimes[activeShader]['longest'] = id
                shaderTimes[activeShader]['longestDuration'] = duration
        else:
            shaderTimes[activeShader] = {'draws': 1, 'duration': duration, 'longest': id, 'longestDuration': duration}

sortedShaderTimes = sorted(shaderTimes.items(), key=lambda x: x[1]['duration'], reverse=True)

print '+------------+--------------+--------------------+--------------+-------------+'
print '| Shader[id] |   Draws [#]  |   Duration [ns]  v | Per Call[ns] | Longest[id] |'
print '+------------+--------------+--------------------+--------------+-------------+'

for shader in sortedShaderTimes:
    id = str(shader[0]).rjust(10)
    draw = str(shader[1]['draws']).rjust(12)
    dura = str(shader[1]['duration']).rjust(18)
    perCall = str(shader[1]['duration'] / shader[1]['draws']).rjust(12)
    longest = str(shader[1]['longest']).rjust(11)
    print "| %s | %s | %s | %s | %s |" % (id, draw, dura, perCall, longest)

print '+------------+--------------+--------------------+--------------+-------------+'

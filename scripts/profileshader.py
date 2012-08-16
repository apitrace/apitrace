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


def process(stream):
    times = {}

    # call no gpu_start gpu_dura cpu_start cpu_dura pixels program name

    for line in stream:
        words = line.split(' ')

        if line.startswith('#'):
            continue

        if words[0] == 'call':
            id = long(words[1])
            duration = long(words[3])
            shader = long(words[7])
            func = words[8]

            if times.has_key(shader):
                times[shader]['draws'] += 1
                times[shader]['duration'] += duration

                if duration > times[shader]['longestDuration']:
                    times[shader]['longest'] = id
                    times[shader]['longestDuration'] = duration
            else:
                times[shader] = {'draws': 1, 'duration': duration, 'longest': id, 'longestDuration': duration}

    times = sorted(times.items(), key=lambda x: x[1]['duration'], reverse=True)

    print '+------------+--------------+--------------------+--------------+-------------+'
    print '| Shader[id] |   Draws [#]  |   Duration [ns]  v | Per Call[ns] | Longest[id] |'
    print '+------------+--------------+--------------------+--------------+-------------+'

    for shader in times:
        id = str(shader[0]).rjust(10)
        draw = str(shader[1]['draws']).rjust(12)
        dura = str(shader[1]['duration']).rjust(18)
        perCall = str(shader[1]['duration'] / shader[1]['draws']).rjust(12)
        longest = str(shader[1]['longest']).rjust(11)
        print "| %s | %s | %s | %s | %s |" % (id, draw, dura, perCall, longest)

    print '+------------+--------------+--------------------+--------------+-------------+'


def main():
    if len(sys.argv) > 1:
        for arg in sys.argv[1:]:
            process(open(arg, 'rt'))
    else:
        process(sys.stdin)


if __name__ == '__main__':
    main()

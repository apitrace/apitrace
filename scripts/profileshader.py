#!/usr/bin/env python
##########################################################################
#
# Copyright 2012-2013 VMware, Inc.
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


import optparse
import sys


def process(stream, groupField):
    times = {}

    # Read header describing fields
    header = stream.readline()
    assert header.startswith('#')

    fields = header.rstrip('\r\n').split(' ')[1:]
    columns = {}
    for column in range(len(fields)):
        columns[fields[column]] = column

    callCol = columns['call']
    callIdCol = columns['no']
    gpuDuraCol = columns['gpu_dura']
    programCol = columns['program']
    funcNameCol = columns['name']

    groupCol = columns[groupField]

    maxGroupLen = 0

    for line in stream:
        fields = line.rstrip('\r\n').split(' ')

        if line.startswith('#'):
            continue

        if fields[callCol] == 'call':
            callId = long(fields[callIdCol])
            duration = long(fields[gpuDuraCol])
            group = fields[groupCol]

            maxGroupLen = max(maxGroupLen, len(group))

            if times.has_key(group):
                times[group]['draws'] += 1
                times[group]['duration'] += duration

                if duration > times[group]['longestDuration']:
                    times[group]['longest'] = callId
                    times[group]['longestDuration'] = duration
            else:
                times[group] = {'draws': 1, 'duration': duration, 'longest': callId, 'longestDuration': duration}

    times = sorted(times.items(), key=lambda x: x[1]['duration'], reverse=True)

    if groupField == 'program':
        groupTitle = 'Shader[id]'
    else:
        groupTitle = groupField
    maxGroupLen = max(maxGroupLen, len(groupTitle))
    groupTitle = groupField.center(maxGroupLen)
    groupLine = '-' * maxGroupLen

    print '+-%s-+--------------+--------------------+--------------+-------------+' % groupLine
    print '| %s |   Draws [#]  |   Duration [ns]  v | Per Call[ns] | Longest[id] |' % groupTitle
    print '+-%s-+--------------+--------------------+--------------+-------------+' % groupLine

    for group in times:
        id = str(group[0]).rjust(maxGroupLen)
        draw = str(group[1]['draws']).rjust(12)
        dura = str(group[1]['duration']).rjust(18)
        perCall = str(group[1]['duration'] / group[1]['draws']).rjust(12)
        longest = str(group[1]['longest']).rjust(11)
        print "| %s | %s | %s | %s | %s |" % (id, draw, dura, perCall, longest)

    print '+-%s-+--------------+--------------------+--------------+-------------+' % groupLine


def main():

    # Parse command line options
    optparser = optparse.OptionParser(
        usage='\n\t%prog [options] <profile_input>',
        version='%%prog')

    optparser.add_option(
        '-g', '--group', metavar='FIELD',
        type="string", dest="group", default='program',
        help="group by specified field [default: %default]")
    
    (options, args) = optparser.parse_args(sys.argv[1:])

    if len(args):
        for arg in args:
            process(open(arg, 'rt'), options.group)
    else:
        process(sys.stdin, options.group)


if __name__ == '__main__':
    main()

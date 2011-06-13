#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 Jose Fonseca
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/

'''Script to recompress a trace.
'''


import optparse
import os.path
import sys
import gzip
import tempfile
import shutil


def repack(in_name):
    mtime = os.path.getmtime(in_name)
    out_name = tempfile.mktemp()

    in_stream = gzip.GzipFile(in_name, 'rb')
    out_stream = gzip.GzipFile(out_name, 'wb', compresslevel=9, mtime=mtime)
    
    shutil.copyfileobj(in_stream, out_stream)
    
    in_stream.close()
    out_stream.close()
    
    in_size = os.path.getsize(in_name)
    out_size = os.path.getsize(out_name)

    print '%u -> %u' % (in_size, out_size)
    
    if out_size < in_size:
        shutil.move(out_name, in_name)
    else:
        os.unlink(out_name)


def main():
    optparser = optparse.OptionParser(
        usage='\n\t%prog <trace> ...',
        version='%%prog')

    (options, args) = optparser.parse_args(sys.argv[1:])
    if not args:
        optparser.error("incorrect number of arguments")

    map(repack, args)


if __name__ == '__main__':
    main()

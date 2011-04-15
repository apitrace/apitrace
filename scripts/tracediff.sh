#!/bin/bash
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

set -e

TRACEDUMP=${TRACEDUMP:-`dirname "$0"`/tracedump}

$TRACEDUMP

stripdump () {
    # http://www.theeggeadventure.com/wikimedia/index.php/Linux_Tips#Use_sed_to_remove_ANSI_colors
    $TRACEDUMP "$1" \
    | sed \
        -e 's/\x1b\[[0-9]\{1,2\}\(;[0-9]\{1,2\}\)\{0,2\}m//g' \
        -e 's/\r$//g' \
        -e 's/^[0-9]\+ //' \
        -e 's/hdc = \w\+/hdc/g' \
    | head -1000 \
    > "$2"
}

FIFODIR=`mktemp -d`
FIFO1="$FIFODIR/1"
FIFO2="$FIFODIR/2"

mkfifo "$FIFO1"
mkfifo "$FIFO2"

stripdump "$1" "$FIFO1" &
stripdump "$2" "$FIFO2" &

sdiff --width=`tput cols` --speed-large-files "$FIFO1" "$FIFO2" | less -R

rm -rf "$FIFODIR"

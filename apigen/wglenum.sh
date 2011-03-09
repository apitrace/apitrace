#!/bin/sh

sed -n -e 's/^\s\+\(WGL_\S\+\)\s*=\s*\(0x2\w\w\w\)\s*$/\2 \1/p' "$@" \
| sort -u \
| sed -e 's/\(\S\+\)\s\+\(\S\+\)/    "\2",\t\t# \1/'

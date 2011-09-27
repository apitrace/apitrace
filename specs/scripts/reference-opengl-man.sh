#!/bin/sh
# Script to extract reference URLS for functions documented in OpenGL man pages

wget -N -r -np http://www.opengl.org/sdk/docs/{man,man2,man3,man4}/

find www.opengl.org -type f -name '*.xml' \
| xargs grep -o '<b class="fsfunc">[^<]*</b>' \
| sed -e 's/<[^>]*>//g' -e 's@^\(.*\):\(.*\)$@\2\thttp://\1@'

#!/bin/bash
# Script to extract reference URLS for functions documented in OpenGL man pages

wget -N -r -np https://www.opengl.org/sdk/docs/{man,man2,man3,man4}/

find www.opengl.org -type f -name '*.xml' -o -name '*.xhtml' \
| xargs grep -o '<\(b\|strong\) class="fsfunc">[^<]*</\1>' \
| sed -e 's/<[^>]*>//g' -e 's@^\(.*\):\(.*\)$@\2\thttps://\1@'

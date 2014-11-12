#!/bin/bash
# Script to extract reference URLs for functions documented in OpenGL ARB specs

extract_urls () {
    for URL
    do
            lynx -dump "$URL" | sed -n -e '/^References$/,$s/^ *[0-9]\+\. \+//p' | sed -e 's/ /%20/g'
    done
}

extract_functions () {
    sed -n -e '/^New Procedures and Functions$/,/^\w/ s/.* \(\w\+\)(.*$/\1/p' "$@" \
    | sed -e '/^[A-Z]/s/^/gl/'
}

extract_urls https://www.opengl.org/registry/ \
| grep '^https://www\.opengl\.org/registry/specs/\w\+/.*\.txt$' \
| sort -u \
| while read URL
do
    wget --quiet -O - $URL \
    | extract_functions \
    | while read FUNCTION
    do
        echo "$FUNCTION	$URL"
    done
done
    
                


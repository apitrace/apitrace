#!/bin/sh
# Script to recompress the traces

set -e

for TRACE
do
    gzip -l "$TRACE" | sed -n '2p' | while read COMPRESSED UNCOMPRESSED RATIO FILENAME
    do
        gzip -dc "$TRACE" | pv -s "$UNCOMPRESSED" | gzip --best --no-name > "$TRACE.pack"
    done
    COMPRESSED=`stat -c %s "$TRACE"`
    RECOMPRESSED=`stat -c %s "$TRACE.pack"`
    echo "$COMPRESSED -> $RECOMPRESSED"
    if [ "$RECOMPRESSED" -lt "$COMPRESSED" ]
    then
        touch -r "$TRACE" "$TRACE.pack"
        mv "$TRACE.pack" "$TRACE"
    else
        rm "$TRACE.pack"
    fi
done

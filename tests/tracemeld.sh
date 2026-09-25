#!/bin/bash
#
#   tracemeld.sh foo.ref.txt foo.trace
#
DIFFTOOL=${DIFFTOOL:-meld}
exec $DIFFTOOL "$1" <(apitrace dump --call-nos=no "$2")

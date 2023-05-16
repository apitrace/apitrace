#!/bin/sh
export TEST=${TEST:-true}
exec ci/build-docker.sh ubuntu32 \
    -G Ninja \
    -DCMAKE_C_FLAGS=-m32 \
    -DCMAKE_CXX_FLAGS=-m32 \
    -DENABLE_STATIC_EXE=1 \
    -DENABLE_GUI=0

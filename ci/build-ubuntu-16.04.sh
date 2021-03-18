#!/bin/sh
exec ci/build-docker.sh ubuntu-16.04 \
    -G Ninja \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=OFF \
    -DENABLE_STATIC_LIBSTDCXX=OFF \
    -DENABLE_STATIC_EXE=OFF \
    -DENABLE_WAFFLE=on

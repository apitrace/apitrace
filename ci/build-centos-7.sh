#!/bin/sh
exec ci/build-docker.sh centos-7 \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=ON \
    -DENABLE_STATIC_LIBSTDCXX=ON \
    -DENABLE_STATIC_EXE=ON

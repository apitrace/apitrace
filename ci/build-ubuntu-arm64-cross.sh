#!/bin/sh
exec ci/build-docker.sh ubuntu-arm64-cross \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchain/aarch64-linux-gnu.cmake \
    -DX11_X11_INCLUDE_PATH=/usr/include \
    -DX11_X11_LIB=/usr/lib/aarch64-linux-gnu/libX11.so \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=OFF \
    -DENABLE_STATIC_LIBSTDCXX=OFF \
    -DENABLE_STATIC_EXE=OFF

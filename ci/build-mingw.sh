#!/bin/sh

set -eux

test $# -gt 0 || set -- mingw32 mingw64

for target
do
    test -f cmake/toolchain/$target.cmake
    test -f dxsdk-master/Cache-$target.cmake
    cmake \
        -S . \
        -B build/$target \
        -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchain/$target.cmake \
        -C dxsdk-master/Cache-mingw32.cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DENABLE_GUI=NO
    cmake --build build/$target --use-stderr
    cmake --build build/$target --use-stderr --target check
    if [ "${GITHUB_ACTIONS:-false}" = true ]
    then
        cmake --build build/$target --use-stderr --target package
    fi
done

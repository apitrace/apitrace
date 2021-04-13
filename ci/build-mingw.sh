#!/bin/sh

set -eux

test $# -gt 0 || set -- mingw32 mingw64

test ! -d /usr/lib/ccache || export PATH="/usr/lib/ccache:$PATH"

for target
do
    test -f cmake/toolchain/$target.cmake
    test -f dxsdk-master/Cache-$target.cmake
    if [ ! build/$target/CMakeCache.txt -nt "$0" ]
    then
        cmake \
            -S . \
            -B build/$target \
            -G Ninja \
            -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchain/$target.cmake \
            -C dxsdk-master/Cache-mingw32.cmake \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=YES \
            -DENABLE_GUI=NO
    fi
    cmake --build build/$target --use-stderr
    cmake --build build/$target --use-stderr --target check
    if [ "${GITHUB_ACTIONS:-false}" = true ]
    then
        cmake --build build/$target --use-stderr --target package
    fi
done

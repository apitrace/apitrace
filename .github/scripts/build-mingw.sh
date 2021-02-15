#!/bin/sh

set -ex

test -n "$MINGW_PREFIX"

echo "set (CMAKE_SYSTEM_NAME Windows)" >> Toolchain.cmake
echo "set (CMAKE_C_COMPILER $MINGW_PREFIX-gcc)" >> Toolchain.cmake
echo "set (CMAKE_CXX_COMPILER $MINGW_PREFIX-g++)" >> Toolchain.cmake
echo "set (CMAKE_RC_COMPILER $MINGW_PREFIX-windres)" >> Toolchain.cmake
echo "set (CMAKE_FIND_ROOT_PATH /usr/$MINGW_PREFIX)" >> Toolchain.cmake
echo "set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)" >> Toolchain.cmake
echo "set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)" >> Toolchain.cmake
echo "set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)" >> Toolchain.cmake

cmake -H$PWD -B$PWD/build -G Ninja -DCMAKE_TOOLCHAIN_FILE=Toolchain.cmake -Cdxsdk-master/Cache-mingw32.cmake -DENABLE_GUI=0
cmake --build $PWD/build --use-stderr
cmake --build $PWD/build --use-stderr --target check
cmake --build $PWD/build --use-stderr --target package

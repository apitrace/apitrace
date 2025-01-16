#!/bin/sh

set -eu

set -x

if which ninja > /dev/null
then
    cmakeGenerator="Ninja"
else
    cmakeGenerator="Unix Makefiles"
fi

export CMAKE_BUILD_PARALLEL_LEVEL=$(getconf _NPROCESSORS_ONLN)


cmake -S. -Bbuild -G "$cmakeGenerator" -DENABLE_GUI=OFF
cmake --build build --use-stderr
cmake --build build --use-stderr --target check

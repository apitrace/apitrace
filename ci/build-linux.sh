#!/bin/sh

set -eu

set -x

cmake --version

ninja --version

build_dir=build/linux

export CTEST_PARALLEL_LEVEL=${CTEST_PARALLEL_LEVEL:-$(getconf _NPROCESSORS_ONLN)}

test -f $build_dir/CMakeCache.txt || cmake -S. -B$build_dir -G Ninja -DENABLE_STATIC_SNAPPY=0 -DENABLE_GUI=1 -DENABLE_TESTS=1
cmake --build $build_dir --use-stderr
xwfb-run -n 8 -c weston -s \\-geometry -s 1024x768 -- cmake --build $build_dir --use-stderr --target check
cmake --build $build_dir --use-stderr --target package

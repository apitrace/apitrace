#!/bin/sh

set -e

distro=centos-6
source_dir=$PWD
build_dir=$source_dir/build/docker-$distro
docker_image=$distro-apitrace
nproc=`nproc`
uid=`id -u`

set -x

test -d $source_dir/ci/docker/$distro
docker build -t $docker_image $source_dir/ci/docker/$distro

docker \
    run -i -t -v $PWD:$PWD -u "$uid" $docker_image cmake \
    -H$source_dir \
    -B$build_dir \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release} \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=ON \
    -DENABLE_STATIC_LIBSTDCXX=ON \
    -DENABLE_STATIC_EXE=ON

docker run -i -t -v $PWD:$PWD -u "$uid" $docker_image cmake --build $build_dir -- -j$nproc all
if false
then
    docker run -i -t -v $PWD:$PWD -u "$uid" $docker_image cmake --build $build_dir -- -j$nproc test
fi

# https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html
objdump -p $build_dir/wrappers/glxtrace.so
objdump -p $build_dir/glretrace

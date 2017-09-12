#!/bin/sh

set -e

distro=ubuntu-16.04
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
    -G Ninja \
    -H$source_dir \
    -B$build_dir \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release} \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=OFF \
    -DENABLE_STATIC_LIBSTDCXX=OFF \
    -DENABLE_STATIC_EXE=OFF \
    -DENABLE_WAFFLE=on

docker run -i -t -v $PWD:$PWD -u "$uid" $docker_image cmake --build $build_dir -- -j$nproc all
if false
then
    docker run -i -t -v $PWD:$PWD -u "$uid" $docker_image cmake --build $build_dir -- -j$nproc test
fi

ldd -r $build_dir/glretrace
ldd -r $build_dir/eglretrace

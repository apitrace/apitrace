#!/bin/sh

set -e

distro=ubuntu-16.04
source_dir=$PWD
build_dir=$source_dir/build/docker-$distro
docker_image=$distro-apitrace
uid=$(id -u)

test -t 0 && interactive=true || interactive=false

set -x

docker_run () {
    docker run -i=$interactive --tty=$interactive --rm -v "$PWD:$PWD" -u "$uid" $docker_image "$@"
}

test -d $source_dir/ci/docker/$distro
docker build -t $docker_image $source_dir/ci/docker/$distro

docker_run \
    cmake \
    -G Ninja \
    -H$source_dir \
    -B$build_dir \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release} \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=OFF \
    -DENABLE_STATIC_LIBSTDCXX=OFF \
    -DENABLE_STATIC_EXE=OFF \
    -DENABLE_WAFFLE=on

docker_run cmake --build $build_dir
if false
then
    docker_run cmake --build $build_dir --target test
fi

ldd -r $build_dir/glretrace
ldd -r $build_dir/eglretrace

docker_run cmake --build $build_dir --target package

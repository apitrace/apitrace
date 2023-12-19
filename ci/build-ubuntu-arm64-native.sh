#!/bin/sh
set -eux

# https://www.stereolabs.com/docs/docker/building-arm-container-on-x86
if ! docker run --rm -t arm64v8/ubuntu:22.04 uname -m
then
    docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
    docker run --rm -t arm64v8/ubuntu:22.04 uname -m
fi

exec ci/build-docker.sh ubuntu-arm64-native \
    -G Ninja \
    -DENABLE_GUI=OFF \
    -DENABLE_STATIC_LIBGCC=OFF \
    -DENABLE_STATIC_LIBSTDCXX=OFF \
    -DENABLE_STATIC_EXE=OFF \
    -DENABLE_WAFFLE=on

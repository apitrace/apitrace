#!/bin/sh

set -e

distro=ubuntu-16.04
source_dir=$PWD
build_dir=$source_dir/build/docker-$distro
docker_image=$distro-apitrace
uid=$(id -u)

test -t 0 && interactive=true || interactive=false

docker_run () {
    docker run -i=$interactive --tty=$interactive --rm -v "$PWD:$PWD" -u "$uid" $docker_image "$@"
}

symbol_versions () {
    nm -D -u --with-symbol-versions "$@" | sed -n -e 's/.*@/\t/p' | sort -u
}

set -x

test -d $source_dir/ci/docker/$distro

if [ "$GITHUB_ACTIONS" = true ]
then
    # See:
    # - https://dev.to/dtinth/caching-docker-builds-in-github-actions-which-approach-is-the-fastest-a-research-18ei
    # - https://github.com/dtinth/github-actions-docker-layer-caching-poc/blob/master/.github/workflows/dockerimage.yml
    docker pull docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache || true
    docker build -t $docker_image --cache-from=docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache $source_dir/ci/docker/$distro
    docker tag $docker_image docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache && docker push docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache || true
else
    docker build -t $docker_image $source_dir/ci/docker/$distro
fi

if [ "$PACKAGE" = "true" ]
then
    CMAKE_BUILD_TYPE=Release
else
    CMAKE_BUILD_TYPE=Debug
fi

docker_run \
    cmake \
    -G Ninja \
    -H$source_dir \
    -B$build_dir \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
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

symbol_versions $build_dir/glretrace
symbol_versions $build_dir/eglretrace

if [ "$PACKAGE" = true ]
then
    docker_run cmake --build $build_dir --target package
fi

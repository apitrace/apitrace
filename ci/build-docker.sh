#!/bin/sh

set -e
set -x

source_dir=$PWD

distro="$1"
shift
test -f $source_dir/ci/docker/$distro.Dockerfile

build_dir=$source_dir/build/docker-$distro
docker_tag=$distro-apitrace
uid=$(id -u)

test -t 0 && interactive=true || interactive=false

docker_run () {
    docker run \
        --rm \
        -i=$interactive \
        --tty=$interactive \
        -e CI \
        -e GITHUB_ACTIONS \
        -e GITHUB_REF \
        -e GITHUB_REPOSITORY \
        -e GITHUB_RUN_ID \
        -e GITHUB_SERVER_URL \
        -e GITHUB_SHA \
        -v "$PWD:$PWD" \
        -w "$PWD" \
        -u "$uid" \
        $docker_tag \
        "$@"
}

symbol_versions () {
    # https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html
    objdump -p "$@" | sed -n -e '/^Version References:/,$p'
}


if [ "$GITHUB_ACTIONS" = true ]
then
    # See:
    # - https://dev.to/dtinth/caching-docker-builds-in-github-actions-which-approach-is-the-fastest-a-research-18ei
    # - https://github.com/dtinth/github-actions-docker-layer-caching-poc/blob/master/.github/workflows/dockerimage.yml
    docker pull docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache || true
    docker build -t $docker_tag --cache-from=docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache -f $source_dir/ci/docker/$distro.Dockerfile $source_dir/ci/docker
    docker tag $docker_tag docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache && docker push docker.pkg.github.com/$GITHUB_REPOSITORY/build-cache || true
else
    docker build -t $docker_tag -f $source_dir/ci/docker/$distro.Dockerfile $source_dir/ci/docker
fi

if [ "$PACKAGE" = "true" ]
then
    CMAKE_BUILD_TYPE=Release
else
    CMAKE_BUILD_TYPE=Debug
fi

docker_run \
    cmake \
    -H$source_dir \
    -B$build_dir \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
    "$@"

docker_run cmake --build $build_dir
if [ "$TEST" = "true" ]
then
    docker_run cmake --build $build_dir --target check
fi

symbol_versions $build_dir/glretrace
symbol_versions $build_dir/eglretrace
symbol_versions $build_dir/wrappers/glxtrace.so

if [ "$PACKAGE" = true ]
then
    docker_run cmake --build $build_dir --target package
fi

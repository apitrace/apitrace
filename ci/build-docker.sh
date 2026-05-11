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


if ! docker buildx version
then
   echo 'error: docker buildx plugin required (sudo apt install docker-buildx)' 1>&2
   exit 1
fi

if [ "$GITHUB_ACTIONS" = true ]
then
    # https://docs.docker.com/build/cache/backends/gha/
    docker buildx build \
        -t $docker_tag \
        --cache-from type=gha,scope=$distro \
        --cache-to type=gha,mode=max,scope=$distro \
        --load \
        -f $source_dir/ci/docker/$distro.Dockerfile $source_dir/ci/docker
else
    docker buildx build -t $docker_tag -f $source_dir/ci/docker/$distro.Dockerfile $source_dir/ci/docker
fi

if [ "${GITHUB_EVENT_NAME:-}" = "push" -a \( "${GITHUB_REF:=}" = "refs/heads/master" -o "${GITHUB_REF%/*}" = "refs/tags" \) ]
then
    CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release}
else
    CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Debug}
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

if [ "$GITHUB_ACTIONS" = true ]
then
    docker_run cmake --build $build_dir --target package
fi

#!/bin/bash

set -eu

qtVersion=6.8.3
qtTag=0-202503201723
qtVersionDotless=$(echo $qtVersion | sed -e 's/\.//g')
qtArchive=$qtVersion-${qtTag}qtbase-MacOS-MacOS_14-Clang-MacOS-MacOS_14-X86_64-ARM64.7z
qtArchiveUrl=https://download.qt.io/online/qtsdkrepository/mac_x64/desktop/qt6_$qtVersionDotless/qt6_$qtVersionDotless/qt.qt6.$qtVersionDotless.clang_64/$qtArchive

set -x

mkdir -p qt
pushd qt

if [ ! -e $qtArchive ]
then
    curl -sLO $qtArchiveUrl
fi

qtToolchainPath=$PWD/$qtVersion/macos
if [ ! -d $qtToolchainPath ]
then
    # Unlike Qt5, Qt6 archives are relative to the toolchain directory
    mkdir -p $qtToolchainPath
    cmake -E chdir $qtToolchainPath cmake -E tar xf $PWD/$qtArchive --format=7zip
    test -d $qtToolchainPath/lib
fi

popd

if which ninja > /dev/null
then
    cmakeGenerator="Ninja"
else
    cmakeGenerator="Unix Makefiles"
fi

export CMAKE_BUILD_PARALLEL_LEVEL=$(getconf _NPROCESSORS_ONLN)


cmake -S. -Bbuild -G "$cmakeGenerator" -DCMAKE_PREFIX_PATH=$qtToolchainPath -DENABLE_GUI=ON -DENABLE_QT6=ON
cmake --build build --use-stderr
cmake --build build --use-stderr --target check

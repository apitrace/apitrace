#!/bin/sh

qtVersion=5.12.10
qtVersionDotless=$(echo $qtVersion | sed -e 's/\.//g')
qtArchive=5.12.10-0-202011040843qtbase-MacOS-MacOS_10_13-Clang-MacOS-MacOS_10_13-X86_64.7z
qtArchiveUrl=https://download.qt.io/online/qtsdkrepository/mac_x64/desktop/qt5_$qtVersionDotless/qt.qt5.$qtVersionDotless.clang_64/$qtArchive

set -ex

mkdir -p qt
pushd qt

if [ ! -e $qtArchive ]
then
    curl -sLO $qtArchiveUrl
fi

qtToolchainPath=$PWD/$qtVersion/clang_64
if [ ! -d $qtToolchainPath ]
then
    cmake -E tar xf $qtArchive --format=7zip
    test -d $qtToolchainPath
fi

popd


export CMAKE_BUILD_PARALLEL_LEVEL=$(getconf _NPROCESSORS_ONLN)


cmake -S. -Bbuild -DCMAKE_PREFIX_PATH=$qtToolchainPath -DENABLE_GUI=ON
cmake --build build --use-stderr
cmake --build build --use-stderr --target check

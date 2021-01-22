FROM ubuntu:16.04
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

# https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/test
RUN \
 echo deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu xenial main > /etc/apt/sources.list.d/ubuntu-toolchain-r.list && \
 apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F && \
 apt-get update && apt-get install -y \
  build-essential \
  zlib1g-dev \
  libdwarf-dev \
  libx11-dev \
  libgl-dev \
  libwaffle-dev \
  python2.7 \
  python3 \
  ninja-build \
  cmake \
  g++-7 && \
 rm -rf /var/lib/apt/lists/*

RUN \
 update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60 \
                     --slave /usr/bin/g++ g++ /usr/bin/g++-7 \
                     --slave /usr/bin/gcov gcov /usr/bin/gcov-7 \
                     --slave /usr/bin/gcov-tool gcov-tool /usr/bin/gcov-tool-7 \
                     --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-7 \
                     --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-7 \
                     --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-7

CMD ["/bin/bash"]

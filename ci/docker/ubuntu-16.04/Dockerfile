FROM ubuntu:16.04
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

# See also:
# - https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/test
# - https://apt.kitware.com/

RUN \
 apt-get update && apt-get install -y \
  apt-transport-https ca-certificates gnupg software-properties-common wget && \
 add-apt-repository ppa:ubuntu-toolchain-r/test && \
 apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main' && \
 wget -q -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor - > /etc/apt/trusted.gpg.d/kitware.gpg && \
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
 rm -rf /var/lib/apt/lists/* && \
 update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60 \
                     --slave /usr/bin/g++ g++ /usr/bin/g++-7 \
                     --slave /usr/bin/gcov gcov /usr/bin/gcov-7 \
                     --slave /usr/bin/gcov-tool gcov-tool /usr/bin/gcov-tool-7 \
                     --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-7 \
                     --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-7 \
                     --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-7

CMD ["/bin/bash"]

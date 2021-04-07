FROM ubuntu:18.04
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

RUN \
 apt-get update && apt-get install -y --no-install-recommends \
  g++-8 \
  zlib1g-dev \
  libdwarf-dev \
  libx11-dev \
  libgl-dev \
  libwaffle-dev \
  python3 \
  ninja-build \
  cmake && \
 rm -rf /var/lib/apt/lists/* && \
 update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 \
                     --slave /usr/bin/g++ g++ /usr/bin/g++-8 \
                     --slave /usr/bin/gcov gcov /usr/bin/gcov-8 \
                     --slave /usr/bin/gcov-tool gcov-tool /usr/bin/gcov-tool-8 \
                     --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-8 \
                     --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-8 \
                     --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-8

CMD ["/bin/bash"]

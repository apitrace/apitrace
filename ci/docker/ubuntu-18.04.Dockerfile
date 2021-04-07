FROM ubuntu:18.04
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

RUN \
 apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  zlib1g-dev \
  libdwarf-dev \
  libx11-dev \
  libgl-dev \
  libwaffle-dev \
  python3 \
  ninja-build \
  cmake && \
 rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]

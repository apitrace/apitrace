FROM ubuntu:20.04
MAINTAINER "Jose Fonseca" <jose.fonseca@broadcom.com>
ENV container docker

ENV DEBIAN_FRONTEND=noninteractive

RUN \
 apt-get update && apt-get install -y --no-install-recommends \
  g++ \
  zlib1g-dev \
  libdwarf-dev \
  libx11-dev \
  libgl-dev \
  libwaffle-dev \
  pkg-config \
  python3 \
  ninja-build \
  cmake \
  git && \
 rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]

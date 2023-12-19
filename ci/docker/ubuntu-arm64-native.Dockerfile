FROM arm64v8/ubuntu:22.04
MAINTAINER "Jose Fonseca" <jose.fonseca@broadcom.com>
ENV container docker

# See also:
# - https://dev.to/flpslv/running-winehq-inside-a-docker-container-52ej
# - https://wiki.winehq.org/Ubuntu

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
  g++ \
  zlib1g-dev \
  libdwarf-dev \
  libx11-dev \
  libgl-dev \
  libwaffle-dev \
  python3 \
  ninja-build \
  cmake \
  git \
&& rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]

FROM ubuntu:22.04
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
 python3 \
 ninja-build \
 cmake \
 gcc-multilib g++-multilib \
&& rm -rf /var/lib/apt/lists/*

RUN dpkg --add-architecture i386 \
&& apt-get update && apt-get install -y --no-install-recommends \
 libx11-dev:i386 \
&& rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]

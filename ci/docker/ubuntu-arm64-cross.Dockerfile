FROM ubuntu:22.04
MAINTAINER "Jose Fonseca" <jose.fonseca@broadcom.com>
ENV container docker

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture arm64 \
&& sed -i -e 's/^deb /deb [arch=amd64] /' /etc/apt/sources.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy main restricted' > /etc/apt/sources.list.d/arm64.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates main restricted' >> /etc/apt/sources.list.d/arm64.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy universe' >> /etc/apt/sources.list.d/arm64.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates universe' >> /etc/apt/sources.list.d/arm64.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy multiverse' >> /etc/apt/sources.list.d/arm64.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates multiverse' >> /etc/apt/sources.list.d/arm64.list \
&& echo 'deb [arch=arm64] http://ports.ubuntu.com/ jammy-backports main restricted universe multiverse' >> /etc/apt/sources.list.d/arm64.list

RUN apt-get update && apt-get install -y --no-install-recommends \
 python3 \
 ninja-build \
 cmake \
 gcc-11-aarch64-linux-gnu \
 g++-11-aarch64-linux-gnu \
 git \
 libx11-dev:arm64 \
&& rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]

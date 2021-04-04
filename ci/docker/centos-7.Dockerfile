FROM centos:7
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

RUN yum -y update && \
    yum clean all
RUN yum -y install zlib-devel libX11-devel make && \
    yum clean all

RUN yum -y install epel-release && \
    yum clean all
RUN yum -y install cmake3 && \
    yum clean all && \
    ln -s cmake3 /usr/bin/cmake

RUN yum -y install centos-release-scl && \
    yum clean all

# https://www.softwarecollections.org/en/scls/rhscl/rh-python36/
RUN yum -y install rh-python36 && \
    yum clean all && \
    /usr/bin/scl enable rh-python36 true

# https://www.softwarecollections.org/en/scls/rhscl/devtoolset-7/
RUN yum -y install devtoolset-7-gcc devtoolset-7-binutils devtoolset-7-gcc-c++ && \
    yum clean all && \
    /usr/bin/scl enable devtoolset-7 true

# Make sure the above SCLs are already enabled
ENTRYPOINT ["/usr/bin/scl", "enable", "rh-python36", "devtoolset-7", "--"]
CMD ["/usr/bin/scl", "enable", "rh-python36", "devtoolset-7", "--", "/bin/bash"]

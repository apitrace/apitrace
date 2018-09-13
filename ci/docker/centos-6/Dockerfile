FROM centos:6
MAINTAINER "Jose Fonseca" <jfonseca@vmware.com>
ENV container docker

RUN yum -y update && yum clean all
RUN yum -y install zlib-devel libX11-devel make cmake && yum clean all

RUN yum -y install centos-release-scl && yum clean all

# https://www.softwarecollections.org/en/scls/rhscl/python27/
RUN yum -y install python27 && yum clean all
RUN /usr/bin/scl enable python27 true

# https://www.softwarecollections.org/en/scls/rhscl/devtoolset-7/
#RUN yum -y install devtoolset-7 && yum clean all
RUN yum -y install devtoolset-7-gcc devtoolset-7-binutils devtoolset-7-gcc-c++ && yum clean all
RUN /usr/bin/scl enable devtoolset-7 true

# Make sure the above SCLs are already enabled
ENTRYPOINT ["/usr/bin/scl", "enable", "python27", "devtoolset-7", "--"]
CMD ["/usr/bin/scl", "enable", "python27", "devtoolset-7", "--", "/bin/bash"]

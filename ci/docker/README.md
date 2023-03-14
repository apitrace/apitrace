# About

Docker image that enables building [apitrace](https://github.com/apitrace/apitrace/) for CentOS 7 / RHEL 7.

# Instructions

Run

    ci/build-centos-7.sh

There should be:

* no `glibc` symbol with version higher than `GLIBC_2.14`

* no `GLIBCXX_*`symbol whatsoever, given that `libstdc++` is statically linked

See also https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html

# About

Docker image that enables building [apitrace](https://github.com/apitrace/apitrace/) for CentOS 6 / RHEL 6.

# Instructions

Run

    ci/build-centos-6.sh

There should be:

* no `glibc` symbol with version higher than `GLIBC_2.12`

* no `GLIBCXX_*`symbol whatsoever, given that `libstdc++` is statically linked

See also https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html

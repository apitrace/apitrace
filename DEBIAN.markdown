Building Debian package
=======================

To build a debian package you should

 * check out apitrace from git

 * make tarball

    debian/rules tarball

  The tarball is created in parent directory so that dpkg-buildpackage finds it.

 * build package

    dpkg-buildpackage


Obviously, you will need the Debian development tools (dpkg-dev).



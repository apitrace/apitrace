set (CMAKE_SYSTEM_NAME Linux)
set (CMAKE_SYSTEM_PROCESSOR aarch64)

set (CMAKE_C_COMPILER aarch64-linux-gnu-gcc-11)
set (CMAKE_CXX_COMPILER aarch64-linux-gnu-g++-11)

set (CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html#rpath-handling-under-unix
set (CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set (CMAKE_INSTALL_RPATH /usr/lib/aarch64-linux-gnu)

set (CMAKE_CROSSCOMPILING TRUE)

# Copyright 2017 Igalia S.L. All Rights Reserved.
#
# Distributed under MIT license.
# See file LICENSE for detail or copy at https://opensource.org/licenses/MIT

# Try to find BrotliEnc. Once done, this will define
#
#  BROTLIENC_FOUND - system has BrotliEnc.
#  BROTLIENC_INCLUDE_DIRS - the BrotliEnc include directories
#  BROTLIENC_LIBRARIES - link these to use BrotliEnc.

find_package(PkgConfig)

pkg_check_modules(PC_BROTLIENC libbrotlienc)

find_path(BROTLIENC_INCLUDE_DIRS
    NAMES brotli/encode.h
    HINTS ${PC_BROTLIENC_INCLUDEDIR}
)

find_library(BROTLIENC_LIBRARIES
    NAMES brotlienc
    HINTS ${PC_BROTLIENC_LIBDIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BrotliEnc
    REQUIRED_VARS BROTLIENC_INCLUDE_DIRS BROTLIENC_LIBRARIES
    FOUND_VAR BROTLIENC_FOUND
    VERSION_VAR PC_BROTLIENC_VERSION)

mark_as_advanced(
    BROTLIENC_INCLUDE_DIRS
    BROTLIENC_LIBRARIES
)

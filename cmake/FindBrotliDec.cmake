# Copyright 2017 Igalia S.L. All Rights Reserved.
#
# Distributed under MIT license.
# See file LICENSE for detail or copy at https://opensource.org/licenses/MIT

# Try to find BrotliDec. Once done, this will define
#
#  BROTLIDEC_FOUND - system has BrotliDec.
#  BROTLIDEC_INCLUDE_DIRS - the BrotliDec include directories
#  BROTLIDEC_LIBRARIES - link these to use BrotliDec.

find_package(PkgConfig)

pkg_check_modules(PC_BROTLIDEC libbrotlidec)

find_path(BROTLIDEC_INCLUDE_DIRS
    NAMES brotli/decode.h
    HINTS ${PC_BROTLIDEC_INCLUDEDIR}
)

find_library(BROTLIDEC_LIBRARIES
    NAMES brotlidec
    HINTS ${PC_BROTLIDEC_LIBDIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BrotliDec
    REQUIRED_VARS BROTLIDEC_INCLUDE_DIRS BROTLIDEC_LIBRARIES
    FOUND_VAR BROTLIDEC_FOUND
    VERSION_VAR PC_BROTLIDEC_VERSION)

mark_as_advanced(
    BROTLIDEC_INCLUDE_DIRS
    BROTLIDEC_LIBRARIES
)

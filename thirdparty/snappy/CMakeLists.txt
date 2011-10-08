# We only do the subset of the snappy autoconf that actually affect the library
# (as most of them are for the tests which we don't build.)

include (CheckIncludeFileCXX)
include (CheckCXXSourceCompiles)
include (TestBigEndian)

check_include_file_cxx (sys/mman.h HAVE_SYS_MMAN_H)
check_include_file_cxx (sys/byteorder.h HAVE_SYS_BYTEORDER_H)
check_include_file_cxx (sys/endian.h HAVE_SYS_ENDIAN_H)
check_include_file_cxx (byteswap.h HAVE_BYTESWAP_H)

check_cxx_source_compiles (
    "main() { return __builtin_expect(1, 1) ? 1 : 0; }"
    HAVE_BUILTIN_EXPECT
)

check_cxx_source_compiles (
    "main() { return (__builtin_ctzll(0x100000000LL) == 32) ? 1 : 0; }"
    HAVE_BUILTIN_CTZ
)

test_big_endian (WORDS_BIGENDIAN)

configure_file (config.h.in config.h)

add_definitions (-DHAVE_CONFIG_H)

# Adjust warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions (-Wno-unused-function)
endif ()


include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_library (snappy_bundled STATIC
    snappy.cc
    snappy-sinksource.cc
    snappy-stubs-internal.cc
    snappy-c.cc
)

set_target_properties (snappy_bundled PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
)

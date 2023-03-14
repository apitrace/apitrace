project (Snappy VERSION 1.1.8 LANGUAGES C CXX)

# TODO generate config.h from snappy/cmake/config.h.in

check_include_file ("sys/uio.h" HAVE_SYS_UIO_H)
if (HAVE_SYS_UIO_H)
    set (HAVE_SYS_UIO_H_01 1)
else ()
    set (HAVE_SYS_UIO_H_01 0)
endif ()

configure_file (
    snappy/snappy-stubs-public.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config/snappy/snappy-stubs-public.h)

add_convenience_library (snappy EXCLUDE_FROM_ALL
    snappy/snappy-internal.h
    snappy/snappy-stubs-internal.h
    snappy/snappy-c.cc
    snappy/snappy-sinksource.cc
    snappy/snappy-stubs-internal.cc
    snappy/snappy.cc

    snappy/snappy-c.h
    snappy/snappy-sinksource.h
    snappy/snappy.h
    ${CMAKE_CURRENT_BINARY_DIR}/config/snappy/snappy-stubs-public.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/snappy/config.h
)
target_compile_definitions (snappy PRIVATE
    NDEBUG
    HAVE_CONFIG_H
)
if (NOT MSVC)
    target_compile_options (snappy PRIVATE
        -Wno-unused-function
        -fstrict-aliasing
    )
endif ()

target_include_directories (snappy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/support/snappy
)
target_include_directories (snappy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/snappy
    ${CMAKE_CURRENT_BINARY_DIR}/config/snappy
)

target_optimize (snappy)

add_library (Snappy::snappy ALIAS snappy)

install (
    FILES snappy/COPYING
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-snappy.txt
)

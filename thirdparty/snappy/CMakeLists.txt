force_release_build ()

add_convenience_library (snappy EXCLUDE_FROM_ALL
    snappy-internal.h
    snappy-stubs-internal.h
    snappy-c.cc
    snappy-sinksource.cc
    snappy-stubs-internal.cc
    snappy.cc

    snappy-c.h
    snappy-sinksource.h
    snappy.h
    snappy-stubs-public.h
    config/config.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)
target_include_directories (snappy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library (Snappy::snappy ALIAS snappy)

install (
    FILES COPYING
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-snappy.txt
)

force_release_build ()

add_definitions (
    -DNDEBUG
    -DHAVE_CONFIG_H
)

# Adjust warnings
if (NOT MSVC)
    add_compiler_flags (-Wno-unused-function)
    add_compiler_flags (-fstrict-aliasing)
endif ()

add_convenience_library (snappy_bundled EXCLUDE_FROM_ALL
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

target_include_directories (snappy_bundled PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)

install (
    FILES COPYING
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-snappy.txt
)

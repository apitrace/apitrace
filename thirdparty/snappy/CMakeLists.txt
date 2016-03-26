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
    snappy.cc
    snappy-sinksource.cc
    snappy-stubs-internal.cc
    snappy-c.cc
)

install (
    FILES COPYING
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-snappy.txt
)

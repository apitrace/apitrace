add_definitions (
    -DNDEBUG
    -DHAVE_CONFIG_H
)

# Adjust warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions (-Wno-unused-function)
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

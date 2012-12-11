include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_library (getopt_bundled STATIC EXCLUDE_FROM_ALL
    getopt_long.c
)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-getopt.txt
)

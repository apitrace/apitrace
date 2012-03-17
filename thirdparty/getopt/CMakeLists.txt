include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_library (getopt_bundled STATIC
    getopt_long.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library (zlib STATIC
    adler32.c
    compress.c
    crc32.c
    gzio.c
    uncompr.c
    deflate.c
    trees.c
    zutil.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
)

set_target_properties (zlib PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}"
)

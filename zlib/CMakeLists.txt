include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (NOT WIN32)
    add_definitions (-fPIC)
endif (NOT WIN32)

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

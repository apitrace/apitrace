force_release_build ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# adjust warnings
if (MSVC)
    add_definitions (-wd4131) # uses old-style declarator
else ()
    add_definitions (-DHAVE_UNISTD_H)
endif ()

add_convenience_library (z_bundled EXCLUDE_FROM_ALL
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    zutil.c
)

install (
    FILES README
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-zlib.txt
)

force_release_build ()

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

target_include_directories (z_bundled PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
    # adjust warnings
    target_compile_options (z_bundled PRIVATE -wd4131) # uses old-style declarator
else ()
    target_compile_definitions (z_bundled PRIVATE HAVE_UNISTD_H)
endif ()

install (
    FILES README
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-zlib.txt
)

add_library (ZLIB::ZLIB ALIAS z_bundled)

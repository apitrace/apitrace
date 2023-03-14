add_convenience_library (z_bundled EXCLUDE_FROM_ALL
    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/deflate.c
    zlib/gzclose.c
    zlib/gzlib.c
    zlib/gzread.c
    zlib/gzwrite.c
    zlib/inflate.c
    zlib/infback.c
    zlib/inftrees.c
    zlib/inffast.c
    zlib/trees.c
    zlib/uncompr.c
    zlib/zutil.c
)

target_include_directories (z_bundled PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/zlib)

if (MSVC)
    # adjust warnings
    target_compile_options (z_bundled PRIVATE -wd4131) # uses old-style declarator
else ()
    target_compile_definitions (z_bundled PRIVATE HAVE_UNISTD_H)
    if (CMAKE_C_COMPILER_ID MATCHES Clang)
        target_compile_options (z_bundled PRIVATE -Wno-deprecated-non-prototype)
    endif ()
endif ()

target_optimize (z_bundled)

install (
    FILES zlib/README
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-zlib.txt
)

add_library (ZLIB::ZLIB ALIAS z_bundled)

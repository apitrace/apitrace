include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_convenience_library (png_bundled EXCLUDE_FROM_ALL
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

if (APPLE)
    # https://github.com/apitrace/apitrace/issues/711
    target_compile_definitions (png_bundled PRIVATE PNG_ARM_NEON_OPT=0)
endif ()

target_link_libraries (png_bundled PUBLIC ZLIB::ZLIB)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-libpng.txt
)

# TODO: Generate pnglibconf.h from libpng/scripts/pnglibconf.h.prebuilt

add_convenience_library (png_bundled EXCLUDE_FROM_ALL
    libpng/png.c
    libpng/pngerror.c
    libpng/pngget.c
    libpng/pngmem.c
    libpng/pngpread.c
    libpng/pngread.c
    libpng/pngrio.c
    libpng/pngrtran.c
    libpng/pngrutil.c
    libpng/pngset.c
    libpng/pngtrans.c
    libpng/pngwio.c
    libpng/pngwrite.c
    libpng/pngwtran.c
    libpng/pngwutil.c
)

target_include_directories (png_bundled PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libpng
    ${CMAKE_CURRENT_SOURCE_DIR}/support/libpng
)

if (APPLE)
    # https://github.com/apitrace/apitrace/issues/711
    target_compile_definitions (png_bundled PRIVATE PNG_ARM_NEON_OPT=0)
endif ()

target_link_libraries (png_bundled PUBLIC ZLIB::ZLIB)

install (
    FILES libpng/LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-libpng.txt
)

add_library (PNG::PNG ALIAS png_bundled)

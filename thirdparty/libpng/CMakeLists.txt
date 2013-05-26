include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library (png_bundled STATIC EXCLUDE_FROM_ALL
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

set_target_properties (png_bundled PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}"
)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-libpng.txt
)

if (ENABLE_STATIC_SNAPPY OR NOT Snappy_FOUND)
    message (STATUS "Using bundled SNAPPY")
    add_subdirectory (snappy)
endif ()

if (NOT ZLIB_FOUND)
    message (STATUS "Using bundled ZLIB")
    add_subdirectory (zlib)
endif ()

if (NOT PNG_FOUND)
    message (STATUS "Using bundled PNG")
    add_subdirectory (libpng)
endif ()

if (NOT BROTLIDEC_FOUND OR NOT BROTLIENC_FOUND)
    add_subdirectory (brotli)
endif ()

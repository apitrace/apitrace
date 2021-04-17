# Force certain components to be optimized as release
macro (target_optimize TARGET)
    target_compile_definitions (${TARGET} PRIVATE -DNDEBUG)
    if (MSVC)
        # TODO: MSVC makes it impossible to override early options.
    else ()
        target_compile_options (${TARGET} PRIVATE -O3)
    endif ()
endmacro ()

if (ENABLE_STATIC_SNAPPY OR NOT Snappy_FOUND)
    message (STATUS "Using bundled Snappy")
    include (snappy.cmake)
endif ()

if (NOT ZLIB_FOUND)
    message (STATUS "Using bundled ZLIB")
    include (zlib.cmake)
endif ()

if (NOT PNG_FOUND)
    message (STATUS "Using bundled PNG")
    include (libpng.cmake)
endif ()

if (NOT BROTLIDEC_FOUND OR NOT BROTLIENC_FOUND)
    message (STATUS "Using bundled Brotli")
    include (brotli.cmake)
endif ()

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    add_subdirectory (libbacktrace)
endif ()

# We use non-standard C++ flags, so we can't just use GTest's CMakeLists.txt
if (NOT GTEST_FOUND)
    message (STATUS "Using bundled GTest")
    include (gtest.cmake)
endif ()

if (MSVC)
    add_subdirectory (getopt)
else ()
    add_library (getopt INTERFACE)
endif ()

if (WIN32)
    add_subdirectory (dxerr)
    add_subdirectory (directxtex)
    add_subdirectory (devcon)
    add_subdirectory (mhook)
endif ()

add_subdirectory (crc32c)
add_subdirectory (md5)

# Force certain components to be optimized as release
macro (target_optimize TARGET)
    target_compile_definitions (${TARGET} PRIVATE -DNDEBUG)
    if (MSVC)
        # TODO: MSVC makes it impossible to override early options.
        if (ENABLE_ASAN)
            target_compile_options (${TARGET} PRIVATE -wd5072) # ASan enabled without debug info
        endif ()
    else ()
        target_compile_options (${TARGET} PRIVATE -O3)
    endif ()
endmacro ()

# Same as include command, but inside a function scope
function (include_with_scope)
    include (${ARGV})
endfunction ()

set (SUBMODULES_MISSING FALSE)
foreach (path IN ITEMS
    brotli/LICENSE
    gtest/LICENSE
    libbacktrace/LICENSE
    libpng/LICENSE
    snappy/COPYING
    zlib/README
    directxmath/LICENSE
)
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
        message (SEND_ERROR "error: ${CMAKE_CURRENT_SOURCE_DIR}/${path} does not exist")
        set (SUBMODULES_MISSING TRUE)
    endif ()
endforeach ()
if (SUBMODULES_MISSING)
    message (FATAL_ERROR "Update Git submodules by running\ngit submodule update --init --depth 1 --recursive")
endif ()

if (ENABLE_STATIC_SNAPPY OR NOT Snappy_FOUND)
    message (STATUS "Using bundled Snappy")
    include_with_scope (snappy.cmake)
endif ()

if (NOT ZLIB_FOUND)
    message (STATUS "Using bundled ZLIB")
    include_with_scope (zlib.cmake)
endif ()

if (NOT PNG_FOUND)
    message (STATUS "Using bundled PNG")
    include_with_scope (libpng.cmake)
endif ()

if (NOT BROTLIDEC_FOUND OR NOT BROTLIENC_FOUND)
    message (STATUS "Using bundled Brotli")
    include_with_scope (brotli.cmake)
endif ()

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    include_with_scope (libbacktrace.cmake)
endif ()

if (BUILD_TESTING)
    # We use non-standard C++ flags, so we can't just use GTest's CMakeLists.txt
    if (NOT GTEST_FOUND)
        message (STATUS "Using bundled GTest")
        include_with_scope (gtest.cmake)
    endif ()
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
    if (HAVE_X86)
        add_subdirectory (mhook)
    endif ()
endif ()

add_subdirectory (crc32c)
add_subdirectory (md5)

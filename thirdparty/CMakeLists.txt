# Force thirdparty components to always be built as release
if (MSVC)
    # TODO: Needs special care due since runtime must match
else ()
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif ()

if (ENABLE_STATIC_SNAPPY OR NOT Snappy_FOUND)
    message (STATUS "Using bundled Snappy")
    add_subdirectory (snappy)
endif ()

if (NOT ZLIB_FOUND)
    message (STATUS "Using bundled ZLIB")
    include (zlib.cmake)
endif ()

if (NOT PNG_FOUND)
    message (STATUS "Using bundled PNG")
    add_subdirectory (libpng)
endif ()

if (NOT BROTLIDEC_FOUND OR NOT BROTLIENC_FOUND)
    message (STATUS "Using bundled Brotli")
    add_subdirectory (brotli)
endif ()

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    add_subdirectory (libbacktrace)
endif ()

# We use non-standard C++ flags, so we can't just use GTest's CMakeLists.txt
if (NOT GTEST_FOUND)
    message (STATUS "Using bundled GTest")
    add_library (gtest gtest/src/gtest-all.cc)
    target_compile_definitions (gtest PUBLIC GTEST_HAS_TR1_TUPLE=0)
    target_include_directories (gtest
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gtest/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gtest
    )
    target_link_libraries (gtest PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    add_library (GTest::GTest ALIAS gtest)
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

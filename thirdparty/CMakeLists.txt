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

# We use non-standard C++ flags, so we can't just use GTest's CMakeLists.txt
if (NOT GTEST_FOUND)
    add_library (gtest gtest/src/gtest-all.cc)
    target_compile_definitions (gtest PUBLIC GTEST_HAS_TR1_TUPLE=0)
    target_include_directories (gtest
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gtest/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gtest
    )
    target_link_libraries (gtest PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    add_library (GTest::GTest ALIAS gtest)
endif ()

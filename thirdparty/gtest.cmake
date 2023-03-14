add_library (gtest
    gtest/googletest/src/gtest-all.cc
)

target_compile_definitions (gtest PUBLIC GTEST_HAS_TR1_TUPLE=0)

target_include_directories (gtest
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest
)

target_link_libraries (gtest PUBLIC ${CMAKE_THREAD_LIBS_INIT})

add_library (GTest::GTest ALIAS gtest)

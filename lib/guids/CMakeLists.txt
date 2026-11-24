add_library (guids STATIC
    guids.cpp
    guids_defs.hpp
    guids_entries.h
    guids.hpp
)

target_include_directories (guids
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries (guids PRIVATE os)

if (BUILD_TESTING)
    add_gtest (guids_test guids_test.cpp)
    target_link_libraries (guids_test PRIVATE guids)
endif ()

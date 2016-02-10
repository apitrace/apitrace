add_library (guids STATIC
    guids.cpp
)

add_gtest (guids_test guids_test.cpp)
target_link_libraries (guids_test guids)

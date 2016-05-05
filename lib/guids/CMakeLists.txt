add_library (guids STATIC
    guids.cpp
    guids_defs.hpp
    guids_entries.h
    guids.hpp
)

add_gtest (guids_test guids_test.cpp)
target_link_libraries (guids_test guids)

add_library (guids STATIC
    guids.cpp
)

add_executable (guids_test guids_test.cpp)
target_link_libraries (guids_test guids gtest)
add_dependencies (check guids_test)
add_test (NAME guids_test COMMAND $<TARGET_FILE:guids_test>)

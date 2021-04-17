include_directories (
    ${CMAKE_SOURCE_DIR}/thirdparty
)

if (WIN32)
    set (os os_win32.cpp)
elseif (APPLE)
    set (os os_posix.cpp os_osx.mm)
else ()
    set (os os_posix.cpp)
endif ()

add_convenience_library (os
    ${os}
    os_backtrace.cpp
    os_crtdbg.cpp
)

target_link_libraries (os PUBLIC Threads::Threads)

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    target_compile_definitions (os PRIVATE HAVE_BACKTRACE=1)
    target_link_libraries (os PUBLIC backtrace)
endif ()

if (WIN32)
    target_link_libraries (os PUBLIC
        shell32
    )
endif ()
if (APPLE)
    target_link_libraries (os PUBLIC
        "-framework Foundation"
    )
endif ()

if (BUILD_TESTING)
    add_gtest (os_thread_test os_thread_test.cpp)
    target_link_libraries (os_thread_test os)
endif ()

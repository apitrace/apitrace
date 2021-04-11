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

target_link_libraries (os
    ${LIBBACKTRACE_LIBRARIES}
)
if (WIN32)
    target_link_libraries (os
        shell32
    )
endif ()
if (APPLE)
    target_link_libraries (os
        "-framework Foundation"
    )
endif ()

if (BUILD_TESTING)
    add_gtest (os_thread_test os_thread_test.cpp)
    target_link_libraries (os_thread_test os)
endif ()

add_convenience_library (os
    os_backtrace.cpp
    os_crtdbg.cpp
)

target_include_directories (os
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty
    # XXX: os_backtrace.hpp includes trace_model.hpp, whereas common depends on
    # os.  Linking `common` here would be circular, so only borrow its headers.
    PRIVATE ${PROJECT_SOURCE_DIR}/lib/trace
)

if (WIN32)
    target_sources (os PRIVATE os_win32.cpp)
else ()
    target_sources (os PRIVATE os_posix.cpp)
    if (APPLE)
        target_sources (os PRIVATE os_osx.mm)
    endif ()
endif ()

target_link_libraries (os PUBLIC compat Threads::Threads)

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
    target_link_libraries (os_thread_test PRIVATE os)
endif ()

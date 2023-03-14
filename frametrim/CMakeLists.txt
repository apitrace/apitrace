include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cli
    ${CMAKE_SOURCE_DIR}/lib/highlight
    ${CMAKE_SOURCE_DIR}/helpers
    ${CMAKE_SOURCE_DIR}/retrace
    ${CMAKE_BINARY_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/lib/image
    ${CMAKE_SOURCE_DIR}/lib/ubjson
    ${CMAKE_SOURCE_DIR}/thirdparty/dxerr
    ${CMAKE_SOURCE_DIR}/thirdparty/mhook/mhook-lib
)

add_executable(gltrim
   ft_dependecyobject.cpp
   ft_frametrimmer.cpp
   ft_main.cpp
   ft_matrixstate.cpp
   ft_tracecall.cpp)

target_link_libraries (gltrim
    retrace_common
    glretrace_common
    )

install (TARGETS gltrim RUNTIME DESTINATION bin)

option (ENABLE_GLTRIM_TESTS "Enable running the gltrim tests." OFF)

if (${ENABLE_GLTRIM_TESTS})
   SET(gltrim_test_available "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
   if (EXISTS "${gltrim_test_available}")
      add_subdirectory (tests)
   else()
      message(FATAL_ERROR
         " gltrim tests requested but test submodule was not cloned, run\n"
         "    git submodule update --init --recursive\n"
         " to obtain test data and then re-run cmake.\n")
   endif()
endif()

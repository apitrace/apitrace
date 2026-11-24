add_executable(gltrim
   ft_dependecyobject.cpp
   ft_frametrimmer.cpp
   ft_main.cpp
   ft_matrixstate.cpp
   ft_opengl.cpp
   ft_tracecall.cpp)

target_include_directories (gltrim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/cli
)

target_link_libraries (gltrim PRIVATE
    retrace_common
    glretrace_common
)

install (TARGETS gltrim)

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

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

if (DirectX_D3D9_INCLUDE_FOUND)
   include_directories (BEFORE SYSTEM ${DirectX_D3D9_INCLUDE_DIR})
   add_definitions (-DHAVE_D3D9)
   set (D3D9_SOURCES ft_d3d9.cpp)
endif ()

add_executable(gltrim
   ft_dependecyobject.cpp
   ft_frametrimmer.cpp
   ft_main.cpp
   ft_matrixstate.cpp
   ft_opengl.cpp
   ft_tracecall.cpp
   ${D3D9_SOURCES})

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

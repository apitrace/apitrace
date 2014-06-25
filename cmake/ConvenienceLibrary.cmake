# Function for convenience libraries, ie., libraries which can be statically
# linked into shared libraries.
function (add_convenience_library)

    set (name "${ARGV0}")
    list (REMOVE_AT ARGV 0)

    add_library ("${name}" STATIC ${ARGV})

    if (NOT "${CMAKE_SHARED_LIBRARY_C_FLAGS}" STREQUAL "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
        message (FATAL_ERROR "CMAKE_SHARED_LIBRARY_C_FLAGS (${CMAKE_SHARED_LIBRARY_C_FLAGS}) != CMAKE_SHARED_LIBRARY_CXX_FLAGS (${CMAKE_SHARED_LIBRARY_CXX_FLAGS})")
    endif ()

    set_target_properties ("${name}" PROPERTIES
        # Ensure it can be statically linked in shared libraries
        COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}"
    )

endfunction ()

# Function for convenience libraries, ie., libraries which can be statically
# linked into shared libraries.
function (add_convenience_library name)

    add_library ("${name}" STATIC ${ARGN})

    set_target_properties ("${name}" PROPERTIES
        # Ensure it can be statically linked onto shared libraries
        POSITION_INDEPENDENT_CODE ON
    )

endfunction ()

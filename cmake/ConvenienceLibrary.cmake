# Function for convenience libraries, ie., libraries which can be statically
# linked into shared libraries.
function (add_convenience_library)

    set (name "${ARGV0}")
    list (REMOVE_AT ARGV 0)

    add_library ("${name}" STATIC ${ARGV})

    set_target_properties ("${name}" PROPERTIES
        # Ensure it can be statically linked onto shared libraries
        POSITION_INDEPENDENT_CODE ON
    )

endfunction ()

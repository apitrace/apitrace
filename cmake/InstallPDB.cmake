function (INSTALL_PDB target_name)
    if (MSVC)
        get_target_property (target_output_name ${target_name} OUTPUT_NAME)
        if (NOT target_output_name)
            set (target_output_name ${target_name})
        endif ()

        # The documentation for the INSTALL command doesn't mention generator
        # expressions, but they actually work quite well, both when building
        # MSBuild and Ninja.
        install (
            FILES "$<TARGET_FILE_DIR:${target_name}>/${target_output_name}.pdb"
            CONFIGURATIONS Debug RelWithDebInfo
            ${ARGV}
        )
    endif()
endfunction ()

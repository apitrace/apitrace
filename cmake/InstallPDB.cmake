function (INSTALL_PDB target_name)
    if (MSVC)
        install (
            FILES "$<TARGET_PDB_FILE:${target_name}>"
            CONFIGURATIONS Debug RelWithDebInfo
            ${ARGN}
        )
    endif()
endfunction ()

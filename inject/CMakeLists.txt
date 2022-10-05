set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories (
    ${CMAKE_SOURCE_DIR}/thirdparty/devcon
    ${CMAKE_SOURCE_DIR}/thirdparty/mhook/mhook-lib
)

add_library (injectee_iat MODULE
    injectee_iat.cpp
)
target_link_libraries (injectee_iat
    advapi32
)
set_target_properties (injectee_iat PROPERTIES
    PREFIX ""
)
if (MSVC)
    set_target_properties (injectee_iat PROPERTIES LINK_FLAGS "/ENTRY:DllMainStartup")
else ()
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set_target_properties (injectee_iat PROPERTIES LINK_FLAGS "-Wl,--entry=_DllMainStartup@12")
    else ()
        set_target_properties (injectee_iat PROPERTIES LINK_FLAGS "-Wl,--entry=DllMainStartup")
    endif ()
endif ()
install (TARGETS injectee_iat LIBRARY DESTINATION bin)
install_pdb (injectee_iat DESTINATION bin)

if (HAVE_X86)
    add_library (injectee_mhook MODULE
        injectee_mhook.cpp
    )
    set_target_properties (injectee_mhook PROPERTIES
        PREFIX ""
    )
    if (MSVC)
        set_target_properties (injectee_mhook PROPERTIES LINK_FLAGS "/ENTRY:DllMainStartup")
    else ()
        if (CMAKE_SIZEOF_VOID_P EQUAL 4)
            set_target_properties (injectee_mhook PROPERTIES LINK_FLAGS "-Wl,--entry=_DllMainStartup@12")
        else ()
            set_target_properties (injectee_mhook PROPERTIES LINK_FLAGS "-Wl,--entry=DllMainStartup")
        endif ()
    endif ()
    target_link_libraries (injectee_mhook
        advapi32
        mhook
    )
    install (TARGETS injectee_mhook LIBRARY DESTINATION bin)
    install_pdb (injectee_mhook DESTINATION bin)
endif ()

add_executable (injector
    injector.cpp
)
set_target_properties (injector PROPERTIES
    OUTPUT_NAME inject
)
target_link_libraries (injector
    getopt
    devcon
    psapi
    advapi32
)
install (TARGETS injector RUNTIME DESTINATION bin)
install_pdb (injector DESTINATION bin)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories (${CMAKE_SOURCE_DIR}/thirdparty/devcon)

add_library (injectee MODULE
    injectee.cpp
)
set_target_properties (injectee PROPERTIES
    PREFIX ""
)
if (MSVC)
    set_target_properties (injectee PROPERTIES LINK_FLAGS "/ENTRY:DllMainStartup")
else ()
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set_target_properties (injectee PROPERTIES LINK_FLAGS "-Wl,--entry=_DllMainStartup@12")
    else ()
        set_target_properties (injectee PROPERTIES LINK_FLAGS "-Wl,--entry=DllMainStartup")
    endif ()
endif ()

install (TARGETS injectee LIBRARY DESTINATION bin)
install_pdb (injectee DESTINATION bin)

add_executable (injector
    injector.cpp
)
set_target_properties (injector PROPERTIES
    OUTPUT_NAME inject
)
target_link_libraries (injector
    ${GETOPT_LIBRARIES}
    devcon
    psapi
)
install (TARGETS injector RUNTIME DESTINATION bin)
install_pdb (injector DESTINATION bin)

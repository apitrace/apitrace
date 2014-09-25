set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories (${CMAKE_SOURCE_DIR}/thirdparty/devcon)

add_library (injectee MODULE
    injectee.cpp
)
set_target_properties (injectee PROPERTIES
    PREFIX ""
)
install (TARGETS injectee LIBRARY DESTINATION bin)
install_pdb (injectee DESTINATION bin)

add_executable (injector
    injector.cpp
)
set_target_properties (injector PROPERTIES
    OUTPUT_NAME inject
)
target_link_libraries (injector
    devcon
    psapi
)
install (TARGETS injector RUNTIME DESTINATION bin)
install_pdb (injector DESTINATION bin)

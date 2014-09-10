set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library (injectee MODULE
    injectee.cpp
)
set_target_properties (injectee PROPERTIES
    PREFIX ""
)
install (TARGETS injectee LIBRARY DESTINATION bin)

add_executable (injector
    injector.cpp
)
set_target_properties (injector PROPERTIES
    OUTPUT_NAME inject
)
target_link_libraries (injector psapi)
install (TARGETS injector RUNTIME DESTINATION bin)

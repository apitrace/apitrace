set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library (injectee MODULE
    injectee.cpp
)
set_target_properties (injectee PROPERTIES
    PREFIX ""
    OUTPUT_NAME inject
)
install (TARGETS injectee LIBRARY DESTINATION bin)

add_executable (injector
    injector.cpp
)
install (TARGETS injector RUNTIME DESTINATION bin)

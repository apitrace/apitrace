add_convenience_library (highlight
    highlight.hpp
    highlight.cpp
)

target_include_directories (highlight
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

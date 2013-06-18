include_directories (
    ${PNG_INCLUDE_DIR}
)

add_library (image STATIC
    image_bmp.cpp
    image_png.cpp
    image_pnm.cpp
    image_raw.cpp
)

target_link_libraries (image
    ${PNG_LIBRARIES}
)

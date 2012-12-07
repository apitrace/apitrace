include_directories (
    ${PNG_INCLUDE_DIR}
)

add_library (image STATIC
    image.cpp
    image_bmp.cpp
    image_png.cpp
    image_pnm.cpp
)

target_link_libraries (image
    ${PNG_LIBRARIES}
)

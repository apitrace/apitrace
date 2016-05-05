include_directories (
    ${PNG_INCLUDE_DIR}
    ${MD5_INCLUDE_DIR}
)

add_library (image STATIC
    image.hpp
    image_bmp.cpp
    image_png.cpp
    image_pnm.cpp
    image_raw.cpp
    image_md5.cpp
)

target_link_libraries (image
    ${PNG_LIBRARIES}
    ${MD5_LIBRARIES}
)

add_library (image STATIC
    image.hpp
    image_bmp.cpp
    image_png.cpp
    image_pnm.cpp
    image_raw.cpp
    image_md5.cpp
)

target_include_directories (image
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (image PUBLIC
    md5
    PNG::PNG
)

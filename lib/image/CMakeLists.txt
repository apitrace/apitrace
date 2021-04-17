add_library (image STATIC
    image.hpp
    image_bmp.cpp
    image_png.cpp
    image_pnm.cpp
    image_raw.cpp
    image_md5.cpp
)

target_link_libraries (image
    md5
    PNG::PNG
)

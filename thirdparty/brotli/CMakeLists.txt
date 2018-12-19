force_release_build ()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DOS_LINUX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  add_definitions(-DOS_FREEBSD)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DOS_MACOSX)
endif()

add_convenience_library(brotli_common STATIC EXCLUDE_FROM_ALL
    c/common/dictionary.c
    c/common/transform.c
)
add_convenience_library(brotli_dec STATIC EXCLUDE_FROM_ALL
    c/dec/bit_reader.c
    c/dec/decode.c
    c/dec/huffman.c
    c/dec/state.c
)
add_convenience_library(brotli_enc STATIC EXCLUDE_FROM_ALL
    c/enc/backward_references.c
    c/enc/backward_references_hq.c
    c/enc/bit_cost.c
    c/enc/block_splitter.c
    c/enc/brotli_bit_stream.c
    c/enc/cluster.c
    c/enc/compress_fragment.c
    c/enc/compress_fragment_two_pass.c
    c/enc/dictionary_hash.c
    c/enc/encode.c
    c/enc/encoder_dict.c
    c/enc/entropy_encode.c
    c/enc/histogram.c
    c/enc/literal_cost.c
    c/enc/memory.c
    c/enc/metablock.c
    c/enc/static_dict.c
    c/enc/utf8_util.c
)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-brotli.txt
)

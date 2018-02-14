force_release_build ()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DOS_LINUX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  add_definitions(-DOS_FREEBSD)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DOS_MACOSX)
endif()

add_convenience_library(brotli_common STATIC EXCLUDE_FROM_ALL
  common/dictionary.c)
add_convenience_library(brotli_dec STATIC EXCLUDE_FROM_ALL
  dec/bit_reader.c
  dec/decode.c
  dec/huffman.c
  dec/state.c)
add_convenience_library(brotli_enc STATIC EXCLUDE_FROM_ALL
  enc/backward_references.c
  enc/bit_cost.c
  enc/block_splitter.c
  enc/brotli_bit_stream.c
  enc/cluster.c
  enc/compress_fragment.c
  enc/compress_fragment_two_pass.c
  enc/encode.c
  enc/entropy_encode.c
  enc/histogram.c
  enc/literal_cost.c
  enc/memory.c
  enc/metablock.c
  enc/static_dict.c
  enc/utf8_util.c)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-brotli.txt
)

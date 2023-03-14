if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (BROTLI_OS "OS_LINUX")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    set (BROTLI_OS "OS_FREEBSD")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (BROTLI_OS "DOS_MACOSX")
endif ()

add_convenience_library(brotli_common STATIC EXCLUDE_FROM_ALL
    brotli/c/common/constants.c
    brotli/c/common/context.c
    brotli/c/common/dictionary.c
    brotli/c/common/platform.c
    brotli/c/common/shared_dictionary.c
    brotli/c/common/transform.c
)

target_include_directories (brotli_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/brotli/c/include
)

add_convenience_library (brotli_dec STATIC EXCLUDE_FROM_ALL
    brotli/c/dec/bit_reader.c
    brotli/c/dec/decode.c
    brotli/c/dec/huffman.c
    brotli/c/dec/state.c
)

add_convenience_library (brotli_enc STATIC EXCLUDE_FROM_ALL
    brotli/c/enc/backward_references.c
    brotli/c/enc/backward_references_hq.c
    brotli/c/enc/bit_cost.c
    brotli/c/enc/block_splitter.c
    brotli/c/enc/brotli_bit_stream.c
    brotli/c/enc/cluster.c
    brotli/c/enc/command.c
    brotli/c/enc/compound_dictionary.c
    brotli/c/enc/compress_fragment.c
    brotli/c/enc/compress_fragment_two_pass.c
    brotli/c/enc/dictionary_hash.c
    brotli/c/enc/encode.c
    brotli/c/enc/encoder_dict.c
    brotli/c/enc/entropy_encode.c
    brotli/c/enc/fast_log.c
    brotli/c/enc/histogram.c
    brotli/c/enc/literal_cost.c
    brotli/c/enc/memory.c
    brotli/c/enc/metablock.c
    brotli/c/enc/static_dict.c
    brotli/c/enc/utf8_util.c
)

target_compile_definitions (brotli_common PRIVATE ${BROTLI_OS})
target_compile_definitions (brotli_dec    PRIVATE ${BROTLI_OS})
target_compile_definitions (brotli_enc    PRIVATE ${BROTLI_OS})

target_link_libraries (brotli_dec PUBLIC brotli_common)
target_link_libraries (brotli_enc PUBLIC brotli_common)

target_optimize (brotli_common)
target_optimize (brotli_dec)
target_optimize (brotli_enc)

install (
    FILES brotli/LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-brotli.txt
)

if (NOT BROTLIDEC_FOUND)
    add_library (PkgConfig::BROTLIDEC ALIAS brotli_dec)
endif ()
if (NOT BROTLIENC_FOUND)
    add_library (PkgConfig::BROTLIENC ALIAS brotli_enc)
endif ()

force_release_build ()

add_convenience_library (brotli_dec_bundled EXCLUDE_FROM_ALL

    dec/bit_reader.c
    dec/bit_reader.h
    dec/context.h
    dec/decode.c
    dec/decode.h
    dec/dictionary.c
    dec/dictionary.h
    dec/huffman.c
    dec/huffman.h
    dec/port.h
    dec/prefix.h
    dec/state.c
    dec/state.h
    dec/transform.h
    dec/types.h
)

add_convenience_library (brotli_enc_bundled EXCLUDE_FROM_ALL
    enc/backward_references.cc
    enc/backward_references.h
    enc/bit_cost.h
    enc/block_splitter.cc
    enc/block_splitter.h
    enc/brotli_bit_stream.cc
    enc/brotli_bit_stream.h
    enc/cluster.h
    enc/command.h
    enc/compress_fragment.cc
    enc/compress_fragment.h
    enc/compress_fragment_two_pass.cc
    enc/compress_fragment_two_pass.h
    enc/context.h
    enc/dictionary.cc
    enc/dictionary.h
    enc/dictionary_hash.h
    enc/encode.cc
    enc/encode.h
    enc/encode_parallel.cc
    enc/encode_parallel.h
    enc/entropy_encode.cc
    enc/entropy_encode.h
    enc/entropy_encode_static.h
    enc/fast_log.h
    enc/find_match_length.h
    enc/hash.h
    enc/histogram.cc
    enc/histogram.h
    enc/literal_cost.cc
    enc/literal_cost.h
    enc/metablock.cc
    enc/metablock.h
    enc/port.h
    enc/prefix.h
    enc/ringbuffer.h
    enc/static_dict.cc
    enc/static_dict.h
    enc/static_dict_lut.h
    enc/streams.cc
    enc/streams.h
    enc/transform.h
    enc/types.h
    enc/utf8_util.cc
    enc/utf8_util.h
    enc/write_bits.h
)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-brotli.txt
)

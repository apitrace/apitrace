# Zstandard compression library
# Includes main zstd library plus seekable format

# Common library (shared between compression and decompression)
add_convenience_library(zstd_bundled STATIC EXCLUDE_FROM_ALL
    zstd/lib/common/debug.c
    zstd/lib/common/entropy_common.c
    zstd/lib/common/error_private.c
    zstd/lib/common/fse_decompress.c
    zstd/lib/common/pool.c
    zstd/lib/common/threading.c
    zstd/lib/common/zstd_common.c
    zstd/lib/decompress/huf_decompress.c
    zstd/lib/decompress/zstd_ddict.c
    zstd/lib/decompress/zstd_decompress.c
    zstd/lib/decompress/zstd_decompress_block.c
    zstd/lib/compress/fse_compress.c
    zstd/lib/compress/hist.c
    zstd/lib/compress/huf_compress.c
    zstd/lib/compress/zstd_compress.c
    zstd/lib/compress/zstd_compress_literals.c
    zstd/lib/compress/zstd_compress_sequences.c
    zstd/lib/compress/zstd_compress_superblock.c
    zstd/lib/compress/zstd_double_fast.c
    zstd/lib/compress/zstd_fast.c
    zstd/lib/compress/zstd_lazy.c
    zstd/lib/compress/zstd_ldm.c
    zstd/lib/compress/zstdmt_compress.c
    zstd/lib/compress/zstd_opt.c
    zstd/lib/compress/zstd_preSplit.c
)

target_include_directories(zstd_bundled PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/zstd/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/zstd/lib/common
)

target_compile_definitions(zstd_bundled PRIVATE
    NDEBUG

    # Would be better to copy their logic for enabling this asm support.
    DYNAMIC_BMI2=0
)

target_optimize(zstd_bundled)

# Platform-specific compiler flags
if(NOT MSVC)
    # Disable specific warnings that are common in zstd
    target_compile_options(zstd_bundled PRIVATE -Wno-unused-function)
endif()

# Install license
install(
    FILES zstd/LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-zstd.txt
)

# Create alias for easy linking
add_library(PkgConfig::ZSTD ALIAS zstd_bundled)

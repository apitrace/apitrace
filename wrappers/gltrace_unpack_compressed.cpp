/*
 * Copyright © 2020 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include "gltrace_unpack_compressed.hpp"
#include "gltrace.hpp"


template<typename X, typename Y>
auto _div_round_up(X a, Y b) -> decltype(a / b) {
    return (a + b - 1) / b;
}

struct UnpackParams
{
    GLint block_size = 0;
    GLint block_width = 1;
    GLint block_height = 1;
    GLint block_depth = 1;

    GLint skip_pixels = 0;
    GLint row_length = 0;
    GLint skip_rows = 0;
    GLint image_height = 0;
    GLint skip_images = 0;
};

UnpackParams
getUnpackParams(bool compressed) {
    UnpackParams unpack_params;

    _glGetIntegerv(GL_UNPACK_SKIP_PIXELS,  &unpack_params.skip_pixels);
    _glGetIntegerv(GL_UNPACK_ROW_LENGTH,   &unpack_params.row_length);
    _glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT, &unpack_params.image_height);
    _glGetIntegerv(GL_UNPACK_SKIP_ROWS,    &unpack_params.skip_rows);
    _glGetIntegerv(GL_UNPACK_SKIP_IMAGES,  &unpack_params.skip_images);

    if (compressed) {
        _glGetIntegerv(GL_UNPACK_COMPRESSED_BLOCK_SIZE,   &unpack_params.block_size);
        _glGetIntegerv(GL_UNPACK_COMPRESSED_BLOCK_WIDTH,  &unpack_params.block_width);
        _glGetIntegerv(GL_UNPACK_COMPRESSED_BLOCK_HEIGHT, &unpack_params.block_height);
        _glGetIntegerv(GL_UNPACK_COMPRESSED_BLOCK_DEPTH,  &unpack_params.block_depth);
    }

    return unpack_params;
}

bool
canTakeFastPath(const UnpackParams& unpack_params, GLsizei width, GLsizei height, GLsizei depth) {
  /* OpenGL 4.6, 8.7 Compressed Texture Images:
   *
   *  By default the pixel storage modes UNPACK_ROW_LENGTH, UNPACK_SKIP_ROWS,
   *  UNPACK_SKIP_PIXELS, UNPACK_IMAGE_HEIGHT and UNPACK_SKIP_IMAGES are ignored
   *  for compressed images. To enable UNPACK_SKIP_PIXELS and UNPACK_ROW_LENGTH,
   *  block size and b_w must both be non-zero.
   */
  if (unpack_params.block_size == 0 || unpack_params.block_width == 0)
    return true;

  if (unpack_params.skip_pixels)
    return false;

  if (width > 0 && unpack_params.row_length > 0 && width < unpack_params.row_length)
    return false;

   /* OpenGL 4.6, 8.7 Compressed Texture Images:
    *
    *  To also enable UNPACK_SKIP_ROWS and UNPACK_IMAGE_HEIGHT, b_h must be non-zero.
    *
    * If zero - other paramters won't matter.
    */
  if (height == 0 || unpack_params.block_height == 0)
    return true;

  if (unpack_params.skip_rows)
    return false;

  if (unpack_params.image_height > 0 && height < unpack_params.image_height)
    return false;

   /* OpenGL 4.6, 8.7 Compressed Texture Images:
    *
    *  And to also enable UNPACK_SKIP_IMAGES, b_d must be non-zero.
    */
  if (depth == 0 || unpack_params.block_depth == 0)
    return true;

  if(unpack_params.skip_images)
    return false;

  return true;
}

void
writeCompressedTex(const void * data, GLenum format, GLsizei width, GLsizei height, GLsizei depth,
                   GLsizei imageSize, GLboolean has_unpack_subimage, std::function<void(const void*, GLsizei)> callback)
{
   /* imageSize may not be the amount of bytes we must copy from user's ptr
    * if the pixel storage modes were set by user. Supplied imageSize​ has to be
    *   GL_UNPACK_COMPRESSED_BLOCK_SIZE * (width / GL_UNPACK_COMPRESSED_BLOCK_WIDTH) *
    *   (height / GL_UNPACK_COMPRESSED_BLOCK_HEIGHT) * (depth / GL_UNPACK_COMPRESSED_BLOCK_DEPTH)
    * which doesn't account storage modes. However storage mods do affect how
    * much bytes we should copy.
    */

    if (!has_unpack_subimage) {
        callback(data, imageSize);
        return;
    }

    UnpackParams unpack_params = getUnpackParams(true);

    if (canTakeFastPath(unpack_params, width, height, depth)) {
        callback(data, imageSize);
        return;
    }

    size_t blocks_in_row = _div_round_up(width, unpack_params.block_width);
    size_t copy_bytes_per_row= blocks_in_row * unpack_params.block_size;
    size_t total_bytes_per_row = copy_bytes_per_row;

    size_t copy_rows_per_slice = height > 0 ? _div_round_up(height, unpack_params.block_height) : 1;
    size_t total_rows_per_slice = copy_rows_per_slice;

    size_t copy_slices = depth > 0 ? _div_round_up(depth, unpack_params.block_depth) : 1;

    /* OpenGL 4.6, 8.7 Compressed Texture Images:
     *
     *  Before obtaining the first compressed image block from memory,
     *  the data pointer is advanced by:
     *    (UNPACK_SKIP_PIXELS / b_w) * n + (UNPACK_SKIP_ROWS / b_h) * k
     *  elements.
     *
     * Where n = 1 and k is blocks in a row.
     * This is "(UNPACK_SKIP_PIXELS / b_w) * n" part
     */
    size_t skip_bytes = unpack_params.skip_pixels / unpack_params.block_width * 1 * unpack_params.block_size;

    /* OpenGL 4.6, 8.4.4.1 Unpacking:
     *
     *  If the value of UNPACK_ROW_LENGTH is zero, then the number of groups in a row
     *  is width; otherwise the number of groups is the value of UNPACK_ROW_LENGTH.
     */

    if (unpack_params.row_length) {
        total_bytes_per_row = unpack_params.block_size * _div_round_up(unpack_params.row_length, unpack_params.block_width);
    }

    /* OpenGL 4.6, 8.7 Compressed Texture Images:
     *
     *  To also enable UNPACK_SKIP_ROWS and UNPACK_IMAGE_HEIGHT, b_h must be non-zero.
     */
    if (height > 0 && unpack_params.block_height > 0) {
        /* OpenGL 4.6, 8.7 Compressed Texture Images:
         *
         *  + (UNPACK_SKIP_ROWS / b_h) * k
         */
        skip_bytes += unpack_params.skip_rows / unpack_params.block_height * total_bytes_per_row;

        if (unpack_params.image_height) {
            total_rows_per_slice = _div_round_up(unpack_params.image_height, unpack_params.block_height);
        }
    }

    /* OpenGL 4.6, 8.5 Texture Image Specification:
     *
     *  And to also enable UNPACK_SKIP_IMAGES, b_d must be non-zero.
     */
    if (depth > 0 && unpack_params.block_depth > 0) {
        /* OpenGL 4.6, 8.7 Compressed Texture Images:
         *
         *  For three-dimensional compressed images the pointer is advanced by
         *  UNPACK_SKIP_IMAGES / b_d times the number of elements in one two-dimensional
         *  image before obtaining the first group from memory.
         */
        skip_bytes += unpack_params.skip_images * total_bytes_per_row * total_rows_per_slice / unpack_params.block_depth;
    }

    size_t alloc_size = skip_bytes + depth * total_bytes_per_row * total_rows_per_slice / std::max(unpack_params.block_depth, 1);
    alloc_size += total_bytes_per_row * copy_rows_per_slice;
    std::vector<uint8_t> copied_data(alloc_size, 0);

    uint8_t* dst_pointer = copied_data.data();
    const uint8_t* src_pointer = reinterpret_cast<const uint8_t*>(data);

    src_pointer += skip_bytes;
    dst_pointer += skip_bytes;

    for (size_t slice = 0; slice < copy_slices; slice++) {
         assert(dst_pointer < (copied_data.data() + copied_data.size()));

         if (total_bytes_per_row == copy_bytes_per_row) {
            assert(dst_pointer < (copied_data.data() + copied_data.size()));

            // No skips between rows so we can get away with one copy
            size_t to_copy = copy_bytes_per_row * copy_rows_per_slice;
            memcpy(reinterpret_cast<void*>(dst_pointer), src_pointer, to_copy);
            src_pointer += to_copy;
            dst_pointer += to_copy;
         } else {
            /* OpenGL 4.6, 8.7 Compressed Texture Images:
             *
             * Then width / b_w blocks are obtained from contiguous blocks in memory (without advancing the pointer),
             * after which the pointer is advanced by k elements.
             * height / b_h sets of width / b_w blocks are obtained this way.
             */
            for (size_t row = 0; row < copy_rows_per_slice; row++) {
               assert(dst_pointer < (copied_data.data() + copied_data.size()));

               memcpy(reinterpret_cast<void*>(dst_pointer), src_pointer, copy_bytes_per_row);
               src_pointer += total_bytes_per_row;
               dst_pointer += total_bytes_per_row;
            }
         }

         /* OpenGL 4.6, 8.7 Compressed Texture Images:
          *
          *  Then after height rows are obtained, if UNPACK_IMAGE_HEIGHT is positive,
          *  the pointer skips over the (UNPACK_IMAGE_HEIGHT - height) / b_h rows,
          *  before obtaining the next set of blocks.
          *
          * Pointer will be right at the start of the sub-image in the next slice.
          */
         size_t advance = total_bytes_per_row * (total_rows_per_slice - copy_rows_per_slice);
         src_pointer += advance;
         dst_pointer += advance;
    }

    callback(copied_data.data(), copied_data.size());
}